// bdld_datum.h                                                       -*-C++-*-
#ifndef INCLUDED_BDLD_DATUM
#define INCLUDED_BDLD_DATUM

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide a discriminated variant type with a small footprint.
//
//@CLASSES:
//  bdld::Datum: POD type representing general-purpose values
//  bdld::DatumArrayRef: type for const ref to array of datums
//  bdld::DatumIntMapEntry: type for entry inside int-map of datums
//  bdld::DatumIntMapRef: type for const ref to int-map of datums
//  bdld::DatumMapEntry: type for entry inside map of datums
//  bdld::DatumMapRef: type for const ref to map of datums
//  bdld::DatumMutableArrayRef: type for mutable ref to array of datums
//  bdld::DatumMutableMapRef: type for mutable ref to a map of datums
//  bdld::DatumMutableMapOwningKeysRef: mutable ref to a map owning keys
//
//@SEE_ALSO: bdld_datumerror, bdld_datumudt, bdld_datumbinaryref,
//   bdld_manageddatum
//
//@DESCRIPTION: This component defines a mechanism, `bdld::Datum`, that
// provides a space-efficient discriminated union (i.e., a variant) that holds
// the value of either a scalar type (e.g., `int`, `double`, `Date`) or an
// aggregate (i.e., array or map) of `Datum` objects.  The set of possible
// types that a datum may hold is described in the {Supported Types} section.
//
// The `Datum` class is implemented as a POD-type, such that instances of the
// class are bitwise copyable and have trivial initialization, assignment and
// destruction.  The `Datum` class is also (primarily) designed to be compact,
// especially on a 32-bit platform.  Being a compact POD type, `Datum` is
// ideal for applications creating and copying very large numbers of variant
// values (the canonical use-case is for the values in a spreadsheet).
//
// However, not all representable values can be stored in-line in footprint of
// a `Datum` object itself.  Those types may require memory be allocated for
// storage.  In order to keep the footprint of a `Datum` object as small as
// possible, a `Datum` object does not hold a reference to an allocator, and so
// memory must be explicitly managed by the user of `Datum`.  See
// {Memory Management} for more details.
//
///Notion of Value
///---------------
// `Datum` has a notion of value, but is neither a value-semantic type, nor is
// it an in-core value-semantic type (see {`bsldoc_glossary`}).  A consequence
// of the `Datum` class's space-efficient design is that it does not fall
// neatly into any of the standard BDE type-classifications.  The `Datum`
// type's notion of value is expressed by its equality-operator -- notice, in
// particular, that two `Datum` objects compare equal if the values they refer
// to are the same.  However, `Datum`, as a POD, has compiler supplied copy and
// assignment operators that do not copy any of the storage a `Datum` may be
// pointing to, and only copy the address to which the `Datum` is pointing.
//
// Notice that the differing treatment of references to external data between
// the equality comparison and the copy and assignment operations violates a
// couple properties required of a value-semantic type, most obviously: "The
// value of an object of the type is independent of any modifiable state that
// is not owned exclusively by that object." (see {`bsldoc_glossary`}).
//
///Special Floating Point Values
///- - - - - - - - - - - - - - -
// Floating point data can represent special values, and of particular interest
// for `Datum` are values of NaN and infinity.  `Datum` may internally store
// NaN and infinity values in a different way than the IEEE-754 representation,
// and this section describes the resulting behavior for NaN and infinity
// values.
//
///Treatment of NaN (Not-A-Number)
///- - - - - - - - - - - - - - - -
// When storing a NaN value in a `Datum`, `Datum` guarantees only that *a* NaN
// value will be represented, but does not guarantee that the particular bit
// pattern supplied for a NaN value will be preserved.  Note that an IEEE-754
// representation for `double` allows for signaling and quiet NaN values, as
// well as a sign bit, and other bits of NaN payload data.  These non-salient
// elements of the "value" of the `double` may not be preserved (and in the
// case of signaling NaNs, cannot be preserved on some platforms).
//
///Treatment of Infinity
///- - - - - - - - - - -
// `Datum` is provides unique representations for positive and negative
// infinity.  IEEE-754 double precisions format requires also only those two
// infinity values.  (Unlike NaN values, these two infinity values have no
// non-normative bits in their representations, or signaling/quiet forms.)
//
///Immutability
///------------
// `Datum` objects are generally immutable, meaning the value stored inside a
// `Datum` object cannot be changed *except* through the assignment operation.
// A `Datum` is copy-assignable, so a `Datum` object can assigned another
// `Datum` object. On assignment, a `Datum` object is "shallow-copied".
// Meaning that the footprint of original `Datum` object is copied into the
// footprint of the destination `Datum` object, but if the `Datum` refers to
// dynamically allocated memory, only the value of the address is copied (not
// the contents of the dynamic allocation). `Datum` also exposes a `clone`
// method to "deep-copy" `Datum` objects, so that any externally allocated
// memory (except user defined types) is cloned and not shared like
// copy-assignment. See also {Deep Copying}.
//
///Memory Management
///-----------------
// A primary design goal for `Datum` is space-efficiency, particularly on
// 32-bit platforms.  In order to minimize the foot-print (i.e., the `sizeof`)
// of a `Datum` object, `Datum` does not hold a reference to the allocator that
// was used to allocate its contents.  This component provides static functions
// that allocate dynamic data structures referred to by a `Datum` object (i.e.
// the `Datum::create*` static functions).  This memory is said to be
// "externally managed" because it not released when a `Datum` object is
// destroyed, instead clients must explicitly call `Datum::destroy` on a
// `Datum` to release its memory (see {Analogy to Raw Pointers}).  The
// `bdld` package provides tools and components that can simplify the process
// of managing the memory (see `bdld_manageddatum`, and the various builder
// components like `bdld_datumarraybuilder`).
//
///Analogy to Raw Pointers
///- - - - - - - - - - - -
// A good way to understand the model for a `Datum` object's relationship to
// its data is by analogy: The relationship between a `Datum` object and the
// memory to which it refers is analogous to that of a raw-pointer and the data
// to which it points.  Where `new` and `delete` are used allocate and free
// memory a that a pointer points to, the static class methods `Datum::create*`
// and `Datum::destroy` are used to allocate and release the memory a `Datum`
// refers to.
//
// In order to create a `Datum` object a client calls one of the `create*`
// static methods on the `Datum` class.  In order to release the data a
// `Datum` holds, a client calls `destroy`.
//
// Copying, or copy assigning a `Datum` object to another behaves just like
// copying a raw pointer.  This copy does not allocate or deallocate data.
// That also means assigning to a datum object is not safe if the `Datum` being
// assigned to refers to dynamically allocated memory, and there isn't a (user
// controlled) strategy in place to release that memory.
//
/// Deep Copying
///- - - - - - -
// `Datum` exposes a `clone` method that "deep-copies" `Datum` objects, so that
// any dynamically or externally referenced memory is cloned and not shared
// like it would be when using a copy or copy-assignment operation.  The
// exception is {User Defined Types} as they are opaque, so `Datum` has no way
// to deep-copy them.
//
// The purpose of `clone` is to create an independent copy of the content of
// any `Datum`, which also includes `Datum` values where `isExternalreference`
// returns `true` (except of course UDTs, as mentioned above).  Cloning a
// reference to a string results in an owned string, not a reference to a
// string, with the cloned `Datum` object's `isExternalReference` returning
// `false`.  When cloning a map with keys that are references to external
// strings the clone will have deep copies of those string keys, it will become
// a map with owned keys.  This behavior is intentional.  The deep-copy
// operation (`clone`) is designed to ensure that the lifetime of the new clone
// does not, in any way, depend on the lifetime of the original `Datum`, or any
// data that `Datum` may have referenced.  So (except for UDTs), if a `Datum`
// is cloned, the original `Datum` can be destroyed without any effect on the
// cloned `Datum`.
//
///Creating a Datum that Requires No Allocation
/// - - - - - - - - - - - - - - - - - - - - - -
// Datum's containing certain types of scalar values do not require any memory
// allocation, so their factory functions do *not* take an allocator.  These
// values are small enough that they can always fit inside of the footprint of
// the `Datum` object itself.
// ```
// Datum boolean = Datum::createBoolean(true);   // Create a boolean datum
// Datum integer = Datum::createInteger(7);      // Create a integer
// Datum    real = Datum::createDouble(2.0);     // Create a double
// ```
//
///Creating a Datum that *May* Require Allocation
/// - - - - - - - - - - - - - - - - - - - - - - -
// Datum objects containing certain types *may* (or *may*-*not*!) require
// memory allocation, so their creation functions *require* an allocator:
// ```
// bslma::Allocator *allocator = bslma::Default::defaultAllocator();
// Datum datetime = Datum::createDatetime(bdlt::Datetime(), allocator);
// Datum int64    = Datum::createInteger64(1LL, allocator);
// ```
// In the example above, `createDatetime` takes an allocator, but may not
// allocate memory.  Depending on the value of the `Datetime`, a `Datum` might
// either store the value within the footprint of the `Datum` (requiring no
// allocation) or allocate external storage.  The situations in which creation
// functions taking an allocator do, and do not, actually allocate memory is
// *implementation*-*defined*.
//
// Clients of `Datum` should treat any creation function taking an allocator
// *as-if* it allocated memory, and eventually call `Datum::destroy` on the
// resulting `Datum`, even though in some instances memory allocation may not
// be required.
//
///Destroying a `Datum` Object
///- - - - - - - - - - - - - -
// The contents of a `Datum` object are destroyed using the static method
// `destroy`.  For example:
// ```
// bslma::Allocator *allocator = bslma::Default::defaultAllocator();
// Datum datetime = Datum::createDatetime(bdlt::Datetime(), allocator);
//
// Datum::destroy(datetime, allocator);
//    // 'datetime' now refers to deallocated memory.  It cannot be used
//    // used unless it is assigned a new value.
// ```
// Notice that the destroyed `Datum` again behaves similar to a raw-pointer
// that has been deallocated: the destroyed `Datum` refers to garbage and must
// be assigned a new value before it can be used.
//
// For aggregate types -- i.e., maps and arrays -- `destroy` will recursively
// call `destroy` on the `Datum` objects that compose the aggregate.  The
// exception to this is references to external arrays (discussed below).
//
// The `destroy` method does not nothing for {User Defined Types} as they are
// opaque, unknown, for `Datum`.
//
///References to External Strings and Arrays
///- - - - - - - - - - - - - - - - - - - - -
// Although a `Datum` does not own memory in the traditional sense, a call to
// `Datum::destroy` will release the memory to which that `Datum` refers.
// However, a `Datum` object also allows a user to create a `Datum` referring
// to an externally managed array or string.  For a `Datum` having a reference
// to an external string or array, the `isExternalReference` method will return
// `true` and `Datum::destroy` will not deallocate memory for the data;
// otherwise, `isExternalReference` will return `false` and `Datum::destroy`
// will deallocate memory for the data.
//
// For example, to create a `Datum` for an externally managed string:
// ```
// Datum externalStringRef = Datum::createStringRef("text", allocator);
// ```
// Notice that the supplied `allocator` is *not* used to allocate memory in
// order copy the contents of the string, but *may* (or *may*-*not*) be used to
// allocate meta-data that the `Datum` stores about the string (e.g., the
// string's length).
//
// To create a `Datum` that is responsible for the memory of a string:
// ```
// Datum managedString = Datum::copyString("text", allocator);
// ```
// Here the contents of the string are copied and managed by the created
// datum, and later released by `Datum::destroy`.
//
// External references to arrays and strings are important for efficiently
// handling memory allocations in situations where a string or array is
// externally supplied (e.g., as input to a function) and will clearly outlive
// the `Datum` object being created (e.g., a `Datum` variable within the scope
// of that function).
//
// In general factory methods of the form `create*Ref` create a reference to
// external data that the `Datum` is not responsible for, while `copy*`
// methods copy the data and the resulting `Datum` is responsible for the
// allocated memory.
//
///Supported Types
///---------------
// The table below describes the set of types that a `Datum` may be.
//
// ```
//                       external   requires
// dataType              reference  allocation  Description
// --------              ---------  ----------  -----------
// e_NIL                 no         no          null value
// e_INTEGER             no         no          integer value
// e_DOUBLE              no         no          double value
// e_STRING              maybe      maybe       string value
// e_BOOLEAN             no         no          boolean value
// e_ERROR               no         maybe       error value
// e_DATE                no         no          date value
// e_TIME                no         no          time value
// e_DATETIME            no         maybe       date+time value
// e_DATETIME_INTERVAL   no         maybe       date+time interval value
// e_INTEGER64           no         maybe       64-bit integer value
// e_USERDEFINED         always     maybe       pointer to a user-defined obj
// e_BINARY              no         maybe       binary data
// e_DECIMAL64           no         maybe       Decimal64
//
//                       external   requires
// dataType              reference  allocation  Description
// --------              ---------  ----------  -----------
// e_ARRAY               maybe      maybe       array
// e_MAP                 no         maybe       map keyed by string values
// e_INT_MAP             no         maybe       map keyed by 32-bit int values
// ```
// * *dataType* - the value returned by the `type()`
// * *external-reference* - whether `isExternalReference` will return `true`,
//   in which case `Datum::destroy` will not release the externally
//   referenced data (see
//   [](bdld_datum#References to External Strings and Arrays))
// * *requires-allocation* - whether a `Datum` referring to this type requires
//   memory allocation.  Note that for externally represented string or
//   arrays, meta-data may still need to be allocated.
//
///User Defined Types
/// - - - - - - - - -
// `Datum` exposes a type `DatumUdt` with which a user can arbitrarily expand
// the set of types a `Datum` can support.  A `DatumUdt` object hold a void
// pointer, and an integer value identifying the type.  A `DatumUdt` object is
// always treated as an external reference, and the memory it refers to is not
// released by `Datum::destroy`, or deep-copied by `clone`.  The meaning of the
// integer type identifier is determined by the application, which is
// responsible for ensuring the set of "user-defined" type identifiers remains
// unique.  From the viewpoint of `Datum` a UDT is an opaque pointer with an
// integer value that holds no defined meaning.  In that sense it is more akin
// akin to a `void` pointer than to any of the other kind of values a `Datum`
// may hold.  All knowledge of what the pointer and integer value means is
// elsewhere, in the application that created the UDT.
//
///Map and IntMap Types
/// - - - - - - - - - -
// Datum provides two `map` types, map (datatype `e_MAP`) and int-map (
// datatype `e_INT_MAP`).  These types provide a mapping of key to value, as
// represented by a sequence of key-value pairs (and are not directly related
// to `std::map`).  The key types for map and int-map are `bslstl::StringRef`
// and `int` respectively, and the value is always a `Datum`.  Both map types
// keep track of whether they are sorted by key.  Key-based lookup is done via
// the `find` function.  If the map is in a sorted state, `find` has O(logN)
// complexity and `find` is O(N) otherwise (where N is the number of elements
// in the map).  If entries with duplicate keys are present, which matching
// entry will be found is unspecified.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of `bdld::Datum`
///- - - - - - - - - - - - - - - - - - -
// This example illustrates the construction, manipulation and lifecycle of
// datums.  Datums are created via a set of static methods called `createTYPE`,
// `copyTYPE` or `adoptTYPE` where TYPE is one of the supported types.  The
// creation methods take a value and sometimes an allocator.
//
// First, we create an allocator that will supply dynamic memory needed for the
// `Datum` objects being created:
// ```
// bslma::TestAllocator oa("object");
// ```
// Then, we create a `Datum`, `number`, having an integer value of `3`:
// ```
// Datum number = Datum::createInteger(3);
// ```
// Next, we verify that the created object actually represents an integer value
// and verify that the value was set correctly:
// ```
// assert(true == number.isInteger());
// assert(3    == number.theInteger());
// ```
// Note that this object does not allocate any dynamic memory on any supported
// platforms and thus we do not need to explicitly destroy this object to
// release any dynamic memory.
//
// Then, we create a `Datum`, `cityName`, having the string value "Boston":
// ```
// Datum cityName = Datum::copyString("Boston", strlen("Boston"), &oa);
// ```
// Note, that the `copyString` makes a copy of the specified string and will
// allocate memory to hold the copy.  Whether the copy is stored in the object
// internal storage buffer or in memory obtained from the allocator depends on
// the length of the string and the platform.
//
// Next, we verify that the created object actually represents a string value
// and verify that the value was set correctly:
// ```
// assert(true     == cityName.isString());
// assert("Boston" == cityName.theString());
// ```
// Finally, we destroy the `cityName` object to deallocate memory used to hold
// string value:
// ```
// Datum::destroy(cityName, &oa);
// ```
//
///Example 2: Creating a `Datum` Referring to an Array of `Datum` Objects
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates the construction of the `Datum` object referring
// to an existing array of `Datum` object.
//
// First, we create array of the `Datum` object:
// ```
// const char theDay[] = "Birthday";
// const Datum array[2] = { Datum::createDate(bdlt::Date(2015, 10, 15)),
//                          Datum::createStringRef(StringRef(theDay), &oa) };
// ```
// Note, that in this case, the second element of the array does not make a
// copy of the string, but represents a string reference.
//
// Then, we create a `Datum` that refers to the array of Datums:
// ```
// const Datum arrayRef = Datum::createArrayReference(array, 2, &oa);
// ```
// Next, we verify that the created `Datum` represents the array value and that
// elements of this array can be accessed.  We also verify that the object
// refers to external data:
// ```
// assert(true == arrayRef.isArray());
// assert(true == arrayRef.isExternalReference());
// assert(2    == arrayRef.theArray().length());
// assert(array[0] == arrayRef.theArray().data()[0]);
// assert(array[1] == arrayRef.theArray().data()[1]);
// ```
// Then, we call `destroy` on `arrayRef`, releasing any memory it may have
// allocated, and verify that the external array is intact:
// ```
// Datum::destroy(arrayRef, &oa);
//
// assert(bdlt::Date(2015, 10, 15) == array[0].theDate());
// assert("Birthday"               == array[1].theString());
// ```
// Finally, we need to deallocate memory that was potentially allocated for the
// (external) `Datum` string in the external `array`:
// ```
// Datum::destroy(array[1], &oa);
// ```
//
///Example 3: Creating a `Datum` with an Array Value
///- - - - - - - - - - - - - - - - - - - - - - - - -
// The following example illustrates the construction of an owned array of
// datums.
//
// > **WARNING**
// >> Using corresponding builder components is a preferred way of
// >> constructing `Datum` array objects.  This example shows how a
// >> user-facing builder component might use the primitives provided in
// >> `bdld_datum`.
//
// First we create an array of datums:
// ```
// DatumMutableArrayRef bartArray;
// Datum::createUninitializedArray(&bartArray, 3, &oa);
// bartArray.data()[0] = Datum::createStringRef("Bart", &oa);
// bartArray.data()[1] = Datum::createStringRef("Simpson", &oa);
// bartArray.data()[2] = Datum::createInteger(10);
// *bartArray.length() = 3;
// ```
// Then, we construct the Datum that holds the array itself:
// ```
// Datum bart = Datum::adoptArray(bartArray);
// ```
// Note that after the `bartArray` has been adopted, the `bartArray` object can
// be destroyed without invalidating the array contained in the datum.
//
// A DatumArray may be adopted by only one datum. If the DatumArray is not
// adopted, it must be destroyed via `disposeUnitializedArray`.
//
// Now, we can access the contents of the array through the datum:
// ```
// assert(3      == bart.theArray().length());
// assert("Bart" == bart.theArray()[0].theString());
// ```
// Finally, we destroy the datum, which releases all memory associated with the
// array:
// ```
// Datum::destroy(bart, &oa);
// ```
// Note that the same allocator must be used to create the array, the
// elements, and to destroy the datum.
//
///Example 4: Creating a `Datum` with a Map Value
/// - - - - - - - - - - - - - - - - - - - - - - -
// The following example illustrates the construction of a map of datums
// indexed by string keys.
//
// > **WARNING**
// >> Using corresponding builder components is a preferred way of
// >> constructing `Datum` map objects.  This example shows how a user-facing
// >> builder component might use the primitives provided in `bdld_datum`.
//
// First we create a map of datums:
// ```
// DatumMutableMapRef lisaMap;
// Datum::createUninitializedMap(&lisaMap, 3, &oa);
// lisaMap.data()[0] = DatumMapEntry(StringRef("firstName"),
//                                   Datum::createStringRef("Lisa", &oa));
// lisaMap.data()[1] = DatumMapEntry(StringRef("lastName"),
//                                   Datum::createStringRef("Simpson", &oa));
// lisaMap.data()[2] = DatumMapEntry(StringRef("age"),
//                                   Datum::createInteger(8));
// *lisaMap.size() = 3;
// ```
// Then, we construct the Datum that holds the map itself:
// ```
// Datum lisa = Datum::adoptMap(lisaMap);
// ```
// Note that after the `lisaMap` has been adopted, the `lisaMap` object can be
// destroyed without invalidating the map contained in the datum.
//
// A `DatumMutableMapRef` may be adopted by only one datum. If the
// `DatumMutableMapRef` is not adopted, it must be destroyed via
// `disposeUninitializedMap`.
//
// Now, we can access the contents of the map through the datum:
// ```
// assert(3      == lisa.theMap().size());
// assert("Lisa" == lisa.theMap().find("firstName")->theString());
// ```
// Finally, we destroy the datum, which releases all memory associated with the
// array:
// ```
// Datum::destroy(lisa, &oa);
// ```
// Note that the same allocator must be used to create the map, the elements,
// and to destroy the datum.
//
///Example 5: Mass Destruction
///- - - - - - - - - - - - - -
// The following example illustrates an important idiom: the en masse
// destruction of a series of datums allocated in an arena.
// ```
// {
//     // scope
//     bsls::AlignedBuffer<200> bufferStorage;
//     bdlma::BufferedSequentialAllocator arena(bufferStorage.buffer(), 200);
//
//     Datum patty = Datum::copyString("Patty Bouvier",
//                                     strlen("Patty Bouvier"),
//                                     &arena);
//
//     Datum selma = Datum::copyString("Selma Bouvier",
//                                     strlen("Selma Bouvier"),
//                                     &arena);
//     DatumMutableArrayRef maggieArray;
//     Datum::createUninitializedArray(&maggieArray, 2, &arena);
//     maggieArray.data()[0] = Datum::createStringRef("Maggie", &arena);
//     maggieArray.data()[1] = Datum::createStringRef("Simpson", &arena);
//     *maggieArray.length() = 2;
//     Datum maggie = Datum::adoptArray(maggieArray);
//    // end of scope
// }
// ```
// Here all the allocated memory is lodged in the `arena` allocator. At the end
// of the scope the memory is freed in a single step.  Calling `destroy` for
// each datum individually is neither necessary nor permitted.
//
///Example 6: User-defined, error and binary types
///- - - - - - - - - - - - - - - - - - - - - - - -
// Imagine we are using `Datum` within an expression evaluation subsystem.
// Within that subsystem, along with the set of types defined by
// `Datum::DataType` we also need to hold `Sequence` and `Choice` types within
// `Datum` values (which are not natively represented by `Datum`).  First, we
// define the set of types used by our subsystem that are an extension to the
// types in `DatumType`:
// ```
// struct Sequence {
//     struct Sequence *d_next_p;
//     int              d_value;
// };
//
// enum ExtraExpressionTypes {
//     e_SEQUENCE = 5,
//     e_CHOICE = 6
// };
// ```
// Notice that the numeric values will be provided as the `type` attribute when
// constructing `Datum` object.
//
// Then we create a `Sequence` object, and create a `Datum` to hold it (note
// that we've created the object on the stack for clarity):
// ```
// Sequence sequence;
// const Datum datumS0 = Datum::createUdt(&sequence, e_SEQUENCE);
// assert(true == datumS0.isUdt());
// ```
// Next, we verify that the `datumS0` refers to the external `Sequence` object:
// ```
// bdld::DatumUdt udt = datumS0.theUdt();
// assert(e_SEQUENCE == udt.type());
// assert(&sequence  == udt.data());
// ```
// Then, we create a `Datum` to hold a `DatumError`, consisting of an error
// code and an error description message:
// ```
// enum { e_FATAL_ERROR = 100 };
// Datum datumError = Datum::createError(e_FATAL_ERROR, "Fatal error.", &oa);
// assert(true == datumError.isError());
// DatumError error = datumError.theError();
// assert(e_FATAL_ERROR == error.code());
// assert("Fatal error." == error.message());
// Datum::destroy(datumError, &oa);
// ```
// Finally, we create a `Datum` that holds an arbitrary binary data:
// ```
// int buffer[] = { 1, 2, 3 };
// Datum datumBlob = Datum::copyBinary(buffer, sizeof(buffer), &oa);
// buffer[2] = 666;
// assert(true == datumBlob.isBinary());
// DatumBinaryRef blob = datumBlob.theBinary();
// assert(blob.size() == 3 * sizeof(int));
// assert(reinterpret_cast<const int*>(blob.data())[2] == 3);
// Datum::destroy(datumBlob, &oa);
// ```
// Note that the bytes have been copied.

#include <bdlscm_version.h>

#include <bdld_datumbinaryref.h>
#include <bdld_datumerror.h>
#include <bdld_datumudt.h>

#include <bdlb_float.h>         // 'isSignalingNan'
#include <bdlb_printmethods.h>

#include <bdldfp_decimal.fwd.h>
#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
    #include <bdldfp_decimal.h>
#endif

#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimeinterval.h>
#include <bdlt_epochutil.h>
#include <bdlt_time.h>

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    #include <bsla_deprecated.h>
#endif

#include <bslma_allocator.h>
#include <bslma_allocatorutil.h>
#include <bslma_bslallocator.h>

#include <bslmf_assert.h>
#include <bslmf_isbitwisecopyable.h>
#include <bslmf_isbitwisemoveable.h>
#include <bslmf_istriviallydefaultconstructible.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bslmf_nil.h>

#include <bsls_alignedbuffer.h>
#include <bsls_annotation.h>
#include <bsls_assert.h>
#include <bsls_performancehint.h>
#include <bsls_platform.h>
#include <bsls_review.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_climits.h>
#include <bsl_cstring.h>
#include <bsl_iosfwd.h>
#include <bsl_limits.h>
#include <bsl_string.h>
#include <bsl_utility.h>

#if !defined(BSLS_PLATFORM_CPU_32_BIT) && !defined(BSLS_PLATFORM_CPU_64_BIT)
    #error 'bdld::Datum' supports 32- or 64-bit platforms only.
    BSLS_PLATFORM_COMPILER_ERROR;
#endif

#ifdef BSLS_PLATFORM_CMP_MSVC
    #define BDLD_DATUM_FORCE_INLINE __forceinline
#else
    #define BDLD_DATUM_FORCE_INLINE inline
#endif

namespace BloombergLP {

namespace bdld {

class DatumArrayRef;
class DatumIntMapEntry;
class DatumIntMapRef;
class DatumMapEntry;
class DatumMapRef;
class DatumMutableArrayRef;
class DatumMutableIntMapRef;
class DatumMutableMapOwningKeysRef;
class DatumMutableMapRef;

/// Metafunction for use in templates to create a dependent `type` that is
/// identical to the `t_WANT_TO_BE_DEPENDENT` type specified as first
/// template argument using the `t_ALREADY_DEPENDENT` type of the user
/// template (of this metafunction).
template <class t_WANT_TO_BE_DEPENDENT, class t_ALREADY_DEPENDENT>
struct Datum_MakeDependent {
    typedef t_WANT_TO_BE_DEPENDENT type;
};

                                // ===========
                                // class Datum
                                // ===========

/// This class implements a mechanism that provides a space-efficient
/// discriminated union that holds the value of ether scalar type or an
/// aggregate of `Datum` objects.  The size of `Datum` is 8 bytes (same as a
/// `double`) on 32-bit platforms and 16 bytes on 64-bit platforms.
/// Separate representation are needed on 32 and 64 bit platforms because of
/// the differing size of a pointer (a 64-bit pointer cannot reasonably be
/// held in a 32-bit footprint).
///
/// Representation on a 32-bit Platforms: Values are stored inside an 8-byte
/// unsigned char array (`d_data`).  Any `double` value (including NaN and
/// infinity values) can be stored inside `Datum`.  When storing a value of
/// a type other than `double`, the bits in `d_data` that correspond to the
/// exponent part of a `double` value are set to 1, with the 4 bits in the
/// fraction part used to indicate the type of value stored.
///
/// Representation on 64-bit platforms:  Values are stored inside a 16 byte
/// unsigned char array (`d_data`) to store values.  The type information is
/// stored in the upper 2 bytes of the character array.  Remaining 14 bytes
/// are used to store the actual value or the pointer to the external memory
/// that holds the value.
///
/// For details on the internal representations that are used for various
/// types on 32 and 64 bit platforms, please see the implementation notes in
/// `bdld_datum.cpp`.
///
/// Datum objects are bitwise copyable and have trivial initialization,
/// assignment and destruction.  Only one of the copies of the same `Datum`
/// object can be passed to `destroy`.  The rest of those copies then become
/// invalid and it is undefined behavior to deep-copy or destroy them.
/// Although, these copies can be used on the left hand side of assignment.
class Datum {

  private:
    // TYPES
    typedef bslma::AllocatorUtil AllocUtil;

  public:
    // TYPES
    typedef bsl::allocator<> AllocatorType;

    /// Enumeration used to discriminate among the different externally-
    /// exposed types of values that can be stored inside `bdld::Datum`.
    enum DataType {
          e_NIL                  =  0  // null value
        , e_INTEGER              =  1  // integer value
        , e_DOUBLE               =  2  // double value
        , e_STRING               =  3  // string value
        , e_BOOLEAN              =  4  // boolean value
        , e_ERROR                =  5  // error value
        , e_DATE                 =  6  // date value
        , e_TIME                 =  7  // time value
        , e_DATETIME             =  8  // datetime value
        , e_DATETIME_INTERVAL    =  9  // datetime interval value
        , e_INTEGER64            = 10  // 64-bit integer value
        , e_USERDEFINED          = 11  // pointer to a user-defined object
        , e_ARRAY                = 12  // array reference
        , e_MAP                  = 13  // map reference
        , e_BINARY               = 14  // pointer to the binary data
        , e_DECIMAL64            = 15  // Decimal64
        , e_INT_MAP              = 16  // integer map reference
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        , e_REAL         BSLA_DEPRECATED = e_DOUBLE // old spelling
        , e_ERROR_VALUE  BSLA_DEPRECATED = e_ERROR  // old spelling
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
    };

    // Define `k_NUM_TYPES` to be the number of consecutively valued
    // enumerators in the range `[ e_NIL .. e_DECIMAL64 ]`.
    enum {
          k_NUM_TYPES    = 17           // number of distinct enumerated types
    };

#ifdef BSLS_PLATFORM_CPU_32_BIT
  private:
    // PRIVATE TYPES
    // 32-bit variation

    /// Enumeration used to discriminate among the different types of values
    /// that can be stored inside `bdld::Datum`.
    enum InternalDataType {

          e_INTERNAL_INF                 =  0  // +/- infinity value
        , e_INTERNAL_LONGEST_SHORTSTRING =  1  // 6 character string
        , e_INTERNAL_BOOLEAN             =  2  // boolean value
        , e_INTERNAL_SHORTSTRING         =  3  // short string value
        , e_INTERNAL_STRING              =  4  // string value
        , e_INTERNAL_DATE                =  5  // date value
        , e_INTERNAL_TIME                =  6  // time value
        , e_INTERNAL_DATETIME            =  7  // date+time value
        , e_INTERNAL_DATETIME_INTERVAL   =  8  // date+time interval value
        , e_INTERNAL_INTEGER             =  9  // integer value
        , e_INTERNAL_INTEGER64           = 10  // 64-bit integer value
        , e_INTERNAL_USERDEFINED         = 11  // pointer to a user-defined obj
        , e_INTERNAL_ARRAY               = 12  // array of datums
        , e_INTERNAL_STRING_REFERENCE    = 13  // unowned string
        , e_INTERNAL_ARRAY_REFERENCE     = 14  // unowned array of
        , e_INTERNAL_EXTENDED            = 15  // extended data types
        , e_INTERNAL_DOUBLE              = 16  // double value
    };

    /// Define `k_NUM_INTERNAL_TYPES` to be the number of consecutively
    /// valued enumerators in the range
    /// `[ e_INTERNAL_INF .. e_INTERNAL_DOUBLE ]`.
    enum {

        k_NUM_INTERNAL_TYPES             = 17  // number of internal types
    };

    /// Enumeration used to discriminate among different types of values
    /// that map on to the `e_INTERNAL_EXTENDED` discriminator value inside
    /// `bdld::Datum`.  It is used to add any new required types.
    enum ExtendedInternalDataType {
          e_EXTENDED_INTERNAL_MAP         = 0  // map of datums keyed by string
                                               // values that are not owned

        , e_EXTENDED_INTERNAL_OWNED_MAP   = 1  // map of datums keyed by string
                                               // values that are owned

        , e_EXTENDED_INTERNAL_NAN2        = 2  // NaN double value

        , e_EXTENDED_INTERNAL_ERROR       = 3  // error with code only

        , e_EXTENDED_INTERNAL_ERROR_ALLOC = 4  // error with code and
                                               // description string

        // We never need to externally allocate the reference types with the
        // 64-bit implementation because we can fit 32 bits of length inline.

        , e_EXTENDED_INTERNAL_SREF_ALLOC        = 5  // allocated string ref

        , e_EXTENDED_INTERNAL_AREF_ALLOC        = 6  // allocated array ref

        , e_EXTENDED_INTERNAL_DATETIME_ALLOC    = 7  // allocated datetime

        , e_EXTENDED_INTERNAL_DATETIME_INTERVAL_ALLOC = 8 // allocated datetime
                                                          // interval

        , e_EXTENDED_INTERNAL_INTEGER64_ALLOC   = 9  // 64-bit integer value

        , e_EXTENDED_INTERNAL_BINARY_ALLOC      = 10 // binary data

        , e_EXTENDED_INTERNAL_DECIMAL64         = 11 // Decimal64

        , e_EXTENDED_INTERNAL_DECIMAL64_SPECIAL = 12 // Decimal64 NaN of Inf

        , e_EXTENDED_INTERNAL_DECIMAL64_ALLOC   = 13 // allocated Decimal64

        , e_EXTENDED_INTERNAL_NIL               = 14 // null value

        , e_EXTENDED_INTERNAL_INT_MAP           = 15 // map of datums keyed by
                                                     // 32-bit integer values
    };

    // Define `k_NUM_EXTENDED_INTERNAL_TYPES` to be the number of
    // consecutively valued enumerators in the range
    // `[ e_EXTENDED_INTERNAL_MAP .. e_EXTENDED_INTERNAL_INT_MAP ]`.
    enum {
        k_NUM_EXTENDED_INTERNAL_TYPES = 16  // number of distinct enumerated
                                            // extended types
    };

    // PRIVATE CLASS DATA
    // 32-bit variation
    static const unsigned short k_DOUBLE_MASK = 0x7ff0U;  // mask value to be
                                                          // stored in the
                                                          // exponent part of
                                                          // `d_data` to
                                                          // indicate a special
                                                          // `double` value

    static const int k_SHORTSTRING_SIZE  = 6; // maximum size of short strings
                                              // stored in the internal storage
                                              // buffer

    static const int k_TYPE_MASK_BITS = 16;   // number of bits the internal
                                              // data type needs to be shifted
                                              // into place

    static const short k_DATETIME_OFFSET_FROM_EPOCH = 18262;
        // Number of days offset from 1970 Jan 1 used to create the epoch used
        // to determine if `Datum` stores a date-time value using dynamic
        // memory allocation or withing the `Datum` itself.  This offset, added
        // to the 1970 Jan 1 date, creates an (mid) epoch of 2020 Jan 1.
        // Given that `Datum` uses a signed 16 bits day-offset when storing
        // date-time internally, the 2020 Jan 1 epoch enables of storing
        // date-times internally the range of 1930 Apr 15 to 2109 Sept 18.
        // Note that the time part of date-time can be stored internally
        // without data loss, so that makes the no-allocation range to be
        // 1930 Apr 15 00:00:00.000000 to 2109 Sept 18 24:00:00.000000. See
        // `createDatetime` and `theDatetime` methods for the implementation.

#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
    // Check if platform is little endian.
    static const int k_EXPONENT_OFFSET  = 6;  // offset of the exponent part in
                                              // the internal storage buffer

    static const int k_EXPONENT_LSB     = k_EXPONENT_OFFSET;      // Low Byte

    static const int k_EXPONENT_MSB     = k_EXPONENT_OFFSET + 1;  // High Byte

    static const int k_DATA_OFFSET      = 0;  // offset of the data part in the
                                              // internal storage buffer

    static const int k_SHORTSTRING_OFFSET = 0;// offset of short-strings stored
                                              // in the internal storage buffer

    static const int k_SHORT_OFFSET     = 4;  // offset of (2 byte values like)
                                              // discriminator values for
                                              // extended types and information
                                              // for user-defined objects in
                                              // the internal storage buffer

    static const int k_MASK_OFFSET      = 4;  // offset of the special mask
                                              // value in the internal storage
                                              // buffer

    static const int k_NEARDATE_OFFSET  = 4;  // offset of the short date
                                              // offset from now value in the
                                              // internal storage buffer

    static const int k_TIME_OFFSET      = 0;  // offset of the time value in
                                              // the internal storage buffer
#else  // end - little endian / begin - big endian
    // Check if platform is big endian.
    static const int k_EXPONENT_OFFSET  = 0;  // offset of the exponent part in
                                              // the internal storage buffer

    static const int k_EXPONENT_LSB     = k_EXPONENT_OFFSET + 1;  // Low Byte

    static const int k_EXPONENT_MSB     = k_EXPONENT_OFFSET;     // High Byte

    static const int k_DATA_OFFSET      = 4;  // offset of the data part in the
                                              // internal storage buffer

    static const int k_SHORTSTRING_OFFSET = 2;// offset of short-strings stored
                                              // in the internal storage buffer

    static const int k_SHORT_OFFSET     = 2;  // offset of (2 byte values like)
                                              // discriminator values for
                                              // extended types and information
                                              // for user-defined objects in
                                              // the internal storage buffer

    static const int k_MASK_OFFSET      = 0;  // offset of the special mask
                                              // value in the internal storage
                                              // buffer

    static const int k_NEARDATE_OFFSET  = 2;  // offset of the short date
                                              // offset from now value in the
                                              // internal storage buffer

    static const int k_TIME_OFFSET      = 4;  // offset of the time value in
                                              // the internal storage buffer
#endif  // end - big endian

    /// Enumeration used to discriminate between the special incompressible
    /// Decimal64 values.
    enum {
        e_DECIMAL64_SPECIAL_NAN,
        e_DECIMAL64_SPECIAL_INFINITY,
        e_DECIMAL64_SPECIAL_NEGATIVE_INFINITY
    };

    // DATA
    // 32-bit variation

    struct ShortString5 {
        // Storage for a string shorter than 6 chars and its length.
#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
        char                d_chars[5]; // the string's characters
        char                d_length;   // the string's length
        unsigned short      d_exponent; // the exponent inside the double
#else  // end - little endian / begin - big endian
        unsigned short      d_exponent; // the exponent inside the double
        char                d_chars[5]; // the string's characters
        char                d_length;   // the string's length
#endif  // end - big endian
    };

    struct ShortString6 {
        // Storage for a string of exactly 6 chars. Length is implicit.
#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
        char                d_chars[6]; // the string's characters
        unsigned short      d_exponent; // the exponent inside the double
#else  // end - little endian / begin - big endian
        unsigned short      d_exponent; // the exponent inside the double
        char                d_chars[6]; // the string's characters
#endif  // end - big endian
    };

    struct TypedAccess {
        // Storage for various combinations of short, int and pointer.
        // TYPE                     FIELDS
        // ---------------          -------------------------------
        // Null                     d_short = extended type
        //
        // Boolean                  d_int = value
        //
        // Integer                  d_int = value
        //
        // String                   d_cvp = allocated memory containing copy
        // (length > 6)                     of string preceded by length
        //
        // StringRef                d_ushort = length
        // (length < USHORT_MAX)    d_cvp = pointer to the c-string
        //
        // StringRef                d_short = extended type
        // (length >= USHORT_MAX)   d_cvp = pointer to allocated memory
        //                                  containing pointer to the c-string
        //                                  preceded by c-string length
        //
        // Date                     d_int = value
        //
        // Time                     d_int = value
        //
        // Datetime                 d_short = days from now
        // (near offset)            d_int = time part
        //
        // Datetime                 d_short = extended type
        // (far offset)             d_cvp = pointer to allocated value
        //
        //
        // DatetimeInterval         d_short = upper 16 bits
        // (short)                  d_int = lower 32
        //
        // DatetimeInterval         d_short = extended type
        // (long)                   d_cvp = pointer to allocated value
        //
        // Error                    d_short = extended type
        // (code only)              d_int = value
        //
        // Error                    d_short = extended type
        // (code + error string)    d_cvp = pointer to allocated memory
        //                                  containing: code, length, c-string
        //
        // Udt                      d_ushort = udt type
        //                          d_cvp = pointer to udt object
        //
        // ArrayReference           d_ushort = length
        // (length < USHORT_MAX)    d_cvp = pointer to array
        //
        // ArrayReference
        // (length >= USHORT_MAX)   d_short = extended type
        //                          d_cvp = pointer to allocated memory
        //                                  containing: pointer to array,
        //                                  length
        //
        // Map                      d_short = extended type
        //                          d_cvp = pointer to allocated memory
        //                                  containing: length, sorted flag,
        //                                  array of map entries
        //
        // Int-map                  d_short = extended type
        //                          d_cvp = pointer to allocated memory
        //                                  containing: length, sorted flag,
        //                                  array of int-map entries
        //
        // Binary:                  d_short = extended type
        //                          d_cvp = pointer to allocated memory
        //                                  containing: length, binary copy

#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
        union {
            int             d_int;      // as integer value
            const void     *d_cvp;      // as const void* value
        };
        union {
            short           d_short;    // as signed short value
            unsigned short  d_ushort;   // as unsigned short value
        };
        unsigned short      d_exponent; // the exponent inside the double
#else  // end - little endian / begin - big endian
        unsigned short      d_exponent; // the exponent inside the double
        union {
            short           d_short;    // as signed short value
            unsigned short  d_ushort;   // as unsigned short
        };
        union {
            int             d_int;      // as integer value
            const void     *d_cvp;      // as const void* value
        };
#endif  // end - big endian
    };

    struct ExponentAccess {
        // For accessing exponent as a word, for better performance.
#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
        unsigned int        d_dummy;
        unsigned int        d_value;  // the exponent as a 32 bit word
#else  // end - little endian / begin - big endian
        unsigned int        d_value;  // the exponent as a 32 bit word
        unsigned int        d_dummy;
#endif  // end - big endian
    };

    // Internal Datum representation
    union {
        // Do not change the order of these member, otherwise the code will not
        // work properly with the clang compiler.

        char              d_data[8];  // as a byte array of internal storage

        ShortString5      d_string5;  // as a string shorter than 5 chars

        ShortString6      d_string6;  // as a string of exactly 6 chars

        TypedAccess       d_as;       // as a combination of pointer, int and
                                      // short

        ExponentAccess    d_exp;      // as the exponent as a 32 bit word

        double            d_double;   // as a double value
    };

    // PRIVATE CLASS METHODS
    // 32-bit variation

    // Return a datum by copying the specified `data` of the specified
    // `type`.  Note that the pointer value in `data` is copied and the
    // pointed object is not cloned.
    static Datum createExtendedDataObject(ExtendedInternalDataType  type,
                                          void                     *data);
    static Datum createExtendedDataObject(ExtendedInternalDataType  type,
                                          int                       data);

    // PRIVATE ACCESSORS
    // 32-bit variation

    /// Return the extended type of the value stored in this object (which
    /// cannot be represented by the 4-bit discriminator `InternalDataType`)
    /// as one of the enumeration values defined in
    /// `ExtendedInternalDataType`.
    ExtendedInternalDataType extendedInternalType() const;

    /// Return the type of the value stored in this object as one of the
    /// enumeration values defined in `DataType` (mapped from the
    /// `ExtendedInternalDataType` value).
    DataType typeFromExtendedInternalType() const;

    /// Return the 64-bit integer value stored in the allocated storage.
    bsls::Types::Int64 theLargeInteger64() const;

    // Return the array referenced by this object.  The behavior is
    // undefined unless this object references an array with `length >=
    // USHORT_MAX`.
    DatumArrayRef theLongArrayReference() const;

    /// Return the short string value stored in this object as a
    /// `bslstl::StringRef` object.  The behavior is undefined unless this
    /// object actually stores a short string value.
    bslstl::StringRef theLongestShortString() const;

    /// Return the string referenced by this object.  The behavior is
    /// undefined unless this object holds a reference to a string with
    /// `length >= USHORT_MAX`.
    bslstl::StringRef theLongStringReference() const;

    /// Return the 64-bit integer value stored inline in this object.
    bsls::Types::Int64 theSmallInteger64() const;

#else // end - 32 bit / begin - 64 bit
  private:
    // PRIVATE TYPES


    // Enumeration used to discriminate among the different types of values
    // that can be stored inside `bdld::Datum`.
    enum InternalDataType {
        e_INTERNAL_UNINITIALIZED     =  0,  // zero-filled Datums are invalid

        e_INTERNAL_INF               =  1,  // +/- infinity value

        e_INTERNAL_NIL               =  2,  // null value

        e_INTERNAL_BOOLEAN           =  3,  // boolean value

        e_INTERNAL_SHORTSTRING       =  4,  // short string value

        e_INTERNAL_STRING            =  5,  // string value

        e_INTERNAL_DATE              =  6,  // date value

        e_INTERNAL_TIME              =  7,  // time value

        e_INTERNAL_DATETIME          =  8,  // date+time value

        e_INTERNAL_DATETIME_INTERVAL =  9,  // date+time interval value

        e_INTERNAL_INTEGER           = 10,  // integer value

        e_INTERNAL_INTEGER64         = 11,  // 64-bit integer value

        e_INTERNAL_USERDEFINED       = 12,  // pointer to a user-defined object

        e_INTERNAL_ARRAY             = 13,  // array of datums

        e_INTERNAL_STRING_REFERENCE  = 14,  // not owned string

        e_INTERNAL_ARRAY_REFERENCE   = 15,  // not owned array

        e_INTERNAL_DOUBLE            = 16,  // double value

        e_INTERNAL_MAP               = 17,  // map of datums keyed by string
                                            // values that are not owned

        e_INTERNAL_OWNED_MAP         = 18,  // map of datums keyed by string
                                            // values that are owned

        e_INTERNAL_ERROR             = 19,  // error code, internal storage

        e_INTERNAL_ERROR_ALLOC       = 20,  // error code, allocated storage

        e_INTERNAL_BINARY            = 21,  // binary data, internal storage

        e_INTERNAL_BINARY_ALLOC      = 22,  // binary data, allocated storage

        e_INTERNAL_DECIMAL64         = 23,  // Decimal64

        e_INTERNAL_INT_MAP           = 24   // map of datums keyed by 32-bit
                                            // integer values
    };

    /// Define `k_NUM_INTERNAL_TYPES` to be the number of consecutively
    /// valued enumerators in the range
    /// `[ e_INTERNAL_UNINITIALIZED .. e_INTERNAL_DECIMAL64 ]`.
    enum {
        k_NUM_INTERNAL_TYPES         = 25  // number of internal types
    };

    // CLASS DATA

    // 64-bit variation
    static const int k_TYPE_OFFSET             = 14;  // offset of type in the
                                                      // internal storage
                                                      // buffer

    static const int k_SHORTSTRING_SIZE        = 13;  // maximum size of short
                                                      // strings that stored in
                                                      // the internal storage
                                                      // buffer

    static const int k_SMALLBINARY_SIZE_OFFSET = 13;  // offset of the size of
                                                      // small-size binaries
                                                      // stored in the internal
                                                      // storage buffer

    static const int k_SMALLBINARY_SIZE        = 13;  // maximum size of
                                                      // small-size binaries
                                                      // stored in the internal
                                                      // storage buffer

    // DATA

    // 64-bit variation

    /// Typed access to the bits of the `Datum` internal representation
    struct TypedAccess {
        union {                                       // Offset: 0
            bsls::Types::Int64  d_int64;
            void               *d_ptr;
            double              d_double;
        };
        int                     d_int32;              // Offset: 8
        short                   d_filler;             // Offset: 12
        short                   d_type;               // Offset: 14
    };

    /// Ensures proper alignment (16 byte) and provides 2 types of access to
    /// the 64-bit `Datum` internal representation.  The `d_data` array
    /// allows us raw access to the bytes; while `d_as` provides typed
    /// access to the individual "data compartments".
    union {
        bsls::AlignedBuffer<16> d_data;
        TypedAccess             d_as;
    };

    // PRIVATE CLASS METHODS

    // 64-bit variation

    /// Create a `Datum` object of the specified `type` with the specified
    /// `data` value.
    static Datum createDatum(InternalDataType type, void *data);

    /// Create a `Datum` object of the specified `type` with the specified
    /// `data` value.
    static Datum createDatum(InternalDataType type, int data);

    // PRIVATE ACCESSORS

    // 64-bit variation

    /// Return a pointer to the internal storage buffer
    void* theInlineStorage();

    /// Return a non-modifiable pointer to the internal storage buffer.
    const void* theInlineStorage() const;

#endif // end - 64 bit

  private:
    // FRIENDS
    friend bool operator==(const Datum& lhs, const Datum& rhs);
    friend bool operator!=(const Datum& lhs, const Datum& rhs);
    friend bsl::ostream& operator<<(bsl::ostream& stream, const Datum& rhs);

    // PRIVATE ACCESSORS

    /// Using the specified `allocator` deallocate the specified `nBytes`
    /// allocated for the value of this object.  The behavior is undefined
    /// unless `0 == nBytes` when `0 == this->allocatedPtr<void>()`.  The
    /// behavior is also undefined unless the non-null value of
    /// `this->allocatedPtr<void>()` has been obtained by
    /// `AllocUtil::allocateBytes` from `allocator`.
    void safeDeallocateBytes(const AllocatorType& allocator,
                             bsl::size_t          nBytes) const;

    /// Using the specified `allocator` deallocate the specified `nBytes`
    /// allocated for the value of this object with the specified
    /// `alignment`.  The behavior is undefined unless `0 == nBytes` when
    /// `0 == this->allocatedPtr<void>()`.  The behavior is also undefined
    /// unless the non-null value of `this->allocatedPtr<void>()` has been
    /// obtained by `AllocUtil::allocateBytes` from `allocator`.
    void safeDeallocateBytes(const AllocatorType& allocator,
                             bsl::size_t          nBytes,
                             bsl::size_t          alignment) const;

    /// Return the pointer to the first byte of the memory allocated by this
    /// `Datum` object.  The behavior is undefined unless the internal type
    /// indicates the object *has* allocated.
    template <class t_TYPE>
    t_TYPE *allocatedPtr() const;

    /// Return the internal type of value stored in this object as one of
    /// the enumeration values defined in `InternalDataType`.
    InternalDataType internalType() const;

    /// Return the array reference represented by this object as
    /// `DatumArrayRef` object.  The behavior is undefined unless the object
    /// represents an array reference whose size is stored in the object
    /// internal storage buffer.  Note that all array references store their
    /// size in the object internal storage buffer on 64-bit platforms.
    DatumArrayRef theArrayReference() const;

    /// Return the array represented by this object as `DatumArrayRef`
    /// object.  The behavior is undefined unless the object represents an
    /// array of `Datum`s.
    DatumArrayRef theInternalArray() const;

    /// Return the string value represented by this object as a
    /// `bslstl::StringRef` object.  The behavior is undefined unless the
    /// object represents an internal (non-reference, non-short) string.
    bslstl::StringRef theInternalString() const;

    /// Return the short string value represented by this object as a
    /// `bslstl::StringRef` object.  The behavior is undefined unless the
    /// object actually represents a short string value.
    bslstl::StringRef theShortString() const;

    /// Return the string reference represented by this object as a
    /// `bslstl::StringRef` object.  The behavior is undefined unless the
    /// object represents a string reference whose size is stored in the
    /// object internal storage buffer.  Note that the size always stored in
    /// the object internal storage buffer on 64-bit platforms.
    bslstl::StringRef theStringReference() const;


    /// Return the number of bytes that have been directly allocated for
    /// this object (not for elements or entries).  Used in deallocation.
    /// The behavior is undefined unless the type of the object matches the
    /// allocated internal variant of the type in the function name.
    bsl::size_t theMapAllocNumBytes() const;
    bsl::size_t theIntMapAllocNumBytes() const;
    bsl::size_t theErrorAllocNumBytes() const;
    bsl::size_t theBinaryAllocNumBytes() const;
    bsl::size_t theInternalStringAllocNumBytes() const;
    bsl::size_t theInternalArrayAllocNumBytes() const;

  public:
    // TYPES

    /// `SizeType` is an alias for an unsigned integral value, representing
    /// the capacity of a datum array, the capacity of a datum map, the
    /// capacity of the *keys-capacity* of a datum-key-owning map or the
    /// length of a string.
    typedef bsls::Types::size_type SizeType;

    // CLASS METHODS

    /// Return, by value, a datum referring to the specified `array`,
    /// having the specified `length`, using the specified `allocator` to
    /// supply memory (if needed).  `array` is not copied, and is not freed
    /// when the returned object is destroyed with `Datum::destroy`.  The
    /// behavior is undefined unless `array` contains at least `length`
    /// elements.  The behavior is also undefined unless `length <
    /// UINT_MAX`.
    static Datum createArrayReference(const Datum          *array,
                                      SizeType              length,
                                      const AllocatorType&  allocator);

    /// Return, by value, a datum having the specified `value`, using the
    /// specified `allocator` to supply memory (if needed).  The array
    /// referenced by `value` is not copied, and is not freed if
    /// `Datum::destroy` is called on the returned object.  The behavior is
    /// undefined unless `value.length() < UINT_MAX`.
    static Datum createArrayReference(const DatumArrayRef& value,
                                      const AllocatorType& allocator);

    /// Return, by value, a datum having the specified `bool` `value`.
    static Datum createBoolean(bool value);

    /// Return, by value, a datum having the specified `Date` `value`.
    static Datum createDate(const bdlt::Date& value);

    /// Return, by value, a datum having the specified `Datetime` `value`,
    /// using the specified `allocator` to supply memory (if needed).
    static Datum createDatetime(const bdlt::Datetime& value,
                                const AllocatorType&  allocator);

    /// Return, by value, a datum holding the specified `DatetimeInterval`
    /// `value`, using the specified `allocator` to supply memory (if
    /// needed).
    static Datum createDatetimeInterval(
                                const bdlt::DatetimeInterval& value,
                                const AllocatorType&          allocator);

    /// Return, by value, a datum having the specified `Decimal64` `value`,
    /// using the specified `allocator` to supply memory (if needed).  Note
    /// that the argument is passed by value because it is assumed to be a
    /// fundamental type.
    static Datum createDecimal64(bdldfp::Decimal64    value,
                                 const AllocatorType& allocator);

    /// Return, by value, a datum having the specified `double` `value`.
    /// When `value` is NaN this method guarantees only that a NaN value is
    /// stored.  The sign and NaN payload bits of a NaN `value` later
    /// retrieved by the `theDouble` method are unspecified (see also
    /// {Special Floating Point Values}.
    static Datum createDouble(double value);

    /// Return, by value, a datum having a `DatumError` value with the
    /// specified `code`.
    static Datum createError(int code);

    /// Return, by value, a datum having a `DatumError` value with the
    /// specified `code` and the specified `message`, using the specified
    /// `allocator` to supply memory (if needed).
    static Datum createError(int                      code,
                             const bslstl::StringRef& message,
                             const AllocatorType&     allocator);

    /// Return, by value, a datum having the specified `int` `value`.
    static Datum createInteger(int value);

    /// Return, by value, a datum having the specified `Integer64` `value`,
    /// using the specified `allocator` to supply memory (if needed).
    static Datum createInteger64(bsls::Types::Int64   value,
                                 const AllocatorType& allocator);

    /// Return, by value, a datum having no value.
    static Datum createNull();

    /// Return, by value, a datum that refers to the specified `string`
    /// having the specified `length`, using the specified `allocator` to
    /// supply memory (if needed).  The behavior is undefined unless
    /// `0 != string || 0 == length`.  The behavior is also undefined
    /// unless `length < UINT_MAX`.  Note that `string` is not copied, and
    /// is not freed if `Datum::destroy` is called on the returned object.
    static Datum createStringRef(const char           *string,
                                 SizeType              length,
                                 const AllocatorType&  allocator);

    /// Return, by value, a datum that refers to the specified `string`,
    /// using the specified `allocator` to supply memory (if needed).  The
    /// behavior is undefined unless `string` points to a UTF-8 encoded
    /// c-string.  The behavior is also undefined unless 'strlen(string) <
    /// UINT_MAX'.  Note that `string` is not copied, and is not freed if
    /// `Datum::destroy` is called on the returned object.
    static Datum createStringRef(const char           *string,
                                 const AllocatorType&  allocator);

    /// Return, by value, a datum having the specified `StringRef` `value`,
    /// using the specified `allocator` to supply memory (if needed).  The
    /// behavior is undefined unless `value.length() < UINT_MAX`.  Note that
    /// `string` is not copied, and is not freed if `Datum::destroy` is
    /// called on the returned object.
    static Datum createStringRef(const bslstl::StringRef& value,
                                 const AllocatorType&     allocator);

    /// Return, by value, a datum having the specified `Time` `value`.
    static Datum createTime(const bdlt::Time& value);

    /// Return, by value, a datum having the `DatumUdt` value with the
    /// specified `data` and the specified `type` values.  The behavior is
    /// undefined unless `0 <= type <= 65535`.  Note that `data` is held,
    /// not owned.  Also note that the content pointed to by `data` object
    /// is not copied.
    static Datum createUdt(void *data, int type);

    /// Return, by value, a datum referring to the copy of the specified
    /// `value` of the specified `size`, using the specified
    /// `allocator` to supply memory (if needed).  The behavior is undefined
    /// unless `size < UINT_MAX`.  Note that the copy of the binary data is
    /// owned and will be freed if `Datum::destroy` is called on the
    /// returned object.
    static Datum copyBinary(const void           *value,
                            SizeType              size,
                            const AllocatorType&  allocator);

    /// Return, by value, a datum that refers to the copy of the specified
    /// `string` having the specified `length`, using the specified
    /// `allocator` to supply memory (if needed).  The behavior is undefined
    /// unless `0 != string || 0 == length`.  The behavior is also undefined
    /// unless `length < UINT_MAX`.  Note that the copied string is owned
    /// and will be freed if `Datum::destroy` is called on the returned
    /// object.
    static Datum copyString(const char           *string,
                            SizeType              length,
                            const AllocatorType&  allocator);

    /// Return, by value, a datum having the copy of the specified
    /// `StringRef` `value`, using the specified `allocator` to supply
    /// memory (if needed).  The behavior is undefined unless
    /// `value.length() < UINT_MAX`.  Note that the copied string is owned,
    /// and will be freed if `Datum::destroy` is called on the returned
    /// object.
    static Datum copyString(const bslstl::StringRef& value,
                            const AllocatorType&     allocator);

    /// Return, by value, a datum that refers to the specified `array`.  The
    /// behavior is undefined unless `array` was created using
    /// `createUninitializedArray` method.  The behavior is also undefined
    /// unless each element in the held datum array has been assigned a
    /// value and the array's length has been set accordingly.  Note that
    /// the adopted array is owned and will be freed if `Datum::destroy` is
    /// called on the returned object.
    static Datum adoptArray(const DatumMutableArrayRef& array);

    /// Return, by value, a datum that refers to the specified `intMap`.
    /// The behavior is undefined unless `map` was created using
    /// `createUninitializedIntMap` method.  The behavior is also undefined
    /// unless each element in the held map has been assigned a value and
    /// the size of the map has been set accordingly.  Note that the adopted
    /// map is owned and will be freed if `Datum::destroy` is called on the
    /// returned object.
    static Datum adoptIntMap(const DatumMutableIntMapRef& intMap);

    /// Return, by value, a datum that refers to the specified `map`.  The
    /// behavior is undefined unless `map` was created using
    /// `createUninitializedMap` method.  The behavior is also undefined
    /// unless each element in the held map has been assigned a value and
    /// the size of the map has been set accordingly.  Note that the adopted
    /// map is owned and will be freed if `Datum::destroy` is called on the
    /// returned object.
    static Datum adoptMap(const DatumMutableMapRef& map);

    /// Return, by value, a datum that refers to the specified `map`.  The
    /// behavior is undefined unless `map` was created using
    /// `createUninitializedMapOwningKeys` method.  The behavior is also
    /// undefined unless each element in the held map has been assigned a
    /// value and the size of the map has been set accordingly.  The
    /// behavior is also undefined unless keys have been copied into the
    /// map.  Note that the adopted map is owned and will be freed if
    /// `Datum::destroy` is called on the returned object.
    static Datum adoptMap(const DatumMutableMapOwningKeysRef& map);

    /// Load the specified `result` with a reference to a newly created
    /// datum array having the specified `capacity`, using the specified
    /// `allocator` to supply memory.  The behavior is undefined if
    /// `capacity` `Datum` objects would exceed the addressable memory for
    /// the platform.  Note that the caller is responsible for filling in
    /// elements into the datum array and setting its length accordingly.
    /// The number of elements in the datum array cannot exceed `capacity`.
    /// Also note that any elements in the datum array that need dynamic
    /// memory must be allocated with `allocator`.
    static void createUninitializedArray(DatumMutableArrayRef *result,
                                         SizeType              capacity,
                                         const AllocatorType&  allocator);

    /// Load the specified `result` with a reference to a newly created
    /// datum int-map having the specified `capacity`, using the specified
    /// `allocator` to supply memory.  The behavior is undefined if
    /// `capacity` `DatumIntMapEntry` objects would exceed the addressable
    /// memory for the platform.  Note that the caller is responsible for
    /// filling in elements into the datum int-map and setting its size
    /// accordingly.  The number of elements in the datum int-map cannot
    /// exceed `capacity`.  Also note that any elements in the datum int-map
    /// that need dynamic memory, should also be allocated with `allocator`.
    static void createUninitializedIntMap(
                                        DatumMutableIntMapRef *result,
                                        SizeType               capacity,
                                        const AllocatorType&   allocator);

    /// Load the specified `result` with a reference to a newly created
    /// datum map having the specified `capacity`, using the specified
    /// `allocator` to supply memory.  The behavior is undefined if
    /// `capacity` `DatumMapEntry` objects would exceed the addressable
    /// memory for the platform.  Note that the caller is responsible for
    /// filling in elements into the datum map and setting its size
    /// accordingly.  The number of elements in the datum map cannot exceed
    /// `capacity`.  Also note that any elements in the datum map that need
    /// dynamic memory, should also be allocated with `allocator`.
    static void createUninitializedMap(DatumMutableMapRef   *result,
                                       SizeType              capacity,
                                       const AllocatorType&  allocator);

    /// Load the specified `result` with a reference to a newly created
    /// datum-key-owning map having the specified `capacity` and
    /// `keysCapacity`, using the specified `allocator` to supply memory.
    /// The behavior is undefined if `capacity` `DatumMapEntry` object plus
    /// `keysCapacity` would exceed the addressable memory for the platform.
    /// Note that the caller is responsible for filling in elements into the
    /// datum-key-owning map, copying the keys into it, and setting its size
    /// accordingly.  The number of elements in the datum-key-owning map
    /// cannot exceed `capacity` and total size of all the keys cannot
    /// exceed `keysCapacity`.  Also note that any elements in the
    /// datum-key-owning map that need dynamic memory, should also be
    /// allocated with `allocator`.
    static void createUninitializedMap(
                                 DatumMutableMapOwningKeysRef *result,
                                 SizeType                      capacity,
                                 SizeType                      keysCapacity,
                                 const AllocatorType&          allocator);

    /// Load the specified `result` with a reference to a newly created
    /// character buffer of the specified `length`, using the specified
    /// `allocator` to supply memory, and return the address of this buffer
    /// The behavior is undefined unless `length < UINT_MAX`.  Note that the
    /// caller is responsible for initializing the returned buffer with a
    /// UTF-8 encoded string.
    static char *createUninitializedString(Datum                *result,
                                           SizeType              length,
                                           const AllocatorType&  allocator);

    /// Return the non-modifiable string representation corresponding to the
    /// specified `type`, if it exists, and a unique (error) string
    /// otherwise.  The string representation of `type` matches its
    /// corresponding enumerator name with the `e_` prefix elided.
    ///
    /// For example:
    /// ```
    /// bsl::cout << bdld::Datum::dataTypeToAscii(bdld::Datum::e_NIL);
    /// ```
    /// will print the following on standard output:
    /// ```
    /// NIL
    /// ```
    /// Note that specifying a `type` that does not match any of the
    /// enumerators will result in a string representation that is distinct
    /// from any of those corresponding to the enumerators, but is otherwise
    /// unspecified.
    static const char *dataTypeToAscii(DataType type);

    /// Deallocate any memory that was previously allocated within the
    /// specified `value` using the specified `allocator`.  If the `value`
    /// contains an adopted array of datums, `destroy` is called on each
    /// array element.  If the `value` contains an adopted map of datums,
    /// `destroy` is called on each map element.  The behavior is undefined
    /// unless all dynamically allocated memory owned by `value` was
    /// allocated using `allocator`, and has not previously been released by
    /// a call to `destroy`, either on this object, or on another object
    /// referring to same contents as this object (i.e., only one copy of a
    /// `Datum` object can be destroyed).  The behavior is also undefined if
    /// `value` has an uninitialized or partially initialized array or map
    /// (created using `createUninitializedArray`, `createUninitializedMap`
    /// or `createUninitializeMapOwningKeys`).  Note that after this
    /// operation completes, `value` is left in an uninitialized state, and
    /// must be assigned a new value before being accessed again.
    static void destroy(const Datum& value, const AllocatorType& allocator);

    /// Deallocate the memory used by the specified `array` (but *not*
    /// memory allocated for its contained elements) using the specified
    /// `allocator`.  This method does not destroy individual array elements
    /// and the memory allocated for those elements must be explicitly
    /// deallocated before calling this method.  The behavior is undefined
    /// unless `array` was created with `createUninitializedArray` using
    /// `allocator`.
    static void disposeUninitializedArray(
                                        const DatumMutableArrayRef& array,
                                        const AllocatorType&        allocator);

    /// Deallocate the memory used by the specified `intMap` (but *not*
    /// memory allocated for its contained elements) using the specified
    /// `allocator`.  This method does not destroy individual map elements
    /// and the memory allocated for those elements must be explicitly
    /// deallocated before calling this method.  The behavior is undefined
    /// unless `map` was created with `createUninitializedIntMap` using
    /// `allocator`.
    static void disposeUninitializedIntMap(
                                       const DatumMutableIntMapRef& intMap,
                                       const AllocatorType&         allocator);

    /// Deallocate the memory used by the specified `map` (but *not* memory
    /// allocated for its contained elements) using the specified
    /// `allocator`.  This method does not destroy individual map elements
    /// and the memory allocated for those elements must be explicitly
    /// deallocated before calling this method.  The behavior is undefined
    /// unless `map` was created with `createUninitializedMap` using
    /// `allocator`.
    static void disposeUninitializedMap(const DatumMutableMapRef& map,
                                        const AllocatorType&      allocator);
    static void disposeUninitializedMap(
                                const DatumMutableMapOwningKeysRef& map,
                                const AllocatorType&                allocator);

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Datum, bsl::is_trivially_copyable);
    BSLMF_NESTED_TRAIT_DECLARATION(Datum,
                                   bsl::is_trivially_default_constructible);
    BSLMF_NESTED_TRAIT_DECLARATION(Datum, bslmf::IsBitwiseMoveable);
    BSLMF_NESTED_TRAIT_DECLARATION(Datum, bdlb::HasPrintMethod);

    // CREATORS

    /// Create a datum having an uninitialized value.  The behavior for
    /// every accessor method is undefined until this object is assigned a
    /// value.
    //! Datum() = default;

    /// Create a datum having the value of the specified `original`.
    //! Datum(const Datum& original) = default;

    /// Destroy this object. Note that this method does not deallocate any
    /// dynamically allocated memory used by this object (see `destroy`).
    //! ~Datum() = default;

    // MANIPULATORS

    // Assign to this object the value of the specified `rhs` object. Note
    // that this method's definition is compiler generated.
    //! Datum& operator=(const Datum& rhs) = default;

    // ACCESSORS

    /// Apply the specified `visitor` to the current value represented by
    /// this object by passing held value to the `visitor` object's
    /// `operator()` overload.
    template <class t_VISITOR>
    void apply(t_VISITOR& visitor) const;

    /// Return a datum holding a "deep-copy" of this object, using the
    /// specified `allocator` to supply memory.  This method creates an
    /// independent deep-copy of the data of this object, including any
    /// referenced data, with the exception of {User Defined Types}.  For
    /// further information see {Deep Copying}.
    Datum clone(const AllocatorType& allocator) const;

                               // Type-Identifiers

    /// Return `true` if this object represents an array of `Datum`s and
    /// `false` otherwise.
    bool isArray() const;

    /// Return `true` if this object represents a binary value and `false`
    /// otherwise.
    bool isBinary() const;

    /// Return `true` if this object represents a boolean value and `false`
    /// otherwise.
    bool isBoolean() const;

    /// Return `true` if this object represents a `bdlt::Date` value and
    /// `false` otherwise.
    bool isDate() const;

    /// Return `true` if this object represents a `bdlt::Datetime` value and
    /// `false` otherwise.
    bool isDatetime() const;

    /// Return `true` if this object represents a `bdlt::DatetimeInterval`
    /// value and `false` otherwise.
    bool isDatetimeInterval() const;

    /// Return `true` if this object represents a `bdlfpd::Decimal64` value
    /// and `false` otherwise.
    bool isDecimal64() const;

    /// Return `true` if this object represents a `double` value and `false`
    /// otherwise.
    bool isDouble() const;

    /// Return `true` if this object represents a `DatumError` value and
    /// `false` otherwise.
    bool isError() const;

    /// Return `true` if this object represents a reference to an externally
    /// managed array, string or user-defined object and `false` otherwise.
    /// If this method returns `false`, calling `destroy` on this object
    /// will release the memory used by the array, string, or used-defined
    /// object as well as any meta-data directly used by this datum (e.g.,
    /// length information); otherwise (if this method returns `true`)
    /// calling `destroy` on this object will release any allocated
    /// meta-data, but will not impact the externally managed array, string,
    /// or user-defined object.
    bool isExternalReference() const;

    /// Return `true` if this object represents an integer value and `false`
    /// otherwise.
    bool isInteger() const;

    /// Return `true` if this object represents a `Int64` value and `false`
    /// otherwise.
    bool isInteger64() const;

    /// Return `true` if this object represents a map of datums that are
    /// keyed by 32-bit int values and `false` otherwise.
    bool isIntMap() const;

    /// Return `true` if this object represents a map of datums that are
    /// keyed by string values and `false` otherwise.
    bool isMap() const;

    /// Return `true` if this object represents no value and `false`
    /// otherwise.
    bool isNull() const;

    /// Return `true` if this object represents a string value and `false`
    /// otherwise.
    bool isString() const;

    /// Return `true` if this object represents a `bdlt::Time` value and
    /// `false` otherwise.
    bool isTime() const;

    /// Return `true` if this object represents a `DatumUdt` value and
    /// `false` otherwise.
    bool isUdt() const;

                               // Type-Accessors

    /// Return the array value represented by this object as a
    /// `DatumArrayRef` object.  The behavior is undefined unless this
    /// object actually represents an array of datums.
    DatumArrayRef theArray() const;

    /// Return the binary reference represented by this object as a
    /// `DatumBinaryRef` object.  The behavior is undefined unless this
    /// object actually represents a binary reference.
    DatumBinaryRef theBinary() const;

    /// Return the boolean value represented by this object.  The behavior
    /// is undefined unless this object actually represents a `bool` value.
    bool theBoolean() const;

    /// Return the date value represented by this object as a `bdlt::Date`
    /// object.  The behavior is undefined unless this object actually
    /// represents a date value.
    bdlt::Date theDate() const;

    /// Return the date+time value represented by this object as a
    /// `bdlt::Datetime` object.  The behavior is undefined unless this
    /// object actually represents date+time value.
    bdlt::Datetime theDatetime() const;

    /// Return the date+time interval value represented by this object as a
    /// `bdlt::DatetimeInterval`.  The behavior is undefined unless this
    /// object actually represents a date+time interval value.
    bdlt::DatetimeInterval theDatetimeInterval() const;

    /// Return the decimal floating point value represented by this object
    /// as a `bdlfpd::Decimal64` value.  The behavior is undefined unless
    /// this object actually represents a decimal floating point value.
    bdldfp::Decimal64 theDecimal64() const;

    /// Return the double value represented by this object.  The behavior is
    /// undefined unless this object actually represents a double value.
    /// If the returned value is NaN this method guarantees only that a NaN
    /// value will be returned.  The sign and NaN payload bits of NaN values
    /// returned are unspecified (see also {Special Floating Point Values}.
    double theDouble() const;

    /// Return the error value represented by this object as a `DatumError`
    /// value.  The behavior is undefined unless this object actually
    /// represents an error value.
    DatumError theError() const;

    /// Return the integer value represented by this object.  The behavior
    /// is undefined unless this object actually represents an integer
    /// value.
    int theInteger() const;

    /// Return the 64-bit integer value represented by this object as a
    /// `Int64` value.  The behavior is undefined unless this object
    /// actually represents a 64-bit integer value.
    bsls::Types::Int64 theInteger64() const;

    /// Return the int-map value represented by this object as a
    /// `DatumIntMapRef` object.  The behavior is undefined unless this
    /// object actually represents an int-map of datums.
    DatumIntMapRef theIntMap() const;

    /// Return the map value represented by this object as a `DatumMapRef`
    /// object.  The behavior is undefined unless this object actually
    /// represents a map of datums.
    DatumMapRef theMap() const;

    /// Return the string value represented by this object as a
    /// `bslstl::StringRef` object.  The behavior is undefined unless this
    /// object actually represents a string value.
    bslstl::StringRef theString() const;

    /// Return the time value represented by this object as a `bdlt::Time`
    /// object.  The behavior is undefined unless this object actually
    /// represents a time value.
    bdlt::Time theTime() const;

    /// Return the user-defined object represented by this object as a
    /// `DatumUdt` object.  The behavior is undefined unless this object
    /// actually represents a user-defined object.
    DatumUdt theUdt() const;

    /// Return the type of value represented by this object as one of the
    /// enumeration values defined in `DataType`.
    DataType type() const;

    /// Write the value of this object to the specified output `stream` in a
    /// human-readable format, and return a reference to the modifiable
    /// `stream`.  Optionally specify an initial indentation `level`, whose
    /// absolute value is incremented recursively for nested objects.  If
    /// `level` is specified, optionally specify `spacesPerLevel`, whose
    /// absolute value indicates the number of spaces per indentation level
    /// for this and all of its nested objects.  If `level` is negative,
    /// suppress indentation of the first line.  If `spacesPerLevel` is
    /// negative, format the entire output on one line, suppressing all but
    /// the initial indentation (as governed by `level`).  If `stream` is
    /// not valid on entry, this operation has no effect.  Note that this
    /// human-readable format is not fully specified, and can change without
    /// notice.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    // DEPRECATED

    /// [**DEPRECATED**] Use `createUninitializedMap` instead.
    static void createUninitializedMapOwningKeys(
                                    DatumMutableMapOwningKeysRef *result,
                                    SizeType                      capacity,
                                    SizeType                      keysCapacity,
                                    const AllocatorType&          allocator);

    /// [**DEPRECATED**] Use `adoptMap` instead.
    static Datum adoptMapOwningKeys(
                                  const DatumMutableMapOwningKeysRef& mapping);

    /// [**DEPRECATED**] Use `disposeUninitializedMap` instead.
    static void disposeUninitializedMapOwningKeys(
                                const DatumMutableMapOwningKeysRef& mapping,
                                const AllocatorType&                allocator);
#endif  // end - do not omit deprecated symbols
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` represent the same value,
/// and `false` otherwise.  Two datums (not holding strings and user-
/// defined objects) represent the same value if they have the same type of
/// value stored inside them and invoking `==` operator on the stored values
/// returns `true`.  Two datums holding strings are equal if the strings
/// have the same length and and values at each respective character
/// position are also same.  Two datums holding user-defined objects are
/// equal if the user-defined objects have the same pointer and type values.
/// Two `nil` datums are always equal.  Two `Datum` objects holding `NaN`
/// values are never equal.  Two datums that hold arrays of datums have the
/// same value if the underlying arrays have the same length and invoking
/// `==` operator on each corresponding element returns `true`.  Two datums
/// that hold maps of datums have the same value if the underlying maps have
/// the same size and each corresponding pair of elements in the maps have
/// the same keys and invoking `==` operator on the values returns `true`.
bool operator==(const Datum& lhs, const Datum& rhs);

/// Return `true` if the specified `lhs` and `rhs` datums do not represent
/// the same value, and `false` otherwise.  Two datums do not represent the
/// same value if they do not hold values of the same type, or they hold
/// values of the same type but invoking `==` operator on the stored values
/// returns `false`.  Two strings do not have the same value if they have
/// different lengths or values at one of the respective character position
/// are not the same.  Two `DatumUdt` objects are not equal if they have
/// different pointer or type values.  Two `bslmf::Nil` values are always
/// equal.  Two datums with `NaN` values are never equal.  Two datums that
/// hold arrays of datums have different values if the underlying arrays
/// have different lengths or invoking `==` operator on at least one of the
/// corresponding pair of contained elements returns `false`.  Two datums
/// that hold maps of datums have different values if the underlying maps
/// have different sizes or at least one of the corresponding pair of
/// elements in the maps have different keys or invoking `==` operator on
/// the values returns `false`.
bool operator!=(const Datum& lhs, const Datum& rhs);

/// Write the specified `rhs` value to the specified output `stream` in the
/// format shown in the second column in the table below (based on the type
/// of value stored, indicated by the first column):
/// ```
/// null                   - nil
///
/// bool                   - true/false
///
/// DatumError             - error(code)/error(code, `msg`)
///                          where `code` is the integer error code and
///                          `msg` is the error description message
///
/// int                    - plain integer value
///
/// Int64                  - plain Int64 value
///
/// double                 - plain double value
///
/// string                 - plain double-quoted string value
///
/// array                  - [ elem0, ..., elemN]
///                          where elem1..elemN are output for individual
///                          array elements
///
/// int-map                - [key0 = val0, ..., keyN = valN]
///                          where keyX and valX are respectively key and
///                          value of the map entry elements of the map
///
/// map                    - [key0 = val0, ..., keyN = valN]
///                          where keyX and valX are respectively key and
///                          value of the map entry elements of the map
///
/// bdlt::Date             - ddMONyyyy
///
/// bdlt::Time             - hh:mm:ss.sss
///
/// bdlt::Datetime         - ddMONyyyy_hh:mm:ss.sss
///
/// bdlt::DatetimeInterval - sDD_HH:MM:SS.SSS (where s is the sign(+/-))
///
/// DatumUdt               - user-defined(address,type)
///                          where `address` is a hex encoded pointer to
///                          the user-defined object and `type` is its type
/// ```
/// and return a reference to the modifiable `stream`.  The function will
/// have no effect if the specified `stream` is not valid.
bsl::ostream& operator<<(bsl::ostream& stream, const Datum& rhs);

/// Invoke the specified `hashAlgorithm` on the value of the specified
/// `datum` object.  Note that the value of a User Defined Type in Datum is
/// a combination of its type integer and the address (pointer value), not
/// the actual value of the object that the pointer points to.
template <class t_HASH_ALGORITHM>
void hashAppend(t_HASH_ALGORITHM& hashAlgorithm, const Datum& datum);

/// Write the string representation of the specified enumeration `rhs` to
/// the specified `stream` in a single-line format, and return a reference
/// to the modifiable `stream`.  See `dataTypeToAscii` for what constitutes
/// the string representation of a `Datum::DataType` value.
bsl::ostream& operator<<(bsl::ostream& stream, Datum::DataType rhs);

                         // ==========================
                         // class DatumMutableArrayRef
                         // ==========================

/// This `class` provides mutable access to a datum array.  The users of
/// this class can read from and assign to the individual elements as well
/// as change the length of the array.
class DatumMutableArrayRef {

  public:
    // TYPES

    /// `SizeType` is an alias for an unsigned integral value, representing
    /// the capacity of a datum array.
    typedef Datum::SizeType SizeType;

  private:
    // DATA
    Datum    *d_data_p;    // pointer to an array (not owned)
    SizeType *d_length_p;  // pointer to the length of the array
    SizeType  d_capacity;  // array capacity (to dispose of uninitialized
                           // arrays)
  public:
    // CREATORS

    /// Create a `DatumMutableArrayRef` object that refers to no array.
    DatumMutableArrayRef();

    /// Create a `DatumMutableArrayRef` object having the specified `data`,
    /// `length`, and `capacity`.
    DatumMutableArrayRef(Datum *data, SizeType *length, SizeType capacity);

    /// Create a `DatumMutableArrayRef` having the value of the specified
    /// `original` object.  Note that this method's definition is compiler
    /// generated.
    //! DatumMutableArrayRef(const DatumMutableArrayRef& original) = default;

    /// Destroy this object. Note that this method's definition is compiler
    /// generated.
    //! ~DatumMutableArrayRef() = default;

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object. Note
    /// that this method's definition is compiler generated.
    //! DatumMutableArrayRef& operator=(
    //!                             const DatumMutableArrayRef& rhs) = default;

    // ACCESSORS

    /// Return pointer to the memory allocated for the array.
    void *allocatedPtr() const;

    /// Return pointer to the first element of the held array.
    Datum *data() const;

    /// Return pointer to the length of the array.
    SizeType *length() const;

    /// Return the allocated capacity of the array.
    SizeType capacity() const;
};

                        // =========================
                        // struct Datum_IntMapHeader
                        // =========================

/// This component-local class provides a layout of the meta-information
/// stored in front of the Datum int-maps.
struct Datum_IntMapHeader {

    // DATA
    Datum::SizeType d_size;      // size of the map
    Datum::SizeType d_capacity;  // number of allocated map entries
    bool            d_sorted;    // sorted flag
};

                          // ======================
                          // struct Datum_MapHeader
                          // ======================

/// This component-local class provides a layout of the meta-information
/// stored in front of the Datum maps.
struct Datum_MapHeader {

    // DATA
    Datum::SizeType d_size;          // size of the map
    Datum::SizeType d_capacity;      // number of allocated map entries
    Datum::SizeType d_allocatedSize; // full allocated memory size in bytes
    bool            d_sorted;        // sorted flag
    bool            d_ownsKeys;      // owns keys flag
};

                          // ========================
                          // class DatumMutableMapRef
                          // ========================

/// This `class` provides a mutable access to a datum map.  The users of
/// this class can assign to the individual elements and also change the
/// size of the map.
class DatumMutableMapRef {

  public:

    /// `SizeType` is an alias for an unsigned integral value, representing
    /// the capacity of a datum array, the capacity of a datum map, the
    /// capacity of the *keys-capacity* of a datum-key-owning map or the
    /// length of a string.
    typedef Datum::SizeType SizeType;

  private:
    // DATA
    DatumMapEntry *d_data_p;    // pointer to a map of datums (not owned)

    SizeType      *d_size_p;    // pointer to the size of the map

    bool          *d_sorted_p;  // pointer to flag indicating whether the map
                                // is sorted or not

  public:
    // CREATORS

    /// Create a `DatumMutableMapRef` object.
    DatumMutableMapRef();

    /// Create a `DatumMutableMapRef` object having the specified `data`,
    /// `size`, and `sorted`.
    DatumMutableMapRef(DatumMapEntry *data, SizeType *size, bool *sorted);

    /// Create a `DatumMutableMapRef` having the value of the specified
    /// `original` object.  Note that this method's definition is compiler
    /// generated.
    //! DatumMutableMapRef(const DatumMutableMapRef& original) = default;

    /// Destroy this object. Note that this method's definition is compiler
    /// generated.
    //! ~DatumMutableMapRef() = default;

    // MANIPULATORS

    // Assign to this object the value of the specified `rhs` object. Note
    // that this method's definition is compiler generated.
    //! DatumMutableMapRef& operator=(const DatumMutableMapRef& rhs) = default;

    // ACCESSORS

    /// Return pointer to the memory allocated for the map.
    void *allocatedPtr() const;

    /// Return pointer to the first element in the (held) map.
    DatumMapEntry *data() const;

    /// Return pointer to the location where the (held) map's size is
    /// stored.
    SizeType *size() const;

    /// Return pointer to the location where the (held) map's *sorted* flag
    /// is stored.
    bool *sorted() const;
};

                        // ===========================
                        // class DatumMutableIntMapRef
                        // ===========================

/// This `class` provides a mutable access to a datum int-map.  The users of
/// this class can assign to the individual elements and also change the
/// size of the map.
class DatumMutableIntMapRef {

  public:

    /// `SizeType` is an alias for an unsigned integral value, representing
    /// the capacity of a datum array, the capacity of a datum map, the
    /// capacity of the *keys-capacity* of a datum-key-owning map or the
    /// length of a string.
    typedef Datum::SizeType SizeType;

  private:
    // DATA
    DatumIntMapEntry *d_data_p;    // pointer to an int-map of datums (not
                                   // owned)

    SizeType         *d_size_p;    // pointer to the size of the map

    bool             *d_sorted_p;  // pointer to flag indicating whether the
                                   // int-map is sorted or not

  public:
    // CREATORS

    /// Create a `DatumMutableIntMapRef` object.
    DatumMutableIntMapRef();

    /// Create a `DatumMutableIntMapRef` object having the specified `data`,
    /// `size`, and `sorted`.
    DatumMutableIntMapRef(DatumIntMapEntry *data,
                          SizeType         *size,
                          bool             *sorted);

    /// Create a `DatumMutableIntMapRef` having the value of the specified
    /// `original` object.  Note that this method's definition is compiler
    /// generated.
    //! DatumMutableIntMapRef(const DatumMutableIntMapRef& original) = default;

    /// Destroy this object. Note that this method's definition is compiler
    /// generated.
    //! ~DatumMutableIntMapRef() = default;

    // MANIPULATORS

    // Assign to this object the value of the specified `rhs` object. Note
    // that this method's definition is compiler generated.
    //! DatumMutableIntMapRef& operator=(const DatumMutableIntMapRef& rhs)
    //!                                                              = default;

    // ACCESSORS

    /// Return pointer to the memory allocated for the map.
    void *allocatedPtr() const;

    /// Return pointer to the first element in the (held) map.
    DatumIntMapEntry *data() const;

    /// Return pointer to the location where the (held) map's size is
    /// stored.
    SizeType *size() const;

    /// Return pointer to the location where the (held) map's *sorted* flag
    /// is stored.
    bool *sorted() const;
};

                     // ==================================
                     // class DatumMutableMapOwningKeysRef
                     // ==================================

/// This `class` provides mutable access to a datum key-owning map.  The
/// users of this class can assign to the individual elements, copy keys and
/// change the size of the map.
class DatumMutableMapOwningKeysRef {

  public:

    /// `SizeType` is an alias for an unsigned integral value, representing
    /// the capacity of a datum array, the capacity of a datum map, the
    /// capacity of the *keys-capacity* of a datum-key-owning map or the
    /// length of a string.
    typedef Datum::SizeType SizeType;

  private:
    // DATA
    DatumMapEntry *d_data_p;         // pointer to a map of datums (not owned)

    SizeType      *d_size_p;         // pointer to the size of the map

    SizeType       d_allocatedSize;  // number of bytes allocated for the map

    char          *d_keys_p;         // pointer to the key storage

    bool          *d_sorted_p;       // pointer to flag indicating whether the
                                     // map is sorted or not

  public:
    // CREATORS

    /// Create a `DatumMutableMapOwningKeysRef` object.
    DatumMutableMapOwningKeysRef();

    /// Create a `DatumMutableMapOwningKeysRef` object having the specified
    /// `data`, `size`, `allocatedSize`, `keys`, and `sorted`.
    DatumMutableMapOwningKeysRef(DatumMapEntry *data,
                                 SizeType      *size,
                                 SizeType      allocatedSize,
                                 char          *keys,
                                 bool          *sorted);

    /// Create a `DatumMutableMapOwningKeysRef` having the value of the
    /// specified `original` object.
    //! DatumMutableMapOwningKeysRef(
    //!                const DatumMutableMapOwningKeysRef& original) = default;

    /// Destroy this object.
    //! ~DatumMutableMapOwningKeysRef() = default;

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object. Note
    /// that this method's definition is compiler generated.
    //! DatumMutableMapOwningKeysRef& operator=(
    //!                     const DatumMutableMapOwningKeysRef& rhs) = default;

    // ACCESSORS

    /// Return the number of bytes allocated for the map.
    SizeType allocatedSize() const;

    /// Return pointer to the memory allocated for the map.
    void *allocatedPtr() const;

    /// Return pointer to the first element in the held map.
    DatumMapEntry *data() const;

    /// Return pointer to the start of the buffer where keys are stored.
    char *keys() const;

    /// Return pointer to the location where the (held) map's size is
    /// stored.
    SizeType *size() const;

    /// Return pointer to the location where the (held) map's *sorted* flag
    /// is stored.
    bool *sorted() const;
};

                          // ===================
                          // class DatumArrayRef
                          // ===================

/// This `class` provides a read-only view to an array of datums.  It holds
/// the array by a `const` pointer and an integral length value.  It acts as
/// return value for accessors inside the `Datum` class that return an array
/// of datums.  Note that zero-length arrays are valid.
class DatumArrayRef {

  public:
    // PUBLIC TYPES
    typedef const bdld::Datum element_type;
    typedef       bdld::Datum value_type;

    typedef bsl::size_t     size_type;
    typedef bsl::ptrdiff_t  difference_type;

    typedef       element_type       *pointer;
    typedef const element_type *const_pointer;

    typedef       element_type&       reference;
    typedef const element_type& const_reference;

    typedef       pointer       iterator;
    typedef const_pointer const_iterator;

    typedef bsl::reverse_iterator<iterator>             reverse_iterator;
    typedef bsl::reverse_iterator<const_iterator> const_reverse_iterator;

    /// `SizeType` is an alias for an unsigned integral value, representing
    /// the length of a datum array.
    typedef Datum::SizeType SizeType;

  private:
    // DATA
    const Datum *d_data_p;  // pointer to the first array element (not owned)
    SizeType     d_length;  // length of the array of

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(DatumArrayRef, bsl::is_trivially_copyable);
    BSLMF_NESTED_TRAIT_DECLARATION(DatumArrayRef, bdlb::HasPrintMethod);


    // CREATORS

    /// Create a `DatumArrayRef` object representing an empty array.
    DatumArrayRef();

    /// Create a `DatumArrayRef` object having the specified `data` and
    /// `length`.  The behavior is undefined unless `0 != data` or `0 ==
    /// length`.  Note that the pointer to the array is just copied.
    DatumArrayRef(const Datum *data, SizeType length);

    /// Create a `DatumArrayRef` object having the value of the specified
    /// `original` object.
    //! DatumArrayRef(const DatumArrayRef& other) = default;

    // Destroy this object.
    //! ~DatumArrayRef() = default;

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object.
    //! DatumArrayRef& operator=(const DatumArrayRef& rhs) = default;

    // ACCESSORS

    /// Return a reference providing non-modifiable access to the element at
    /// the specified `position` in the array this reference object
    /// represents.  The behavior is undefined unless `position < size()`.
    const_reference operator[](size_type position) const;

    const_iterator begin() const BSLS_KEYWORD_NOEXCEPT;

    /// Return an iterator providing non-modifiable access to the first
    /// element of the array this reference object represents; return a
    /// past-the-end iterator if `size() == 0`.
    const_iterator cbegin() const BSLS_KEYWORD_NOEXCEPT;

    const_iterator end() const BSLS_KEYWORD_NOEXCEPT;

    /// Return an iterator providing non-modifiable access pointing
    /// past-the-end of the array this reference object represents.
    const_iterator cend() const BSLS_KEYWORD_NOEXCEPT;

    const_reverse_iterator rbegin() const BSLS_KEYWORD_NOEXCEPT;

    /// Return a reverse iterator providing non-modifiable access to the
    /// last element of the array this reference object represents, and the
    /// past-the-end reverse iterator if `size() == 0`.
    const_reverse_iterator crbegin() const BSLS_KEYWORD_NOEXCEPT;

    const_reverse_iterator rend() const BSLS_KEYWORD_NOEXCEPT;

    /// Return a reverse iterator providing non-modifiable access pointing
    /// past-the-end of the array this reference object represents.
    const_reverse_iterator crend() const BSLS_KEYWORD_NOEXCEPT;

    /// Return `size() == 0`.
    bool empty() const BSLS_KEYWORD_NOEXCEPT;

    /// Return a const-pointer to the number of elements of the array this
    /// reference object represents.
    size_type size() const BSLS_KEYWORD_NOEXCEPT;

    /// Return a reference providing non-modifiable access to the first
    /// element of the array this reference object represents.  The behavior
    /// is undefined unless `size() > 0`.
    const_reference front() const;

    /// Return a reference providing non-modifiable access to the last
    /// element of the array this reference object represents.  The behavior
    /// is undefined unless `size() > 0`.
    const_reference back() const;

    /// Return the address providing non-modifiable access to the first
    /// element of the array this reference object represents.  Return a
    /// valid pointer which cannot be dereferenced if the `size() == 0`.
    pointer data() const BSLS_KEYWORD_NOEXCEPT;

    /// Return a const pointer to the length of the array.
    size_type length() const;

    /// Write the value of this object to the specified output `stream` in a
    /// human-readable format, and return a reference to the modifiable
    /// `stream`.  Optionally specify an initial indentation `level`, whose
    /// absolute value is incremented recursively for nested objects.  If
    /// `level` is specified, optionally specify `spacesPerLevel`, whose
    /// absolute value indicates the number of spaces per indentation level
    /// for this and all of its nested objects.  If `level` is negative,
    /// suppress indentation of the first line.  If `spacesPerLevel` is
    /// negative, format the entire output on one line, suppressing all but
    /// the initial indentation (as governed by `level`).  If `stream` is
    /// not valid on entry, this operation has no effect.  Note that this
    /// human-readable format is not fully specified, and can change without
    /// notice.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` have the same value, and
/// `false` otherwise.  Two `DatumArrayRef` objects have the same value if
/// they hold arrays of the same length and all the corresponding `Datum`
/// objects in the two arrays also compare equal.
bool operator==(const DatumArrayRef& lhs, const DatumArrayRef& rhs);

/// Return `true` if the specified `lhs` and `rhs` have different values,
/// and `false` otherwise.  Two `DatumArrayRef` objects have different
/// values if they hold arrays of different lengths or invoking operator
/// `==` returns false for at least one of the corresponding elements in the
/// arrays.
bool operator!=(const DatumArrayRef& lhs, const DatumArrayRef& rhs);

/// Write the specified `rhs` value to the specified output `stream` in the
/// format shown below:
/// ```
/// [aa,bb,cc] - aa, bb and cc are the result of invoking operator `<<`
///              on the individual elements in the array
/// ```
/// and return a reference to the modifiable `stream`.  The function will
/// have no effect if the `stream` is not valid.
bsl::ostream& operator<<(bsl::ostream& stream, const DatumArrayRef& rhs);

                          // ======================
                          // class DatumIntMapEntry
                          // ======================

/// This class represents an entry in a datum map keyed by string values.
class DatumIntMapEntry {

    BSLMF_ASSERT(sizeof(int) == 4 && CHAR_BIT == 8);

  private:
    // DATA
    int   d_key;    // key for this entry
    Datum d_value;  // value for this entry

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(DatumIntMapEntry,
                                   bsl::is_trivially_copyable);
    BSLMF_NESTED_TRAIT_DECLARATION(DatumIntMapEntry, bdlb::HasPrintMethod);

    // CREATORS

    /// Create a `DatumIntMapEntry` object.
    DatumIntMapEntry();

    /// Create a `DatumIntMapEntry` object using the specified `key` and
    /// `value`.
    DatumIntMapEntry(int key, const Datum& value);

    /// Destroy this object.
    //! ~DatumIntMapEntry() = default;

    // MANIPULATORS

    /// Set the key for this entry to the specified `key`.
    void setKey(int key);

    /// Set the value for this entry to the specified `value`.
    void setValue(const Datum& value);

    // ACCESSORS

    /// Return the key for this entry.
    int key() const;

    /// Return the value for this entry.
    const Datum& value() const;

    /// Write the value of this object to the specified output `stream` in a
    /// human-readable format, and return a reference to the modifiable
    /// `stream`.  Optionally specify an initial indentation `level`, whose
    /// absolute value is incremented recursively for nested objects.  If
    /// `level` is specified, optionally specify `spacesPerLevel`, whose
    /// absolute value indicates the number of spaces per indentation level
    /// for this and all of its nested objects.  If `level` is negative,
    /// suppress indentation of the first line.  If `spacesPerLevel` is
    /// negative, format the entire output on one line, suppressing all but
    /// the initial indentation (as governed by `level`).  If `stream` is
    /// not valid on entry, this operation has no effect.  Note that this
    /// human-readable format is not fully specified, and can change without
    /// notice.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` have the same value, and
/// `false` otherwise.  Two `DatumIntMapEntry` objects have the same value
/// if their keys and values compare equal.
bool operator==(const DatumIntMapEntry& lhs, const DatumIntMapEntry& rhs);

/// Return `true` if the specified `lhs` and `rhs` have different values,
/// and `false` otherwise.  Two `DatumIntMapEntry` objects have different
/// values if either the keys or values are not equal.
bool operator!=(const DatumIntMapEntry& lhs, const DatumIntMapEntry& rhs);

/// Write the specified `rhs` value to the specified output `stream` in the
/// format shown below:
/// ```
/// (nnn,aa) - nnn is key integer, while aa is the result of invoking
///            operator `<<` on the value
/// ```
/// and return a reference to the modifiable `stream`.  The function will
/// have no effect if the `stream` is not valid.
bsl::ostream& operator<<(bsl::ostream& stream, const DatumIntMapEntry& rhs);

                          // ====================
                          // class DatumIntMapRef
                          // ====================

/// This class provides a read-only view to a map of datums (an array of
/// `DatumIntMapEntry` objects).  It holds the array by a `const` pointer
/// and an integral size value.  It acts as return value for accessors
/// inside the `Datum` class that return a map of `Datum` objects.  Note
/// that zero-size maps are valid.
class DatumIntMapRef {

  public:
    // PUBLIC TYPES
    typedef const bdld::DatumIntMapEntry element_type;
    typedef       bdld::DatumIntMapEntry value_type;

    typedef bsl::size_t     size_type;
    typedef bsl::ptrdiff_t  difference_type;

    typedef       element_type       *pointer;
    typedef const element_type *const_pointer;

    typedef       element_type&       reference;
    typedef const element_type& const_reference;

    typedef       pointer       iterator;
    typedef const_pointer const_iterator;

    typedef bsl::reverse_iterator<iterator>             reverse_iterator;
    typedef bsl::reverse_iterator<const_iterator> const_reverse_iterator;

    /// `SizeType` is an alias for an unsigned integral value, representing
    /// the capacity of a datum array, the capacity of a datum map, the
    /// capacity of the *keys-capacity* of a datum-key-owning map or the
    /// length of a string.
    typedef Datum::SizeType SizeType;

  private:
    // DATA
    const DatumIntMapEntry *d_data_p;  // entry-array pointer (not owned)
    SizeType                d_size;    // length of the map
    bool                    d_sorted;  // flag indicating whether the array is
                                       // sorted or not
  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(DatumIntMapRef, bsl::is_trivially_copyable);
    BSLMF_NESTED_TRAIT_DECLARATION(DatumIntMapRef, bdlb::HasPrintMethod);

    // CREATORS

    /// Create a `DatumIntMapRef` object having the specified `data` of the
    /// specified `size` and the specified `sorted` flag.  The behavior is
    /// undefined unless `0 != data` or `0 == size`.  Note that the pointer
    /// to the array is just copied.
    DatumIntMapRef(const DatumIntMapEntry *data,
                  SizeType                 size,
                  bool                     sorted);

    /// Destroy this object.
    //! ~DatumIntMapRef() = default;

    // ACCESSORS

    /// Return a reference providing non-modifiable access to the element at
    /// the specified `position` in the array of map entries this reference
    /// object represents.  The behavior is undefined unless
    /// `position < size()`.
    const_reference operator[](size_type position) const;

    const_iterator begin() const BSLS_KEYWORD_NOEXCEPT;

    /// Return an iterator providing non-modifiable access to the first
    /// element of the array of map entries this reference object
    /// represents; return a past-the-end iterator if `size() == 0`.
    const_iterator cbegin() const BSLS_KEYWORD_NOEXCEPT;

    const_iterator end() const BSLS_KEYWORD_NOEXCEPT;

    /// Return an iterator providing non-modifiable access pointing
    /// past-the-end of the array this reference object represents.
    const_iterator cend() const BSLS_KEYWORD_NOEXCEPT;

    const_reverse_iterator rbegin() const BSLS_KEYWORD_NOEXCEPT;

    /// Return a reverse iterator providing non-modifiable access to the
    /// last element of the array this reference object represents, and the
    /// past-the-end reverse iterator if `size() == 0`.
    const_reverse_iterator crbegin() const BSLS_KEYWORD_NOEXCEPT;

    const_reverse_iterator rend() const BSLS_KEYWORD_NOEXCEPT;

    /// Return a reverse iterator providing non-modifiable access pointing
    /// past-the-end of the array this reference object represents.
    const_reverse_iterator crend() const BSLS_KEYWORD_NOEXCEPT;

    /// Return `size() == 0`.
    bool empty() const BSLS_KEYWORD_NOEXCEPT;

    /// Return a const-pointer to the number of elements of the array this
    /// reference object represents.
    size_type size() const BSLS_KEYWORD_NOEXCEPT;

    /// Return a reference providing non-modifiable access to the first
    /// element of the array this reference object represents.  The behavior
    /// is undefined unless `size() > 0`.
    const_reference front() const;

    /// Return a reference providing non-modifiable access to the last
    /// element of the array this reference object represents.  The behavior
    /// is undefined unless `size() > 0`.
    const_reference back() const;

    /// Return the address providing non-modifiable access to the first
    /// element of the array this reference object represents.  Return a
    /// valid pointer which cannot be dereferenced if the `size() == 0`.
    pointer data() const BSLS_KEYWORD_NOEXCEPT;

    /// Return `true` if underlying map is sorted and `false` otherwise.
    bool isSorted() const;

    /// Return a const pointer to the datum having the specified `key`, if
    /// it exists and 0 otherwise.  Note that the `find` has order of `O(n)`
    /// if the data is not sorted based on the keys; if the data is sorted,
    /// it has order of `O(log(n))`.  Also note that if multiple entries
    /// with matching keys are present, which matching record is found is
    /// unspecified.
    const Datum *find(int key) const;

    /// Write the value of this object to the specified output `stream` in a
    /// human-readable format, and return a reference to the modifiable
    /// `stream`.  Optionally specify an initial indentation `level`, whose
    /// absolute value is incremented recursively for nested objects.  If
    /// `level` is specified, optionally specify `spacesPerLevel`, whose
    /// absolute value indicates the number of spaces per indentation level
    /// for this and all of its nested objects.  If `level` is negative,
    /// suppress indentation of the first line.  If `spacesPerLevel` is
    /// negative, format the entire output on one line, suppressing all but
    /// the initial indentation (as governed by `level`).  If `stream` is
    /// not valid on entry, this operation has no effect.  Note that this
    /// human-readable format is not fully specified, and can change without
    /// notice.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` have the same value, and
/// `false` otherwise.  Two `DatumIntMapRef` objects have the same value if
/// they hold maps of the same size and all the corresponding
/// `DatumIntMapEntry` elements in the two maps also compare equal.
bool operator==(const DatumIntMapRef& lhs, const DatumIntMapRef& rhs);

/// Return `true` if the specified `lhs` and `rhs` have different values,
/// and `false` otherwise.  Two `DatumIntMapRef` objects have different
/// values if they hold maps of different sizes or operator `==` returns
/// `false` for at least one of the corresponding elements in the maps.
bool operator!=(const DatumIntMapRef& lhs, const DatumIntMapRef& rhs);

/// Write the specified `rhs` value to the specified output `stream` in the
/// format shown below:
/// ```
/// [ nnn = aa, mmm = bb] - nnn and mmm are key ints, while aa and bb
///                         are the result of invoking operator `<<` on the
///                         individual value elements in the map
/// ```
/// and return a reference to the modifiable `stream`.  The function will
/// have no effect if the `stream` is not valid.
bsl::ostream& operator<<(bsl::ostream& stream, const DatumIntMapRef& rhs);

                            // ===================
                            // class DatumMapEntry
                            // ===================

/// This class represents an entry in a datum map keyed by string values.
class DatumMapEntry {

  private:
    // DATA
    bslstl::StringRef d_key_p;  // key for this entry (not owned)
    Datum             d_value;  // value for this entry

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(DatumMapEntry, bsl::is_trivially_copyable);
    BSLMF_NESTED_TRAIT_DECLARATION(DatumMapEntry, bdlb::HasPrintMethod);

    // CREATORS

    /// Create a `DatumMapEntry` object.
    DatumMapEntry();

    /// Create a `DatumMapEntry` object using the specified `key` and
    /// `value`.
    DatumMapEntry(const bslstl::StringRef& key, const Datum& value);

    /// Destroy this object.
    //! ~DatumMapEntry() = default;

    // MANIPULATORS

    /// Set the key for this entry to the specified `key`.
    void setKey(const bslstl::StringRef& key);

    /// Set the value for this entry to the specified `value`.
    void setValue(const Datum& value);

    // ACCESSORS

    /// Return the key for this entry.
    const bslstl::StringRef& key() const;

    /// Return the value for this entry.
    const Datum& value() const;

    /// Write the value of this object to the specified output `stream` in a
    /// human-readable format, and return a reference to the modifiable
    /// `stream`.  Optionally specify an initial indentation `level`, whose
    /// absolute value is incremented recursively for nested objects.  If
    /// `level` is specified, optionally specify `spacesPerLevel`, whose
    /// absolute value indicates the number of spaces per indentation level
    /// for this and all of its nested objects.  If `level` is negative,
    /// suppress indentation of the first line.  If `spacesPerLevel` is
    /// negative, format the entire output on one line, suppressing all but
    /// the initial indentation (as governed by `level`).  If `stream` is
    /// not valid on entry, this operation has no effect.  Note that this
    /// human-readable format is not fully specified, and can change without
    /// notice.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` have the same value, and
/// `false` otherwise.  Two `DatumMapEntry` objects have the same value if
/// their keys and values compare equal.
bool operator==(const DatumMapEntry& lhs, const DatumMapEntry& rhs);

/// Return `true` if the specified `lhs` and `rhs` have different values,
/// and `false` otherwise.  Two `DatumMapEntry` objects have different
/// values if either the keys or values are not equal.
bool operator!=(const DatumMapEntry& lhs, const DatumMapEntry& rhs);

/// Write the specified `rhs` value to the specified output `stream` in the
/// format shown below:
/// ```
/// (abc,aa) - abc is key string, while aa is the result of invoking
///            operator `<<` on the value
/// ```
/// and return a reference to the modifiable `stream`.  The function will
/// have no effect if the `stream` is not valid.
bsl::ostream& operator<<(bsl::ostream& stream, const DatumMapEntry& rhs);

                            // =================
                            // class DatumMapRef
                            // =================

/// This class provides a read-only view to a map of datums (an array of
/// `DatumMapEntry` objects).  It holds the array by a `const` pointer and
/// an integral size value.  It acts as return value for accessors inside
/// the `Datum` class that return a map of `Datum` objects.  Note that
/// zero-size maps are valid.
class DatumMapRef {

  public:
    // PUBLIC TYPES
    typedef const bdld::DatumMapEntry element_type;
    typedef       bdld::DatumMapEntry value_type;

    typedef bsl::size_t     size_type;
    typedef bsl::ptrdiff_t  difference_type;

    typedef       element_type       *pointer;
    typedef const element_type *const_pointer;

    typedef       element_type&       reference;
    typedef const element_type& const_reference;

    typedef       pointer       iterator;
    typedef const_pointer const_iterator;

    typedef bsl::reverse_iterator<iterator>             reverse_iterator;
    typedef bsl::reverse_iterator<const_iterator> const_reverse_iterator;

    /// `SizeType` is an alias for an unsigned integral value, representing
    /// the capacity of a datum array, the capacity of a datum map, the
    /// capacity of the *keys-capacity* of a datum-key-owning map or the
    /// length of a string.
    typedef Datum::SizeType SizeType;

  private:
    // DATA
    const DatumMapEntry *d_data_p;   // pointer to the entry-array (not owned)
    SizeType             d_size;     // length of the map
    bool                 d_sorted;   // flag indicating whether the array is
                                     // sorted or not
    bool                 d_ownsKeys; // flag indicating whether the map owns
                                     // the keys or not
  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(DatumMapRef, bsl::is_trivially_copyable);
    BSLMF_NESTED_TRAIT_DECLARATION(DatumMapRef, bdlb::HasPrintMethod);

    // CREATORS

    /// Create a `DatumMapRef` object having the specified `data` of the
    /// specified `size` and the specified `sorted` and `ownsKeys` flags.
    /// The behavior is undefined unless `0 != data` or `0 == size`.  Note
    /// that the pointer to the array is just copied.
    DatumMapRef(const DatumMapEntry *data,
                SizeType             size,
                bool                 sorted,
                bool                 ownsKeys);

    /// Destroy this object.
    //! ~DatumMapRef() = default;

    // ACCESSORS

    /// Return a reference providing non-modifiable access to the element at
    /// the specified `position` in the array of map entries this reference
    /// object represents.  The behavior is undefined unless
    /// `position < size()`.
    const_reference operator[](size_type position) const;

    const_iterator begin() const BSLS_KEYWORD_NOEXCEPT;

    /// Return an iterator providing non-modifiable access to the first
    /// element of the array of map entries this reference object
    /// represents; return a past-the-end iterator if `size() == 0`.
    const_iterator cbegin() const BSLS_KEYWORD_NOEXCEPT;

    const_iterator end() const BSLS_KEYWORD_NOEXCEPT;

    /// Return an iterator providing non-modifiable access pointing
    /// past-the-end of the array this reference object represents.
    const_iterator cend() const BSLS_KEYWORD_NOEXCEPT;

    const_reverse_iterator rbegin() const BSLS_KEYWORD_NOEXCEPT;

    /// Return a reverse iterator providing non-modifiable access to the
    /// last element of the array this reference object represents, and the
    /// past-the-end reverse iterator if `size() == 0`.
    const_reverse_iterator crbegin() const BSLS_KEYWORD_NOEXCEPT;

    const_reverse_iterator rend() const BSLS_KEYWORD_NOEXCEPT;

    /// Return a reverse iterator providing non-modifiable access pointing
    /// past-the-end of the array this reference object represents.
    const_reverse_iterator crend() const BSLS_KEYWORD_NOEXCEPT;

    /// Return `size() == 0`.
    bool empty() const BSLS_KEYWORD_NOEXCEPT;

    /// Return a const-pointer to the number of elements of the map this
    /// reference object represents.
    size_type size() const BSLS_KEYWORD_NOEXCEPT;

    /// Return a reference providing non-modifiable access to the first
    /// element of the map this reference object represents.  The behavior
    /// is undefined unless `size() > 0`.
    const_reference front() const;

    /// Return a reference providing non-modifiable access to the last
    /// element of the map this reference object represents.  The behavior
    /// is undefined unless `size() > 0`.
    const_reference back() const;

    /// Return the address providing non-modifiable access to the first
    /// element of the map this reference object represents.  Return a
    /// valid pointer which cannot be dereferenced if the `size() == 0`.
    pointer data() const BSLS_KEYWORD_NOEXCEPT;

    /// Return `true` if underlying map is sorted and `false` otherwise.
    bool isSorted() const;

    /// Return `true` if underlying map owns the keys and `false` otherwise.
    /// Note that `false` is always returned for zero-sized `DatumMapRef`.
    bool ownsKeys() const;

    /// Return a const pointer to the datum having the specified `key`, if
    /// it exists and 0 otherwise.  Note that the `find` has order of `O(n)`
    /// if the data is not sorted based on the keys.  If the data is sorted,
    /// it has order of `O(log(n))`.  Also note that if multiple entries
    /// with matching keys are present, which matching record is found is
    /// unspecified.
    const Datum *find(const bslstl::StringRef& key) const;

    /// Write the value of this object to the specified output `stream` in a
    /// human-readable format, and return a reference to the modifiable
    /// `stream`.  Optionally specify an initial indentation `level`, whose
    /// absolute value is incremented recursively for nested objects.  If
    /// `level` is specified, optionally specify `spacesPerLevel`, whose
    /// absolute value indicates the number of spaces per indentation level
    /// for this and all of its nested objects.  If `level` is negative,
    /// suppress indentation of the first line.  If `spacesPerLevel` is
    /// negative, format the entire output on one line, suppressing all but
    /// the initial indentation (as governed by `level`).  If `stream` is
    /// not valid on entry, this operation has no effect.  Note that this
    /// human-readable format is not fully specified, and can change without
    /// notice.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
};

// FREE OPERATORS

/// Return `true` if the specified `lhs` and `rhs` have the same value, and
/// `false` otherwise.  Two `DatumMapRef` objects have the same value if
/// they hold maps of the same size and all the corresponding
/// `DatumMapEntry` elements in the two maps also compare equal.
bool operator==(const DatumMapRef& lhs, const DatumMapRef& rhs);

/// Return `true` if the specified `lhs` and `rhs` have different values,
/// and `false` otherwise.  Two `DatumMapRef` objects have different values
/// if they hold maps of different sizes or operator `==` returns false for
/// at least one of the corresponding elements in the maps.
bool operator!=(const DatumMapRef& lhs, const DatumMapRef& rhs);

/// Write the specified `rhs` value to the specified output `stream` in the
/// format shown below:
/// ```
/// [ abc = aa, pqr = bb] - abc and pqr are key strings, while aa and bb
///                         are the result of invoking operator '<<' on the
///                         individual value elements in the map
/// ```
/// and return a reference to the modifiable `stream`.  The function will
/// have no effect if the `stream` is not valid.
bsl::ostream& operator<<(bsl::ostream& stream, const DatumMapRef& rhs);


                            // ====================
                            // struct Datum_Helpers
                            // ====================

/// This struct contains helper functions used to access typed objects
/// within a buffer.  The functions assume that objects within the buffers
/// have proper alignment and use casts to suppress compiler warnings about
/// possible alignment problems.
struct Datum_Helpers {

    /// Return the typed value found at the specified `offset` within the
    /// specified `source`.
    template <class t_TYPE>
    static t_TYPE load(const void *source, int offset);

    /// Store the specified typed `value` at the specified `offset` within
    /// the specified `destination` and return `value`.
    template <class t_TYPE>
    static t_TYPE store(void *destination, int offset, t_TYPE value);
};

#ifdef BSLS_PLATFORM_CPU_32_BIT

                        // ======================
                        // struct Datum_Helpers32
                        // ======================

/// This struct contains helper functions used in the 32-bit variation.  The
/// functions are for internal use only and may change or disappear without
/// notice.
struct Datum_Helpers32 : Datum_Helpers {

    // CLASS DATA
#ifdef BSLS_PLATFORM_IS_LITTLE_ENDIAN
    static const int b00          = 0; // Bits 0 to 32.
    static const int b32          = 4; // Bits 32 to 48.
    static const int b48          = 6; // Bits 48 to 64.
#else  // end - little endian / begin - big endian
    static const int b00          = 4;
    static const int b32          = 2;
    static const int b48          = 0;
#endif  // end - big endian

    // CLASS METHODS

    /// Load an Int64 from the specified `high16` and `low32` values created
    /// by storeSmallInt48.  This method is public for testing purpose only.
    /// It may change or be removed without notice.
    static bsls::Types::Int64 loadInt48(short high16, int low32);

    /// Store an Int64  in short at `phigh16` and int at `plow32` if its
    /// highest order 16 bits are zero.  Return true if it fits.  This
    /// method is public for testing purpose only. It may change or be
    /// removed without notice.
    static bool storeInt48(bsls::Types::Int64  value,
                           short              *phigh16,
                           int                *plow32);

    /// Load an Int64 from the specified `high16` and `low32` values created
    /// by storeSmallInt64.  This method is public for testing purpose only.
    /// It may change or be removed without notice.
    static bsls::Types::Int64 loadSmallInt64(short high16, int low32);

    /// Store an Int64 in short at `phigh16` and int at `plow32`.  Return
    /// true if it fits.  This method is public for testing purpose only.
    /// It may change or be removed without notice.
    static bool storeSmallInt64(bsls::Types::Int64  value,
                                short              *phigh16,
                                int                *plow32);
};

#endif  // end - 32 bit

// ============================================================================
//                               INLINE DEFINITIONS
// ============================================================================

                            // --------------------
                            // struct Datum_Helpers
                            // --------------------

// CLASS METHODS
template <class t_TYPE>
inline
t_TYPE Datum_Helpers::load(const void *source, int offset)
{
    // The intermediate cast to `void *` avoids warnings about the cast to a
    // pointer of stricter alignment.
    return *static_cast<const t_TYPE *>(
            static_cast<const void *>(
            static_cast<const char *>(source) + offset));
}

template <class t_TYPE>
inline
t_TYPE Datum_Helpers::store(void *destination, int offset, t_TYPE value)
{
    // The intermediate cast to `void *` avoids warnings about the cast to a
    // pointer of stricter alignment.
    return *static_cast<t_TYPE *>(
            static_cast<void *>(
            static_cast<char *>(destination) + offset)) = value;
}

#ifdef BSLS_PLATFORM_CPU_32_BIT

                        // ----------------------
                        // struct Datum32_Helpers
                        // ----------------------

// CLASS METHODS
inline
bsls::Types::Int64 Datum_Helpers32::loadSmallInt64(short high16, int low32)
{
    bsls::Types::Int64 value;

    store<short>(&value, b48, store<short>(&value, b32, high16) < 0 ? -1 : 0);
    store<long> (&value, b00, low32);

    return value;
}

inline
bool Datum_Helpers32::storeSmallInt64(bsls::Types::Int64  value,
                                      short              *phigh16,
                                      int                *plow32)
{
    // Check that the sign can be inferred from the compressed 6-byte integer.
    // It is the case if the upper 16 bits are the same as the 17th bit.

    if ((load<short>(&value, b48) ==  0 && load<short>(&value, b32) >= 0) ||
        (load<short>(&value, b48) == -1 && load<short>(&value, b32) <  0)) {
        *phigh16 = load<short>(&value, b32);
        *plow32  = load<long> (&value, b00);
        return true;                                                  // RETURN
    }
    return false;
}

inline
bsls::Types::Int64 Datum_Helpers32::loadInt48(short high16, int low32)
{
    bsls::Types::Int64 value;

    store<short>(&value, b48, 0);
    store<short>(&value, b32, high16);
    store<long>(&value, b00, low32);

    return value;
}

inline
bool Datum_Helpers32::storeInt48(bsls::Types::Int64  value,
                                 short              *phigh16,
                                 int                *plow32)
{
    // Check 'value' is a 6-byte integer.  It is the case if the upper 16 bits
    // are zero.

    if (load<short>(&value, b48) == 0) {
        *phigh16 = load<short>(&value, b32);
        *plow32 = load<long>(&value, b00);
        return true;                                                  // RETURN
    }
    return false;
}
#endif  // end - 32 bit

                                // -----------
                                // class Datum
                                // -----------

// This section contains all class methods and private accessors that are used
// only in implementation for specific platform.

#ifdef BSLS_PLATFORM_CPU_32_BIT
// PRIVATE CLASS METHODS
// 32-bit only
inline
Datum Datum::createExtendedDataObject(ExtendedInternalDataType  type,
                                      void                     *data)
{
    Datum result;
    result.d_exp.d_value = (k_DOUBLE_MASK | e_INTERNAL_EXTENDED)
                            << k_TYPE_MASK_BITS | type;
    result.d_as.d_cvp = data;
    return result;
}

inline
Datum Datum::createExtendedDataObject(ExtendedInternalDataType  type,
                                      int                       data)
{
    Datum result;
    result.d_exp.d_value = (k_DOUBLE_MASK | e_INTERNAL_EXTENDED)
                            << k_TYPE_MASK_BITS | type;
    result.d_as.d_int = data;
    return result;
}

// PRIVATE ACCESSORS
// 32-bit only
inline
Datum::ExtendedInternalDataType Datum::extendedInternalType() const
{
    BSLS_ASSERT_SAFE(e_INTERNAL_EXTENDED == internalType());
    return static_cast<ExtendedInternalDataType>(d_as.d_short);
}

inline
Datum::DataType Datum::typeFromExtendedInternalType() const
{
    BSLS_ASSERT_SAFE(e_INTERNAL_EXTENDED == internalType());

    static const DataType convert[] = {
        Datum::e_MAP            // e_EXTENDED_INTERNAL_MAP                 = 0
      , Datum::e_MAP            // e_EXTENDED_INTERNAL_OWNED_MAP           = 1
      , Datum::e_DOUBLE         // e_EXTENDED_INTERNAL_NAN2                = 2
      , Datum::e_ERROR          // e_EXTENDED_INTERNAL_ERROR               = 3
      , Datum::e_ERROR          // e_EXTENDED_INTERNAL_ERROR_ALLOC         = 4
      , Datum::e_STRING         // e_EXTENDED_INTERNAL_SREF_ALLOC          = 5
      , Datum::e_ARRAY          // e_EXTENDED_INTERNAL_AREF_ALLOC          = 6
      , Datum::e_DATETIME       // e_EXTENDED_INTERNAL_DATETIME_ALLOC      = 7
      , Datum::e_DATETIME_INTERVAL
                      // e_EXTENDED_INTERNAL_DATETIME_INTERVAL_ALLOC       = 8
      , Datum::e_INTEGER64      // e_EXTENDED_INTERNAL_INTEGER64_ALLOC     = 9
      , Datum::e_BINARY         // e_EXTENDED_INTERNAL_BINARY_ALLOC        = 10
      , Datum::e_DECIMAL64      // e_EXTENDED_INTERNAL_DECIMAL64           = 11
      , Datum::e_DECIMAL64      // e_EXTENDED_INTERNAL_DECIMAL64_SPECIAL   = 12
      , Datum::e_DECIMAL64      // e_EXTENDED_INTERNAL_DECIMAL64_ALLOC     = 13
      , Datum::e_NIL            // e_EXTENDED_INTERNAL_NIL                 = 14
      , Datum::e_INT_MAP        // e_EXTENDED_INTERNAL_INT_MAP             = 15
    };

    BSLMF_ASSERT(sizeof(convert)/sizeof(convert[0]) ==
                 k_NUM_EXTENDED_INTERNAL_TYPES);

    const ExtendedInternalDataType type = extendedInternalType();

    BSLS_ASSERT_OPT(static_cast<int>(type) <
                    static_cast<int>(k_NUM_EXTENDED_INTERNAL_TYPES));

    return convert[type];
}

inline
bsls::Types::Int64 Datum::theLargeInteger64() const
{
    BSLS_ASSERT_SAFE(internalType() == e_INTERNAL_EXTENDED);
    BSLS_ASSERT_SAFE(
        extendedInternalType() == e_EXTENDED_INTERNAL_INTEGER64_ALLOC);
    return Datum_Helpers::load<bsls::Types::Int64>(d_as.d_cvp, 0);
}

inline
DatumArrayRef Datum::theLongArrayReference() const
{
    return DatumArrayRef(
        Datum_Helpers::load<Datum *> (d_as.d_cvp, 0),
        Datum_Helpers::load<SizeType>(d_as.d_cvp, sizeof(Datum *)));
}

inline
bslstl::StringRef Datum::theLongestShortString() const
{
    return bslstl::StringRef(d_string6.d_chars, sizeof d_string6.d_chars);
}

inline
bslstl::StringRef Datum::theLongStringReference() const
{
    return bslstl::StringRef(
        Datum_Helpers::load<char *>  (d_as.d_cvp, 0),
        Datum_Helpers::load<SizeType>(d_as.d_cvp, sizeof(char *)));
}

inline
bsls::Types::Int64 Datum::theSmallInteger64() const
{
    BSLS_ASSERT_SAFE(internalType() == e_INTERNAL_INTEGER64);
    return Datum_Helpers32::loadSmallInt64(d_as.d_short, d_as.d_int);
}
#else   // end - 32 bit / begin - 64 bit
// PRIVATE CLASS METHODS

// 64-bit only
inline
Datum Datum::createDatum(InternalDataType type, void *data)
{
    Datum result;
    result.d_as.d_type = type;
    result.d_as.d_ptr  = data;
    return result;
}

inline
Datum Datum::createDatum(InternalDataType type, int data)
{
    Datum result;
    result.d_as.d_type  = type;
    result.d_as.d_int64 = data;
    return result;
}

// PRIVATE ACCESSORS

// 64-bit only
inline
void* Datum::theInlineStorage()
{
    return d_data.buffer();
}

inline
const void* Datum::theInlineStorage() const
{
    return d_data.buffer();
}
#endif // end - 64 bit

// This section contains all methods that are common for all platforms, but may
// have platform-specific implementation.

// PRIVATE ACCESSORS
inline
void Datum::safeDeallocateBytes(const AllocatorType& allocator,
                                bsl::size_t          nBytes) const
{
    void *ptr = allocatedPtr<void>();

    if (ptr) {
        AllocUtil::deallocateBytes(allocator, ptr, nBytes);
    }
}

inline
void Datum::safeDeallocateBytes(const AllocatorType& allocator,
                                bsl::size_t          nBytes,
                                bsl::size_t          alignment) const
{
    void *ptr = allocatedPtr<void>();

    if (ptr) {
        AllocUtil::deallocateBytes(allocator, ptr, nBytes, alignment);
    }
}

template <class t_TYPE>
inline
t_TYPE *Datum::allocatedPtr() const
{
#ifdef BSLS_PLATFORM_CPU_32_BIT
    return static_cast<t_TYPE *>(const_cast<void*>(d_as.d_cvp));
#else   // end - 32 bit / begin - 64 bit
    return static_cast<t_TYPE *>(d_as.d_ptr);
#endif  // end - 64 bit
}
inline
Datum::InternalDataType Datum::internalType() const
{
#ifdef BSLS_PLATFORM_CPU_32_BIT
    if (0x7f == d_data[k_EXPONENT_MSB] &&
        0xf0 == (d_data[k_EXPONENT_LSB] & 0xf0)) {
        return static_cast<InternalDataType>(d_data[k_EXPONENT_LSB] & 0x0f);
    }
    return e_INTERNAL_DOUBLE;
#else   // end - 32 bit / begin - 64 bit
    return static_cast<InternalDataType>(d_as.d_type);
#endif  // end - 64 bit
}

inline
DatumArrayRef Datum::theArrayReference() const
{
#ifdef BSLS_PLATFORM_CPU_32_BIT
    return DatumArrayRef(allocatedPtr<const Datum>(), d_as.d_ushort);
#else   // end - 32 bit / begin - 64 bit
    return DatumArrayRef(allocatedPtr<const Datum>(), d_as.d_int32);
#endif  // end - 64 bit
}

inline
DatumArrayRef Datum::theInternalArray() const
{
    const Datum *data = allocatedPtr<const Datum>();
    if (data) {
        const SizeType size = *reinterpret_cast<const SizeType *>(data);
        return DatumArrayRef(data + 1, size);                         // RETURN
    }
    return DatumArrayRef(0, 0);
}

inline
bslstl::StringRef Datum::theInternalString() const
{
#ifdef BSLS_PLATFORM_CPU_32_BIT
    const char *data = allocatedPtr<const char>();
    return bslstl::StringRef(data + sizeof(SizeType),
                             Datum_Helpers::load<SizeType>(data, 0));
#else   // end - 32 bit / begin - 64 bit
    return bslstl::StringRef(allocatedPtr<const char>(),
                             d_as.d_int32);
#endif  // end - 64 bit
}

BDLD_DATUM_FORCE_INLINE
bslstl::StringRef Datum::theShortString() const
{
#ifdef BSLS_PLATFORM_CPU_32_BIT
    return bslstl::StringRef(d_string5.d_chars, d_string5.d_length);
#else   // end - 32 bit / begin - 64 bit
    const char     *str = reinterpret_cast<const char *>(theInlineStorage());
    const SizeType  len = *str++;
    return bslstl::StringRef(str, static_cast<int>(len));
#endif  // end - 64 bit
}

inline
bslstl::StringRef Datum::theStringReference() const
{
#ifdef BSLS_PLATFORM_CPU_32_BIT
    return bslstl::StringRef(allocatedPtr<const char>(), d_as.d_ushort);
#else   // end - 32 bit / begin - 64 bit
    return bslstl::StringRef(allocatedPtr<const char>(), d_as.d_int32);
#endif // BSLS_PLATFORM_CPU_32_BIT
}

inline
bsl::size_t Datum::theMapAllocNumBytes() const
{
    BSLS_ASSERT_SAFE(isMap());

    // Map header is stored in the place of the first DatumMapEntry
    const Datum_MapHeader *header = allocatedPtr<const Datum_MapHeader>();

    return header ? header->d_allocatedSize : 0;
}

inline
bsl::size_t Datum::theIntMapAllocNumBytes() const
{
    BSLS_ASSERT_SAFE(isIntMap());

    // Map header is stored in the place of the first DatumIntMapEntry
    const Datum_IntMapHeader *hdr = allocatedPtr<const Datum_IntMapHeader>();

    return hdr ? (hdr->d_capacity + 1) * sizeof(DatumIntMapEntry) : 0;
}

inline
bsl::size_t Datum::theErrorAllocNumBytes() const
{
    BSLS_ASSERT(isError());

#ifdef BSLS_PLATFORM_CPU_32_BIT
    // If the extended type is 'e_EXTENDED_INTERNAL_ERROR', we are storing
    // just a code, at the data offset.  Otherwise, we're storing an allocated
    // object.

    BSLS_ASSERT_SAFE(
                    e_EXTENDED_INTERNAL_ERROR_ALLOC == extendedInternalType());
#else   // end - 32 bit / begin - 64 bit
    BSLS_ASSERT_SAFE(e_INTERNAL_ERROR_ALLOC == internalType());
#endif  // end - 64 bit

    const char* data = allocatedPtr<const char>();

    const bsl::size_t msgLen  = Datum_Helpers::load<int>(data, sizeof(int));
    const bsl::size_t align   = sizeof(int);
    const bsl::size_t headLen = 2 * sizeof(int);

    return headLen + ((msgLen + align - 1) & ~(align - 1));
}

inline
bsl::size_t Datum::theBinaryAllocNumBytes() const
{
    BSLS_ASSERT_SAFE(isBinary());

#ifdef BSLS_PLATFORM_CPU_32_BIT
    BSLS_ASSERT_SAFE(
                   e_EXTENDED_INTERNAL_BINARY_ALLOC == extendedInternalType());
    return *allocatedPtr<const SizeType>() + sizeof(double);
#else   // end - 32 bit / begin - 64 bit
    BSLS_ASSERT_SAFE(e_INTERNAL_BINARY_ALLOC == internalType());
    return d_as.d_int32;
#endif  // end - 64 bit
}

inline
bsl::size_t Datum::theInternalStringAllocNumBytes() const
{
    BSLS_ASSERT_SAFE(isString());
    BSLS_ASSERT_SAFE(e_INTERNAL_STRING == internalType());

#ifdef BSLS_PLATFORM_CPU_32_BIT
    const char *data = allocatedPtr<const char>();
    const bsl::size_t msgLen  = Datum_Helpers::load<SizeType>(data, 0);
    const bsl::size_t align   = sizeof(SizeType);
    const bsl::size_t headLen = sizeof(SizeType);

    return headLen + ((msgLen + align - 1) & ~(align - 1));
#else   // end - 32 bit / begin - 64 bit

    return d_as.d_int32;
#endif  // end - 64 bit
    }

inline
bsl::size_t Datum::theInternalArrayAllocNumBytes() const
{
    BSLS_ASSERT_SAFE(isArray());
    BSLS_ASSERT_SAFE(e_INTERNAL_ARRAY == internalType());

    const Datum *data = allocatedPtr<const Datum>();
    if (data) {
        const bsl::size_t length = *reinterpret_cast<const SizeType*>(data);
        return (length + 1) * sizeof(Datum);                          // RETURN
    }
    return 0;

}

// CLASS METHODS
inline
Datum Datum::createArrayReference(const Datum          *array,
                                  SizeType              length,
                                  const AllocatorType&  allocator)
{
    BSLS_ASSERT(array || 0 == length);

#ifdef BSLS_PLATFORM_CPU_32_BIT
    // If the length will fit in the 'd_ushort' area, store everything inline;
    // otherwise, must allocate space.

    if (bsl::numeric_limits<unsigned short>::max() >= length) {
        Datum result;
        result.d_as.d_exponent = k_DOUBLE_MASK | e_INTERNAL_ARRAY_REFERENCE;
        result.d_as.d_ushort = static_cast<unsigned short>(length);
        result.d_as.d_cvp = array;
        return result;                                                // RETURN
    }

    void *mem = AllocUtil::allocateBytes(allocator,
                                         sizeof(array) + sizeof(length));
    Datum_Helpers::store<const Datum *>(mem, 0,             array);
    Datum_Helpers::store<SizeType>     (mem, sizeof(array), length);

    return createExtendedDataObject(e_EXTENDED_INTERNAL_AREF_ALLOC, mem);
#else   // end - 32 bit / begin - 64 bit
    (void)allocator;

    BSLS_ASSERT(length <= bsl::numeric_limits<unsigned int>::max());

    Datum result;
    result.d_as.d_type  = e_INTERNAL_ARRAY_REFERENCE;
    result.d_as.d_int32 = static_cast<int>(length);
    result.d_as.d_ptr   = reinterpret_cast<void*>(const_cast<Datum*>(array));
    return result;
#endif  // end - 64 bit
}

inline
Datum Datum::createArrayReference(const DatumArrayRef& value,
                                  const AllocatorType& allocator)
{
    return createArrayReference(value.data(), value.length(), allocator);
}

inline
Datum Datum::createBoolean(bool value)
{
    Datum result;
#ifdef BSLS_PLATFORM_CPU_32_BIT
    result.d_exp.d_value = (k_DOUBLE_MASK | e_INTERNAL_BOOLEAN)
                            << k_TYPE_MASK_BITS;
    result.d_as.d_int    = value;
#else   // end - 32 bit / begin - 64 bit
    result.d_as.d_type   = e_INTERNAL_BOOLEAN;
    result.d_as.d_int32  = value;
#endif  // end - 64 bit
    return result;
}

inline
Datum Datum::createDate(const bdlt::Date& value)
{
    Datum result;
#ifdef BSLS_PLATFORM_CPU_32_BIT
    BSLMF_ASSERT(sizeof(value) == sizeof(result.d_as.d_int));
    BSLMF_ASSERT(bslmf::IsBitwiseCopyable<bdlt::Date>::value);

    result.d_exp.d_value = (k_DOUBLE_MASK | e_INTERNAL_DATE)
                            << k_TYPE_MASK_BITS;
    *reinterpret_cast<bdlt::Date*>(&result.d_as.d_int) = value;
#else   // end - 32 bit / begin - 64 bit
    result.d_as.d_type = e_INTERNAL_DATE;
    new (result.theInlineStorage()) bdlt::Date(value);
#endif  // end - 64 bit
    return result;
}

inline
Datum Datum::createDatetime(const bdlt::Datetime& value,
                            const AllocatorType&  allocator)
{
    Datum result;
#ifdef BSLS_PLATFORM_CPU_32_BIT
    // Check if number of days from now fits in two bytes.

    int dateOffsetFromEpoch = (value.date() - bdlt::EpochUtil::epoch().date())
                                                - k_DATETIME_OFFSET_FROM_EPOCH;
    short shortDateOffsetFromEpoch = static_cast<short>(dateOffsetFromEpoch);

    if (static_cast<int>(shortDateOffsetFromEpoch) == dateOffsetFromEpoch &&
        value.microsecond() == 0) {
        result.d_exp.d_value =
            (k_DOUBLE_MASK | e_INTERNAL_DATETIME) << k_TYPE_MASK_BITS
            | (0xffff & dateOffsetFromEpoch);
        bdlt::DatetimeInterval interval = value.time() - bdlt::Time();
        result.d_as.d_int = static_cast<int>(interval.totalMilliseconds());
    } else {
        void *mem = AllocUtil::newObject<bdlt::Datetime>(allocator, value);
        result = createExtendedDataObject(e_EXTENDED_INTERNAL_DATETIME_ALLOC,
                                          mem);
    }
#else   // end - 32 bit / begin - 64 bit
    (void)allocator;

    result.d_as.d_type = e_INTERNAL_DATETIME;
    new (result.theInlineStorage()) bdlt::Datetime(value);
#endif  // end - 64 bit
    return result;
}

inline
Datum Datum::createDatetimeInterval(const bdlt::DatetimeInterval& value,
                                    const AllocatorType&          allocator)
{
    Datum result;
#ifdef BSLS_PLATFORM_CPU_32_BIT
    const int                usValue = value.microseconds();
    const bsls::Types::Int64 msValue = value.totalMilliseconds();

    if (usValue == 0 &&  // Low-resolution (old) interval
        Datum_Helpers32::storeSmallInt64(msValue,
                                         &result.d_as.d_short,
                                         &result.d_as.d_int)) {
        result.d_as.d_exponent = k_DOUBLE_MASK | e_INTERNAL_DATETIME_INTERVAL;
    } else {
        void *mem = AllocUtil::newObject<bdlt::DatetimeInterval>(allocator,
                                                                 value);
        result = createExtendedDataObject(
                                   e_EXTENDED_INTERNAL_DATETIME_INTERVAL_ALLOC,
                                   mem);
    }
#else   // end - 32 bit / begin - 64 bit
    (void)allocator;

    result.d_as.d_type = e_INTERNAL_DATETIME_INTERVAL;
    result.d_as.d_int32 = value.days();
    result.d_as.d_int64 = value.fractionalDayInMicroseconds();
#endif  // end - 64 bit
    return result;
}

inline
Datum Datum::createDouble(double value)
{
    Datum result;
#ifdef BSLS_PLATFORM_CPU_32_BIT
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(!(value == value))) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return createExtendedDataObject(e_EXTENDED_INTERNAL_NAN2, 0); // RETURN
    } else {
        result.d_double = value;
    }
#else   // end - 32 bit / begin - 64 bit
    result.d_as.d_type   = e_INTERNAL_DOUBLE;
    result.d_as.d_double = value;
#endif  // end - 64 bit
    return result;
}

inline
Datum Datum::createError(int code)
{
#ifdef BSLS_PLATFORM_CPU_32_BIT
    return createExtendedDataObject(e_EXTENDED_INTERNAL_ERROR, code);
#else   // end - 32 bit / begin - 64 bit
    return createDatum(e_INTERNAL_ERROR, code);
#endif  // end - 64 bit
}

inline
Datum Datum::createInteger(int value)
{
    Datum result;
#ifdef BSLS_PLATFORM_CPU_32_BIT
    result.d_exp.d_value = (k_DOUBLE_MASK | e_INTERNAL_INTEGER)
                            << k_TYPE_MASK_BITS;
    result.d_as.d_int    = value;
#else   // end - 32 bit / begin - 64 bit
    result.d_as.d_type   = e_INTERNAL_INTEGER;
    result.d_as.d_int32  = value;
#endif  // end - 64 bit
    return result;
}

inline
Datum Datum::createInteger64(bsls::Types::Int64   value,
                             const AllocatorType& allocator)
{
    Datum result;
#ifdef BSLS_PLATFORM_CPU_32_BIT
    if (Datum_Helpers32::storeSmallInt64(value,
                                         &result.d_as.d_short,
                                         &result.d_as.d_int)) {
        result.d_as.d_exponent = k_DOUBLE_MASK | e_INTERNAL_INTEGER64;
    } else {
        void *mem = AllocUtil::newObject<bsls::Types::Int64>(allocator, value);
        result = createExtendedDataObject(e_EXTENDED_INTERNAL_INTEGER64_ALLOC,
                                          mem);
    }
#else   // end - 32 bit / begin - 64 bit
    (void)allocator;

    result.d_as.d_type  = e_INTERNAL_INTEGER64;
    result.d_as.d_int64 = value;
#endif  // end - 64 bit
    return result;
}

inline
Datum Datum::createNull()
{
    Datum result;
#ifdef BSLS_PLATFORM_CPU_32_BIT
    // Setting exponent using half-word is faster, maybe the compiler folds the
    // two statements into one?

    result.d_as.d_exponent = k_DOUBLE_MASK | Datum::e_INTERNAL_EXTENDED;
    result.d_as.d_ushort   = e_EXTENDED_INTERNAL_NIL;
#else   // end - 32 bit / begin - 64 bit
    result.d_as.d_type     = e_INTERNAL_NIL;
#endif  // end - 64 bit
    return result;
}

inline
Datum Datum::createStringRef(const char           *string,
                             SizeType              length,
                             const AllocatorType&  allocator)
{
    BSLS_ASSERT(string || 0 == length);

#ifdef BSLS_PLATFORM_CPU_32_BIT
    // If the length will fit in the 'k_SHORT_OFFSET' area, store everything
    // inline; otherwise allocate space.

    if (bsl::numeric_limits<unsigned short>::max() >= length) {
        Datum result;
        result.d_exp.d_value = (k_DOUBLE_MASK | e_INTERNAL_STRING_REFERENCE)
                                << k_TYPE_MASK_BITS | length;
        result.d_as.d_cvp = string;
        return result;                                                // RETURN
    }

    void *mem = AllocUtil::allocateBytes(allocator,
                                         sizeof(length) + sizeof(string));
    Datum_Helpers::store<const char *>(mem, 0,              string);
    Datum_Helpers::store<SizeType>    (mem, sizeof(string), length);

    return createExtendedDataObject(e_EXTENDED_INTERNAL_SREF_ALLOC, mem);
#else   // end - 32 bit / begin - 64 bit
    (void)allocator;

    BSLS_ASSERT(length <= bsl::numeric_limits<unsigned int>::max());

    Datum result;
    result.d_as.d_type  = e_INTERNAL_STRING_REFERENCE;
    result.d_as.d_int32 = static_cast<int>(length);
    result.d_as.d_ptr   = const_cast<char*>(string);
    return result;
#endif  // end - 64 bit
}

inline
Datum Datum::createStringRef(const char           *string,
                             const AllocatorType&  allocator)
{
    BSLS_ASSERT(string);

    return createStringRef(string, bsl::strlen(string), allocator);
}

inline
Datum Datum::createStringRef(const bslstl::StringRef& value,
                             const AllocatorType&     allocator)
{
    return createStringRef(value.data(), value.length(), allocator);
}

inline
Datum Datum::createTime(const bdlt::Time& value)
{
    Datum result;
#ifdef BSLS_PLATFORM_CPU_32_BIT
    result.d_exp.d_value = (k_DOUBLE_MASK | e_INTERNAL_TIME)
                            << k_TYPE_MASK_BITS;
    bsls::Types::Int64 rawTime;
    BSLMF_ASSERT(bslmf::IsBitwiseCopyable<bdlt::Time>::value);
    *reinterpret_cast<bdlt::Time*>(&rawTime) = value;
    const bool rc = Datum_Helpers32::storeInt48(rawTime,
                                                &result.d_as.d_short,
                                                &result.d_as.d_int);
    BSLS_ASSERT(rc);  (void)rc;
#else   // end - 32 bit / begin - 64 bit
    result.d_as.d_type = e_INTERNAL_TIME;
    new (result.theInlineStorage()) bdlt::Time(value);
#endif  // end - 64 bit
    return result;
}

inline
Datum Datum::createUdt(void *data, int type)
{
    BSLS_ASSERT(0 <= type && type <= 65535);

    Datum result;
#ifdef BSLS_PLATFORM_CPU_32_BIT
    result.d_as.d_exponent = k_DOUBLE_MASK | e_INTERNAL_USERDEFINED;
    result.d_as.d_ushort = static_cast<unsigned short>(type);
    result.d_as.d_cvp = data;
#else   // end - 32 bit / begin - 64 bit
    result.d_as.d_type  = e_INTERNAL_USERDEFINED;
    result.d_as.d_int32 = type;
    result.d_as.d_ptr   = data;
#endif  // end - 64 bit
    return result;
}

inline
Datum Datum::adoptArray(const DatumMutableArrayRef& array)
{
    // Note that 'array.length' contains the *address* of the 'length'
    // information for the array, which precedes the 'array' data in a
    // contiguously allocated block (see 'DatumMutableArrayRef').

    Datum result;
#ifdef BSLS_PLATFORM_CPU_32_BIT
    result.d_as.d_exponent = k_DOUBLE_MASK | e_INTERNAL_ARRAY;
    result.d_as.d_cvp = array.length();
#else   // end - 32 bit / begin - 64 bit
    result.d_as.d_type = e_INTERNAL_ARRAY;
    result.d_as.d_ptr  = array.length();
#endif  // end - 64 bit
    return result;
}

inline
Datum Datum::adoptMap(const DatumMutableMapRef& map)
{
    // Note that 'map.size' contains the *address* of the 'size' information
    // for the map, which precedes the 'map' data in a contiguously allocated
    // block (see 'DatumMutableMapRef').

#ifdef BSLS_PLATFORM_CPU_32_BIT
    return createExtendedDataObject(e_EXTENDED_INTERNAL_MAP, map.size());
#else   // end - 32 bit / begin - 64 bit
    return createDatum(e_INTERNAL_MAP, map.size());
#endif  // end - 64 bit
}

inline
Datum Datum::adoptIntMap(const DatumMutableIntMapRef& map)
{
    // Note that 'map.size' contains the *address* of the 'size' information
    // for the map, which precedes the 'map' data in a contiguously allocated
    // block (see 'DatumMutableIntMapRef').

#ifdef BSLS_PLATFORM_CPU_32_BIT
    return createExtendedDataObject(e_EXTENDED_INTERNAL_INT_MAP, map.size());
#else   // end - 32 bit / begin - 64 bit
    return createDatum(e_INTERNAL_INT_MAP, map.size());
#endif  // end - 64 bit
}

inline
Datum Datum::adoptMap(const DatumMutableMapOwningKeysRef& map)
{
    // Note that 'map.size' contains the *address* of the 'size' information
    // for the map, which precedes the 'map' data in a contiguously allocated
    // block (see 'DatumMutableMapOwningKeysRefRef').

#ifdef BSLS_PLATFORM_CPU_32_BIT
    return createExtendedDataObject(e_EXTENDED_INTERNAL_OWNED_MAP,
                                    map.size());
#else   // end - 32 bit / begin - 64 bit
    return createDatum(e_INTERNAL_OWNED_MAP, map.size());
#endif  // end - 64 bit
}

inline
Datum Datum::copyString(const bslstl::StringRef& value,
                        const AllocatorType&     allocator)
{
    return copyString(value.data(), value.length(), allocator);
}

inline
void Datum::disposeUninitializedArray(const DatumMutableArrayRef& array,
                                      const AllocatorType&        allocator)
{
    void *ptr = array.allocatedPtr();

    if (ptr) {
        AllocUtil::deallocateBytes(allocator, ptr, array.capacity());
    }
}

inline
void Datum::disposeUninitializedIntMap(const DatumMutableIntMapRef& map,
                                       const AllocatorType&         allocator)
{
    void *ptr = map.allocatedPtr();

    if (ptr) {
        Datum_IntMapHeader *hdr = static_cast<Datum_IntMapHeader*>(ptr);

        AllocUtil::deallocateBytes(
                             allocator,
                             ptr,
                             (hdr->d_capacity + 1) * sizeof(DatumIntMapEntry));
    }
}

inline
void Datum::disposeUninitializedMap(const DatumMutableMapRef& map,
                                    const AllocatorType&      allocator)
{
    void *ptr = map.allocatedPtr();

    if (ptr) {
        Datum_MapHeader *hdr = static_cast<Datum_MapHeader*>(ptr);

        AllocUtil::deallocateBytes(allocator, ptr, hdr->d_allocatedSize);
    }
}

inline
void Datum::disposeUninitializedMap(
                                 const DatumMutableMapOwningKeysRef& map,
                                 const AllocatorType&                allocator)
{
    void *ptr = map.allocatedPtr();

    if (ptr) {
        Datum_MapHeader *hdr = static_cast<Datum_MapHeader*>(ptr);

        AllocUtil::deallocateBytes(allocator, ptr, hdr->d_allocatedSize);
    }
}

// ACCESSORS
inline
bool Datum::isArray() const
{
    return (e_ARRAY == type());
}

inline
bool Datum::isBinary() const
{
    return (e_BINARY == type());
}

inline
bool Datum::isBoolean() const
{
    return (e_BOOLEAN == type());
}

inline
bool Datum::isDate() const
{
    return (e_DATE == type());
}

inline
bool Datum::isDatetime() const
{
    return (e_DATETIME == type());
}

inline
bool Datum::isDatetimeInterval() const
{
    return (e_DATETIME_INTERVAL == type());
}

inline
bool Datum::isDecimal64() const
{
    return (e_DECIMAL64 == type());
}

inline
bool Datum::isDouble() const
{
    return (e_DOUBLE == type());
}

inline
bool Datum::isError() const
{
    return (e_ERROR == type());
}

inline
bool Datum::isExternalReference() const
{
#ifdef BSLS_PLATFORM_CPU_32_BIT
    switch (internalType()) {
      case e_INTERNAL_STRING_REFERENCE:
      case e_INTERNAL_ARRAY_REFERENCE:
      case e_INTERNAL_USERDEFINED:
        return true;                                                  // RETURN
      case e_INTERNAL_EXTENDED:
        switch (extendedInternalType()) {
          case e_EXTENDED_INTERNAL_SREF_ALLOC:
          case e_EXTENDED_INTERNAL_AREF_ALLOC:
            return true;                                              // RETURN
          default:
            break;
        }
      default:
          break;
    }
#else   // end - 32 bit / begin - 64 bit
    switch (internalType()) {
      case e_INTERNAL_STRING_REFERENCE:
      case e_INTERNAL_ARRAY_REFERENCE:
      case e_INTERNAL_USERDEFINED:
        return true;                                                  // RETURN
      case e_INTERNAL_UNINITIALIZED:
        BSLS_ASSERT(0 == "Uninitialized Datum");
        break;
      default:
        break;
    }
#endif  // end - 64 bit
    return false;
}

inline
bool Datum::isInteger() const
{
    return (e_INTEGER == type());
}

inline
bool Datum::isInteger64() const
{
    return (e_INTEGER64 == type());
}

inline
bool Datum::isIntMap() const
{
    return (e_INT_MAP == type());
}

inline
bool Datum::isMap() const
{
    return (e_MAP == type());
}

inline
bool Datum::isNull() const
{
    return (e_NIL == type());
}

inline
bool Datum::isString() const
{
    return (e_STRING == type());
}

inline
bool Datum::isTime() const
{
    return (e_TIME == type());
}

inline
bool Datum::isUdt() const
{
    return (e_USERDEFINED == type());
}

inline
DatumArrayRef Datum::theArray() const
{
    BSLS_ASSERT_SAFE(isArray());

    const InternalDataType type = internalType();
    if (e_INTERNAL_ARRAY == type) {
        return theInternalArray();                                    // RETURN
    }

#ifdef BSLS_PLATFORM_CPU_32_BIT
    if (e_INTERNAL_EXTENDED == type) {
        return theLongArrayReference();                               // RETURN
    }
#endif // end - 32 bit

    return theArrayReference();
}

inline
DatumBinaryRef Datum::theBinary() const
{
    BSLS_ASSERT_SAFE(isBinary());

#ifdef BSLS_PLATFORM_CPU_32_BIT
    return DatumBinaryRef(allocatedPtr<const double>() + 1,
                          *allocatedPtr<const SizeType>());           // RETURN
#else   // end - 32 bit / begin - 64 bit
    const InternalDataType type = internalType();
    switch(type) {
      case e_INTERNAL_BINARY:
        return DatumBinaryRef(d_data.buffer(),                        // RETURN
                              d_data.buffer()[k_SMALLBINARY_SIZE_OFFSET]);
      case e_INTERNAL_BINARY_ALLOC:
        return DatumBinaryRef(d_as.d_ptr, d_as.d_int32);              // RETURN
      default:
        BSLS_ASSERT(0 == "Bad binary internal-type (memory corruption?)");
    }
    return DatumBinaryRef();
#endif  // end - 64 bit
}

inline
bool Datum::theBoolean() const
{
    BSLS_ASSERT_SAFE(isBoolean());

#ifdef BSLS_PLATFORM_CPU_32_BIT
    return static_cast<bool>(d_as.d_int);
#else   // end - 32 bit / begin - 64 bit
    return d_as.d_int32;
#endif  // end - 64 bit
}

inline
bdlt::Date Datum::theDate() const
{
    BSLS_ASSERT_SAFE(isDate());

#ifdef BSLS_PLATFORM_CPU_32_BIT
    return *reinterpret_cast<const bdlt::Date *>(&d_as.d_int);
#else   // end - 32 bit / begin - 64 bit
    return *reinterpret_cast<const bdlt::Date *>(theInlineStorage());
#endif  // end - 64 bit
}

inline
bdlt::Datetime Datum::theDatetime() const
{
    BSLS_ASSERT_SAFE(isDatetime());

#ifdef BSLS_PLATFORM_CPU_32_BIT
    const InternalDataType type = internalType();

    if (type == e_INTERNAL_DATETIME) {
        bdlt::Time time;
        time.addMilliseconds(d_as.d_int);
        return bdlt::Datetime(
               bdlt::EpochUtil::epoch().date() + k_DATETIME_OFFSET_FROM_EPOCH +
                                                                  d_as.d_short,
               time);                                                 // RETURN
    }

    BSLS_ASSERT_SAFE(type == e_INTERNAL_EXTENDED);
    BSLS_ASSERT_SAFE(
            extendedInternalType() == e_EXTENDED_INTERNAL_DATETIME_ALLOC);
    return *allocatedPtr<const bdlt::Datetime>();
#else   // end - 32 bit / begin - 64 bit
    return *reinterpret_cast<const bdlt::Datetime *>(theInlineStorage());
#endif  // end - 64 bit
}

inline // BDLD_DATUM_FORCE_INLINE
bdlt::DatetimeInterval Datum::theDatetimeInterval() const
{
    BSLS_ASSERT_SAFE(isDatetimeInterval());

#ifdef BSLS_PLATFORM_CPU_32_BIT
    const InternalDataType type = internalType();

    if (type == e_INTERNAL_DATETIME_INTERVAL) {
        bdlt::DatetimeInterval result;
        result.setTotalMilliseconds(
            Datum_Helpers32::loadSmallInt64(d_as.d_short, d_as.d_int));
        return result;                                                // RETURN
    }

    BSLS_ASSERT_SAFE(type == e_INTERNAL_EXTENDED);
    BSLS_ASSERT_SAFE(
        extendedInternalType() == e_EXTENDED_INTERNAL_DATETIME_INTERVAL_ALLOC);
    return *allocatedPtr<const bdlt::DatetimeInterval>();
#else   // end - 32 bit / begin - 64 bit
    return bdlt::DatetimeInterval(d_as.d_int32,   // days
                                  0,              // hours
                                  0,              // minutes
                                  0,              // seconds
                                  0,              // milliseconds
                                  d_as.d_int64);  // microseconds
#endif  // end - 64 bit
}

inline
double Datum::theDouble() const
{
    BSLS_ASSERT_SAFE(isDouble());

#ifdef BSLS_PLATFORM_CPU_32_BIT
    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(
           0x7f != d_data[k_EXPONENT_MSB] ||           // exponent is not the
           0xf0 != (d_data[k_EXPONENT_LSB] & 0xf0) ||  // special '7ff' value
           e_INTERNAL_INF == (d_data[k_EXPONENT_LSB] & 0x0f))) { // or infinity
        return d_double;                                              // RETURN
    }
    BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
    return bsl::numeric_limits<double>::quiet_NaN();                  // RETURN
#else   // end - 32 bit / begin - 64 bit
    return d_as.d_double;
#endif  // end - 64 bit
}

inline
DatumError Datum::theError() const
{
    BSLS_ASSERT(isError());

#ifdef BSLS_PLATFORM_CPU_32_BIT
    // If the extended type is 'e_EXTENDED_INTERNAL_ERROR', we are storing
    // just a code, at the data offset.  Otherwise, we're storing an allocated
    // object.

    if (e_EXTENDED_INTERNAL_ERROR == extendedInternalType()) {
        return DatumError(d_as.d_int);                                // RETURN
    }

    const char *data = allocatedPtr<const char>();
#else   // end - 32 bit / begin - 64 bit
    if (e_INTERNAL_ERROR == internalType()) {
        return DatumError(static_cast<int>(d_as.d_int64));            // RETURN
    }

    const char *data = allocatedPtr<const char>();
#endif  // end - 64 bit

    return DatumError(
        Datum_Helpers::load<int>(data, 0),
        bslstl::StringRef(data + 2 * sizeof(int),
                          Datum_Helpers::load<int>(data, sizeof(int))));
}

inline
int Datum::theInteger() const
{
    BSLS_ASSERT_SAFE(isInteger());

#ifdef BSLS_PLATFORM_CPU_32_BIT
    return d_as.d_int;
#else   // end - 32 bit / begin - 64 bit
    return d_as.d_int32;
#endif  // end - 64 bit
}

inline
bsls::Types::Int64 Datum::theInteger64() const
{
    BSLS_ASSERT_SAFE(isInteger64());

#ifdef BSLS_PLATFORM_CPU_32_BIT
    const InternalDataType type = internalType();

    if (BSLS_PERFORMANCEHINT_PREDICT_LIKELY(e_INTERNAL_INTEGER64 == type)) {
        return theSmallInteger64();                                   // RETURN
    }
    BSLS_ASSERT_SAFE(e_INTERNAL_EXTENDED == type);
    BSLS_ASSERT_SAFE(
            e_EXTENDED_INTERNAL_INTEGER64_ALLOC == extendedInternalType());
    return theLargeInteger64();                                       // RETURN
#else   // end - 32 bit / begin - 64 bit
    return d_as.d_int64;
#endif  // end - 64 bit
}

inline
DatumMapRef Datum::theMap() const
{
    BSLS_ASSERT_SAFE(isMap());

    const DatumMapEntry *map = allocatedPtr<const DatumMapEntry>();

    if (map) {
        // Map header takes first DatumMapEntry
        const Datum_MapHeader *header =
                                reinterpret_cast<const Datum_MapHeader *>(map);

        return DatumMapRef(map + 1,
                           header->d_size,
                           header->d_sorted,
                           header->d_ownsKeys);                       // RETURN
    }
    return DatumMapRef(0, 0, false, false);
}

inline
DatumIntMapRef Datum::theIntMap() const
{
    BSLS_ASSERT_SAFE(isIntMap());

    const DatumIntMapEntry *map = allocatedPtr<const DatumIntMapEntry>();

    if (map) {
        // Map header takes first DatumMapEntry
        const Datum_IntMapHeader *header =
                             reinterpret_cast<const Datum_IntMapHeader *>(map);

        return DatumIntMapRef(map + 1,
                              header->d_size,
                              header->d_sorted);                      // RETURN
    }
    return DatumIntMapRef(0, 0, false);
}

inline
bslstl::StringRef Datum::theString() const
{
    BSLS_ASSERT_SAFE(isString());

    const InternalDataType type = internalType();
    switch(type) {
      case e_INTERNAL_SHORTSTRING:
        return theShortString();                                      // RETURN
      case e_INTERNAL_STRING:
        return theInternalString();                                   // RETURN
      case e_INTERNAL_STRING_REFERENCE:
        return theStringReference();                                  // RETURN
#ifdef BSLS_PLATFORM_CPU_32_BIT
      case e_INTERNAL_LONGEST_SHORTSTRING:
        return theLongestShortString();                               // RETURN
      default:
        return theLongStringReference();                              // RETURN
#else   // end - 32 bit / begin - 64 bit
      default: {
        BSLS_ASSERT(0 == "Bad string internal-type (memory corruption?)");
        return bslstl::StringRef();                                   // RETURN
      }
#endif  // end - 64 bit
    }
}

inline
bdlt::Time Datum::theTime() const
{
    BSLS_ASSERT_SAFE(isTime());

#ifdef BSLS_PLATFORM_CPU_32_BIT
    BSLMF_ASSERT(bslmf::IsBitwiseCopyable<bdlt::Time>::value);
    bsls::Types::Int64 rawTime;
    rawTime = Datum_Helpers32::loadInt48(d_as.d_short, d_as.d_int);
    return *reinterpret_cast<bdlt::Time*>(&rawTime);
#else   // end - 32 bit / begin - 64 bit
    return *reinterpret_cast<const bdlt::Time *>(theInlineStorage());
#endif  // end - 64 bit
}

inline
DatumUdt Datum::theUdt() const
{
    BSLS_ASSERT_SAFE(isUdt());
#ifdef BSLS_PLATFORM_CPU_32_BIT
    return DatumUdt(allocatedPtr<void>(), d_as.d_ushort);
#else   // end - 32 bit / begin - 64 bit
    return DatumUdt(allocatedPtr<void>(), d_as.d_int32);
#endif  // end - 64 bit
}

inline
Datum::DataType Datum::type() const
{
#ifdef BSLS_PLATFORM_CPU_32_BIT
    static const DataType convert[] = {
          e_DOUBLE                      // e_INTERNAL_INF                = 0x00
        , e_STRING                      // e_INTERNAL_LONGEST_SHORTSTR   = 0x01
        , e_BOOLEAN                     // e_INTERNAL_BOOLEAN            = 0x02
        , e_STRING                      // e_INTERNAL_SHORTSTRING        = 0x03
        , e_STRING                      // e_INTERNAL_STRING             = 0x04
        , e_DATE                        // e_INTERNAL_DATE               = 0x05
        , e_TIME                        // e_INTERNAL_TIME               = 0x06
        , e_DATETIME                    // e_INTERNAL_DATETIME           = 0x07
        , e_DATETIME_INTERVAL           // e_INTERNAL_DATETIME_INTERVAL  = 0x08
        , e_INTEGER                     // e_INTERNAL_INTEGER            = 0x09
        , e_INTEGER64                   // e_INTERNAL_INTEGER64          = 0x0a
        , e_USERDEFINED                 // e_INTERNAL_USERDEFINED        = 0x0b
        , e_ARRAY                       // e_INTERNAL_ARRAY              = 0x0c
        , e_STRING                      // e_INTERNAL_STRING_REFERENCE   = 0x0d
        , e_ARRAY                       // e_INTERNAL_ARRAY_REFERENCE    = 0x0e
        , e_NIL                         // e_INTERNAL_EXTENDED           = 0x0f
        , e_DOUBLE                      // e_INTERNAL_DOUBLE             = 0x10
    };

    const InternalDataType type = internalType();
    if (e_INTERNAL_EXTENDED == type) {
        return typeFromExtendedInternalType();                        // RETURN
    }
    return convert[type];
#else   // end - 32 bit / begin - 64 bit
    static const DataType convert[] = {
        e_ERROR                            // e_INTERNAL_UNINITIALIZED; invalid
      , e_DOUBLE                           // e_INTERNAL_INF               = 1
      , e_NIL                              // e_INTERNAL_NIL               = 2
      , e_BOOLEAN                          // e_INTERNAL_BOOLEAN           = 3
      , e_STRING                           // e_INTERNAL_SHORTSTRING       = 4
      , e_STRING                           // e_INTERNAL_STRING            = 5
      , e_DATE                             // e_INTERNAL_DATE              = 6
      , e_TIME                             // e_INTERNAL_TIME              = 7
      , e_DATETIME                         // e_INTERNAL_DATETIME          = 8
      , e_DATETIME_INTERVAL                // e_INTERNAL_DATETIME_INTERVAL = 9
      , e_INTEGER                          // e_INTERNAL_INTEGER           = 10
      , e_INTEGER64                        // e_INTERNAL_INTEGER64         = 11
      , e_USERDEFINED                      // e_INTERNAL_USERDEFINED       = 12
      , e_ARRAY                            // e_INTERNAL_ARRAY             = 13
      , e_STRING                           // e_INTERNAL_STRING_REFERENCE  = 14
      , e_ARRAY                            // e_INTERNAL_ARRAY_REFERENCE   = 15
      , e_DOUBLE                           // e_INTERNAL_DOUBLE            = 16
      , e_MAP                              // e_INTERNAL_MAP               = 17
      , e_MAP                              // e_INTERNAL_OWNED_MAP         = 18
      , e_ERROR                            // e_INTERNAL_ERROR             = 19
      , e_ERROR                            // e_INTERNAL_ERROR_ALLOC       = 20
      , e_BINARY                           // e_INTERNAL_BINARY            = 21
      , e_BINARY                           // e_INTERNAL_BINARY_ALLOC      = 22
      , e_DECIMAL64                        // e_INTERNAL_DECIMAL64         = 23
      , e_INT_MAP                          // e_INTERNAL_INT_MAP           = 24
    };

    const InternalDataType type = internalType();

    BSLS_ASSERT_SAFE(e_INTERNAL_UNINITIALIZED != type);

    return convert[type];
#endif  // end - 64 bit
}

template <class t_VISITOR>
void Datum::apply(t_VISITOR& visitor) const
{
#ifdef BSLS_PLATFORM_CPU_32_BIT
    switch (internalType()) {
      case e_INTERNAL_INF:
        visitor(bsl::numeric_limits<double>::infinity());
        break;
      case e_INTERNAL_BOOLEAN:
        visitor(theBoolean());
        break;
      case e_INTERNAL_SHORTSTRING:
        visitor(theShortString());
        break;
      case e_INTERNAL_LONGEST_SHORTSTRING:
        visitor(theLongestShortString());
        break;
      case e_INTERNAL_STRING:
        visitor(theInternalString());
        break;
      case e_INTERNAL_DATE:
        visitor(theDate());
        break;
      case e_INTERNAL_TIME:
        visitor(theTime());
        break;
      case e_INTERNAL_DATETIME:
        visitor(theDatetime());
        break;
      case e_INTERNAL_DATETIME_INTERVAL:
        visitor(theDatetimeInterval());
        break;
      case e_INTERNAL_INTEGER:
        visitor(theInteger());
        break;
      case e_INTERNAL_INTEGER64:
        visitor(theInteger64());
        break;
      case e_INTERNAL_USERDEFINED:
        visitor(theUdt());
        break;
      case e_INTERNAL_ARRAY:
        visitor(theInternalArray());
        break;
      case e_INTERNAL_STRING_REFERENCE:
        visitor(theStringReference());
        break;
      case e_INTERNAL_ARRAY_REFERENCE:
        visitor(theArrayReference());
        break;
      case e_INTERNAL_EXTENDED:
        switch (extendedInternalType()) {
          case e_EXTENDED_INTERNAL_INT_MAP:
            visitor(theIntMap());
            break;
          case e_EXTENDED_INTERNAL_MAP:
            BSLS_ANNOTATION_FALLTHROUGH;
          case e_EXTENDED_INTERNAL_OWNED_MAP:
            visitor(theMap());
            break;
          case e_EXTENDED_INTERNAL_NAN2:
            visitor(theDouble());
            break;
          case e_EXTENDED_INTERNAL_ERROR:
            BSLS_ANNOTATION_FALLTHROUGH;
          case e_EXTENDED_INTERNAL_ERROR_ALLOC:
            visitor(theError());
            break;
          case e_EXTENDED_INTERNAL_SREF_ALLOC:
            visitor(theLongStringReference());
            break;
          case e_EXTENDED_INTERNAL_AREF_ALLOC:
            visitor(theLongArrayReference());
            break;
          case e_EXTENDED_INTERNAL_DATETIME_ALLOC:
            visitor(theDatetime());
            break;
          case e_EXTENDED_INTERNAL_DATETIME_INTERVAL_ALLOC:
            visitor(theDatetimeInterval());
            break;
          case e_EXTENDED_INTERNAL_INTEGER64_ALLOC:
            visitor(theInteger64());
            break;
          case e_EXTENDED_INTERNAL_BINARY_ALLOC:
            visitor(theBinary());
            break;
          case e_EXTENDED_INTERNAL_DECIMAL64:
          case e_EXTENDED_INTERNAL_DECIMAL64_SPECIAL:
          case e_EXTENDED_INTERNAL_DECIMAL64_ALLOC: {
            // Code below is identical to 'visitor(theDecimal64())' but
            // postpones the name lookup for 'theDecimal64()' to the second
            // (instantiation) phase, so that we can use only a forward
            // declaration of 'bdldfp::Decimal64' in this header.  Without this
            // workaround clang won't compile calls to 'apply' or 'hashAppend'
            // even if '<bdldfp_decimal.h>' is included after this header.  See
            // the test driver for more information.
            typedef bdldfp::Decimal64 (Datum::*Dec64MemFunPtr)() const;
            const typename Datum_MakeDependent<
                        Dec64MemFunPtr,
                        t_VISITOR>::type dec64MemFunPtr = &Datum::theDecimal64;
            visitor((this->*dec64MemFunPtr)());
          } break;
          case e_EXTENDED_INTERNAL_NIL:
            visitor(bslmf::Nil());
            break;
          default:
            BSLS_ASSERT_SAFE(
                            0 == "Unknown extended-type (memory corruption?)");
        }
        break;
      case e_INTERNAL_DOUBLE:
        visitor(d_double);
        break;
      default:
        BSLS_ASSERT_SAFE(0 == "Unknown type (memory corruption?)");
    }
#else   // end - 32 bit / begin - 64 bit
    switch (internalType()) {
      case e_INTERNAL_INF:
        visitor(bsl::numeric_limits<double>::infinity());
        break;
      case e_INTERNAL_NIL:
        visitor(bslmf::Nil());
        break;
      case e_INTERNAL_BOOLEAN:
        visitor(theBoolean());
        break;
      case e_INTERNAL_SHORTSTRING:
        visitor(theShortString());
        break;
      case e_INTERNAL_STRING:
        visitor(theInternalString());
        break;
      case e_INTERNAL_DATE:
        visitor(theDate());
        break;
      case e_INTERNAL_TIME:
        visitor(theTime());
        break;
      case e_INTERNAL_DATETIME:
        visitor(theDatetime());
        break;
      case e_INTERNAL_DATETIME_INTERVAL:
        visitor(theDatetimeInterval());
        break;
      case e_INTERNAL_INTEGER:
        visitor(theInteger());
        break;
      case e_INTERNAL_INTEGER64:
        visitor(theInteger64());
        break;
      case e_INTERNAL_USERDEFINED:
        visitor(theUdt());
        break;
      case e_INTERNAL_ARRAY:
        visitor(theInternalArray());
        break;
      case e_INTERNAL_STRING_REFERENCE:
        visitor(theStringReference());
        break;
      case e_INTERNAL_ARRAY_REFERENCE:
        visitor(theArrayReference());
        break;
      case e_INTERNAL_MAP:
        BSLS_ANNOTATION_FALLTHROUGH;
      case e_INTERNAL_OWNED_MAP:
        visitor(theMap());
        break;
      case e_INTERNAL_ERROR:
        BSLS_ANNOTATION_FALLTHROUGH;
      case e_INTERNAL_ERROR_ALLOC:
        visitor(theError());
        break;
      case e_INTERNAL_DOUBLE:
        visitor(d_as.d_double);
        break;
      case e_INTERNAL_BINARY:
        BSLS_ANNOTATION_FALLTHROUGH;
      case e_INTERNAL_BINARY_ALLOC:
        visitor(theBinary());
        break;
      case e_INTERNAL_DECIMAL64: {
        // Code below is identical to 'visitor(theDecimal64())' but postpones
        // the name lookup for 'theDecimal64()' to the second (instantiation)
        // phase, so that we can use only forward declaration of
        // 'bdldfp::Decimal64' in this header.  Without this workaround clang
        // won't compile calls to 'apply' or 'hashAppend' even if
        // '<bdldfp_decimal.h>' is included after this header.  See the test
        // driver for more information.
        typedef bdldfp::Decimal64 (Datum::*Dec64MemFunPtr)() const;
        const typename Datum_MakeDependent<
                        Dec64MemFunPtr,
                        t_VISITOR>::type dec64MemFunPtr = &Datum::theDecimal64;
        visitor((this->*dec64MemFunPtr)());
      } break;
      case e_INTERNAL_INT_MAP:
          visitor(theIntMap());
          break;
      case e_INTERNAL_UNINITIALIZED:
        BSLS_ASSERT(0 == "Uninitialized Datum");
        break;
      default:
        BSLS_ASSERT_SAFE(0 == "Unknown type (memory corruption?)");
    }
#endif // end - 64 bit
}

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
inline
void Datum::createUninitializedMapOwningKeys(
                                    DatumMutableMapOwningKeysRef *result,
                                    SizeType                      capacity,
                                    SizeType                      keysCapacity,
                                    const AllocatorType&          allocator)
{
    createUninitializedMap(result, capacity, keysCapacity, allocator);
}

inline
Datum Datum::adoptMapOwningKeys(const DatumMutableMapOwningKeysRef& mapping)
{
    return adoptMap(mapping);
}

inline
void Datum::disposeUninitializedMapOwningKeys(
                                 const DatumMutableMapOwningKeysRef& mapping,
                                 const AllocatorType&                allocator)
{
    return disposeUninitializedMap(mapping, allocator);
}
#endif  // end - do not omit internal deprecated

                         // -------------------
                         // class DatumArrayRef
                         // -------------------

// CREATORS
inline
DatumArrayRef::DatumArrayRef()
: d_data_p(0)
, d_length(0)
{
}

inline
DatumArrayRef::DatumArrayRef(const Datum *data,
                             SizeType     length)
: d_data_p(data)
, d_length(length)
{
    BSLS_ASSERT(data || 0 == length);
}

// ACCESSORS
inline
DatumArrayRef::const_reference DatumArrayRef::operator[](SizeType index) const
{
    BSLS_ASSERT_SAFE(index < d_length);
    return d_data_p[index];
}

inline
DatumArrayRef::pointer
DatumArrayRef::data() const BSLS_KEYWORD_NOEXCEPT
{
    return d_data_p;
}

inline
DatumArrayRef::size_type
DatumArrayRef::length() const
{
    return d_length;
}

inline
DatumArrayRef::size_type
DatumArrayRef::size() const BSLS_KEYWORD_NOEXCEPT
{
    return d_length;
}

inline
DatumArrayRef::const_iterator
DatumArrayRef::begin() const BSLS_KEYWORD_NOEXCEPT
{
    return d_data_p;
}

inline
DatumArrayRef::const_iterator
DatumArrayRef::cbegin() const BSLS_KEYWORD_NOEXCEPT
{
    return d_data_p;
}

inline
DatumArrayRef::const_reverse_iterator
DatumArrayRef::rbegin() const BSLS_KEYWORD_NOEXCEPT
{
    return const_reverse_iterator(end());
}

inline
DatumArrayRef::const_reverse_iterator
DatumArrayRef::crbegin() const BSLS_KEYWORD_NOEXCEPT
{
    return const_reverse_iterator(cend());
}

inline
DatumArrayRef::const_iterator
DatumArrayRef::end() const BSLS_KEYWORD_NOEXCEPT
{
    return d_data_p + d_length;
}

inline
DatumArrayRef::const_iterator
DatumArrayRef::cend() const BSLS_KEYWORD_NOEXCEPT
{
    return end();
}

inline
DatumArrayRef::const_reverse_iterator
DatumArrayRef::rend() const BSLS_KEYWORD_NOEXCEPT
{
    return const_reverse_iterator(begin());
}

inline
DatumArrayRef::const_reverse_iterator
DatumArrayRef::crend() const BSLS_KEYWORD_NOEXCEPT
{
    return const_reverse_iterator(cbegin());
}


inline
DatumArrayRef::const_reference
DatumArrayRef::front() const
{
    BSLS_ASSERT(!empty());
    return *begin();
}

inline
DatumArrayRef::const_reference
DatumArrayRef::back()  const
{
    BSLS_ASSERT(!empty());
    return *(end() - 1);
}

inline
bool DatumArrayRef::empty() const BSLS_KEYWORD_NOEXCEPT
{
    return 0 == d_length;
}

                          // ----------------------
                          // class DatumIntMapEntry
                          // ----------------------
// CREATORS
inline
DatumIntMapEntry::DatumIntMapEntry()
{
}

inline
DatumIntMapEntry::DatumIntMapEntry(int          key,
                                   const Datum& value)
: d_key(key)
, d_value(value)
{
}

// MANIPULATORS
inline
void DatumIntMapEntry::setKey(int key)
{
    d_key = key;
}

inline
void DatumIntMapEntry::setValue(const Datum& value)
{
    d_value = value;
}

// ACCESSORS
inline
int DatumIntMapEntry::key() const
{
    return d_key;
}

inline
const Datum& DatumIntMapEntry::value() const
{
    return d_value;
}

                        // --------------------
                        // class DatumIntMapRef
                        // --------------------
// CREATORS
inline
DatumIntMapRef::DatumIntMapRef(const DatumIntMapEntry *data,
                               SizeType                size,
                               bool                    sorted)
: d_data_p(data)
, d_size(size)
, d_sorted(sorted)
{
    BSLS_ASSERT((size && data) || !size);
}

// ACCESSORS
inline
const DatumIntMapEntry& DatumIntMapRef::operator[](SizeType index) const
{
    BSLS_ASSERT_SAFE(index < d_size);
    return d_data_p[index];
}

inline
DatumIntMapRef::pointer
DatumIntMapRef::data() const BSLS_KEYWORD_NOEXCEPT
{
    return d_data_p;
}

inline
DatumIntMapRef::size_type DatumIntMapRef::size() const
BSLS_KEYWORD_NOEXCEPT
{
    return d_size;
}

inline
DatumIntMapRef::const_iterator
DatumIntMapRef::begin() const BSLS_KEYWORD_NOEXCEPT
{
    return d_data_p;
}

inline
DatumIntMapRef::const_iterator
DatumIntMapRef::cbegin() const BSLS_KEYWORD_NOEXCEPT
{
    return d_data_p;
}

inline
DatumIntMapRef::const_reverse_iterator
DatumIntMapRef::rbegin() const BSLS_KEYWORD_NOEXCEPT
{
    return const_reverse_iterator(end());
}

inline
DatumIntMapRef::const_reverse_iterator
DatumIntMapRef::crbegin() const BSLS_KEYWORD_NOEXCEPT
{
    return const_reverse_iterator(cend());
}

inline
DatumIntMapRef::const_iterator
DatumIntMapRef::end() const BSLS_KEYWORD_NOEXCEPT
{
    return d_data_p + d_size;
}

inline
DatumIntMapRef::const_iterator
DatumIntMapRef::cend() const BSLS_KEYWORD_NOEXCEPT
{
    return end();
}

inline
DatumIntMapRef::const_reverse_iterator
DatumIntMapRef::rend() const BSLS_KEYWORD_NOEXCEPT
{
    return const_reverse_iterator(begin());
}

inline
DatumIntMapRef::const_reverse_iterator
DatumIntMapRef::crend() const BSLS_KEYWORD_NOEXCEPT
{
    return const_reverse_iterator(cbegin());
}


inline
DatumIntMapRef::const_reference
DatumIntMapRef::front() const
{
    BSLS_ASSERT(!empty());
    return *begin();
}

inline
DatumIntMapRef::const_reference
DatumIntMapRef::back()  const
{
    BSLS_ASSERT(!empty());
    return *(end() - 1);
}

inline
bool DatumIntMapRef::empty() const BSLS_KEYWORD_NOEXCEPT
{
    return 0 == d_size;
}

inline
bool DatumIntMapRef::isSorted() const
{
    return d_sorted;
}

                            // -------------------
                            // class DatumMapEntry
                            // -------------------
// CREATORS
inline
DatumMapEntry::DatumMapEntry()
{
}

inline
DatumMapEntry::DatumMapEntry(const bslstl::StringRef& key,
                             const Datum&             value)
: d_key_p(key)
, d_value(value)
{
}

// MANIPULATORS
inline
void DatumMapEntry::setKey(const bslstl::StringRef& key)
{
    d_key_p = key;
}

inline
void DatumMapEntry::setValue(const Datum& value)
{
    d_value = value;
}

// ACCESSORS
inline
const bslstl::StringRef& DatumMapEntry::key() const
{
    return d_key_p;
}

inline
const Datum& DatumMapEntry::value() const
{
    return d_value;
}

                          // -----------------
                          // class DatumMapRef
                          // -----------------
// CREATORS
inline
DatumMapRef::DatumMapRef(const DatumMapEntry *data,
                         SizeType             size,
                         bool                 sorted,
                         bool                 ownsKeys)
: d_data_p(data)
, d_size(size)
, d_sorted(sorted)
, d_ownsKeys(ownsKeys)
{
    BSLS_ASSERT((size && data) || !size);
    if (0 == size) {
        d_ownsKeys = false;
    }
}

// ACCESSORS
inline
DatumMapRef::const_reference DatumMapRef::operator[](SizeType index) const
{
    BSLS_ASSERT_SAFE(index < d_size);
    return d_data_p[index];
}

inline
DatumMapRef::pointer
DatumMapRef::data() const BSLS_KEYWORD_NOEXCEPT
{
    return d_data_p;
}

inline
DatumMapRef::size_type DatumMapRef::size() const
BSLS_KEYWORD_NOEXCEPT
{
    return d_size;
}

inline
DatumMapRef::const_iterator
DatumMapRef::begin() const BSLS_KEYWORD_NOEXCEPT
{
    return d_data_p;
}

inline
DatumMapRef::const_iterator
DatumMapRef::cbegin() const BSLS_KEYWORD_NOEXCEPT
{
    return d_data_p;
}

inline
DatumMapRef::const_reverse_iterator
DatumMapRef::rbegin() const BSLS_KEYWORD_NOEXCEPT
{
    return const_reverse_iterator(end());
}

inline
DatumMapRef::const_reverse_iterator
DatumMapRef::crbegin() const BSLS_KEYWORD_NOEXCEPT
{
    return const_reverse_iterator(cend());
}

inline
DatumMapRef::const_iterator
DatumMapRef::end() const BSLS_KEYWORD_NOEXCEPT
{
    return d_data_p + d_size;
}

inline
DatumMapRef::const_iterator
DatumMapRef::cend() const BSLS_KEYWORD_NOEXCEPT
{
    return end();
}

inline
DatumMapRef::const_reverse_iterator
DatumMapRef::rend() const BSLS_KEYWORD_NOEXCEPT
{
    return const_reverse_iterator(begin());
}

inline
DatumMapRef::const_reverse_iterator
DatumMapRef::crend() const BSLS_KEYWORD_NOEXCEPT
{
    return const_reverse_iterator(cbegin());
}


inline
DatumMapRef::const_reference
DatumMapRef::front() const
{
    BSLS_ASSERT(!empty());
    return *begin();
}

inline
DatumMapRef::const_reference
DatumMapRef::back()  const
{
    BSLS_ASSERT(!empty());
    return *(end() - 1);
}

inline
bool DatumMapRef::empty() const BSLS_KEYWORD_NOEXCEPT
{
    return 0 == d_size;
}

inline
bool DatumMapRef::isSorted() const
{
    return d_sorted;
}

inline
bool DatumMapRef::ownsKeys() const
{
    return d_ownsKeys;
}

                         // --------------------------
                         // class DatumMutableArrayRef
                         // --------------------------

// CREATORS
inline
DatumMutableArrayRef::DatumMutableArrayRef()
: d_data_p(0)
, d_length_p(0)
{
}

inline
DatumMutableArrayRef::DatumMutableArrayRef(Datum    *data,
                                           SizeType *length,
                                           SizeType  capacity)
: d_data_p(data)
, d_length_p(length)
, d_capacity(capacity)
{
}

// ACCESSORS
inline
void *DatumMutableArrayRef::allocatedPtr() const
{
    return d_length_p;
}

inline
Datum *DatumMutableArrayRef::data() const
{
    return d_data_p;
}

inline
DatumMutableArrayRef::SizeType *DatumMutableArrayRef::length() const
{
    return d_length_p;
}

inline
DatumMutableArrayRef::SizeType DatumMutableArrayRef::capacity() const
{
    return d_capacity;
}

                       // ---------------------------
                       // class DatumMutableIntMapRef
                       // ---------------------------

// CREATORS
inline
DatumMutableIntMapRef::DatumMutableIntMapRef()
: d_data_p(0)
, d_size_p(0)
, d_sorted_p(0)
{
}

inline
DatumMutableIntMapRef::DatumMutableIntMapRef(DatumIntMapEntry *data,
                                             SizeType         *size,
                                             bool             *sorted)
: d_data_p(data)
, d_size_p(size)
, d_sorted_p(sorted)
{
}

// ACCESSORS
inline
void *DatumMutableIntMapRef::allocatedPtr() const
{
    return d_size_p;
}

inline
DatumIntMapEntry *DatumMutableIntMapRef::data() const
{
    return d_data_p;
}

inline
DatumMutableIntMapRef::SizeType *DatumMutableIntMapRef::size() const
{
    return d_size_p;
}

inline
bool *DatumMutableIntMapRef::sorted() const
{
    return d_sorted_p;
}

                          // ------------------------
                          // class DatumMutableMapRef
                          // ------------------------

// CREATORS
inline
DatumMutableMapRef::DatumMutableMapRef()
: d_data_p(0)
, d_size_p(0)
, d_sorted_p(0)
{
}

inline
DatumMutableMapRef::DatumMutableMapRef(DatumMapEntry *data,
                                       SizeType      *size,
                                       bool          *sorted)
: d_data_p(data)
, d_size_p(size)
, d_sorted_p(sorted)
{
}

// ACCESSORS
inline
void *DatumMutableMapRef::allocatedPtr() const
{
    return d_size_p;
}

inline
DatumMapEntry *DatumMutableMapRef::data() const
{
    return d_data_p;
}

inline
DatumMutableMapRef::SizeType *DatumMutableMapRef::size() const
{
    return d_size_p;
}

inline
bool *DatumMutableMapRef::sorted() const
{
    return d_sorted_p;
}

                     // ----------------------------------
                     // class DatumMutableMapOwningKeysRef
                     // ----------------------------------

// CREATORS
inline
DatumMutableMapOwningKeysRef::DatumMutableMapOwningKeysRef()
: d_data_p(0)
, d_size_p(0)
, d_keys_p(0)
, d_sorted_p(0)
{
}

inline
DatumMutableMapOwningKeysRef::DatumMutableMapOwningKeysRef(
                                                  DatumMapEntry *data,
                                                  SizeType      *size,
                                                  SizeType       allocatedSize,
                                                  char          *keys,
                                                  bool          *sorted)
: d_data_p(data)
, d_size_p(size)
, d_allocatedSize(allocatedSize)
, d_keys_p(keys)
, d_sorted_p(sorted)
{
}

// ACCESSORS
inline
void *DatumMutableMapOwningKeysRef::allocatedPtr() const
{
    return d_size_p;
}

inline
DatumMutableMapOwningKeysRef::SizeType
DatumMutableMapOwningKeysRef::allocatedSize() const
{
    return d_allocatedSize;
}

inline
DatumMapEntry *DatumMutableMapOwningKeysRef::data() const
{
    return d_data_p;
}

inline
char *DatumMutableMapOwningKeysRef::keys() const
{
    return d_keys_p;
}

inline
DatumMutableMapOwningKeysRef::SizeType *
DatumMutableMapOwningKeysRef::size() const
{
    return d_size_p;
}

inline
bool *DatumMutableMapOwningKeysRef::sorted() const
{
    return d_sorted_p;
}

}  // close package namespace

// FREE OPERATORS
inline
bool bdld::operator!=(const Datum& lhs, const Datum& rhs)
{
    return !(lhs == rhs);
}

inline
bool bdld::operator!=(const DatumArrayRef& lhs, const DatumArrayRef& rhs)
{
    return !(lhs == rhs);
}

inline
bool bdld::operator==(const DatumIntMapEntry& lhs, const DatumIntMapEntry& rhs)
{
    return (lhs.key() == rhs.key()) && (lhs.value() == rhs.value());
}

inline
bool bdld::operator!=(const DatumIntMapEntry& lhs, const DatumIntMapEntry& rhs)
{
    return !(lhs == rhs);
}

inline
bool bdld::operator!=(const DatumIntMapRef& lhs, const DatumIntMapRef& rhs)
{
    return !(lhs == rhs);
}

inline
bool bdld::operator==(const DatumMapEntry& lhs, const DatumMapEntry& rhs)
{
    return (lhs.key() == rhs.key()) && (lhs.value() == rhs.value());
}

inline
bool bdld::operator!=(const DatumMapEntry& lhs, const DatumMapEntry& rhs)
{
    return !(lhs == rhs);
}

inline
bool bdld::operator!=(const DatumMapRef& lhs, const DatumMapRef& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& bdld::operator<<(bsl::ostream& stream, const Datum& rhs)
{
    return rhs.print(stream, 0, -1);
}

template <class t_HASH_ALGORITHM>
void bdld::hashAppend(t_HASH_ALGORITHM& hashAlg, const bdld::Datum& input)
{
    using bslh::hashAppend;
    hashAppend(hashAlg, input.type());

    switch (input.type()) {
        case bdld::Datum::e_NIL: {
          // Do nothing.  This is sufficient because 'type' has already been
          // hashed (differentiating the nil value).
        } break;
        case bdld::Datum::e_INTEGER: {
            hashAppend(hashAlg, input.theInteger());
        } break;
        case bdld::Datum::e_DOUBLE: {
            hashAppend(hashAlg, input.theDouble());
        } break;
        case bdld::Datum::e_STRING: {
            hashAppend(hashAlg, input.theString());
        } break;
        case bdld::Datum::e_BOOLEAN: {
            hashAppend(hashAlg, input.theBoolean());
        } break;
        case bdld::Datum::e_ERROR: {
            hashAppend(hashAlg, input.theError().code());
            hashAppend(hashAlg, input.theError().message());
        } break;
        case bdld::Datum::e_DATE: {
            hashAppend(hashAlg, input.theDate());
        } break;
        case bdld::Datum::e_TIME: {
            hashAppend(hashAlg, input.theTime());
        } break;
        case bdld::Datum::e_DATETIME: {
            hashAppend(hashAlg, input.theDatetime());
        } break;
        case bdld::Datum::e_DATETIME_INTERVAL: {
            hashAppend(hashAlg, input.theDatetimeInterval());
        } break;
        case bdld::Datum::e_INTEGER64: {
            hashAppend(hashAlg, input.theInteger64());
        } break;
        case bdld::Datum::e_USERDEFINED: {
            hashAppend(hashAlg, input.theUdt().type());
            hashAppend(hashAlg, input.theUdt().data());
        } break;
        case bdld::Datum::e_ARRAY: {
            hashAppend(hashAlg, input.theArray().length());
            for (bsl::size_t i = 0; i < input.theArray().length(); ++i) {
                bdld::hashAppend(hashAlg, input.theArray()[i]);
            }
        } break;
        case bdld::Datum::e_MAP: {
            hashAppend(hashAlg, input.theMap().size());
            for (bsl::size_t i = 0; i < input.theMap().size(); ++i) {
                hashAppend(hashAlg, input.theMap()[i].key());
                bdld::hashAppend(hashAlg, input.theMap()[i].value());
            }
        } break;
        case bdld::Datum::e_BINARY: {
            hashAppend(hashAlg, input.theBinary().size());
            if (input.theBinary().size() > 0) {
                hashAlg(input.theBinary().data(), input.theBinary().size());
            }
        } break;
        case bdld::Datum::e_DECIMAL64: {
          // Code below is identical to
          // 'hashAppend(hashAlg, input.theDecimal64())', but postpones the
          // name lookup for 'theDecimal64()' to the second (instantiation)
          // phase so that we can use only forward declaration of
          // 'bdldfp::Decimal64' in this header.  Without this workaround clang
          // won't compile calls to 'apply' or 'hashAppend' even if
          // '<bdldfp_decimal.h>' is included after this header.  See the test
          // driver for more information.
          typedef bdldfp::Decimal64 (Datum::*Dec64MemFunPtr)() const;
          const typename Datum_MakeDependent<Dec64MemFunPtr,
                                             t_HASH_ALGORITHM>::type
                                         dec64MemFunPtr = &Datum::theDecimal64;
          hashAppend(hashAlg, (input.*dec64MemFunPtr)());
        } break;
        case bdld::Datum::e_INT_MAP: {
            hashAppend(hashAlg, input.theIntMap().size());
            for (bsl::size_t i = 0; i < input.theIntMap().size(); ++i) {
                hashAppend(hashAlg, input.theIntMap()[i].key());
                bdld::hashAppend(hashAlg, input.theIntMap()[i].value());
            }
        } break;
        default: {
            BSLS_ASSERT(0 == "Unknown type (memory corruption?)");
        }
    }
}

inline
bsl::ostream& bdld::operator<<(bsl::ostream& stream, const DatumArrayRef& rhs)
{
    return rhs.print(stream, 0 , -1);
}

inline
bsl::ostream& bdld::operator<<(bsl::ostream& stream, const DatumMapEntry& rhs)
{
    return rhs.print(stream, 0 , -1);
}

inline
bsl::ostream& bdld::operator<<(bsl::ostream&           stream,
                               const DatumIntMapEntry& rhs)
{
    return rhs.print(stream, 0 , -1);
}

inline
bsl::ostream& bdld::operator<<(bsl::ostream& stream, const DatumIntMapRef& rhs)
{
    return rhs.print(stream, 0 , -1);
}

inline
bsl::ostream& bdld::operator<<(bsl::ostream& stream, const DatumMapRef& rhs)
{
    return rhs.print(stream, 0 , -1);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
