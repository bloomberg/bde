// ball_logthrottle.cpp                                               -*-C++-*-
#include <ball_logthrottle.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_logthrottle_cpp,"$Id$ $CSID$")

                          // IMPLEMENTATION NOTES

                    // ---------------------------------
                    // BALL_LOGTHROTTLE_STREAM_CONST_IMP
                    // ---------------------------------

// 'BALL_LOGTHROTTLE_STREAM_CONST_IMP' creates a static 'bdlt::Throttle' object
// with the specified 'maxSimultaneousMessages' and 'nanosecondsPerMessage',
// and, if the specified 'severity' is less than or equal to (lower values are
// more severe') than the category threshold and the 'time debt' (see component
// doc) registered in the throttle is not execessive, executes the statement or
// block that follows the macro.  If the severity is greater than (less severe
// than) the category threshold or the time debt is excessive, the statement or
// block following the macro is not executed.  Note that all 3 arguments of
// this macro must be compile-time constants containing no floating-point
// subexpressions.
//
// If you want to access the stream within the controlled block or statement,
// use 'BALL_LOG_OUTPUT_STREAM' as defined by 'ball_log.h'.  There is no
// 'BALL_LOGTHROTTLE_OUTPUT_STREAM'.

                        // ---------------------------
                        // BALL_LOGTHROTTLE_STREAM_IMP
                        // ---------------------------

// 'BALL_LOGTHROTTLE_STREAM_IMP' creates a static 'bdlt::Throttle' object with
// the specified 'maxSimultaneousMessages' and 'nanosecondsPerMessage', and, if
// the specified 'severity' is less than or equal to (lower values are more
// severe') then the category threshold and the 'time debt' (see component doc)
// registered in the throttle is not execessive, executes the statement or
// block that follows the macro.  If the severity is greater than (less severe
// than) the category threshold or the time debt is excessive, the statement or
// block following the macro is not executed.  Note that, while 'severity' may
// be a variable, the other two arguments of this macro must be compile-time
// constants containing no floating-point subexpressions.
//
// If you want to access the stream within the controlled block or statement,
// use 'BALL_LOG_OUTPUT_STREAM' as defined by 'ball_log.h'.  There is no
// 'BALL_LOGTHROTTLE_OUTPUT_STREAM'.

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
