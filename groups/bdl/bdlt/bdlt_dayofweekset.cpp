// bdlt_dayofweekset.cpp                                              -*-C++-*-
#include <bdlt_dayofweekset.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlt_dayofweekset_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsl_cstdlib.h>
#include <bsl_iomanip.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace bdlt {

                         // -----------------------
                         // class DayOfWeekSet_Iter
                         // -----------------------

// DATA
const DayOfWeek::Enum DayOfWeekSet_Iter::s_dayOfWeekArray[9] = {
    DayOfWeek::e_SUN,
    DayOfWeek::e_SUN,
    DayOfWeek::e_MON,
    DayOfWeek::e_TUE,
    DayOfWeek::e_WED,
    DayOfWeek::e_THU,
    DayOfWeek::e_FRI,
    DayOfWeek::e_SAT,
    DayOfWeek::e_SUN,
};

// CREATORS
DayOfWeekSet_Iter::DayOfWeekSet_Iter(int data, int index)
: d_data( static_cast<unsigned char>(data))
, d_index(static_cast<signed   char>(index))
{
    BSLS_ASSERT_SAFE(0 == (data & 1));
    BSLS_ASSERT_SAFE(index >= 0 && index <= 8);

    while (d_index < 8 && !((1 << d_index) & d_data)) {
        ++d_index;
    }
}

// MANIPULATORS
DayOfWeekSet_Iter& DayOfWeekSet_Iter::operator++()
{
    while (d_index < 8) {
        ++d_index;
        if ((1 << d_index) & d_data) {
            break;
        }
    }

    return *this;
}

DayOfWeekSet_Iter& DayOfWeekSet_Iter::operator--()
{
    while (d_index > 0) {
        --d_index;
        if ((1 << d_index) & d_data) {
            break;
        }
    }

    return *this;
}

                         // ------------------
                         // class DayOfWeekSet
                         // ------------------

// ACCESSORS
bsl::ostream& DayOfWeekSet::print(bsl::ostream& stream,
                                  int           level,
                                  int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (length() > 0) {
        // Print the indentation: This is equivalent to
        // 'bslim::Printer::printIndentation', but that's a private function.

        const int numSpaces = spacesPerLevel >= 0
                            ? spacesPerLevel * (bsl::abs(level) + 1)
                            : 1;
        stream << bsl::setw(numSpaces) << "";

        // Print the days.

        bool first = true;
        const iterator e = end();
        for (iterator iter = begin(); e != iter; ++iter) {
            stream << (first ? (first = false, "") : " ") << *iter;
        }

        // Finish up with newline.

        if (spacesPerLevel >= 0) {
            stream << '\n';
        }
    }

    printer.end();

    return stream;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
