#if defined(ELLE_CRYPTOGRAPHY_ROTATION)

# include <cryptography/Seed.hh>
# include <cryptography/PrivateKey.hh>
# include <cryptography/PublicKey.hh>
# include <cryptography/KeyPair.hh>
# include <cryptography/random.hh>
# include <cryptography/rsa/Seed.hh>

# include <elle/log.hh>
# include <elle/assert.hh>

ELLE_LOG_COMPONENT("infinit.cryptography.Seed");

namespace infinit
{
  namespace cryptography
  {
    /*---------------.
    | Static Methods |
    `---------------*/

    Seed
    Seed::generate(KeyPair const& pair)
    {
      ELLE_TRACE_FUNCTION(pair);

      switch (pair.cryptosystem())
      {
        case Cryptosystem::rsa:
        {
          // Construct a high-level seed by providing its freshly generated
          // implementation.

          std::unique_ptr<seed::Interface> implementation(
            new rsa::Seed(
              rsa::seed::generate(pair.K().implementation(),
                                  pair.k().implementation())));

          return (Seed(std::move(implementation)));
        }
        default:
          throw Exception(
            elle::sprintf("unknown or non-supported asymmetric "
                          "cryptosystem '%s'", pair.cryptosystem()));
      }

      elle::unreachable();
    }

    /*-------------.
    | Construction |
    `-------------*/

    Seed::Seed()
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    Seed::Seed(std::unique_ptr<seed::Interface>&& implementation):
      _implementation(std::move(implementation))
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    Seed::Seed(Seed const& other):
      _implementation(other._implementation->clone())
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    Seed::Seed(Seed&& other):
      _implementation(std::move(other._implementation))
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    ELLE_SERIALIZE_CONSTRUCT_DEFINE(Seed)
    {
      // Make sure the cryptographic system is set up.
      cryptography::require();
    }

    /*--------.
    | Methods |
    `--------*/

    Cryptosystem
    Seed::cryptosystem() const
    {
      ELLE_TRACE_METHOD("");

      ELLE_ASSERT_NEQ(this->_implementation, nullptr);

      return (this->_implementation->cryptosystem());
    }

    seed::Interface const&
    Seed::implementation() const
    {
      ELLE_TRACE_METHOD("");

      ELLE_ASSERT_NEQ(this->_implementation, nullptr);

      return (*this->_implementation);
    }

    /*----------.
    | Operators |
    `----------*/

    elle::Boolean
    Seed::operator ==(Seed const& other) const
    {
      if (this == &other)
        return (true);

      ELLE_ASSERT_NEQ(this->_implementation, nullptr);
      ELLE_ASSERT_NEQ(other._implementation, nullptr);

      return (*this->_implementation == *other._implementation);
    }

    /*----------.
    | Printable |
    `----------*/

    void
    Seed::print(std::ostream& stream) const
    {
      ELLE_ASSERT_NEQ(this->_implementation, nullptr);

      this->_implementation->print(stream);
    }
  }
}

//
// ---------- Factory ---------------------------------------------------------
//

namespace infinit
{
  namespace cryptography
  {
    namespace seed
    {
      /*-----------------.
      | Static Functions |
      `-----------------*/

      static
      elle::utility::Factory<Cryptosystem>
      _factory()
      {
        ELLE_DEBUG_FUNCTION("");

        elle::utility::Factory<Cryptosystem> factory;

        factory.record<rsa::Seed>(Cryptosystem::rsa);

        return (factory);
      }

      /*----------.
      | Functions |
      `----------*/

      elle::utility::Factory<Cryptosystem> const&
      factory()
      {
        static elle::utility::Factory<Cryptosystem> factory = _factory();

        return (factory);
      }
    }
  }
}

#endif
