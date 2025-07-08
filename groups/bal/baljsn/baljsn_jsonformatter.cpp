// baljsn_jsonformatter.cpp                                           -*-C++-*-
#include <baljsn_jsonformatter.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baljsn_jsonformatter_cpp,"$Id$ $CSID$")

#include <baljsn_parserutil.h>  // for testing only

#include <bslim_printer.h>

#define STATUS()                                                           \
    {                                                                      \
        if (0 < d_stack.size()) {                                          \
            bdljsn::Json& top   = *d_stack.top().first;                    \
            bsl::cout << "top.type()     " << top.type()     << bsl::endl; \
            bsl::cout << "state          " << state          << bsl::endl; \
        }                                                                  \
        dumpStack();                                                       \
    }
#define PUSH_STACK()                                                       \
            bsl::cout << "PUSH_STACK TO: " << d_stack.size() << bsl::endl;
#define POP_STACK()                                                        \
            bsl::cout << "POP_STACK TO:  " << d_stack.size() << bsl::endl;

namespace BloombergLP {
namespace baljsn {
                          // --------------------------
                          // struct JsonFormatter_State
                          // --------------------------

// CLASS METHODS
bsl::ostream& JsonFormatter_State::print(
                                      bsl::ostream&             stream,
                                      JsonFormatter_State::Enum value,
                                      int                       level,
                                      int                       spacesPerLevel)
{
    bslim::Printer printer(&stream, level, spacesPerLevel);

    printer.start(true);
    stream << toAscii(value);
    printer.end(true);

    return stream;
}

const char *JsonFormatter_State::toAscii(JsonFormatter_State::Enum value)
{
#define CASE(X) case(e_ ## X): return #X;

    switch (value) {
      CASE(AT_START)                                                  // RETURN
      CASE(IN_ARRAY)                                                  // RETURN
      CASE(IN_OBJECT_EXPECTING_MEMBER_NAME)                           // RETURN
      CASE(IN_OBJECT_EXPECTING_MEMBER_VALUE)                          // RETURN
      CASE(IN_OBJECT_EXPECTING_MEMBER_CLOSE)                          // RETURN
      default: return "(* UNKNOWN *)";                                // RETURN
    }

#undef CASE
}

                          // -------------------
                          // class JsonFormatter
                          // -------------------

// PRIVATE MANIPULATORS

void JsonFormatter::dumpStack()
{
    if (d_stack.empty()) {
        return;                                                       // RETURN
    }

    bsl::size_t stackSize = d_stack.size();

    bsl::stack<Status> d_stackBackup;

    for (bsl::size_t i = stackSize; !d_stack.empty(); --i) {
        Status        status =  d_stack.top();
        bdljsn::Json& top    = *d_stack.top().first;
        State::Enum   state  =  d_stack.top().second;

        bsl::cout << "\t\t" << i          << "\t"
                            << top.type() << "\t"
                            << state      << bsl::endl;

        d_stackBackup.push(status);
        d_stack.pop();
    }

    BSLS_ASSERT_SAFE(stackSize == d_stackBackup.size());
    BSLS_ASSERT_SAFE(0         == d_stack      .size());

    // Restore `d_stack`;
    while (!d_stackBackup.empty()) {
        Status status = d_stackBackup.top();
        d_stack      .push(status);
        d_stackBackup.pop();
    }

    BSLS_ASSERT_SAFE(0         == d_stackBackup.size());
    BSLS_ASSERT_SAFE(stackSize == d_stack      .size());
}

// MANIPULATORS
void JsonFormatter::openObject()
{
    BSLS_ASSERT_SAFE(0 < d_stack.size());
    bdljsn::Json& top   = *d_stack.top().first;
    State::Enum   state =  d_stack.top().second;

    switch (state) {
      case State::e_AT_START: {
        BSLS_ASSERT_SAFE(top.isNull());
        BSLS_ASSERT_SAFE(1 == d_stack.size());
        top.makeObject();
        d_stack.top().second = State::e_IN_OBJECT_EXPECTING_MEMBER_NAME;
      } break;
      case State::e_IN_ARRAY: {
        BSLS_ASSERT_SAFE(top.isArray());
        bdljsn::Json newObject(bslma::AllocatorUtil::adapt(d_allocator));
        newObject.makeObject();
        top.theArray().pushBack(newObject);

        d_stack.push(bsl::make_pair(&top.theArray().back(),
                                    State::e_IN_OBJECT_EXPECTING_MEMBER_NAME));
      } break;
      case State::e_IN_OBJECT_EXPECTING_MEMBER_VALUE: {
        BSLS_ASSERT_SAFE(top.isNull());
        top.makeObject();
        d_stack.top().second = State::e_IN_OBJECT_EXPECTING_MEMBER_NAME;
      } break;
      default: {
        BSLS_ASSERT_INVOKE_NORETURN("Should not reach.");
      } break;
    }
}

void JsonFormatter::closeObject()
{
    BSLS_ASSERT_SAFE(0 < d_stack.size());

    State::Enum state = d_stack.top().second;

    if (State::e_IN_OBJECT_EXPECTING_MEMBER_CLOSE == state) {
        closeMember();   // This function pops for this state.
    }

    BSLS_ASSERT_SAFE(0 < d_stack.size());  // Check again, we may have popped.
    BSLS_ASSERT_SAFE(d_stack.top().first->isObject());

    d_stack.pop();
}

void JsonFormatter::openArray(BSLA_MAYBE_UNUSED bool formatAsEmptyArrayFlag)
{
    BSLS_ASSERT_SAFE(0 < d_stack.size());
    bdljsn::Json& top   = *d_stack.top().first;
    State::Enum   state =  d_stack.top().second;

    switch (state) {
        case State::e_AT_START: {
        BSLS_ASSERT_SAFE(1 == d_stack.size());
        BSLS_ASSERT_SAFE(top.isNull());
        top.makeArray();
        d_stack.top().second = State::e_IN_ARRAY;
      } break;
        case State::e_IN_ARRAY: {
        // Add array element (default `Json` object) to array.
        bdljsn::Json newArray(bslma::AllocatorUtil::adapt(d_allocator));
        newArray.makeArray();
        top.theArray().pushBack(newArray);

        d_stack.push(bsl::make_pair(&top.theArray().back(),
                                    State::e_IN_ARRAY));
      } break;
        case State::e_IN_OBJECT_EXPECTING_MEMBER_VALUE: {
        BSLS_ASSERT_SAFE(top.isNull());
        top.makeArray();
        d_stack.top().second = State::e_IN_ARRAY;
      } break;
      default: {
        BSLS_ASSERT_INVOKE_NORETURN("Should not reach.");
      } break;
    }
}

void JsonFormatter::closeArray(BSLA_MAYBE_UNUSED bool formatAsEmptyArrayFlag)
{
    BSLS_ASSERT_SAFE(0                 <  d_stack.size());
    BSLS_ASSERT_SAFE(State::e_IN_ARRAY == d_stack.top().second);

    d_stack.pop();
}

int JsonFormatter::openMember(const bsl::string_view& name)
{
    if (!bdlde::Utf8Util::isValid(name)) {
        return -1;
    }

    BSLS_ASSERT_SAFE(0 < d_stack.size());
    BSLS_ASSERT_SAFE(State::e_IN_OBJECT_EXPECTING_MEMBER_NAME
                                                      == d_stack.top().second);

    bdljsn::Json& top = *d_stack.top().first;

    BSLS_ASSERT_SAFE(top.isObject());

    typedef bdljsn::JsonObject::Iterator Iterator;

    bsl::pair<Iterator, bool> rv = top.theObject().insert(
                       name,
                       bdljsn::Json(bslma::AllocatorUtil::adapt(d_allocator)));

    bdljsn::Json *jp = &(*(rv.first)).second;

    d_stack.push(bsl::make_pair(jp,
                                State::e_IN_OBJECT_EXPECTING_MEMBER_VALUE));

    return 0;
}

void JsonFormatter::closeMember()
{
    BSLS_ASSERT_SAFE(0 < d_stack.size());

    State::Enum state = d_stack.top().second;

    switch (state) {
      case State::e_IN_OBJECT_EXPECTING_MEMBER_CLOSE: {
        d_stack.pop();
      } break;
      case State::e_IN_OBJECT_EXPECTING_MEMBER_NAME: {
        BSLS_ASSERT_SAFE(d_stack.top().first->isObject());
        // We are in this case because we are closing a member that had an
        // object as the value part of a name/value pair. The `closeObject`
        // for that value took it off the stack and left us with an object on
        // top.  Nothing to be done until the next `openMember` (if any) or
        // `closeObject`.
      } break;
      default: {
        BSLS_ASSERT_INVOKE_NORETURN("Should not reach.");
      } break;
    }
}

int JsonFormatter::putNullValue()
{
    BSLS_ASSERT_OPT(0 < d_stack.size());
    bdljsn::Json& top   = *d_stack.top().first;
    State::Enum   state =  d_stack.top().second;

    switch (state) {
      case State::e_AT_START: {
        BSLS_ASSERT_SAFE(1 == d_stack.size());
        BSLS_ASSERT_SAFE(top.isNull());  // Already in desired state.
        d_stack.pop();
      } break;
      case State::e_IN_ARRAY: {
        BSLS_ASSERT_SAFE(top.isArray());
        top.theArray().pushBack(bdljsn::Json(
                                    bslma::AllocatorUtil::adapt(d_allocator)));
      } break;
      case State::e_IN_OBJECT_EXPECTING_MEMBER_VALUE: {
        BSLS_ASSERT_SAFE(top.isNull());  // Already in desired state.
        d_stack.top().second = State::e_IN_OBJECT_EXPECTING_MEMBER_CLOSE;
      } break;
      case State::e_IN_OBJECT_EXPECTING_MEMBER_NAME: {
        // Unexpected state for `putNullValue`.
        return 666;                                                   // RETURN
      } break;
      default : {
        BSLS_ASSERT_OPT(false && "reached");
      } break;
    }
    return 0;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
