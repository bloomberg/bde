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
//@DESCRIPTION: This component provides a value-semantic type, `bdljsn::Json`,
// used as an in-memory representation for a JSON document.  This component
// also provides `bdljsn::JsonArray` and `bdljsn::JsonObject` types for
// representing JSON Arrays and Objects respectively.
//
// `bdljsn::Json` has a close structural similarity to the JSON grammar itself,
// which at a high-level looks like:
// ```
// JSON ::= Object
//        | Array
//        | String
//        | Number
//        | Boolean
//        | null
// ```
// Where the Object and Array alternatives can recursively contain JSON.  Just
// like this grammar, a `bdljsn::Json` is a variant holding either an Object,
// Array, String, Number, Boolean, or null.  These variant selections are
// represented by the following types:
//
// * Object: `JsonObject`
// * Array: `JsonArray`
// * String: `bsl::string`
// * Number: `JsonNumber`
// * Boolean: `bool`
// * null: `JsonNull`
//
// For more details on the JSON grammar see:
//
// * website: https://www.json.org/
// * RFC: https://datatracker.ietf.org/doc/html/rfc8259
//
///Reading and Writing a `bdljsn::Json` Object
///-------------------------------------------
// `bdljsn_jsonutil` is the recommended facility to write and read
// `bdljsn::Json` objects to and from JSON document text.
//
// `operator<<` overloads are available for all the types.  In addition a
// canonical BDE print method is available for `bdljsn::Json`,
// `bdljsn::JsonObject`, `bdljsn::JsonArray` `bdljsn::JsonNumber` and
// `bdljsn::JsonNull`.
//
///All `bdljsn::Json` Objects are Valid JSON Documents
///---------------------------------------------------
// Every `bdljsn::Json` represents a (valid) JSON document (`bdljsn::Json` does
// not have an "invalid" state).  This means writing a `bdljsn::Json` to a
// string can only fail if an out-of-memory condition occurs.
//
// Similarly, every JSON document has a `bdljsn::Json` representation.
// However, `bdljsn::JsonObject` represents Objects having only unique member
// names, meaning that duplicate member names in a JSON document will be
// ignored (see `bdljsn::JsonUtil` for more information on how duplicate names
// are handled when parsing a JSON document).  Note that the JSON RFC says that
// Object member names "SHOULD be unique", and there is no standard behavior
// for JSON parsers where member names are not unique (typically an in-process
// representation with unique member names is used).
//
///Important Preconditions
///- - - - - - - - - - - -
// In order to preserve the invariant that all `bdljsn::Json` objects are valid
// JSON documents there are some constructors and assignment operations in
// `bdljsn` package that have notable preconditions:
//
// * `bdljsn::JsonNumber` constructors from string require that the string
//    conform to the JSON grammar for a number (see
//   `bdljsn::NumberUtil::isValidNumber`).
// * Constructors and assignment operators from `double` or
//   `bdldfp::Decimal64` require the value *not* be either INF or NaN
// * `bdljsn::Json` constructors and assignment operators require that
//    strings contain valid UTF-8 (see `bdlde::Utf8Util::isValid`).
//
///`operator==` and the Definition of Value
///----------------------------------------
// `bdljsn::Json` type's definition of value (i.e., the behavior for
// `operator==`) mirrors comparing the text of two JSON documents where all the
// white-space is ignored.
//
// Concretely, `bdljsn::Json` is a variant type, whose definition of equality
// is derived from the definition of equality of its constituent types.  I.e.,
// two `bdljsn::Json` objects compare equal if they have the same `type` and
// the two objects of that `type` they contain compare equal.  The definition
// of equality for Object, Array, Boolean, string, and `JsonNull` types are
// relatively self-explanatory (see respective `operator==` definitions for
// details).  The definition of equality for `JsonNumber` is notable:
// `JsonNumber` objects define value in terms of the text of the JSON number
// string they contain.  So two JSON numbers having the same numerical value
// may compare *unequal* (e.g., "2" and "2.0" and "20e-1" are considered
// different `JsonNumber` values!).  Note that `bdljsn::JsonNumber::isEqual`
// provides a semantic comparison of two numbers (see `bdljsn_jsonnumber` for
// more detail).
//
///Usage
///-----
// This section illustrates the intended use of this component.
//
///Example 1: Constructor a Basic `bdljsn::Json` Object
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// Most often `bdljsn::Json` objects will be written and read from JSON text
// using `bdljsn_jsonutil`.  In this simple example, we demonstrate manually
// creating the document below and then verify the properties of the resulting
// object:
// ```
// {
//     "number": 3.14,
//     "boolean": true,
//     "string": "text",
//     "null": null,
//     "array": [ "2.76", true ],
//     "object": { "boolean": false }
// }
// ```
// First, we use `bdljsn::Json::makeObject` to configure the top level
// `bdljsn::Json` object to be a `bdljsn::Json` object, and use the various
// manipulators of `bdljsn::JsonObject` to configure its value:
// ```
// using namespace bdldfp::DecimalLiterals;
//
// bdljsn::Json json;
//
// json.makeObject();
// json["number"]  = 3.14;
// json["boolean"] = true;
// json["string"]  = "text";
// json["array"].makeArray();
// json["array"].theArray().pushBack(bdljsn::Json(2.76_d64));
// json["array"].theArray().pushBack(bdljsn::Json(true));
// json["object"].makeObject()["boolean"] = false;
// ```
// Notice that we used `operator[]` to implicitly create new members of the
// top-level object.  Using `json.theObject().insert` would be more efficient
// (see example 2).
//
// Finally, we validate the properties of the resulting object:
// ```
// assert(3.14     == json["number"].asDouble());
// assert(true     == json["boolean"].theBoolean());
// assert("text"   == json["string"].theString());
// assert(true     == json["null"].isNull());
// assert(2.76_d64 == json["array"][0].asDecimal64());
// assert(false    == json["object"]["boolean"].theBoolean());
// ```
//
///Example 2: More Efficiently Creating a `bdljsn::Json`
///-----------------------------------------------------
// Example 1 used `operator[]` to implicitly add members to the Objects.  Using
// `operator[]` is intuitive but not the most efficient method to add new
// members to a `bdljsn::JsonObject` (similar to using `operator[]` to add
// elements to an `unordered_map`).  The following code demonstrates a more
// efficient way to create the same `bdljsn::Json` representation as example 1:
// ```
// using namespace bdldfp::DecimalLiterals;
//
// bdljsn::Json       json;
// bdljsn::JsonArray  subArray;
// bdljsn::JsonObject subObject;
//
// json.makeObject();
// json.theObject().insert("number", bdljsn::JsonNumber(3.14));
// json.theObject().insert("boolean", true);
// json.theObject().insert("string", "text");
// json.theObject().insert("null", bdljsn::JsonNull());
//
// subArray.pushBack(bdljsn::Json(2.76_d64));
// subArray.pushBack(bdljsn::Json(true));
// json.theObject().insert("array", bsl::move(subArray));
//
// subObject.insert("boolean", false);
// json.theObject().insert("object", bsl::move(subObject));
// ```
//
///Example 3: Using the 'visit' Method to Traverse a 'Json' Object
///---------------------------------------------------------------
// The `Json` class provides the (overloaded) `visit` method that invokes a
// user-supplied "visitor" functor according to the current `type()` of the
// `Json` object.
//
// For example, suppose one needs to survey the structure of the `Json` object
// created in {Example 1} (and again in {Example 2}) and, in doing so, compile
// a tally of each of each of the `Json` sub-objects and their their types
// (i.e., object, array, string, ...).
//
// First, we define a compliant visitor class, `TallyByTypeVisitor`:
// ```
//                             // ==================
//                             // TallyByTypeVisitor
//                             // ==================
//
// class TallyByTypeVisitor {
//
//     int d_tally[6] = { 0, 0, 0, 0, 0, 0 };
//
//   public:
//     // CREATORS
//
//     /// Create a `TallyByTypeVisitor` object that when passed to the
//     /// `Json::visit` method will increment the specified `tally` array
//     /// according to `type()` and visit subordinate `Json` objects, if
//     /// any.  The behavior is undefined unless `tally` has at least
//     /// 6 elements.
//     explicit TallyByTypeVisitor();
//
//     // ACCESSORS
//
//     /// Increment the element corresponding to object in the tally array
//     /// supplied at construction and visit the value of each member of
//     /// the specified `object`.
//     void operator()(const JsonObject& object);
//
//
//     /// Increment the element corresponding to array in the tally array
//     /// supplied at construction and visit each element of the specified
//     /// `array`.
//     void operator()(const JsonArray& array);
//
//     /// Increment the element corresponding to
//     /// string/number/boolean/null in the tally array supplied at
//     /// construction.  Ignore the specified
//     /// `string`/`number`/`boolean`/`null`.
//     void operator()(const bsl::string& string );
//     void operator()(const JsonNumber&  number );
//     void operator()(const bool&        boolean);
//     void operator()(const JsonNull&    null   );
//
//     /// Return the address of an array of 6 elements containing the
//     /// tally by type.  The array is ordered according to
//     /// `JsonType::Enum`.
//     const int *tally() const;
// };
// ```
// Notice that we have no need to change the value of the examined `Json`
// objects so we use a set of `operator()` overloads compatible with the
// `visit` accessor method.  Accordingly, we careful below to use this visitor
// only with `const`-qualified `Json` objects
//
// Then, we define the constructor and the six `operator()` overloads.  In each
// overload by type we increment the appropriate element in the user-supplied
// array of integers.
// ```
//                             // ------------------
//                             // TallyByTypeVisitor
//                             // ------------------
//
// // CREATORS
// TallyByTypeVisitor::TallyByTypeVisitor()
// {
// }
//
// // ACCESSORS
// void TallyByTypeVisitor::operator()(const bsl::string& string)
// {
//     (void) string;
//     ++d_tally[bdljsn::JsonType::e_STRING];
// }
//
// void TallyByTypeVisitor::operator()(const JsonNumber& number)
// {
//     (void) number;
//     ++d_tally[bdljsn::JsonType::e_NUMBER];
// }
//
// void TallyByTypeVisitor::operator()(const bool& boolean)
// {
//     (void) boolean;
//     ++d_tally[bdljsn::JsonType::e_BOOLEAN];
// }
//
// void TallyByTypeVisitor::operator()(const JsonNull& null)
// {
//     (void) null;
//     ++d_tally[bdljsn::JsonType::e_NULL];
// }
// ```
// Next, we define the visitor overload for array types.  After incrementing
// the tally array we pass this same visitor object to the `Json` object in the
// array so those objects are included in the tally.
// ```
// void TallyByTypeVisitor::operator()(const JsonArray& array)
// {
//      ++d_tally[bdljsn::JsonType::e_ARRAY];
//
//     typedef JsonArray::ConstIterator ConstItr;
//
//     for (ConstItr cur  = array.cbegin(),
//                   end  = array.cend();
//                   end != cur; ++cur) {
//         const Json constElement = *cur;
//         constElement.visit<void>(*this);
//     }
// }
//
// const int *TallyByTypeVisitor::tally() const
// {
//     return d_tally;
// }
// ```
// Notice that `element` is `const`-qualified so the accessor `visit` method is
// invoked.
//
// Then, we implement the visitor overload for the object type.  Examination
// of the object produces a sequence of name-value pairs where the `second`
// part is a `Json` object that we must visit.
// ```
// void TallyByTypeVisitor::operator()(const JsonObject& object)
// {
//      ++d_tally[bdljsn::JsonType::e_OBJECT];
//
//     typedef JsonObject::ConstIterator ConstItr;
//
//     for (ConstItr cur  = object.cbegin(),
//                   end  = object.cend();
//                   end != cur; ++cur) {
//         const bsl::pair<const bsl::string, Json> member = *cur;
//         const Json&                              json   = member.second;
//         json.visit<void>(*this);
//     }
// }
// ```
// Again, notice that this visitor is used as an argument to a
// `const`-qualified `Json` object.
//
// Finally, we make a survey of the `Json` object created in {Example 1} (and
// duplicated in {Example 2}).  From visual inspection of the source JSON
// document we expect 10 `Json` objects distributed thus:
//
// * Object
//   1. top-level of the document is an object
//   2. '{"boolean": false }', interior object
//
// * Array
//   1. '[2.76, true]'
//
// * String
//   1. '"text"'
//
// * Number
//   1. '2.76'
//   2. '3.14'
//
// * Boolean
//   1. 'false', from the internal object
//   2. 'true', from the array
//   3. 'true', from top-level object
//
// * Null
//   1. null
//
// Use of our visitor functor on `example1` confirms these observations:
// ```
// int main()
// {
//     TallyByTypeVisitor visitor;
//
//     const Json& constExample1 = example1;
//
//     constExample1.visit<void>(&visitor);
//
//     const int *const tally = visitor.tally();
//
//     assert(2 == tally[bdljsn::JsonType::e_OBJECT ]);
//     assert(1 == tally[bdljsn::JsonType::e_ARRAY  ]);
//     assert(1 == tally[bdljsn::JsonType::e_STRING ]);
//     assert(2 == tally[bdljsn::JsonType::e_NUMBER ]);
//     assert(3 == tally[bdljsn::JsonType::e_BOOLEAN]);
//     assert(1 == tally[bdljsn::JsonType::e_NULL   ]);
//
//     return 0;
// }
// ```

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
#include <bslma_bslallocator.h>
#include <bslma_polymorphicallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_assert.h>
#include <bslmf_enableif.h>
#include <bslmf_issame.h>
#include <bslmf_movableref.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bslmf_util.h>

#include <bsla_unreachable.h>

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_types.h>
#include <bsls_util.h>

#include <bsl_exception.h>     // 'bsl::uncaught_exceptions'
#include <bsl_iterator.h>
#include <bsl_string.h>
#include <bsl_string_view.h>
#include <bsl_type_traits.h>   // 'bsl::enable_if', 'bsl::is_same'
#include <bsl_unordered_map.h>
#include <bsl_utility.h>       // 'bsl::forward'
#include <bsl_vector.h>

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY)
#include <string_view>
#endif

namespace BloombergLP {
namespace bdljsn {

// FORWARD DECLARATIONS
class Json;
class JsonArray;
class JsonObject;
class Json_Initializer;
class Json_MemberInitializer;

                              // ===============
                              // class JsonArray
                              // ===============

/// This type is designed to replicate much of the standard sequence
/// container interface, eliding interfaces that are less relevant for a
/// non-generic container.  Note that a `bsl::vector` is chosen for the
/// implementation because our implementation permits the element type to be
/// incomplete (when just spelling the type name).
class JsonArray {

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

    /// Create an empty `JsonArray`.  Optionally specify a `basicAllocator`
    /// used to supply memory.  If `basicAllocator` is not specified, the
    /// currently installed default allocator is used.
    JsonArray();
    explicit JsonArray(bslma::Allocator *basicAllocator);

    /// Create a `JsonArray` having the same value as the specified
    /// `original` object.  Optionally specify `basicAllocator` to supply
    /// memory.  If `basicAllocator` is not specified, the allocator
    /// associated with `original` is propagated for use in the
    /// newly-created `JsonArray`.
    JsonArray(const JsonArray& original, bslma::Allocator *basicAllocator = 0);

    /// Create a `JsonArray` having the same value as the specified
    /// `original` object by moving (in constant time) the contents of
    /// `original` to the new `JsonArray` object.  The allocator associated
    /// with `original` is propagated for use in the newly-created
    /// `JsonArray` object.  `original` is left in a valid but unspecified
    /// state.
    JsonArray(bslmf::MovableRef<JsonArray> original) BSLS_KEYWORD_NOEXCEPT;

    /// Create a `JsonArray` having the same value as the specified
    /// `original` object that uses the specified `basicAllocator` to supply
    /// memory.  The contents of `original` are moved (in constant time) to
    /// the new `JsonArray` object if
    /// `basicAllocator == original.allocator()`, and are move-inserted (in
    /// linear time) using `basicAllocator` otherwise.  `original` is left
    /// in a valid but unspecified state.
    JsonArray(bslmf::MovableRef<JsonArray>  original,
              bslma::Allocator             *basicAllocator);

    /// Create a `JsonArray` and insert (in order) each `Json` object in the
    /// range starting at the specified `first` element, and ending
    /// immediately before the specified `last` element.  Optionally specify
    /// `basicAllocator` used to supply memory.  If `basicAllocator` is not
    /// specified, the currently installed default allocator is used.  Throw
    /// `bsl::length_error` if the number of elements in `[first .. last)`
    /// exceeds the value returned by the method `maxSize`.  The (template
    /// parameter) type `INPUT_ITERATOR` shall meet the requirements of an
    /// input iterator defined in the c++11 standard [24.2.3] providing
    /// access to values of a type convertible to `Json`, and `Json` must be
    /// `emplace-constructible` form `*i`, where `i` is a dereferenceable
    /// iterator in the range `[first .. last)`.  The behavior is undefined
    /// unless `first` and `last` refer to a range of valid values where
    /// `first` is at a position at or before `last`.
    template <class INPUT_ITERATOR>
    JsonArray(INPUT_ITERATOR    first,
              INPUT_ITERATOR    last,
              bslma::Allocator *basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    /// Create a `JsonArray` and insert (in order) each `Json` object in the
    /// specified `elements` initializer list.  Optionally specify a
    /// `basicAllocator` used to supply memory.  If `basicAllocator` is not
    /// specified, the currently installed default allocator is used.
    JsonArray(std::initializer_list<Json_Initializer>  elements,
              bslma::Allocator                        *basicAllocator = 0);
                                                                    // IMPLICIT
#endif

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.  If
    /// an exception is thrown, `*this` is left in a valid but unspecified
    /// state.
    JsonArray& operator=(const JsonArray& rhs);

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.  The
    /// contents of `rhs` are moved (in constant time) to this JsonArray if
    /// `allocator() == rhs.allocator()`; otherwise, all elements in this
    /// `JsonArray` are either destroyed or move-assigned to and each
    /// additional element in `rhs` is move-inserted into this JsonArray.
    /// `rhs` is left in a valid but unspecified state.
    JsonArray& operator=(bslmf::MovableRef<JsonArray> rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    /// Assign to this object the value resulting from first clearing this
    /// `JsonArray` and then inserting (in order) each `Json` object in the
    /// specified `initializer` initializer list.  Return a reference to
    /// `*this`.  If an exception is thrown, `*this` is left in a valid but
    /// unspecified state.
    JsonArray& operator=(std::initializer_list<Json_Initializer> initializer);

    /// Assign to this object the value resulting from first clearing this
    /// JsonArray and then inserting (in order) each `Json` object in the
    /// specified `initializer` initializer list.  If an exception is
    /// thrown, `*this` is left in a valid but unspecified state.
    JsonArray& assign(std::initializer_list<Json_Initializer> initializer);
#endif

    /// Return a reference providing modifiable access to the element at the
    /// specified `index` in this `JsonArray`.  The behavior is undefined
    /// unless `index < size()`.
    Json& operator[](bsl::size_t index);

    /// Assign to this object the value resulting from first clearing this
    /// `JsonArray` and then inserting (in order) each `Json` object in the
    /// range starting at the specified `first` element, and ending
    /// immediately before the specified `last` element.  If an exception is
    /// thrown, `*this` is left in a valid but unspecified state.  Throw
    /// `bsl::length_error` if `distance(first,last) > maxSize()`.  The
    /// (template parameter) type `INPUT_ITERATOR` shall meet the
    /// requirements of an input iterator defined in the c++11 standard
    /// [24.2.3] providing access to values of a type convertible to `Json`,
    /// and `Json` must be `emplace-constructible` form `*i`, where `i` is a
    /// dereferenceable iterator in the range `[first .. last)`.  The
    /// behavior is undefined unless `first` and `last` refer to a range of
    /// valid values where `first` is at a position at or before `last`.
    template <class INPUT_ITERATOR>
    JsonArray& assign(INPUT_ITERATOR first, INPUT_ITERATOR last);

    // BDE_VERIFY pragma: -FABC01

    /// Return an iterator providing modifiable access to the first element
    /// in this `JsonArray, or the past-the-end iterator if this `JsonArray'
    /// is empty.
    Iterator begin();

    /// Return the past-the-end iterator providing modifiable access to this
    /// `JsonArray`.
    Iterator end();

    /// Return a reference providing modifiable access to the first element
    /// in this `JsonArray`.  The behavior is undefined unless this
    /// `JsonArray` is not empty.
    Json& front();

    /// Return a reference providing modifiable access to the last element
    /// in this `JsonArray`.  The behavior is undefined unless this
    /// `JsonArray` is not empty.
    Json& back();

    // BDE_VERIFY pragma: +FABC01

    /// Remove all elements from this `JsonArray` making its size 0.  Note
    /// that although this `JsonArray` is empty after this method returns,
    /// it preserves the same capacity it had before the method was called.
    void clear();

    /// Remove from this `JsonArray` the element at the specified `index`,
    /// and return an iterator providing modifiable access to the element
    /// immediately following the removed element, or the position returned
    /// by the method `end` if the removed element was the last in the
    /// sequence.  The behavior is undefined unless `index` is in the range
    /// `[0 .. size())`.
    Iterator erase(bsl::size_t index);

    /// Remove from this `JsonArray` the element at the specified
    /// `position`, and return an iterator providing modifiable access to
    /// the element immediately following the removed element, or the
    /// position returned by the method `end` if the removed element was the
    /// last in the sequence.  The behavior is undefined unless `position`
    /// is an iterator in the range `[cbegin() .. cend())`.
    Iterator erase(ConstIterator position);

    /// Remove from this `JsonArray` the sequence of elements starting at
    /// the specified `first` position and ending before the specified
    /// `last` position, and return an iterator providing modifiable access
    /// to the element immediately following the last removed element, or
    /// the position returned by the method `end` if the removed elements
    /// were last in the sequence.  The behavior is undefined unless `first`
    /// is an iterator in the range `[cbegin() .. cend()]` (both endpoints
    /// included) and `last` is an iterator in the range `[first .. cend()]`
    /// (both endpoints included).
    Iterator erase(ConstIterator first, ConstIterator last);

    /// Insert at the specified `index` in this JsonArray a copy of the
    /// specified `json`, and return an iterator referring to the newly
    /// inserted element.  If an exception is thrown, `*this` is unaffected.
    /// Throw `bsl::length_error` if `size() == maxSize()`.  The behavior is
    /// undefined unless `index` is in the range `[0 .. size()]`.
    Iterator insert(bsl::size_t index, const Json& json);

    /// Insert at the specified `index` in this JsonArray the specified
    /// move-insertable `json`, and return an iterator referring to the
    /// newly inserted element.  `json` is left in a valid but unspecified
    /// state.  If an exception is thrown, `this` is unaffected.  Throw
    /// `bsl::length_error` if `size() == maxSize()`.  The behavior is
    /// undefined unless `index` is in the range `[0 .. size()]` (both
    /// endpoints included).
    Iterator insert(bsl::size_t index, bslmf::MovableRef<Json> json);

    /// Insert at the specified `index` in this JsonArray the values in the
    /// range starting at the specified `first` element, and ending
    /// immediately before the specified `last` element.  Return an iterator
    /// referring to the first newly inserted element.  If an exception is
    /// thrown, `*this` is unaffected.  Throw `bsl::length_error` if
    /// `size() + distance(first, last) > maxSize()`.  The (template
    /// parameter) type `INPUT_ITERATOR` shall meet the requirements of an
    /// input iterator defined in the C++11 standard [24.2.3] providing
    /// access to values of a type convertible to `Json`, and `Json` must be
    /// `emplace-constructible` from `*i` into this `JsonArray, where `i' is
    /// a dereferenceable iterator in the range `[first .. last)`.  The
    /// behavior is undefined unless `index` is in the range `[0 .. size()]`
    /// (both endpoints included), and `first` and `last` refer to a range
    /// of valid values where `first` is at a position at or before `last`.
    template <class INPUT_ITERATOR>
    Iterator insert(bsl::size_t    index,
                    INPUT_ITERATOR first,
                    INPUT_ITERATOR last);

    /// Insert at the specified `position` in this `JsonArray` a copy of the
    /// specified `json`, and return an iterator referring to the newly
    /// inserted element.  If an exception is thrown, `*this` is unaffected.
    /// Throw `bsl::length_error` if `size() == maxSize()`.  The behavior is
    /// undefined unless `position` is an iterator in the range
    /// `[begin() .. end()]` (both endpoints included).
    Iterator insert(ConstIterator position, const Json& json);

    /// Insert at the specified `position` in this `JsonArray` the specified
    /// move-insertable `json`, and return an iterator referring to the
    /// newly inserted element.  `json` is left in a valid but unspecified
    /// state.  If an exception is thrown, `this` is unaffected.  Throw
    /// `bsl::length_error` if `size() == maxSize()`.  The behavior is
    /// undefined unless `position` is an iterator in the range
    /// `[begin() .. end()]` (both endpoints included).
    Iterator insert(ConstIterator position, bslmf::MovableRef<Json> json);

    /// Insert at the specified `position` in this `JsonArray` the values in
    /// the range starting at the specified `first` element, and ending
    /// immediately before the specified `last` element.  Return an iterator
    /// referring to the first newly inserted element.  If an exception is
    /// thrown, `*this` is unaffected.  Throw `bsl::length_error` if
    /// `size() + distance(first, last) > maxSize()`.  The (template
    /// parameter) type `INPUT_ITERATOR` shall meet the requirements of an
    /// input iterator defined in the C++11 standard [24.2.3] providing
    /// access to values of a type convertible to `value_type`, and
    /// `value_type` must be `emplace-constructible` from `*i` into this
    /// JsonArray, where `i` is a dereferenceable iterator in the range
    /// `[first .. last)`.  The behavior is undefined unless `position` is
    /// an iterator in the range `[begin() .. end()]` (both endpoints
    /// included), and `first` and `last` refer to a range of valid values
    /// where `first` is at a position at or before `last`.
    template <class INPUT_ITERATOR>
    Iterator insert(ConstIterator  position,
                    INPUT_ITERATOR first,
                    INPUT_ITERATOR last);

    /// Erase the last element from this `JsonArray`.  The behavior is
    /// undefined if this JsonArray is empty.
    void popBack();

    /// Append to the end of this `JsonArray` a copy of the specified
    /// `json`/`array`/`object`/`number` and return a non-`const` reference
    /// to this array.  If an exception is thrown, `*this` is unaffected.
    /// Throw `bsl::length_error` if `size() == maxSize()`.
    JsonArray& pushBack(const Json&       json);
    JsonArray& pushBack(const JsonArray&  array);
    JsonArray& pushBack(const JsonObject& object);
    JsonArray& pushBack(const JsonNumber& number);

    /// Append to the end of this `JsonArray` the specified move-insertable
    /// `json`/`array`/`object`/`number` and return a non-`const` reference
    /// to this array.  `json`/`array`/`object`/`number` is left in a valid
    /// but unspecified state.  If an exception is thrown, '*this' is
    /// unaffected.  Throw `bsl::length_error` if `size() == maxSize()`.
    JsonArray& pushBack(bslmf::MovableRef<Json>       json);
    JsonArray& pushBack(bslmf::MovableRef<JsonArray>  array);
    JsonArray& pushBack(bslmf::MovableRef<JsonObject> object);
    JsonArray& pushBack(bslmf::MovableRef<JsonNumber> number);


    /// Append to the end of this `JsonArray` a copy of the specified
    /// `value` and return a non-`const` reference to this array.  If an
    /// exception is thrown, `*this` is unaffected.  Throw
    /// `bsl::length_error` if `size() == maxSize()`.
    JsonArray& pushBack(const JsonNull&    value);
    JsonArray& pushBack(bool               value);
    JsonArray& pushBack(int                value);
    JsonArray& pushBack(unsigned int       value);
    JsonArray& pushBack(long               value);
    JsonArray& pushBack(unsigned long      value);
    JsonArray& pushBack(long long          value);
    JsonArray& pushBack(unsigned long long value);
    JsonArray& pushBack(float              value);
    JsonArray& pushBack(double             value);
    JsonArray& pushBack(bdldfp::Decimal64  value);

    /// Append to the end of this `JsonArray` a copy of the specified
    /// `string` and return a non-`const` reference to this array.  If an
    /// exception is thrown, `*this` is unaffected.  Throw
    /// `bsl::length_error` if `size() == maxSize()`.
    JsonArray& pushBack(const char              *string);
    JsonArray& pushBack(const bsl::string_view&  string);

    /// Append to the end of this `JsonArray` an element having the value of
    /// the specified `string` by moving (in constant time) the contents of
    /// `string` to the new element and return a non-`const` reference to
    /// this array.  `string` is left in a valid but unspecified state.
    /// This function does not participate in overload resolution unless the
    /// specified `STRING_TYPE` is `bsl::string`.  The behavior is undefined
    /// unless `string` is valid UTF-8 (see `bdlde::Utf8Util::isValid`).
    template <class STRING_TYPE>
    typename bsl::enable_if<bsl::is_same<STRING_TYPE,
                                         bsl::string>::value,
                            JsonArray&>::type
    pushBack(BSLMF_MOVABLEREF_DEDUCE(STRING_TYPE) string);

    /// Change the size of this JsonArray to the specified `count`.
    /// If `count < size()`, the elements in the range `[count .. size())` are
    /// erased `json` is ignored, and this method does not throw.  If
    /// `count > size()`  and an exception is thrown, `*this` is unaffected.
    /// Throw `bsl::length_error` if `count > maxSize()`.
    void resize(bsl::size_t count);

    /// Change the size of this JsonArray to the specified `count`,
    /// inserting copies of the specified `json` at the end if
    /// `count > size()`.  If `count < size()`, the elements in the range
    /// `[count .. size())` are erased `json` is ignored, and this method
    /// does not throw.  If `count > size()` and an exception is thrown,
    /// `*this` is unaffected.  Throw `bsl::length_error` if
    /// `count > maxSize()`.
    void resize(bsl::size_t count, const Json& json);

    /// Exchange the value of this object with that of the specified `other`
    /// object.  If an exception is thrown, both objects are left in valid
    /// but unspecified states.  This operation guarantees O[1] complexity.
    /// The behavior is undefined unless this object was created with the
    /// same allocator as `other`.
    void swap(JsonArray& other);

    // ACCESSORS

    /// Return a reference providing non-modifiable access to the element at
    /// the specified `index` in this `JsonArray`.  The behavior is
    /// undefined unless `index < size()`.
    const Json& operator[](bsl::size_t index) const;

    // BDE_VERIFY pragma: -FABC01

    /// Return an iterator providing non-modifiable access to the first
    /// element in this `JsonArray`, and the past-the-end iterator if this
    /// `JsonArray` is empty.
    ConstIterator  begin() const BSLS_KEYWORD_NOEXCEPT;
    ConstIterator cbegin() const BSLS_KEYWORD_NOEXCEPT;

    /// Return the past-the-end iterator providing non-modifiable access to
    /// this `JsonArray`.
    ConstIterator  end() const BSLS_KEYWORD_NOEXCEPT;
    ConstIterator cend() const BSLS_KEYWORD_NOEXCEPT;

    /// Return a reference providing non-modifiable access to the first
    /// element in this `JsonArray`.  The behavior is undefined unless this
    /// `JsonArray` is not empty.
    const Json& front() const;

    /// Return a reference providing non-modifiable access to the last
    /// element in this `JsonArray`.  The behavior is undefined unless this
    /// `JsonArray` is not empty.
    const Json& back() const;

    // BDE_VERIFY pragma: +FABC01

    /// Return `true` if this `JsonArray` has size 0, and `false` otherwise.
    bool empty() const;

    /// Return the number of elements in this `JsonArray`.
    bsl::size_t size() const;

                                  // Aspects

    /// Return the allocator used by this object to allocate memory.
    bslma::Allocator *allocator() const BSLS_KEYWORD_NOEXCEPT;

    /// Return a theoretical upper bound on the largest number of elements
    /// that this `JsonArray` could possibly hold.  Note that there is no
    /// guarantee that the `JsonArray` can successfully grow to the returned
    /// size, or even close to that size without running out of resources.
    /// Also note that requests to create a `JsonArray` longer than this
    /// number of elements are guaranteed to raise a `bsl::length_error`
    /// exception.
    bsl::size_t maxSize() const BSLS_KEYWORD_NOEXCEPT;

    /// Write the value of this object to the specified output `stream` in a
    /// human-readable format, and return a reference to `stream`.
    /// Optionally specify an initial indentation `level`, whose absolute
    /// value is incremented recursively for nested objects.  If `level` is
    /// specified, optionally specify `spacesPerLevel`, whose absolute value
    /// indicates the number of spaces per indentation level for this and
    /// all of its nested objects.  If `level` is negative, suppress
    /// indentation of the first line.  If `spacesPerLevel` is negative,
    /// format the entire output on one line, suppressing all but the
    /// initial indentation (as governed by `level`).  If `stream` is not
    /// valid on entry, this operation has no effect.  Note that this
    /// human-readable format is not fully specified, and can change without
    /// notice.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
};

// FREE OPERATORS

/// Write the value of the specified `object` to the specified output
/// `stream` in a single-line format, and return a reference to `stream`.
/// If `stream` is not valid on entry, this operation has no effect.  Note
/// that this human-readable format is not fully specified, can change
/// without notice, and is logically equivalent to:
/// ```
/// print(stream, 0, -1);
/// ```
bsl::ostream& operator<<(bsl::ostream& stream, const JsonArray& object);

/// Return `true` if the specified `lhs` and `rhs` objects have the same
/// value, and `false` otherwise.  Two `JsonArray` objects `lhs` and `rhs`
/// have the same value if they have the same number of elements, and each
/// element in the ordered sequence of elements of `lhs` has the same value
/// as the corresponding element in the ordered sequence of elements of
/// `rhs`.
bool operator==(const JsonArray& lhs, const JsonArray& rhs);

/// Return `false` if the specified `lhs` and `rhs` objects have the same
/// value, and `true` otherwise.  Two `JsonArray` objects `lhs` and `rhs`
/// have the same value if they have the same number of elements, and each
/// element in the ordered sequence of elements of `lhs` has the same value
/// as the corresponding element in the ordered sequence of elements of
/// `rhs`.
bool operator!=(const JsonArray& lhs, const JsonArray& rhs);

/// Invoke the specified `hashAlg` on the attributes of the specified
/// `object`.
template <class HASHALG>
void hashAppend(HASHALG& hashAlg, const JsonArray& object);

/// Exchange the value of the specified `a` `JsonArray` with that of the
/// specified `b` `JsonArray`.  This function provides the no-throw
/// exception-safety guarantee.  This operation has O[1] complexity if `a`
/// was created with the same allocator as `b`; otherwise, it has O[n+m]
/// complexity, where n and m are the number of elements in `a` and `b`,
/// respectively.
void swap(JsonArray& a, JsonArray& b);

                              // ================
                              // class JsonObject
                              // ================

/// This type is designed to replicate much of the standard associative
/// container interface, eliding interfaces that are less relevant for a
/// non-generic container, like hasher and comparator access, emplacement,
/// nodes, capacity management, etc.
class JsonObject {

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

    /// Create an empty `JsonObject`.  Optionally specify the
    /// `basicAllocator` used to supply memory.  If `basicAllocator` is not
    /// specified, the currently installed default allocator is used to
    /// supply memory.
    JsonObject();
    explicit JsonObject(bslma::Allocator *basicAllocator);

    /// Create a `JsonObject` having the same value as the specified
    /// `original`.  Optionally specify the `basicAllocator` used to supply
    /// memory.  If `basicAllocator` is not specified, the currently
    /// installed default allocator is used to supply memory.
    JsonObject(const JsonObject&  original,
               bslma::Allocator  *basicAllocator = 0);

    /// Create a `JsonObject` having the same value as the specified
    /// `original` object by moving (in constant time) the contents of
    /// `original` to the new `JsonObject`.  The allocator associated with
    /// `original` is propagated for use in the newly-created `JsonObject`.
    /// `original` is left in a valid but unspecified state.
    JsonObject(bslmf::MovableRef<JsonObject> original) BSLS_KEYWORD_NOEXCEPT;

    /// Create a `JsonObject` having the same value as the specified
    /// `original`.  Use the specified `basicAllocator` to supply memory.
    /// If `basicAllocator == original.allocator()` the value of `original`
    /// will be moved (in constant time) to the newly-created `JsonObject`,
    /// and `original` will be left in a valid but unspecified state.
    /// Otherwise, `original` is copied, and `basicAllocator` used to supply
    /// memory.
    JsonObject(bslmf::MovableRef<JsonObject>  original,
               bslma::Allocator              *basicAllocator);

    /// Create an empty `JsonObject`, and then create a `Json` object for
    /// each iterator in the range starting at the specified `first`
    /// iterator and ending immediately before the specified `last`
    /// iterator, by converting from the object referred to by each
    /// iterator.  Insert into this `JsonObject` each such object, ignoring
    /// those having a key that appears earlier in the sequence.  Optionally
    /// specify a `basicAllocator` used to supply memory.  If
    /// `basicAllocator` is not supplied, the currently installed default
    /// allocator is used to supply memory.  The (template parameter) type
    /// `INPUT_ITERATOR` shall meet the requirements of an input iterator
    /// defined in the C++11 standard [24.2.3] providing access to values of
    /// a type convertible to `Member`.  The behavior is undefined unless
    /// `first` and `last` refer to a sequence of valid values where `first`
    /// is at a position at or before `last`, and all keys of all `Member`
    /// objects inserted are valid UTF-8 (see `bdlde::Utf8Util::isValid`).
    template <class INPUT_ITERATOR>
    JsonObject(INPUT_ITERATOR    first,
               INPUT_ITERATOR    last,
               bslma::Allocator *basicAllocator = 0);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY)
    /// Create an `JsonObject` containing a single member consisting of the
    /// specified `key` and a value constructed from the specified `init`.
    /// The behavior is undefined unless `key` is valid UTF-8 (see
    /// `bdlde::Utf8Util::isValid`).
    JsonObject(const std::string_view&  key,
               const Json_Initializer&  init,
               bslma::Allocator        *basicAllocator = 0);        // IMPLICIT
#endif
    /// Create an empty `JsonObject`, and then create a `Json` object for
    /// each in the range specified by `members` argument, ignoring elements
    /// having a key that appears earlier in the sequence.  Optionally
    /// specify the `basicAllocator` used to supply memory.  If
    /// `basicAllocator` is not specified, the currently installed default
    /// allocator is used to supply memory.  The behavior is undefined
    /// unless the keys of all `Json_MemberInitializer` objects in `members`
    /// have a valid UTF-8 `key()` (see `bdlde::Utf8Util::isValid`).
    JsonObject(
            std::initializer_list<Json_MemberInitializer>  members,
            bslma::Allocator                              *basicAllocator = 0);
                                                                    // IMPLICIT
#endif

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.
    JsonObject& operator=(const JsonObject& rhs);

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.  The
    /// contents of `rhs` are moved (in constant time) to this `JsonObject`
    /// if `allocator() == rhs.allocator()`; otherwise, all elements in this
    /// container are either destroyed or move-assigned to, and each
    /// additional element in `rhs`, if any, is move-inserted into this
    /// `JsonObject`.  `rhs` is left in a valid but unspecified state.
    JsonObject& operator=(bslmf::MovableRef<JsonObject> rhs);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    /// Assign to this object the value resulting from first clearing this
    /// `JsonObject` and then inserting (in order) each `Member` object in
    /// the specified `members` initializer list.  Return a reference to
    /// `*this`.  If an exception is thrown, `*this` is left in a valid but
    /// unspecified state.
    JsonObject& operator=(std::initializer_list<Json_MemberInitializer>
                                                                      members);
#endif

    /// Return a reference providing modifiable access to the `Json` object
    /// associated with the specified `key` in this `JsonObject`; if this
    /// `JsonObject` does not already contain a `Json` object associated
    /// with `key`, first insert a new default-constructed `Json` object
    /// associated with `key`.  The behavior is undefined unless `key` is
    /// valid UTF-8 (see `bdlde::Utf8Util::isValid`).
    Json& operator[](const bsl::string_view& key);

    // BDE_VERIFY pragma: -FABC01

    /// Return an iterator providing modifiable access to the first `Member`
    /// object in the sequence of `Member` objects maintained by this
    /// `JsonObject`, or the `end` iterator if this `JsonObject` is empty.
    Iterator begin() BSLS_KEYWORD_NOEXCEPT;

    /// Return an iterator providing modifiable access to the past-the-end
    /// position in the sequence of `Member` objects maintained by this
    /// `JsonObject`.
    Iterator end() BSLS_KEYWORD_NOEXCEPT;

    // BDE_VERIFY pragma: +FABC01

    /// Remove all entries from this `JsonObject`.  Note that this
    /// `JsonObject` will be empty after calling this method, but allocated
    /// memory may be retained for future use.
    void clear() BSLS_KEYWORD_NOEXCEPT;

    /// Remove from this `JsonObject` the `Member` object having the
    /// specified `key`, if it exists, and return 1; otherwise (there is no
    /// object with a key equivalent to `key` in this `JsonObject`) return 0
    /// with no other effect.  This method invalidates only iterators and
    /// references to the removed element and previously saved values of the
    /// `end()` iterator, and preserves the relative order of the elements
    /// not removed.
    bsl::size_t erase(const bsl::string_view& key);

    /// Remove from this unordered map the `value_type` object at the
    /// specified `position`, and return an iterator referring to the
    /// element immediately following the removed element, or to the
    /// past-the-end position if the removed element was the last element in
    /// the sequence of elements maintained by this unordered map.  This
    /// method invalidates only iterators and references to the removed
    /// element and previously saved values of the `end()` iterator, and
    /// preserves the relative order of the elements not removed.  The
    /// behavior is undefined unless `position` refers to a `value_type`
    /// object in this unordered map.
    Iterator erase(Iterator position);
    Iterator erase(ConstIterator position);

    /// Return an iterator providing modifiable access to the `Member`
    /// object in this `JsonObject` with a key equivalent to the specified
    /// `key`, if such an entry exists, and the past-the-end iterator
    /// (`end`) otherwise.
    Iterator find(const bsl::string_view& key);

    /// Insert the specified `member` into this `JsonObject` if the key (the
    /// `first` element) of the object referred to by `value` does not
    /// already exist in this `JsonObject`; otherwise, this method has no
    /// effect.  Return a `pair` whose `first` member is an iterator
    /// referring to the (possibly newly inserted) `value_type` object in
    /// this `JsonObject` whose key is equivalent to that of the object to
    /// be inserted, and whose `second` member is `true` if a new value was
    /// inserted, and `false` if a value having an equivalent key was
    /// already present.  The behavior is undefined unless `member.first` is
    /// valid UTF-8 (see `bdlde::Utf8Util::isValid`).
    bsl::pair<Iterator, bool> insert(const Member& member);

    /// Insert the specified `member` into this `JsonObject` if the key (the
    /// `first` element) of the object referred to by `member` does not
    /// already exist in this `JsonObject`; otherwise, this method has no
    /// effect.  Return a `pair` whose `first` member is an iterator
    /// referring to the (possibly newly inserted) `member` object in this
    /// `JsonObject` whose key is the equivalent to that of the object to be
    /// inserted, and whose `second` member is `true` if a new value was
    /// inserted, and `false` otherwise.  The behavior is undefined unless
    /// `member.first` is valid UTF-8 (see `bdlde::Utf8Util::isValid`).
    bsl::pair<Iterator, bool> insert(bslmf::MovableRef<Member> member);

    /// Create a `member` object for each iterator in the range starting at
    /// the specified `first` iterator and ending immediately before the
    /// specified `last` iterator, by converting from the object referred to
    /// by each iterator.  Insert into this `JsonObject` each such object
    /// whose key is not already contained.  The (template parameter) type
    /// `INPUT_ITERATOR` shall meet the requirements of an input iterator
    /// defined in the C++11 standard [24.2.3] providing access to values of
    /// a type convertible to `Member`.  The behavior is undefined unless
    /// `first` and `last` refer to a sequence of value values where `first`
    /// is at a position at or before `last`.  The behavior is undefined
    /// unless the keys of all `Member` objects inserted are valid UTF-8
    /// (see `bdlde::Utf8Util::isValid`).
    template <class INPUT_ITERATOR>
    typename bsl::enable_if<!bsl::is_convertible<INPUT_ITERATOR,
                                                 bsl::string_view>::value,
                            JsonObject&>::type
    insert(INPUT_ITERATOR first, INPUT_ITERATOR last);

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
    /// Create a `Member` object for each element in the specified
    /// `members`.  Insert into this `JsonObject` each such object whose key
    /// is not already contained.  The behavior is undefined unless the keys
    /// of all `Member` objects inserted are valid UTF-8 (see
    /// `bdlde::Utf8Util::isValid`).
    JsonObject& insert(std::initializer_list<Json_MemberInitializer> members);
#endif

    /// Insert into this `JsonObject` a `Member` constructed from the
    /// specified `key` and the specified `value`, respectively, if `key`
    /// does not already exist in this `JsonObject`; otherwise, this method
    /// has no effect.  Return a `pair` whose `first` member is an iterator
    /// referring to the (possibly newly inserted) `Member` object in this
    /// `JsonObject` whose key is the equivalent to that of the object to be
    /// inserted, and whose `second` member is `true` if a new value was
    /// inserted, and `false` otherwise.  The behavior is undefined unless
    /// `key` is valid UTF-8 (see `bdlde::Utf8Util::isValid`).
    template <class VALUE>
    bsl::pair<Iterator, bool> insert(
                               const bsl::string_view&                  key,
                               BSLS_COMPILERFEATURES_FORWARD_REF(VALUE) value);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
    /// Insert into this `JsonObject` a `Member` constructed from the
    /// specified `key` and the specified `init`, respectively, if `key`
    /// does not already exist in this `JsonObject`; otherwise, this method
    /// has no effect.  Return a `pair` whose `first` member is an iterator
    /// referring to the (possibly newly inserted) `Member` object in this
    /// `JsonObject` whose key is the equivalent to that of the object to be
    /// inserted, and whose `second` member is `true` if a new value was
    /// inserted, and `false` otherwise.  The behavior is undefined unless
    /// `key` is valid UTF-8 (see `bdlde::Utf8Util::isValid`).
    bsl::pair<Iterator, bool> insert(const bsl::string_view& key,
                                     const Json_Initializer& init);
#endif

    /// Exchange the value of this object with that of the specified `other`
    /// object.  If an exception is thrown, both objects are left in valid
    /// but unspecified states.  This operation guarantees O[1] complexity.
    /// The behavior is undefined unless this object was created with the
    /// same allocator as `other`.
    void swap(JsonObject& other);

    // ACCESSORS

    /// Return a reference providing non-modifiable access to the `Json`
    /// object associated with the specified `key` in this `JsonObject`.
    /// The behavior is undefined unless `key` is valid UTF-8 (see
    /// `bdlde::Utf8Util::isValid`) and this `JsonObject` already contains a
    /// `Json` object associated with `key`.
    const Json& operator[](const bsl::string_view& key) const;

    // BDE_VERIFY pragma: -FABC01

    /// Return an iterator providing non-modifiable access to the first
    /// `Member` object in the sequence of `Member` objects maintained by
    /// this `JsonObject`, or the `end` iterator if this `JsonObject` is
    /// empty.
    ConstIterator  begin() const BSLS_KEYWORD_NOEXCEPT;
    ConstIterator cbegin() const BSLS_KEYWORD_NOEXCEPT;

    /// Return an iterator providing non-modifiable access to the
    /// past-the-end position in the sequence of `Member` objects maintained
    /// by this `JsonObject`.
    ConstIterator  end() const BSLS_KEYWORD_NOEXCEPT;
    ConstIterator cend() const BSLS_KEYWORD_NOEXCEPT;

    // BDE_VERIFY pragma: +FABC01

    /// Return `true` if there is a `Member` object in this `JsonObject`
    /// with a key equivalent to the specified `key`, and return `false`
    /// otherwise.
    bool contains(const bsl::string_view& key) const;

    /// Return `true` if this `JsonObject` contains no elements, and `false`
    /// otherwise.
    bool empty() const BSLS_KEYWORD_NOEXCEPT;

    /// Return an iterator providing non-modifiable access to the `Member`
    /// object in this `JsonObject` with a key equivalent to the specified
    /// `key`, if such an entry exists, and the past-the-end iterator
    /// (`end`) otherwise.
    ConstIterator find(const bsl::string_view& key) const;

    /// Return the number of elements in this `JsonObject`.
    bsl::size_t size() const BSLS_KEYWORD_NOEXCEPT;

                                  // Aspects

    /// Return the allocator used by this object to allocate memory.
    bslma::Allocator *allocator() const BSLS_KEYWORD_NOEXCEPT;

    /// Write the value of this object to the specified output `stream` in a
    /// human-readable format, and return a reference to `stream`.
    /// Optionally specify an initial indentation `level`, whose absolute
    /// value is incremented recursively for nested objects.  If `level` is
    /// specified, optionally specify `spacesPerLevel`, whose absolute value
    /// indicates the number of spaces per indentation level for this and
    /// all of its nested objects.  If `level` is negative, suppress
    /// indentation of the first line.  If `spacesPerLevel` is negative,
    /// format the entire output on one line, suppressing all but the
    /// initial indentation (as governed by `level`).  If `stream` is not
    /// valid on entry, this operation has no effect.  Note that this
    /// human-readable format is not fully specified, and can change without
    /// notice.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
};

// FREE OPERATORS

/// Write the value of the specified `object` to the specified output
/// `stream` in a single-line format, and return a reference to `stream`.
/// If `stream` is not valid on entry, this operation has no effect.  Note
/// that this human-readable format is not fully specified, can change
/// without notice, and is logically equivalent to:
/// ```
/// print(stream, 0, -1);
/// ```
bsl::ostream& operator<<(bsl::ostream& stream, const JsonObject& object);

/// Return `true` if the specified `lhs` and `rhs` objects have the same
/// value, and `false` otherwise.  Two `JsonObject` objects have the same
/// value if they have the same number of `Member` objects, and for each
/// `Member` object that is contained in `lhs` there is a key-value pair
/// contained in `rhs` having the same value, and vice versa.
bool operator==(const JsonObject& lhs, const JsonObject& rhs);

/// Return `false` if the specified `lhs` and `rhs` objects have the same
/// value, and `true` otherwise.  Two `JsonObject` objects have the same
/// value if they have the same number of `Member` objects, and for each
/// `Member` object that is contained in `lhs` there is a key-value pair
/// contained in `rhs` having the same value, and vice versa.
bool operator!=(const JsonObject& lhs, const JsonObject& rhs);

/// Invoke the specified `hashAlg` on the attributes of the specified
/// `object`.
template <class HASHALG>
void hashAppend(HASHALG& hashAlg, const JsonObject& object);

/// Exchange the value of the specified `a` `JsonObject` with that of the
/// specified `b` `JsonObject`.  This function provides the no-throw
/// exception-safety guarantee.  This operation has O[1] complexity if `a`
/// was created with the same allocator as `b`; otherwise, it has O[n+m]
/// complexity, where n and m are the number of elements in `a` and `b`,
/// respectively.
void swap(JsonObject& a, JsonObject& b);

                                 // ==========
                                 // class Json
                                 // ==========

/// This type is designed to be a thin wrapper around a variant of the
/// possible JSON types, using a BDE-style variant interface.
class Json {

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

    /// Create a `Json` object having the type (and the singleton value) of
    /// `JsonNull`.  Optionally specify the `basicAllocator` used to supply
    /// memory.  If `basicAllocator` is not specified, the currently
    /// installed default allocator is used to supply memory.
    Json();
    explicit Json(bslma::Allocator *basicAllocator);

    /// Create a `Json` object having the same value as the specified
    /// `original`.  Optionally specify the `basicAllocator` used to supply
    /// memory.  If `basicAllocator` is not specified, the currently
    /// installed default allocator is used to supply memory.
    Json(const Json& original, bslma::Allocator *basicAllocator = 0);

    /// Create a `Json` object having the same value as the specified
    /// `original` object by moving (in constant time) the contents of
    /// `original` to the new `JsonObject`.  The allocator associated with
    /// `original` is propagated for use in the newly-created `Json` object.
    /// `original` is left in a valid but unspecified state.
    Json(bslmf::MovableRef<Json> original);

    /// Create a `Json` object having the same value as the specified
    /// `original`.  Use the specified `basicAllocator` to supply memory.
    Json(bslmf::MovableRef<Json> original, bslma::Allocator *basicAllocator);

    /// Create a `Json` object having the type `bool` and the same value as
    /// the specified `boolean`.  Optionally specify the `basicAllocator`
    /// used to supply memory.  If `basicAllocator` is not specified, the
    /// currently installed default allocator is used to supply memory.
    explicit Json(bool boolean, bslma::Allocator *basicAllocator = 0);

    /// Create a `Json` object having the type `JsonNull` and the same value
    /// as the specified `null`.  Optionally specify the `basicAllocator`
    /// used to supply memory.  If `basicAllocator` is not specified, the
    /// currently installed default allocator is used to supply memory.
    explicit Json(const JsonNull& null, bslma::Allocator *basicAllocator = 0);

    /// Create a `Json` object having the type `JsonArray`/`JsonObject`/
    /// `JsonNumber` and the value of the specified
    /// `array`/`object`/`number`.  Optionally specify `basicAllocator`
    /// used to supply memory.  If `basicAllocator` is not specified, the
    /// currently installed default allocator is used to supply memory.
             Json(const JsonArray&   array,
                  bslma::Allocator  *basicAllocator = 0);           // IMPLICIT
             Json(const JsonObject&  object,
                  bslma::Allocator  *basicAllocator = 0);           // IMPLICIT
    explicit Json(const JsonNumber&  number,
                  bslma::Allocator  *basicAllocator = 0);

    /// Create a `Json` object having the type `JsonArray`/`JsonObject`/
    /// `JsonNumber` and the value of the specified `array`/`object`/
    /// `number` object by moving the contents of `array`/`object`/`number`
    /// to the new object.  Optionally specify `basicAllocator` used to
    /// supply memory.  If `basicAllocator` is not specified, the allocator
    /// associated with `array`/`object`/`number` is propagated for use in
    /// the newly-created `Json` object.  The allocator or `array`/`object`/
    /// `number` is unchanged and is otherwise left in (valid) unspecified
    /// state.
             Json(bslmf::MovableRef<JsonArray>   array,
                  bslma::Allocator              *basicAllocator = 0);
                                                                    // IMPLICIT
             Json(bslmf::MovableRef<JsonObject>  object,
                  bslma::Allocator              *basicAllocator = 0);
                                                                    // IMPLICIT
    explicit Json(bslmf::MovableRef<JsonNumber>  number,
                  bslma::Allocator              *basicAllocator = 0);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
    /// Create a `Json` object having the same structure as the specified
    /// `init`.  Optionally specify the `basicAllocator` used to supply
    /// memory.  If `basicAllocator` is not specified, the currently
    /// installed default allocator is used to supply memory.
    Json(const Json_Initializer& init, bslma::Allocator *basicAllocator = 0);
                                                                    // IMPLICIT

    /// Create a `Json` object having the same structure as the specified
    /// `initializer` list.  Optionally specify the `basicAllocator` used to
    /// supply memory.  If `basicAllocator` is not specified, the currently
    /// installed default allocator is used to supply memory.
    Json(std::initializer_list<Json_Initializer>  initializer,
         bslma::Allocator                        *basicAllocator = 0);
                                                                    // IMPLICIT
#endif

    /// Create a `Json` object having the type `JsonNumber` and the the
    /// specified `value`.  Optionally specify the `basicAllocator` used to
    /// supply memory.  If `basicAllocator` is not specified, the currently
    /// installed default allocator is used to supply memory.
    explicit Json(int                  value,
                  bslma::Allocator    *basicAllocator = 0);
    explicit Json(unsigned int         value,
                  bslma::Allocator    *basicAllocator = 0);
    explicit Json(long                 value,
                  bslma::Allocator    *basicAllocator = 0);
    explicit Json(unsigned long        value,
                  bslma::Allocator    *basicAllocator = 0);
    explicit Json(long long            value,
                  bslma::Allocator    *basicAllocator = 0);
    explicit Json(unsigned long long   value,
                  bslma::Allocator    *basicAllocator = 0);
    explicit Json(float                value,
                  bslma::Allocator    *basicAllocator = 0);
    explicit Json(double               value,
                  bslma::Allocator    *basicAllocator = 0);
    explicit Json(bdldfp::Decimal64    value,
                  bslma::Allocator    *basicAllocator = 0);

    // BDE_VERIFY pragma: -FD06 'string' and 'bsl::string' are too similar
    // BDE_VERIFY pragma: -FD07 'string' and 'bsl::string' are too similar

    /// Create a `Json` object having the type `bsl::string` and the same
    /// value as the specified `string`.  Optionally specify the
    /// `basicAllocator` used to supply memory.  If `basicAllocator` is not
    /// specified, the currently installed default allocator is used to
    /// supply memory.  The behavior is undefined unless `string` is valid
    /// UTF-8 (see `bdlde::Utf8Util::isValid`).
    explicit Json(const char              *string,
                  bslma::Allocator        *basicAllocator = 0);
#ifndef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
    explicit Json(const bsl::string_view&  string,
                  bslma::Allocator        *basicAllocator = 0);
#endif

    // BDE_VERIFY pragma: -IND01 DEDUCE macro confuses bde_verify

    /// Create a `Json` object having the type `bsl::string` and the same
    /// value as the specified `string` by moving (in constant time) the
    /// contents of `string` to the new `Json` object.  Optionally specify
    /// the `basicAllocator` used to supply memory.  If `basicAllocator` is
    /// not specified, the allocator associated with `string` is propagated
    /// for use in the newly-created `Json` object.  `string` is left in a
    /// valid but unspecified state.  This function does not participate in
    /// overload resolution unless the specified `STRING_TYPE` is
    /// `bsl::string`.  The behavior is undefined unless `string` is valid
    /// UTF-8 (see `bdlde::Utf8Util::isValid`).
    template <class STRING_TYPE>
    Json(BSLMF_MOVABLEREF_DEDUCE(STRING_TYPE) string,
         bslma::Allocator                    *basicAllocator = 0,
         typename bsl::enable_if<
             bsl::is_same<STRING_TYPE, bsl::string>::value>::type * = 0);
                                                                    // IMPLICIT

    // BDE_VERIFY pragma: +IND01 DEDUCE macro confuses bde_verify
    // BDE_VERIFY pragma: +FD06 'string' and 'bsl::string' are too similar
    // BDE_VERIFY pragma: +FD07 'string' and 'bsl::string' are too similar

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.  The
    /// value currently held by this variant is destroyed if that value's
    /// type is not the same as the type held by the `rhs` object.
    Json& operator=(const Json& rhs);

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.  The
    /// contents of `rhs` are moved (in constant time) to this `Json` object
    /// if `allocator() == rhs.allocator()`; otherwise they are copied.
    /// `rhs` is left in a valid but unspecified state.
    Json& operator=(bslmf::MovableRef<Json> rhs);

    /// Assign to this object a value of type `JsonNumber` initialized from
    /// the specified `rhs`, and return a reference providing modifiable
    /// access to this object.  The value currently held by this object (if
    /// any) is destroyed if that value's type is not `JsonNumber`.
    Json& operator=(const JsonNumber&  rhs);
    Json& operator=(int                rhs);
    Json& operator=(unsigned int       rhs);
    Json& operator=(long               rhs);
    Json& operator=(unsigned long      rhs);
    Json& operator=(long long          rhs);
    Json& operator=(unsigned long long rhs);
    Json& operator=(float              rhs);
    Json& operator=(double             rhs);
    Json& operator=(bdldfp::Decimal64  rhs);

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.  The
    /// contents of `rhs` are moved (in constant time) to this `Json` object
    /// if `allocator() == rhs.allocator()`; otherwise, they are copied.
    /// `rhs` is left in a valid but unspecified state.  The value currently
    /// held by this object (if any) is destroyed if that value's type is
    /// not `JsonNumber`.
    Json& operator=(bslmf::MovableRef<JsonNumber> rhs);

    /// Assign to this object a value of type `bsl::string` initialized from
    /// the specified `rhs`, and return a reference providing modifiable
    /// access to this object.  The value currently held by this object (if
    /// any) is destroyed if that value's type is not `bsl::string`.  The
    /// behavior is undefined unless `rhs` is valid UTF-8 (see
    /// `bdlde::Utf8Util::isValid`).
    Json& operator=(const char              *rhs);
    Json& operator=(const bsl::string_view&  rhs);

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.  The
    /// contents of `rhs` are moved (in constant time) to this `Json` object
    /// if `allocator() == rhs.allocator()`; otherwise, they are copied.
    /// `rhs` is left in a valid but unspecified state.  The value currently
    /// held by this object (if any) is destroyed if that value's type is
    /// not `bsl::string`.  This function does not participate in overload
    /// resolution unless the specified `STRING_TYPE` is `bsl::string`.  The
    /// behavior is undefined unless `rhs` is valid UTF-8 (see
    /// `bdlde::Utf8Util::isValid`).
    template <class STRING_TYPE>
    typename bsl::enable_if<bsl::is_same<STRING_TYPE, bsl::string>::value,
                            Json>::type&
    operator=(BSLMF_MOVABLEREF_DEDUCE(STRING_TYPE) rhs);

    /// Assign to this object a value of type `bool` initialized from the
    /// specified `rhs`, and return a reference providing modifiable access
    /// to this object.  The value currently held by this object (if any) is
    /// destroyed if that value's type is not `bsl::string`.
    Json& operator=(bool rhs);

    /// Assign to this object a value of type `JsonObject` initialized from
    /// the specified `rhs`, and return a reference providing modifiable
    /// access to this object.  The value currently held by this object (if
    /// any) is destroyed if that value's type is not `JsonObject`.
    Json& operator=(const JsonObject& rhs);

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.  The
    /// contents of `rhs` are moved (in constant time) to this `Json` object
    /// if `allocator() == rhs.allocator()`; otherwise, they are copied.
    /// `rhs` is left in a valid but unspecified state.  The value currently
    /// held by this object (if any) is destroyed if that value's type is
    /// not `JsonObject`.
    Json& operator=(bslmf::MovableRef<JsonObject> rhs);

    /// Assign to this object a value of type `JsonArray` initialized from
    /// the specified `rhs`, and return a reference providing modifiable
    /// access to this object.  The value currently held by this object (if
    /// any) is destroyed if that value's type is not `JsonArray`.
    Json& operator=(const JsonArray& rhs);

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.  The
    /// contents of `rhs` are moved (in constant time) to this `Json` object
    /// if `allocator() == rhs.allocator()`; otherwise, they are copied.
    /// `rhs` is left in a valid but unspecified state.  The value currently
    /// held by this object (if any) is destroyed if that value's type is
    /// not `JsonArray`.
    Json& operator=(bslmf::MovableRef<JsonArray> rhs);

    /// Assign to this object the singleton value of type `JsonNull`, and
    /// return a reference providing modifiable access to this object.  The
    /// value currently held by this object (if any) is destroyed if that
    /// value's type is not `JsonNull`.
    Json& operator=(const JsonNull&);

    /// Assign to this object the singleton value of type `JsonNull`, and
    /// return a reference providing modifiable access to this object.
    /// `rhs` is left in a valid but unspecified state.  The value currently
    /// held by this object (if any) is destroyed if that value's type is
    /// not `JsonNull`.
    Json& operator=(bslmf::MovableRef<JsonNull>);

    /// Create an instance of type `JsonArray` in this object, using the
    /// allocator currently held by this object to supply memory, and return
    /// a reference providing modifiable access to the created instance.
    /// Optionally specify `array` to initialize the `JsonArray` created.
    /// This method first destroys the current value held by this object
    /// (even if the type currently held is `JsonArray`).
    JsonArray& makeArray();
    JsonArray& makeArray(const JsonArray&             array);
    JsonArray& makeArray(bslmf::MovableRef<JsonArray> array);

    /// Create an instance of type `bool` in this object and return a
    /// reference providing modifiable access to the created instance.
    /// Optionally specify `boolean` to initialize the `bool` created.  This
    /// method first destroys the current value held by this object.
    bool& makeBoolean();
    bool& makeBoolean(bool boolean);

    /// Create an instance of type `JsonNull` in this object.  This method
    /// first destroys the current value held by this object.
    void makeNull();

    /// Create an instance of type `JsonNumber` in this object, using the
    /// allocator currently held by this object to supply memory, and return
    /// a reference providing modifiable access to the created instance.
    /// Optionally specify `number` to initialize the `JsonNumber` created.
    /// This method first destroys the current value held by this object
    /// (even if the type currently held is `JsonNumber`).
    JsonNumber& makeNumber();
    JsonNumber& makeNumber(const JsonNumber&             number);
    JsonNumber& makeNumber(bslmf::MovableRef<JsonNumber> number);

    /// Create an instance of type `JsonObject` in this object, using the
    /// allocator currently held by this object to supply memory, and return
    /// a reference providing modifiable access to the created instance.
    /// Optionally specify `object` to initialize the `JsonObject` created.
    /// This method first destroys the current value held by this object
    /// (even if the type currently held is `JsonObject`).
    JsonObject& makeObject();
    JsonObject& makeObject(const JsonObject&             object);
    JsonObject& makeObject(bslmf::MovableRef<JsonObject> object);

    // BDE_VERIFY pragma: -FD06 'string' and 'bsl::string' are too similar
    // BDE_VERIFY pragma: -FD07 'string' and 'bsl::string' are too similar

    /// Create an instance of type `bsl::string` in this object, using the
    /// allocator currently held by this object to supply memory.
    /// Optionally specify `string` to initialize the `bsl::string` created.
    /// This method first destroys the current value held by this object
    /// (even if the type currently held is `bsl::string`).  The behavior is
    /// undefined unless `string` is valid UTF-8 (see
    /// `bdlde::Utf8Util::isValid`).
    void makeString(const char              *string);
    void makeString(const bsl::string_view&  string);

    /// Create an instance of type `bsl::string` in this object, using the
    /// allocator currently held by this object to supply memory.
    /// Optionally specify `string` to initialize the `bsl::string` created.
    /// This method first destroys the current value held by this object
    /// (even if the type currently held is `bsl::string`).  This function
    /// does not participate in overload resolution unless the specified
    /// `STRING_TYPE` is `bsl::string`.  The behavior is undefined unless
    /// `string` is valid UTF-8 (see `bdlde::Utf8Util::isValid`).
    template <class STRING_TYPE>
    typename bsl::enable_if<bsl::is_same<STRING_TYPE,
                                         bsl::string>::value>::type
        makeString(BSLMF_MOVABLEREF_DEDUCE(STRING_TYPE) string);

    // BDE_VERIFY pragma: +FD06 'string' and 'bsl::string' are too similar
    // BDE_VERIFY pragma: +FD07 'string' and 'bsl::string' are too similar

    /// Return a reference providing modifiable access to the value of type
    /// `JsonArray` held by this object.  The behavior is undefined unless
    /// `isArray()` returns true.
    JsonArray& theArray();

    /// Return a reference providing modifiable access to the value of type
    /// `boolean` held by this object.  The behavior is undefined unless
    /// `isBoolean()` returns true.
    bool& theBoolean();

    /// Return a reference providing modifiable access to the value of type
    /// `JsonNull` held by this object.  The behavior is undefined unless
    /// `isNull()` returns true.
    JsonNull& theNull();

    /// Return a reference providing modifiable access to the value of type
    /// `JsonNumber` held by this object.  The behavior is undefined unless
    /// `isNumber()` returns true.
    JsonNumber& theNumber();

    /// Return a reference providing modifiable access to the value of type
    /// `JsonObject` held by this object.  The behavior is undefined unless
    /// `isObject()` returns true.
    JsonObject& theObject();

#if defined(BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT)
    /// Return a reference providing modifiable access to the value of type
    /// `JsonArray` held by this object.  The behavior is undefined unless
    /// `isArray()` returns true.
    explicit operator JsonArray &();

    /// Return a reference providing modifiable access to the value of type
    /// `boolean` held by this object.  The behavior is undefined unless
    /// `isBoolean()` returns true.
    explicit operator bool &();

    /// Return a reference providing modifiable access to the value of type
    /// `JsonNull` held by this object.  The behavior is undefined unless
    /// `isNull()` returns true.
    explicit operator JsonNull &();

    /// Return a reference providing modifiable access to the value of type
    /// `JsonNumber` held by this object.  The behavior is undefined unless
    /// `isNumber()` returns true.
    explicit operator JsonNumber &();

    /// Return a reference providing modifiable access to the value of type
    /// `JsonObject` held by this object.  The behavior is undefined unless
    /// `isObject()` returns true.
    explicit operator JsonObject &();
#endif

    /// Return a reference providing modifiable access to the `Json` object
    /// associated with the specified `key` in the `JsonObject` held by this
    /// object; if the `JsonObject` does not already contain a `Json` object
    /// associated with `key`, first insert a new default-constructed `Json`
    /// object associated with `key`.  The behavior is undefined unless
    /// one of `isObject()` or `isNull()` returns true.
    Json& operator[](const bsl::string_view& key);

    /// Return a reference providing modifiable access to the element at the
    /// specified `index` in the `JsonArray` held by this object.  The
    /// behavior is undefined unless `isArray()` returns true and
    /// `index < theArray().size()`.
    Json& operator[](bsl::size_t index);

// BDE_VERIFY pragma: -FABC01 // not in order

                        // 'pushBack' methods

    /// Append to the end of this `Json` object's array a copy of the
    /// specified `json`/`array`/`object`/`number` and return a non-`const`
    /// reference to this object.  If `isNull()`, then the type is changed
    /// to array.  If an exception is thrown, type to array.  `*this` is
    /// unaffected.  Throw `bsl::length_error` if `size() == maxSize()`.
    /// The behavior is undefined unless `isArray()` or `isNull()`.
    Json& pushBack(const Json&       json);
    Json& pushBack(const JsonArray&  array);
    Json& pushBack(const JsonObject& object);
    Json& pushBack(const JsonNumber& number);

    /// Append to the end of this `Json` object's array the specified
    /// move-insertable `json`/`array`/`object`/`number` and return a
    /// non-`const` reference to this object.
    /// `json`/`array`/`object`/`number` is left in a valid but unspecified
    /// state.  If `isNull`, then the type is changed to array.  If an
    /// exception is thrown, `*this` is unaffected.  Throw
    /// `bsl::length_error` if `size() == maxSize()`.  The behavior is
    /// undefined unless `isArray()` or `isNull()`.
    Json& pushBack(bslmf::MovableRef<Json>       json);
    Json& pushBack(bslmf::MovableRef<JsonArray>  array);
    Json& pushBack(bslmf::MovableRef<JsonObject> object);
    Json& pushBack(bslmf::MovableRef<JsonNumber> number);

    /// Append to the end of this `Json` object's array a copy of the
    /// specified `value` and return a non-`const` reference to this object.
    /// If `isNull()`, then type is changed to array.  If an exception is
    /// thrown, `*this` is unaffected.  Throw `bsl::length_error` if
    /// `size() == maxSize()`.  The behavior is undefined unless `isArray()`
    /// or `isNull()`.
    Json& pushBack(const JsonNull&    value);
    Json& pushBack(bool               value);
    Json& pushBack(int                value);
    Json& pushBack(unsigned int       value);
    Json& pushBack(long               value);
    Json& pushBack(unsigned long      value);
    Json& pushBack(long long          value);
    Json& pushBack(unsigned long long value);
    Json& pushBack(float              value);
    Json& pushBack(double             value);
    Json& pushBack(bdldfp::Decimal64  value);

    /// Append to the end of this `Json` objects' array a copy of the
    /// specified `string` and return a non-`const` reference to this
    /// object.  If `isNull`, then type is changed to array.  If an
    /// exception is thrown, `*this` is unaffected.  Throw
    /// `bsl::length_error` if `size() == maxSize()`.  The behavior is
    /// undefined unless `isArray()` or `isNull`.
    Json& pushBack(const char              *string);
    Json& pushBack(const bsl::string_view&  string);

    // Append to the end of this `Json` objects' array an element having
    // value of the specified `string` by moving (in constant time) the
    // contents of `string` to the new element and return a non-`const`
    // reference to this object.  If `isNull()`, then the type is changed
    // to array.  `string` is left in a valid but unspecified state.  This
    // function does not participate in overload resolution unless the
    // specified `STRING_TYPE` is `bsl::string`.  The behavior is undefined
    // unless `string` is valid UTF-8 (see `bdlde::Utf8Util::isValid`).
    // The behavior is undefined unless `isArray()` or `isNull()`.
    template <class STRING_TYPE>
    typename bsl::enable_if<bsl::is_same<STRING_TYPE,
                                         bsl::string>::value,
                            Json&>::type
    pushBack(BSLMF_MOVABLEREF_DEDUCE(STRING_TYPE) string);

    /// Return the result (having `RETURN_TYPE`) of invoking the specified
    /// `visitor` (callable) object on this `Json` object.  `visitor` must
    /// be callable as if it provides the following overloads of
    /// `operator()`:
    ///```
    /// RETURN_TYPE operator()(JsonObject  *object );
    /// RETURN_TYPE operator()(JsonArray   *array  );
    /// RETURN_TYPE operator()(bsl::string *string );
    /// RETURN_TYPE operator()(JsonNumber  *number );
    /// RETURN_TYPE operator()(bool        *boolean);
    /// RETURN_TYPE operator()(JsonNull    *null   );
    ///```
    /// The overload corresponding to the current `type()` of this `Json`
    /// object is invoked.  For C++03 `visitor` must be `const` qualified.
    /// The behavior is undefined unless all of these six overloads are
    /// defined.  The behavior is undefined if the `string` overload leaves
    /// this `Json` object with a string that is not valid UTF-8.  Note that
    /// overloads that are not applicable in a given application need not do
    /// anything other than return.  Also note that if `visitor` is `const`
    /// qualified then each of its `operator()` overloads must also be
    /// `const` qualified.
    template <class RETURN_TYPE, class VISITOR>
    RETURN_TYPE visit(BSLS_COMPILERFEATURES_FORWARD_REF(VISITOR) visitor);

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
    /// Return the result (having a deduced type) of invoking the specified
    /// `visitor` (callable) object on this `Json` object.  `visitor` must
    /// be callable as if it provides the following overloads of
    /// `operator()`, any of which can have deduced return types:
    ///```
    /// decltype(auto) operator()(JsonObject  *object );
    /// decltype(auto) operator()(JsonArray   *array  );
    /// decltype(auto) operator()(bsl::string *string );
    /// decltype(auto) operator()(JsonNumber  *number );
    /// decltype(auto) operator()(bool        *boolean);
    /// decltype(auto) operator()(JsonNull    *null   );
    ///```
    /// The overload corresponding to the current `type()` of this `Json`
    /// object is invoked.  The behavior is undefined unless all of these
    /// six overloads are defined *and* each has the same return type.  The
    /// behavior is undefined if the `string` overload leaves this `Json`
    /// object with a string that is not valid UTF-8.  Note that overloads
    /// that are not applicable in a given application need not do anything
    /// other than return.  Also note that if `visitor` is `const` qualified
    /// then each of its `operator()` overloads must also be `const`
    /// qualified.
    template <class VISITOR>
    decltype(auto) visit(BSLS_COMPILERFEATURES_FORWARD_REF(VISITOR) visitor);
#endif

                        // Aspects

    /// Exchange the value of this with that of the specified `other`.  If
    /// an exception is thrown, both objects are left in valid but
    /// unspecified states.  This operation guarantees O[1] complexity.  The
    /// behavior is undefined unless this object was created with the same
    /// allocator as `other`.
    void swap(Json& other);

    // ACCESSORS

// BDE_VERIFY pragma: -FABC01

    /// Load into the specified `result` the integer value of the value of
    /// type `JsonNumber` held by this object.  Return 0 on success,
    /// `JsonNumber::k_OVERFLOW` if `value` is larger than can be
    /// represented by `result`, `JsonNumber::k_UNDERFLOW` if `value` is
    /// smaller than can be represented by `result`, and
    /// `JsonNumber::k_NOT_INTEGRAL` if `value` is not an integral number
    /// (i.e., there is a fractional part).  For underflow, `result` will be
    /// loaded with the minimum representable value, for overflow, `result`
    /// will be loaded with the maximum representable value, for
    /// non-integral values `result` will be loaded with the integer part of
    /// `value` (truncating the value to the nearest integer).  If the
    /// result is not an integer and also either overflows or underflows, it
    /// is treated as an overflow or underflow (respectively).  Note that
    /// this operation returns a status value (unlike similar floating point
    /// conversions) because typically it is an error if a conversion to an
    /// integer results in an inexact value.  The behavior is undefined
    /// unless `isNumber()` returns true.  Also note that on all supported
    /// platforms `bsls::Types::Int64` and `bsls::Types::Uint64` are,
    /// respectively, aliases for `long long` and `unsigned long long`.
    int asShort    (short               *result) const;
    int asInt      (int                 *result) const;
    int asLong     (long                *result) const;
    int asLonglong (long long           *result) const;
    int asInt64    (bsls::Types::Int64  *result) const;
    int asUshort   (unsigned short      *result) const;
    int asUint     (unsigned int        *result) const;
    int asUlong    (unsigned long       *result) const;
    int asUlonglong(unsigned long long  *result) const;
    int asUint64   (bsls::Types::Uint64 *result) const;

    /// Return the closest floating point representation to the value of the
    /// type `JsonNumber` held by this object.  If this number is outside the
    /// representable range, return +INF or -INF (as appropriate).  The
    /// behavior is undefined unless `isNumber()` returns true.
    float             asFloat() const;
    double            asDouble() const;
    bdldfp::Decimal64 asDecimal64() const;

// BDE_VERIFY pragma: +FABC01 // not in order

    /// Load the specified `result` with the closest floating point
    /// representation to the value of type `JsonNumber` held by this
    /// object, even if a non-zero status is returned.  Return 0 if this
    /// number can be represented exactly, and return
    /// `JsonNumber::k_INEXACT` if `value` cannot be represented exactly.
    /// If this number is outside the representable range, load `result`
    /// with +INF or -INF (as appropriate).  A number can be represented
    /// exactly as a `Decimal64` if, for the significand and exponent,
    /// `abs(significand) <= 9,999,999,999,999,999` and '-398 <= exponent <=
    /// 369'.  The behavior is undefined unless `isNumber()` returns true;
    int asDecimal64Exact(bdldfp::Decimal64 *result) const;

    /// Return true if the value held by this object is of type `JsonArray`,
    /// and false otherwise.
    bool isArray() const;

    /// Return true if the value held by this object is of type `bool`, and
    /// false otherwise.
    bool isBoolean() const;

    /// Return true if the value held by this object is of type `JsonNull`,
    /// and false otherwise.
    bool isNull() const;

    /// Return true if the value held by this object is of type
    /// `JsonNumber`, and false otherwise.
    bool isNumber() const;

    /// Return true if the value held by this object is of type
    /// `JsonObject`, and false otherwise.
    bool isObject() const;

    /// Return true if the value held by this object is of type
    /// `bsl::string`, and false otherwise.
    bool isString() const;

    /// Return a reference providing non-modifiable access to the value of
    /// type `JsonArray` held by this object.  The behavior is undefined
    /// unless `isArray()` returns true.
    const JsonArray& theArray() const;

    /// Return a reference providing non-modifiable access to the value of
    /// type `boolean` held by this object.  The behavior is undefined
    /// unless `isBoolean()` returns true.
    const bool& theBoolean() const;

    /// Return a reference providing non-modifiable access to the value of
    /// type `JsonNull` held by this object.  The behavior is undefined
    /// unless `isNull()` returns true.
    const JsonNull& theNull() const;

    /// Return a reference providing non-modifiable access to the value of
    /// type `JsonNumber` held by this object.  The behavior is undefined
    /// unless `isNumber()` returns true.
    const JsonNumber& theNumber() const;

    /// Return a reference providing non-modifiable access to the value of
    /// type `JsonObject` held by this object.  The behavior is undefined
    /// unless `isObject()` returns true.
    const JsonObject& theObject() const;

    /// Return a reference providing non-modifiable access to the value of
    /// type `JsonString` held by this object.  The behavior is undefined
    /// unless `isString()` returns true.
    const bsl::string& theString() const;

    /// Return the type of this `Json` value.
    JsonType::Enum type() const BSLS_KEYWORD_NOEXCEPT;

    /// Return a reference providing non-modifiable access to the `Json`
    /// object associated with the specified `key` in this `JsonObject`.
    /// The behavior is undefined unless `key` is valid UTF-8 (see
    /// `bdlde::Utf8Util::isValid`) and this `JsonObject` already contains a
    /// `Json` object associated with `key`.
    const Json& operator[](const bsl::string_view& key) const;

    /// Return a reference providing non-modifiable access to the element at
    /// the specified `index` in the `JsonArray` held by this object.  The
    /// behavior is undefined unless `isArray()` returns true and
    /// `index < theArray().size()`.
    const Json& operator[](bsl::size_t index) const;

#if defined(BSLS_COMPILERFEATURES_SUPPORT_OPERATOR_EXPLICIT)
    /// Return a reference providing non-modifiable access to the value of
    /// type `JsonArray` held by this object.  The behavior is undefined
    /// unless `isArray()` returns true.
    explicit operator const JsonArray &() const;

    /// Return a reference providing non-modifiable access to the value of
    /// type `boolean` held by this object.  The behavior is undefined
    /// unless `isBoolean()` returns true.
    explicit operator const bool &() const;

    /// Return a reference providing non-modifiable access to the value of
    /// type `JsonNull` held by this object.  The behavior is undefined
    /// unless `isNull()` returns true.
    explicit operator const JsonNull &() const;

    /// Return a reference providing non-modifiable access to the value of
    /// type `JsonNumber` held by this object.  The behavior is undefined
    /// unless `isNumber()` returns true.
    explicit operator const JsonNumber &() const;

    /// Return a reference providing non-modifiable access to the value of
    /// type `JsonObject` held by this object.  The behavior is undefined
    /// unless `isObject()` returns true.
    explicit operator const JsonObject &() const;

    /// Return a reference providing non-modifiable access to the value of
    /// type `JsonString` held by this object.  The behavior is undefined
    /// unless `isString()` returns true.
    explicit operator const bsl::string &() const;
#endif

    /// Return the number of elements in the `JsonObject` or `JsonArray`
    /// held by this object.  The behavior is undefined unless
    /// `isArray() || isObject()` evaluates to true.
    bsl::size_t size() const;

    /// Return the result (having `RETURN_TYPE`) of invoking the specified
    /// `visitor` (callable) object on this `Json` object.  `visitor` must
    /// be callable as if it provides the following overloads of
    /// `operator()`:
    ///```
    /// RETURN_TYPE operator()(const JsonObject&  object );
    /// RETURN_TYPE operator()(const JsonArray&   array  );
    /// RETURN_TYPE operator()(const bsl::string& string );
    /// RETURN_TYPE operator()(const JsonNumber&  number );
    /// RETURN_TYPE operator()(const bool&        boolean);
    /// RETURN_TYPE operator()(const JsonNull&    null   );
    ///```
    /// The overload corresponding to the current `type()` of this `Json`
    /// object is invoked.  For C++03 visitor must be `const`-qualified.
    /// The behavior is undefined unless all of these six overloads are
    /// defined.  Note that overloads that are not applicable in a given
    /// application need not do anything other than return.  Also note that
    /// if `visitor` is `const` qualified then each of its `operator()`
    /// overloads must also be `const` qualified.
    template <class RETURN_TYPE, class VISITOR>
    RETURN_TYPE visit(BSLS_COMPILERFEATURES_FORWARD_REF(VISITOR) visitor)
                                                                         const;

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
    /// Return the result (having a deduced type) of invoking the specified
    /// `visitor` (callable) object on this `Json` object.  `visitor` must
    /// be callable as if it provides the following overloads of
    /// 'operator()', any of which can have deduced return types:
    ///```
    /// decltype(auto) operator()(JsonObject  *object );
    /// decltype(auto) operator()(JsonArray   *array  );
    /// decltype(auto) operator()(bsl::string *string );
    /// decltype(auto) operator()(JsonNumber  *number );
    /// decltype(auto) operator()(bool        *boolean);
    /// decltype(auto) operator()(JsonNull    *null   );
    ///```
    /// The overload corresponding to the current `type()` of this `Json`
    /// object is invoked.  The behavior is undefined unless all of these
    /// six overloads are defined *and* each has the same return type.  Note
    /// that overloads that are not applicable in a given application need
    /// not do anything other than return.  Also note that if `visitor` is
    /// `const` qualified then each of its `operator()` overloads must also
    /// be `const` qualified.
    template <class VISITOR>
    decltype(auto) visit(BSLS_COMPILERFEATURES_FORWARD_REF(VISITOR) visitor)
                                                                         const;
#endif

                                  // Aspects

    /// Return the allocator used by this object to allocate memory.
    bslma::Allocator *allocator() const BSLS_KEYWORD_NOEXCEPT;

    /// Write the value of this object to the specified output `stream` in a
    /// human-readable format, and return a reference to `stream`.
    /// Optionally specify an initial indentation `level`, whose absolute
    /// value is incremented recursively for nested objects.  If `level` is
    /// specified, optionally specify `spacesPerLevel`, whose absolute value
    /// indicates the number of spaces per indentation level for this and
    /// all of its nested objects.  If `level` is negative, suppress
    /// indentation of the first line.  If `spacesPerLevel` is negative,
    /// format the entire output on one line, suppressing all but the
    /// initial indentation (as governed by `level`).  If `stream` is not
    /// valid on entry, this operation has no effect.  Note that this
    /// human-readable format is not fully specified, and can change without
    /// notice.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
};

// FREE OPERATORS

/// Write the value of the specified `object` to the specified output
/// `stream` in a single-line format, and return a reference to `stream`.
/// If `stream` is not valid on entry, this operation has no effect.  Note
/// that this human-readable format is not fully specified, can change
/// without notice, and is logically equivalent to:
/// ```
/// print(stream, 0, -1);
/// ```
bsl::ostream& operator<<(bsl::ostream& stream, const Json& object);

/// Return `true` if the specified `lhs` and `rhs` objects have the same
/// value, and `false` otherwise.  Two `Json` objects `lhs` and `rhs` have
/// the same value if they hold objects of the same type, and those objects
/// have the same value.
bool operator==(const Json& lhs, const Json& rhs);

/// Return `false` if the specified `lhs` and `rhs` objects have the same
/// value, and `true` otherwise.  Two `Json` objects `lhs` and `rhs` have
/// the same value if they hold objects of the same type, and those objects
/// have the same value.
bool operator!=(const Json& lhs, const Json& rhs);

// Return `true` if the specified `lhs` and `rhs` arguments have the same
// value, and `false` otherwise.  Two arguments have the same value if the
// `Json` object has the same value as a `Json` object constructed from the
// other argument.
bool operator==(const JsonArray&         lhs, const Json&              rhs);
bool operator==(const JsonObject&        lhs, const Json&              rhs);
bool operator==(const JsonNumber&        lhs, const Json&              rhs);
bool operator==(const JsonNull&          lhs, const Json&              rhs);
bool operator==(bool                     lhs, const Json&              rhs);
bool operator==(int                      lhs, const Json&              rhs);
bool operator==(unsigned int             lhs, const Json&              rhs);
bool operator==(long                     lhs, const Json&              rhs);
bool operator==(unsigned long            lhs, const Json&              rhs);
bool operator==(long long                lhs, const Json&              rhs);
bool operator==(unsigned long long       lhs, const Json&              rhs);
bool operator==(float                    lhs, const Json&              rhs);
bool operator==(double                   lhs, const Json&              rhs);
bool operator==(bdldfp::Decimal64        lhs, const Json&              rhs);
bool operator==(const char              *lhs, const Json&              rhs);
bool operator==(const bsl::string_view&  lhs, const Json&              rhs);
bool operator==(const Json&              lhs, const JsonArray&         rhs);
bool operator==(const Json&              lhs, const JsonObject&        rhs);
bool operator==(const Json&              lhs, const JsonNumber&        rhs);
bool operator==(const Json&              lhs, const JsonNull&          rhs);
bool operator==(const Json&              lhs, bool                     rhs);
bool operator==(const Json&              lhs, int                      rhs);
bool operator==(const Json&              lhs, unsigned int             rhs);
bool operator==(const Json&              lhs, long                     rhs);
bool operator==(const Json&              lhs, unsigned long            rhs);
bool operator==(const Json&              lhs, long long                rhs);
bool operator==(const Json&              lhs, unsigned long long       rhs);
bool operator==(const Json&              lhs, float                    rhs);
bool operator==(const Json&              lhs, double                   rhs);
bool operator==(const Json&              lhs, bdldfp::Decimal64        rhs);
bool operator==(const Json&              lhs, const char              *rhs);
bool operator==(const Json&              lhs, const bsl::string_view&  rhs);

// Return `true` if the specified `lhs` and `rhs` arguments do not have the
// same value, and `false` otherwise.  Two arguments do not the same value
// if the `Json` object does not has the same value as a `Json` object
// constructed from the other argument.
bool operator!=(const JsonArray&         lhs, const Json&              rhs);
bool operator!=(const JsonObject&        lhs, const Json&              rhs);
bool operator!=(const JsonNumber&        lhs, const Json&              rhs);
bool operator!=(const JsonNull&          lhs, const Json&              rhs);
bool operator!=(bool                     lhs, const Json&              rhs);
bool operator!=(int                      lhs, const Json&              rhs);
bool operator!=(unsigned int             lhs, const Json&              rhs);
bool operator!=(long                     lhs, const Json&              rhs);
bool operator!=(unsigned long            lhs, const Json&              rhs);
bool operator!=(long long                lhs, const Json&              rhs);
bool operator!=(unsigned long long       lhs, const Json&              rhs);
bool operator!=(float                    lhs, const Json&              rhs);
bool operator!=(double                   lhs, const Json&              rhs);
bool operator!=(bdldfp::Decimal64        lhs, const Json&              rhs);
bool operator!=(const char              *lhs, const Json&              rhs);
bool operator!=(const bsl::string_view&  lhs, const Json&              rhs);
bool operator!=(const Json&              lhs, const JsonArray&         rhs);
bool operator!=(const Json&              lhs, const JsonObject&        rhs);
bool operator!=(const Json&              lhs, const JsonNumber&        rhs);
bool operator!=(const Json&              lhs, const JsonNull&          rhs);
bool operator!=(const Json&              lhs, bool                     rhs);
bool operator!=(const Json&              lhs, int                      rhs);
bool operator!=(const Json&              lhs, unsigned int             rhs);
bool operator!=(const Json&              lhs, long                     rhs);
bool operator!=(const Json&              lhs, unsigned long            rhs);
bool operator!=(const Json&              lhs, long long                rhs);
bool operator!=(const Json&              lhs, unsigned long long       rhs);
bool operator!=(const Json&              lhs, float                    rhs);
bool operator!=(const Json&              lhs, double                   rhs);
bool operator!=(const Json&              lhs, bdldfp::Decimal64        rhs);
bool operator!=(const Json&              lhs, const char              *rhs);
bool operator!=(const Json&              lhs, const bsl::string_view&  rhs);

/// Invoke the specified `hashAlg` on the attributes of the specified
/// `object`.
template <class HASHALG>
void hashAppend(HASHALG& hashAlg, const Json& object);

/// Exchange the value of the specified `a` `Json` object with that of the
/// specified `b` `Json` object.  If an exception is thrown, both objects
/// are left in valid but unspecified states.  This operation guarantees
/// O[1] complexity.  The behavior is undefined unless `a` was created with
/// the same allocator as `b`.
void swap(Json& a, Json& b);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS

                           // ======================
                           // class Json_Initializer
                           // ======================

/// This component-private type is designed capture a set of values that can be
/// used to initialize a `Json` object, or an element in a `JsonArray`. It is
/// not modifiable, and (except for small types like `int`) does not own the
/// objects that are passed in.  The purpose here is to enable
/// list-initialization (including nested lists) of JSON objects.
class Json_Initializer {

  public:
    // PUBLIC TYPES
    typedef bdlb::Variant<JsonNull,
                          bool,
                          long long,
                          unsigned long long,
                          double,
                          bdldfp::Decimal64,
                          bsl::string_view,
                          std::initializer_list<Json_Initializer>,
                          const JsonObject *,
                          const JsonArray  *,
                          const JsonNumber *,
                          const Json       *> Storage;

  private:
    // DATA
    Storage d_storage;

  public:
    // CREATORS

    /// Create a `Json_Initializer` object containing a `JsonNull` in the
    /// internal variant.
    Json_Initializer();
    Json_Initializer(const JsonNull& );                             // IMPLICIT

    // Create a `Json_Initializer` object containing the boolean value of
    // the specified `b` in the internal variant.
    Json_Initializer(bool b);                                       // IMPLICIT

    /// Create a `Json_Initializer` object containing the value of the
    /// specified `num` in the internal variant.  If `num` is a signed
    /// integral type, then the value is stored as a `long long`.  If `num`
    /// is an unsigned integral type, then the value is stored as an
    /// `unsigned long long`.
    template <class NUMBER, class = typename bsl::enable_if<
                                        std::is_integral<NUMBER>::value>::type>
    Json_Initializer(NUMBER num);                                   // IMPLICIT

    /// Create a `Json_Initializer` object containing the specified `f`.
    Json_Initializer(float f);                                      // IMPLICIT

    /// Create a `Json_Initializer` object containing the specified `d`.
    Json_Initializer(double d);                                     // IMPLICIT

    /// Create a `Json_Initializer` object containing the specified `value`.
    Json_Initializer(bdldfp::Decimal64 value);                      // IMPLICIT

    /// Create a `Json_Initializer` object containing a `bsl::string_view`
    /// referring to the null-terminated sequence pointed to by the
    /// specified `p`.
    Json_Initializer(const char *p);                                // IMPLICIT

    /// Create a `Json_Initializer` object containing a 'bsl::string_view'
    /// referring to the contents of the specified `s`.
    Json_Initializer(const bsl::string&      s);                    // IMPLICIT
    Json_Initializer(const std::string&      s);                    // IMPLICIT
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING
    Json_Initializer(const std::pmr::string& s);                    // IMPLICIT
#endif

    /// Create a `Json_Initializer` object containing a `bsl::string_view`
    /// containing the specified `sv`.
    Json_Initializer(const bsl::string_view& sv);                   // IMPLICIT

#if  defined(BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY) \
&&  !defined(BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY)
    /// Create a `Json_Initializer` object containing a `bsl::string_view`
    /// containing the specified `sv`.
    Json_Initializer(const std::string_view& sv);                   // IMPLICIT
#endif

    /// Create a `Json_Initializer` object containing an initializer list
    /// with the value of the specified `il`.
    Json_Initializer(std::initializer_list<Json_Initializer> il);   // IMPLICIT

    /// Create a `Json_Initializer` object containing a pointer to the
    /// specified `jObj`.
    Json_Initializer(const JsonObject& jObj);                       // IMPLICIT

    /// Create a `Json_Initializer` object containing a pointer to the
    /// specified `jArr`.
    Json_Initializer(const JsonArray& jArr);                        // IMPLICIT

    /// Create a `Json_Initializer` object containing a pointer to the
    /// specified `jNum`.
    Json_Initializer(const JsonNumber& jNum);                       // IMPLICIT

    /// Create a `Json_Initializer` object containing a pointer to the
    /// specified `json`.
    Json_Initializer(const Json& json);                             // IMPLICIT

    // ACCESSORS

    /// Return a reference providing non-modifiable access to the internal
    /// variant.
    const Storage& get_storage() const;
};

/// This component-private type is designed capture a set of values that can
/// be used to initialize an element in a `JsonObject`.  The purpose here is
/// to enable list-initialization (including nested lists) of JSON objects.
class Json_MemberInitializer {

  private:
    // DATA
    bsl::string_view    d_first;
    Json_Initializer    d_second;

  public:
// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -IEC01  // implicit conversions is the reason we exist

    // CREATORS

    /// Create a `Json_MemberInitializer` object a key whose value is equal to
    /// the specified `s` and whose value is equal to the specified `ji`.
    /// specified `jNum`.
    Json_MemberInitializer(const char         *s, const Json_Initializer& ji);
    Json_MemberInitializer(const bsl::string&  s, const Json_Initializer& ji);
    Json_MemberInitializer(const std::string&  s, const Json_Initializer& ji);

    /// Create a `Json_MemberInitializer` object a key whose value is equal to
    /// the specified `sv` and whose value is equal to the specified `ji`.
    Json_MemberInitializer(const bsl::string_view& sv,
                           const Json_Initializer& ji);

#if  defined(BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY) \
&&  !defined(BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY)
    // Create a `Json_MemberInitializer` object a key whose value is equal to
    // the specified `sv` and whose value is equal to the specified `ji`.
    Json_MemberInitializer(const std::string_view& sv,
                           const Json_Initializer& ji);

#endif

    // ACCESSORS

    /// Return the key that the object was created with.
    bsl::string_view key() const;

    /// Return a reference providing non-modifiable access to the value that
    /// the object was created with.
    const Json_Initializer& value() const;

// BDE_VERIFY pragma: pop

};

#endif  // BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS

                                 // =====================
                                 // struct Json_VisitUtil
                                 // =====================

/// This component-private utility `struct` provides a namespace for a suite of
/// operations used in the implementation of "visit" operations provided by
/// classes in this component.
struct Json_VisitUtil {

    // CLASS METHODS

    /// Invoke the specified `visitor` (callable object) with the specified
    /// `argument` and return the computed value of (template parameter) the
    /// `RETURN_TYPE`.  Note that in typical use, `argument` has the
    /// current value and type of a `Json` object.
    template <class RETURN_TYPE,
              class VISITOR,
              class ARGUMENT>
    static
    RETURN_TYPE invokeVisitor(VISITOR                         *visitor,
                              const ARGUMENT&                  argument);
    template <class RETURN_TYPE,
              class VISITOR,
              class ARGUMENT>
    static
    RETURN_TYPE invokeVisitor(VISITOR&                         visitor,
                              const ARGUMENT&                  argument);
    template <class RETURN_TYPE,
              class VISITOR,
              class ARGUMENT>
    static
    RETURN_TYPE invokeVisitor(const VISITOR&                   visitor,
                              const ARGUMENT&                  argument);
    template <class RETURN_TYPE,
              class VISITOR,
              class ARGUMENT>
    static
    RETURN_TYPE invokeVisitor(BSLMF_MOVABLEREF_DEDUCE(VISITOR) visitor,
                              const ARGUMENT&                  argument);
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
    template <class VISITOR,
              class ARGUMENT>
    static
    decltype(auto) invokeVisitor(const VISITOR&                   visitor,
                                 const ARGUMENT&                  argument);
    template <class VISITOR,
              class ARGUMENT>
    static
    decltype(auto) invokeVisitor(BSLMF_MOVABLEREF_DEDUCE(VISITOR) visitor,
                                 const ARGUMENT&                  argument);
#endif

};

                        // ===============================
                        // class Json_StringInvariantGuard
                        // ===============================

/// This is a component-private guard class.
class Json_StringInvariantGuard {

    // DATA
    bsl::string *d_string_p;

  public:
    // CREATORS

    /// Create an object that guards the invariant (valid UTF-8) of the
    /// JSON string at the specified `string`.
    explicit Json_StringInvariantGuard(bsl::string *string);

    /// Validate the JSON string specified on construction using
    /// `BSLS_ASSERT` and destroy this guard object.
    ~Json_StringInvariantGuard() BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false);
};

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
JsonArray::JsonArray(std::initializer_list<Json_Initializer>  elements,
                     bslma::Allocator                        *basicAllocator)
: d_elements(basicAllocator)
{
    d_elements.reserve(elements.size());
    for (const auto& e : elements) {
        d_elements.emplace_back(e);
        }
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
JsonArray& JsonArray::operator=(std::initializer_list<Json_Initializer>
                                                                   initializer)
{
    assign(initializer);
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
JsonArray& JsonArray::assign(INPUT_ITERATOR first, INPUT_ITERATOR last)
{
    d_elements.assign(first, last);
    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
inline
JsonArray& JsonArray::assign(std::initializer_list<Json_Initializer>
                                                                   initializer)
{
    Elements newElements(d_elements.get_allocator());
    newElements.reserve(initializer.size());
    for (const auto& e : initializer) {
        newElements.emplace_back(e);
    }
    bslalg::SwapUtil::swap(&d_elements, &newElements);
    return *this;
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
JsonArray& JsonArray::pushBack(const Json& json)
{
    d_elements.push_back(json);
    return *this;
}

inline
JsonArray& JsonArray::pushBack(bslmf::MovableRef<Json> json)
{
    d_elements.push_back(bslmf::MovableRefUtil::move(json));
    return *this;
}

inline
JsonArray& JsonArray::pushBack(const JsonArray& array)
{
    d_elements.emplace_back(array);
    return *this;
}

inline
JsonArray& JsonArray::pushBack(const JsonObject& object)
{
    d_elements.emplace_back(object);
    return *this;
}

inline
JsonArray& JsonArray::pushBack(const JsonNumber& number)
{
    d_elements.emplace_back(number);
    return *this;
}

inline
JsonArray& JsonArray::pushBack(bslmf::MovableRef<JsonArray> array)
{
    d_elements.emplace_back(bslmf::MovableRefUtil::move(array));
    return *this;
}

inline
JsonArray& JsonArray::pushBack(bslmf::MovableRef<JsonObject> object)
{
    d_elements.emplace_back(bslmf::MovableRefUtil::move(object));
    return *this;
}

inline
JsonArray& JsonArray::pushBack(bslmf::MovableRef<JsonNumber> number)
{
    d_elements.emplace_back(bslmf::MovableRefUtil::move(number));
    return *this;
}

inline
JsonArray& JsonArray::pushBack(const JsonNull& value)
{
    d_elements.emplace_back(value);
    return *this;
}

inline
JsonArray& JsonArray::pushBack(bool value)
{
    d_elements.emplace_back(value);
    return *this;
}

inline
JsonArray& JsonArray::pushBack(int value)
{
    d_elements.emplace_back(value);
    return *this;
}

inline
JsonArray& JsonArray::pushBack(unsigned int value)
{
    d_elements.emplace_back(value);
    return *this;
}

inline
JsonArray& JsonArray::pushBack(long value)
{
    d_elements.emplace_back(value);
    return *this;
}

inline
JsonArray& JsonArray::pushBack(unsigned long value)
{
    d_elements.emplace_back(value);
    return *this;
}

inline
JsonArray& JsonArray::pushBack(long long value)
{
    d_elements.emplace_back(value);
    return *this;
}

inline
JsonArray& JsonArray::pushBack(unsigned long long value)
{
    d_elements.emplace_back(value);
    return *this;
}

inline
JsonArray& JsonArray::pushBack(float value)
{
    d_elements.emplace_back(value);
    return *this;
}

inline
JsonArray& JsonArray::pushBack(double value)
{
    d_elements.emplace_back(value);
    return *this;
}

inline
JsonArray& JsonArray::pushBack(bdldfp::Decimal64 value)
{
    d_elements.emplace_back(value);
    return *this;
}

inline
JsonArray& JsonArray::pushBack(const char *string)
{
    BSLS_ASSERT(string);

    d_elements.emplace_back(string);
    return *this;
}

inline
JsonArray& JsonArray::pushBack(const bsl::string_view& string)
{
    d_elements.emplace_back(string);
    return *this;
}

template <class STRING_TYPE>
inline
typename bsl::enable_if<bsl::is_same<STRING_TYPE,
                                     bsl::string>::value,
                        JsonArray&>::type
JsonArray::pushBack(BSLMF_MOVABLEREF_DEDUCE(STRING_TYPE) string)
{
    BSLS_ASSERT(bdlde::Utf8Util::isValid(
                                       bslmf::MovableRefUtil::access(string)));

    d_elements.emplace_back(bslmf::MovableRefUtil::move(string));
    return *this;
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
JsonObject::JsonObject(
                 std::initializer_list<Json_MemberInitializer>  members,
                 bslma::Allocator                              *basicAllocator)
: d_members(basicAllocator)
{
    d_members.reserve(members.size());
    for (const auto& mem : members) {
        BSLS_ASSERT(bdlde::Utf8Util::isValid(mem.key().data(),
                                             mem.key().size()));
        insert(mem.key(), mem.value());
    }
}

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY)
inline
JsonObject::JsonObject(const std::string_view&  key,
                       const Json_Initializer&  init,
                       bslma::Allocator        *basicAllocator)
: d_members(basicAllocator)
{
    BSLS_ASSERT(bdlde::Utf8Util::isValid(key.data(), key.size()));
    insert(key, init);
}
#endif
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
JsonObject& JsonObject::operator=(
                         std::initializer_list<Json_MemberInitializer> members)
{
    Container new_container(d_members.get_allocator());
    new_container.reserve(members.size());
    for (const auto& mem : members) {
        BSLS_ASSERT(bdlde::Utf8Util::isValid(mem.key().data(),
                                             mem.key().size()));
        if (!new_container.contains(mem.key())) {
            Json   json(mem.value(), allocator());
            Member member(mem.key(), Json(), allocator());
            member.second = bslmf::MovableRefUtil::move(json);
            new_container.insert(bslmf::MovableRefUtil::move(member));
        }
    }
    bslalg::SwapUtil::swap(&d_members, &new_container);
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

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR)
    return d_members.emplace(std::piecewise_construct,
                             std::forward_as_tuple(key),
                             std::forward_as_tuple())
                .first->second;
#else
    bslma::Allocator *alloc = allocator();
    bsl::pair<bsl::string_view, Json> value(alloc);
    value.first = key;
    return d_members.emplace(bslmf::MovableRefUtil::move(value)).first->second;
#endif
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
    JsonObject&>::type
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
    return *this;
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS)
inline
JsonObject& JsonObject::insert(std::initializer_list<Json_MemberInitializer>
                                                                       members)
{
    for (const auto& mem : members) {
        BSLS_ASSERT(bdlde::Utf8Util::isValid(mem.key().data(),
                                             mem.key().size()));
        insert(mem.key(), mem.value());
    }
    return *this;
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
Json::Json(const JsonArray& array, bslma::Allocator *basicAllocator)
: d_value(array, basicAllocator)
{
}

inline
Json::Json(const JsonObject& object, bslma::Allocator *basicAllocator)
: d_value(object, basicAllocator)
{
}

inline
Json::Json(const JsonNumber& number, bslma::Allocator *basicAllocator)
: d_value(number, basicAllocator)
{
}

inline
Json::Json(bslmf::MovableRef<JsonArray>  array,
           bslma::Allocator             *basicAllocator)
: d_value(bslmf::MovableRefUtil::move(array), basicAllocator)
{
}

inline
Json::Json(bslmf::MovableRef<JsonObject>  object,
           bslma::Allocator              *basicAllocator)
: d_value(bslmf::MovableRefUtil::move(object), basicAllocator)
{
}

inline
Json::Json(bslmf::MovableRef<JsonNumber>  number,
           bslma::Allocator              *basicAllocator)
: d_value(bslmf::MovableRefUtil::move(number), basicAllocator)
{
}

inline
Json::Json(int value, bslma::Allocator *basicAllocator)
: d_value(JsonNumber(value), basicAllocator)
{
}

inline
Json::Json(unsigned int value, bslma::Allocator *basicAllocator)
: d_value(JsonNumber(value), basicAllocator)
{
}

inline
Json::Json(long value, bslma::Allocator *basicAllocator)
: d_value(JsonNumber(value), basicAllocator)
{
}

inline
Json::Json(unsigned long value, bslma::Allocator *basicAllocator)
: d_value(JsonNumber(value), basicAllocator)
{
}

inline
Json::Json(bsls::Types::Int64 value, bslma::Allocator *basicAllocator)
: d_value(JsonNumber(value), basicAllocator)
{
}

inline
Json::Json(bsls::Types::Uint64 value, bslma::Allocator *basicAllocator)
: d_value(JsonNumber(value), basicAllocator)
{
}

inline
Json::Json(float value, bslma::Allocator *basicAllocator)
: d_value(JsonNumber(value), basicAllocator)
{
}

inline
Json::Json(double value, bslma::Allocator *basicAllocator)
: d_value(JsonNumber(value), basicAllocator)
{
}

inline
Json::Json(bdldfp::Decimal64 value, bslma::Allocator *basicAllocator)
: d_value(JsonNumber(value), basicAllocator)
{
}

inline
Json::Json(const char *string, bslma::Allocator *basicAllocator)
: d_value(basicAllocator)
{
    BSLS_ASSERT(bdlde::Utf8Util::isValid(string));
    d_value.createInPlace<bsl::string>(string);
}

#ifndef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
inline
Json::Json(const bsl::string_view& string, bslma::Allocator *basicAllocator)
: d_value(basicAllocator)
{
    BSLS_ASSERT(bdlde::Utf8Util::isValid(string));
    d_value.createInPlace<bsl::string>(string);
}
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS
// Construct from 'Json_Initializer' is in the 'cpp' file.

inline
Json::Json(std::initializer_list<Json_Initializer>  initializer,
           bslma::Allocator                        *basicAllocator)
: d_value(basicAllocator)
{
    makeArray(JsonArray(initializer, basicAllocator));
}
#endif

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
Json& Json::operator=(long rhs)
{
    d_value.createInPlace<JsonNumber>(rhs);
    return *this;
}

inline
Json& Json::operator=(unsigned long rhs)
{
    d_value.createInPlace<JsonNumber>(rhs);
    return *this;
}

inline
Json& Json::operator=(long long rhs)
{
    d_value.createInPlace<JsonNumber>(rhs);
    return *this;
}

inline
Json& Json::operator=(unsigned long long rhs)
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
Json& Json::operator=(const JsonNull& )
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

                        // 'pushBack' methods

inline
Json& Json::pushBack(const Json& json)
{
    BSLS_ASSERT(isArray() || isNull());
    if (isNull()) {
        makeArray();
    }
    theArray().pushBack(json);
    return *this;
}

inline
Json& Json::pushBack(const JsonArray& array)
{
    BSLS_ASSERT(isArray() || isNull());
    if (isNull()) {
        makeArray();
    }
    theArray().pushBack(array);
    return *this;
}

inline
Json& Json::pushBack(const JsonObject& object)
{
    BSLS_ASSERT(isArray() || isNull());
    if (isNull()) {
        makeArray();
    }
    theArray().pushBack(object);
    return *this;
}

inline
Json& Json::pushBack(const JsonNumber& number)
{
    BSLS_ASSERT(isArray() || isNull());
    if (isNull()) {
        makeArray();
    }
    theArray().pushBack(number);
    return *this;
}

inline
Json& Json::pushBack(bslmf::MovableRef<Json> json)
{
    BSLS_ASSERT(isArray() || isNull());
    if (isNull()) {
        makeArray();
    }
    theArray().pushBack(bslmf::MovableRefUtil::move(json));
    return *this;
}

inline
Json& Json::pushBack(bslmf::MovableRef<JsonArray> array)
{
    BSLS_ASSERT(isArray() || isNull());
    if (isNull()) {
        makeArray();
    }
    theArray().pushBack(bslmf::MovableRefUtil::move(array));
    return *this;
}

inline
Json& Json::pushBack(bslmf::MovableRef<JsonObject> object)
{
    BSLS_ASSERT(isArray() || isNull());
    if (isNull()) {
        makeArray();
    }
    theArray().pushBack(bslmf::MovableRefUtil::move(object));
    return *this;
}

inline
Json& Json::pushBack(bslmf::MovableRef<JsonNumber> number)
{
    BSLS_ASSERT(isArray() || isNull());
    if (isNull()) {
        makeArray();
    }
    theArray().pushBack(bslmf::MovableRefUtil::move(number));
    return *this;
}

inline
Json& Json::pushBack(const JsonNull& value)
{
    BSLS_ASSERT(isArray() || isNull());
    if (isNull()) {
        makeArray();
    }
    theArray().pushBack(value);
    return *this;
}

inline
Json& Json::pushBack(bool value)
{
    BSLS_ASSERT(isArray() || isNull());
    if (isNull()) {
        makeArray();
    }
    theArray().pushBack(value);
    return *this;
}

inline
Json& Json::pushBack(int value)
{
    BSLS_ASSERT(isArray() || isNull());
    if (isNull()) {
        makeArray();
    }
    theArray().pushBack(value);
    return *this;
}

inline
Json& Json::pushBack(unsigned int value)
{
    BSLS_ASSERT(isArray() || isNull());
    if (isNull()) {
        makeArray();
    }
    theArray().pushBack(value);
    return *this;
}

inline
Json& Json::pushBack(long value)
{
    BSLS_ASSERT(isArray() || isNull());
    if (isNull()) {
        makeArray();
    }
    theArray().pushBack(value);
    return *this;
}

inline
Json& Json::pushBack(unsigned long value)
{
    BSLS_ASSERT(isArray() || isNull());
    if (isNull()) {
        makeArray();
    }
    theArray().pushBack(value);
    return *this;
}

inline
Json& Json::pushBack(long long value)
{
    BSLS_ASSERT(isArray() || isNull());
    if (isNull()) {
        makeArray();
    }
    theArray().pushBack(value);
    return *this;
}

inline
Json& Json::pushBack(unsigned long long value)
{
    BSLS_ASSERT(isArray() || isNull());
    if (isNull()) {
        makeArray();
    }
    theArray().pushBack(value);
    return *this;
}

inline
Json& Json::pushBack(float value)
{
    BSLS_ASSERT(isArray() || isNull());
    if (isNull()) {
        makeArray();
    }
    theArray().pushBack(value);
    return *this;
}

inline
Json& Json::pushBack(double value)
{
    BSLS_ASSERT(isArray() || isNull());
    if (isNull()) {
        makeArray();
    }
    theArray().pushBack(value);
    return *this;
}

inline
Json& Json::pushBack(bdldfp::Decimal64 value)
{
    BSLS_ASSERT(isArray() || isNull());
    if (isNull()) {
        makeArray();
    }
    theArray().pushBack(value);
    return *this;
}

inline
Json& Json::pushBack(const char *string)
{
    BSLS_ASSERT(string);
    BSLS_ASSERT(isArray() || isNull());
    if (isNull()) {
        makeArray();
    }
    theArray().pushBack(string);
    return *this;
}

inline
Json& Json::pushBack(const bsl::string_view& string)
{
    BSLS_ASSERT(isArray() || isNull());
    if (isNull()) {
        makeArray();
    }
    theArray().pushBack(string);
    return *this;
}

template <class STRING_TYPE>
inline
typename bsl::enable_if<bsl::is_same<STRING_TYPE,
                                     bsl::string>::value,
                        Json&>::type
Json::pushBack(BSLMF_MOVABLEREF_DEDUCE(STRING_TYPE) string)
{
    BSLS_ASSERT(isArray() || isNull());
    if (isNull()) {
        makeArray();
    }
    theArray().pushBack(bslmf::MovableRefUtil::move(string));
    return *this;
}

template <class RETURN_TYPE, class VISITOR>
RETURN_TYPE Json::visit(BSLS_COMPILERFEATURES_FORWARD_REF(VISITOR) visitor)
{
    typedef Json_VisitUtil Util;

    switch (type()) {
      case JsonType::e_OBJECT : {
        return Util::invokeVisitor<RETURN_TYPE>(visitor, &theObject());
                                                                      // RETURN
      } break;
      case JsonType::e_ARRAY  : {
        return Util::invokeVisitor<RETURN_TYPE>(visitor, &theArray());
                                                                      // RETURN
      } break;
      case JsonType::e_STRING : {
#ifdef BSLS_ASSERT_IS_ACTIVE
        Json_StringInvariantGuard         guard(&d_value.the<bsl::string>());
#endif
        return Util::invokeVisitor<RETURN_TYPE>(visitor,
                                                &d_value.the<bsl::string>());
                                                                      // RETURN
      } break;
      case JsonType::e_NUMBER : {
        return Util::invokeVisitor<RETURN_TYPE>(visitor, &theNumber());
                                                                      // RETURN
      } break;
      case JsonType::e_BOOLEAN: {
        return Util::invokeVisitor<RETURN_TYPE>(visitor, &theBoolean());
                                                                      // RETURN
      } break;
      case JsonType::e_NULL   : {
        return Util::invokeVisitor<RETURN_TYPE>(visitor, &theNull());
                                                                      // RETURN
      } break;
      default: {
        BSLS_ASSERT_OPT(false && "reachable");
      } break;
    }
    BSLA_UNREACHABLE;
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
template <class VISITOR>
decltype(auto) Json::visit(BSLS_COMPILERFEATURES_FORWARD_REF(VISITOR) visitor)
{
    using Util = Json_VisitUtil;

    switch (type()) {
      case JsonType::e_OBJECT : {
        return Util::invokeVisitor(visitor, &theObject());            // RETURN
      } break;
      case JsonType::e_ARRAY  : {
        return Util::invokeVisitor(visitor, &theArray());             // RETURN
      } break;
      case JsonType::e_STRING : {
#ifdef BSLS_ASSERT_IS_ACTIVE
        Json_StringInvariantGuard     guard(&d_value.the<bsl::string>());
#endif
        return Util::invokeVisitor(visitor, &d_value.the<bsl::string>());
                                                                      // RETURN
      } break;
      case JsonType::e_NUMBER : {
        return Util::invokeVisitor(visitor, &theNumber());            // RETURN
      } break;
      case JsonType::e_BOOLEAN: {
        return Util::invokeVisitor(visitor, &theBoolean());           // RETURN
      } break;
      case JsonType::e_NULL   : {
      return Util::invokeVisitor(visitor, &theNull());                // RETURN
      } break;
      default: {
        BSLS_ASSERT_OPT(false && "reachable");
      } break;
    }
    BSLA_UNREACHABLE;
}
#endif

                        // Aspects

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
    BSLS_ASSERT(isObject() || isNull());
    if (isNull()) {
        makeObject();
    }
    return theObject()[key];
}

inline
Json& Json::operator[](bsl::size_t index)
{
    BSLS_ASSERT(d_value.is<JsonArray>());
    return theArray()[index];
}

// ACCESSORS
inline
bsl::size_t Json::size() const
{
    BSLS_ASSERT(isArray() || isObject());
    return isArray() ? theArray().size() : theObject().size();
}

// BDE_VERIFY pragma: -FABC01 // not in order

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
int Json::asShort(short *result) const
{
    BSLS_ASSERT(isNumber());
    return theNumber().asShort(result);
}

inline
int Json::asInt(int *result) const
{
    BSLS_ASSERT(isNumber());
    return theNumber().asInt(result);
}

inline
int Json::asLong(long *result) const
{
    BSLS_ASSERT(isNumber());
    return theNumber().asLong(result);
}

inline
int Json::asLonglong(long long *result) const
{
    BSLS_ASSERT(isNumber());
    return theNumber().asLonglong(result);
}

inline
int Json::asInt64(bsls::Types::Int64 *result) const
{
    BSLS_ASSERT(isNumber());
    return theNumber().asInt64(result);
}

inline
int Json::asUshort(unsigned short *result) const
{
    BSLS_ASSERT(isNumber());
    return theNumber().asUshort(result);
}

inline
int Json::asUint(unsigned int *result) const
{
    BSLS_ASSERT(isNumber());
    return theNumber().asUint(result);
}

inline
int Json::asUlong(unsigned long  *result) const
{
    BSLS_ASSERT(isNumber());
    return theNumber().asUlong(result);
}

inline
int Json::asUlonglong(unsigned long long  *result) const
{
    BSLS_ASSERT(isNumber());
    return theNumber().asUlonglong(result);
}

inline
int Json::asUint64(bsls::Types::Uint64 *result) const
{
    BSLS_ASSERT(isNumber());
    return theNumber().asUint64(result);
}

// BDE_VERIFY pragma: +FABC01 // not in order

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

template <class RETURN_TYPE, class VISITOR>
RETURN_TYPE Json::visit(BSLS_COMPILERFEATURES_FORWARD_REF(VISITOR) visitor)
                                                                          const
{
    typedef Json_VisitUtil Util;

    switch (type()) {
      case   JsonType::e_OBJECT : {
        return Util::invokeVisitor<RETURN_TYPE>(visitor, theObject());
                                                                      // RETURN
      } break;
      case   JsonType::e_ARRAY  : {
        return Util::invokeVisitor<RETURN_TYPE>(visitor, theArray());
                                                                      // RETURN
      } break;
      case   JsonType::e_STRING : {
        return Util::invokeVisitor<RETURN_TYPE>(visitor, theString());
                                                                      // RETURN
      } break;
      case   JsonType::e_NUMBER : {
        return Util::invokeVisitor<RETURN_TYPE>(visitor, theNumber());
                                                                      // RETURN
      } break;
      case   JsonType::e_BOOLEAN: {
        return Util::invokeVisitor<RETURN_TYPE>(visitor, theBoolean());
                                                                      // RETURN
      } break;
      case   JsonType::e_NULL   : {
        return Util::invokeVisitor<RETURN_TYPE>(visitor, theNull());  // RETURN
      } break;
      default: {
        BSLS_ASSERT_OPT(false && "reachable");
      } break;
    }
    BSLA_UNREACHABLE;
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
template <class VISITOR>
decltype(auto) Json::visit(BSLS_COMPILERFEATURES_FORWARD_REF(VISITOR) visitor)
                                                                          const
{
    using Util = Json_VisitUtil;

    switch (type()) {
      case   JsonType::e_OBJECT : {
        return Util::invokeVisitor(visitor, theObject());             // RETURN
      } break;
      case   JsonType::e_ARRAY  : {
        return Util::invokeVisitor(visitor, theArray());              // RETURN
      } break;
      case   JsonType::e_STRING : {
        return Util::invokeVisitor(visitor, theString());             // RETURN
      } break;
      case   JsonType::e_NUMBER : {
        return Util::invokeVisitor(visitor, theNumber());             // RETURN
      } break;
      case   JsonType::e_BOOLEAN: {
        return Util::invokeVisitor(visitor, theBoolean());            // RETURN
      } break;
      case   JsonType::e_NULL   : {
        return Util::invokeVisitor(visitor, theNull());               // RETURN
      } break;
      default: {
        BSLS_ASSERT_OPT(false && "reachable");
      } break;
    }
    BSLA_UNREACHABLE;
}
#endif
                        // Aspects

inline
bslma::Allocator *Json::allocator() const BSLS_KEYWORD_NOEXCEPT
{
    return d_value.getAllocator();
}

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

inline
bool bdljsn::operator==(const JsonArray& lhs, const Json& rhs)
{
    return rhs.isArray() && rhs.theArray() == lhs;
}

inline
bool bdljsn::operator==(const JsonObject& lhs, const Json& rhs)
{
    return rhs.isObject() && rhs.theObject() == lhs;
}

inline
bool bdljsn::operator==(const JsonNumber& lhs, const Json& rhs)
{
    return rhs.isNumber() && rhs.theNumber() == lhs;
}

inline
bool bdljsn::operator==(const JsonNull& , const Json& rhs)
{
    return rhs.isNull();
}

inline
bool bdljsn::operator==(bool lhs, const Json& rhs)
{
    return rhs.isBoolean() && rhs.theBoolean() == lhs;
}

inline
bool bdljsn::operator==(int lhs, const Json& rhs)
{
    return rhs.isNumber() && rhs.theNumber() == JsonNumber(lhs);
}

inline
bool bdljsn::operator==(unsigned int lhs, const Json& rhs)
{
    return rhs.isNumber() && rhs.theNumber() == JsonNumber(lhs);
}

inline
bool bdljsn::operator==(long lhs, const Json& rhs)
{
    return rhs.isNumber() && rhs.theNumber() == JsonNumber(lhs);
}

inline
bool bdljsn::operator==(unsigned long lhs, const Json& rhs)
{
    return rhs.isNumber() && rhs.theNumber() == JsonNumber(lhs);
}

inline
bool bdljsn::operator==(long long lhs, const Json& rhs)
{
    return rhs.isNumber() && rhs.theNumber() == JsonNumber(lhs);
}

inline
bool bdljsn::operator==(unsigned long long lhs, const Json& rhs)
{
    return rhs.isNumber() && rhs.theNumber() == JsonNumber(lhs);
}

inline
bool bdljsn::operator==(float lhs, const Json& rhs)
{
    return rhs.isNumber() && rhs.theNumber() == JsonNumber(lhs);
}

inline
bool bdljsn::operator==(double lhs, const Json& rhs)
{
    return rhs.isNumber() && rhs.theNumber() == JsonNumber(lhs);
}

inline
bool bdljsn::operator==(bdldfp::Decimal64 lhs, const Json& rhs)
{
    return rhs.isNumber() && rhs.theNumber() == JsonNumber(lhs);
}

inline
bool bdljsn::operator==(const char *lhs, const Json& rhs)
{
    BSLS_ASSERT(lhs);
    return rhs.isString() && rhs.theString() == lhs;
}

inline
bool bdljsn::operator==(const bsl::string_view& lhs, const Json& rhs)
{
    return rhs.isString() && rhs.theString() == lhs;
}

inline
bool bdljsn::operator==(const Json& lhs, const JsonArray& rhs)
{
    return lhs.isArray() && lhs.theArray() == rhs;
}

inline
bool bdljsn::operator==(const Json& lhs, const JsonObject& rhs)
{
    return lhs.isObject() && lhs.theObject() == rhs;
}

inline
bool bdljsn::operator==(const Json& lhs, const JsonNumber& rhs)
{
    return lhs.isNumber() && lhs.theNumber() == rhs;
}

inline
bool bdljsn::operator==(const Json& lhs, const JsonNull& )
{
    return lhs.isNull();
}

inline
bool bdljsn::operator==(const Json& lhs, bool rhs)
{
    return lhs.isBoolean() && lhs.theBoolean() == rhs;
}

inline
bool bdljsn::operator==(const Json& lhs, int rhs)
{
    return lhs.isNumber() && lhs.theNumber() == JsonNumber(rhs);
}

inline
bool bdljsn::operator==(const Json& lhs, unsigned int rhs)
{
    return lhs.isNumber() && lhs.theNumber() == JsonNumber(rhs);
}

inline
bool bdljsn::operator==(const Json& lhs, long rhs)
{
    return lhs.isNumber() && lhs.theNumber() == JsonNumber(rhs);
}

inline
bool bdljsn::operator==(const Json& lhs, unsigned long rhs)
{
    return lhs.isNumber() && lhs.theNumber() == JsonNumber(rhs);
}

inline
bool bdljsn::operator==(const Json& lhs, long long rhs)
{
    return lhs.isNumber() && lhs.theNumber() == JsonNumber(rhs);
}

inline
bool bdljsn::operator==(const Json& lhs, unsigned long long rhs)
{
    return lhs.isNumber() && lhs.theNumber() == JsonNumber(rhs);
}

inline
bool bdljsn::operator==(const Json& lhs, float rhs)
{
    return lhs.isNumber() && lhs.theNumber() == JsonNumber(rhs);
}

inline
bool bdljsn::operator==(const Json& lhs, double rhs)
{
    return lhs.isNumber() && lhs.theNumber() == JsonNumber(rhs);
}

inline
bool bdljsn::operator==(const Json& lhs, bdldfp::Decimal64 rhs)
{
    return lhs.isNumber() && lhs.theNumber() == JsonNumber(rhs);
}

inline
bool bdljsn::operator==(const Json& lhs, const char *rhs)
{
    BSLS_ASSERT(rhs);
    return lhs.isString() && lhs.theString() == rhs;
}

inline
bool bdljsn::operator==(const Json& lhs, const bsl::string_view& rhs)
{
    return lhs.isString() && lhs.theString() == rhs;
}

inline
bool bdljsn::operator!=(const JsonArray& lhs, const Json& rhs)
{
    return !rhs.isArray() || rhs.theArray() != lhs;
}

inline
bool bdljsn::operator!=(const JsonObject& lhs, const Json& rhs)
{
    return !rhs.isObject() || rhs.theObject() != lhs;
}

inline
bool bdljsn::operator!=(const JsonNumber& lhs, const Json& rhs)
{
    return !rhs.isNumber() || rhs.theNumber() != lhs;
}

inline
bool bdljsn::operator!=(const JsonNull& , const Json& rhs)
{
    return !rhs.isNull();
}

inline
bool bdljsn::operator!=(bool lhs, const Json& rhs)
{
    return !rhs.isBoolean() || rhs.theBoolean() != lhs;
}

inline
bool bdljsn::operator!=(int lhs, const Json& rhs)
{
    return !rhs.isNumber() || rhs.theNumber() != JsonNumber(lhs);
}

inline
bool bdljsn::operator!=(unsigned int lhs, const Json& rhs)
{
    return !rhs.isNumber() || rhs.theNumber() != JsonNumber(lhs);
}

inline
bool bdljsn::operator!=(long lhs, const Json& rhs)
{
    return !rhs.isNumber() || rhs.theNumber() != JsonNumber(lhs);
}

inline
bool bdljsn::operator!=(unsigned long lhs, const Json& rhs)
{
    return !rhs.isNumber() || rhs.theNumber() != JsonNumber(lhs);
}

inline
bool bdljsn::operator!=(long long lhs, const Json& rhs)
{
    return !rhs.isNumber() || rhs.theNumber() != JsonNumber(lhs);
}

inline
bool bdljsn::operator!=(unsigned long long lhs, const Json& rhs)
{
    return !rhs.isNumber() || rhs.theNumber() != JsonNumber(lhs);
}

inline
bool bdljsn::operator!=(float lhs, const Json& rhs)
{
    return !rhs.isNumber() || rhs.theNumber() != JsonNumber(lhs);
}

inline
bool bdljsn::operator!=(double lhs, const Json& rhs)
{
    return !rhs.isNumber() || rhs.theNumber() != JsonNumber(lhs);
}

inline
bool bdljsn::operator!=(bdldfp::Decimal64 lhs, const Json& rhs)
{
    return !rhs.isNumber() || rhs.theNumber() != JsonNumber(lhs);
}

inline
bool bdljsn::operator!=(const char *lhs, const Json& rhs)
{
    BSLS_ASSERT(lhs);
    return !rhs.isString() || rhs.theString() != lhs;
}

inline
bool bdljsn::operator!=(const bsl::string_view& lhs, const Json& rhs)
{
    return !rhs.isString() || rhs.theString() != lhs;
}

inline
bool bdljsn::operator!=(const Json& lhs, const JsonArray& rhs)
{
    return !lhs.isArray() || lhs.theArray() != rhs;
}

inline
bool bdljsn::operator!=(const Json& lhs, const JsonObject& rhs)
{
    return !lhs.isObject() || lhs.theObject() != rhs;
}

inline
bool bdljsn::operator!=(const Json& lhs, const JsonNumber& rhs)
{
    return !lhs.isNumber() || lhs.theNumber() != rhs;
}

inline
bool bdljsn::operator!=(const Json& lhs, const JsonNull& )
{
    return !lhs.isNull();
}

inline
bool bdljsn::operator!=(const Json& lhs, bool rhs)
{
    return !lhs.isBoolean() || lhs.theBoolean() != rhs;
}

inline
bool bdljsn::operator!=(const Json& lhs, int rhs)
{
    return !lhs.isNumber() || lhs.theNumber() != JsonNumber(rhs);
}

inline
bool bdljsn::operator!=(const Json& lhs, unsigned int rhs)
{
    return !lhs.isNumber() || lhs.theNumber() != JsonNumber(rhs);
}

inline
bool bdljsn::operator!=(const Json& lhs, long rhs)
{
    return !lhs.isNumber() || lhs.theNumber() != JsonNumber(rhs);
}

inline
bool bdljsn::operator!=(const Json& lhs, unsigned long rhs)
{
    return !lhs.isNumber() || lhs.theNumber() != JsonNumber(rhs);
}

inline
bool bdljsn::operator!=(const Json& lhs, long long rhs)
{
    return !lhs.isNumber() || lhs.theNumber() != JsonNumber(rhs);
}

inline
bool bdljsn::operator!=(const Json& lhs, unsigned long long rhs)
{
    return !lhs.isNumber() || lhs.theNumber() != JsonNumber(rhs);
}

inline
bool bdljsn::operator!=(const Json& lhs, float rhs)
{
    return !lhs.isNumber() || lhs.theNumber() != JsonNumber(rhs);
}

inline
bool bdljsn::operator!=(const Json& lhs, double rhs)
{
    return !lhs.isNumber() || lhs.theNumber() != JsonNumber(rhs);
}

inline
bool bdljsn::operator!=(const Json& lhs, bdldfp::Decimal64 rhs)
{
    return !lhs.isNumber() || lhs.theNumber() != JsonNumber(rhs);
}

inline
bool bdljsn::operator!=(const Json& lhs, const char *rhs)
{
    BSLS_ASSERT(rhs);
    return !lhs.isString() || lhs.theString() != rhs;
}

inline
bool bdljsn::operator!=(const Json& lhs, const bsl::string_view& rhs)
{
    return !lhs.isString() || lhs.theString() != rhs;
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

namespace bdljsn {

#ifdef BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS

                           // ----------------------
                           // class Json_Initializer
                           // ----------------------

// CREATORS
inline
Json_Initializer::Json_Initializer()
: d_storage(JsonNull())
{
}

inline
Json_Initializer::Json_Initializer(const JsonNull &)
: d_storage(JsonNull())
{
}

inline
Json_Initializer::Json_Initializer(bool b)
: d_storage(b)
{
}

template <class NUMBER, class>
inline
Json_Initializer::Json_Initializer(NUMBER num) {

    BSLMF_ASSERT(bsl::is_unsigned<NUMBER>::value ||
                 bsl::is_signed<NUMBER>::value);

    if (bsl::is_unsigned<NUMBER>::value) {
        d_storage = static_cast<unsigned long long>(num);
    }
    else {
        d_storage = static_cast<long long>(num);
    }
}

inline
Json_Initializer::Json_Initializer(float f)
: d_storage(static_cast<double>(f))
{
}

inline
Json_Initializer::Json_Initializer(double d)
: d_storage(d)
{
}

inline
Json_Initializer::Json_Initializer(bdldfp::Decimal64 value)
: d_storage(value)
{
}

inline
Json_Initializer::Json_Initializer(const char *p)
: d_storage(bsl::string_view(p))
{
}

inline
Json_Initializer::Json_Initializer(const bsl::string &s)
: d_storage(bsl::string_view(s))
{
}

inline
Json_Initializer::Json_Initializer(const std::string &s)
: d_storage(bsl::string_view(s.data(), s.size()))
{
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR_STRING
inline
Json_Initializer::Json_Initializer(const std::pmr::string &s)
: d_storage(bsl::string_view(s.data(), s.size()))
{
}
#endif

inline
Json_Initializer::Json_Initializer(const bsl::string_view& sv)
: d_storage(sv)
{
}

#if  defined(BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY) \
&&  !defined(BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY)
inline
Json_Initializer::Json_Initializer(const std::string_view& sv)
: d_storage(bsl::string_view(sv.data(), sv.size()))
{
}
#endif

inline
Json_Initializer::Json_Initializer(
                                    std::initializer_list<Json_Initializer> il)
: d_storage(il)
{
}

inline
Json_Initializer::Json_Initializer(const JsonObject& jObj)
: d_storage(&jObj)
{
}

inline
Json_Initializer::Json_Initializer(const JsonArray& jArr)
: d_storage(&jArr)
{
}

inline
Json_Initializer::Json_Initializer(const JsonNumber& jNum)
: d_storage(&jNum)
{
}

inline
Json_Initializer::Json_Initializer(const Json& json)
: d_storage(&json)
{
}

// ACCESSORS
inline
const Json_Initializer::Storage&
Json_Initializer::get_storage() const
{
    return d_storage;
}

                        // ----------------------------
                        // class Json_MemberInitializer
                        // ----------------------------

// CREATORS
inline
Json_MemberInitializer::Json_MemberInitializer(const char              *s,
                                               const Json_Initializer&  ji)
: d_first (s)
, d_second(ji)
{
}

inline
Json_MemberInitializer::Json_MemberInitializer(const bsl::string&      s,
                                               const Json_Initializer& ji)
: d_first (s.data(), s.size())
, d_second(ji)
{
}

inline
Json_MemberInitializer::Json_MemberInitializer(const std::string&      s,
                                               const Json_Initializer& ji)
: d_first (s.data(), s.size())
, d_second(ji)
{
}

inline
Json_MemberInitializer::Json_MemberInitializer(const bsl::string_view& sv,
                                               const Json_Initializer& ji)
: d_first (sv.data(), sv.size())
, d_second(ji)
{
}

#if  defined(BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY) \
&&  !defined(BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY)
inline
Json_MemberInitializer::Json_MemberInitializer(const std::string_view& sv,
                                               const Json_Initializer& ji)
: d_first (sv.data(), sv.size())
, d_second(ji)
{
}
#endif

// ACCESSORS
inline
bsl::string_view Json_MemberInitializer::key() const
{
    return d_first;
}

inline
const Json_Initializer& Json_MemberInitializer::value() const
{
    return d_second;
}

#endif

                                 // ---------------------
                                 // struct Json_VisitUtil
                                 // ---------------------

template <class RETURN_TYPE,
          class VISITOR,
          class ARGUMENT>
RETURN_TYPE Json_VisitUtil::invokeVisitor(VISITOR         *visitor,
                                          const ARGUMENT&  argument)
{
    BSLS_ASSERT(visitor);
    return (*visitor)(argument);
}

template <class RETURN_TYPE,
          class VISITOR,
          class ARGUMENT>
RETURN_TYPE Json_VisitUtil::invokeVisitor(VISITOR&        visitor,
                                          const ARGUMENT& argument)
{
    return visitor(argument);
}

template <class RETURN_TYPE,
          class VISITOR,
          class ARGUMENT>
RETURN_TYPE Json_VisitUtil::invokeVisitor(const VISITOR&  visitor,
                                          const ARGUMENT& argument)
{
    return visitor(argument);
}

template <class RETURN_TYPE,
          class VISITOR,
          class ARGUMENT>
RETURN_TYPE Json_VisitUtil::invokeVisitor(
                                     BSLMF_MOVABLEREF_DEDUCE(VISITOR) visitor,
                                     const ARGUMENT&                  argument)
{
  #ifdef BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES
    return         (bsl::forward<VISITOR>(visitor))(argument);
  #else
    return (bslmf::MovableRefUtil::access(visitor))(argument);
  #endif
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
template <class VISITOR,
          class ARGUMENT>
decltype(auto) Json_VisitUtil::invokeVisitor(const VISITOR&  visitor,
                                             const ARGUMENT& argument)
{
    return visitor(argument);
}

template <class VISITOR,
          class ARGUMENT>
decltype(auto) Json_VisitUtil::invokeVisitor(
                                     BSLMF_MOVABLEREF_DEDUCE(VISITOR) visitor,
                                     const ARGUMENT&                  argument)
{
  #ifdef BSLMF_MOVABLEREF_USES_RVALUE_REFERENCES
    return         (bsl::forward<VISITOR>(visitor))(argument);
  #else
    return (bslmf::MovableRefUtil::access(visitor))(argument);
  #endif
}
#endif

                        // -------------------------------
                        // class Json_StringInvariantGuard
                        // -------------------------------

// CREATORS
inline
Json_StringInvariantGuard::Json_StringInvariantGuard(bsl::string *string)
: d_string_p(string)
{
    BSLS_ASSERT(string);
}

inline
Json_StringInvariantGuard::~Json_StringInvariantGuard()
                                     BSLS_KEYWORD_NOEXCEPT_SPECIFICATION(false)
{
    if (0 == bsl::uncaught_exceptions()) {
        if (false == bdlde::Utf8Util::isValid(*d_string_p)) {
            BSLS_ASSERT_INVOKE_NORETURN(
                             "false == bdlde::Utf8Util::isValid(*d_string_p)");
        }
    }
}

}  // close package namespace
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
