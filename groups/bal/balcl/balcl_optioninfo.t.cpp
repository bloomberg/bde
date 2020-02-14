// balcl_optioninfo.t.cpp                                             -*-C++-*-

#include <balcl_optioninfo.h>

#include <balcl_typeinfo.h>
#include <balcl_occurrenceinfo.h>

#include <bdlb_tokenizer.h>

#include <bslim_testutil.h>

#include <bslma_default.h>  // 'bslma::Default::globalAllocator'
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>
#include <bsls_platform.h>  // 'BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC'

#include <bsl_cstddef.h>    // 'bsl::size_t'
#include <bsl_cstdlib.h>    // 'bsl::atoi'
#include <bsl_cstring.h>    // 'bsl::strspn'
#include <bsl_iostream.h>
#include <bsl_memory.h>     // 'bsl::shared_ptr'
#include <bsl_ostream.h>    // 'operator<<'
#include <bsl_sstream.h>    // 'bsl::ostringstream'
#include <bsl_string.h>     // 'bsl::string', 'bslstl::StringRef'

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// This component defines a 'struct' that is used as a POD having no
// user-defined methods.  Thus, our concerns are that the 'struct' has all of
// the expected methods and that the several free operators work as expected.
//
// ----------------------------------------------------------------------------
// FREE OPERATORS
// [ 3] bool operator==(const OptionInfo& lhs, rhs);
// [ 3] bool operator!=(const OptionInfo& lhs, rhs);
// [ 4] ostream& operator<<(ostream& stream, const OptionInfo& rhs);
// ----------------------------------------------------------------------------
// [ 2] CONCERN: The 'struct' has the expected members and enumerators.
// [ 1] BREATHING TEST

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                      GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef balcl::OptionInfo     Obj;

typedef balcl::OccurrenceInfo OccurrenceInfo;
typedef balcl::TypeInfo       TypeInfo;

// ============================================================================
//                      GLOBAL DATA FOR TESTING
// ----------------------------------------------------------------------------

bool        linkedFlag;
bsl::string defaultValue("default");
bsl::string tagString("genericTag");

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
static const Obj specTable[] = {
                    {
                    }
                  , {
                      tagString                              // tag
                    }
                  , {
                      "s|longTag"                            // tag
                    }
                  , {
                      "s|longTag",                           // tag
                      "name"                                 // name
                    }
                  , {
                      "s|longTag",                           // tag
                      "name",                                // name
                      "description"                          // description
                    }
                  , {
                      "s|longTag",                           // tag
                      "name",                                // name
                      "description",                         // description
                      balcl::TypeInfo(&linkedFlag)           // linked variable
                    }
                  , {
                      "s|longTag",                           // tag
                      "name",                                // name
                      "description",                         // description
                      balcl::TypeInfo(&linkedFlag),          // linked variable
                      balcl::OccurrenceInfo::e_REQUIRED      // occurrence info
                    }
                  , {
                      "s|longTag",                           // tag
                      "name",                                // name
                      "description",                         // description
                      balcl::TypeInfo(&linkedFlag),          // linked variable
                      balcl::OccurrenceInfo(defaultValue)    // occurrence info
                    }
};
#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic pop
#endif

#define NL "\n"
static const char * const expect[] = {
    "{"                                      NL
    "    NON_OPTION"                         NL
    "    NAME            \"\""               NL
    "    DESCRIPTION     \"\""               NL
    "    TYPE_INFO       {"                  NL
    "        TYPE       STRING"              NL
    "        CONSTRAINT 0x0000000"           NL
    "    }"                                  NL
    "    OCCURRENCE_INFO OPTIONAL"           NL
    "}"                                // no-NL

  , "{"                                      NL
    "    TAG             \"genericTag\""     NL
    "    NAME            \"\""               NL
    "    DESCRIPTION     \"\""               NL
    "    TYPE_INFO       {"                  NL
    "        TYPE       STRING"              NL
    "        CONSTRAINT 0x0000000"           NL
    "    }"                                  NL
    "    OCCURRENCE_INFO OPTIONAL"           NL
    "}"  //                               no-NL

  , "{"                                      NL
    "    TAG             \"s|longTag\""      NL
    "    NAME            \"\""               NL
    "    DESCRIPTION     \"\""               NL
    "    TYPE_INFO       {"                  NL
    "        TYPE       STRING"              NL
    "        CONSTRAINT 0x0000000"           NL
    "    }"                                  NL
    "    OCCURRENCE_INFO OPTIONAL"           NL
    "}"  //                               no-NL

  , "{"                                      NL
    "    TAG             \"s|longTag\""      NL
    "    NAME            \"name\""           NL
    "    DESCRIPTION     \"\""               NL
    "    TYPE_INFO       {"                  NL
    "        TYPE       STRING"              NL
    "        CONSTRAINT 0x0000000"           NL
    "    }"                                  NL
    "    OCCURRENCE_INFO OPTIONAL"           NL
    "}"                                // no-NL

  , "{"                                      NL
    "    TAG             \"s|longTag\""      NL
    "    NAME            \"name\""           NL
    "    DESCRIPTION     \"description\""    NL
    "    TYPE_INFO       {"                  NL
    "        TYPE       STRING"              NL
    "        CONSTRAINT 0x0000000"           NL
    "    }"                                  NL
    "    OCCURRENCE_INFO OPTIONAL"           NL
    "}"  //                               no-NL

  , "{"                                      NL
    "    TAG             \"s|longTag\""      NL
    "    NAME            \"name\""           NL
    "    DESCRIPTION     \"description\""    NL
    "    TYPE_INFO       {"                  NL
    "        TYPE       BOOL"                NL
    "        VARIABLE   0x00000000"          NL
    "        CONSTRAINT 0x0000000"           NL
    "    }"                                  NL
    "    OCCURRENCE_INFO OPTIONAL"           NL
    "}"                                // no-NL

  , "{"                                      NL
    "    TAG             \"s|longTag\""      NL
    "    NAME            \"name\""           NL
    "    DESCRIPTION     \"description\""    NL
    "    TYPE_INFO       {"                  NL
    "        TYPE       BOOL"                NL
    "        VARIABLE   0x00000000"          NL
    "        CONSTRAINT 0x0000000"           NL
    "    }"                                  NL
    "    OCCURRENCE_INFO REQUIRED"           NL
    "}"                                // no-NL

  , "{"                                      NL
    "    TAG             \"s|longTag\""      NL
    "    NAME            \"name\""           NL
    "    DESCRIPTION     \"description\""    NL
    "    TYPE_INFO       {"                  NL
    "        TYPE       BOOL"                NL
    "        VARIABLE   0x00000000"          NL
    "        CONSTRAINT 0x0000000"           NL
    "    }"                                  NL
    "    OCCURRENCE_INFO {"                  NL
    "        OPTIONAL"                       NL
    "        DEFAULT_TYPE  STRING"           NL
    "        DEFAULT_VALUE default"          NL
    "    }"                                  NL
    "}"                                // no-NL
};
#undef NL

enum { k_NUM_SPEC_TABLE = sizeof specTable / sizeof *specTable
     , k_NUM_EXPECT     = sizeof expect    / sizeof *expect };

BSLMF_ASSERT(k_NUM_SPEC_TABLE == k_NUM_EXPECT);

// ============================================================================
//                          HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

namespace {
namespace u {

bool isMatch(const char *expect, const char *actual, const Obj& obj)
    // Return 'true' if the specified 'actual' string matches the specified
    // 'expect' string, and 'false' otherwise.  The two strings must be
    // byte-for-byte identical, including whitespace, except for the
    // (process-specific address) fields following the fields 'CONSTRAINT' and
    // 'VARIABLE', if present.  When a field is expected to contain an address
    // the placeholder field in 'expect' is ignored and the expected field is
    // calculated from the specified 'obj'.
{
    BSLS_ASSERT(expect);
    BSLS_ASSERT(actual);

    bool               firstToken      = true;
    bool               addressExpected = false;
    const char * const softDelimiters  = " \n";

    bdlb::Tokenizer expectLexer(expect, softDelimiters);
    bdlb::Tokenizer actualLexer(actual, softDelimiters);

    for (;
         expectLexer.isValid() && actualLexer.isValid();
         ++expectLexer, ++actualLexer) {

        typedef bslstl::StringRef SR;

        SR expectToken         = expectLexer.token();
        SR actualToken         = actualLexer.token();

        SR expectTrailingDelim = expectLexer.trailingDelimiter();
        SR actualTrailingDelim = actualLexer.trailingDelimiter();

        static bsl::string expectedAddress(bslma::Default::globalAllocator());

        if (addressExpected) {
            if (expectedAddress != actualToken) {
                return false;                                         // RETURN
            }

            addressExpected = false;
            continue;
        } else {
            if (firstToken) {
                SR expectPreviousDelim = expectLexer.previousDelimiter();
                SR actualPreviousDelim = actualLexer.previousDelimiter();

                if (expectPreviousDelim != actualPreviousDelim) {
                    return false;                                     // RETURN
                }

                firstToken = false;
            }
            if (expectToken         != actualToken) {
                return false;                                         // RETURN
            }
            if (expectTrailingDelim != actualTrailingDelim) {
                return false;                                         // RETURN
            }
        }

        if ("VARIABLE"   == expectToken) {
            void *linkedVariableAddress = obj.d_typeInfo.linkedVariable();

            if (0 == linkedVariableAddress) {
                return false;                                         // RETURN
            }

            bsl::ostringstream ossAddress;
            ossAddress << linkedVariableAddress;

            expectedAddress = ossAddress.str();
            addressExpected = true;
        }

        if ("CONSTRAINT" == expectToken) {
            void *constraintAddress = static_cast<void *>(
                                            obj.d_typeInfo.constraint().get());
            if (0 == constraintAddress) {
                return false;                                         // RETURN
            }

            bsl::ostringstream ossAddress;
            ossAddress << constraintAddress;

            expectedAddress = ossAddress.str();
            addressExpected = true;
        }
    }

    if (expectLexer.isValid() || actualLexer.isValid()) { // one not empty
        return false;                                                 // RETURN
    }

    return true;
}

}  // close namespace u
}  // close unnamed namespace

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, const char *argv[])  {
    int                test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int             verbose = argc > 2;
    int         veryVerbose = argc > 3;
    int     veryVeryVerbose = argc > 4;  (void) veryVeryVerbose;
    int veryVeryVeryVerbose = argc > 5;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator        ga("global",   veryVeryVeryVerbose);
    bslma::TestAllocatorMonitor gam(&ga);
    bslma::Default::setGlobalAllocator(&ga);

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::TestAllocatorMonitor  dam(&da);
    bslma::DefaultAllocatorGuard dag(&da);

    switch (test) { case 0:  // Zero is always the leading case.
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'operator<<'
        //
        // Concerns:
        //: 1 The 'operator<<' output matches the expected output except for
        //:   the process-specific fields (addresses) following 'CONSTRAINT'
        //:   and 'VARIABLE', if present.
        //:
        //: 2 The generated string is *not* terminated by a newline.
        //:
        //: 3 The 'operator<<' has the expected signature and return value.
        //:
        //: 4 The 'operator<<' returns a reference to the given 'stream'.
        //
        // Plan:
        //: 1 Use the function-address idiom to confirm the signature and
        //:   return type.  (C-3)
        //:
        //: 2 Compare the output to the expected result for a representative
        //:   set of 'OptionInfo' objects.
        //:
        //:   1 Use the 'u::isMatch' helper function that provides special
        //:     handling for the process-specific address fields.  (C-1)
        //:
        //:   2 Check that no expected result is newline-terminated.  (C-2)
        //:
        //:   3 Compare the address returned by 'operator<<' to the address of
        //:     its 'stream' argument.  (C-4)
        //
        // Testing:
        //   ostream& operator<<(ostream& stream, const OptionInfo& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'operator<<'" << endl
                          << "====================" << endl;

        if (veryVerbose) cout
                 << endl
                 << "Verify that the signatures and return types are standard."
                 << endl;
        {
            using namespace balcl;
            typedef ostream& (*operatorPtr)(ostream&, const Obj&);

            operatorPtr operatorOp  = operator<<;

            (void)operatorOp; // quash potential compiler warnings
        }

        for (bsl::size_t i = 0; i < k_NUM_SPEC_TABLE; ++i) {

            if (veryVerbose)  P(i)

            const Obj X = specTable[i];

            bsl::size_t        expectLength = bsl::strlen(expect[i]);
            bsl::ostringstream oss;

            ASSERTV(i, 0    <  expectLength);
            ASSERTV(i, '\n' != *(expect[i] + (expectLength - 1)));

            ASSERTV(i, &oss == &(oss << X));  // ACTION
            ASSERTV(i, expect[i], oss.str().c_str(),
                                  u::isMatch(expect[i], oss.str().c_str(), X));
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // EQUALITY-COMPARISON OPERATORS
        //
        // Concerns:
        //: 1 Salient Members:
        //:
        //:   1 Two objects, 'X' and 'Y', compare equal if and only if each of
        //:     their corresponding salient attributes respectively compare
        //:     equal.
        //:
        //:   2 All salient attributes participate in the comparison.
        //:     Note that this 'struct' has no non-salient members.
        //:
        //: 2 Mathematical Properties:
        //:
        //:   1 The operators provide the property of identity:
        //:     o 'true  == (X == X)'
        //:     o 'false == (X != X)'
        //:
        //:   2 The operators provide the property of commutativity:
        //:     o 'X == Y' if and only if 'Y == X'
        //:     o 'X != Y' if and only if 'Y != X'
        //:
        //:   3 Each of these two operators is the inverse of the other:
        //:     o 'X != Y' if and only if '!(X == Y)'
        //:
        //: 3 Non-modifiable objects can be compared (i.e., 'const' objects and
        //:   'const' references).
        //:
        //: 4 The two operators have standard signatures and return types.
        //
        //: 5 No memory allocation occurs as a result of comparison (e.g., the
        //:   arguments are not passed by value).
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free
        //:   equality-comparison operators defined in this component.
        //:   (C-3..4)
        //:
        //: 2 Using the table-driven technique, specify a set of unique object
        //:   values (one per row) in terms of their individual salient
        //:   attributes such that each row differs from the others with
        //:   respect to *one* salient value.  Thus, objects created from
        //:   different rows of salient values compare unequal.  By taking the
        //:   cross product of these objects, we demonstrate that a difference
        //:   in *any* individual salient value results in inequality, thus
        //:   demonstrating that each salient value contributes to the equality
        //:   comparison.  Note that objects compare equal for only those cases
        //:   of the cross product when both rows are the same.  (C-1)
        //:
        //: 3 For each test of equality, create a parallel test that checks
        //:   inequality (the inverse operator), and (when the two arguments
        //:   are different) also create a test case where the two arguments
        //:   are switched (showing commutativity).  (C-2)
        //:
        //: 4 Install a test allocator as the default allocator.  Create a test
        //:   allocator monitor object before each group of operator tests and
        //:   confirm afterwards that the 'isTotalSame' returns 'true' (showing
        //:   that no allocations occurred when exercising the operators).
        //
        // Testing:
        //   bool operator==(const OptionInfo& lhs, rhs);
        //   bool operator!=(const OptionInfo& lhs, rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "EQUALITY-COMPARISON OPERATORS" << endl
                          << "=============================" << endl;

        if (veryVerbose) cout
                 << endl
                 << "Verify that the signatures and return types are standard."
                 << endl;
        {
            using namespace balcl;
            typedef bool (*operatorPtr)(const OptionInfo&, const OptionInfo&);

            operatorPtr operatorEq = operator==;
            operatorPtr operatorNe = operator!=;

            (void)operatorEq;  // quash potential compiler warnings
            (void)operatorNe;
        }

        if (veryVerbose) cout
                            << endl
                            << "Confirm that all members contribute to results"
                            << endl;
        {
            bslma::TestAllocator         da("default", veryVeryVeryVerbose);
            bslma::DefaultAllocatorGuard dag(&da);

            const char * const   A1 = "tagA1";
            const char * const   B1 = "tagB1";

            const char * const   A2 = "nameA2";
            const char * const   B2 = "nameB2";

            const char * const   A3 = "descriptionA3";
            const char * const   B3 = "descriptionB3";

            char charLinkedVariable;
            bool  intLinkedVariable;

            const TypeInfo       A4 = TypeInfo(&charLinkedVariable);
            const TypeInfo       B4 = TypeInfo(& intLinkedVariable);

            char charDefaultValue = 'a';
            bool  intDefaultValue =  0 ;

            const OccurrenceInfo A5 = OccurrenceInfo(charDefaultValue);
            const OccurrenceInfo B5 = OccurrenceInfo( intDefaultValue);

            const struct {
                int             d_line;
                const char     *d_tag_p;
                const char     *d_name_p;
                const char     *d_desc_p;
                TypeInfo        d_typeInfo;
                OccurrenceInfo  d_occurrenceInfo;
            } DATA[] = {
                //  LINE TAG  NAME DESC TI  OI
                //  ---- ---  ---- ---- --  --
                  { L_,  A1,  A2,  A3,  A4, A5 }  // baseLine

                , { L_,  B1,  A2,  A3,  A4, A5 }
                , { L_,  A1,  B2,  A3,  A4, A5 }
                , { L_,  A1,  A2,  B3,  A4, A5 }
                , { L_,  A1,  A2,  A3,  B4, A5 }
                , { L_,  A1,  A2,  A3,  A4, B5 }
            };

            enum { k_NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int ti = 0; ti < k_NUM_DATA; ++ti) {
                const int            LINE1       = DATA[ti].d_line;
                const char * const   TAG1        = DATA[ti].d_tag_p;
                const char * const   NAME1       = DATA[ti].d_name_p;
                const char * const   DESC1       = DATA[ti].d_desc_p;
                const TypeInfo       TYPE_INFO1  = DATA[ti].d_typeInfo;
                const OccurrenceInfo OCCUR_INFO1 = DATA[ti].d_occurrenceInfo;

                if (veryVerbose) {
                    T_ P_(LINE1)
                       P_(TAG1)
                       P_(NAME1)
                       P_(DESC1)
                       P_(TYPE_INFO1)
                       P(OCCUR_INFO1)
                }

                const Obj X = { TAG1
                              , NAME1
                              , DESC1
                              , TYPE_INFO1
                              , OCCUR_INFO1
                              };

                bslma::TestAllocatorMonitor dam1(&da);

                ASSERTV(LINE1, X,   X == X );  // identity
                ASSERTV(LINE1, X, !(X != X));  // inverse

                ASSERT(dam1.isTotalSame());

                for (int tj = 0; tj < k_NUM_DATA; ++tj) {
                    const int            LINE2       = DATA[tj].d_line;
                    const char * const   TAG2        = DATA[tj].d_tag_p;
                    const char * const   NAME2       = DATA[tj].d_name_p;
                    const char * const   DESC2       = DATA[tj].d_desc_p;
                    const TypeInfo       TYPE_INFO2  = DATA[tj].d_typeInfo;
                    const OccurrenceInfo OCCUR_INFO2 = DATA[tj].
                                                              d_occurrenceInfo;
                    if (veryVerbose) {
                        T_ T_ P_(LINE2)
                              P_(TAG2)
                              P_(NAME2)
                              P_(DESC2)
                              P_(TYPE_INFO2)
                              P(OCCUR_INFO2)
                    }

                    const Obj Y = { TAG2
                                  , NAME2
                                  , DESC2
                                  , TYPE_INFO2
                                  , OCCUR_INFO2
                                  };

                    const bool EXP = ti == tj;  // expected value for equality
                                                // comparison

                    bslma::TestAllocatorMonitor dam2(&da);

                    // commutatively
                    ASSERTV(LINE1, LINE2, X, Y,  EXP == (X == Y));
                    ASSERTV(LINE1, LINE2, Y, X,  EXP == (Y == X));

                    // inverse
                    ASSERTV(LINE1, LINE2, X, Y, !EXP == (X != Y));
                    ASSERTV(LINE1, LINE2, Y, X, !EXP == (Y != X));

                    ASSERT(dam2.isTotalSame());
                }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // STRUCTURE TEST
        //
        // Concerns:
        //: 1 The 'OptionInfo' 'struct' has each of the expected data members,
        //:   and each of those data members is non-'const' and is publicly
        //:   accessible.
        //:
        //: 2 The 'OptionInfo' 'struct' has each of the expected enumerators
        //:   and each has the expected value and each has type
        //:   'OptionInfo::ArgType'.
        //:
        //: 3 An 'OptionInfo' object can be statically initialized from a
        //:   comma-separated, brace-enclosed list of member
        //:   initializers.  Initializers are matched to data members in
        //:   order of their declaration in the 'struct' and members that are
        //:   omitted from the end of that list are default initialized.
        //
        // Plan:
        //: 1 Explicitly assign a value to each of the expected data members.
        //:   (C-1)
        //:
        //: 2 Compare each of the expected enumerators to its expected value
        //:   and assign each of those values to a variable of
        //:   'OptionInfo::ArgType'.  (C-2)
        //:
        //: 3 Statically initialize a set of objects, each explicitly
        //:   specifying one or more of the object's attributes.  If
        //:   disallowed, the code does not compile.  Note that compilation may
        //:   be accompanied by warning messages.  Confirm that each object
        //:   created has the expected value.  (C-3)
        //
        // Testing:
        //   CONCERN: The 'struct' has the expected members and enumerators.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "STRUCTURE TEST" << endl
                          << "==============" << endl;

        if (veryVerbose) cout << "Test member access" << endl;
        {
            Obj mX;
            mX.d_tag         = "";
            mX.d_name        = "name";
            mX.d_description = "description";
            mX.d_typeInfo    = TypeInfo();
            mX.d_defaultInfo = OccurrenceInfo();

            Obj::ArgType Y;
            Y = Obj::e_FLAG;
            Y = Obj::e_OPTION;
            Y = Obj::e_NON_OPTION;
            (void) Y;
        }

        if (veryVerbose) cout << "Test enumerator values" << endl;
        {
            ASSERT(0 == Obj::e_FLAG);
            ASSERT(1 == Obj::e_OPTION);
            ASSERT(2 == Obj::e_NON_OPTION);
        }

        if (veryVerbose) cout << "Test unspecified attributes" << endl;
        {

            ASSERT(specTable[0].d_tag         ==  bsl::string());
            ASSERT(specTable[0].d_name        ==  bsl::string());
            ASSERT(specTable[0].d_description ==  bsl::string());
            ASSERT(specTable[0].d_typeInfo    ==  TypeInfo());
            ASSERT(specTable[0].d_defaultInfo ==  OccurrenceInfo());

            ASSERT(specTable[1].d_tag         ==  tagString);
            ASSERT(specTable[1].d_name        ==  bsl::string());
            ASSERT(specTable[1].d_description ==  bsl::string());
            ASSERT(specTable[1].d_typeInfo    ==  TypeInfo());
            ASSERT(specTable[1].d_defaultInfo ==  OccurrenceInfo());

            ASSERT(specTable[2].d_tag         ==  bsl::string("s|longTag"));
            ASSERT(specTable[2].d_name        ==  bsl::string());
            ASSERT(specTable[2].d_description ==  bsl::string());
            ASSERT(specTable[2].d_typeInfo    ==  TypeInfo());
            ASSERT(specTable[2].d_defaultInfo ==  OccurrenceInfo());

            ASSERT(specTable[3].d_tag         ==  bsl::string("s|longTag"));
            ASSERT(specTable[3].d_name        ==  bsl::string("name"));
            ASSERT(specTable[3].d_description ==  bsl::string());
            ASSERT(specTable[3].d_typeInfo    ==  TypeInfo());
            ASSERT(specTable[3].d_defaultInfo ==  OccurrenceInfo());

            ASSERT(specTable[4].d_tag         ==  bsl::string("s|longTag"));
            ASSERT(specTable[4].d_name        ==  bsl::string("name"));
            ASSERT(specTable[4].d_description ==  bsl::string("description"));
            ASSERT(specTable[4].d_typeInfo    ==  TypeInfo());
            ASSERT(specTable[4].d_defaultInfo ==  OccurrenceInfo());

            ASSERT(specTable[5].d_tag         ==  bsl::string("s|longTag"));
            ASSERT(specTable[5].d_name        ==  bsl::string("name"));
            ASSERT(specTable[5].d_description ==  bsl::string("description"));
            ASSERT(specTable[5].d_typeInfo    ==  TypeInfo(&linkedFlag));
            ASSERT(specTable[5].d_defaultInfo ==  OccurrenceInfo());

            ASSERT(specTable[6].d_tag         ==  bsl::string("s|longTag"));
            ASSERT(specTable[6].d_name        ==  bsl::string("name"));
            ASSERT(specTable[6].d_description ==  bsl::string("description"));
            ASSERT(specTable[6].d_typeInfo    ==  TypeInfo(&linkedFlag));
            ASSERT(specTable[6].d_defaultInfo ==  OccurrenceInfo::e_REQUIRED);

            ASSERT(specTable[7].d_tag         ==  bsl::string("s|longTag"));
            ASSERT(specTable[7].d_name        ==  bsl::string("name"));
            ASSERT(specTable[7].d_description ==  bsl::string("description"));
            ASSERT(specTable[7].d_typeInfo    ==  TypeInfo(&linkedFlag));
            ASSERT(specTable[7].d_defaultInfo ==  OccurrenceInfo(
                                                                defaultValue));
            ASSERT(7 + 1 == k_NUM_SPEC_TABLE);

            for (int i = 0; i < k_NUM_SPEC_TABLE; ++i) {
                P_(i) P(specTable[i])
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Ad-hoc testing.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        const Obj D = Obj();
        const Obj A = { "s|longTag"          // tag
                      , "option name"        // name
                      , "option description" // description
                      , TypeInfo()
                      , OccurrenceInfo()
                      };
        ASSERT(D == D);
        ASSERT(A == A);
        ASSERT(D != A);

        bsl::ostringstream oss;
        oss << A;
        ASSERT(!oss.str().empty());

        if (veryVerbose) {
            P(oss.str())
        }

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    ASSERT(dam.isInUseSame());

    // CONCERN: In no case does memory come from the global allocator.

    ASSERT(gam.isTotalSame());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

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
