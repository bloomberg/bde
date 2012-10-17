// baejsn_encoder.cpp                                                 -*-C++-*-
#include <baejsn_encoder.h>
#include <baejsn_decoder.h>  // for testing only

namespace BloombergLP {

// PRIVATE MANIPULATORS
bsl::ostream& baejsn_Encoder::logStream()
{
    if (0 == d_logStream) {
        d_logStream = new(d_logArea.buffer()) bsl::ostringstream;
    }
    return *d_logStream;
}

void baejsn_Encoder::encodeSimple(const char value)
{
    std::ostringstream str;

    str << "\\u00" << std::hex << std::setfill('0') << std::setw(2) << (static_cast<unsigned int>(value) & 0xff);

    d_outputStream << '"' << str.str() << '"';
}

void baejsn_Encoder::encodeSimple(const std::string & value)
{
    d_outputStream << '"';

    for (std::string::const_iterator it = value.begin(); it != value.end(); ++it)
    {
        if (*it == '"' || *it == '\\' || *it == '/') // printable (but miss-enterpreted)
            d_outputStream << '\\' << *it;
        else if (*it == '\b') // non printable
            d_outputStream << "\\b";
        else if (*it == '\f')
            d_outputStream << "\\f";
        else if (*it == '\n')
            d_outputStream << "\\n";
        else if (*it == '\r')
            d_outputStream << "\\r";
        else if (*it == '\t')
            d_outputStream << "\\t";
        // TBD: Disable mOptions for now.
        //else if (*it == '\0' && mOptions[Options::useModifiedUtf8])
        //{
        //    d_outputStream << "\\uc080";
        //}
        //else if ((static_cast<unsigned int>(*it) & 0xff) < 32) // any other control characters as hex
        //{
        //    // todo add back multi byte support, but unicode >= 0xD800 && unicode <= 0xDBFF checks are required to ensure that
        //    //      surrogate encodings are or are not needed when pushing binary as a string
        //    //
        //    //std::string::const_iterator next = it;
        //    //++next;

        //    std::ostringstream str;

        //    //if (*it == 0 || next == value.end())
        //    {
        //        str << "\\u00" << std::hex << std::setfill('0') << std::setw(2) << (static_cast<unsigned int>(*it) & 0xff);
        //    }
        //    //else
        //    //{
        //    //    str << "\\u" << std::hex << std::setfill('0') << std::setw(2) << (static_cast<unsigned int>(*it) & 0xff)
        //    //                 << std::hex << std::setfill('0') << std::setw(2) << (static_cast<unsigned int>(*next) & 0xff);
        //    //    ++it;
        //    //}

        //    d_outputStream << str.str();
        //}
        else
        {
            d_outputStream << *it;
        }
    }

    d_outputStream << '"';
}

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
