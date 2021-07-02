// balxml_configschema.t.cpp                                          -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balxml_configschema.h>

#include <balxml_decoder.h>
#include <balxml_decoderoptions.h>
#include <balxml_errorinfo.h>
#include <balxml_minireader.h>

#include <bdlsb_fixedmeminstreambuf.h>

#include <bslim_testutil.h>

#include <bsls_assert.h>

#include <s_baltst_basicschema.h>
#include <s_baltst_basicschemautil.h>

#include <bsl_cstring.h>     // strlen()
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under test provides a textual representation of an XML Schema
// Definition for the XML codec's encoding options, decoding options, and
// encoding style.  The content of this file needs to be synchronized with the
// structure represented by 'balxml::EncoderOptions',
// 'balxml::DecoderOptions', and 'balxml::EncodingStyle'.
//
// Global Concerns:
//: o The content of 'balxml::ConfigSchema::TEXT' is a valid XML Schema
//:   Definition.
//:
//: o Changes to the content of 'balxml::ConfigSchema::TEXT' are
//:   backward-compatible.
// ----------------------------------------------------------------------------
// CLASS DATA
// [ 2] balxml::ConfigSchema::TEXT
//
// FREE OPERATORS
// [ 1] balxml::operator<<(ostream&, const balxml::ConfigSchema&);
// ----------------------------------------------------------------------------
// [ 2] CONCERN: The structure of 'TEXT' is equal to 'balxml.xsd'
// [ 2] CONCERN: Changes to 'TEXT' are backward-compatible.
// ----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {
using namespace BloombergLP;

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        bsl::cout << "Error " __FILE__ "(" << line << "): " << message
                  << "    (failed)" << bsl::endl;

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
//                   NAMESPACE-SCOPE ENTITIES FOR TESTING
// ----------------------------------------------------------------------------

#define ASSERT_EQ(X,Y) ASSERTV(X,Y,(X) == (Y))

namespace {
namespace u {
using namespace BloombergLP;

                              // ===============
                              // struct TestUtil
                              // ===============

struct TestUtil {
    // This utility 'struct' provides a namespace for a suite of functions for
    // loading the *expected*, structural representation of
    // 'balxml::ConfigSchema::TEXT' into a 's_baltst::BasicSchema'.

    // TYPES
    typedef s_baltst::BasicSchemaAnnotationElement   AnnotationElement;
    typedef s_baltst::BasicSchemaComplexTypeElement  ComplexTypeElement;
    typedef s_baltst::BasicSchemaElement             Element;
    typedef s_baltst::BasicSchemaRestrictionElement  RestrictionElement;
    typedef s_baltst::BasicSchema                    Schema;
    typedef s_baltst::BasicSchemaUtil                SchemaUtil;
    typedef s_baltst::BasicSchemaSequenceElement     SequenceElement;
    typedef s_baltst::BasicSchemaSimpleTypeElement   SimpleTypeElement;

    // CLASS METHODS
    static void loadExpectedSchema(Schema *schema);
        // Load the expected structural representation of
        // 'balxml::ConfigSchema::TEXT' into the specified 'schema'.

    static void loadEncodingStyle(SimpleTypeElement *encodingStyle);
        // Load the expected structural representation of the 'EncodingStyle'
        // type defined in 'balxml::ConfigSchema::TEXT' into the specified
        // 'encodingStyle'.

    static void loadEncoderOptions(ComplexTypeElement *encoderOptions);
        // Load the expected structural representation of the 'EncoderOptions'
        // type defined in 'balxml::ConfigSchema::TEXT' into the specified
        // 'encoderOptions'.

    static void loadDecoderOptions(ComplexTypeElement *decoderOptions);
        // Load the expected structural representation of the 'DecoderOptions'
        // type defined in 'balxml::ConfigSchema::TEXT' into the specified
        // 'decoderOptions'.
};

// ============================================================================
//                    NAMESPACE-SCOPE ENTITY DEFNIITIONS
// ----------------------------------------------------------------------------

                              // ---------------
                              // struct TestUtil
                              // ---------------

// CLASS METHODS
void TestUtil::loadExpectedSchema(s_baltst::BasicSchema *schema)
{
    schema->package() = "balxml";
    schema->elementFormDefault() = "qualified";

    SimpleTypeElement encodingStyle;
    loadEncodingStyle(&encodingStyle);
    SchemaUtil::append(schema, encodingStyle);

    ComplexTypeElement encoderOptions;
    loadEncoderOptions(&encoderOptions);
    SchemaUtil::append(schema, encoderOptions);

    ComplexTypeElement decoderOptions;
    loadDecoderOptions(&decoderOptions);
    SchemaUtil::append(schema, decoderOptions);
}

void TestUtil::loadEncodingStyle(SimpleTypeElement *encodingStyle)
{
    encodingStyle->name() = "EncodingStyle";
    encodingStyle->preserveEnumOrder() = "1";

    RestrictionElement restriction;
    restriction.base() = "xs:string";
    SchemaUtil::appendEnumeration(&restriction, "COMPACT");
    SchemaUtil::appendEnumeration(&restriction, "PRETTY");
    SchemaUtil::appendEnumeration(&restriction, "BAEXML_COMPACT");
    SchemaUtil::appendEnumeration(&restriction, "BAEXML_PRETTY");
    SchemaUtil::append(encodingStyle, restriction);
}

void TestUtil::loadEncoderOptions(ComplexTypeElement *encoderOptions)
{
    encoderOptions->name() = "EncoderOptions";

    SequenceElement sequence;

    {
        Element objectNamespace;
        objectNamespace.name()                     = "ObjectNamespace";
        objectNamespace.type()                     = "xs:string";
        objectNamespace.minOccurs()                = "0";
        objectNamespace.maxOccurs()                = "1";
        objectNamespace.allowsDirectManipulation() = "0";
        SchemaUtil::append(&sequence, objectNamespace);
    }

    {
        Element schemaLocation;
        schemaLocation.name()                     = "SchemaLocation";
        schemaLocation.type()                     = "xs:string";
        schemaLocation.minOccurs()                = "0";
        schemaLocation.maxOccurs()                = "1";
        schemaLocation.allowsDirectManipulation() = "0";
        SchemaUtil::append(&sequence, schemaLocation);
    }

    {
        Element tag;
        tag.name()                     = "Tag";
        tag.type()                     = "xs:string";
        tag.minOccurs()                = "0";
        tag.maxOccurs()                = "1";
        tag.allowsDirectManipulation() = "0";
        SchemaUtil::append(&sequence, tag);
    }

    {
        Element formattingMode;
        formattingMode.name()            = "FormattingMode";
        formattingMode.type()            = "xs:int";
        formattingMode.minOccurs()       = "0";
        formattingMode.maxOccurs()       = "1";
        formattingMode.defaultValue()    = "0";
        formattingMode.cppdefault()      = "bdlat_FormattingMode::e_DEFAULT";
        formattingMode.cpptype()         = "int";
        formattingMode.allocatesMemory() = "0";
        formattingMode.allowsDirectManipulation() = "0";
        SchemaUtil::append(&sequence, formattingMode);
    }

    {
        Element initialIndentLevel;
        initialIndentLevel.name()                     = "InitialIndentLevel";
        initialIndentLevel.type()                     = "xs:int";
        initialIndentLevel.minOccurs()                = "0";
        initialIndentLevel.maxOccurs()                = "1";
        initialIndentLevel.defaultValue()             = "0";
        initialIndentLevel.allowsDirectManipulation() = "0";
        SchemaUtil::append(&sequence, initialIndentLevel);
    }

    {
        Element spacesPerLevel;
        spacesPerLevel.name()                     = "SpacesPerLevel";
        spacesPerLevel.type()                     = "xs:int";
        spacesPerLevel.minOccurs()                = "0";
        spacesPerLevel.maxOccurs()                = "1";
        spacesPerLevel.defaultValue()             = "4";
        spacesPerLevel.allowsDirectManipulation() = "0";
        SchemaUtil::append(&sequence, spacesPerLevel);
    }

    {
        Element wrapColumn;
        wrapColumn.name()                     = "WrapColumn";
        wrapColumn.type()                     = "xs:int";
        wrapColumn.minOccurs()                = "0";
        wrapColumn.maxOccurs()                = "1";
        wrapColumn.defaultValue()             = "80";
        wrapColumn.allowsDirectManipulation() = "0";
        SchemaUtil::append(&sequence, wrapColumn);
    }

    {
        Element maxDecimalTotalDigits;
        maxDecimalTotalDigits.name()      = "MaxDecimalTotalDigits";
        maxDecimalTotalDigits.type()      = "xs:int";
        maxDecimalTotalDigits.minOccurs() = "0";
        maxDecimalTotalDigits.maxOccurs() = "1";
        maxDecimalTotalDigits.allowsDirectManipulation() = "0";
        SchemaUtil::append(&sequence, maxDecimalTotalDigits);
    }

    {
        Element maxDecimalFractionDigits;
        maxDecimalFractionDigits.name()      = "MaxDecimalFractionDigits";
        maxDecimalFractionDigits.type()      = "xs:int";
        maxDecimalFractionDigits.minOccurs() = "0";
        maxDecimalFractionDigits.maxOccurs() = "1";
        maxDecimalFractionDigits.allowsDirectManipulation() = "0";
        SchemaUtil::append(&sequence, maxDecimalFractionDigits);
    }

    {
        Element significantDoubleDigits;
        significantDoubleDigits.name()      = "SignificantDoubleDigits";
        significantDoubleDigits.type()      = "xs:int";
        significantDoubleDigits.minOccurs() = "0";
        significantDoubleDigits.maxOccurs() = "1";
        significantDoubleDigits.allowsDirectManipulation() = "0";
        SchemaUtil::append(&sequence, significantDoubleDigits);
    }

    {
        Element encodingStyle;
        encodingStyle.name()                     = "EncodingStyle";
        encodingStyle.type()                     = "EncodingStyle";
        encodingStyle.minOccurs()                = "0";
        encodingStyle.defaultValue()             = "COMPACT";
        encodingStyle.allowsDirectManipulation() = "0";
        SchemaUtil::append(&sequence, encodingStyle);
    }

    {
        Element allowControlCharacters;
        allowControlCharacters.name()            = "AllowControlCharacters";
        allowControlCharacters.type()            = "xs:boolean";
        allowControlCharacters.minOccurs()       = "0";
        allowControlCharacters.maxOccurs()       = "1";
        allowControlCharacters.defaultValue()    = "false";
        allowControlCharacters.allocatesMemory() = "0";
        allowControlCharacters.allowsDirectManipulation() = "0";
        SchemaUtil::append(&sequence, allowControlCharacters);
    }

    {
        Element outputXmlHeader;
        outputXmlHeader.name()                     = "OutputXMLHeader";
        outputXmlHeader.type()                     = "xs:boolean";
        outputXmlHeader.minOccurs()                = "0";
        outputXmlHeader.maxOccurs()                = "1";
        outputXmlHeader.defaultValue()             = "true";
        outputXmlHeader.allocatesMemory()          = "0";
        outputXmlHeader.allowsDirectManipulation() = "0";
        SchemaUtil::append(&sequence, outputXmlHeader);
    }

    {
        Element outputXsiAlias;
        outputXsiAlias.name()                     = "OutputXSIAlias";
        outputXsiAlias.type()                     = "xs:boolean";
        outputXsiAlias.minOccurs()                = "0";
        outputXsiAlias.maxOccurs()                = "1";
        outputXsiAlias.defaultValue()             = "true";
        outputXsiAlias.allocatesMemory()          = "0";
        outputXsiAlias.allowsDirectManipulation() = "0";
        SchemaUtil::append(&sequence, outputXsiAlias);
    }

    {
        Element datetimeFractionalSecondPrecision;
        datetimeFractionalSecondPrecision.name() =
            "DatetimeFractionalSecondPrecision";
        datetimeFractionalSecondPrecision.type()         = "xs:int";
        datetimeFractionalSecondPrecision.minOccurs()    = "0";
        datetimeFractionalSecondPrecision.maxOccurs()    = "1";
        // As of October 15 2020, the value of 'defaultValue' is 6 in
        // 'balxml.xsd', but must be 3 in 'balxml_configschema' in order to
        // maintain backwards-compatibility with existing clients.  Please
        // consider backwards-compatibility concerns before changing this
        // value.
        datetimeFractionalSecondPrecision.defaultValue() = "3";
        datetimeFractionalSecondPrecision.allowsDirectManipulation() = "0";
        SchemaUtil::append(&sequence, datetimeFractionalSecondPrecision);
    }

    {
        Element useZAbbreviationForUtc;
        useZAbbreviationForUtc.name()            = "UseZAbbreviationForUtc";
        useZAbbreviationForUtc.type()            = "xs:boolean";
        useZAbbreviationForUtc.minOccurs()       = "0";
        useZAbbreviationForUtc.maxOccurs()       = "1";
        useZAbbreviationForUtc.defaultValue()    = "false";
        useZAbbreviationForUtc.allocatesMemory() = "0";
        useZAbbreviationForUtc.allowsDirectManipulation() = "0";
        SchemaUtil::append(&sequence, useZAbbreviationForUtc);
    }

    SchemaUtil::append(encoderOptions, sequence);
}

void TestUtil::loadDecoderOptions(ComplexTypeElement *decoderOptions)
{
    decoderOptions->name() = "DecoderOptions";

    SequenceElement sequence;

    {
        Element maxDepth;
        maxDepth.name()                     = "MaxDepth";
        maxDepth.type()                     = "xs:int";
        maxDepth.minOccurs()                = "0";
        maxDepth.maxOccurs()                = "1";
        maxDepth.defaultValue()             = "32";
        maxDepth.allowsDirectManipulation() = "0";
        SchemaUtil::append(&sequence, maxDepth);
    }

    {
        Element formattingMode;
        formattingMode.name()            = "FormattingMode";
        formattingMode.type()            = "xs:int";
        formattingMode.minOccurs()       = "0";
        formattingMode.maxOccurs()       = "1";
        formattingMode.defaultValue()    = "0";
        formattingMode.cppdefault()      = "bdlat_FormattingMode::e_DEFAULT";
        formattingMode.cpptype()         = "int";
        formattingMode.allocatesMemory() = "0";
        formattingMode.allowsDirectManipulation() = "0";
        SchemaUtil::append(&sequence, formattingMode);
    }

    {
        Element skipUnknownElements;
        skipUnknownElements.name()            = "SkipUnknownElements";
        skipUnknownElements.type()            = "xs:boolean";
        skipUnknownElements.minOccurs()       = "0";
        skipUnknownElements.maxOccurs()       = "1";
        skipUnknownElements.defaultValue()    = "true";
        skipUnknownElements.allocatesMemory() = "0";
        skipUnknownElements.allowsDirectManipulation() = "0";
        SchemaUtil::append(&sequence, skipUnknownElements);
    }

    {
        Element validateInputIsUtf8;
        validateInputIsUtf8.name()            = "ValidateInputIsUtf8";
        validateInputIsUtf8.type()            = "xs:boolean";
        validateInputIsUtf8.minOccurs()       = "0";
        validateInputIsUtf8.maxOccurs()       = "1";
        validateInputIsUtf8.defaultValue()    = "false";
        validateInputIsUtf8.allowsDirectManipulation() = "0";
        SchemaUtil::append(&sequence, validateInputIsUtf8);
    }

    SchemaUtil::append(decoderOptions, sequence);
}

}  // close namespace u
}  // close unnamed namespace

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    using namespace BloombergLP;

    const int test        = argc > 1 ? bsl::atoi(argv[1]) : 0;
    const int verbose     = argc > 2;
    const int veryVerbose = argc > 3;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    typedef balxml::ConfigSchema Obj;

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // STRUCTURAL CONTENT OF 'TEXT'
        //   Ensure that the structural content of 'TEXT' is equal to what we
        //   expect.
        //
        //   **NOTE** Whenever this test case fails due to a change to 'TEXT',
        //   update the test driver to match the new content, and consider
        //   whether all of those changes are backward-compatible.  This test
        //   case is designed to catch spurious, accidental, or unnoticed
        //   changes to 'TEXT'.
        //
        // Concerns:
        //: 1 Incidental changes to 'balxml::ConfigSchema::TEXT' (likely due to
        //:   careless code-generation) do not go unchecked.
        //:
        //: 2 The content of 'balxml::ConfigSchema::TEXT' is valid XML.
        //
        // Plan:
        //: 1 Parse the content of 'balxml::ConfigSchema::TEXT' into a
        //:   's_baltst::BasicSchema', 'S', which is a rough, structural
        //:   representation of an XML Schema Definition.
        //:
        //: 2 Verify that the structure of 'S' is equal to the expected
        //:   structure (modulo annotations), where the "expected structure"
        //:   refers to an encoding of the structure of 'S' in this test driver
        //:   that does not get automatically modified by code generation.
        //
        // Testing:
        //   balxml::ConfigSchema::TEXT
        // --------------------------------------------------------------------

        if (verbose) {
            bsl::cout << bsl::endl
                      << "STRUCTURAL CONTENT OF 'TEXT'" << bsl::endl
                      << "============================" << bsl::endl;
        }

        // First, initialize a XML decoder.

        balxml::DecoderOptions options;
        balxml::MiniReader     reader;
        balxml::Decoder        decoder(&options, &reader);

        // Then, create a stream buffer that contains the content of 'TEXT'.

        const bslstl::StringRef configSchemaText(balxml::ConfigSchema::TEXT);

        bdlsb::FixedMemInStreamBuf configSchemaStreamBuf(
            configSchemaText.data(), configSchemaText.length());

        // Next, parse the content of the stream buffer into a
        // 's_baltst::BasicSchema', which provides a rough, structural
        // representation of an XML Schema Definition.  Note that
        // 's_baltst::BasicSchema' is not sufficient to represent an XML Schema
        // Defition in a production environment.

        s_baltst::BasicSchema configSchema;
        int rc = decoder.decode(&configSchemaStreamBuf, &configSchema);
        BSLS_ASSERT(0 == rc);

        // Next, load the content that we expect the parsed schema to have into
        // another schema, so that we can compare the two for structural
        // equality (modulo annotations.)

        s_baltst::BasicSchema expectedSchema;
        u::TestUtil::loadExpectedSchema(&expectedSchema);

        // Finally, assert that the content of 'TEXT' is structural equal
        // (modulo annotations) to the content that we expect.

        typedef s_baltst::BasicSchemaUtil SchemaUtil;
        ASSERT(
             SchemaUtil::areEqualUpToAnnotation(configSchema, expectedSchema));
        if (veryVerbose && !SchemaUtil::areEqualUpToAnnotation(
                               configSchema, expectedSchema)) {
            bsl::cerr << "\nCONFIG SCHEMA"
                      << "\n=============\n\n";
            configSchema.print(bsl::cerr);

            bsl::cerr << "\n";

            bsl::cerr << "\nEXPECTED SCHEMA"
                      << "\n===============\n\n";
            expectedSchema.print(bsl::cerr);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // Basic Attribute Test:
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << bsl::endl
                               << "Basic Attribute Test" << bsl::endl
                               << "=====================" << bsl::endl;

        if (verbose) bsl::cout << "\n Check streaming " << bsl::endl;
        {
            Obj mX;
            bsl::ostringstream stream;
            stream << mX;
            if (verbose) {
                P(stream.str());
            }
        }
      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }
    return testStatus;
}

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
