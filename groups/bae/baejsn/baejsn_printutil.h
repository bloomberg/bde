// baejsn_printutil.h                                                 -*-C++-*-
#ifndef INCLUDED_BAEJSN_PRINTUTIL
#define INCLUDED_BAEJSN_PRINTUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a JSON printutil class.
//
//@CLASSES:
// baejsn_PrintUtil: JSON print utility class
//
//@SEE_ALSO: baejsn_decoder
//
//@AUTHOR: Raymond Chiu (schiu49)
//
//@DESCRIPTION: This component provides utility functions for
// encoding a 'bdeat' Simple type into JSON string.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEPU_ISO8601
#include <bdepu_iso8601.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_IOMANIP
#include <bsl_iomanip.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_LIMITS
#include <bsl_limits.h>
#endif

#ifndef INCLUDED_BSL_C_MATH
#include <bsl_c_math.h>
#endif

namespace BloombergLP {

                        // ======================
                        // class baejsn_PrintUtil
                        // ======================

class baejsn_PrintUtil {
  private:
    // PRIVATE CLASS METHODS
    template <class TYPE>
    static int printDateAndTime(bsl::ostream& stream, const TYPE& value);
        // Encode the specified 'value' into JSON using ISO 8601 format and
        // output the result to the specified 'stream'.

    template <typename TYPE>
    static int printDecimal(bsl::ostream& stream, TYPE value);
        // Encode the specified floating point 'value' into JSON and output the
        // result to the specified 'stream'.

    static int printString(bsl::ostream&            stream,
                           const bslstl::StringRef& value);
        // Encode the specified string 'value' into JSON format and output the
        // result to the specified 'stream'.

    static int printValueImp(bsl::ostream& stream, bool value);
    static int printValueImp(bsl::ostream& stream, short value);
    static int printValueImp(bsl::ostream& stream, int value);
    static int printValueImp(bsl::ostream& stream, bsls::Types::Int64 value);
    static int printValueImp(bsl::ostream& stream, unsigned char value);
    static int printValueImp(bsl::ostream& stream, unsigned short value);
    static int printValueImp(bsl::ostream& stream, unsigned int value);
    static int printValueImp(bsl::ostream& stream, bsls::Types::Uint64 value);
    static int printValueImp(bsl::ostream& stream, float value);
    static int printValueImp(bsl::ostream& stream, double value);
    static int printValueImp(bsl::ostream& stream, const bsl::string & value);
    static int printValueImp(bsl::ostream& stream, const char *value);
    static int printValueImp(bsl::ostream& stream, char value);
    static int printValueImp(bsl::ostream& stream, const bdet_Time& value);
    static int printValueImp(bsl::ostream& stream, const bdet_Date& value);
    static int printValueImp(bsl::ostream& stream, const bdet_Datetime& value);
    static int printValueImp(bsl::ostream& stream, const bdet_TimeTz& value);
    static int printValueImp(bsl::ostream& stream, const bdet_DateTz& value);
    static int printValueImp(bsl::ostream&          stream,
                             const bdet_DatetimeTz& value);
        // Encode the specified 'value' into JSON format and output the result
        // to the specified 'stream'.

  public:
    template <class TYPE>
    static int printValue(bsl::ostream& stream, const TYPE& value);
        // Encode the specified 'value' into JSON format and output the result
        // to the specified 'stream'.
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                            // ----------------------
                            // class baejsn_PrintUtil
                            // ----------------------

// PRIVATE MANIPULATORS
template <class TYPE>
inline
int baejsn_PrintUtil::printDateAndTime(bsl::ostream& stream, const TYPE& value)
{
    char buffer[bdepu_Iso8601::BDEPU_MAX_DATETIME_STRLEN + 1];
    bdepu_Iso8601::generate(buffer, value, sizeof buffer);
    return printValueImp(stream, buffer);
}

template <typename TYPE>
int baejsn_PrintUtil::printDecimal(bsl::ostream& stream, TYPE value)
{
    if (isnan(value)
     || value == bsl::numeric_limits<TYPE>::infinity()
     || value == -bsl::numeric_limits<TYPE>::infinity()) {
        return -1;                                                    // RETURN
    }

    bsl::streamsize         prec  = stream.precision();
    bsl::ios_base::fmtflags flags = stream.flags();

    stream.precision(bsl::numeric_limits<TYPE>::digits10);

    stream << value;

    stream.precision(prec);
    stream.flags(flags);
    return 0;
}

inline
int baejsn_PrintUtil::printValueImp(bsl::ostream& stream, bool value)
{
    stream << (value ? "true" : "false");
    return 0;
}

inline
int baejsn_PrintUtil::printValueImp(bsl::ostream& stream, short value)
{
    stream << value;
    return 0;
}

inline
int baejsn_PrintUtil::printValueImp(bsl::ostream& stream, int value)
{
    stream << value;
    return 0;
}

inline
int baejsn_PrintUtil::printValueImp(bsl::ostream&      stream,
                                    bsls::Types::Int64 value)
{
    stream << value;
    return 0;
}

inline
int baejsn_PrintUtil::printValueImp(bsl::ostream& stream, unsigned char value)
{
    stream << static_cast<int>(value);
    return 0;
}

inline
int baejsn_PrintUtil::printValueImp(bsl::ostream& stream, unsigned short value)
{
    stream << value;
    return 0;
}

inline
int baejsn_PrintUtil::printValueImp(bsl::ostream& stream, unsigned int value)
{
    stream << value;
    return 0;
}

inline
int baejsn_PrintUtil::printValueImp(bsl::ostream&       stream,
                                    bsls::Types::Uint64 value)
{
    stream << value;
    return 0;
}

inline
int baejsn_PrintUtil::printValueImp(bsl::ostream& stream, float value)
{
    return printDecimal(stream, value);
}

inline
int baejsn_PrintUtil::printValueImp(bsl::ostream& stream, double value)
{
    return printDecimal(stream, value);
}

inline
int baejsn_PrintUtil::printValueImp(bsl::ostream& stream, const char * value)
{
    return printString(stream, value);
}

inline
int baejsn_PrintUtil::printValueImp(bsl::ostream&      stream,
                                    const bsl::string& value)
{
    return printString(stream, value);
}

inline
int baejsn_PrintUtil::printValueImp(bsl::ostream&    stream,
                                    const bdet_Time& value)
{
    return printDateAndTime(stream, value);
}

inline
int baejsn_PrintUtil::printValueImp(bsl::ostream&    stream,
                                    const bdet_Date& value)
{
    return printDateAndTime(stream, value);
}

inline
int baejsn_PrintUtil::printValueImp(bsl::ostream&        stream,
                                    const bdet_Datetime& value)
{
    return printDateAndTime(stream, value);
}

inline
int baejsn_PrintUtil::printValueImp(bsl::ostream&      stream,
                                    const bdet_TimeTz& value)
{
    return printDateAndTime(stream, value);
}

inline
int baejsn_PrintUtil::printValueImp(bsl::ostream&      stream,
                                    const bdet_DateTz& value)
{
    return printDateAndTime(stream, value);
}

inline
int baejsn_PrintUtil::printValueImp(bsl::ostream&          stream,
                                    const bdet_DatetimeTz& value)
{
    return printDateAndTime(stream, value);
}


// CLASS METHODS
template <class TYPE>
int baejsn_PrintUtil::printValue(bsl::ostream& stream,
                                 const TYPE&   value)
{
    return printValueImp(stream, value);
}

}  // close namespace BloombergLP

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
