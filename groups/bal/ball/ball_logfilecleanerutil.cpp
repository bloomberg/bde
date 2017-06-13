// ball_logfilecleanerutil.cpp                                        -*-C++-*-
#include <ball_logfilecleanerutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_logfilecleanerutil_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>

#include <bsl_cstddef.h>

namespace BloombergLP {
namespace ball {

namespace {
    enum ConversionState {
        e_NORMAL_STATE,        // normal characters
        e_ESCAPE_START_STATE,  // found '%' character
        e_ESCAPE_POST_STATE,   // found first '%'-escape sequence
        e_ESCAPE_DROP_STATE    // found subsequent '%'-escape sequence (drop)
    };
}  // close unnamed namespace

                           // -------------------------
                           // struct LogFileCleanerUtil
                           // -------------------------

// PRIVATE CLASS METHODS
void LogFileCleanerUtil::logFileCleanupOnRotationDefault(
                                  int,
                                  const bsl::string&,
                                  const balb::FileCleanerConfiguration& config)
{
    balb::FileCleanerUtil::removeFiles(config);
}

// CLASS METHODS
void LogFileCleanerUtil::logPatternToFilePattern(
                                         bsl::string              *filePattern,
                                         const bslstl::StringRef&  logPattern)
{
    filePattern->reserve(logPattern.length());
    filePattern->clear();

    size_t      idx = 0;
    const char *logPatternData = logPattern.data();

    ConversionState state = e_NORMAL_STATE;

    while (idx < logPattern.length()) {
        switch (state) {
          case e_NORMAL_STATE: {
            switch (logPatternData[idx]) {
              case '%': {
                state = e_ESCAPE_START_STATE;
              } break;
              case '*': {
                filePattern->push_back(logPatternData[idx]);
                state = e_ESCAPE_POST_STATE;
              } break;
              default: {
                filePattern->push_back(logPatternData[idx]);
              } break;
            }
          } break;
          case e_ESCAPE_START_STATE: {
            switch (logPatternData[idx]) {
              case 'T':
              case 'Y':
              case 'M':
              case 'D':
              case 'h':
              case 'm':
              case 's':
              case 'p': {
                // Recognized valid '%'-escape sequence.
                filePattern->push_back('*');
                state = e_ESCAPE_POST_STATE;
              } break;
              case '*': {
                filePattern->push_back('%');
                filePattern->push_back('*');
                state = e_ESCAPE_POST_STATE;
              } break;
              case '%': {
                filePattern->push_back(logPatternData[idx]);
                state = e_NORMAL_STATE;
              } break;
              default: {
                // Any other character after '%' is not valid escape.
                filePattern->push_back('%');
                filePattern->push_back(logPatternData[idx]);
                state = e_NORMAL_STATE;
              } break;
            }
          } break;
          case e_ESCAPE_POST_STATE: {
            switch (logPatternData[idx]) {
              case '%': {
                // Recognized subsequent [potentially] '%'-escape sequence.
                state = e_ESCAPE_DROP_STATE;
              } break;
              case '*': {
                // Drop subsequent '*'.
              } break;
              default: {
                filePattern->push_back(logPatternData[idx]);
                state = e_NORMAL_STATE;
              } break;
            }
          } break;
          case e_ESCAPE_DROP_STATE: {
            switch (logPatternData[idx]) {
              case 'T':
              case 'Y':
              case 'M':
              case 'D':
              case 'h':
              case 'm':
              case 's':
              case 'p': {
                // Recognized subsequent valid '%'-escape sequence.  Drop it.
                state = e_ESCAPE_POST_STATE;
              }  break;
              case '%': {
                filePattern->push_back(logPatternData[idx]);
                state = e_NORMAL_STATE;
              } break;
              case '*': {
                filePattern->push_back('%');
                filePattern->push_back('*');
                state = e_ESCAPE_POST_STATE;
              } break;
              default: {
                // Any other character after '%' is not valid escape.
                filePattern->push_back('%');
                filePattern->push_back(logPatternData[idx]);
                state = e_NORMAL_STATE;
              } break;
            }
          } break;
          default: {
            BSLS_ASSERT(!"Invalid parser state.");
          } break;
        }
        ++idx;
    }

    // Handle the pattern ending.
    switch (state) {
      case e_ESCAPE_START_STATE:
      case e_ESCAPE_DROP_STATE: {
        filePattern->push_back('%');
        filePattern->push_back('*');
      } break;
      case e_NORMAL_STATE: {
        filePattern->push_back('*');
      } break;
      case e_ESCAPE_POST_STATE: {
      } break;
      default: {
        BSLS_ASSERT(!"Invalid parser state.");
      } break;
    }
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
