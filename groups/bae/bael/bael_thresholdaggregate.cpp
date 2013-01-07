// bael_thresholdaggregate.cpp                                        -*-C++-*-
#include <bael_thresholdaggregate.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bael_thresholdaggregate_cpp,"$Id$ $CSID$")

#include <bdeu_hashutil.h>
#include <bdeu_print.h>

#include <bsls_platformutil.h>

#include <bsl_algorithm.h>
#include <bsl_ostream.h>

// Note: on Windows -> WinDef.h:#define max(a,b) ...
#if defined(BSLS_PLATFORM_CMP_MSVC) && defined(max)
#undef max
#endif

namespace BloombergLP {

                        // -----------------------------
                        // class bael_ThresholdAggregate
                        // -----------------------------

// CLASS METHODS
int bael_ThresholdAggregate::hash(const bael_ThresholdAggregate& aggregate,
                                  int                            size)
{
#ifdef BSLS_PLATFORMUTIL_IS_BIG_ENDIAN
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

    unsigned int h = bdeu_HashUtil::hash1((const char*)&value, sizeof(value));

    return h % size;
}

int bael_ThresholdAggregate::maxLevel(int recordLevel,
                                      int passLevel,
                                      int triggerLevel,
                                      int triggerAllLevel)
{
    return bsl::max(bsl::max(passLevel,    recordLevel),
                    bsl::max(triggerLevel, triggerAllLevel));
}

// MANIPULATORS
bael_ThresholdAggregate&
bael_ThresholdAggregate::operator=(const bael_ThresholdAggregate& rhs)
{
    d_recordLevel     = rhs.d_recordLevel;
    d_passLevel       = rhs.d_passLevel;
    d_triggerLevel    = rhs.d_triggerLevel;
    d_triggerAllLevel = rhs.d_triggerAllLevel;

    return *this;
}

int bael_ThresholdAggregate::setLevels(int recordLevel,
                                       int passLevel,
                                       int triggerLevel,
                                       int triggerAllLevel)
{
    if (areValidThresholdLevels(recordLevel,
                                passLevel,
                                triggerLevel,
                                triggerAllLevel)) {
        d_recordLevel     = recordLevel;
        d_passLevel       = passLevel;
        d_triggerLevel    = triggerLevel;
        d_triggerAllLevel = triggerAllLevel;

        return 0;                                                     // RETURN
    }

    return -1;
}

// ACCESSORS
bsl::ostream&
bael_ThresholdAggregate::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    const char NL = spacesPerLevel >= 0 ? '\n' : ' ';

    if (level < 0) {
        level = -level;
    }
    else {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    stream << '[' << NL;

    bdeu_Print::indent(stream, level + 1, spacesPerLevel);
    stream << recordLevel() << NL;

    bdeu_Print::indent(stream, level + 1, spacesPerLevel);
    stream << passLevel() << NL;

    bdeu_Print::indent(stream, level + 1, spacesPerLevel);
    stream << triggerLevel() << NL;

    bdeu_Print::indent(stream, level + 1, spacesPerLevel);
    stream << triggerAllLevel() << NL;

    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << ']' << NL;

    return stream;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
