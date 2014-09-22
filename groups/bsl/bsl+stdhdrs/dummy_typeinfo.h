// These dummy headers are trying to accomplish the following: bde_build.pl
// automatically copy all the .h files to the include directory for a non
// standard compliant package (bsl+stdhdrs in this case).

// However, this process is complicated by two problems.  First, there are a
// lot of headers that does not end with .h here, such as <limits> and
// <limits.SUNWCCh>.  This problem is solved by creating a script 'cphdrs.pl'
// that, given a header file name (provided by bde_build.pl), it will copy that
// file and also its corresponding without the .h and with the .SUNWCCh.

// The second problem is that when a header does not have a
// corresponding .h file, such as <algorithm>, there is no way to make
// bde_build.pl provide this name unless we create this dummy header file.

#error "This dummy file is used by build script and should not be included"

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
