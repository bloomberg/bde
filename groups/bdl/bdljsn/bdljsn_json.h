// bdljsn_json.h                                                      -*-C++-*-
#ifndef INCLUDED_BDLJSN_JSON
#define INCLUDED_BDLJSN_JSON

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an in-memory representation of a JSON document.
//
//@CLASSES:
//  bdljsn::Json: representation of a JSON document
//  bdljsn::JsonArray: representation of a JSON array
//  bdljsn::JsonObject: representation of a JSON object
//
//@SEE_ALSO: bdljsn_jsonutil, bdljsn_jsonnumber, bdljsn_jsonnull
//
//@DESCRIPTION: This component provides a value-semantic type, 'bdljsn::Json',
// used as an in-memory representation for a JSON document.  This component
// also provides 'bdljsn::JsonArray' and 'bdljsn::JsonObject' types for
// representing JSON Arrays and Objects respectively.
//
// 'bdljsn::Json' has a close structural similarity to the JSON grammar itself,
// which at a high-level looks like:
//..
//  JSON ::= Object
//         | Array
//         | String
//         | Number
//         | Boolean
//         | null
//..
// Where the Object and Array alternatives can recursively contain JSON.  Just
// like this grammar, a 'bdljsn::Json' is a variant holding either an Object,
// Array, String, Number, Boolean, or null.  These variant selections are
// represented by the following types:
//
//: o Object: 'JsonObject'
//: o Array: 'JsonArray'
//: o String: 'bsl::string'
//: o Number: 'JsonNumber'
//: o Boolean: 'bool'
//: o null: 'JsonNull'
//
// For more details on the JSON grammar see:
//
//: o website: https://www.json.org/
//: o RFC: https://datatracker.ietf.org/doc/html/rfc8259
//
///Reading and Writing a 'bdljsn::Json' Object
///-------------------------------------------
// 'bdljsn_jsonutil' is the recommended facility to write and read
// 'bdljsn::Json' objects to and from JSON document text.
//
// 'operator<<' overloads are available for all the types.  In addition a
// canonical BDE print method is available for 'bdljsn::Json',
// 'bdljsn::JsonObject', 'bdljsn::JsonArray' 'bdljsn::JsonNumber' and
// 'bdljsn::JsonNull'.
//
///All 'bdljsn::Json' Objects are Valid JSON Documents
///---------------------------------------------------
// Every 'bdljsn::Json' represents a (valid) JSON document ('bdljsn::Json' does
// not have an "invalid" state).  This means writing a 'bdljsn::Json' to a
// string can only fail if an out-of-memory condition occurs.
//
// Similarly, every JSON document has a 'bdljsn::Json' representation.
// However, 'bdljsn::JsonObject' represents Objects having only unique member
// names, meaning that duplicate member names in a JSON document will be
// ignored (see 'bdljsn::JsonUtil' for more information on how duplicate names
// are handled when parsing a JSON document).  Note that the JSON RFC says that
// Object member names "SHOULD be unique", and there is no standard behavior
// for JSON parsers where member names are not unique (typically an in-process
// representation with unique member names is used).
//
///Important Preconditions
///- - - - - - - - - - - -
// In order to preserve the invariant that all 'bdljsn::Json' objects are valid
// JSON documents there are some constructors and assignment operations in
// 'bdljsn' package that have notable preconditions:
//
//: o 'bdljsn::JsonNumber' constructors from string require that the string
//:    conform to the JSON grammar for a number (see
//:   'bdljsn::NumberUtil::isValidNumber').
//:
//: o Constructors and assignment operators from 'double' or
//:   'bdldfp::Decimal64' require the value *not* be either INF or NaN
//:
//: o 'bdljsn::Json' constructors and assignment operators require that
//:    strings contain valid UTF-8 (see 'bdlde::Utf8Util::isValid').
//
///'operator==' and the Definition of Value
///----------------------------------------
// 'bdljsn::Json' type's definition of value (i.e., the behavior for
// 'operator==') mirrors comparing the text of two JSON documents where all the
// white-space is ignored.
//
// Concretely, 'bdljsn::Json' is a variant type, whose definition of equality
// is derived from the definition of equality of its constituent types.  I.e.,
// two 'bdljsn::Json' objects compare equal if they have the same 'type' and
// the two objects of that 'type' they contain compare equal.  The definition
// of equality for Object, Array, Boolean, string, and 'JsonNull' types are
// relatively self-explanatory (see respective 'operator==' definitions for
// details).  The definition of equality for 'JsonNumber' is notable:
// 'JsonNumber' objects define value in terms of the text of the JSON number
// string they contain.  So two JSON numbers having the same numerical value
// may compare *unequal* (e.g., "2" and "2.0" and "20e-1" are considered
// different 'JsonNumber' values!).  Note that 'bdljsn::JsonNumber::isEqual'
// provides a semantic comparison of two numbers (see 'bdljsn_jsonnumber' for
// more detail).
//
///Usage
///-----
// This section illustrates the intended use of this component.
//
///Example 1: Constructor a Basic 'bdljsn::Json' Object
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// Most often 'bdljsn::Json' objects will be written and read from JSON text
// using 'bdljsn_jsonutil'.  In this simple example, we demonstrate manually
// creating the document below and then verify the properties of the resulting
// object:
//..
//  {
//      "number": 3.14,
//      "boolean": true,
//      "string": "text",
//      "null": null,
//      "array": [ "2.76", true ],
//      "object": { "boolean": false }
//  }
//..
// First, we use 'bdljsn::Json::makeObject' to configure the top level
// 'bdljsn::Json' object to be a 'bdljsn::Json' object, and use the various
// manipulators of 'bdljsn::JsonObject' to configure its value:
//..
//  using namespace bdldfp::DecimalLiterals;
//
//  bdljsn::Json json;
//
//  json.makeObject();
//  json["number"]  = 3.14;
//  json["boolean"] = true;
//  json["string"]  = "text";
//  json["array"].makeArray();
//  json["array"].theArray().pushBack(bdljsn::Json(2.76_d64));
//  json["array"].theArray().pushBack(bdljsn::Json(true));
//  json["object"].makeObject()["boolean"] = false;
//..
// Notice that we used 'operator[]' to implicitly create new members of the
// top-level object.  Using 'json.theObject().insert' would be more efficient
// (see example 2).
//
// Finally, we validate the properties of the resulting object:
//..
//  assert(3.14     == json["number"].asDouble());
//  assert(true     == json["boolean"].theBoolean());
//  assert("text"   == json["string"].theString());
//  assert(true     == json["null"].isNull());
//  assert(2.76_d64 == json["array"][0].asDecimal64());
//  assert(false    == json["object"]["boolean"].theBoolean());
//..
//
///Example 2: More Efficiently Creating a 'bdljsn::Json'
///-----------------------------------------------------
// Example 1 used 'operator[]' to implicitly add members to the Objects.  Using
// 'operator[]' is intuitive but not the most efficient method to add new
// members to a 'bdljsn::JsonObject' (similar to using 'operator[]' to add
// elements to an 'unordered_map').  The following code demonstrates a more
// efficient way to create the same 'bdljsn::Json' representation as example 1:
//..
//  using namespace bdldfp::DecimalLiterals;
//
//  bdljsn::Json       json;
//  bdljsn::JsonArray  subArray;
//  bdljsn::JsonObject subObject;
//
//  json.makeObject();
//  json.theObject().insert("number", bdljsn::JsonNumber(3.14));
//  json.theObject().insert("boolean", true);
//  json.theObject().insert("string", "text");
//  json.theObject().insert("null", bdljsn::JsonNull());
//
//  subArray.pushBack(bdljsn::Json(2.76_d64));
//  subArray.pushBack(bdljsn::Json(true));
//  json.theObject().insert("array", bsl::move(subArray));
//
//  subObject.insert("boolean", false);
//  json.theObject().insert("object", bsl::move(subObject));
//..
#include <bdlscm_version.h>

#include <bdlb_variant.h>

#include <bdlde_utf8util.h>

#include <bdljsn_jsonnull.h>
#include <bdljsn_jsonnumber.h>
#include <bdljsn_jsontype.h>

#include <bdlb_transparentequalto.h>
#include <bdlb_transparenthash.h>
#include <bdldfp_decimal.h>

#include <bslma_allocator.h>
#include <bslma_stdallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_assert.h>
#include <bslmf_enableif.h>
#include <bslmf_issame.h>
#include <bslmf_movableref.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bslmf_util.h>

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_types.h>
#include <bsls_util.h>

#include <bsl_iterator.h>
#include <bsl_unordered_map.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace bdljsn {

// FORWARD DECLARATIONS
class Json;
class JsonArray;
class JsonObject;

                              // ===============
                              // class JsonArray
                              // ===============

class JsonArray {
    // This type is designed to replicate much of the standard sequence
    // container interface, eliding interfaces that are less relevant for a
    // non-generic container.  Note that a 'bsl::vector' is chosen for the
    // implementation because our implementation permits the element type to be
    // incomplete (when just spelling the type name).

  private:
    // PRIVATE TYPES
    typedef bsl::vector<Json> Elements;

  public:
    // TYPES
    typedef Elements::iterator        Iterator;
    typedef Elements::const_iterator  ConstIterator;

    typedef Elements::value_type      value_type;
    typedef Elements::reference       reference;
    typedef Elements::const_reference const_reference;

    typedef Elements::iterator        iterator;
    typedef Elements::const_iterator  const_iterator;

    typedef Elements::difference_type difference_type;
    typedef Elements::size_type       size_type;

  private:
    // DATA
    Elements d_elements;  // the underlying sequence of elements

    // FRIENDS
    friend bool operator==(const JsonArray&, const JsonArray&);
    friend bool operator!=(const JsonArray&, const JsonArray&);
    template <class HASHALG>
    friend void hashAppend(HASHALG&, const JsonArray&);
    friend void swap(JsonArray&, JsonArray&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(JsonArray, bslma::UsesBslmaAllocator);
    BSLMF_NESTED_TRAIT_DECLARATION(JsonArray, bslmf::IsBitwiseMoveable);
    BSLMF_NESTED_TRAIT_DECLARATION(JsonArray, bdlb::HasPrintMethod);

    // CREATORS
    JsonArray();
    explicit JsonArray(bslma::Allocator *basicAllocator);
        // Create an empty 'JsonArray'.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is not specified, the
        // currently installed default allocator is used.

    JsonArray(const JsonArray& original, bslma::Allocator *basicAllocator = 0);
        // Create a 'JsonArray' having the same value as the specified
        // 'original' object.  Optionally specify 'basicAllocator' to supply
        // memory.  If 'basicAllocator' is not specified, the allocator
        // associated with 'original' is propagated for use in the
        // newly-created 'JsonArray'.

    JsonArray(bslmf::MovableRef<JsonArray> original) BSLS_KEYWORD_NOEXCEPT;
        // Create a 'JsonArray' having the same value as the specified
        // 'original' object by moving (in constant time) the contents of
        // 'original' to the new 'JsonArray' object.  The allocator associated
        // with 'original' is propagated for use in the newly-created
        // 'JsonArray' object.  'original' is left in a valid but unspecified
        // state.

    JsonArray(bslmf::MovableRef<JsonArray>  original,
              bslma::Allocator             *basicAllocator);
        // Create a 'JsonArray' having the same value as the specified
        // 'original' object that uses the specified 'basicAllocator' to supply
        // memory.  The contents of 'original' are moved (in constant time) to
        // the new 'JsonArray' object if
        // 'basicAllocator == original.allocator()', and are move-inserted (in
        // linear time) using 'basicAllocator' otherwise.  'original' is left
        // in a valid but unspecified state.

    template <class INPUT_ITERATOR>
    JsonArray(INPUT_ITERATOR    first,
              INPUT_ITERATOR    last,
              bslma::Allocator *basicAllocator = 0);
        // Create a 'JsonArray' and insert (in order) each 'Json' object in the
        // range starting at the specified 'first' element, and ending
        // immediately before the specified 'last' element.  Optionally specify
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is not
        // specified, the currently installed default allocator is used.  Throw
        // 'bsl::length_error' if the number of elements in '[first .. last)'
        // exceeds the value returned by the method 'maxSize'.  The (template
        // parameter) type 'INPUT_ITERATOR' shall meet the requirements of an
        // input iterator defined in the c++11 standard [24.2.3] providing
        // access to values of a type convertible to 'Json', and 'Json' must be
        // 'emplace-constructible' form '*i', where 'i' is a dereferenceable
        // iterator in the range '[first .. last)'.  The behavior is undefined
        // unless 'first' and 'last' refer to a range of valid values where
        // 'first' is at a position at or before 'last'.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)

    JsonArray(std::initializer_list<Json>  elements,
    bslma::Allocator                      *basicAllocator = 0);     // IMPLICIT
        // Create a 'JsonArray' and insert (in order) each 'Json' object in the
        // specified 'elements' initializer list.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is not
        // specified, the currently installed default allocator is used.

#endif

    // MANIPULATORS
    JsonArray& operator=(const JsonArray& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  If
        // an exception is thrown, '*this' is left in a valid but unspecified
        // state.

    JsonArray& operator=(bslmf::MovableRef<JsonArray> rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  The
        // contents of 'rhs' are moved (in constant time) to this JsonArray if
        // 'allocator() == rhs.allocator()'; otherwise, all elements in this
        // 'JsonArray' are either destroyed or move-assigned to and each
        // additional element in 'rhs' is move-inserted into this JsonArray.
        // 'rhs' is left in a valid but unspecified state.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    JsonArray& operator=(std::initializer_list<Json> initializer);
        // Assign to this object the value resulting from first clearing this
        // JsonArray and then inserting (in order) each 'Json' object in the
        // specified 'initializer' initializer list.  Return a reference to
        // `*this`.  If an exception is thrown, '*this' is left in a valid but
        // unspecified state.

    void assign(std::initializer_list<Json> initializer);
        // Assign to this object the value resulting from first clearing this
        // JsonArray and then inserting (in order) each 'Json' object in the
        // specified 'initializer' initializer list.  If an exception is
        // thrown, '*this' is left in a valid but unspecified state.
#endif

    Json& operator[](bsl::size_t index);
        // Return a reference providing modifiable access to the element at the
        // specified 'index' in this 'JsonArray'.  The behavior is undefined
        // unless 'index < size()'.

    template <class INPUT_ITERATOR>
    void assign(INPUT_ITERATOR first, INPUT_ITERATOR last);
        // Assign to this object the value resulting from first clearing this
        // 'JsonArray' and then inserting (in order) each 'Json' object in the
        // range starting at the specified 'first' element, and ending
        // immediately before the specified 'last' element.  If an exception is
        // thrown, '*this' is left in a valid but unspecified state.  Throw
        // 'bsl::length_error' if 'distance(first,last) > maxSize()'.  The
        // (template parameter) type 'INPUT_ITERATOR' shall meet the
        // requirements of an input iterator defined in the c++11 standard
        // [24.2.3] providing access to values of a type convertible to 'Json',
        // and 'Json' must be 'emplace-constructible' form '*i', where 'i' is a
        // dereferenceable iterator in the range '[first .. last)'.  The
        // behavior is undefined unless 'first' and 'last' refer to a range of
        // valid values where 'first' is at a position at or before 'last'.

    // BDE_VERIFY pragma: -FABC01

    Iterator begin();
        // Return an iterator providing modifiable access to the first element
        // in this 'JsonArray, or the past-the-end iterator if this 'JsonArray'
        // is empty.

    Iterator end();
        // Return the past-the-end iterator providing modifiable access to this
        // 'JsonArray'.

    Json& front();
        // Return a reference providing modifiable access to the first element
        // in this 'JsonArray'.  The behavior is undefined unless this
        // 'JsonArray' is not empty.

    Json& back();
        // Return a reference providing modifiable access to the last element
        // in this 'JsonArray'.  The behavior is undefined unless this
        // 'JsonArray' is not empty.

    // BDE_VERIFY pragma: +FABC01

    void clear();
        // Remove all elements from this 'JsonArray' making its size 0.  Note
        // that although this 'JsonArray' is empty after this method returns,
        // it preserves the same capacity it had before the method was called.

    Iterator erase(bsl::size_t index);
        // Remove from this 'JsonArray' the element at the specified 'index',
        // and return an iterator providing modifiable access to the element
        // immediately following the removed element, or the position returned
        // by the method 'end' if the removed element was the last in the
        // sequence.  The behavior is undefined unless 'index' is in the range
        // '[0 .. size())'.

    Iterator erase(ConstIterator position);
        // Remove from this 'JsonArray' the element at the specified
        // 'position', and return an iterator providing modifiable access to
        // the element immediately following the removed element, or the
        // position returned by the method 'end' if the removed element was the
        // last in the sequence.  The behavior is undefined unless 'position'
        // is an iterator in the range '[cbegin() .. cend())'.

    Iterator erase(ConstIterator first, ConstIterator last);
        // Remove from this 'JsonArray' the sequence of elements starting at
        // the specified 'first' position and ending before the specified
        // 'last' position, and return an iterator providing modifiable access
        // to the element immediately following the last removed element, or
        // the position returned by the method 'end' if the removed elements
        // were last in the sequence.  The behavior is undefined unless 'first'
        // is an iterator in the range '[cbegin() .. cend()]' (both endpoints
        // included) and 'last' is an iterator in the range '[first .. cend()]'
        // (both endpoints included).

    Iterator insert(bsl::size_t index, const Json& json);
        // Insert at the specified 'index' in this JsonArray a copy of the
        // specified 'json', and return an iterator referring to the newly
        // inserted element.  If an exception is thrown, '*this' is unaffected.
        // Throw 'bsl::length_error' if 'size() == maxSize()'.  The behavior is
        // undefined unless 'index' is in the range '[0 .. size()]'.

    Iterator insert(bsl::size_t index, bslmf::MovableRef<Json> json);
        // Insert at the specified 'index' in this JsonArray the specified
        // move-insertable 'json', and return an iterator referring to the
        // newly inserted element.  'json' is left in a valid but unspecified
        // state.  If an exception is thrown, 'this' is unaffected.  Throw
        // 'bsl::length_error' if 'size() == maxSize()'.  The behavior is
        // undefined unless 'index' is in the range '[0 .. size()]' (both
        // endpoints included).

    template <class INPUT_ITERATOR>
    Iterator insert(bsl::size_t    index,
                    INPUT_ITERATOR first,
                    INPUT_ITERATOR last);
        // Insert at the specified 'index' in this JsonArray the values in the
        // range starting at the specified 'first' element, and ending
        // immediately before the specified 'last' element.  Return an iterator
        // referring to the first newly inserted element.  If an exception is
        // thrown, '*this' is unaffected.  Throw 'bsl::length_error' if
        // 'size() + distance(first, last) > maxSize()'.  The (template
        // parameter) type 'INPUT_ITERATOR' shall meet the requirements of an
        // input iterator defined in the C++11 standard [24.2.3] providing
        // access to values of a type convertible to 'Json', and 'Json' must be
        // 'emplace-constructible' from '*i' into this 'JsonArray, where 'i' is
        // a dereferenceable iterator in the range '[first .. last)'.  The
        // behavior is undefined unless 'index' is in the range '[0 .. size()]'
        // (both endpoints included), and 'first' and 'last' refer to a range
        // of valid values where 'first' is at a position at or before 'last'.

    Iterator insert(ConstIterator position, const Json& json);
        // Insert at the specified 'position' in this 'JsonArray' a copy of the
        // specified 'json', and return an iterator referring to the newly
        // inserted element.  If an exception is thrown, '*this' is unaffected.
        // Throw 'bsl::length_error' if 'size() == maxSize()'.  The behavior is
        // undefined unless 'position' is an iterator in the range
        // '[begin() .. end()]' (both endpoints included).

    Iterator insert(ConstIterator position, bslmf::MovableRef<Json> json);
        // Insert at the specified 'position' in this 'JsonArray' the specified
        // move-insertable 'json', and return an iterator referring to the
        // newly inserted element.  'json' is left in a valid but unspecified
        // state.  If an exception is thrown, 'this' is unaffected.  Throw
        // 'bsl::length_error' if 'size() == maxSize()'.  The behavior is
        // undefined unless 'position' is an iterator in the range
        // '[begin() .. end()]' (both endpoints included).

    template <class INPUT_ITERATOR>
    Iterator insert(ConstIterator  position,
                    INPUT_ITERATOR first,
                    INPUT_ITERATOR last);
        // Insert at the specified 'position' in this 'JsonArray' the values in
        // the range starting at the specified 'first' element, and ending
        // immediately before the specified 'last' element.  Return an iterator
        // referring to the first newly inserted element.  If an exception is
        // thrown, '*this' is unaffected.  Throw 'bsl::length_error' if
        // 'size() + distance(first, last) > maxSize()'.  The (template
        // parameter) type 'INPUT_ITERATOR' shall meet the requirements of an
        // input iterator defined in the C++11 standard [24.2.3] providing
        // access to values of a type convertible to 'value_type', and
        // 'value_type' must be 'emplace-constructible' from '*i' into this
        // JsonArray, where 'i' is a dereferenceable iterator in the range
        // '[first .. last)'.  The behavior is undefined unless 'position' is
        // an iterator in the range '[begin() .. end()]' (both endpoints
        // included), and 'first' and 'last' refer to a range of valid values
        // where 'first' is at a position at or before 'last'.

    void popBack();
        // Erase the last element from this 'JsonArray'.  The behavior is
        // undefined if this JsonArray is empty.

    void pushBack(const Json& json);
        // Append to the end of this 'JsonArray' a copy of the specified
        // 'json'.  If an exception is thrown, '*this' is unaffected.  Throw
        // 'bsl::length_error' if 'size() == maxSize()'.

    void pushBack(bslmf::MovableRef<Json> json);
        // Append to the end of this 'JsonArray' the specified move-insertable
        // 'Json'.  'value' is left in a valid but unspecified state.  If an
        // exception is thrown, '*this' is unaffected.  Throw
        // 'bsl::length_error' if 'size() == maxSize()'.

    void resize(bsl::size_t count);
        // Change the size of this 'JsonArray' to the specified 'count'.  If
        // 'count < size()', the elements in the range '[count .. size())' are
        // erased, and this function does not throw.  If 'count > size()', the
        // (newly created) elements in the range '[size() .. count)' are
        // default-constructed 'Json' objects, and if an exception is thrown,
        // '*this' is unaffected.  Throw 'bsl::length_error' if
        // 'count > maxSize()'.

    void resize(bsl::size_t count, const Json& json);
        // Change the size of this JsonArray to the specified 'count',
        // inserting copies of the specified 'json' at the end if
        // 'count > size()'.  If 'count < size()', the elements in the range
        // '[count .. size())' are erased 'json' is ignored, and this method
        // does not throw.  If 'count > size()' and an exception is thrown,
        // '*this' is unaffected.  Throw 'bsl::length_error' if
        // 'count > maxSize()'.

    void swap(JsonArray& other);
        // Exchange the value of this object with that of the specified 'other'
        // object.  If an exception is thrown, both objects are left in valid
        // but unspecified states.  This operation guarantees O[1] complexity.
        // The behavior is undefined unless this object was created with the
        // same allocator as 'other'.

    // ACCESSORS
    const Json& operator[](bsl::size_t index) const;
        // Return a reference providing non-modifiable access to the element at
        // the specified 'index' in this 'JsonArray'.  The behavior is
        // undefined unless 'index < size()'.

    // BDE_VERIFY pragma: -FABC01

    ConstIterator begin() const BSLS_KEYWORD_NOEXCEPT;
    ConstIterator cbegin() const BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing non-modifiable access to the first
        // element in this 'JsonArray', and the past-the-end iterator if this
        // 'JsonArray' is empty.

    ConstIterator end() const BSLS_KEYWORD_NOEXCEPT;
    ConstIterator cend() const BSLS_KEYWORD_NOEXCEPT;
        // Return the past-the-end iterator providing non-modifiable access to
        // this 'JsonArray'.

    const Json& front() const;
        // Return a reference providing non-modifiable access to the first
        // element in this 'JsonArray'.  The behavior is undefined unless this
        // 'JsonArray' is not empty.

    const Json& back() const;
        // Return a reference providing non-modifiable access to the last
        // element in this 'JsonArray'.  The behavior is undefined unless this
        // 'JsonArray' is not empty.

    // BDE_VERIFY pragma: +FABC01

    bool empty() const;
        // Return 'true' if this 'JsonArray' has size 0, and 'false' otherwise.

    bsl::size_t size() const;
        // Return the number of elements in this 'JsonArray'.

                                  // Aspects

    bslma::Allocator *allocator() const BSLS_KEYWORD_NOEXCEPT;
        // Return the allocator used by this object to allocate memory.

    bsl::size_t maxSize() const BSLS_KEYWORD_NOEXCEPT;
        // Return a theoretical upper bound on the largest number of elements
        // that this 'JsonArray' could possibly hold.  Note that there is no
        // guarantee that the 'JsonArray' can successfully grow to the returned
        // size, or even close to that size without running out of resources.
        // Also note that requests to create a 'JsonArray' longer than this
        // number of elements are guaranteed to raise a 'bsl::length_error'
        // exception.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
        // Write the value of this object to the specified output 'stream' in a
        // human-readable format, and return a reference to 'stream'.
        // Optionally specify an initial indentation 'level', whose absolute
        // value is incremented recursively for nested objects.  If 'level' is
        // specified, optionally specify 'spacesPerLevel', whose absolute value
        // indicates the number of spaces per indentation level for this and
        // all of its nested objects.  If 'level' is negative, suppress
        // indentation of the first line.  If 'spacesPerLevel' is negative,
        // format the entire output on one line, suppressing all but the
        // initial indentation (as governed by 'level').  If 'stream' is not
        // valid on entry, this operation has no effect.  Note that this
        // human-readable format is not fully specified, and can change without
        // notice.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, const JsonArray& object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a reference to 'stream'.
    // If 'stream' is not valid on entry, this operation has no effect.  Note
    // that this human-readable format is not fully specified, can change
    // without notice, and is logically equivalent to:
    //..
    //  print(stream, 0, -1);
    //..

bool operator==(const JsonArray& lhs, const JsonArray& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'JsonArray' objects 'lhs' and 'rhs'
    // have the same value if they have the same number of elements, and each
    // element in the ordered sequence of elements of 'lhs' has the same value
    // as the corresponding element in the ordered sequence of elements of
    // 'rhs'.

bool operator!=(const JsonArray& lhs, const JsonArray& rhs);
    // Return 'false' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'true' otherwise.  Two 'JsonArray' objects 'lhs' and 'rhs'
    // have the same value if they have the same number of elements, and each
    // element in the ordered sequence of elements of 'lhs' has the same value
    // as the corresponding element in the ordered sequence of elements of
    // 'rhs'.

template <class HASHALG>
void hashAppend(HASHALG& hashAlg, const JsonArray& object);
    // Invoke the specified 'hashAlg' on the attributes of the specified
    // 'object'.

void swap(JsonArray& a, JsonArray& b);
    // Exchange the value of the specified 'a' 'JsonArray' with that of the
    // specified 'b' 'JsonArray'.  This function provides the no-throw
    // exception-safety guarantee.  This operation has O[1] complexity if 'a'
    // was created with the same allocator as 'b'; otherwise, it has O[n+m]
    // complexity, where n and m are the number of elements in 'a' and 'b',
    // respectively.

                              // ================
                              // class JsonObject
                              // ================

class JsonObject {
    // This type is designed to replicate much of the standard associative
    // container interface, eliding interfaces that are less relevant for a
    // non-generic container, like hasher and comparator access, emplacement,
    // nodes, capacity management, etc.

  private:
    // PRIVATE TYPES
    typedef bsl::unordered_map<bsl::string,
                               Json,
                               bdlb::TransparentHash,
                               bdlb::TransparentEqualTo>
        Container;

  public:
    // TYPES
    typedef Container::value_type      Member;
    typedef Container::const_iterator  ConstIterator;
    typedef Container::iterator        Iterator;
    typedef bsl::pair<Iterator, bool>  IteratorAndStatus;

    typedef Container::value_type      value_type;
    typedef Container::reference       reference;
    typedef Container::const_reference const_reference;

    typedef Container::iterator        iterator;
    typedef Container::const_iterator  const_iterator;

    typedef Container::difference_type difference_type;
    typedef Container::size_type       size_type;

  private:
    // DATA
    Container d_members;  // the underlying container of element mappings

    // FRIENDS
    friend bool operator==(const JsonObject&, const JsonObject&);
    friend bool operator!=(const JsonObject&, const JsonObject&);
    template <class HASHALG>
    friend void hashAppend(HASHALG&, const JsonObject&);
    friend void swap(JsonObject&, JsonObject&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(JsonObject, bslma::UsesBslmaAllocator);
    BSLMF_NESTED_TRAIT_DECLARATION(JsonObject, bslmf::IsBitwiseMoveable);
    BSLMF_NESTED_TRAIT_DECLARATION(JsonObject, bdlb::HasPrintMethod);

    // CREATORS
    JsonObject();
    explicit JsonObject(bslma::Allocator *basicAllocator);
        // Create an empty 'JsonObject'.  Optionally specify the
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is not
        // specified, the currently installed default allocator is used to
        // supply memory.

    JsonObject(const JsonObject&  original,
               bslma::Allocator  *basicAllocator = 0);
        // Create a 'JsonObject' having the same value as the specified
        // 'original'.  Optionally specify the 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is not specified, the currently
        // installed default allocator is used to supply memory.

    JsonObject(bslmf::MovableRef<JsonObject> original) BSLS_KEYWORD_NOEXCEPT;
        // Create a 'JsonObject' having the same value as the specified
        // 'original' object by moving (in constant time) the contents of
        // 'original' to the new 'JsonObject'.  The allocator associated with
        // 'original' is propagated for use in the newly-created 'JsonObject'.
        // 'original' is left in a valid but unspecified state.

    JsonObject(bslmf::MovableRef<JsonObject>  original,
               bslma::Allocator              *basicAllocator);
        // Create a 'JsonObject' having the same value as the specified
        // 'original'.  Use the specified 'basicAllocator' to supply memory.
        // If 'basicAllocator == original.allocator()' the value of 'original'
        // will be moved (in constant time) to the newly-created 'JsonObject',
        // and 'original' will be left in a valid but unspecified state.
        // Otherwise, 'original' is copied, and 'basicAllocator' used to supply
        // memory.

    template <class INPUT_ITERATOR>
    JsonObject(INPUT_ITERATOR    first,
               INPUT_ITERATOR    last,
               bslma::Allocator *basicAllocator = 0);
        // Create an empty 'JsonObject', and then create a 'Json' object for
        // each iterator in the range starting at the specified 'first'
        // iterator and ending immediately before the specified 'last'
        // iterator, by converting from the object referred to by each
        // iterator.  Insert into this 'JsonObject' each such object, ignoring
        // those having a key that appears earlier in the sequence.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is not supplied, the currently installed default
        // allocator is used to supply memory.  The (template parameter) type
        // 'INPUT_ITERATOR' shall meet the requirements of an input iterator
        // defined in the C++11 standard [24.2.3] providing access to values of
        // a type convertible to 'Member'.  The behavior is undefined unless
        // 'first' and 'last' refer to a sequence of valid values where 'first'
        // is at a position at or before 'last', and all keys of all 'Member'
        // objects inserted are valid UTF-8 (see 'bdlde::Utf8Util::isValid').

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    JsonObject(std::initializer_list<Member>  members,
    bslma::Allocator                         *basicAllocator = 0);  // IMPLICIT
        // Create an empty 'JsonObject', and then create a 'Json' object for
        // each in the range specified by 'members' argument, ignoring elements
        // having a key that appears earlier in the sequence.  Optionally
        // specify the 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is not specified, the currently installed default
        // allocator is used to supply memory.  The behavior is undefined
        // unless the keys of all 'Member' objects in 'members' are valid UTF-8
        // (see 'bdlde::Utf8Util::isValid').
#endif

    // MANIPULATORS
    JsonObject& operator=(const JsonObject& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    JsonObject& operator=(bslmf::MovableRef<JsonObject> rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  The
        // contents of 'rhs' are moved (in constant time) to this 'JsonObject'
        // if 'allocator() == rhs.allocator()'; otherwise, all elements in this
        // container are either destroyed or move-assigned to, and each
        // additional element in 'rhs', if any, is move-inserted into this
        // 'JsonObject'.  'rhs' is left in a valid but unspecified state.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    JsonObject& operator=(std::initializer_list<Member> members);
        // Assign to this object the value resulting from first clearing this
        // 'JsonObject' and then inserting (in order) each 'Member' object in
        // the specified 'members' initializer list.  Return a reference to
        // `*this`.  If an exception is thrown, '*this' is left in a valid but
        // unspecified state.
#endif

    Json& operator[](const bsl::string_view& key);
        // Return a reference providing modifiable access to the 'Json' object
        // associated with the specified 'key' in this 'JsonObject'; if this
        // 'JsonObject' does not already contain a 'Json' object associated
        // with 'key', first insert a new default-constructed 'Json' object
        // associated with 'key'.  The behavior is undefined unless 'key' is
        // valid UTF-8 (see 'bdlde::Utf8Util::isValid').

    // BDE_VERIFY pragma: -FABC01

    Iterator begin() BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing modifiable access to the first 'Member'
        // object in the sequence of 'Member' objects maintained by this
        // 'JsonObject', or the 'end' iterator if this 'JsonObject' is empty.

    Iterator end() BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing modifiable access to the past-the-end
        // position in the sequence of 'Member' objects maintained by this
        // 'JsonObject'.

    // BDE_VERIFY pragma: +FABC01

    void clear() BSLS_KEYWORD_NOEXCEPT;
        // Remove all entries from this 'JsonObject'.  Note that this
        // 'JsonObject' will be empty after calling this method, but allocated
        // memory may be retained for future use.

    bsl::size_t erase(const bsl::string_view& key);
        // Remove from this 'JsonObject' the 'Member' object having the
        // specified 'key', if it exists, and return 1; otherwise (there is no
        // object with a key equivalent to 'key' in this 'JsonObject') return 0
        // with no other effect.  This method invalidates only iterators and
        // references to the removed element and previously saved values of the
        // 'end()' iterator, and preserves the relative order of the elements
        // not removed.

    Iterator erase(Iterator position);
    Iterator erase(ConstIterator position);
        // Remove from this unordered map the 'value_type' object at the
        // specified 'position', and return an iterator referring to the
        // element immediately following the removed element, or to the
        // past-the-end position if the removed element was the last element in
        // the sequence of elements maintained by this unordered map.  This
        // method invalidates only iterators and references to the removed
        // element and previously saved values of the 'end()' iterator, and
        // preserves the relative order of the elements not removed.  The
        // behavior is undefined unless 'position' refers to a 'value_type'
        // object in this unordered map.

    Iterator find(const bsl::string_view& key);
        // Return an iterator providing modifiable access to the 'Member'
        // object in this 'JsonObject' with a key equivalent to the specified
        // 'key', if such an entry exists, and the past-the-end iterator
        // ('end') otherwise.

    bsl::pair<Iterator, bool> insert(const Member& member);
        // Insert the specified 'member' into this 'JsonObject' if the key (the
        // 'first' element) of the object referred to by 'value' does not
        // already exist in this 'JsonObject'; otherwise, this method has no
        // effect.  Return a 'pair' whose 'first' member is an iterator
        // referring to the (possibly newly inserted) 'value_type' object in
        // this 'JsonObject' whose key is equivalent to that of the object to
        // be inserted, and whose 'second' member is 'true' if a new value was
        // inserted, and 'false' if a value having an equivalent key was
        // already present.  The behavior is undefined unless 'member.first' is
        // valid UTF-8 (see 'bdlde::Utf8Util::isValid').

    bsl::pair<Iterator, bool> insert(bslmf::MovableRef<Member> member);
        // Insert the specified 'member' into this 'JsonObject' if the key (the
        // 'first' element) of the object referred to by 'member' does not
        // already exist in this 'JsonObject'; otherwise, this method has no
        // effect.  Return a 'pair' whose 'first' member is an iterator
        // referring to the (possibly newly inserted) 'member' object in this
        // 'JsonObject' whose key is the equivalent to that of the object to be
        // inserted, and whose 'second' member is 'true' if a new value was
        // inserted, and 'false' otherwise.  The behavior is undefined unless
        // 'member.first' is valid UTF-8 (see 'bdlde::Utf8Util::isValid').

    template <class INPUT_ITERATOR>
    typename bsl::enable_if<
        !bsl::is_convertible<INPUT_ITERATOR, bsl::string_view>::value,
        void>::type
    insert(INPUT_ITERATOR first, INPUT_ITERATOR last);
        // Create a 'member' object for each iterator in the range starting at
        // the specified 'first' iterator and ending immediately before the
        // specified 'last' iterator, by converting from the object referred to
        // by each iterator.  Insert into this 'JsonObject' each such object
        // whose key is not already contained.  The (template parameter) type
        // 'INPUT_ITERATOR' shall meet the requirements of an input iterator
        // defined in the C++11 standard [24.2.3] providing access to values of
        // a type convertible to 'Member'.  The behavior is undefined unless
        // 'first' and 'last' refer to a sequence of value values where 'first'
        // is at a position at or before 'last'.  The behavior is undefined
        // unless the keys of all 'Member' objects inserted are valid UTF-8
        // (see 'bdlde::Utf8Util::isValid').

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    void insert(std::initializer_list<Member> members);
        // Create a 'Member' object for each element in the specified
        // 'members'.  Insert into this 'JsonObject' each such object whose key
        // is not already contained.  The behavior is undefined unless the keys
        // of all 'Member' objects inserted are valid UTF-8 (see
        // 'bdlde::Utf8Util::isValid').
#endif

    template <class VALUE>
    bsl::pair<Iterator, bool> insert(
                               const bsl::string_view&                  key,
                               BSLS_COMPILERFEATURES_FORWARD_REF(VALUE) value);
        // Insert into this 'JsonObject' a 'Member' constructed from the
        // specified 'key' and the specified 'value', respectively, if 'key'
        // does not already exist in this 'JsonObject'; otherwise, this method
        // has no effect.  Return a 'pair' whose 'first' member is an iterator
        // referring to the (possibly newly inserted) 'Member' object in this
        // 'JsonObject' whose key is the equivalent to that of the object to be
        // inserted, and whose 'second' member is 'true' if a new value was
        // inserted, and 'false' otherwise.  The behavior is undefined unless
        // 'key' is valid UTF-8 (see 'bdlde::Utf8Util::isValid').

    void swap(JsonObject& other);
        // Exchange the value of this object with that of the specified 'other'
        // object.  If an exception is thrown, both objects are left in valid
        // but unspecified states.  This operation guarantees O[1] complexity.
        // The behavior is undefined unless this object was created with the
        // same allocator as 'other'.

    // ACCESSORS
    const Json& operator[](const bsl::string_view& key) const;
        // Return a reference providing non-modifiable access to the 'Json'
        // object associated with the specified 'key' in this 'JsonObject'.
        // The behavior is undefined unless 'key' is valid UTF-8 (see
        // 'bdlde::Utf8Util::isValid') and this 'JsonObject' already contains a
        // 'Json' object associated with 'key'.

    // BDE_VERIFY pragma: -FABC01

    ConstIterator begin() const BSLS_KEYWORD_NOEXCEPT;
    ConstIterator cbegin() const BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing non-modifiable access to the first
        // 'Member' object in the sequence of 'Member' objects maintained by
        // this 'JsonObject', or the 'end' iterator if this 'JsonObject' is
        // empty.

    ConstIterator end() const BSLS_KEYWORD_NOEXCEPT;
    ConstIterator cend() const BSLS_KEYWORD_NOEXCEPT;
        // Return an iterator providing non-modifiable access to the
        // past-the-end position in the sequence of 'Member' objects maintained
        // by this 'JsonObject'.

    // BDE_VERIFY pragma: +FABC01

    bool contains(const bsl::string_view& key) const;
        // Return 'true' if there is a 'Member' object in this 'JsonObject'
        // with a key equivalent to the specified 'key', and return 'false'
        // otherwise.

    bool empty() const BSLS_KEYWORD_NOEXCEPT;
        // Return 'true' if this 'JsonObject' contains no elements, and 'false'
        // otherwise.

    ConstIterator find(const bsl::string_view& key) const;
        // Return an iterator providing non-modifiable access to the 'Member'
        // object in this 'JsonObject' with a key equivalent to the specified
        // 'key', if such an entry exists, and the past-the-end iterator
        // ('end') otherwise.

    bsl::size_t size() const BSLS_KEYWORD_NOEXCEPT;
        // Return the number of elements in this 'JsonObject'.

                                  // Aspects

    bslma::Allocator *allocator() const BSLS_KEYWORD_NOEXCEPT;
        // Return the allocator used by this object to allocate memory.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
        // Write the value of this object to the specified output 'stream' in a
        // human-readable format, and return a reference to 'stream'.
        // Optionally specify an initial indentation 'level', whose absolute
        // value is incremented recursively for nested objects.  If 'level' is
        // specified, optionally specify 'spacesPerLevel', whose absolute value
        // indicates the number of spaces per indentation level for this and
        // all of its nested objects.  If 'level' is negative, suppress
        // indentation of the first line.  If 'spacesPerLevel' is negative,
        // format the entire output on one line, suppressing all but the
        // initial indentation (as governed by 'level').  If 'stream' is not
        // valid on entry, this operation has no effect.  Note that this
        // human-readable format is not fully specified, and can change without
        // notice.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, const JsonObject& object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a reference to 'stream'.
    // If 'stream' is not valid on entry, this operation has no effect.  Note
    // that this human-readable format is not fully specified, can change
    // without notice, and is logically equivalent to:
    //..
    //  print(stream, 0, -1);
    //..

bool operator==(const JsonObject& lhs, const JsonObject& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'JsonObject' objects have the same
    // value if they have the same number of 'Member' objects, and for each
    // 'Member' object that is contained in 'lhs' there is a key-value pair
    // contained in 'rhs' having the same value, and vice versa.

bool operator!=(const JsonObject& lhs, const JsonObject& rhs);
    // Return 'false' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'true' otherwise.  Two 'JsonObject' objects have the same
    // value if they have the same number of 'Member' objects, and for each
    // 'Member' object that is contained in 'lhs' there is a key-value pair
    // contained in 'rhs' having the same value, and vice versa.

template <class HASHALG>
void hashAppend(HASHALG& hashAlg, const JsonObject& object);
    // Invoke the specified 'hashAlg' on the attributes of the specified
    // 'object'.

void swap(JsonObject& a, JsonObject& b);
    // Exchange the value of the specified 'a' 'JsonObject' with that of the
    // specified 'b' 'JsonObject'.  This function provides the no-throw
    // exception-safety guarantee.  This operation has O[1] complexity if 'a'
    // was created with the same allocator as 'b'; otherwise, it has O[n+m]
    // complexity, where n and m are the number of elements in 'a' and 'b',
    // respectively.

                                 // ==========
                                 // class Json
                                 // ==========

class Json {
    // This type is designed to be a thin wrapper around a variant of the
    // possible JSON types, using a BDE-style variant interface.

    // PRIVATE TYPES
    typedef bdlb::
        Variant<JsonObject, JsonArray, bsl::string, JsonNumber, bool, JsonNull>
            Value;

    BSLMF_ASSERT(0 == JsonType::e_OBJECT);
    BSLMF_ASSERT(1 == JsonType::e_ARRAY);
    BSLMF_ASSERT(2 == JsonType::e_STRING);
    BSLMF_ASSERT(3 == JsonType::e_NUMBER);
    BSLMF_ASSERT(4 == JsonType::e_BOOLEAN);
    BSLMF_ASSERT(5 == JsonType::e_NULL);

  private:
    // DATA
    Value d_value;  // the underlying variant value

    // FRIENDS
    friend bool operator==(const Json&, const Json&);
    friend bool operator!=(const Json&, const Json&);
    template <class HASHALG>
    friend void hashAppend(HASHALG&, const Json&);
    friend void swap(Json&, Json&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Json, bslma::UsesBslmaAllocator);
    BSLMF_NESTED_TRAIT_DECLARATION(Json, bslmf::IsBitwiseMoveable);
    BSLMF_NESTED_TRAIT_DECLARATION(Json, bdlb::HasPrintMethod);

    // CREATORS
    Json();
    explicit Json(bslma::Allocator *basicAllocator);
        // Create a 'Json' object having the type (and the singleton value) of
        // 'JsonNull'.  Optionally specify the 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is not specified, the currently
        // installed default allocator is used to supply memory.

    Json(const Json& original, bslma::Allocator *basicAllocator = 0);
        // Create a 'Json' object having the same value as the specified
        // 'original'.  Optionally specify the 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is not specified, the currently
        // installed default allocator is used to supply memory.

    Json(bslmf::MovableRef<Json> original);
        // Create a 'Json' object having the same value as the specified
        // 'original' object by moving (in constant time) the contents of
        // 'original' to the new 'JsonObject'.  The allocator associated with
        // 'original' is propagated for use in the newly-created 'Json' object.
        // 'original' is left in a valid but unspecified state.

    Json(bslmf::MovableRef<Json> original, bslma::Allocator *basicAllocator);
        // Create a 'Json' object having the same value as the specified
        // 'original'.  Use the specified 'basicAllocator' to supply memory.

    explicit Json(const JsonArray&  array,
                  bslma::Allocator *basicAllocator = 0);
        // Create a 'Json' object having the type 'JsonArray' and the same
        // value as the specified 'array'.  Optionally specify the
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is not
        // specified, the currently installed default allocator is used to
        // supply memory.

    explicit Json(bslmf::MovableRef<JsonArray>  array,
                  bslma::Allocator             *basicAllocator = 0);
        // Create a 'Json' object having the type 'JsonArray' and the same
        // value as the specified 'array' object by moving (in constant time)
        // the contents of 'array' to the new 'Json' object.  Optionally
        // specify the 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is not specified, the allocator associated with
        // 'array' is propagated for use in the newly-created 'Json' object.
        // 'array' is left in a valid but unspecified state.

    explicit Json(bool boolean, bslma::Allocator *basicAllocator = 0);
        // Create a 'Json' object having the type 'bool' and the same value as
        // the specified 'boolean'.  Optionally specify the 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is not specified, the
        // currently installed default allocator is used to supply memory.

    explicit Json(const JsonNull& null, bslma::Allocator *basicAllocator = 0);
        // Create a 'Json' object having the type 'JsonNull' and the same value
        // as the specified 'null'.  Optionally specify the 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is not specified, the
        // currently installed default allocator is used to supply memory.

    explicit Json(float number, bslma::Allocator *basicAllocator = 0);
    explicit Json(double number, bslma::Allocator *basicAllocator = 0);
    explicit Json(bdldfp::Decimal64  number,
                  bslma::Allocator  *basicAllocator = 0);
    explicit Json(int number, bslma::Allocator *basicAllocator = 0);
    explicit Json(unsigned int number, bslma::Allocator *basicAllocator = 0);
    explicit Json(bsls::Types::Int64  number,
                  bslma::Allocator   *basicAllocator = 0);
    explicit Json(bsls::Types::Uint64  number,
                  bslma::Allocator    *basicAllocator = 0);
    explicit Json(const JsonNumber&  number,
                  bslma::Allocator  *basicAllocator = 0);
        // Create a 'Json' object having the type 'JsonNumber' and the same
        // value as the specified 'number'.  Optionally specify the
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is not
        // specified, the currently installed default allocator is used to
        // supply memory.

    explicit Json(bslmf::MovableRef<JsonNumber>  number,
                  bslma::Allocator              *basicAllocator = 0);
        // Create a 'Json' object having the type 'JsonNumber' and the same
        // value as the specified 'number' object by moving (in constant time)
        // the contents of 'number' to the new 'Json' object.  Optionally
        // specify the 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is not specified, the allocator associated with
        // 'number' is propagated for use in the newly-created 'Json' object.
        // 'number' is left in a valid but unspecified state.

    explicit Json(const JsonObject&  object,
                  bslma::Allocator  *basicAllocator = 0);
        // Create a 'Json' object having the type 'JsonObject' and the same
        // value as the specified 'object'.  Optionally specify the
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is not
        // specified, the currently installed default allocator is used to
        // supply memory.

    explicit Json(bslmf::MovableRef<JsonObject>  object,
                  bslma::Allocator              *basicAllocator = 0);
        // Create a 'Json' object having the type 'JsonObject' and the same
        // value as the specified 'object' by moving (in constant time) the
        // contents of 'object' to the new 'Json' object.  Optionally specify
        // the 'basicAllocator' used to supply memory.  If 'basicAllocator' is
        // not specified, the allocator associated with 'object' is propagated
        // for use in the newly-created 'Json' object.  'object' is left in a
        // valid but unspecified state.

    // BDE_VERIFY pragma: -FD06 'string' and 'bsl::string' are too similar
    // BDE_VERIFY pragma: -FD07 'string' and 'bsl::string' are too similar

    explicit Json(const char *string, bslma::Allocator *basicAllocator = 0);
    explicit Json(const bsl::string_view&  string,
                  bslma::Allocator        *basicAllocator = 0);
        // Create a 'Json' object having the type 'bsl::string' and the same
        // value as the specified 'string'.  Optionally specify the
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is not
        // specified, the currently installed default allocator is used to
        // supply memory.  The behavior is undefined unless 'string' is valid
        // UTF-8 (see 'bdlde::Utf8Util::isValid').

    // BDE_VERIFY pragma: -IND01 DEDUCE macro confuses bde_verify

    template <class STRING_TYPE>
    explicit Json(
               BSLMF_MOVABLEREF_DEDUCE(STRING_TYPE) string,
               bslma::Allocator                    *basicAllocator = 0,
               typename bsl::enable_if<
                   bsl::is_same<STRING_TYPE, bsl::string>::value>::type * = 0);
        // Create a 'Json' object having the type 'bsl::string' and the same
        // value as the specified 'string' by moving (in constant time) the
        // contents of 'string' to the new 'Json' object.  Optionally specify
        // the 'basicAllocator' used to supply memory.  If 'basicAllocator' is
        // not specified, the allocator associated with 'string' is propagated
        // for use in the newly-created 'Json' object.  'string' is left in a
        // valid but unspecified state.  This function does not participate in
        // overload resolution unless the specified 'STRING_TYPE' is
        // 'bsl::string'.  The behavior is undefined unless 'string' is valid
        // UTF-8 (see 'bdlde::Utf8Util::isValid').

    // BDE_VERIFY pragma: +IND01 DEDUCE macro confuses bde_verify
    // BDE_VERIFY pragma: +FD06 'string' and 'bsl::string' are too similar
    // BDE_VERIFY pragma: +FD07 'string' and 'bsl::string' are too similar

    // MANIPULATORS
    Json& operator=(const Json& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  The
        // value currently held by this variant is destroyed if that value's
        // type is not the same as the type held by the 'rhs' object.

    Json& operator=(bslmf::MovableRef<Json> rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  The
        // contents of 'rhs' are moved (in constant time) to this 'Json' object
        // if 'allocator() == rhs.allocator()'; otherwise they are copied.
        // 'rhs' is left in a valid but unspecified state.

    Json& operator=(float rhs);
    Json& operator=(double rhs);
    Json& operator=(bdldfp::Decimal64 rhs);
    Json& operator=(int rhs);
    Json& operator=(unsigned int rhs);
    Json& operator=(bsls::Types::Int64 rhs);
    Json& operator=(bsls::Types::Uint64 rhs);
    Json& operator=(const JsonNumber& rhs);
        // Assign to this object a value of type 'JsonNumber' initialized from
        // the specified 'rhs', and return a reference providing modifiable
        // access to this object.  The value currently held by this object (if
        // any) is destroyed if that value's type is not 'JsonNumber'.

    Json& operator=(bslmf::MovableRef<JsonNumber> rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  The
        // contents of 'rhs' are moved (in constant time) to this 'Json' object
        // if 'allocator() == rhs.allocator()'; otherwise, they are copied.
        // 'rhs' is left in a valid but unspecified state.  The value currently
        // held by this object (if any) is destroyed if that value's type is
        // not 'JsonNumber'.

    Json& operator=(const char *rhs);
    Json& operator=(const bsl::string_view& rhs);
        // Assign to this object a value of type 'bsl::string' initialized from
        // the specified 'rhs', and return a reference providing modifiable
        // access to this object.  The value currently held by this object (if
        // any) is destroyed if that value's type is not 'bsl::string'.  The
        // behavior is undefined unless 'rhs' is valid UTF-8 (see
        // 'bdlde::Utf8Util::isValid').

    template <class STRING_TYPE>
    typename bsl::enable_if<bsl::is_same<STRING_TYPE, bsl::string>::value,
                            Json>::type&
    operator=(BSLMF_MOVABLEREF_DEDUCE(STRING_TYPE) rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  The
        // contents of 'rhs' are moved (in constant time) to this 'Json' object
        // if 'allocator() == rhs.allocator()'; otherwise, they are copied.
        // 'rhs' is left in a valid but unspecified state.  The value currently
        // held by this object (if any) is destroyed if that value's type is
        // not 'bsl::string'.  This function does not participate in overload
        // resolution unless the specified 'STRING_TYPE' is 'bsl::string'.  The
        // behavior is undefined unless 'rhs' is valid UTF-8 (see
        // 'bdlde::Utf8Util::isValid').

    Json& operator=(bool rhs);
        // Assign to this object a value of type 'bool' initialized from the
        // specified 'rhs', and return a reference providing modifiable access
        // to this object.  The value currently held by this object (if any) is
        // destroyed if that value's type is not 'bsl::string'.

    Json& operator=(const JsonObject& rhs);
        // Assign to this object a value of type 'JsonObject' initialized from
        // the specified 'rhs', and return a reference providing modifiable
        // access to this object.  The value currently held by this object (if
        // any) is destroyed if that value's type is not 'JsonObject'.

    Json& operator=(bslmf::MovableRef<JsonObject> rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  The
        // contents of 'rhs' are moved (in constant time) to this 'Json' object
        // if 'allocator() == rhs.allocator()'; otherwise, they are copied.
        // 'rhs' is left in a valid but unspecified state.  The value currently
        // held by this object (if any) is destroyed if that value's type is
        // not 'JsonObject'.

    Json& operator=(const JsonArray& rhs);
        // Assign to this object a value of type 'JsonArray' initialized from
        // the specified 'rhs', and return a reference providing modifiable
        // access to this object.  The value currently held by this object (if
        // any) is destroyed if that value's type is not 'JsonArray'.

    Json& operator=(bslmf::MovableRef<JsonArray> rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  The
        // contents of 'rhs' are moved (in constant time) to this 'Json' object
        // if 'allocator() == rhs.allocator()'; otherwise, they are copied.
        // 'rhs' is left in a valid but unspecified state.  The value currently
        // held by this object (if any) is destroyed if that value's type is
        // not 'JsonArray'.

    Json& operator=(const JsonNull&);
        // Assign to this object the singleton value of type 'JsonNull', and
        // return a reference providing modifiable access to this object.  The
        // value currently held by this object (if any) is destroyed if that
        // value's type is not 'JsonNull'.

    Json& operator=(bslmf::MovableRef<JsonNull>);
        // Assign to this object the singleton value of type 'JsonNull', and
        // return a reference providing modifiable access to this object.
        // 'rhs' is left in a valid but unspecified state.  The value currently
        // held by this object (if any) is destroyed if that value's type is
        // not 'JsonNull'.

    JsonArray& makeArray();
    JsonArray& makeArray(const JsonArray& array);
    JsonArray& makeArray(bslmf::MovableRef<JsonArray> array);
        // Create an instance of type 'JsonArray' in this object, using the
        // allocator currently held by this object to supply memory, and return
        // a reference providing modifiable access to the created instance.
        // Optionally specify 'array' to initialize the 'JsonArray' created.
        // This method first destroys the current value held by this object
        // (even if the type currently held is 'JsonArray').

    bool& makeBoolean();
    bool& makeBoolean(bool boolean);
        // Create an instance of type 'bool' in this object and return a
        // reference providing modifiable access to the created instance.
        // Optionally specify 'boolean' to initialize the 'bool' created.  This
        // method first destroys the current value held by this object.

    void makeNull();
        // Create an instance of type 'JsonNull' in this object.  This method
        // first destroys the current value held by this object.

    JsonNumber& makeNumber();
    JsonNumber& makeNumber(const JsonNumber& number);
    JsonNumber& makeNumber(bslmf::MovableRef<JsonNumber> number);
        // Create an instance of type 'JsonNumber' in this object, using the
        // allocator currently held by this object to supply memory, and return
        // a reference providing modifiable access to the created instance.
        // Optionally specify 'number' to initialize the 'JsonNumber' created.
        // This method first destroys the current value held by this object
        // (even if the type currently held is 'JsonNumber').

    JsonObject& makeObject();
    JsonObject& makeObject(const JsonObject& object);
    JsonObject& makeObject(bslmf::MovableRef<JsonObject> object);
        // Create an instance of type 'JsonObject' in this object, using the
        // allocator currently held by this object to supply memory, and return
        // a reference providing modifiable access to the created instance.
        // Optionally specify 'object' to initialize the 'JsonObject' created.
        // This method first destroys the current value held by this object
        // (even if the type currently held is 'JsonObject').

    // BDE_VERIFY pragma: -FD06 'string' and 'bsl::string' are too similar
    // BDE_VERIFY pragma: -FD07 'string' and 'bsl::string' are too similar

    void makeString(const char *string);
    void makeString(const bsl::string_view& string);
        // Create an instance of type 'bsl::string' in this object, using the
        // allocator currently held by this object to supply memory.
        // Optionally specify 'string' to initialize the 'bsl::string' created.
        // This method first destroys the current value held by this object
        // (even if the type currently held is 'bsl::string').  The behavior is
        // undefined unless 'string' is valid UTF-8 (see
        // 'bdlde::Utf8Util::isValid').

    template <class STRING_TYPE>
    typename bsl::enable_if<
        bsl::is_same<STRING_TYPE, bsl::string>::value>::type
        makeString(BSLMF_MOVABLEREF_DEDUCE(STRING_TYPE) string);
        // Create an instance of type 'bsl::string' in this object, using the
        // allocator currently held by this object to supply memory.
        // Optionally specify 'string' to initialize the 'bsl::string' created.
        // This method first destroys the current value held by this object
        // (even if the type currently held is 'bsl::string').  This function
        // does not participate in overload resolution unless the specified
        // 'STRING_TYPE' is 'bsl::string'.  The behavior is undefined unless
        // 'string' is valid UTF-8 (see 'bdlde::Utf8Util::isValid').

    // BDE_VERIFY pragma: +FD06 'string' and 'bsl::string' are too similar
    // BDE_VERIFY pragma: +FD07 'string' and 'bsl::string' are too similar

    void swap(Json& other);
        // Exchange the value of this with that of the specified 'other'.  If
        // an exception is thrown, both objects are left in valid but
        // unspecified states.  This operation guarantees O[1] complexity.  The
        // behavior is undefined unless this object was created with the same
        // allocator as 'other'.

    JsonArray& theArray();
        // Return a reference providing modifiable access to the value of type
        // 'JsonArray' held by this object.  The behavior is undefined unless
        // 'isArray()' returns true.

    bool& theBoolean();
        // Return a reference providing modifiable access to the value of type
        // 'boolean' held by this object.  The behavior is undefined unless
        // 'isBoolean()' returns true.

    JsonNull& theNull();
        // Return a reference providing modifiable access to the value of type
        // 'JsonNull' held by this object.  The behavior is undefined unless
        // 'isNull()' returns true.

    JsonNumber& theNumber();
        // Return a reference providing modifiable access to the value of type
        // 'JsonNumber' held by this object.  The behavior is undefined unless
        // 'isNumber()' returns true.

    JsonObject& theObject();
        // Return a reference providing modifiable access to the value of type
        // 'JsonObject' held by this object.  The behavior is undefined unless
        // 'isObject()' returns true.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT)
    explicit operator JsonArray &();
        // Return a reference providing modifiable access to the value of type
        // 'JsonArray' held by this object.  The behavior is undefined unless
        // 'isArray()' returns true.

    explicit operator bool &();
        // Return a reference providing modifiable access to the value of type
        // 'boolean' held by this object.  The behavior is undefined unless
        // 'isBoolean()' returns true.

    explicit operator JsonNull &();
        // Return a reference providing modifiable access to the value of type
        // 'JsonNull' held by this object.  The behavior is undefined unless
        // 'isNull()' returns true.

    explicit operator JsonNumber &();
        // Return a reference providing modifiable access to the value of type
        // 'JsonNumber' held by this object.  The behavior is undefined unless
        // 'isNumber()' returns true.

    explicit operator JsonObject &();
        // Return a reference providing modifiable access to the value of type
        // 'JsonObject' held by this object.  The behavior is undefined unless
        // 'isObject()' returns true.
#endif

    Json& operator[](const bsl::string_view& key);
        // Return a reference providing modifiable access to the 'Json' object
        // associated with the specified 'key' in the 'JsonObject' held by this
        // object; if the 'JsonObject' does not already contain a 'Json' object
        // associated with 'key', first insert a new default-constructed 'Json'
        // object associated with 'key'.  The behavior is undefined unless
        // 'isObject()' returns true.

    Json& operator[](bsl::size_t index);
        // Return a reference providing modifiable access to the element at the
        // specified 'index' in the 'JsonArray' held by this object.  The
        // behavior is undefined unless 'isArray()' returns true and
        // 'index < theArray().size()'.

    // ACCESSORS
    // BDE_VERIFY pragma: -FABC01
    int asInt(int *result) const;
    int asInt64(bsls::Types::Int64 *result) const;
    int asUint(unsigned int *result) const;
    int asUint64(bsls::Types::Uint64 *result) const;
        // Load into the specified 'result' the integer value of the value of
        // type 'JsonNumber' held by this object.  Return 0 on success,
        // 'JsonNumber::k_OVERFLOW' if 'value' is larger than can be
        // represented by 'result', 'JsonNumber::k_UNDERFLOW' if 'value' is
        // smaller than can be represented by 'result', and
        // 'JsonNumber::k_NOT_INTEGRAL' if 'value' is not an integral number
        // (i.e., there is a fractional part).  For underflow, 'result' will be
        // loaded with the minimum representable value, for overflow, 'result'
        // will be loaded with the maximum representable value, for
        // non-integral values 'result' will be loaded with the integer part of
        // 'value' (truncating the value to the nearest integer).  If the
        // result is not an integer and also either overflows or underflows, it
        // is treated as an overflow or underflow (respectively).  Note that
        // this operation returns a status value (unlike similar floating point
        // conversions) because typically it is an error if a conversion to an
        // integer results in an inexact value.  The behavior is undefined
        // unless 'isNumber()' returns true.

    float             asFloat() const;
    double            asDouble() const;
    bdldfp::Decimal64 asDecimal64() const;
        // Return the closest floating point representation to the value of the
        // type 'JsonNumber' held by this object.  If this number is outside
        // the representable range, return +INF or -INF (as appropriate).  The
        // behavior is undefined unless 'isNumber()' returns true.
    // BDE_VERIFY pragma: +FABC01

    int asDecimal64Exact(bdldfp::Decimal64 *result) const;
        // Load the specified 'result' with the closest floating point
        // representation to the value of type 'JsonNumber' held by this
        // object, even if a non-zero status is returned.  Return 0 if this
        // number can be represented exactly, and return
        // 'JsonNumber::k_INEXACT' if 'value' cannot be represented exactly.
        // If this number is outside the representable range, load 'result'
        // with +INF or -INF (as appropriate).  A number can be represented
        // exactly as a 'Decimal64' if, for the significand and exponent,
        // 'abs(significand) <= 9,999,999,999,999,999' and '-398 <= exponent <=
        // 369'.  The behavior is undefined unless 'isNumber()' returns true;

    bool isArray() const;
        // Return true if the value held by this object is of type 'JsonArray',
        // and false otherwise.

    bool isBoolean() const;
        // Return true if the value held by this object is of type 'bool', and
        // false otherwise.

    bool isNull() const;
        // Return true if the value held by this object is of type 'JsonNull',
        // and false otherwise.

    bool isNumber() const;
        // Return true if the value held by this object is of type
        // 'JsonNumber', and false otherwise.

    bool isObject() const;
        // Return true if the value held by this object is of type
        // 'JsonObject', and false otherwise.

    bool isString() const;
        // Return true if the value held by this object is of type
        // 'bsl::string', and false otherwise.

    const JsonArray& theArray() const;
        // Return a reference providing non-modifiable access to the value of
        // type 'JsonArray' held by this object.  The behavior is undefined
        // unless 'isArray()' returns true.

    const bool& theBoolean() const;
        // Return a reference providing non-modifiable access to the value of
        // type 'boolean' held by this object.  The behavior is undefined
        // unless 'isBoolean()' returns true.

    const JsonNull& theNull() const;
        // Return a reference providing non-modifiable access to the value of
        // type 'JsonNull' held by this object.  The behavior is undefined
        // unless 'isNull()' returns true.

    const JsonNumber& theNumber() const;
        // Return a reference providing non-modifiable access to the value of
        // type 'JsonNumber' held by this object.  The behavior is undefined
        // unless 'isNumber()' returns true.

    const JsonObject& theObject() const;
        // Return a reference providing non-modifiable access to the value of
        // type 'JsonObject' held by this object.  The behavior is undefined
        // unless 'isObject()' returns true.

    const bsl::string& theString() const;
        // Return a reference providing non-modifiable access to the value of
        // type 'JsonString' held by this object.  The behavior is undefined
        // unless 'isString()' returns true.

    JsonType::Enum type() const BSLS_KEYWORD_NOEXCEPT;
        // Return the type of this 'Json' value.

    const Json& operator[](const bsl::string_view& key) const;
        // Return a reference providing non-modifiable access to the 'Json'
        // object associated with the specified 'key' in this 'JsonObject'.
        // The behavior is undefined unless 'key' is valid UTF-8 (see
        // 'bdlde::Utf8Util::isValid') and this 'JsonObject' already contains a
        // 'Json' object associated with 'key'.

    const Json& operator[](bsl::size_t index) const;
        // Return a reference providing non-modifiable access to the element at
        // the specified 'index' in the 'JsonArray' held by this object.  The
        // behavior is undefined unless 'isArray()' returns true and
        // 'index < theArray().size()'.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT)
    explicit operator const JsonArray &() const;
        // Return a reference providing non-modifiable access to the value of
        // type 'JsonArray' held by this object.  The behavior is undefined
        // unless 'isArray()' returns true.

    explicit operator const bool &() const;
        // Return a reference providing non-modifiable access to the value of
        // type 'boolean' held by this object.  The behavior is undefined
        // unless 'isBoolean()' returns true.

    explicit operator const JsonNull &() const;
        // Return a reference providing non-modifiable access to the value of
        // type 'JsonNull' held by this object.  The behavior is undefined
        // unless 'isNull()' returns true.

    explicit operator const JsonNumber &() const;
        // Return a reference providing non-modifiable access to the value of
        // type 'JsonNumber' held by this object.  The behavior is undefined
        // unless 'isNumber()' returns true.

    explicit operator const JsonObject &() const;
        // Return a reference providing non-modifiable access to the value of
        // type 'JsonObject' held by this object.  The behavior is undefined
        // unless 'isObject()' returns true.

    explicit operator const bsl::string &() const;
        // Return a reference providing non-modifiable access to the value of
        // type 'JsonString' held by this object.  The behavior is undefined
        // unless 'isString()' returns true.
#endif

    bsl::size_t size() const;
        // Return the number of elements in the 'JsonObject' or 'JsonArray'
        // held by this object.  The behavior is undefined unless
        // 'isArray() || isObject()' evaluates to true.

                                  // Aspects

    bslma::Allocator *allocator() const BSLS_KEYWORD_NOEXCEPT;
        // Return the allocator used by this object to allocate memory.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
        // Write the value of this object to the specified output 'stream' in a
        // human-readable format, and return a reference to 'stream'.
        // Optionally specify an initial indentation 'level', whose absolute
        // value is incremented recursively for nested objects.  If 'level' is
        // specified, optionally specify 'spacesPerLevel', whose absolute value
        // indicates the number of spaces per indentation level for this and
        // all of its nested objects.  If 'level' is negative, suppress
        // indentation of the first line.  If 'spacesPerLevel' is negative,
        // format the entire output on one line, suppressing all but the
        // initial indentation (as governed by 'level').  If 'stream' is not
        // valid on entry, this operation has no effect.  Note that this
        // human-readable format is not fully specified, and can change without
        // notice.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, const Json& object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a reference to 'stream'.
    // If 'stream' is not valid on entry, this operation has no effect.  Note
    // that this human-readable format is not fully specified, can change
    // without notice, and is logically equivalent to:
    //..
    //  print(stream, 0, -1);
    //..

bool operator==(const Json& lhs, const Json& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'Json' objects 'lhs' and 'rhs' have
    // the same value if they hold objects of the same type, and those objects
    // have the same value.

bool operator!=(const Json& lhs, const Json& rhs);
    // Return 'false' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'true' otherwise.  Two 'Json' objects 'lhs' and 'rhs' have
    // the same value if they hold objects of the same type, and those objects
    // have the same value.

template <class HASHALG>
void hashAppend(HASHALG& hashAlg, const Json& object);
    // Invoke the specified 'hashAlg' on the attributes of the specified
    // 'object'.

void swap(Json& a, Json& b);
    // Exchange the value of the specified 'a' 'Json' object with that of the
    // specified 'b' 'Json' object.  If an exception is thrown, both objects
    // are left in valid but unspecified states.  This operation guarantees
    // O[1] complexity.  The behavior is undefined unless 'a' was created with
    // the same allocator as 'b'.

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                              // ---------------
                              // class JsonArray
                              // ---------------

// CREATORS
inline
JsonArray::JsonArray()
: d_elements()
{
}

inline
JsonArray::JsonArray(bslma::Allocator *basicAllocator)
: d_elements(basicAllocator)
{
}

inline
JsonArray::JsonArray(const JsonArray&  original,
                     bslma::Allocator *basicAllocator)
: d_elements(original.d_elements, basicAllocator)
{
}

inline
JsonArray::JsonArray(bslmf::MovableRef<JsonArray> original)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_elements(bslmf::MovableRefUtil::move(
                           bslmf::MovableRefUtil::access(original).d_elements))
{
}

inline
JsonArray::JsonArray(bslmf::MovableRef<JsonArray>  original,
                     bslma::Allocator             *basicAllocator)
: d_elements(basicAllocator)
{
    JsonArray& originalAsLvalue = bslmf::MovableRefUtil::access(original);

    if (basicAllocator == originalAsLvalue.allocator()) {
        d_elements = bslmf::MovableRefUtil::move(originalAsLvalue.d_elements);
    }
    else {
        d_elements = originalAsLvalue.d_elements;
    }
}

template <class INPUT_ITERATOR>
inline
JsonArray::JsonArray(INPUT_ITERATOR    first,
                     INPUT_ITERATOR    last,
                     bslma::Allocator *basicAllocator)
: d_elements(first, last, basicAllocator)
{
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
inline
JsonArray::JsonArray(std::initializer_list<Json>  elements,
                     bslma::Allocator            *basicAllocator)
: d_elements(elements, basicAllocator)
{
}
#endif

// MANIPULATORS
inline
JsonArray& JsonArray::operator=(const JsonArray& rhs)
{
    d_elements = rhs.d_elements;
    return *this;
}

inline
JsonArray& JsonArray::operator=(bslmf::MovableRef<JsonArray> rhs)
{
    d_elements = bslmf::MovableRefUtil::move(
                                bslmf::MovableRefUtil::access(rhs).d_elements);
    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
inline
JsonArray& JsonArray::operator=(std::initializer_list<Json> initializer)
{
    d_elements = initializer;
    return *this;
}
#endif

inline
Json& JsonArray::operator[](bsl::size_t index)
{
    BSLS_ASSERT(index < d_elements.size());
    return d_elements[index];
}

template <class INPUT_ITERATOR>
inline
void JsonArray::assign(INPUT_ITERATOR first, INPUT_ITERATOR last)
{
    d_elements.assign(first, last);
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
inline
void JsonArray::assign(std::initializer_list<Json> initializer)
{
    d_elements.assign(initializer);
}
#endif

inline
Json& JsonArray::back()
{
    return d_elements.back();
}

inline
JsonArray::Iterator JsonArray::begin()
{
    return d_elements.begin();
}

inline
void JsonArray::clear()
{
    d_elements.clear();
}

inline
JsonArray::Iterator JsonArray::end()
{
    return d_elements.end();
}

inline
JsonArray::Iterator JsonArray::erase(bsl::size_t index)
{
    return d_elements.erase(d_elements.begin() + index);
}

inline
JsonArray::Iterator JsonArray::erase(ConstIterator position)
{
    return d_elements.erase(position);
}

inline
JsonArray::Iterator JsonArray::erase(ConstIterator first, ConstIterator last)
{
    return d_elements.erase(first, last);
}

inline
Json& JsonArray::front()
{
    return d_elements.front();
}

inline
JsonArray::Iterator JsonArray::insert(bsl::size_t index, const Json& json)
{
    return d_elements.insert(d_elements.begin() + index, json);
}

inline
JsonArray::Iterator JsonArray::insert(bsl::size_t             index,
                                      bslmf::MovableRef<Json> json)
{
    return d_elements.insert(d_elements.begin() + index,
                             bslmf::MovableRefUtil::move(json));
}

template <class INPUT_ITERATOR>
inline
JsonArray::Iterator JsonArray::insert(bsl::size_t    index,
                                      INPUT_ITERATOR first,
                                      INPUT_ITERATOR last)
{
    return d_elements.insert(d_elements.begin() + index, first, last);
}

inline
JsonArray::Iterator JsonArray::insert(ConstIterator position, const Json& json)
{
    return d_elements.insert(position, json);
}

inline
JsonArray::Iterator JsonArray::insert(ConstIterator           position,
                                      bslmf::MovableRef<Json> json)
{
    return d_elements.insert(position, bslmf::MovableRefUtil::move(json));
}

template <class INPUT_ITERATOR>
inline
JsonArray::Iterator JsonArray::insert(ConstIterator  position,
                                      INPUT_ITERATOR first,
                                      INPUT_ITERATOR last)
{
    return d_elements.insert(position, first, last);
}

inline
void JsonArray::popBack()
{
    d_elements.pop_back();
}

inline
void JsonArray::pushBack(const Json& json)
{
    d_elements.push_back(json);
}

inline
void JsonArray::pushBack(bslmf::MovableRef<Json> json)
{
    d_elements.push_back(bslmf::MovableRefUtil::move(json));
}

inline
void JsonArray::resize(bsl::size_t count)
{
    d_elements.resize(count);
}

inline
void JsonArray::resize(bsl::size_t count, const Json& json)
{
    d_elements.resize(count, json);
}

inline
void JsonArray::swap(JsonArray& other)
{
    BSLS_ASSERT(allocator() == other.allocator());
    d_elements.swap(other.d_elements);
}

// ACCESSORS
inline
const Json& JsonArray::operator[](bsl::size_t index) const
{
    BSLS_ASSERT(index < d_elements.size());
    return d_elements[index];
}

inline
bslma::Allocator *JsonArray::allocator() const BSLS_KEYWORD_NOEXCEPT
{
    return d_elements.get_allocator().mechanism();
}

inline
const Json& JsonArray::back() const
{
    return d_elements.back();
}

inline
JsonArray::ConstIterator JsonArray::begin() const BSLS_KEYWORD_NOEXCEPT
{
    return d_elements.begin();
}

inline
JsonArray::ConstIterator JsonArray::cbegin() const BSLS_KEYWORD_NOEXCEPT
{
    return d_elements.begin();
}

inline
JsonArray::ConstIterator JsonArray::cend() const BSLS_KEYWORD_NOEXCEPT
{
    return d_elements.end();
}

inline
bool JsonArray::empty() const
{
    return d_elements.empty();
}

inline
JsonArray::ConstIterator JsonArray::end() const BSLS_KEYWORD_NOEXCEPT
{
    return d_elements.end();
}

inline
const Json& JsonArray::front() const
{
    return d_elements.front();
}

inline
bsl::size_t JsonArray::maxSize() const BSLS_KEYWORD_NOEXCEPT
{
    return d_elements.max_size();
}

inline
bsl::size_t JsonArray::size() const
{
    return d_elements.size();
}

                              // ----------------
                              // class JsonObject
                              // ----------------

// CREATORS
inline
JsonObject::JsonObject()
: d_members()
{
}

inline
JsonObject::JsonObject(bslma::Allocator *basicAllocator)
: d_members(basicAllocator)
{
}

inline
JsonObject::JsonObject(const JsonObject&  original,
                       bslma::Allocator  *basicAllocator)
: d_members(original.d_members, basicAllocator)
{
}

inline
JsonObject::JsonObject(bslmf::MovableRef<JsonObject> original)
    BSLS_KEYWORD_NOEXCEPT
: d_members(bslmf::MovableRefUtil::move(
                            bslmf::MovableRefUtil::access(original).d_members))
{
}

inline
JsonObject::JsonObject(bslmf::MovableRef<JsonObject>  original,
                       bslma::Allocator              *basicAllocator)
: d_members(basicAllocator)
{
    JsonObject& originalAsLvalue = bslmf::MovableRefUtil::access(original);

    if (basicAllocator == originalAsLvalue.allocator()) {
        d_members = bslmf::MovableRefUtil::move(originalAsLvalue.d_members);
    }
    else {
        d_members = originalAsLvalue.d_members;
    }
}

template <class INPUT_ITERATOR>
inline
JsonObject::JsonObject(INPUT_ITERATOR    first,
                       INPUT_ITERATOR    last,
                       bslma::Allocator *basicAllocator)
: d_members(first, last, basicAllocator)
{
#ifdef BSLS_ASSERT_IS_ACTIVE
    for (Container::const_iterator iter = d_members.begin();
         iter != d_members.end();
         ++iter) {
        BSLS_ASSERT(bdlde::Utf8Util::isValid(iter->first.data(),
                                             iter->first.size()));
    }
#endif
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
inline
JsonObject::JsonObject(std::initializer_list<Member>  members,
                       bslma::Allocator              *basicAllocator)
: d_members(members, basicAllocator)
{
#ifdef BSLS_ASSERT_IS_ACTIVE
    for (Container::const_iterator iter = d_members.begin();
         iter != d_members.end();
         ++iter) {
        BSLS_ASSERT(bdlde::Utf8Util::isValid(iter->first.data(),
                                             iter->first.size()));
    }
#endif
}
#endif

// MANIPULATORS
inline
JsonObject& JsonObject::operator=(const JsonObject& rhs)
{
    d_members = rhs.d_members;
    return *this;
}

inline
JsonObject& JsonObject::operator=(bslmf::MovableRef<JsonObject> rhs)
{
    Container& rhsMembers = bslmf::MovableRefUtil::access(rhs).d_members;
    if (d_members.get_allocator() == rhsMembers.get_allocator()) {
        d_members = bslmf::MovableRefUtil::move(rhsMembers);
    }
    else {
        d_members = rhsMembers;
    }
    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
inline
JsonObject& JsonObject::operator=(std::initializer_list<Member> members)
{
    d_members = members;
    return *this;
}
#endif

inline
Json& JsonObject::operator[](const bsl::string_view& key)
{
    BSLS_ASSERT(bdlde::Utf8Util::isValid(key.data(), key.size()));
    Iterator it = d_members.find(key);
    if (it != d_members.end()) {
        return it->second;                                            // RETURN
    }
    return d_members[bsl::string(key)];
}

inline
JsonObject::Iterator JsonObject::begin() BSLS_KEYWORD_NOEXCEPT
{
    return d_members.begin();
}

inline
void JsonObject::clear() BSLS_KEYWORD_NOEXCEPT
{
    d_members.clear();
}

inline
JsonObject::Iterator JsonObject::end() BSLS_KEYWORD_NOEXCEPT
{
    return d_members.end();
}

inline
bsl::size_t JsonObject::erase(const bsl::string_view& key)
{
    Iterator it = d_members.find(key);
    if (it == d_members.end()) {
        return 0;                                                     // RETURN
    }
    d_members.erase(it);
    return 1;
}

inline
JsonObject::Iterator JsonObject::erase(Iterator position)
{
    return d_members.erase(position);
}

inline
JsonObject::Iterator JsonObject::erase(ConstIterator position)
{
    return d_members.erase(position);
}

inline
JsonObject::Iterator JsonObject::find(const bsl::string_view& key)
{
    return d_members.find(key);
}

inline
bsl::pair<JsonObject::Iterator, bool> JsonObject::insert(const Member& member)
{
    return d_members.insert(member);
}

inline
bsl::pair<JsonObject::Iterator, bool> JsonObject::insert(
                                              bslmf::MovableRef<Member> member)
{
#ifdef BSLS_ASSERT_IS_ACTIVE
    const bsl::string& key = bslmf::MovableRefUtil::access(member).first;
    BSLS_ASSERT(bdlde::Utf8Util::isValid(key.data(), key.size()));
#endif
    return d_members.insert(bslmf::MovableRefUtil::move(member));
}

template <class INPUT_ITERATOR>
inline
typename bsl::enable_if<
    !bsl::is_convertible<INPUT_ITERATOR, bsl::string_view>::value,
    void>::type
JsonObject::insert(INPUT_ITERATOR first, INPUT_ITERATOR last)
{
    d_members.insert(first, last);
#ifdef BSLS_ASSERT_SAFE_IS_ACTIVE
    for (Container::const_iterator iter = d_members.begin();
         iter != d_members.end();
         ++iter) {
        BSLS_ASSERT_SAFE(
             bdlde::Utf8Util::isValid(iter->first.data(), iter->first.size()));
    }
#endif
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
inline
void JsonObject::insert(std::initializer_list<Member> members)
{
#ifdef BSLS_ASSERT_IS_ACTIVE
    for (auto member : members) {
        BSLS_ASSERT(bdlde::Utf8Util::isValid(member.first.data(),
                                             member.first.size()));
    }
#endif
    d_members.insert(members);
}
#endif

template <class VALUE>
bsl::pair<JsonObject::Iterator, bool> JsonObject::insert(
                                const bsl::string_view&                  key,
                                BSLS_COMPILERFEATURES_FORWARD_REF(VALUE) value)
{
    BSLS_ASSERT(bdlde::Utf8Util::isValid(key.data(), key.size()));
    Json   json(BSLS_COMPILERFEATURES_FORWARD(VALUE, value), allocator());
    Member member(key, Json(), allocator());
    member.second = bslmf::MovableRefUtil::move(json);
    return insert(bslmf::MovableRefUtil::move(member));
}

inline
void JsonObject::swap(JsonObject& other)
{
    BSLS_ASSERT(allocator() == other.allocator());
    d_members.swap(other.d_members);
}

// ACCESSORS
inline
const Json& JsonObject::operator[](const bsl::string_view& key) const
{
    ConstIterator it = d_members.find(key);
    BSLS_ASSERT(it != d_members.end());
    return it->second;
}

inline
bslma::Allocator *JsonObject::allocator() const BSLS_KEYWORD_NOEXCEPT
{
    return d_members.get_allocator().mechanism();
}

inline
JsonObject::ConstIterator JsonObject::begin() const BSLS_KEYWORD_NOEXCEPT
{
    return d_members.begin();
}

inline
JsonObject::ConstIterator JsonObject::cbegin() const BSLS_KEYWORD_NOEXCEPT
{
    return d_members.cbegin();
}

inline
JsonObject::ConstIterator JsonObject::cend() const BSLS_KEYWORD_NOEXCEPT
{
    return d_members.cend();
}

inline
bool JsonObject::contains(const bsl::string_view& key) const
{
    return d_members.find(key) != d_members.end();
}

inline
bool JsonObject::empty() const BSLS_KEYWORD_NOEXCEPT
{
    return d_members.empty();
}

inline
JsonObject::ConstIterator JsonObject::end() const BSLS_KEYWORD_NOEXCEPT
{
    return d_members.end();
}

inline
JsonObject::ConstIterator JsonObject::find(const bsl::string_view& key) const
{
    return d_members.find(key);
}

inline
bsl::size_t JsonObject::size() const BSLS_KEYWORD_NOEXCEPT
{
    return d_members.size();
}
                                 // ----------
                                 // class Json
                                 // ----------

// CREATORS
inline
Json::Json()
: d_value(JsonNull())
{
}

inline
Json::Json(bslma::Allocator *basicAllocator)
: d_value(JsonNull(), basicAllocator)
{
}

inline
Json::Json(const Json& original, bslma::Allocator *basicAllocator)
: d_value(original.d_value, basicAllocator)
{
}

inline
Json::Json(bslmf::MovableRef<Json> original)
: d_value(bslmf::MovableRefUtil::move(
                              bslmf::MovableRefUtil::access(original).d_value))
{
}

inline
Json::Json(bslmf::MovableRef<Json> original, bslma::Allocator *basicAllocator)
: d_value(bslmf::MovableRefUtil::move(
                              bslmf::MovableRefUtil::access(original).d_value),
          basicAllocator)
{
}

inline
Json::Json(const JsonArray& array, bslma::Allocator *basicAllocator)
: d_value(array, basicAllocator)
{
}

inline
Json::Json(bslmf::MovableRef<JsonArray>  array,
           bslma::Allocator             *basicAllocator)
: d_value(bslmf::MovableRefUtil::move(array), basicAllocator)
{
}

inline
Json::Json(bool boolean, bslma::Allocator *basicAllocator)
: d_value(boolean, basicAllocator)
{
}

inline
Json::Json(const JsonNull& null, bslma::Allocator *basicAllocator)
: d_value(null, basicAllocator)
{
}

inline
Json::Json(float number, bslma::Allocator *basicAllocator)
: d_value(JsonNumber(number), basicAllocator)
{
}

inline
Json::Json(double number, bslma::Allocator *basicAllocator)
: d_value(JsonNumber(number), basicAllocator)
{
}

inline
Json::Json(bdldfp::Decimal64 number, bslma::Allocator *basicAllocator)
: d_value(JsonNumber(number), basicAllocator)
{
}

inline
Json::Json(int number, bslma::Allocator *basicAllocator)
: d_value(JsonNumber(number), basicAllocator)
{
}

inline
Json::Json(unsigned int number, bslma::Allocator *basicAllocator)
: d_value(JsonNumber(number), basicAllocator)
{
}

inline
Json::Json(bsls::Types::Int64 number, bslma::Allocator *basicAllocator)
: d_value(JsonNumber(number), basicAllocator)
{
}

inline
Json::Json(bsls::Types::Uint64 number, bslma::Allocator *basicAllocator)
: d_value(JsonNumber(number), basicAllocator)
{
}

inline
Json::Json(const JsonNumber& number, bslma::Allocator *basicAllocator)
: d_value(number, basicAllocator)
{
}

inline
Json::Json(bslmf::MovableRef<JsonNumber>  number,
           bslma::Allocator              *basicAllocator)
: d_value(bslmf::MovableRefUtil::move(number), basicAllocator)
{
}

inline
Json::Json(const JsonObject& object, bslma::Allocator *basicAllocator)
: d_value(object, basicAllocator)
{
}

inline
Json::Json(bslmf::MovableRef<JsonObject>  object,
           bslma::Allocator              *basicAllocator)
: d_value(bslmf::MovableRefUtil::move(object), basicAllocator)
{
}

inline
Json::Json(const char *string, bslma::Allocator *basicAllocator)
: d_value(basicAllocator)
{
    BSLS_ASSERT(bdlde::Utf8Util::isValid(string));
    d_value.createInPlace<bsl::string>(string);
}

inline
Json::Json(const bsl::string_view& string, bslma::Allocator *basicAllocator)
: d_value(basicAllocator)
{
    BSLS_ASSERT(bdlde::Utf8Util::isValid(string));
    d_value.createInPlace<bsl::string>(string);
}

template <class STRING_TYPE>
inline
Json::Json(BSLMF_MOVABLEREF_DEDUCE(STRING_TYPE) string,
           bslma::Allocator                    *basicAllocator,
           typename bsl::enable_if<
               bsl::is_same<STRING_TYPE, bsl::string>::value>::type *)
: d_value(bslmf::MovableRefUtil::move(string), basicAllocator)
{
    BSLS_ASSERT(bdlde::Utf8Util::isValid(
                   bslmf::MovableRefUtil::access(d_value.the<bsl::string>())));
}

// MANIPULATORS
inline
Json& Json::operator=(const Json& rhs)
{
    d_value = rhs.d_value;
    return *this;
}

inline
Json& Json::operator=(bslmf::MovableRef<Json> rhs)
{
    Json& rhsRef = bslmf::MovableRefUtil::access(rhs);
    d_value      = bslmf::MovableRefUtil::move(rhsRef.d_value);
    return *this;
}

inline
Json& Json::operator=(float rhs)
{
    d_value.createInPlace<JsonNumber>(rhs);
    return *this;
}

inline
Json& Json::operator=(double rhs)
{
    d_value.createInPlace<JsonNumber>(rhs);
    return *this;
}

inline
Json& Json::operator=(bdldfp::Decimal64 rhs)
{
    d_value.createInPlace<JsonNumber>(rhs);
    return *this;
}

inline
Json& Json::operator=(int rhs)
{
    d_value.createInPlace<JsonNumber>(rhs);
    return *this;
}

inline
Json& Json::operator=(unsigned int rhs)
{
    d_value.createInPlace<JsonNumber>(rhs);
    return *this;
}

inline
Json& Json::operator=(bsls::Types::Int64 rhs)
{
    d_value.createInPlace<JsonNumber>(rhs);
    return *this;
}

inline
Json& Json::operator=(bsls::Types::Uint64 rhs)
{
    d_value.createInPlace<JsonNumber>(rhs);
    return *this;
}

inline
Json& Json::operator=(const JsonNumber& rhs)
{
    d_value.createInPlace<JsonNumber>(rhs);
    return *this;
}

inline
Json& Json::operator=(bslmf::MovableRef<JsonNumber> rhs)
{
    d_value = bslmf::MovableRefUtil::move(rhs);
    return *this;
}

inline
Json& Json::operator=(const char *rhs)
{
    makeString(rhs);
    return *this;
}

inline
Json& Json::operator=(const bsl::string_view& rhs)
{
    makeString(rhs);
    return *this;
}

template <class STRING_TYPE>
typename bsl::enable_if<bsl::is_same<STRING_TYPE, bsl::string>::value,
                        Json>::type&
    Json::operator=(BSLMF_MOVABLEREF_DEDUCE(STRING_TYPE) rhs)
{
    makeString(bslmf::MovableRefUtil::move(rhs));
    return *this;
}

inline
Json& Json::operator=(bool rhs)
{
    makeBoolean(rhs);
    return *this;
}

inline
Json& Json::operator=(const JsonObject& rhs)
{
    makeObject(rhs);
    return *this;
}

inline
Json& Json::operator=(bslmf::MovableRef<JsonObject> rhs)
{
    makeObject(bslmf::MovableRefUtil::move(rhs));
    return *this;
}

inline
Json& Json::operator=(const JsonArray& rhs)
{
    makeArray(rhs);
    return *this;
}

inline
Json& Json::operator=(bslmf::MovableRef<JsonArray> rhs)
{
    makeArray(bslmf::MovableRefUtil::move(rhs));
    return *this;
}

inline
Json& Json::operator=(const JsonNull&)
{
    makeNull();
    return *this;
}

inline
Json& Json::operator=(bslmf::MovableRef<JsonNull>)
{
    makeNull();
    return *this;
}

inline
JsonArray& Json::makeArray()
{
    return d_value.createInPlace<JsonArray>();
}

inline
JsonArray& Json::makeArray(const JsonArray& array)
{
    d_value = array;
    return d_value.the<JsonArray>();
}

inline
JsonArray& Json::makeArray(bslmf::MovableRef<JsonArray> array)
{
    d_value = bslmf::MovableRefUtil::move(array);
    return d_value.the<JsonArray>();
}

inline
bool& Json::makeBoolean()
{
    return d_value.createInPlace<bool>();
}

inline
bool& Json::makeBoolean(bool boolean)
{
    d_value = boolean;
    return d_value.the<bool>();
}

inline
void Json::makeNull()
{
    d_value.createInPlace<JsonNull>();
}

inline
JsonNumber& Json::makeNumber()
{
    return d_value.createInPlace<JsonNumber>();
}

inline
JsonNumber& Json::makeNumber(const JsonNumber& number)
{
    d_value = number;
    return d_value.the<JsonNumber>();
}

inline
JsonNumber& Json::makeNumber(bslmf::MovableRef<JsonNumber> number)
{
    d_value = bslmf::MovableRefUtil::move(number);
    return d_value.the<JsonNumber>();
}

inline
JsonObject& Json::makeObject()
{
    return d_value.createInPlace<JsonObject>();
}

inline
JsonObject& Json::makeObject(const JsonObject& object)
{
    d_value = object;
    return d_value.the<JsonObject>();
}

inline
JsonObject& Json::makeObject(bslmf::MovableRef<JsonObject> object)
{
    d_value = bslmf::MovableRefUtil::move(object);
    return d_value.the<JsonObject>();
}

inline
void Json::makeString(const char *string)
{
    BSLS_ASSERT(bdlde::Utf8Util::isValid(string));
    d_value.createInPlace<bsl::string>(string);
}

inline
void Json::makeString(const bsl::string_view& string)
{
    BSLS_ASSERT(bdlde::Utf8Util::isValid(string.data(), string.size()));
    d_value.createInPlace<bsl::string>(string);
}

template <class STRING_TYPE>
inline
typename bsl::enable_if<bsl::is_same<STRING_TYPE, bsl::string>::value>::type
    Json::makeString(BSLMF_MOVABLEREF_DEDUCE(STRING_TYPE) string)
{
    BSLS_ASSERT(
              bdlde::Utf8Util::isValid(bslmf::MovableRefUtil::access(string)));
    d_value = bslmf::MovableRefUtil::move(string);
}

inline
void Json::swap(Json& other)
{
    BSLS_ASSERT(allocator() == other.allocator());
    d_value.swap(other.d_value);
}

// ACCESSORS
inline
bool Json::isArray() const
{
    return type() == JsonType::e_ARRAY;
}

inline
bool Json::isBoolean() const
{
    return type() == JsonType::e_BOOLEAN;
}

inline
bool Json::isNull() const
{
    return type() == JsonType::e_NULL;
}

inline
bool Json::isNumber() const
{
    return type() == JsonType::e_NUMBER;
}

inline
bool Json::isObject() const
{
    return type() == JsonType::e_OBJECT;
}

inline
bool Json::isString() const
{
    return type() == JsonType::e_STRING;
}

inline
JsonArray& Json::theArray()
{
    return d_value.the<JsonArray>();
}

inline
bool& Json::theBoolean()
{
    return d_value.the<bool>();
}

inline
JsonNull& Json::theNull()
{
    return d_value.the<JsonNull>();
}

inline
JsonNumber& Json::theNumber()
{
    return d_value.the<JsonNumber>();
}

inline
JsonObject& Json::theObject()
{
    return d_value.the<JsonObject>();
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT)
inline
Json::operator JsonArray &()
{
    return theArray();
}

inline
Json::operator bool &()
{
    return theBoolean();
}

inline
Json::operator JsonNull &()
{
    return theNull();
}

inline
Json::operator JsonNumber &()
{
    return theNumber();
}

inline
Json::operator JsonObject &()
{
    return theObject();
}
#endif

inline
Json& Json::operator[](const bsl::string_view& key)
{
    BSLS_ASSERT(d_value.is<JsonObject>());
    return theObject()[key];
}

inline
Json& Json::operator[](bsl::size_t index)
{
    BSLS_ASSERT(d_value.is<JsonArray>());
    return theArray()[index];
}

inline
bsl::size_t Json::size() const
{
    BSLS_ASSERT(isArray() || isObject());
    return isArray() ? theArray().size() : theObject().size();
}

// ACCESSORS
inline
bslma::Allocator *Json::allocator() const BSLS_KEYWORD_NOEXCEPT
{
    return d_value.getAllocator();
}

inline
bdldfp::Decimal64 Json::asDecimal64() const
{
    BSLS_ASSERT(isNumber());
    return theNumber().asDecimal64();
}

inline
int Json::asDecimal64Exact(bdldfp::Decimal64 *result) const
{
    BSLS_ASSERT(isNumber());
    return theNumber().asDecimal64Exact(result);
}

inline
double Json::asDouble() const
{
    BSLS_ASSERT(isNumber());
    return theNumber().asDouble();
}

inline
float Json::asFloat() const
{
    BSLS_ASSERT(isNumber());
    return theNumber().asFloat();
}

inline
int Json::asInt(int *result) const
{
    BSLS_ASSERT(isNumber());
    return theNumber().asInt(result);
}

inline
int Json::asInt64(bsls::Types::Int64 *result) const
{
    BSLS_ASSERT(isNumber());
    return theNumber().asInt64(result);
}

inline
int Json::asUint(unsigned int *result) const
{
    BSLS_ASSERT(isNumber());
    return theNumber().asUint(result);
}

inline
int Json::asUint64(bsls::Types::Uint64 *result) const
{
    BSLS_ASSERT(isNumber());
    return theNumber().asUint64(result);
}


inline
const JsonArray& Json::theArray() const
{
    return d_value.the<JsonArray>();
}

inline
const bool& Json::theBoolean() const
{
    return d_value.the<bool>();
}

inline
const JsonNull& Json::theNull() const
{
    return d_value.the<JsonNull>();
}

inline
const JsonNumber& Json::theNumber() const
{
    return d_value.the<JsonNumber>();
}

inline
const JsonObject& Json::theObject() const
{
    return d_value.the<JsonObject>();
}

inline
const bsl::string& Json::theString() const
{
    return d_value.the<bsl::string>();
}

inline
JsonType::Enum Json::type() const BSLS_KEYWORD_NOEXCEPT
{
    return static_cast<JsonType::Enum>(d_value.typeIndex() - 1);
}
inline
const Json& Json::operator[](const bsl::string_view& key) const
{
    BSLS_ASSERT(d_value.is<JsonObject>());
    return theObject()[key];
}

inline
const Json& Json::operator[](bsl::size_t index) const
{
    BSLS_ASSERT(d_value.is<JsonArray>());
    return theArray()[index];
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT)
inline
Json::operator const JsonArray &() const
{
    return theArray();
}

inline
Json::operator const bool &() const
{
    return theBoolean();
}

inline
Json::operator const JsonNull &() const
{
    return theNull();
}

inline
Json::operator const JsonNumber &() const
{
    return theNumber();
}

inline
Json::operator const JsonObject &() const
{
    return theObject();
}

inline
Json::operator const bsl::string &() const
{
    return theString();
}
#endif

}  // close package namespace

// FREE OPERATORS
inline
bsl::ostream& bdljsn::operator<<(bsl::ostream&            stream,
                                 const bdljsn::JsonArray& object)
{
    return object.print(stream, 0, -1);
}

inline
bool bdljsn::operator==(const bdljsn::JsonArray& lhs,
                        const bdljsn::JsonArray& rhs)
{
    return lhs.d_elements == rhs.d_elements;
}

inline
bool bdljsn::operator!=(const bdljsn::JsonArray& lhs,
                        const bdljsn::JsonArray& rhs)
{
    return lhs.d_elements != rhs.d_elements;
}

template <class HASHALG>
inline
void bdljsn::hashAppend(HASHALG& hashAlg, const bdljsn::JsonArray& object)
{
    hashAppend(hashAlg, object.d_elements);
}

inline
void bdljsn::swap(bdljsn::JsonArray& a, bdljsn::JsonArray& b)
{
    bslalg::SwapUtil::swap(&a.d_elements, &b.d_elements);
}

inline
bsl::ostream& bdljsn::operator<<(bsl::ostream&             stream,
                                 const bdljsn::JsonObject& object)
{
    return object.print(stream, 0, -1);
}

inline
bool bdljsn::operator==(const bdljsn::JsonObject& lhs,
                        const bdljsn::JsonObject& rhs)
{
    return lhs.d_members == rhs.d_members;
}

inline
bool bdljsn::operator!=(const bdljsn::JsonObject& lhs,
                        const bdljsn::JsonObject& rhs)
{
    return lhs.d_members != rhs.d_members;
}

template <class HASHALG>
inline
void bdljsn::hashAppend(HASHALG& hashAlg, const bdljsn::JsonObject& object)
{
    hashAppend(hashAlg, object.d_members);
}

inline
void bdljsn::swap(bdljsn::JsonObject& a, bdljsn::JsonObject& b)
{
    if (a.allocator() == b.allocator()) {
        bslalg::SwapUtil::swap(&a.d_members, &b.d_members);
    }
    else {
        bslma::Allocator *const allocA = a.allocator();
        bslma::Allocator *const allocB = b.allocator();
        JsonObject              ta(b, allocA);
        JsonObject              tb(a, allocB);
        swap(a, ta);
        swap(b, tb);
    }
}

inline
bsl::ostream& bdljsn::operator<<(bsl::ostream&       stream,
                                 const bdljsn::Json& object)
{
    return object.print(stream, 0, -1);
}

inline
bool bdljsn::operator==(const bdljsn::Json& lhs, const bdljsn::Json& rhs)
{
    return lhs.d_value == rhs.d_value;
}

inline
bool bdljsn::operator!=(const bdljsn::Json& lhs, const bdljsn::Json& rhs)
{
    return lhs.d_value != rhs.d_value;
}

template <class HASHALG>
inline
void bdljsn::hashAppend(HASHALG& hashAlg, const bdljsn::Json& object)
{
    hashAppend(hashAlg, object.d_value);
}

inline
void bdljsn::swap(bdljsn::Json& a, bdljsn::Json& b)
{
    bslalg::SwapUtil::swap(&a.d_value, &b.d_value);
}

}  // close enterprise namespace

#endif  // INCLUDED_BDLJSN_JSON

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
