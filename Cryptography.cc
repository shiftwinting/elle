//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/cryptography/Cryptography.cc
//
// created       julien quintard   [tue oct 30 12:16:42 2007]
// updated       julien quintard   [sun mar  7 22:06:18 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/cryptography/Cryptography.hh>

namespace elle
{
  using namespace misc;

  namespace cryptography
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method initialises everything related to the cryptographic
    /// classes.
    ///
    /// \todo
    ///   this method relies on open() which is UNIX-specific: make it
    ///   portable.
    ///
    Status		Cryptography::Initialize()
    {
      uint8_t		temporary[256];
      int		fd;

      enter();

      // initialise the random generator.
      ::srand(::getpid()); 

      // get some random data.
      if ((fd = ::open("/dev/random", O_RDONLY)) == -1)
	escape(::strerror(errno));

      // read random data.
      if (::read(fd, temporary, sizeof(temporary)) == -1)
	{
	  ::close(fd);

	  escape(::strerror(errno));
	}

      // close the file descriptor.
      ::close(fd);

      // seed the random generator.
      ::RAND_seed(temporary, sizeof(temporary));

      // load the crypto error strings.
      ::ERR_load_crypto_strings();

      // enable the SSL algorithms, especially for RSA.
      ::SSLeay_add_all_algorithms();

      // initialize the key pair generation context.
      if (KeyPair::Initialize() == StatusError)
	escape("unable to initialize the key pair generation context");

      leave();
    }

    ///
    /// this method cleans static cryptographic resources.
    ///
    Status		Cryptography::Clean()
    {
      enter();

      // clean the key pair generation context.
      if (KeyPair::Clean() == StatusError)
	escape("unable to initialize the key pair generation context");

      // free the current threads error queue.
      ::ERR_remove_state(0);

      // clean the engine.
      ::ENGINE_cleanup();

      // free the error strings.
      ::ERR_free_strings();

      // clean the evp environment.
      ::EVP_cleanup();

      // release the extra data.
      ::CRYPTO_cleanup_all_ex_data();

      leave();
    }

  }
}
