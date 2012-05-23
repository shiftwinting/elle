#ifndef ELLE_CONCURRENCY_CONCURRENCY_HH
# define ELLE_CONCURRENCY_CONCURRENCY_HH

# include <elle/radix/Status.hh>

namespace elle
{
  using namespace radix;

  ///
  /// this namespace contains functionalities for concurrency purposes.
  ///
  namespace concurrency
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides control over the concurrency module.
    ///
    class Concurrency
    {
    public:
      //
      // static methods
      //
      static Status     Initialize();
      static Status     Clean();
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

# include <elle/concurrency/Callback.hh>
# include <elle/concurrency/Closure.hh>
# include <elle/concurrency/Environment.hh>
# include <elle/concurrency/Event.hh>
# include <elle/concurrency/Frame.hh>
# include <elle/concurrency/Function.hh>
# include <elle/concurrency/Method.hh>
# include <elle/concurrency/Mode.hh>
# include <elle/concurrency/Phase.hh>
# include <elle/concurrency/Program.hh>
# include <elle/concurrency/Resource.hh>
# include <elle/concurrency/Routine.hh>
# include <elle/concurrency/Section.hh>
# include <elle/concurrency/Signal.hh>

#endif
