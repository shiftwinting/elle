#ifndef ELLE_VERSION_HH
# define ELLE_VERSION_HH

# include <elle/types.hh>
# include <elle/Printable.hh>
# include <elle/serialize/fwd.hh>

namespace elle
{
  /// This class represents the version of the Infinit software.
  class Version:
    public elle::Printable
  {
    //
    // construction
    //
  public:
    Version(); // XXX[to remove later]
    Version(elle::Natural8 major,
            elle::Natural8 minor);

    //
    // methods
    //
  public:
    /// Returns the major part of the version number.
    elle::Natural8
    major() const;
    /// Returns the minor part of the version number.
    elle::Natural8
    minor() const;

    //
    // interfaces
    //
  public:
    // printable
    void
    print(std::ostream& stream) const;

    // serializable
    ELLE_SERIALIZE_FRIEND_FOR(Version);

    //
    // operators
    //
    elle::Boolean
    operator ==(Version const&) const;
    elle::Boolean
    operator !=(Version const&) const;
    elle::Boolean
    operator <(Version const&) const;
    elle::Boolean
    operator >(Version const&) const;
    elle::Boolean
    operator <=(Version const&) const;
    elle::Boolean
    operator >=(Version const&) const;

    //
    // attributes
    //
  private:
    elle::Natural8 _major;
    elle::Natural8 _minor;
  };
}

# include <elle/Version.hxx>

#endif
