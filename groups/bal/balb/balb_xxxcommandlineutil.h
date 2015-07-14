// balb_xxxcommandlineutil.h   -*-C++-*-
#ifndef INCLUDED_BALB_XXXCOMMANDLINEUTIL
#define INCLUDED_BALB_XXXCOMMANDLINEUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide utility functions for operating on command-line arguments.
//
//@CLASSES:
//  balb::CommandLineUtil: namespace for utilities on command-line arguments
//
//@AUTHOR: David Rubin (drubin6)
//
//@SEE_ALSO:
//
//@DESCRIPTION: This class provides a namespace for a set of utility functions
// that can be used to process command-line arguments.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Multiple Argument Sets
///- - - - - - - - - - - - - - - - -
// Suppose that we need to modify an existing program so that it spawns a
// script, and that furthermore, the script processes a set of command-line
// arguments that are different from the command-line arguments specified to
// the host program.  We can simply concatenate the script's command-line
// arguments to those of the host program, separated by the string '--', and
// then parse the single command-line into two sets of arguments, one of which
// can be passed to a function that executes the script.
//..
//  int main(int argc, char **argv)
//  {
//      bsl::vector<char *> hostArgs;
//      bsl::vector<char *> scriptArgs;
//
//      if (0 != balb::CommandLineUtil::splitCommandLineArguments(&hostArgs,
//                                                               &scriptArgs,
//                                                               argc,
//                                                               argv)) {
//          // ...handle error...
//      }
//
//      // Execute the script with its own command-line arguments.
//
//      forkAndExecute("/path/to/script", scriptArgs.size(), &scriptArgs[0]);
//
//      // Execute the remainder of the original program.
//
//      bsl::string                configFilename;
//      balb::CommandLineOptionInfo specTable[] = {
//          {
//              "",
//              "filename",
//              "configuration file name",
//              balb::CommandLineTypeInfo(&configFilename),
//              balb::CommandLineOccurrenceInfo::REQUIRED,
//          },
//      };
//
//      balb::CommandLine commandLine(specTable);
//      if (0 != commandLine.parse(hostArgs.size(), &hostArgs[0])) {
//          commandLine.printUsage();
//          bsl::exit(-1);
//      }
//
//      process(configFilename);
//
//      return 0;
//  }
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

namespace balb {
struct CommandLineUtil {
    // This class provides a namespace for a set utility functions that can be
    // used to process command-line arguments.

    // CLASS METHODS
    static int splitCommandLineArguments(
                                      bsl::vector<char *>  *leftArgs,
                                      bsl::vector<char *>  *rightArgs,
                                      int                   argc,
                                      char                **argv,
                                      const char           *separator = "--");
        // Load into the specified 'leftArgs' the value 'argv[0]', and all
        // additional arguments up to, but not including, the specified
        // 'separator' from the command line specified by 'argc' and 'argv'.
        // Load into the specified 'rightArgs' the value 'argv[0]', and all
        // additional command line arguments following the 'separator'.  Return
        // 0 on success, and a non-zero value otherwise.  Note that, regardless
        // of whether or not 'separator' appears in the command line, both
        // 'leftArgs' and 'rightArgs' will contain at least 'argv[0]'.
};
}  // close package namespace

namespace bael {
typedef balb::CommandLineUtil CommandLineUtil;
}  // close package namespace
    // *DEPRECATED*  Use the type name 'balb::CommandLineUtil' instead.

}  // close namespace BloombergLP

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2006, 2007, 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
