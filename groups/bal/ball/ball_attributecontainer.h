// ball_attributecontainer.h                                          -*-C++-*-
#ifndef INCLUDED_BALL_ATTRIBUTECONTAINER
#define INCLUDED_BALL_ATTRIBUTECONTAINER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a protocol for containers holding logging attributes.
//
//@CLASSES:
//  ball::AttributeContainer: a protocol for a collection of attributes
//
//@SEE_ALSO: ball_attribute, ball_defaultattributecontainer
//
//@DESCRIPTION: This component defines a protocol class,
// 'ball::AttributeContainer', for containers of 'ball::Attribute' values.  The
// 'ball::AttributeContainer' protocol primarily provides a 'hasValue()'
// method, allowing clients to determine if a given attribute value is held by
// the container.
//
// This component participates in the implementation of "Rule-Based Logging".
// For more information on how to use that feature, please see the package
// level documentation and usage examples for "Rule-Based Logging".
//
///Usage
///-----
// In the following examples we examine two derived implementations of the
// 'ball::AttributeContainer' protocol.  The first implementation is
// potentially more efficient, holding a specific group of attributes relevant
// to a particular application.  The second implementation is more general,
// and can hold any valid 'ball::Attribute' value.  In the final example we
// demonstrate how to call the methods of the 'ball::AttributeContainer'
// protocol.
//
///Example 1: An Implementation of 'ball::AttributeContainer'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In the following example we develop a 'ball::AttributeContainer'
// implementation specifically intended for a service offline that will perform
// rule-based logging, governed by the service client's Bloomberg "uuid",
// "luw", and "firm number".  We define a class 'ServiceAttributes' that
// contains three integer 'ball::Attribute' values having the names "uuid",
// "luw", and "firmNumber".
//
// Note that this implementation requires no memory allocation, so it will be
// more efficient than a more general set-based implementation if the container
// is frequently created, destroyed, or modified.  We will develop a
// 'ball::AttributeContainer' implementation that can hold any
// 'ball::Attribute' value in example 2 (and one is provided by the 'ball'
// package in the 'ball_defaultattributecontainer' component).
//..
//    // serviceattributes.h
//
//  class ServiceAttributes : public ball::AttributeContainer {
//      // Provide a concrete implementation of the 'ball::AttributeContainer'
//      // that holds the 'uuid', 'luw', and 'firmNumber' associated with a
//      // request to the example service.  This concrete container
//      // exposes those properties in attributes named "uuid", "luw", and
//      // "firmNumber" respectively.
//
//..
// Note that we use the type 'ball::Attribute' for our data members for
// simplicity.  It would be a little more efficient to use 'int' data members,
// but the implementation of 'hasValue()' would be less readable.
//..
//      // DATA
//      ball::Attribute d_uuid;        // Bloomberg user id
//      ball::Attribute d_luw;         // Bloomberg luw
//      ball::Attribute d_firmNumber;  // Bloomberg firm number
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
//      virtual bool hasValue(const ball::Attribute& value) const;
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
//
//      virtual void visitAttributes(
//           const bsl::function<void(const ball::Attribute&)>& visitor) const;
//          // Invoke the specified 'visitor' function for all attributes in
//          // this container.
//  };
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
//  bool ServiceAttributes::hasValue(const ball::Attribute& value) const
//  {
//      return d_uuid == value || d_luw == value || d_firmNumber == value;
//  }
//
//  bsl::ostream& ServiceAttributes::print(bsl::ostream& stream,
//                                         int           level,
//                                         int           spacesPerLevel) const
//  {
//      bslim::Printer printer(&stream, level, spacesPerLevel);
//      printer.start();
//      printer.printAttribute("uuid", d_uuid);
//      printer.printAttribute("luw", d_luw);
//      printer.printAttribute("firmNumber", d_firmNumber);
//      printer.end();
//      return stream;
//  }
//
//  void ServiceAttributes::visitAttributes(
//            const bsl::function<void(const ball::Attribute&)> &visitor) const
//  {
//      visitor(d_uuid);
//      visitor(d_luw);
//      visitor(d_firmNumber);
//  }
//..
//
///Example 2: A Generic Implementation of 'ball::AttributeContainer'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this second example we define a 'ball::AttributeContainer' that can
// contain any valid 'ball::Attribute' value (a "generic"
// 'ball::AttributeContainer').  In practice, an implementation that can
// contain any attribute values may be less efficient than one specifically
// created for a particular group of attributes needed by an application (as
// shown in {Example 1}).
//
// Note that the 'ball' package provides a similar 'ball::AttributeContainer'
// implementation in the 'ball_defaultattributecontainer' component.
//..
//   // attributeset.h
//
//  class AttributeSet : public ball::AttributeContainer {
//      // A simple set-based implementation of the 'ball::AttributeContainer'
//      // protocol, used for testing.
//
//..
// To define an STL set (or hash set) for 'ball::Attribute' values, we must
// define a comparison (or hash) operation for attribute values.  Here we
// define a comparison functor that compares attributes by name, then by
// value-type, and finally by value.
//..
//      struct AttributeComparator {
//          bool operator()(const ball::Attribute& lhs,
//                          const ball::Attribute& rhs) const
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
//                case 1: // int
//                  return lhs.value().the<int>() < rhs.value().the<int>();
//                case 2: // long
//                  return lhs.value().the<long>() < rhs.value().the<long>();
//                case 3: // long long
//                  return lhs.value().the<long long>()
//                       < rhs.value().the<long long>();
//                case 4: // unsigned int
//                  return lhs.value().the<unsigned int>()
//                       < rhs.value().the<unsigned int>();
//                case 5: // unsigned long
//                  return lhs.value().the<unsigned long>()
//                       < rhs.value().the<unsigned long>();
//                case 6: // unsigned long long
//                  return lhs.value().the<unsigned long long>()
//                       < rhs.value().the<unsigned long long>();
//                case 7: // string
//                  return lhs.value().the<bsl::string>() <
//                         rhs.value().the<bsl::string>();
//                case 8: // const void *
//                  return lhs.value().the<const void *>() <
//                         rhs.value().the<const void *>();
//              }
//              BSLS_ASSERT(false);
//              return false;
//          }
//      };
//
//      // DATA
//      bsl::set<ball::Attribute, AttributeComparator> d_set;
//
//    public:
//      // CREATORS
//      AttributeSet(bslma::Allocator *basicAllocator = 0);
//          // Create an attribute set.
//
//      virtual ~AttributeSet();
//          // Destroy this attribute set.
//
//      // MANIPULATORS
//      void insert(const ball::Attribute& value);
//          // Add the specified value to this attribute set.
//
//      bool remove(const ball::Attribute& value);
//          // Remove the specified value from this attribute set.  Return
//          // 'true' if the attribute was found, and 'false' if 'value' was
//          // not a member of this set.
//
//      // ACCESSORS
//      virtual bool hasValue(const ball::Attribute& value) const;
//          // Return 'true' if the attribute having specified 'value' exists
//          // in this object, and 'false' otherwise.
//
//      virtual bsl::ostream& print(bsl::ostream& stream,
//                                  int           level = 0,
//                                  int           spacesPerLevel = 4) const;
//          // Format this object to the specified output 'stream' at the
//          // (absolute value of) the optionally specified indentation 'level'
//          // and return a reference to 'stream'.
//
//      virtual void visitAttributes(
//           const bsl::function<void(const ball::Attribute&)>& visitor) const;
//          // Invoke the specified 'visitor' function for all attributes in
//          // this container.
//  };
//..
// The 'AttributeSet' methods are simple wrappers around 'bsl::set' methods:
//..
//  inline
//  AttributeSet::AttributeSet(bslma::Allocator *basicAllocator)
//  : d_set(AttributeComparator(), basicAllocator)
//  {
//  }
//
//  // MANIPULATORS
//  inline
//  void AttributeSet::insert(const ball::Attribute& value)
//  {
//      d_set.insert(value);
//  }
//
//  inline
//  bool AttributeSet::remove(const ball::Attribute& value)
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
//  bool AttributeSet::hasValue(const ball::Attribute& value) const
//  {
//      return d_set.find(value) != d_set.end();
//  }
//
//  bsl::ostream& AttributeSet::print(bsl::ostream& stream,
//                                    int           level,
//                                    int           spacesPerLevel) const
//  {
//      bslim::Printer printer(&stream, level, spacesPerLevel);
//      printer.start();
//
//      bsl::set<ball::Attribute, AttributeComparator>::const_iterator it
//                                                             = d_set.begin();
//      for (; it != d_set.end(); ++it) {
//          printer.printValue(*it);
//      }
//      printer.end();
//
//      return stream;
//  }
//
//  void AttributeSet::visitAttributes(
//            const bsl::function<void(const ball::Attribute&)> &visitor) const
//  {
//      bsl::set<ball::Attribute, AttributeComparator>::const_iterator it
//                                                             = d_set.begin();
//      for (; it != d_set.end(); ++it) {
//          visitor(*it);
//      }
//  }
//..
//
///Example 3. Using a 'ball::AttributeContainer'
///- - - - - - - - - - - - - - - - - - - - - - -
// In this final example, we demonstrate how to call the methods of the
// 'ball::AttributeContainer' protocol.
//
// First we create an object of a concrete type that implements the
// 'ball::AttributeContainer' protocol (e.g., 'ServiceAttributes' defined in
// {Example 1}).  Then we obtain a reference to this object.
//..
//  ServiceAttributes serviceAttributes(3938908, 1, 9001);
//  const ball::AttributeContainer& attributes = serviceAttributes;
//..
// We use 'hasValue()' to examine the values in the container:
//..
//  assert(true == attributes.hasValue(ball::Attribute("uuid", 3938908)));
//  assert(true == attributes.hasValue(ball::Attribute("luw",  1)));
//  assert(true == attributes.hasValue(ball::Attribute("firmNumber", 9001)));
//
//  assert(false == attributes.hasValue(ball::Attribute("uuid", 1391015)));
//  assert(false == attributes.hasValue(ball::Attribute("luw",  2)));
//  assert(false == attributes.hasValue(ball::Attribute("bad name", 3938908)));
//..
// Finally we can print the attribute values in the container:
//..
//  bsl::cout << attributes << bsl::endl;
//..
// The resulting output should look like:
//..
// [ [ uuid = 3938908 ] [ luw = 1 ] [ firmNumber = 9001 ] ]
//..

#include <balscm_version.h>

#include <bsl_functional.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace ball {

class Attribute;

                    // ========================
                    // class AttributeContainer
                    // ========================

class AttributeContainer {
    // This class defines a protocol for a container of attribute values.  The
    // attribute container provides a 'hasValue()' method to determine whether
    // an attribute value is in the container.

  public:
    // CREATORS
    virtual ~AttributeContainer();
        // Destroy this object.

    // ACCESSORS
    virtual bool hasValue(const Attribute& value) const = 0;
        // Return 'true' if a attribute having the specified 'value' exists in
        // this object, and 'false' otherwise.

    virtual bsl::ostream& print(bsl::ostream& stream,
                                int           level = 0,
                                int           spacesPerLevel = 4) const = 0;
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level' and return a
        // reference to 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.

    virtual void visitAttributes(
         const bsl::function<void(const ball::Attribute&)>& visitor) const = 0;
        // Invoke the specified 'visitor' function for all attributes in this
        // container.
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&             stream,
                         const AttributeContainer& container);
    // Write the value of the specified 'container' to the specified output
    // 'stream' in a single-line format, and return a reference to the
    // modifiable stream.

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                    // ------------------------
                    // class AttributeContainer
                    // ------------------------

}  // close package namespace

// FREE OPERATORS
inline
bsl::ostream& ball::operator<<(bsl::ostream&             stream,
                               const AttributeContainer& container)
{
    return container.print(stream, 0, -1);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
