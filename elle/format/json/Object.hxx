#ifndef  ELLE_FORMAT_JSON_OBJECT_HXX
# define ELLE_FORMAT_JSON_OBJECT_HXX

# include <typeinfo>

# include "_detail.hh"

# include "Float.hh"
# include "String.hh"
# include "Integer.hh"
# include "Bool.hh"
# include "Null.hh"
# include "Dictionary.hh"
# include "Array.hh"

namespace elle
{
  namespace format
  {
    namespace json
    {

      template <typename T>
      void Object::Load(T& out) const
      {
        static_assert(
            !std::is_base_of<T, Object>::value,
            "Cannot load into a json object"
        );
        typedef typename detail::SelectJSONType<T>::type SelfType;
        out = dynamic_cast<SelfType const&>(*this);
      }

      template <typename T>
      T Object::as() const
      {
        T val;
        this->Load(val);
        return val;
      }

      template <typename T>
      bool Object::TryLoad(T& out) const
      {
        typedef typename detail::SelectJSONType<T>::type SelfType;
        if (auto ptr = dynamic_cast<SelfType const*>(this))
          {
            out = *ptr;
            return true;
          }
        return false;
      }


      template <typename T> typename std::enable_if<
            !std::is_base_of<T, Object>::value
          , bool
      >::type Object::operator ==(T const& other) const
      {
        typedef typename detail::SelectJSONType<T>::type SelfType;
        if (auto self = dynamic_cast<SelfType const*>(this))
          return *self == SelfType(other);
        return false;
      }

    struct Object::Factory
    {
      template <typename T> static inline
      typename std::enable_if<
          std::is_same<T, Null>::value
        , std::unique_ptr<Null>
      >::type
      Construct(T const&)
      {
        return std::unique_ptr<Null>(new Null);
      }

      template <typename T> static inline typename std::enable_if<
            std::is_same<T , bool>::value
          , std::unique_ptr<Bool>
        >::type Construct(T value)
          { return std::unique_ptr<Bool>(new Bool(value)); }

        template<typename T> static inline typename std::enable_if<
                std::is_integral<T>::value
            &&  !std::is_same<T , bool>::value
          , std::unique_ptr<Integer>
        >::type Construct(T value)
          { return std::unique_ptr<Integer>(new Integer(value)); }

        template<typename T> static inline typename std::enable_if<
            std::is_floating_point<T>::value
          , std::unique_ptr<Float>
        >::type Construct(T value)
          { return std::unique_ptr<Float>(new Float(value)); }

        template<typename T> static inline typename std::enable_if<
            detail::IsString<T>::value
          , std::unique_ptr<String>
        >::type Construct(T const& value)
          { return std::unique_ptr<String>(new String(value)); }

        template<typename T> static inline typename std::enable_if<
            detail::IsArray<T>::value
          , std::unique_ptr<Array>
        >::type Construct(T const& value)
          { return std::unique_ptr<Array>(new Array(value)); }

        template<typename T> static inline typename std::enable_if<
            detail::IsStringMap<T>::value
          , std::unique_ptr<Dictionary>
        >::type Construct(T const& value)
          { return std::unique_ptr<Dictionary>(new Dictionary(value)); }

        template<typename T> static inline typename std::enable_if<
              std::is_base_of<Object, T>::value
          &&  !std::is_same<T, Null>::value
          , std::unique_ptr<T>
        >::type Construct(T const& value)
          {
            Object* ptr = value.Clone().release();
            assert(dynamic_cast<T*>(ptr) != nullptr);
            return std::unique_ptr<T>(static_cast<T*>(ptr));
          }

        template<typename T> static inline typename std::enable_if<
              std::is_pointer<T>::value
          &&  !std::is_array<T>::value
        >::type Construct(T const&)
          { static_assert(!std::is_pointer<T>::value, "You cannot build JSON Object from a pointer"); }
      };

      Array&      Object::as_array()      { return dynamic_cast<Array&>(*this); }
      Bool&       Object::as_bool()       { return dynamic_cast<Bool&>(*this); }
      Dictionary& Object::as_dictionary() { return dynamic_cast<Dictionary&>(*this); }
      Float&      Object::as_float()      { return dynamic_cast<Float&>(*this); }
      Integer&    Object::as_integer()    { return dynamic_cast<Integer&>(*this); }
      Null&       Object::as_null()       { return dynamic_cast<Null&>(*this); }
      String&     Object::as_string()     { return dynamic_cast<String&>(*this); }

      Array const&      Object::as_array() const { return dynamic_cast<Array const&>(*this); }
      Bool const&       Object::as_bool() const { return dynamic_cast<Bool const&>(*this); }
      Dictionary const& Object::as_dictionary() const { return dynamic_cast<Dictionary const&>(*this); }
      Float const&      Object::as_float() const { return dynamic_cast<Float const&>(*this); }
      Integer const&    Object::as_integer() const { return dynamic_cast<Integer const&>(*this); }
      Null const&       Object::as_null() const { return dynamic_cast<Null const&>(*this); }
      String const&     Object::as_string() const { return dynamic_cast<String const&>(*this); }

}}} // !namespace elle::format::json

# include "Array.hxx" // XXX fix undefined reference in Object::Load<list>()

#endif


