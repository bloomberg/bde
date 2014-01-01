//@AUTHOR: Arthur Chiu (achiu21)
//
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
// Copyright (C) 2013 Bloomberg Finance L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
