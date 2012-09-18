#include <etoile/wall/Object.hh>
#include <etoile/gear/Identifier.hh>
#include <etoile/gear/Scope.hh>
#include <etoile/gear/Guard.hh>
#include <etoile/gear/Object.hh>
#include <etoile/gear/File.hh>
#include <etoile/gear/Directory.hh>
#include <etoile/gear/Link.hh>
#include <etoile/automaton/Object.hh>
#include <etoile/automaton/Rights.hh>
#include <etoile/depot/Depot.hh>
#include <etoile/journal/Journal.hh>
#include <etoile/abstract/Object.hh>

#include <nucleus/proton/Location.hh>
#include <nucleus/neutron/Object.hh>
#include <nucleus/neutron/Genre.hh>

#include <elle/log.hh>
#include <elle/concurrency/Scheduler.hh>

#include <Infinit.hh>

ELLE_LOG_COMPONENT("infinit.etoile.wall.Object");

namespace etoile
{
  namespace wall
  {
    gear::Identifier
    Object::Load(const path::Chemin& chemin)
    {
      gear::Scope*      scope;
      gear::Object*     context;

      ELLE_TRACE_SCOPE("Load()");

      // acquire the scope.
      if (gear::Scope::Acquire(chemin, scope) == elle::Status::Error)
        throw reactor::Exception(elle::concurrency::scheduler(),
                                 "unable to acquire the scope");

      gear::Guard               guard(scope);

      // If the scope is new i.e there is no attached context, the system
      // needs to know what is the genre of the object, e.g directory, in
      // order allocate an appropriate context.
      if (scope->context == nullptr)
        {
          // In this case, the object is manually loaded in order to determine
          // the genre.
          nucleus::proton::Location location;
          std::unique_ptr<nucleus::neutron::Object> object;

          if (chemin.Locate(location) == elle::Status::Error)
            throw reactor::Exception(elle::concurrency::scheduler(),
                                     "unable to locate the object");

          // XXX[remove try/catch later]
          try
            {
              object = depot::Depot::pull_object(location.address,
                                                 location.revision);
            }
          catch (std::runtime_error& e)
            {
              throw reactor::Exception(
                elle::concurrency::scheduler(),
                elle::sprintf("unable to retrieve the object block: %s",
                              e.what()));
            }

          // Depending on the object's genre, a context is allocated
          // for the scope.
          switch (object->genre())
            {
              case nucleus::neutron::GenreFile:
              {
                gear::File* context;

                if (scope->Use(context) == elle::Status::Error)
                  throw reactor::Exception(elle::concurrency::scheduler(),
                                           "unable to create the context");

                break;
              }
              case nucleus::neutron::GenreDirectory:
              {
                gear::Directory* context;

                if (scope->Use(context) == elle::Status::Error)
                  throw reactor::Exception(elle::concurrency::scheduler(),
                                           "unable to create the context");

                break;
              }
              case nucleus::neutron::GenreLink:
              {
                gear::Link* context;

                if (scope->Use(context) == elle::Status::Error)
                  throw reactor::Exception(elle::concurrency::scheduler(),
                                           "unable to create the context");

                break;
              }
              default:
              {
                // XXX[this whole code should probably be put within the
                //     critical section?]
                printf("[XXX] UNABLE TO ALLOCATE THE PROPER CONTEXT\n");
                object.get()->Dump();

                throw reactor::Exception(elle::concurrency::scheduler(),
                                         "unable to allocate the proper context");
              }
            }

          // At this point, the context represents the real object so
          // that, assuming it is a directory, both Object::* and
          // Directory::* methods could be used.
        }

      // declare a critical section.
      reactor::Lock lock(elle::concurrency::scheduler(), scope->mutex.write());
      {
        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "unable to retrieve the context");

        // allocate an actor.
        guard.actor(new gear::Actor(scope));

        // return the identifier.
        gear::Identifier identifier = guard.actor()->identifier;

        // locate the object based on the chemin.
        if (chemin.Locate(context->location) == elle::Status::Error)
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "unable to locate the object");

        // apply the load automaton on the context.
        if (automaton::Object::Load(*context) == elle::Status::Error)
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "unable to load the object");

        // waive the actor and the scope
        if (guard.Release() == elle::Status::Error)
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "unable to release the guard");
        return identifier;
      }
    }

    bool
    Object::Lock(gear::Identifier const&)
    {
      ELLE_TRACE_SCOPE("Lock()");

      // XXX to implement.

      return true;
    }

    /// Release a previously locked object.
    void
    Object::Release(const gear::Identifier&)
    {
      ELLE_TRACE_SCOPE("Release()");

      // XXX to implement.
    }

    abstract::Object
    Object::Information(const gear::Identifier& identifier)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::Object*     context;

      ELLE_TRACE_SCOPE("Information()");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        throw reactor::Exception(elle::concurrency::scheduler(),
                                 "unable to select the actor");

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      reactor::Lock lock(elle::concurrency::scheduler(), scope->mutex);
      {
        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "unable to retrieve the context");

        // apply the information automaton on the context.
        abstract::Object abstract;
        if (automaton::Object::Information(*context,
                                           abstract) == elle::Status::Error)
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "unable to retrieve general information on the object");
        return abstract;
      }
    }

    void
    Object::Discard(gear::Identifier const& identifier)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::Object*     context;

      ELLE_TRACE_SCOPE("Discard()");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        throw reactor::Exception(elle::concurrency::scheduler(),
                                 "unable to select the actor");

      gear::Guard               guard(actor);

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      reactor::Lock lock(elle::concurrency::scheduler(), scope->mutex.write());
      {
        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "unable to retrieve the context");

        // check the permissions before performing the operation in
        // order not to alter the scope should the operation not be
        // allowed.
        if (automaton::Rights::Operate(
              *context,
              gear::OperationDiscard) == elle::Status::Error)
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "the user does not seem to have the necessary permission for "
                                   "discarding this object");

        // specify the closing operation performed by the actor.
        if (actor->Operate(gear::OperationDiscard) == elle::Status::Error)
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "this operation cannot be performed by this actor");

        // delete the actor.
        guard.actor(nullptr);

        // specify the closing operation performed on the scope.
        if (scope->Operate(gear::OperationDiscard) == elle::Status::Error)
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "unable to specify the operation being performed "
                                   "on the scope");

        // trigger the shutdown.
        if (scope->Shutdown() == elle::Status::Error)
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "unable to trigger the shutdown");
      }

      // depending on the context's state.
      switch (context->state)
        {
          case gear::Context::StateDiscarded:
          case gear::Context::StateStored:
          case gear::Context::StateDestroyed:
          {
            //
            // if the object has been sealed, i.e there is no more actor
            // operating on it, record it in the journal.
            //

            // relinquish the scope: at this point we know there is no
            // remaining actor.
            if (gear::Scope::Relinquish(scope) == elle::Status::Error)
              throw reactor::Exception(elle::concurrency::scheduler(),
                                       "unable to relinquish the scope");

            // record the scope in the journal.
            if (journal::Journal::Record(scope) == elle::Status::Error)
              throw reactor::Exception(elle::concurrency::scheduler(),
                                       "unable to record the scope in the journal");

            break;
          }
          default:
          {
            //
            // otherwise, some actors are probably still working on it.
            //

            break;
          }
        }
    }

    void
    Object::Store(gear::Identifier const& identifier)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::Object*     context;

      ELLE_TRACE_SCOPE("Store()");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        throw reactor::Exception(elle::concurrency::scheduler(),
                                 "unable to select the actor");

      gear::Guard               guard(actor);

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      reactor::Lock lock(elle::concurrency::scheduler(), scope->mutex.write());
      {
        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "unable to retrieve the context");

        // check the permissions before performing the operation in
        // order not to alter the scope should the operation not be
        // allowed.
        if (automaton::Rights::Operate(
              *context,
              gear::OperationStore) == elle::Status::Error)
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "the user does not seem to have the necessary permission for "
                                   "storing this object");

        // specify the closing operation performed by the actor.
        if (actor->Operate(gear::OperationStore) == elle::Status::Error)
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "this operation cannot be performed by this actor");

        // delete the actor.
        guard.actor(nullptr);

        // specify the closing operation performed on the scope.
        if (scope->Operate(gear::OperationStore) == elle::Status::Error)
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "unable to specify the operation being performed "
                                   "on the scope");

        // trigger the shutdown.
        if (scope->Shutdown() == elle::Status::Error)
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "unable to trigger the shutdown");
      }

      // depending on the context's state.
      switch (context->state)
        {
          case gear::Context::StateDiscarded:
          case gear::Context::StateStored:
          case gear::Context::StateDestroyed:
          {
            //
            // if the object has been sealed, i.e there is no more actor
            // operating on it, record it in the journal.
            //

            // relinquish the scope: at this point we know there is no
            // remaining actor.
            if (gear::Scope::Relinquish(scope) == elle::Status::Error)
              throw reactor::Exception(elle::concurrency::scheduler(),
                                       "unable to relinquish the scope");

            // record the scope in the journal.
            if (journal::Journal::Record(scope) == elle::Status::Error)
              throw reactor::Exception(elle::concurrency::scheduler(),
                                       "unable to record the scope in the journal");

            break;
          }
          default:
          {
            //
            // otherwise, some actors are probably still working on it.
            //

            break;
          }
        }
    }

    void
    Object::Destroy(gear::Identifier const& identifier)
    {
      gear::Actor*      actor;
      gear::Scope*      scope;
      gear::Object*     context;

      ELLE_TRACE_SCOPE("Destroy()");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::Status::Error)
        throw reactor::Exception(elle::concurrency::scheduler(),
                                 "unable to select the actor");

      gear::Guard               guard(actor);

      // retrieve the scope.
      scope = actor->scope;

      // declare a critical section.
      reactor::Lock lock(elle::concurrency::scheduler(), scope->mutex.write());
      {
        // retrieve the context.
        if (scope->Use(context) == elle::Status::Error)
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "unable to retrieve the context");

        // check the permissions before performing the operation in
        // order not to alter the scope should the operation not be
        // allowed.
        if (automaton::Rights::Operate(
              *context,
              gear::OperationDestroy) == elle::Status::Error)
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "the user does not seem to have the necessary permission for "
                                   "destroying this object");

        // specify the closing operation performed by the actor.
        if (actor->Operate(gear::OperationDestroy) == elle::Status::Error)
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "this operation cannot be performed by this actor");

        // delete the actor.
        guard.actor(nullptr);

        // specify the closing operation performed on the scope.
        if (scope->Operate(gear::OperationDestroy) == elle::Status::Error)
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "unable to specify the operation being performed "
                                   "on the scope");

        // trigger the shutdown.
        if (scope->Shutdown() == elle::Status::Error)
          throw reactor::Exception(elle::concurrency::scheduler(),
                                   "unable to trigger the shutdown");
      }

      // depending on the context's state.
      switch (context->state)
        {
          case gear::Context::StateDiscarded:
          case gear::Context::StateStored:
          case gear::Context::StateDestroyed:
          {
            //
            // if the object has been sealed, i.e there is no more actor
            // operating on it, record it in the journal.
            //

            // relinquish the scope: at this point we know there is no
            // remaining actor.
            if (gear::Scope::Relinquish(scope) == elle::Status::Error)
              throw reactor::Exception(elle::concurrency::scheduler(),
                                       "unable to relinquish the scope");

            // record the scope in the journal.
            if (journal::Journal::Record(scope) == elle::Status::Error)
              throw reactor::Exception(elle::concurrency::scheduler(),
                                       "unable to record the scope in the journal");

            break;
          }
          default:
          {
            //
            // otherwise, some actors are probably still working on it.
            //

            break;
          }
        }
    }

    void
    Object::Purge(gear::Identifier const&)
    {
      ELLE_TRACE_SCOPE("Purge()");

      // XXX to implement.
    }

  }
}
