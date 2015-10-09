// balst_dbghelpdllimpl_windows.h                                     -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALST_DBGHELPDLLIMPL_WINDOWS
#define INCLUDED_BALST_DBGHELPDLLIMPL_WINDOWS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide access to the 'dbghelp.dll' shared library on Windows.
//
//@CLASSES:
//   balst::DbghelpDllImpl_Windows: interface to 'dbghelp.dll' shared library
//
//@DESCRIPTION: This component provides a single class,
// 'balst::DbghelpDllImpl_Windows', that provides Windows platform-specific
// facilities in support of the 'balst' stack trace facility.  This component
// is *not* intended for public use.
//
// The 'balst::DbghelpDllImpl_Windows' class:
//
//: 1 Provides a suite of static methods that wrap calls to several functions
//:   of 'dbghelp.dll', a Windows shared library.
//:
//: 2 Loads (once) the '.dll' on the first call of any of its methods that call
//:   functions in that '.dll'.
//:
//: 3 On process termination, cleans up data structures, and unloads the
//:   '.dll'.
//
// The mapping of class' static methods to '.dll' functions is:
//..
//  balst::DbghelpDllImpl_Windows  dbghelp.dll
//  ----------------------------  -----------
//  symSetOptions                 SymSetOptions
//  symFromAddr                   SymFromAddr
//  symGetSymFromAddr64           SymGetSymFromAddr64
//  symGetLineFromAddr64          SymGetLineFromAddr64
//  stackWalk64                   StackWalk64
//..
// The signatures of these static methods typically have fewer parameters than
// those of the '.dll' functions.  Those arguments (e.g., a Windows handle to
// the current process) are provided in the implementation.  Otherwise, the
// parameters of the static methods match in order and type those of the '.dll'
// functions.
//
///Thread Safety
///-------------
// Since the functions in 'dbghelp.dll' are *not* thread-safe, this class
// provides a static mutex (of type 'bslmt::QLock') which must be acquired
// before any of the '.dll'-invoking methods of this function are called.  In
// multithreaded code, the mutex lock must be locked before any of the methods
// other than 'isLoaded' or 'qLock' are called.  The library is loaded on a
// per-process basis.  The client must ensure that the mutex is locked during
// any call to any function in this class, but it is not necessary for it to be
// locked any longer than that.
//
///Usage
///-----
// In this section we show the intended usage of this component.
//
///Example: Determining line number and source file name
///- - - - - - - - - - - - - - - - - - - - - - - - - - -
// We will demonstrate using 'dbghelp.dll' to find the line number and source
// file name where 'main' is.  This code will only work on Windows, and only
// when compiled in debug mode:
//..
//  #if defined(BSLS_PLATFORM_OS_WINDOWS) && defined(BDE_BUILD_TARGET_DBG)
//..
// First, we lock the mutex:
//..
//  bslmt::QLockGuard guard(&balst::DbghelpDllImpl_Windows::qLock());
//..
// Next, we set the options for the 'dbghelp.dll' library.  Note that any call
// to any of the functions in 'balst::DbghelpDllImpl_Windows' other than
// 'qlock' will load the 'dbghelp.dll' library if necessary.
//..
//  balst::DbghelpDllImpl_Windows::symSetOptions(SYMOPT_NO_PROMPTS
//                                              | SYMOPT_LOAD_LINES
//                                              | SYMOPT_DEFERRED_LOADS);
//..
// Then, we declare and initialize some variables to hold our results:
//..
//  IMAGEHLP_LINE64 line;
//  ZeroMemory(&line, sizeof(IMAGEHLP_LINE64));
//  line.SizeOfStruct = sizeof(line);
//  DWORD offsetFromLine;
//..
// Next, we do the call that finds the line number and source file name:
//..
//  int rc = balst::DbghelpDllImpl_Windows::symGetLineFromAddr64(
//                                                             (DWORD64) &main,
//                                                             &offsetFromLine,
//                                                             &line);
//  assert(rc);
//..
// Finally, we print out our results:
//..
//  bsl::cout << "Source file name: " << line.FileName << bsl::endl;
//  bsl::cout << "Line #: " << line.LineNumber << bsl::endl;
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifdef BSLS_PLATFORM_OS_WINDOWS

#ifndef INCLUDED_BSLMT_QLOCK
#include <bslmt_qlock.h>
#endif

#ifndef INCLUDED_WINDOWS
#include <windows.h>
#define INCLUDED_WINDOWS
#endif

#ifndef INCLUDED_INTRIN
#include <intrin.h>
#define INCLUDED_INTRIN
#endif

#ifndef INCLUDED_DBGHELP
#include <dbghelp.h>
#define INCLUDED_DBGHELP
#endif

namespace BloombergLP {

namespace balst {
                        // ============================
                        // class DbghelpDllImpl_Windows
                        // ============================

class DbghelpDllImpl_Windows {
    // This class provides a namespace for static methods (defined only on
    // Windows platforms) which call functions from the 'dbghelp.dll' shared
    // library.  The dynamic library is lazily loaded and initialized when
    // needed, the implementation arranges for automatic unloading of that
    // library at process termination (it is done by the destructor of a static
    // struct).  Typically, the methods of this class that call '.dll'
    // functions implicitly provide some of the needed arguments (e.g., a
    // handle to the current process); otherwise, the specified parameters of
    // these methods match in order and type those of the '.dll' functions.
    // See '@DESCRIPTION' for further details.
    //
    // The methods of this class are *not* thread-safe.  In a multi-threaded
    // environment acquire the (static) mutex (see the 'qLock' method) before
    // invoking any other methods of this class.

    // DATA
    static bslmt::QLock s_qLock;  // mutex to synchronize access to '.dll'
                                 // functions, which are not *thread* *safe*

  public:
    // CLASS METHODS
    static bool isLoaded();
        // Return 'true' if 'dbghelp.dll' has already been successfully loaded
        // in this process, and 'false' otherwise.  For testing only.

    static bslmt::QLock& qLock();
        // Return a reference providing modifiable access to static mutex
        // provided for synchronizing access to the 'dbghelp.dll' shared
        // library.

    // *** Methods corresponding to Windows 'dbghelp.dll' functions ***

    static DWORD symSetOptions(DWORD symOptions);
        // Invoke the 'SymSetOptions' function of 'dbghelp.dll' with the
        // specified 'symOptions', and return the result.  The behavior is
        // undefined if the mutex provided by the 'qLock' method is not held.
        //
        // Note that 'symOptions' (a bitwise-OR of 'SYMOPT_*' values) specifies
        // flags affecting subsequent calls to the '.dll'.  Also note that the
        // current options are returned.  Finally, note that further details
        // are available at 'http://msdn.com'.

#ifdef BSLS_PLATFORM_CPU_32_BIT
    // 'symFromAddr' doesn't work on 64-bit, so we conditionally compile and
    // use 'SymFromAddr' on 32-bit and 'SymGetSymFromAddr64' on 64-bit.

    static BOOL symFromAddr(DWORD64      address,
                            PDWORD64     displacement,
                            PSYMBOL_INFO symbol);
        // Invoke the 'SymFromAddr' function of 'dbghelp.dll' with the
        // specified 'address', 'displacement', and 'symbol', and with an
        // (internally generated) handle for the current Windows process, and
        // return the result.  The behavior is undefined unless the mutex
        // provided by the 'qLock' method is held and 'symbol->MaxNameLen' is
        // set to the maximum length (a value of '2000' is recommended).
        //
        // Note that 'symbol' is loaded with a pointer to the symbol
        // information for the symbol at 'address', and 'displacement' is
        // loaded with the difference between 'address' and the address of the
        // symbol described at 'symbol'.  Also note that 'true' is returned if
        // a symbol is found for 'address', and 'false' is returned otherwise.
        // Finally, note that further details of 'SymFromAddr' are available at
        // 'http://msdn.com'.

#else
    static BOOL symGetSymFromAddr64(DWORD64            address,
                                    PDWORD64           displacement,
                                    PIMAGEHLP_SYMBOL64 symbol);
        // Invoke the 'SymFromAddr64' function of 'dbghelp.dll' with the
        // specified 'address', 'displacement', and 'symbol', and with an
        // (internally generated) handle for the current Windows process, and
        // return the result.  The behavior is undefined unless the mutex
        // provided by the 'qLock' method is held and 'symbol->MaxNameLen' is
        // set to the maximum length (a value of '2000' is recommended).
        //
        // Note that 'symbol' is loaded with a pointer to the symbol
        // information for the symbol at 'address', and 'displacement' is
        // loaded with the difference between 'address' and the address of the
        // symbol described at 'symbol'.  Also note that 'true' is returned if
        // a symbol is found for 'address', and 'false' is returned otherwise.
        // Finally, note that further details of 'SymFromAddr64' are available
        // at 'http://msdn.com'.

#endif

    static BOOL symGetLineFromAddr64(DWORD64          dwAddr,
                                     PDWORD           pdwDisplacement,
                                     PIMAGEHLP_LINE64 line);
        // Invoke the 'SymGetLineFromAddr64' function of 'dbghelp.dll' with the
        // specified 'dwAddr', 'pdwDisplacement', and 'line', and with an
        // (internally generated) handle for the current Windows process, and
        // return the result.  The behavior is undefined if the mutex provided
        // by the 'qLock' method is not held.
        //
        // Note that 'line' is loaded with a pointer to line information (e.g.,
        // source file name and line number) for the code at 'dwAddr' and
        // 'pdwDisplacement' is loaded with the displacement of that code from
        // the beginning of the line.  Also note that 'true' is returned if
        // information is found for 'dwAddr', and 'false' is returned
        // otherwise.  Finally, note that further details of
        // 'SymGetLineFromAddr64' are available at 'http://msdn.com'.

    static BOOL stackWalk64(DWORD          machineType,
                            HANDLE         hThread,
                            LPSTACKFRAME64 stackFrame,
                            PVOID          contextRecord);
        // Invoke the 'StackWalk64' function of 'dbghelp.dll' with the
        // specified 'machineType', 'hThread', 'stackFrame', and
        // 'contextRecord', and with internally set handlers for remaining
        // arguments, and return the result.  The behavior is undefined if the
        // mutex provided by the 'qLock' method is not held.
        //
        // Note that 'machineType' distinguishes between 32 and 64 bit
        // executables, 'hThread' is a Windows handle for the thread of
        // interest, 'stackFrame' has the address of a properly initialized
        // structure (including a field to receive the program counter), and
        // 'contextRecord' has the address of an initialized Windows 'CONTEXT'
        // structure.  The call to this method provides information on a single
        // stack frame, each repeated call sets '*stackFrame' to the next stack
        // frame.  As long as there remain stack frames to be found, 'TRUE' is
        // returned; 'FALSE' is returned when there were no more stack frames
        // to be found.  Finally, note that further details of 'StackWalk64'
        // are available at 'http://msdn.com'.
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ----------------------------
                        // class DbghelpDllImpl_Windows
                        // ----------------------------

// CLASS METHODS
inline
bslmt::QLock& DbghelpDllImpl_Windows::qLock()
{
    return s_qLock;
}
}  // close package namespace

}  // close namespace BloombergLP

#endif  // BSLS_PLATFORM_OS_WINDOWS

#endif

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
