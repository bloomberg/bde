// baljsn_jsontokenizer.cpp                                           -*-C++-*-
#include <baljsn_jsontokenizer.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baljsn_tokenizer_cpp, "$Id$ $CSID$")

#include <bdljsn_numberutil.h>  // for testing only
#include <bdljsn_stringutil.h>  // for testing only

#include <bdlb_chartype.h>
#include <bdlde_utf8util.h>
#include <bdlsb_fixedmemoutstreambuf.h>

#include <bslim_printer.h>

#include <bsl_cstring.h>
#include <bsl_ios.h>

namespace BloombergLP {
namespace baljsn {
                              // ---------------------------
                              // class JsonTokenizer_Visitor
                              // ---------------------------

void JsonTokenizer_Visitor::operator()(const bdljsn::JsonArray& array) const
{
    BSLS_ASSERT_SAFE(0 < d_tokenizer_p->d_contextStack.size());

    JsonTokenizer::Context& context = d_tokenizer_p->d_contextStack.top();

    const bdljsn::Json *json  = context.d_json_p;

    switch (context.d_state) {
     case JT::e_JSON_START: {
            d_tokenizer_p->d_tokenList.push_back(JT::s_START_ARRAY);
            context.d_state    = JT::e_JSON_BODY;
            context.d_position = array.cbegin();
      } break;
     case JT::e_JSON_BODY: {
            BSLS_ASSERT_SAFE(context.d_position.is<JT::ElementItr>());

            JT::ElementItr elementItr = context.d_position
                                               .the<JT::ElementItr>();

            if (elementItr == array.cend()) {
                context.d_state = JT::e_JSON_END;
                json->visit<void>(*this);
                break;
            }

            if (elementItr->isArray()) {

                JsonTokenizer::Context newContext;
                newContext.d_json_p = elementItr;
                newContext.d_state  = JT::e_JSON_START;
                d_tokenizer_p->d_contextStack.push(newContext);

                elementItr->visit<void>(*this);

            } else if (elementItr->isObject()) {

                JsonTokenizer::Context newContext;
                newContext.d_json_p = elementItr;
                newContext.d_state  = JT::e_JSON_START;
                d_tokenizer_p->d_contextStack.push(newContext);

                elementItr->visit<void>(*this);
            } else {
                // Scalar
                JT::Token token;
                token.d_tokenType  = JT::e_ELEMENT_VALUE;
                token.d_tokenValue = elementItr;
                d_tokenizer_p->d_tokenList.push_back(token);
            }

            ++context.d_position.the<JT::ElementItr>();
      } break;
      case JT::e_JSON_END: {
        d_tokenizer_p->d_tokenList.push_back(JsonTokenizer::s_END_ARRAY);
        d_tokenizer_p->d_contextStack.pop();
      } break;
      default: {
        BSLS_ASSERT_OPT(false && "reachable");
      } break;
    }
}

void JsonTokenizer_Visitor::operator()(const bdljsn::JsonObject& object) const
{
    BSLS_ASSERT_SAFE(0 < d_tokenizer_p->d_contextStack.size());

    JT::Context&        context = d_tokenizer_p->d_contextStack.top();
    const bdljsn::Json *json    = context.d_json_p;

    switch (context.d_state) {
      case JT::e_JSON_START : {
            d_tokenizer_p->d_tokenList.push_back(JT::s_START_OBJECT);
            context.d_state    = JT::e_JSON_BODY;
            context.d_position = object.cbegin();
      } break;
      case JT::e_JSON_BODY: {
            BSLS_ASSERT_SAFE(context.d_position.is<JT::MemberItr>());

            JT::MemberItr memberItr = context.d_position
                                             .the<JT::MemberItr>();

            if (memberItr == object.cend()) {
                context.d_state = JT::e_JSON_END;
                json->visit<void>(*this);
                break;
            }

            JT::Token token;
            token.d_tokenType  = JT::e_ELEMENT_NAME;
            token.d_tokenValue = bsl::string_view(memberItr->first.data(),
                                                  memberItr->first.length());
            d_tokenizer_p->d_tokenList.push_back(token);

            /*  */ if (memberItr->second.isArray()) {
                JT::Context newContext;
                newContext.d_json_p = &memberItr->second;
                newContext.d_state  = JT::e_JSON_START;
                d_tokenizer_p->d_contextStack.push(newContext);

                memberItr->second.visit<void>(*this);

            } else if (memberItr->second.isObject()) {

                JT::Context newContext;
                newContext.d_json_p = &memberItr->second;
                newContext.d_state  = JT::e_JSON_START;
                d_tokenizer_p->d_contextStack.push(newContext);

                memberItr->second.visit<void>(*this);

            } else {
                // visit the scalar (leaf).

                JT::Token token;
                token.d_tokenType  = JT::e_ELEMENT_VALUE;
                token.d_tokenValue = &memberItr->second;
                d_tokenizer_p->d_tokenList.push_back(token);
            }

            ++context.d_position.the<JT::MemberItr>();
      } break;
      case JT::e_JSON_END: {
        d_tokenizer_p->d_tokenList.push_back(JT::s_END_OBJECT);
        d_tokenizer_p->d_contextStack.pop();
      } break;
      default: {
        BSLS_ASSERT_OPT(false && "reachable");
      } break;
    }
}

                              // -------------------
                              // class JsonTokenizer
                              // -------------------

// DATA
const JsonTokenizer::Token
      JsonTokenizer::s_BEGIN        = { e_BEGIN,        TokenValue() };
const JsonTokenizer::Token
      JsonTokenizer::s_START_OBJECT = { e_START_OBJECT, TokenValue() };
const JsonTokenizer::Token
      JsonTokenizer::s_END_OBJECT   = { e_END_OBJECT,   TokenValue() };
const JsonTokenizer::Token
      JsonTokenizer::s_START_ARRAY  = { e_START_ARRAY,  TokenValue() };
const JsonTokenizer::Token
      JsonTokenizer::s_END_ARRAY    = { e_END_ARRAY,    TokenValue() };
const JsonTokenizer::Token
      JsonTokenizer::s_ERROR        = { e_ERROR,        TokenValue() };

// PRIVATE ACCESSORS
void JsonTokenizer::printTokens() const
{
    typedef TokenList::const_iterator ConstItr;

    int i = 0;
    for (ConstItr cur  = d_tokenList.cbegin(),
                  end  = d_tokenList.cend();
                  end != cur; ++cur, ++i) {
        bsl::cout << bsl::right <<  bsl::setw( 5) << i                 << " "
                  << bsl::left  <<  bsl::setw(40) << cur->d_tokenType  << "|";

        /* */  if (cur->d_tokenValue.is<ElementName>()) {
                    bsl::cout << cur->d_tokenValue.the<ElementName>()
                              << bsl::endl;
        } else if (cur->d_tokenValue.is<ElementValue>()) {
                    const bdljsn::Json *value = cur->d_tokenValue
                                                  .the<ElementValue>();
                    if (value) {
                        bsl::cout << *value         << bsl::endl;
                    } else {
                        bsl::cout << "NULL pointer" << bsl::endl;
                    }
        }
        bsl::cout << bsl::endl;
    }
}

// CLASS METHODS

bsl::ostream& JsonTokenizer::print(bsl::ostream&            stream,
                                   JsonTokenizer::TokenType value,
                                   int                      level,
                                   int                      spacesPerLevel)
{
    bslim::Printer printer(&stream, level, spacesPerLevel);

    printer.start(true);
    stream << toAscii(value);
    printer.end(true);

    return stream;
}

const char *JsonTokenizer::toAscii(JsonTokenizer::TokenType value)
{
#define CASE(X) case(e_ ## X): return #X;

    switch (value) {
      CASE(BEGIN)
      CASE(ELEMENT_NAME)
      CASE(START_OBJECT)
      CASE(END_OBJECT)
      CASE(START_ARRAY)
      CASE(END_ARRAY)
      CASE(ELEMENT_VALUE)
      CASE(ERROR)
      default: return "(* UNKNOWN *)";                                // RETURN
    }

#undef CASE
}

// CREATORS
JsonTokenizer::~JsonTokenizer()
{
}

// MANIPULATORS
int JsonTokenizer::advanceToNextToken()
{
    BSLS_ASSERT_SAFE(0 < d_tokenList.size());

    d_tokenList.pop_front();

    if (0 < d_tokenList.size()) {
        return 0;   // We entered with more than one token on the stack.
    } else {
        // The stack is empty.
        int rc = getNextToken();
        if (0 == rc) {
            BSLS_ASSERT_SAFE(0 < d_tokenList.size());
            return 0;
        } else {
            return 99;  // No more token available.
        }
    }

    BSLA_UNREACHABLE;
}

int JsonTokenizer::reset(const bdljsn::Json *json)
{
    BSLS_ASSERT(json);
    BSLS_ASSERT(json->isArray() || json->isObject());

    // Erase existing state
    d_tokenList.clear();
    d_contextStack = ContextStack();

    // Prime the context for (new) `json`.
    Context context;
    context.d_json_p = json;
    context.d_state  = json->isArray()  ? e_JSON_START :
                       json->isObject() ? e_JSON_START :
                                          e_JSON_BODY  ;
    d_contextStack.push(context);
    d_tokenList.push_back(JsonTokenizer::s_BEGIN);

    return 0;
}

}  // close package namespace
}  // close enterprise namespace

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
