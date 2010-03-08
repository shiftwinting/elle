//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/cryptography/Signature.cc
//
// created       julien quintard   [mon feb  2 22:22:12 2009]
// updated       julien quintard   [mon mar  1 13:11:15 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/cryptography/Signature.hh>

namespace elle
{
  using namespace core;
  using namespace misc;
  using namespace archive;

  namespace cryptography
  {

//
// ---------- entity ----------------------------------------------------------
//

    ///
    /// this method check if two signatures match.
    ///
    Boolean		Signature::operator==(const Signature&	element) const
    {
      // compare the regions.
      return (this->region == element.region);
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the signature.
    ///
    Status		Signature::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Signature]" << std::endl;

      if (this->region.Dump(margin + 2) == StatusError)
	escape("unable to dump the region");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes a signature object.
    ///
    Status		Signature::Serialize(Archive&		archive) const
    {
      enter();

      // serialize the region.
      if (archive.Serialize(this->region) == StatusError)
	escape("unable to serialize the region");

      leave();
    }

    ///
    /// this method extracts a signature object.
    ///
    Status		Signature::Extract(Archive&		archive)
    {
      enter();

      // extract the content.
      if (archive.Extract(this->region) == StatusError)
	escape("unable to extract the region");

      leave();
    }

  }
}
