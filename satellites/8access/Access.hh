//
// ---------- header ----------------------------------------------------------
//
// project       applications
//
// license       infinit
//
// author        julien.quintard   [tue nov  1 12:55:11 2011]
//

#ifndef ACCESS_ACCESS_HH
#define ACCESS_ACCESS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <Infinit.hh>
#include <elle/Elle.hh>
#include <lune/Lune.hh>
#include <etoile/Etoile.hh>
#include <hole/Hole.hh>

namespace application
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class implements the 8access application.
  ///
  class Access
  {
  public:
    //
    // enumerations
    //
    enum Operation
      {
	OperationUnknown = 0,

	OperationLookup,
	OperationConsult,
	OperationGrant,
	OperationRevoke
      };

    //
    // static methods
    //
    static elle::Status		Connect();

    static elle::Status		Lookup(const etoile::path::Way&,
				       const nucleus::Subject&);
    static elle::Status		Consult(const etoile::path::Way&);
    static elle::Status		Grant(const etoile::path::Way&,
				      const nucleus::Subject&,
				      const nucleus::Permissions);
    static elle::Status		Revoke(const etoile::path::Way&,
				       const nucleus::Subject&);

    //
    // static attributes
    //
    static elle::Door		Door;
  };

}

#endif
