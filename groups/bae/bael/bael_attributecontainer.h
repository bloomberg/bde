// bael_attributecontainer.h                  -*-C++-*-
#ifndef INCLUDED_BAEL_ATTRIBUTECONTAINER
#define INCLUDED_BAEL_ATTRIBUTECONTAINER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a protocol for containers holding logging attributes.
//
//@CLASSES:
//     bael_AttributeContainer: a protocol for a collection of attributes
//
//@SEE_ALSO: bael_attribute, bael_attributeset
//
//@AUTHOR: Henry Verschell (hverschell)
//
//@DESCRIPTION: This component defines a protocol class,
//'bael_AttributeContainer', for containers of 'bael_Attribute' values.  The
// 'bael_AttributeContainer' protocol primarily provides a 'hasValue()' method,
// allowing clients to determine if a given attribute value is held by the
// container.
//
///Usage
///-----
// In the following examples we examine two derived implementations of the
// 'bael_AttributeContainer' protocol.  The first implementation is
// potentially more efficient, holding a specific group of attributes relevant
// to a particular application.  The second implementation is more general,
// and can hold any valid 'bael_Attribute' value.  In the final example we
// demonstrate how to call the methods of the 'bael_AttributeContainer'
// protocol.
//
///Example 1: An Implementation of 'bael_AttributeContainer'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In the following example we develop a 'bael_AttributeContainer'
// implementation specifically intended for a service offline that will perform
// rule-based logging, governed by the service client's bloomberg "uuid",
// "luw", and "firm number".  We define a class 'ServiceAttributes' that
// contains three integer 'bael_Attribute' values having the names "uuid",
// "luw", and "firmNumber".
//
// Note that this implementation requires no memory allocation, so it will be
// more efficient than a more general set-based implementation if the
// container is frequently created, destroyed, or modified.  We will develop a
// 'bael_AttributeContainer' implementation that can hold any 'bael_Attribute'
// value in example 2 (and one is provided by the 'bael' package in the
// 'bael_defaultattributecontainer' component).
//..
//    // serviceattributes.h
//
//  class ServiceAttributes : public bael_AttributeContainer {
//      // Provide a concrete implementation of the 'bael_AttributeContainer'
//      // that holds the 'uuid', 'luw', and 'firmNumber' associated with a
//      // request to the example service.  This concrete container
//      // exposes those properties in attributes named "uuid", "luw", and
//      // "firmNumber" respectively.
//
//..
// Note that we use the type 'bael_Attribute' for our data members for
// simplicity.  It would be a little more efficient to use 'int' data members,
// but the implementation of 'hasValue()' would be less readable.
//..
//      // DATA
//      bael_Attribute d_uuid;        // bloomberg user id
//      bael_Attribute d_luw;         // bloomberg luw
//      bael_Attribute d_firmNumber;  // bloomberg firm number
//
//      // ...
//
//    public:
//      // PUBLIC CONSTANTS
//      static const char * const UUID_ATTRIBUTE_NAME;
//      static const char * const LUW_ATTRIBUTE_NAME;
//      static const char * const FIRMNUMBER_ATTRIBUTE_NAME;
//          // The names of the attributes exposed by this attribute container.
//
//      // CREATORS
//      ServiceAttributes(int uuid, int luw, int firmNumber);
//          // Create a service-attributes object with the specified 'uuid',
//          // 'luw', and 'firmNumber'.
//
//      virtual ~ServiceAttributes();
//          // Destroy this service-attributes object.
//
//      // ACCESSORS
//      virtual bool hasValue(const bael_Attribute& value) const;
//          // Return 'true' if the attribute having specified 'value' exists
//          // in this object, and 'false' otherwise.  This implementation will
//          // return 'true' if 'value.name()' equals "uuid", "luw", or "firm"
//          // and 'value.value()' is an 'int' equal to the corresponding
//          // property value supplied at construction.
//
//      virtual bsl::ostream& print(bsl::ostream& stream,
//                                  int           level = 0,
//                                  int           spacesPerLevel = 4) const;
//          // Format this object to the specified output 'stream'.
//  };
//
//
//  // CREATORS
//  inline
//  ServiceAttributes::ServiceAttributes(int uuid, int luw, int firmNumber)
//  : d_uuid(UUID_ATTRIBUTE_NAME, uuid)
//  , d_luw(LUW_ATTRIBUTE_NAME, luw)
//  , d_firmNumber(FIRMNUMBER_ATTRIBUTE_NAME, firmNumber)
//  {
//  }
//
//    // serviceattributes.cpp
//
//  // PUBLIC CONSTANTS
//  const char * const ServiceAttributes::UUID_ATTRIBUTE_NAME       = "uuid";
//  const char * const ServiceAttributes::LUW_ATTRIBUTE_NAME        = "luw";
//  const char * const ServiceAttributes::FIRMNUMBER_ATTRIBUTE_NAME =
//                                                                "firmNumber";
//
//  // CREATORS
//  ServiceAttributes::~ServiceAttributes()
//  {
//  }
//
//  // ACCESSORS
//  bool ServiceAttributes::hasValue(const bael_Attribute& value) const
//  {
//      return d_uuid == value || d_luw == value || d_firmNumber == value;
//  }
//
//  bsl::ostream& ServiceAttributes::print(bsl::ostream& stream,
//                                         int           level,
//                                         int           spacesPerLevel) const
//  {
//      char EL = (spacesPerLevel < 0) ? ' ' : '\n';
//      bdeu_Print::indent(stream, level, spacesPerLevel);
//      stream << "[ "
//             << d_uuid << ' '
//             << d_luw << ' '
//             << d_firmNumber << ' '
//             << ']' << EL;
//      return stream;
//  }
//..
//
///Example 2: A Generic Implementation of 'bael_AttributeContainer'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this second example we define a 'bael_AttributeContainer' that can
// contain any valid 'bael_Attribute' value (a "generic"
// 'bael_AttributeContainer').  In practice, an implementation that can
// contain any attribute values may be less efficient than one specifically
// created for a particular group of attributes needed by an application (as
// shown in example 1).
//
// Note that the 'bael' package provides a similar 'bael_AttributeContainer'
// implementation in the 'bael_defaultattributecontainer' component.
//..
//   // attributeset.h
//
//  class AttributeSet : public bael_AttributeContainer {
//      // A simple set-based implementation of the 'bael_AttributeContainer'
//      // protocol, used for testing.
//
//..
// To define an stl-set (or hash set) for 'bael_Attribute' values, we must
// define a comparison (or hash) operation for attribute values.  Here we
// define a comparison functor that compares attributes by name, then by
// value-type, and finally by value.
//..
//      struct AttributeComparator {
//          bool operator()(const bael_Attribute& lhs,
//                          const bael_Attribute& rhs) const
//              // Return 'true' if the specified 'lhs' attribute is ordered
//              // before the specified 'rhs' attribute, and 'false' otherwise.
//          {
//              int cmp = bsl::strcmp(lhs.name(), rhs.name());
//              if (0 != cmp) {
//                  return cmp < 0;
//              }
//              if (lhs.value().typeIndex() != rhs.value().typeIndex()) {
//                  return lhs.value().typeIndex() < rhs.value().typeIndex();
//              }
//              switch (lhs.value().typeIndex()) {
//                case 0: // unset
//                  return true;
//                case 14: // int
//                  return lhs.value().the<int>() < rhs.value().the<int>();
//                case 2: // int64
//                  return lhs.value().the<bsls_PlatformUtil::Int64>() <
//                         rhs.value().the<bsls_PlatformUtil::Int64>();
//               case 3: // string
//                  return lhs.value().the<bsl::string>() <
//                         rhs.value().the<bsl::string>();
//              }
//              BSLS_ASSERT(false);
//              return false;
//          }
//      };
//
//      // DATA
//      bsl::set<bael_Attribute, AttributeComparator> d_set;
//
//    public:
//      // CREATORS
//      AttributeSet(bslma_Allocator *basicAllocator = 0);
//          // Create an attribute set.
//
//      virtual ~AttributeSet();
//          // Destroy this attribute set.
//
//      // MANIPULATORS
//      void insert(const bael_Attribute& value);
//          // Add the specified value to this attribute set.
//
//      bool remove(const bael_Attribute& value);
//          // Remove the specified value from this attribute set.  Return
//          // 'true' if the attribute was found, and 'false' if 'value' was
//          // not a member of this set.
//
//      // ACCESSORS
//      virtual bool hasValue(const bael_Attribute& value) const;
//          // Return 'true' if the attribute having specified 'value' exists
//          // in this object, and 'false' otherwise.
//
//      virtual bsl::ostream& print(bsl::ostream& stream,
//                                  int           level = 0,
//                                  int           spacesPerLevel = 4) const;
//          // Format this object to the specified output 'stream' at the
//          // (absolute value of) the optionally specified indentation 'level'
//          // and return a reference to 'stream'.
//  };
//..
// The 'bael_AttributeContainer' methods are simple wrappers around 'bsl::set'
// methods:
//..
//  inline
//  AttributeSet::AttributeSet(bslma_Allocator *basicAllocator)
//  : d_set(AttributeComparator(), basicAllocator)
//  {
//  }
//
//  // MANIPULATORS
//  inline
//  void AttributeSet::insert(const bael_Attribute& value)
//  {
//      d_set.insert(value);
//  }
//
//  inline
//  bool AttributeSet::remove(const bael_Attribute& value)
//  {
//      return d_set.erase(value) > 0;
//  }
//
//  // attributeset.cpp
//
//  // CREATORS
//  AttributeSet::~AttributeSet()
//  {
//  }
//
//  // ACCESSORS
//  bool AttributeSet::hasValue(const bael_Attribute& value) const
//  {
//      return d_set.find(value) != d_set.end();
//  }
//
//  bsl::ostream& AttributeSet::print(bsl::ostream& stream,
//                                    int           level,
//                                    int           spacesPerLevel) const
//  {
//      char EL = (spacesPerLevel < 0) ? ' ' : '\n';
//      bdeu_Print::indent(stream, level, spacesPerLevel);
//      stream << "[" << EL;
//
//      bsl::set<bael_Attribute>::const_iterator it = d_set.begin();
//      for (; it != d_set.end(); ++it) {
//          it->print(stream, level + 1, spacesPerLevel);
//      }
//      bdeu_Print::indent(stream, level, spacesPerLevel);
//      stream << "]" << EL;
//      return stream;
//  }
//..
//
///Example 3. Using a 'bael_AttributeContainer'
///- - - - - - - - - - - - - - - - - - - - - - -
// In this final example, we demonstrate how to call the methods of the
// 'bael_AttributeContainer' protocol.
//
// First we create an object of a concrete type that implements the
// 'bael_AttributeContainer' protocol (e.g., 'ServiceAttributes' defined in
// example 1).  Then we obtain a reference to this object.
//..
//  ServiceAttributes serviceAttributes(3938908, 1, 9001);
//  const bael_AttributeContainer& attributes = serviceAttributes;
//..
// We use 'hasValue()' to examine the values in the container:
//..
//  assert(true == attributes.hasValue(bael_Attribute("uuid", 3938908)));
//  assert(true == attributes.hasValue(bael_Attribute("luw",  1)));
//  assert(true == attributes.hasValue(bael_Attribute("firmNumber", 9001)));
//
//  assert(false == attributes.hasValue(bael_Attribute("uuid", 1391015)));
//  assert(false == attributes.hasValue(bael_Attribute("luw",  2)));
//  assert(false == attributes.hasValue(bael_Attribute("bad name", 3938908)));
//..
// Finally we can print the attribute values in the container:
//..
//  bsl::cout << attributes << bsl::endl;
//..
// The resulting output should look like:
//..
// [ [ uuid = 3938908 ] [ luw = 1 ] [ firmNumber = 9001 ] ]
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif


namespace BloombergLP {

class bael_Attribute;

                    // =============================
                    // class bael_AttributeContainer
                    // =============================

class bael_AttributeContainer {
    // This class defines a protocol for a container of attribute values.  The
    // attribute container provides a 'hasValue()' method to determine whether
    // an attribute value is in the container.

  public:

    // CREATORS
    virtual ~bael_AttributeContainer();
        // Destroy this object.

    // ACCESSORS
    virtual bool hasValue(const bael_Attribute& value) const = 0;
        // Return 'true' if a attribute having the specified 'value' exists in
        // this object, and 'false' otherwise.

    virtual bsl::ostream& print(bsl::ostream& stream,
                                int           level = 0,
                                int           spacesPerLevel = 4) const = 0;
        // Format this object to the specified output 'stream' at the
        // (absolute value of) the optionally specified indentation 'level'
        // and return a reference to 'stream'.  If 'level' is specified,
        // optionally specify 'spacesPerLevel', the number of spaces per
        // indentation level for this and all of its nested objects.  If
        // 'level' is negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, format the entire output on one line,
        // suppressing all but the initial indentation (as governed by
        // 'level').  If 'stream' is not valid on entry, this operation has no
        // effect.
};

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream&                  stream,
                         const bael_AttributeContainer& container);
    // Write the value of the specified 'container' to the specified
    // output 'stream' in a single-line format, and return a reference to the
    // modifiable stream.

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream&                  output,
                         const bael_AttributeContainer& container)
{
    return container.print(output, 0, -1);
}

} // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
