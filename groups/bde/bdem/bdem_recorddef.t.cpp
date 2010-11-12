// bdem_recorddef.t.cpp                  -*-C++-*-

#include <bdem_recorddef.h>

#include <bdem_enumerationdef.h>

#include <bdema_sequentialallocator.h>

#include <bslma_default.h>


#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//
//
//-----------------------------------------------------------------------------

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ cout << "\t" << flush;             // Print tab w/o newline

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                           CLASSES FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------

namespace BloombergLP {
  
class bdem_Schema {
        // Dummy type.
};
  
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.  Suppress
        //   all 'bsl::cout' statements in non-verbose mode, and add streaming
        //   to a buffer to test programmatically the printing examples.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Usage Examples"
                               << "\n======================" << bsl::endl;


///Schema Description
///- - - - - - - - - -
// In following examples we configure several record definitions and an
// enumeration definition that collectively could be used to describe the
// value of a sale.  For the purposes of this example a sale can be performed
// using either check, credit-card, or cash.  The set of definitions will have
// the following structure: 
//.. 
//        +---------------------------------------------------------+
//        |{                                                        |
//        |    ENUMERATION "CREDIT_CARD_TYPE" {                     |
//        |        "MASTERCARD"                                     |
//        |        "VISA"                                           |
//        |        "AMEX"                                           |
//        |    }                                                    |
//        |                                                         |
//        |    RECORD "CREDIT_CARD_PAYMENT" {                       |
//        |        STRING<"CREDIT_CARD_TYPE">  "cardType";          |
//        |        STRING                      "name";              |
//        |        INT                         "cardNumber";        |
//        |    }                                                    |
//        |                                                         |
//        |    RECORD "CHECK_PAYMENT" {                             |
//        |        STRING  "bankName";                              |
//        |        INT     "routingNumber";                         |
//        |    }                                                    |
//        |                                                         |
//        |    CHOICE RECORD "PAYMENT_INFO" {                       |
//        |        LIST<"CREDIT_CARD_PAYMENT"> "creditCardPayment"; |
//        |        LIST<"CHECK_PAYMENT">       "checkPayment";      |
//        |        BOOL                        "cashPayment";       |
//        |    }                                                    |
//        |                                                         |
//        |    RECORD "SALE" {                                      |
//        |        INT                    "productId";              |
//        |        INT                    "quantity" [default: 1];  |
//        |        STRING                 "customerName";           |
//        |        DOUBLE                 "price"                   |
//        |        BDET_DATE              "date";                   |
//        |        CHOICE<"PAYMENT_INFO"> "paymentInfo";            |
//        |    }                                                    |
//        |}                                                        |
//        +---------------------------------------------------------+
//..
// The selection between 3 different payment options (credit-card, check, and
// ccash) is described by the choice record "PAYMENT_INFO".  Choice record
// definitions (i.e., record definitions of type 'BDEM_CHOICE_RECORD')
// indicate a selection between its contained fields.  A choice record
// definition can be used to constraint fields of type 'CHOICE' and
// 'CHOICE_ARRAY'.  The choice record defintion "PAYMENT_INFO" is used to
// constraint the choice field "paymentInfo" in the "SALE" record definition.
//
// The record definitions "SALE", "CREDIT_CARD_PAYMENT", and "CHECK_PAYMENT",
// are all of type 'BDEM_SEQUENCE_RECORD'.  A sequence records definition
// indicates that the record value is a collection of the indicated field
// values (rather than a choice between field values).  Sequence record
// definitions can be used to constraint fields definitions of type 'LIST' and
// 'TABLE'.  In this example, the sequence records defintions
// "CREDIT_CARD_PAYMENT" and "CHECK_PAYMENT" are used to constraint the values
// in the lists "creditCardPayment" and "cashPayment" (in the "PAYMENT_INFO'
// choice record definition).
//
// Finally, note that "SALE" has a "quanity" whose default value is 1. 
//
// Here are a couple hypothetical record values that this schema might
// describe:
//..
//  {
//      productId:    386
//      quantity:     4
//      customerName: Jeff
//      price:        29.99
//      date:         April 22, 2010
//      paymentInfo:  {
//          cashPayment: { true }
//      }
//  }
//  {
//      productId:    486
//      customerName: Sally
//      price:        48.50
//      date:         May 1, 2010
//      paymentInfo:  {
//          creditCardPayment:  { 
//              cardType:   "VISA"
//              name:       Sally Jenkins
//              cardNumber: 1111222223333
//          } 
//      }
//  }
//..
// Note that the second record does not specify quantity, indicating the
// schema's default value of 1.
//
///Directly Creating a 'bdem_RecordDef' (*Not Recommended*)
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following section demontrates how to create a 'bdem_RecordRef' (and
// 'bdem_EnumerationDef') in isolation -- this is *not* the recommended usage
// of 'bdem_RecordDef' (or 'bdem_EnumerationDef').  A 'bdem_RecordDef' is
// meant to be used in conjuction with the types defined in 'bdem_schema'.
// Clients should not create a 'bdem_RecordDef' directly, but obtain one from
// a 'bdem_Schema'.  For the purpose of this usage example, we define a dummy
// stand-in type for 'bdem_Schema'.  This type is used in name-only by
// 'bdem_RecordDef', so its definition is not necessary: 
//..
//  namespace BloombergLP {
//  
//  class bdem_Schema {
//      // Dummy type.
//  };
//  
//  }
//..
// Now we create record and enumeration definition objects matching the
// names in the example above.  We create one enumeration definition,
// "CREDIT_CARD_TYPE", one record definition, "PAYMENT_INFO", of type choice
// (i.e., 'BDEM_CHOICE_RECORD'), and three record defintions, 
// "CREDIT_CARD_PAYMENT", "CHECK_PAYMENT", and "SALE", of type sequence
// (i.e., 'BDEM_SEQUENCE_RECORD'):
//..
    bslma_Allocator           *allocator = bslma_Default::allocator(); 
    bdema_SequentialAllocator  seqAllocator(allocator);
    bdem_Schema                dummySchema;
 
    bdem_EnumerationDef ccTypeEnumDef(&dummySchema,
                                      0,
                                      "CREDIT_CARD_TYPE",
                                      &seqAllocator);
    bdem_RecordDef ccPaymentRecDef(&dummySchema,
                                   0, 
                                   "CREDIT_CARD_PAYMENT",
                                   bdem_RecordDef::BDEM_SEQUENCE_RECORD,
                                   &seqAllocator,
                                   allocator);
    bdem_RecordDef checkPaymentRecDef(&dummySchema,
                                      1,
                                      "CHECK_PAYMENT",
                                      bdem_RecordDef::BDEM_SEQUENCE_RECORD,
                                      &seqAllocator,
                                      allocator);
    bdem_RecordDef paymentInfoRecDef(&dummySchema,
                                     2,
                                     "PAYMENT_INFO",
                                     bdem_RecordDef::BDEM_CHOICE_RECORD,
                                     &seqAllocator,
                                     allocator);
    bdem_RecordDef saleRecDef(&dummySchema,
                              3, 
                              "SALE",
                              bdem_RecordDef::BDEM_SEQUENCE_RECORD,
                              &seqAllocator,
                              allocator);
//..
//
///Maniupulating and Accessing a 'bdem_RecordDef'
///- - - - - - - - - - - - - - - - - - - - - - - 
// In the following section we demonstrate how to append fields to a record
// definition, and access its properties.  We will use the record definitions
// (and enumeration definition) created in the preceeding section.  In practice
// clients should obtain these definitions from a schema (see 'bdem_schema').
// 
// We start by examining the properties for the sequence record definition, 
// 'ccPaymentRecDef'.  Note that the values returned for 'recordName',
// 'recordIndex', and 'recordType' are the values supplied atconstruction:
//..
    ASSERT(0 == ccPaymentRecDef.recordIndex());
    ASSERT(0 == bsl::strcmp(ccPaymentRecDef.recordName(),
                            "CREDIT_CARD_PAYMENT"));
    ASSERT(bdem_RecordDef::BDEM_SEQUENCE_RECORD == 
                                                 ccPaymentRecDef.recordType());
    ASSERT(0 == ccPaymentRecDef.numFields());
    ASSERT(&dummySchema == &ccPaymentRecDef.schema());
//..
// We also examine the properties for the choice record definition,
// 'paymentInfoRecDef': 
//..
    ASSERT(2 == paymentInfoRecDef.recordIndex());
    ASSERT(0 == bsl::strcmp(paymentInfoRecDef.recordName(),
                            "PAYMENT_INFO"));
    ASSERT(bdem_RecordDef::BDEM_CHOICE_RECORD == 
                                               paymentInfoRecDef.recordType());
    ASSERT(0 == paymentInfoRecDef.numFields());
    ASSERT(&dummySchema == &paymentInfoRecDef.schema());
//..
// Next we add the sequence of enumeration values to 'ccTypeEnumDef' (see
// 'bdem_enumerationdef' for more information):
//..
    int id1 = ccTypeEnumDef.addEnumerator("MASTERCARD");
    int id2 = ccTypeEnumDef.addEnumerator("VISA");
    int id3 = ccTypeEnumDef.addEnumerator("AMEX");
    ASSERT(0 == id1); ASSERT(1 == id2); ASSERT(2 == id3);
//..
// Next we append the field definitions to 'ccPaymentRecDef'.  When we add
// the field definition "cardType" we supply the enumeration defintion 
// 'ccTypeEnumDef' as an enumeration constraint for the values of that field:
//..
    ccPaymentRecDef.appendField(bdem_ElemType::BDEM_STRING,
                                &ccTypeEnumDef,
                                "cardType");
    ccPaymentRecDef.appendField(bdem_ElemType::BDEM_STRING, 
                                "name");
    ccPaymentRecDef.appendField(bdem_ElemType::BDEM_INT,    
                                "cardNumber");
//..
// We verify the properties of a one of the fields we've appended.  By
// default, a field will have a null field-id, it will not be nullable, it
// will not have a default value, and its formatting mode will be
// 'bdeat_FormattingMode::BDEAT_DEFAULT': 
//..
    ASSERT(3 == ccPaymentRecDef.numFields());
    const bdem_FieldDef& cardTypeFldDef = ccPaymentRecDef.field(0);

    ASSERT(0 == bsl::strcmp("cardType", cardTypeFldDef.fieldName()));
    ASSERT(bdem_RecordDef::BDEM_NULL_FIELD_ID == cardTypeFldDef.fieldId());
    ASSERT(bdem_ElemType::BDEM_STRING    == cardTypeFldDef.elemType());
    ASSERT(false                         == cardTypeFldDef.hasDefaultValue());
    ASSERT(false                         == cardTypeFldDef.isNullable());
    ASSERT(bdeat_FormattingMode::BDEAT_DEFAULT == 
                                       cardTypeFldDef.formattingMode());
    ASSERT(&ccTypeEnumDef == cardTypeFldDef.enumerationConstraint());
//..
// Next append the fields to 'checkPaymentRecDef':
//.. 
    checkPaymentRecDef.appendField(bdem_ElemType::BDEM_STRING,
                                   "bankName");
    checkPaymentRecDef.appendField(bdem_ElemType::BDEM_INT,    
                                   "routingNumber");
//..
// Next we append the fields to the choice record definition
// 'paymentInfoRecDef'.  When we add the field definition "creditCardPayment"
// we provide the record definition 'ccPaymentRecDef' as a record constraint
// for the the values of that field.  Similarly, we provide
// 'checkPaymentRecDef' as a constraint for the "checkPayment" field:
//..
    paymentInfoRecDef.appendField(bdem_ElemType::BDEM_LIST,
                                  &ccPaymentRecDef,
                                  "creditCardPayment");
    paymentInfoRecDef.appendField(bdem_ElemType::BDEM_LIST,
                                  &checkPaymentRecDef,
                                  "checkPayment");
    paymentInfoRecDef.appendField(bdem_ElemType::BDEM_BOOL, 
                                  "cashPayment");
//..
// Next we append the fields to the record definition 'saleRecDef'.  For the
// field "quantity" we create a 'bdem_FieldDefAttributes' that we supply to
// 'appendField' so we can provide a default value of 1 for the field:
//..
    saleRecDef.appendField(bdem_ElemType::BDEM_INT, "productId");

    bdem_FieldDefAttributes quantityType(bdem_ElemType::BDEM_INT);
    quantityType.defaultValue().theModifiableInt() = 1;
    saleRecDef.appendField(quantityType, "quantity");

    saleRecDef.appendField(bdem_ElemType::BDEM_INT,    "productId");
    saleRecDef.appendField(bdem_ElemType::BDEM_STRING, "customerName");
    saleRecDef.appendField(bdem_ElemType::BDEM_DOUBLE, "price");
    saleRecDef.appendField(bdem_ElemType::BDEM_DATE,   "date");
    saleRecDef.appendField(bdem_ElemType::BDEM_LIST,
                           &paymentInfoRecDef,
                           "paymentInfo");
//..




      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING/USAGE TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        if (verbose) cout << "\tChoice and Sequence traits." << endl;

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
