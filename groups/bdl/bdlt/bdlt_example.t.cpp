// bdlt_example.t.cpp

#include <bdlt_example.h>

#include <bdlt_datetime.h>
#include <bdlt_dateutil.h>
#include <bdlt_dayofweek.h>
#include <bdlt_dayofweekset.h>
#include <bdlt_epochutil.h>
#include <bdlt_localtimeoffset.h>

#include <bsls_types.h>

#include <bsl_algorithm.h>  // for 'sort'
#include <bsl_cassert.h>
#include <bsl_iostream.h>
#include <bsl_vector.h>

using namespace BloombergLP;

int main()
{
///Usage
///-----
// This section illustrates intended use of these components.
//
///Example 1: Celebrating Milestone Dates
/// - - - - - - - - - - - - - - - - - - -
// Date and time calculations are simple in principle but tedious and error-
// prone in practice.  Consequently, we tend to choose as "milestone" dates
// that allow us to avoid such awkward calculations.  For example, we consider
// ourselves one year older on our birthday (anniversaries) even though some
// years (leap years) are longer than others.  Access to a rich set of
// types and utilities for date and time calculations affords us other options.
// Suppose one wishes to commemorate the 20,000th day of one's life.
// 
// First, create a 'bdlt::Datetime' object to represent our birthday date.
// Let's assume we were born at very beginning of the Unix epoch:
//..
    bdlt::Datetime dtBirthday = bdlt::EpochUtil::epoch();
    assert(bdlt::Datetime(1970, 1, 1, 0, 0, 0, 0) == dtBirthday);
//..
// Next, we calculate the milestone date.
//..
    bdlt::Datetime dt20k(dtBirthday);  dt20k.addDays(20000);
    assert(bdlt::DatetimeInterval(20000) == dt20k - dtBirthday);

    bsl::cout << dt20k << bsl::endl;
//..
// and find on the console:
//
//  04OCT2024_00:00:00.000
//
// Then, we recall that the above values represent UTC datetime values but
// we reside in another time zone (e.g., "New York City").  We must calculate
// the localtime offset for that future date and adjust our milestone date.
//..
    bsls::TimeInterval localTimeOffset
                               = bdlt::LocalTimeOffset::localTimeOffset(dt20k);

    bsls::Types::Int64 offsetSeconds
                         =  localTimeOffset.seconds();
    bsls::Types::Int64 offsetMilliseconds
                          = localTimeOffset.nanoseconds()
                          / bdlt::TimeUnitRatio::k_NANOSECONDS_PER_MILLISECOND;

    bdlt::Datetime dt20kLcl(dt20k);
    dt20kLcl.     addSeconds(     offsetSeconds);
    dt20kLcl.addMilliseconds(offsetMilliseconds);

    bdlt::Date targetDate = dt20kLcl.date();

    bsl::cout << targetDate << bsl::endl;
//..
// which is one calendar day earlier:
//
//  03OCT2024
// 
// Next, we recall that we prefer to hold celebrations on weekend days,
// not weekdays, so we determine where in the week target date falls:
//..
    bdlt::DayOfWeekSet weekendDays;
    weekendDays.add(bdlt::DayOfWeek::e_SATURDAY);
    weekendDays.add(bdlt::DayOfWeek::e_SUNDAY);

    if (weekendDays.isMember(targetDate.dayOfWeek())) {
//..
// Now, if the target date is a weekend day, we hold that date:
//..
        bdlt::Date holdDate = targetDate;
        bsl::cout << "Hold the day:"      << " "
                  << holdDate.dayOfWeek() << " "
                  << holdDate             << bsl::endl;
        } else {
//..
// Otherwise, we consider holding the celebration on the either day of
// the adjacent weekends:
//..
            bsl::cout << "Target date is midweek:" << " "
                      << targetDate.dayOfWeek()    << " "
                      << targetDate                << bsl::endl;
//..
// shows 
//  Target date is midweek: THU 03OCT2024
//
// We calculate candidate dates using functions from 'bdlt::DateUtil'
// in concert with a standard container ('bsl::vector') and a standard
// algorithm ('bsl::sort)':
//..
            bsl::vector<bdlt::Date> candidateDates;

            for (bdlt::DayOfWeekSet::iterator itr  = weekendDays.begin(),
                                              end  = weekendDays.end();
                                              end != itr;
                                               ++itr) {
                bdlt::Date candidateNext = bdlt::DateUtil::nextDayOfWeek(
                                                                   *itr,
                                                                   targetDate);
                bdlt::Date candidatePrev = bdlt::DateUtil::previousDayOfWeek(
                                                                  *itr,
                                                                   targetDate);
                candidateDates.push_back(candidateNext);
                candidateDates.push_back(candidatePrev);
            }

            bsl::sort(candidateDates.begin(), candidateDates.end());

            for (bsl::vector<bdlt::Date>::const_iterator
                                                 itr  = candidateDates.begin(),
                                                 end  = candidateDates.end();
                                                 end != itr;
                                                 ++itr) {
                bdlt::Date candidateDate = *itr;
                bsl::cout << "Candidate:"              << " "
                          << candidateDate.dayOfWeek() << " "
                          << candidateDate             << bsl::endl;
//..
// which shows:
//
//  Candidate: SAT 28SEP2024
//  Candidate: SUN 29SEP2024
//  Candidate: SAT 05OCT2024
//  Candidate: SUN 06OCT2024
//
// Finally, we decide on the best date and send out our hold-the-date notices.
//..
        }
    }
}
