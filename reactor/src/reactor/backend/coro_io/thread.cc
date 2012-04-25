#include <reactor/backend/thread.hh>
#include <reactor/backend/coro_io/libcoroutine/coroutine.hh>
#include <reactor/backend/coro_io/thread.hh>
#include <reactor/debug.hh>


namespace reactor
{
  namespace backend
  {
    namespace coro_io
    {
      /*----------------------.
      | Manager: Construction |
      `----------------------*/

      Manager::Manager()
        : _self(*this)
        , _current(&_self)
      {}

      Manager::~Manager()
      {}

      /*---------------.
      | Manager: State |
      `---------------*/

      Thread*
      Manager::current() const
      {
        return _current;
      }

      /*----------------.
      | Thread: Helpers |
      `----------------*/

      void starter(void* arg)
      {
        Thread* thread = reinterpret_cast<Thread*>(arg);
        thread->_run();
      }

      /*---------------------.
      | Thread: Construction |
      `---------------------*/

      Thread::Thread(Manager& manager,
                     const std::string& name,
                     const Action& action)
        : _name(name)
        , _status(status::starting)
        , _manager(manager)
        , _action(action)
        , _coro(0)
        , _caller(0)

      {}

      Thread::Thread(Manager& manager)
        : _name("<root>")
        , _status(status::running)
        , _manager(manager)
        , _action()
        , _coro(0)
        , _caller(0)

      {
        _coro = Coro_new();
        assert(_coro);
        Coro_initializeMainCoro(_coro);
      }

      Thread::~Thread()
      {
        INFINIT_REACTOR_DEBUG(_name << ": die");
        if (_coro)
          {
            Coro_free(_coro);
            _coro = 0;
          }
      }

      /*---------------.
      | Thread: Naming |
      `---------------*/

      std::string
      Thread::name() const
      {
        return _name;
      }

      Status
      Thread::status() const
      {
        return _status;
      }

      bool
      Thread::done() const
      {
        return _status == status::done;
      }

      /*------------------.
      | Thread: Switching |
      `------------------*/

      void
      Thread::step()
      {
        assert(_caller == 0);
        if (!_coro)
        {
          assert(_status == status::starting);
          _status = status::running;
          Thread* current = _manager._current;
          _caller = current;
          _manager._current = this;
          _coro = Coro_new();
          assert(_coro);
          INFINIT_REACTOR_DEBUG(current->_name << ": start " << _name);
          Coro_startCoro_(_caller->_coro, _coro, this, &starter);
          INFINIT_REACTOR_DEBUG(current->_name << ": back from " << _name);
        }
        else
        {
          assert(_status == status::waiting);
          _status = status::running;
          Thread* current = _manager._current;
          _caller = current;
          _manager._current = this;
          INFINIT_REACTOR_DEBUG(_name << ": step from " << _caller->_name);
          Coro_switchTo_(current->_coro, _coro);
        }
      }

      /*------------------.
      | Thread: Details.  |
      `------------------*/

      void
      Thread::_run()
      {
        _status = status::running;
        try
        {
          _action();
        }
        catch (const std::exception& e)
        {
          std::cerr << "Thread " << name() << " killed by exception: "
                    << e.what() << "." << std::endl;
          std::abort();
        }
        catch (...)
        {
          std::cerr << "Thread " << name() << " killed by unknown exception."
                    << std::endl;
          std::abort();
        }
        Thread* caller = _caller;
        _caller = 0;
        _status = status::done;
        _manager._current = caller;
        INFINIT_REACTOR_DEBUG(_name << ": done");
        Coro_switchTo_(_coro, caller->_coro);
      }

      void
      Thread::yield()
      {
        assert(_manager._current == this);
        assert(_status == status::running);
        _status = status::waiting;
        _manager._current = _caller;
        INFINIT_REACTOR_DEBUG(_name << ": yield back to " << _manager._current->_name);
        _caller = 0;
        Coro_switchTo_(_coro, _manager._current->_coro);
      }
    }
  }
}
