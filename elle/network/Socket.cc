//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/network/Socket.cc
//
// created       julien quintard   [wed feb  3 12:55:47 2010]
// updated       julien quintard   [thu mar 25 18:05:38 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/network/Socket.hh>

namespace elle
{
  namespace network
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the default constructor
    ///
    Socket::Socket():
      type(Socket::TypeUnknown),
      callback(NULL)
    {
    }

    ///
    /// a constructor which specifies the type of socket.
    ///
    Socket::Socket(const Socket::Type&				type):
      type(type),
      callback(NULL)
    {
    }

    ///
    /// destructor.
    ///
    Socket::~Socket()
    {
      // release the callback.
      if (this->callback != NULL)
	delete this->callback;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method registers the callback that will be triggered
    /// should an error occur on the socket.
    ///
    Status		Socket::Monitor(Callback<const String>&	callback)
    {
      enter();

      // delete a previous callback.
      if (this->callback != NULL)
	delete this->callback;

      // allocate and copy a new callback.
      this->callback = new Callback<const String>(callback);

      leave();
    }

    ///
    /// this method withdraws the control management by no longer
    /// forwarding the notifications.
    ///
    Status		Socket::Withdraw()
    {
      enter();

      // delete the callback.
      if (this->callback != NULL)
	delete this->callback;

      // set the callback to null.
      this->callback = NULL;

      leave();
    }

  }
}
