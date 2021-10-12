// balst_stacktraceprinter.h                                          -*-C++-*-
#ifndef INCLUDED_BALST_STACKTRACEPRINTER
#define INCLUDED_BALST_STACKTRACEPRINTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an object for streaming the current stack trace.
//
//@CLASSES:
//  balst::StackTracePrinter: object for streaming the current stack trace
//
//@SEE_ALSO: balst_stacktraceprintutil
//
//@DESCRIPTION: This component provides a mechanism,
// 'balst::StackTracePrinter', that when streamed (using a streaming operator)
// renders a description of the current call-stack to the supplied stream.
// This class can, at construction, be passed up to 3 optional arguments to
// drive the details with which the stack trace is to be performed.  Not all
// properties of a stack trace are printed on all platforms because the set of
// properties describing a stack trace that are obtainable varies according to
// both the platform and build parameters.  For example, on Solaris and HP-UX,
// source file names and line numbers are not provided.  Function names and
// addresses are provided on all platforms.  Streaming a
// 'balst::StackTracePrinter' always prints a description of the stack of the
// calling thread.
//
///Usage
///-----
// In this section we show the intended usage of this component.
//
///Example 1: Streaming to BALL
/// - - - - - - - - - - - - - -
// First, we define a recursive function 'recurseAndPrintStack' that recurses 4
// times, then calls '<< StackTracePrinter()' to obtain a stack trace and print
// it to 'BALL_LOG_FATAL':
//..
//  BALL_LOG_SET_NAMESPACE_CATEGORY("MY.CATEGORY");
//
//  void recurseAndStreamStackDefault()
//      // Recurse 4 times and print a stack trace to 'BALL_LOG_FATAL'.
//  {
//      static int recurseCount = 0;
//
//      if (recurseCount++ < 4) {
//          recurseAndStreamStackDefault();
//      }
//      else {
//          BALL_LOG_FATAL << balst::StackTracePrinter();
//      }
//  }
//..
// which, on Linux, produces the output:
//..
//  02SEP2021_21:55:58.619290 21325 140602555295616 FATAL /bb/.../balst_stacktr
//  aceprinter.t.cpp 733 UNINITIALIZED_LOGGER_MANAGER
//  (0): recurseAndStreamStackDefault()+0xcf at 0x408acc source:balst_stacktrac
//  eprinter.t.cpp:733 in balst_stacktraceprinter.t
//  (1): recurseAndStreamStackDefault()+0x2a at 0x408a27 source:balst_stacktrac
//  eprinter.t.cpp:735 in balst_stacktraceprinter.t
//  (2): recurseAndStreamStackDefault()+0x2a at 0x408a27 source:balst_stacktrac
//  eprinter.t.cpp:735 in balst_stacktraceprinter.t
//  (3): recurseAndStreamStackDefault()+0x2a at 0x408a27 source:balst_stacktrac
//  eprinter.t.cpp:735 in balst_stacktraceprinter.t
//  (4): recurseAndStreamStackDefault()+0x2a at 0x408a27 source:balst_stacktrac
//  eprinter.t.cpp:735 in balst_stacktraceprinter.t
//  (5): main+0x1a7 at 0x408ff8 source:balst_stacktraceprinter.t.cpp:901 in bal
//  st_stacktraceprinter.t
//  (6): __libc_start_main+0xf5 at 0x7fe0943de495 in /lib64/libc.so.6
//  (7): --unknown-- at 0x4074c5 in balst_stacktraceprinter.t
//..
// Note that long lines of output here have been hand-wrapped to fit into
// comments in this 79-column source file.  Also note that if the full path of
// the executable or library is too long, only the basename will be displayed
// by the facility, while if it is short, then the full path will be displayed.
//
// Then we define a similar recursive function, except that when we construct
// the 'StackTracePrinter' object, we pass 2 to the 'maxFrames' argument,
// indicating, for some reason, that we want to see only the top two stack
// frames:
//..
//  void recurseAndStreamStackMaxFrames2()
//      // Recurse 4 times and print a stack trace to 'BALL_LOG_FATAL'.
//  {
//      static int recurseCount = 0;
//
//      if (recurseCount++ < 4) {
//          recurseAndStreamStackMaxFrames2();
//      }
//      else {
//          BALL_LOG_FATAL << balst::StackTracePrinter(2);
//      }
//  }
//..
// which produces the output:
//..
//  02SEP2021_21:55:58.624623 21325 140602555295616 FATAL /bb/.../balst_stacktr
//  aceprinter.t.cpp 773 UNINITIALIZED_LOGGER_MANAGER
//  (0): recurseAndStreamStackMaxFrames2()+0xcf at 0x408be1 source:balst_stackt
//  raceprinter.t.cpp:773 in balst_stacktraceprinter.t
//  (1): recurseAndStreamStackMaxFrames2()+0x2a at 0x408b3c source:balst_stackt
//  raceprinter.t.cpp:775 in balst_stacktraceprinter.t
//..
// Now, we define another similar recursive function, except that when we
// construct the 'StackTracePrinter' object, we default 'maxFrames' to a large
// value by passing it -1, and turn off demangling by passing 'false' to the
// 'damanglingPreferredFlag' argument:
//..
//  void recurseAndStreamStackNoDemangle()
//      // Recurse 4 times and print a stack trace to 'BALL_LOG_FATAL'.
//  {
//      static int recurseCount = 0;
//
//      if (recurseCount++ < 4) {
//          recurseAndStreamStackNoDemangle();
//      }
//      else {
//          BALL_LOG_FATAL << balst::StackTracePrinter(-1, false);
//      }
//  }
//..
// which produces the output:
//..
//  02SEP2021_21:55:58.636414 21325 140602555295616 FATAL /bb/.../balst_stacktr
//  aceprinter.t.cpp 798 UNINITIALIZED_LOGGER_MANAGER
//  (0): _Z31recurseAndStreamStackNoDemanglev+0xcf at 0x408cf6 source:balst_sta
//  cktraceprinter.t.cpp:798 in balst_stacktraceprinter.t
//  (1): _Z31recurseAndStreamStackNoDemanglev+0x2a at 0x408c51 source:balst_sta
//  cktraceprinter.t.cpp:800 in balst_stacktraceprinter.t
//  (2): _Z31recurseAndStreamStackNoDemanglev+0x2a at 0x408c51 source:balst_sta
//  cktraceprinter.t.cpp:800 in balst_stacktraceprinter.t
//  (3): _Z31recurseAndStreamStackNoDemanglev+0x2a at 0x408c51 source:balst_sta
//  cktraceprinter.t.cpp:800 in balst_stacktraceprinter.t
//  (4): _Z31recurseAndStreamStackNoDemanglev+0x2a at 0x408c51 source:balst_sta
//  cktraceprinter.t.cpp:800 in balst_stacktraceprinter.t
//  (5): main+0x1b1 at 0x409002 source:balst_stacktraceprinter.t.cpp:903 in bal
//  st_stacktraceprinter.t
//  (6): __libc_start_main+0xf5 at 0x7fe0943de495 in /lib64/libc.so.6
//  (7): --unknown-- at 0x4074c5 in balst_stacktraceprinter.t
//..
// Finally, we define another similar recursive function, except that we pass
// default values to the first 2 arguments of the 'StackTracePrinter' and pass
// 5 to the third 'additionalIgnoreFrames' argument.  This indicates a number
// of frames from the top of the stack to be ignored, which may be desired if
// the caller wants to do the streaming from within their own stack trace
// facility, in which case the top couple of frames would be within that stack
// trace facility, and unwanted and distracting for clients of that facility:
//..
//  void recurseAndStreamStackAddIgnore5()
//      // Recurse 4 times and print a stack trace to 'BALL_LOG_FATAL'.
//  {
//      static int recurseCount = 0;
//
//      if (recurseCount++ < 4) {
//          recurseAndStreamStackAddIgnore5();
//      }
//      else {
//          BALL_LOG_FATAL << balst::StackTracePrinter(-1, true, 5);
//      }
//  }
//..
// which produces the output:
//..
//  02SEP2021_21:55:58.647501 21325 140602555295616 FATAL /bb/.../balst_stacktr
//  aceprinter.t.cpp 836 UNINITIALIZED_LOGGER_MANAGER
//  (0): main+0x1b6 at 0x409007 source:balst_stacktraceprinter.t.cpp:904 in bal
//  st_stacktraceprinter.t
//  (1): __libc_start_main+0xf5 at 0x7fe0943de495 in /lib64/libc.so.6
//  (2): --unknown-- at 0x4074c5 in balst_stacktraceprinter.t
//..

#include <balscm_version.h>

#include <bsls_compilerfeatures.h>

#include <bsl_ostream.h>

namespace BloombergLP {
namespace balst {

                           // =======================
                           // class StackTracePrinter
                           // =======================

class StackTracePrinter {
    // This 'class' defines an object that, if streamed to a 'bsl::ostream',
    // will output a stack trace to that stream.

    // PRIVATE TYPES
    enum { k_DEFAULT_MAX_FRAMES = 1024 };

    // DATA
    int         d_maxFrames;       // maximum # of frames to show

    bool        d_demanglingPreferredFlag;
                                   // whether symbols are to be demangled

    int         d_additionalIgnoreFrames;
                                   // the number of the frames on the top of
                                   // the stack that are not to be displayed,
                                   // in addition to
                                   // 'bsls::StackAddressUtil::k_IGNORE_FRAMES'

    // FRIENDS
    friend bsl::ostream& operator<<(bsl::ostream&, const StackTracePrinter&);

  private:
    // NOT IMPLEMENTED
    StackTracePrinter& operator=(const StackTracePrinter&);

  public:
    // CREATORS
    explicit StackTracePrinter(int  maxFrames = -1,
                               bool demanglingPreferredFlag = true,
                               int  additionalIgnoreFrames = 0);
        // Create a 'StackTracePrinter' object that will, if streamed, render a
        // description of the current call stack to the stream.  Optionally
        // specify 'maxFrames' indicating maximum number of frames to dispay.
        // If 'maxFrames' is -1 or unspecified, use a value of at least 1024.
        // The optionally specified 'demanglingPreferredFlag' determines
        // whether demangling occurs, on platforms where demangling is
        // available and optional.  Optionally specify 'additionalIgnoreFrames'
        // which is added to 'bsls::StackAddressUtil::k_IGNORE_FRAMES' to
        // ignore the topmost frames of the caller.  Return a reference to
        // 'stream'.  The behavior is undefined unless '-1 == maxFrames' or
        // '0 <= maxFrames', and unless '0 <= additionalIgnoreFrames'.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
    StackTracePrinter(const StackTracePrinter& original) = default;
#endif
        // Create a copy of the specified 'original'.  Default the copy
        // constructor to work around a bug on the Aix compiler that generates
        // warnings every time an object of this type is passed to 'operator<<'
        // if this constructor is deleted or private.
};

bsl::ostream& operator<<(bsl::ostream&            stream,
                         const StackTracePrinter& object);
    // Stream a newline followed by a multiline stack trace according to the
    // parameters passed to the specified 'object' at its creation to the
    // specified 'stream'.

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
