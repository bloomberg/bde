// bdem_elemattrlookup.h                                              -*-C++-*-
#ifndef INCLUDED_BDEM_ELEMATTRLOOKUP
#define INCLUDED_BDEM_ELEMATTRLOOKUP

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide lookup for descriptors and attributes of 'bdem' types.
//
//@CLASSES:
//  bdem_ElemAttrLookup: element type to descriptor lookup
//  bdem_ElemStreamInAttrLookup: element type to stream-in function lookup
//  bdem_ElemStreamOutAttrLookup: element type to stream-out function lookup
//
//@SEE_ALSO: bdem_descriptor, bdem_properties
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: The classes in this component are used to dispatch function
// calls based on the run-time selectable type of the object being operated on.
// 'bdem_ElemAttrLookup' returns a descriptor for a given element type.  The
// descriptor holds pointers to functions to construct an object of that type,
// destroy it, compare two such objects for equality, print an object, etc.
// 'bdem_ElemStreamInAttrLookup' and 'bdem_ElemStreamOutAttrLookup' are
// templated on a 'bdex'-compatible stream type and return pointers to
// instantiations of streaming functions for a specified type.
//
///Usage
///-----
// The following function tests the functionality of one of the bdem types:
//..
//  void testType(bdem_ElemType::Type objType, const void *otherObj) {
//      bslma_TestAllocator testAlloc;
//..
// First, we look up the descriptor for 'objType' in the 'bdem_ElemAttrLookup'
// lookup table:
//..
//  const bdem_Descriptor *desc = bdem_ElemAttrLookup::lookupTable()[objType];
//..
// Now, we use the size and constructor members of the descriptor to construct
// an unset object of 'objType' and test that it is unset.
//..
//  void *obj = testAlloc.allocate(desc->d_size);
//  desc->unsetConstruct(obj, bdem_AggregateOption::BDEM_PASS_THROUGH,
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
//      testType(bdem_ElemType::BDEM_STRING, &obj1);
//      testType(bdem_ElemType::BDEM_DOUBLE_ARRAY, &obj2);
//      testType(bdem_ElemType::BDEM_VOID, 0);
//
//      return 0;
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEM_CHOICEIMP
#include <bdem_choiceimp.h>
#endif

#ifndef INCLUDED_BDEM_CHOICEARRAYIMP
#include <bdem_choicearrayimp.h>
#endif

#ifndef INCLUDED_BDEM_DESCRIPTOR
#include <bdem_descriptor.h>
#endif

#ifndef INCLUDED_BDEM_ELEMTYPE
#include <bdem_elemtype.h>
#endif

#ifndef INCLUDED_BDEM_FUNCTIONTEMPLATES
#include <bdem_functiontemplates.h>
#endif

#ifndef INCLUDED_BDEM_LISTIMP
#include <bdem_listimp.h>
#endif

#ifndef INCLUDED_BDEM_TABLEIMP
#include <bdem_tableimp.h>
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

class bdet_Datetime;
class bdet_DatetimeTz;
class bdet_Date;
class bdet_DateTz;
class bdet_Time;
class bdet_TimeTz;

                        // ==========================
                        // struct bdem_ElemAttrLookup
                        // ==========================

struct bdem_ElemAttrLookup {
  private:
    static const bdem_Descriptor *const *const d_lookupTable;
  public:
    static const bdem_Descriptor *const *lookupTable();
};

                        // ==================================
                        // struct bdem_ElemStreamInAttrLookup
                        // ==================================

template <class STREAM>
struct bdem_ElemStreamInAttrLookup {
    static const bdem_DescriptorStreamIn<STREAM> *lookupTable();
};

                        // ===================================
                        // struct bdem_ElemStreamOutAttrLookup
                        // ===================================

template <class STREAM>
struct bdem_ElemStreamOutAttrLookup {
    static const bdem_DescriptorStreamOut<STREAM> *lookupTable();
};

// ===========================================================================
//                      INLINE AND TEMPLATE FUNCTION DEFINITIONS
// ===========================================================================

                        // --------------------------
                        // struct bdem_ElemAttrLookup
                        // --------------------------

inline
const bdem_Descriptor *const *bdem_ElemAttrLookup::lookupTable()
{
    return d_lookupTable;
}

template <class STREAM>
const bdem_DescriptorStreamIn<STREAM> *
bdem_ElemStreamInAttrLookup<STREAM>::lookupTable()
{
    typedef bsls_Types::Int64 Int64;

    static const bdem_DescriptorStreamIn<STREAM>
        table[bdem_ElemType::BDEM_NUM_TYPES] =
    {
        { &bdem_FunctionTemplates::streamInFundamental<char,      STREAM> },
        { &bdem_FunctionTemplates::streamInFundamental<short,     STREAM> },
        { &bdem_FunctionTemplates::streamInFundamental<int,       STREAM> },
        { &bdem_FunctionTemplates::streamInFundamental<Int64,     STREAM> },
        { &bdem_FunctionTemplates::streamInFundamental<float,     STREAM> },
        { &bdem_FunctionTemplates::streamInFundamental<double,    STREAM> },
        { &bdem_FunctionTemplates::streamIn<bsl::string,          STREAM> },
        { &bdem_FunctionTemplates::streamIn<bdet_Datetime,        STREAM> },
        { &bdem_FunctionTemplates::streamIn<bdet_Date,            STREAM> },
        { &bdem_FunctionTemplates::streamIn<bdet_Time,            STREAM> },
        { &bdem_FunctionTemplates::streamInArray<char,            STREAM> },
        { &bdem_FunctionTemplates::streamInArray<short,           STREAM> },
        { &bdem_FunctionTemplates::streamInArray<int,             STREAM> },
        { &bdem_FunctionTemplates::streamInArray<Int64,           STREAM> },
        { &bdem_FunctionTemplates::streamInArray<float,           STREAM> },
        { &bdem_FunctionTemplates::streamInArray<double,          STREAM> },
        { &bdem_FunctionTemplates::streamInArray<bsl::string,     STREAM> },
        { &bdem_FunctionTemplates::streamInArray<bdet_Datetime,   STREAM> },
        { &bdem_FunctionTemplates::streamInArray<bdet_Date,       STREAM> },
        { &bdem_FunctionTemplates::streamInArray<bdet_Time,       STREAM> },

        { &bdem_ListImp::streamInList<                            STREAM> },
        { &bdem_TableImp::streamInTable<                          STREAM> },

        { &bdem_FunctionTemplates::streamInFundamental<bool,      STREAM> },
        { &bdem_FunctionTemplates::streamIn<bdet_DatetimeTz,      STREAM> },
        { &bdem_FunctionTemplates::streamIn<bdet_DateTz,          STREAM> },
        { &bdem_FunctionTemplates::streamIn<bdet_TimeTz,          STREAM> },
        { &bdem_FunctionTemplates::streamInArray<bool,            STREAM> },
        { &bdem_FunctionTemplates::streamInArray<bdet_DatetimeTz, STREAM> },
        { &bdem_FunctionTemplates::streamInArray<bdet_DateTz,     STREAM> },
        { &bdem_FunctionTemplates::streamInArray<bdet_TimeTz,     STREAM> },

        { &bdem_ChoiceImp::streamInChoice<                        STREAM> },
        { &bdem_ChoiceArrayImp::streamInChoiceArray<              STREAM> }
    };

    return table;
}

template <class STREAM>
const bdem_DescriptorStreamOut<STREAM> *
bdem_ElemStreamOutAttrLookup<STREAM>::lookupTable()
{
    typedef bsls_Types::Int64 Int64;

    static const bdem_DescriptorStreamOut<STREAM>
        table[bdem_ElemType::BDEM_NUM_TYPES] =
    {
        { &bdem_FunctionTemplates::streamOutFundamental<char,      STREAM> },
        { &bdem_FunctionTemplates::streamOutFundamental<short,     STREAM> },
        { &bdem_FunctionTemplates::streamOutFundamental<int,       STREAM> },
        { &bdem_FunctionTemplates::streamOutFundamental<Int64,     STREAM> },
        { &bdem_FunctionTemplates::streamOutFundamental<float,     STREAM> },
        { &bdem_FunctionTemplates::streamOutFundamental<double,    STREAM> },
        { &bdem_FunctionTemplates::streamOut<bsl::string,          STREAM> },
        { &bdem_FunctionTemplates::streamOut<bdet_Datetime,        STREAM> },
        { &bdem_FunctionTemplates::streamOut<bdet_Date,            STREAM> },
        { &bdem_FunctionTemplates::streamOut<bdet_Time,            STREAM> },
        { &bdem_FunctionTemplates::streamOutArray<char,            STREAM> },
        { &bdem_FunctionTemplates::streamOutArray<short,           STREAM> },
        { &bdem_FunctionTemplates::streamOutArray<int,             STREAM> },
        { &bdem_FunctionTemplates::streamOutArray<Int64,           STREAM> },
        { &bdem_FunctionTemplates::streamOutArray<float,           STREAM> },
        { &bdem_FunctionTemplates::streamOutArray<double,          STREAM> },
        { &bdem_FunctionTemplates::streamOutArray<bsl::string,     STREAM> },
        { &bdem_FunctionTemplates::streamOutArray<bdet_Datetime,   STREAM> },
        { &bdem_FunctionTemplates::streamOutArray<bdet_Date,       STREAM> },
        { &bdem_FunctionTemplates::streamOutArray<bdet_Time,       STREAM> },

        { &bdem_ListImp::streamOutList<                            STREAM> },
        { &bdem_TableImp::streamOutTable<                          STREAM> },

        { &bdem_FunctionTemplates::streamOutFundamental<bool,      STREAM> },
        { &bdem_FunctionTemplates::streamOut<bdet_DatetimeTz,      STREAM> },
        { &bdem_FunctionTemplates::streamOut<bdet_DateTz,          STREAM> },
        { &bdem_FunctionTemplates::streamOut<bdet_TimeTz,          STREAM> },
        { &bdem_FunctionTemplates::streamOutArray<bool,            STREAM> },
        { &bdem_FunctionTemplates::streamOutArray<bdet_DatetimeTz, STREAM> },
        { &bdem_FunctionTemplates::streamOutArray<bdet_DateTz,     STREAM> },
        { &bdem_FunctionTemplates::streamOutArray<bdet_TimeTz,     STREAM> },

        { &bdem_ChoiceImp::streamOutChoice<                        STREAM> },
        { &bdem_ChoiceArrayImp::streamOutChoiceArray<              STREAM> }
    };

    return table;
}

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
