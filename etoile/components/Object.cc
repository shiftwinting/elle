//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/components/Object.cc
//
// created       julien quintard   [fri aug 14 19:16:10 2009]
// updated       julien quintard   [mon aug 17 12:36:45 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/components/Object.hh>

namespace etoile
{
  namespace components
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    Status		Object::Load(context::Object&		context,
				     const hole::Address&	address)
    {
      core::Object*	object;

      // set the component.
      context.component = core::ComponentObject;

      // set the object address.
      context.address = address;

      // get the block from Hole.
      if (hole::Hole::Get(address, context.object) == StatusError)
	escape("unable to retrieve the block from Hole");

      // retrieve the subject's rights. that also means that, if the
      // subject is a consumer, the system should be able to find
      // a delegate to vouch for her.
      if (Object::Rights(context, context.rights) == StatusError)
	escape("unable to retrieve the rights");

      leave();
    }

    ///
    /// XXX
    ///
    Status		Object::Store(context::Object&		context)
    {
      // XXX

      leave();
    }

    ///
    /// this method retrieve the permissions the given subject has over
    /// the object.
    ///
    /// \todo add a default argument for the subject being a user or group.
    ///
    Status		Object::Rights(context::Object&		context,
				       core::Permissions&	rights,
				       const core::Subject&	subject)
    {
      // test if the subject is the owner.
      if ((subject.type == core::Subject::TypeUser) &&
	  (*subject.identifier.user == context.object.owner.K))
	{
	  // return the permissions.
	  rights = context.object.meta.owner.permissions;
	}
      else
	{
	  // XXX delegate or consumer
	  printf("XXX[NOT IMPLEMENTED YET]\n");
	}

      leave();
    }

  }
}
