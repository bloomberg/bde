// ball_thresholdaggregate.cpp                                        -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <ball_thresholdaggregate.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_thresholdaggregate_cpp,"$Id$ $CSID$")

#include <bdlb_hashutil.h>
#include <bdlb_print.h>

#include <bsls_platform.h>

#include <bsl_algorithm.h>
#include <bsl_ostream.h>

// Note: on Windows -> WinDef.h:#define max(a,b) ...
#if defined(BSLS_PLATFORM_CMP_MSVC) && defined(max)
#undef max
#endif

namespace BloombergLP {

namespace ball {
                        // ------------------------
                        // class ThresholdAggregate
                        // ------------------------

// CLASS METHODS
int ThresholdAggregate::hash(const ThresholdAggregate& aggregate,
                             int                       size)
{
#ifdef BSLS_PLATFORM_IS_BIG_ENDIAN
    int value = (aggregate.recordLevel()  << 24)
              + (aggregate.passLevel()    << 16)
              + (aggregate.triggerLevel() <<  8)
              +  aggregate.triggerAllLevel();
#else
    // To return the same result on all platforms, we switch the scrambling of
    // the bytes.

    int value = (aggregate.triggerAllLevel() << 24)
              + (aggregate.triggerLevel()    << 16)
              + (aggregate.passLevel()       <<  8)
              +  aggregate.recordLevel();
#endif

    unsigned int h = bdlb::HashUtil::hash1((const char*)&value, sizeof(value));

    return h % size;
}

int ThresholdAggregate::maxLevel(int recordLevel,
                                 int passLevel,
                                 int triggerLevel,
                                 int triggerAllLevel)
{
    return bsl::max(bsl::max(passLevel,    recordLevel),
                    bsl::max(triggerLevel, triggerAllLevel));
}

// MANIPULATORS
ThresholdAggregate&
ThresholdAggregate::operator=(const ThresholdAggregate& rhs)
{
    d_recordLevel     = rhs.d_recordLevel;
    d_passLevel       = rhs.d_passLevel;
    d_triggerLevel    = rhs.d_triggerLevel;
    d_triggerAllLevel = rhs.d_triggerAllLevel;

    return *this;
}

int ThresholdAggregate::setLevels(int recordLevel,
                                  int passLevel,
                                  int triggerLevel,
                                  int triggerAllLevel)
{
    if (areValidThresholdLevels(recordLevel,
                                passLevel,
                                triggerLevel,
                                triggerAllLevel)) {
        d_recordLevel     = static_cast<unsigned char>(recordLevel);
        d_passLevel       = static_cast<unsigned char>(passLevel);
        d_triggerLevel    = static_cast<unsigned char>(triggerLevel);
        d_triggerAllLevel = static_cast<unsigned char>(triggerAllLevel);

        return 0;                                                     // RETURN
    }

    return -1;
}

// ACCESSORS
bsl::ostream&
ThresholdAggregate::print(bsl::ostream& stream,
                          int           level,
                          int           spacesPerLevel) const
{
    const char NL = spacesPerLevel >= 0 ? '\n' : ' ';

    if (level < 0) {
        level = -level;
    }
    else {
        bdlb::Print::indent(stream, level, spacesPerLevel);
    }

    stream << '[' << NL;

    bdlb::Print::indent(stream, level + 1, spacesPerLevel);
    stream << recordLevel() << NL;

    bdlb::Print::indent(stream, level + 1, spacesPerLevel);
    stream << passLevel() << NL;

    bdlb::Print::indent(stream, level + 1, spacesPerLevel);
    stream << triggerLevel() << NL;

    bdlb::Print::indent(stream, level + 1, spacesPerLevel);
    stream << triggerAllLevel() << NL;

    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << ']' << NL;

    return stream;
}

}  // close package namespace

}  // close enterprise namespace

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
