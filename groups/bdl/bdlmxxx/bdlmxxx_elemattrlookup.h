// bdlmxxx_elemattrlookup.h                                              -*-C++-*-
#ifndef INCLUDED_BDLMXXX_ELEMATTRLOOKUP
#define INCLUDED_BDLMXXX_ELEMATTRLOOKUP

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide lookup for descriptors and attributes of 'bdem' types.
//
//@CLASSES:
//  bdlmxxx::ElemAttrLookup: element type to descriptor lookup
//  bdlmxxx::ElemStreamInAttrLookup: element type to stream-in function lookup
//  bdlmxxx::ElemStreamOutAttrLookup: element type to stream-out function lookup
//
//@SEE_ALSO: bdlmxxx_descriptor, bdlmxxx_properties
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: The classes in this component are used to dispatch function
// calls based on the run-time selectable type of the object being operated on.
// 'bdlmxxx::ElemAttrLookup' returns a descriptor for a given element type.  The
// descriptor holds pointers to functions to construct an object of that type,
// destroy it, compare two such objects for equality, print an object, etc.
// 'bdlmxxx::ElemStreamInAttrLookup' and 'bdlmxxx::ElemStreamOutAttrLookup' are
// templated on a 'bdex'-compatible stream type and return pointers to
// instantiations of streaming functions for a specified type.
//
///Usage
///-----
// The following function tests the functionality of one of the bdem types:
//..
//  void testType(bdlmxxx::ElemType::Type objType, const void *otherObj) {
//      bslma::TestAllocator testAlloc;
//..
// First, we look up the descriptor for 'objType' in the 'bdlmxxx::ElemAttrLookup'
// lookup table:
//..
//  const bdlmxxx::Descriptor *desc = bdlmxxx::ElemAttrLookup::lookupTable()[objType];
//..
// Now, we use the size and constructor members of the descriptor to construct
// an unset object of 'objType' and test that it is unset.
//..
//  void *obj = testAlloc.allocate(desc->d_size);
//  desc->unsetConstruct(obj, bdlmxxx::AggregateOption::BDEM_PASS_THROUGH,
//                       &testAlloc);
//  assert(desc->isUnset(obj));
//..
// Copy 'otherObj' to 'obj' and verify the copy.
//..
//  desc->assign(obj, otherObj);
//  assert(desc->areEqual(obj, otherObj));
//..
// Then destroy and deallocate the object:
//..
//  desc->destroy(obj);
//  testAlloc.deallocate(obj);
//  }
//..
// The main program tests string and vector-of-double.  It also tests the VOID
// type.
//..
//  int main() {
//      bsl::string obj1("Hello");
//      bsl::vector<double> obj2;
//      obj2.push_back(-1.2);
//      obj2.push_back(5.8e4);
//
//      testType(bdlmxxx::ElemType::BDEM_STRING, &obj1);
//      testType(bdlmxxx::ElemType::BDEM_DOUBLE_ARRAY, &obj2);
//      testType(bdlmxxx::ElemType::BDEM_VOID, 0);
//
//      return 0;
//  }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLMXXX_CHOICEIMP
#include <bdlmxxx_choiceimp.h>
#endif

#ifndef INCLUDED_BDLMXXX_CHOICEARRAYIMP
#include <bdlmxxx_choicearrayimp.h>
#endif

#ifndef INCLUDED_BDLMXXX_DESCRIPTOR
#include <bdlmxxx_descriptor.h>
#endif

#ifndef INCLUDED_BDLMXXX_ELEMTYPE
#include <bdlmxxx_elemtype.h>
#endif

#ifndef INCLUDED_BDLMXXX_FUNCTIONTEMPLATES
#include <bdlmxxx_functiontemplates.h>
#endif

#ifndef INCLUDED_BDLMXXX_LISTIMP
#include <bdlmxxx_listimp.h>
#endif

#ifndef INCLUDED_BDLMXXX_TABLEIMP
#include <bdlmxxx_tableimp.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {



// Updated by 'bde-replace-bdet-forward-declares.py -m bdlt': 2015-02-03
// Updated declarations tagged with '// bdet -> bdlt'.

namespace bdlt { class Datetime; }                              // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::Datetime Datetime;            // bdet -> bdlt
}  // close package namespace

namespace bdlt { class DatetimeTz; }                            // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::DatetimeTz DatetimeTz;        // bdet -> bdlt
}  // close package namespace

namespace bdlt { class Date; }                                  // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::Date Date;                    // bdet -> bdlt
}  // close package namespace

namespace bdlt { class DateTz; }                                // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::DateTz DateTz;                // bdet -> bdlt
}  // close package namespace

namespace bdlt { class Time; }                                  // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::Time Time;                    // bdet -> bdlt
}  // close package namespace

namespace bdlt { class TimeTz; }                                // bdet -> bdlt

namespace bdet {typedef ::BloombergLP::bdlt::TimeTz TimeTz;                // bdet -> bdlt
}  // close package namespace

namespace bdlmxxx {
                        // ==========================
                        // struct ElemAttrLookup
                        // ==========================

struct ElemAttrLookup {
  private:
    static const Descriptor *const *const s_lookupTable;
  public:
    static const Descriptor *const *lookupTable();
};

                        // ==================================
                        // struct ElemStreamInAttrLookup
                        // ==================================

template <class STREAM>
struct ElemStreamInAttrLookup {
    static const DescriptorStreamIn<STREAM> *lookupTable();
};

                        // ===================================
                        // struct ElemStreamOutAttrLookup
                        // ===================================

template <class STREAM>
struct ElemStreamOutAttrLookup {
    static const DescriptorStreamOut<STREAM> *lookupTable();
};

// ===========================================================================
//                      INLINE AND TEMPLATE FUNCTION DEFINITIONS
// ===========================================================================

                        // --------------------------
                        // struct ElemAttrLookup
                        // --------------------------

inline
const Descriptor *const *ElemAttrLookup::lookupTable()
{
    return s_lookupTable;
}

template <class STREAM>
const DescriptorStreamIn<STREAM> *
ElemStreamInAttrLookup<STREAM>::lookupTable()
{
    typedef bsls::Types::Int64 Int64;

    static const DescriptorStreamIn<STREAM>
        table[ElemType::BDEM_NUM_TYPES] =
    {
        { &FunctionTemplates::streamInFundamental<char,      STREAM> },
        { &FunctionTemplates::streamInFundamental<short,     STREAM> },
        { &FunctionTemplates::streamInFundamental<int,       STREAM> },
        { &FunctionTemplates::streamInFundamental<Int64,     STREAM> },
        { &FunctionTemplates::streamInFundamental<float,     STREAM> },
        { &FunctionTemplates::streamInFundamental<double,    STREAM> },
        { &FunctionTemplates::streamIn<bsl::string,          STREAM> },
        { &FunctionTemplates::streamIn<bdlt::Datetime,        STREAM> },
        { &FunctionTemplates::streamIn<bdlt::Date,            STREAM> },
        { &FunctionTemplates::streamIn<bdlt::Time,            STREAM> },
        { &FunctionTemplates::streamInArray<char,            STREAM> },
        { &FunctionTemplates::streamInArray<short,           STREAM> },
        { &FunctionTemplates::streamInArray<int,             STREAM> },
        { &FunctionTemplates::streamInArray<Int64,           STREAM> },
        { &FunctionTemplates::streamInArray<float,           STREAM> },
        { &FunctionTemplates::streamInArray<double,          STREAM> },
        { &FunctionTemplates::streamInArray<bsl::string,     STREAM> },
        { &FunctionTemplates::streamInArray<bdlt::Datetime,   STREAM> },
        { &FunctionTemplates::streamInArray<bdlt::Date,       STREAM> },
        { &FunctionTemplates::streamInArray<bdlt::Time,       STREAM> },

        { &ListImp::streamInList<                            STREAM> },
        { &TableImp::streamInTable<                          STREAM> },

        { &FunctionTemplates::streamInFundamental<bool,      STREAM> },
        { &FunctionTemplates::streamIn<bdlt::DatetimeTz,      STREAM> },
        { &FunctionTemplates::streamIn<bdlt::DateTz,          STREAM> },
        { &FunctionTemplates::streamIn<bdlt::TimeTz,          STREAM> },
        { &FunctionTemplates::streamInArray<bool,            STREAM> },
        { &FunctionTemplates::streamInArray<bdlt::DatetimeTz, STREAM> },
        { &FunctionTemplates::streamInArray<bdlt::DateTz,     STREAM> },
        { &FunctionTemplates::streamInArray<bdlt::TimeTz,     STREAM> },

        { &ChoiceImp::streamInChoice<                        STREAM> },
        { &ChoiceArrayImp::streamInChoiceArray<              STREAM> }
    };

    return table;
}

template <class STREAM>
const DescriptorStreamOut<STREAM> *
ElemStreamOutAttrLookup<STREAM>::lookupTable()
{
    typedef bsls::Types::Int64 Int64;

    static const DescriptorStreamOut<STREAM>
        table[ElemType::BDEM_NUM_TYPES] =
    {
        { &FunctionTemplates::streamOutFundamental<char,      STREAM> },
        { &FunctionTemplates::streamOutFundamental<short,     STREAM> },
        { &FunctionTemplates::streamOutFundamental<int,       STREAM> },
        { &FunctionTemplates::streamOutFundamental<Int64,     STREAM> },
        { &FunctionTemplates::streamOutFundamental<float,     STREAM> },
        { &FunctionTemplates::streamOutFundamental<double,    STREAM> },
        { &FunctionTemplates::streamOut<bsl::string,          STREAM> },
        { &FunctionTemplates::streamOut<bdlt::Datetime,        STREAM> },
        { &FunctionTemplates::streamOut<bdlt::Date,            STREAM> },
        { &FunctionTemplates::streamOut<bdlt::Time,            STREAM> },
        { &FunctionTemplates::streamOutArray<char,            STREAM> },
        { &FunctionTemplates::streamOutArray<short,           STREAM> },
        { &FunctionTemplates::streamOutArray<int,             STREAM> },
        { &FunctionTemplates::streamOutArray<Int64,           STREAM> },
        { &FunctionTemplates::streamOutArray<float,           STREAM> },
        { &FunctionTemplates::streamOutArray<double,          STREAM> },
        { &FunctionTemplates::streamOutArray<bsl::string,     STREAM> },
        { &FunctionTemplates::streamOutArray<bdlt::Datetime,   STREAM> },
        { &FunctionTemplates::streamOutArray<bdlt::Date,       STREAM> },
        { &FunctionTemplates::streamOutArray<bdlt::Time,       STREAM> },

        { &ListImp::streamOutList<                            STREAM> },
        { &TableImp::streamOutTable<                          STREAM> },

        { &FunctionTemplates::streamOutFundamental<bool,      STREAM> },
        { &FunctionTemplates::streamOut<bdlt::DatetimeTz,      STREAM> },
        { &FunctionTemplates::streamOut<bdlt::DateTz,          STREAM> },
        { &FunctionTemplates::streamOut<bdlt::TimeTz,          STREAM> },
        { &FunctionTemplates::streamOutArray<bool,            STREAM> },
        { &FunctionTemplates::streamOutArray<bdlt::DatetimeTz, STREAM> },
        { &FunctionTemplates::streamOutArray<bdlt::DateTz,     STREAM> },
        { &FunctionTemplates::streamOutArray<bdlt::TimeTz,     STREAM> },

        { &ChoiceImp::streamOutChoice<                        STREAM> },
        { &ChoiceArrayImp::streamOutChoiceArray<              STREAM> }
    };

    return table;
}
}  // close package namespace

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
