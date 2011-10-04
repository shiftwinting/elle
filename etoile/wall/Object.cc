//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [wed mar  3 20:50:57 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/wall/Object.hh>

#include <etoile/gear/Identifier.hh>
#include <etoile/gear/Nature.hh>
#include <etoile/gear/Scope.hh>
#include <etoile/gear/Object.hh>
#include <etoile/gear/Gear.hh>

#include <etoile/automaton/Object.hh>

#include <etoile/journal/Journal.hh>

#include <etoile/miscellaneous/Information.hh>

#include <Infinit.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method loads the object identified by _chemin_ into a new
    /// scope and returns the scope's identifier so that subsequent
    /// operations can be carried out.
    ///
    elle::Status	Object::Load(
			  const path::Chemin&			chemin,
			  gear::Identifier&			identifier)
    {
      gear::Scope*	scope;
      gear::Object*	context;
      gear::Actor*	actor;
      nucleus::Location	location;

      enter(instance(actor));

      // debug.
      if (Infinit::Configuration.debug.etoile == true)
	printf("[etoile] wall::Object::Load()\n");

      // acquire the scope.
      if (gear::Scope::Acquire(chemin, scope) == elle::StatusError)
	escape("unable to acquire the scope");

      // retrieve the context.
      if (scope->Use(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // allocate an actor.
      actor = new gear::Actor(scope);

      // attach the actor to the scope.
      if (actor->Attach() == elle::StatusError)
	escape("unable to attach the actor to the scope");

      // return the identifier.
      identifier = actor->identifier;

      // waive the actor.
      waive(actor);

      // locate the object based on the chemin.
      if (chemin.Locate(location) == elle::StatusError)
	escape("unable to locate the directory");

      // apply the load automaton on the context.
      if (automaton::Object::Load(*context,
				  location) == elle::StatusError)
	escape("unable to load the object");

      leave();
    }

    ///
    /// this method locks the object.
    ///
    /// the method returns true if the lock has been acquired, false
    /// otherwise.
    ///
    elle::Status	Object::Lock(
			  const gear::Identifier&)
    {
      enter();

      // debug.
      if (Infinit::Configuration.debug.etoile == true)
	printf("[etoile] wall::Object::Lock()\n");

      // XXX to implement.

      leave();
    }

    ///
    /// this method releases a previously locked object.
    ///
    elle::Status	Object::Release(
			  const gear::Identifier&)
    {
      enter();

      // debug.
      if (Infinit::Configuration.debug.etoile == true)
	printf("[etoile] wall::Object::Release()\n");

      // XXX to implement.

      leave();
    }

    ///
    /// this method returns general information regarding the identified
    /// object.
    ///
    elle::Status	Object::Information(
			  const gear::Identifier&		identifier,
			  miscellaneous::Information&		information)
    {
      gear::Actor*	actor;
      gear::Object*	context;

      enter();

      // debug.
      if (Infinit::Configuration.debug.etoile == true)
	printf("[etoile] wall::Object::Information()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
	escape("unable to select the actor");

      // retrieve the context.
      if (actor->scope->Use(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // apply the information automaton on the context.
      if (automaton::Object::Information(*context,
					 information) == elle::StatusError)
	escape("unable to retrieve general information on the object");

      leave();
    }

    ///
    /// this method discards the scope hence potentially ignoring some
    /// modifications.
    ///
    elle::Status	Object::Discard(
			  const gear::Identifier&		identifier)
    {
      elle::Callback<
	elle::Status,
	elle::Parameters<
	  gear::Object&
	  >
	>		callback;
      gear::Actor*	actor;
      gear::Object*	context;

      enter(instance(actor));

      // debug.
      if (Infinit::Configuration.debug.etoile == true)
	printf("[etoile] wall::Object::Discard()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
	escape("unable to select the actor");

      // specify the closing operation performed by the actor.
      if (actor->Operate(gear::OperationDiscard) == elle::StatusError)
	escape("this operation cannot be performed by this actor");

      // specify the closing operation performed on the scope.
      if (actor->scope->Operate<automaton::Object>(
	    gear::OperationDiscard,
	    callback) == elle::StatusError)
	escape("unable to specify the operation being performed on the scope");

      // retrieve the context.
      if (actor->scope->Use(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // trigger the closing callback.
      if (callback.Call(*context) == elle::StatusError)
	escape("unable to perform the closing operation");

      // detach the actor.
      if (actor->Detach() == elle::StatusError)
	escape("unable to detach the actor from the scope");

      // record the scope in the journal.
      if (journal::Journal::Record(actor->scope) == elle::StatusError)
	escape("unable to record the scope in the journal");

      // delete the actor.
      delete actor;

      // waive actor.
      waive(actor);

      leave();
    }

    ///
    /// this method commits the pending modifications by placing the
    /// scope in the journal.
    ///
    elle::Status	Object::Store(
			  const gear::Identifier&		identifier)
    {
      elle::Callback<
	elle::Status,
	elle::Parameters<
	  gear::Object&
	  >
	>		callback;
      gear::Object*	context;
      gear::Actor*	actor;

      enter(instance(actor));

      // debug.
      if (Infinit::Configuration.debug.etoile == true)
	printf("[etoile] wall::Object::Store()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
	escape("unable to select the actor");

      // specify the closing operation performed by the actor.
      if (actor->Operate(gear::OperationStore) == elle::StatusError)
	escape("this operation cannot be performed by this actor");

      // specify the closing operation performed on the scope.
      if (actor->scope->Operate<automaton::Object>(
	    gear::OperationStore,
	    callback) == elle::StatusError)
	escape("unable to specify the operation being performed on the scope");

      // retrieve the context.
      if (actor->scope->Use(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // trigger the closing callback.
      if (callback.Call(*context) == elle::StatusError)
	escape("unable to perform the closing operation");

      // detach the actor.
      if (actor->Detach() == elle::StatusError)
	escape("unable to detach the actor from the scope");

      // record the scope in the journal.
      if (journal::Journal::Record(actor->scope) == elle::StatusError)
	escape("unable to record the scope in the journal");

      // delete the actor.
      delete actor;

      // waive actor.
      waive(actor);

      leave();
    }

    ///
    /// this method destroys an object.
    ///
    /// this method should be used with great care since, not knowing the
    /// object's genre, the data blocks will not be removed. therefore,
    /// the genre-specific Destroy() method should always be preferred.
    ///
    elle::Status	Object::Destroy(
			  const gear::Identifier&		identifier)
    {
      elle::Callback<
	elle::Status,
	elle::Parameters<
	  gear::Object&
	  >
	>		callback;
      gear::Actor*	actor;
      gear::Object*	context;

      enter(instance(actor));

      // debug.
      if (Infinit::Configuration.debug.etoile == true)
	printf("[etoile] wall::Object::Destroy()\n");

      // select the actor.
      if (gear::Actor::Select(identifier, actor) == elle::StatusError)
	escape("unable to select the actor");

      // specify the closing operation performed by the actor.
      if (actor->Operate(gear::OperationDestroy) == elle::StatusError)
	escape("this operation cannot be performed by this actor");

      // specify the closing operation performed on the scope.
      if (actor->scope->Operate<automaton::Object>(
	    gear::OperationDestroy,
	    callback) == elle::StatusError)
	escape("unable to specify the operation being performed on the scope");

      // retrieve the context.
      if (actor->scope->Use(context) == elle::StatusError)
	escape("unable to retrieve the context");

      // trigger the closing callback.
      if (callback.Call(*context) == elle::StatusError)
	escape("unable to perform the closing operation");

      // detach the actor.
      if (actor->Detach() == elle::StatusError)
	escape("unable to detach the actor from the scope");

      // record the scope in the journal.
      if (journal::Journal::Record(actor->scope) == elle::StatusError)
	escape("unable to record the scope in the journal");

      // delete the actor.
      delete actor;

      // waive actor.
      waive(actor);

      leave();
    }

    ///
    /// this method purges an object i.e removes all the blocks of all
    /// the versions associated with this object.
    ///
    /// this method should be used with great care since, not knowing the
    /// object's genre, the data blocks will not be removed. therefore,
    /// the genre-specific Destroy() method should always be preferred.
    ///
    elle::Status	Object::Purge(
			  const gear::Identifier&)
    {
      enter();

      // debug.
      if (Infinit::Configuration.debug.etoile == true)
	printf("[etoile] wall::Object::Purge()\n");

      // XXX to implement.

      leave();
    }

  }
}
