// bsla_nullterminated.cpp                                            -*-C++-*-
#include <bsla_nullterminated.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

                            // Implementation Note

// This feature is disabled on Solaris g++ because on that compiler, the only
// acceptable terminator is 'nullptr', even in C++03 builds, and according to
// http://cppreference.com, 'nullptr' is not available until C++11.  So our
// documenatation here is telling clients to terminate argument lists with
// 'NULL', which won't work on Solaris with g++.
//
// Perhaps after we are no longer supporting C++03, we can change the
// documentaion in this component to instruct the clients to terminate argument
// lists with 'nullptr' and re-enable the feature for Solaris g++.

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
