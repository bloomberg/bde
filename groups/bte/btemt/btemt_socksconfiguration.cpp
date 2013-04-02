// btemt_socksconfiguration.cpp   -*-C++-*-   GENERATED FILE -- DO NOT EDIT

#include <bdes_ident.h>
BDES_IDENT_RCSID(bbcn_socksconfiguration_cpp,"$Id$ $CSID$ $CCId$")

#include <btemt_socksconfiguration.h>

#include <bdeat_formattingmode.h>
#include <bdeat_valuetypefunctions.h>
#include <bdeu_print.h>
#include <bdeu_printmethods.h>
#include <bdeu_string.h>

#include <bsl_string.h>

#include <bsls_assert.h>

#include <iomanip>
#include <limits>
#include <ostream>

namespace BloombergLP {
namespace btemt {

                          // ------------------------                          
                          // class SocksConfiguration                          
                          // ------------------------                          

// CONSTANTS

const char SocksConfiguration::CLASS_NAME[] = "SocksConfiguration";

const bdeat_AttributeInfo SocksConfiguration::ATTRIBUTE_INFO_ARRAY[] = {
    {
        ATTRIBUTE_ID_DESTINATION_IP_ADDR,
        "destinationIpAddr",
        sizeof("destinationIpAddr") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_DESTINATION_PORT,
        "destinationPort",
        sizeof("destinationPort") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    },
    {
        ATTRIBUTE_ID_SOCKS_IP_ADDR,
        "socksIpAddr",
        sizeof("socksIpAddr") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_SOCKS_PORT,
        "socksPort",
        sizeof("socksPort") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    },
    {
        ATTRIBUTE_ID_TIMEOUT,
        "timeout",
        sizeof("timeout") - 1,
        "",
        bdeat_FormattingMode::BDEAT_DEC
    },
    {
        ATTRIBUTE_ID_DO_AUTHENTICATION,
        "doAuthentication",
        sizeof("doAuthentication") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_USERNAME,
        "username",
        sizeof("username") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    },
    {
        ATTRIBUTE_ID_PASSWORD,
        "password",
        sizeof("password") - 1,
        "",
        bdeat_FormattingMode::BDEAT_TEXT
    }
};

// CLASS METHODS

const bdeat_AttributeInfo *SocksConfiguration::lookupAttributeInfo(
        const char *name,
        int         nameLength)
{
    switch(nameLength) {
        case 7: {
            if (name[0]=='t'
             && name[1]=='i'
             && name[2]=='m'
             && name[3]=='e'
             && name[4]=='o'
             && name[5]=='u'
             && name[6]=='t')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TIMEOUT];
            }
        } break;
        case 8: {
            switch(name[0]) {
                case 'p': {
                    if (name[1]=='a'
                     && name[2]=='s'
                     && name[3]=='s'
                     && name[4]=='w'
                     && name[5]=='o'
                     && name[6]=='r'
                     && name[7]=='d')
                    {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PASSWORD];
                    }
                } break;
                case 'u': {
                    if (name[1]=='s'
                     && name[2]=='e'
                     && name[3]=='r'
                     && name[4]=='n'
                     && name[5]=='a'
                     && name[6]=='m'
                     && name[7]=='e')
                    {
                        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_USERNAME];
                    }
                } break;
            }
        } break;
        case 9: {
            if (name[0]=='s'
             && name[1]=='o'
             && name[2]=='c'
             && name[3]=='k'
             && name[4]=='s'
             && name[5]=='P'
             && name[6]=='o'
             && name[7]=='r'
             && name[8]=='t')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SOCKS_PORT];
            }
        } break;
        case 11: {
            if (name[0]=='s'
             && name[1]=='o'
             && name[2]=='c'
             && name[3]=='k'
             && name[4]=='s'
             && name[5]=='I'
             && name[6]=='p'
             && name[7]=='A'
             && name[8]=='d'
             && name[9]=='d'
             && name[10]=='r')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SOCKS_IP_ADDR];
            }
        } break;
        case 15: {
            if (name[0]=='d'
             && name[1]=='e'
             && name[2]=='s'
             && name[3]=='t'
             && name[4]=='i'
             && name[5]=='n'
             && name[6]=='a'
             && name[7]=='t'
             && name[8]=='i'
             && name[9]=='o'
             && name[10]=='n'
             && name[11]=='P'
             && name[12]=='o'
             && name[13]=='r'
             && name[14]=='t')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DESTINATION_PORT];
            }
        } break;
        case 16: {
            if (name[0]=='d'
             && name[1]=='o'
             && name[2]=='A'
             && name[3]=='u'
             && name[4]=='t'
             && name[5]=='h'
             && name[6]=='e'
             && name[7]=='n'
             && name[8]=='t'
             && name[9]=='i'
             && name[10]=='c'
             && name[11]=='a'
             && name[12]=='t'
             && name[13]=='i'
             && name[14]=='o'
             && name[15]=='n')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DO_AUTHENTICATION];
            }
        } break;
        case 17: {
            if (name[0]=='d'
             && name[1]=='e'
             && name[2]=='s'
             && name[3]=='t'
             && name[4]=='i'
             && name[5]=='n'
             && name[6]=='a'
             && name[7]=='t'
             && name[8]=='i'
             && name[9]=='o'
             && name[10]=='n'
             && name[11]=='I'
             && name[12]=='p'
             && name[13]=='A'
             && name[14]=='d'
             && name[15]=='d'
             && name[16]=='r')
            {
                return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DESTINATION_IP_ADDR];
            }
        } break;
    }
    return 0;
}

const bdeat_AttributeInfo *SocksConfiguration::lookupAttributeInfo(int id)
{
    switch (id) {
      case ATTRIBUTE_ID_DESTINATION_IP_ADDR:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DESTINATION_IP_ADDR];
      case ATTRIBUTE_ID_DESTINATION_PORT:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DESTINATION_PORT];
      case ATTRIBUTE_ID_SOCKS_IP_ADDR:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SOCKS_IP_ADDR];
      case ATTRIBUTE_ID_SOCKS_PORT:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_SOCKS_PORT];
      case ATTRIBUTE_ID_TIMEOUT:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_TIMEOUT];
      case ATTRIBUTE_ID_DO_AUTHENTICATION:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_DO_AUTHENTICATION];
      case ATTRIBUTE_ID_USERNAME:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_USERNAME];
      case ATTRIBUTE_ID_PASSWORD:
        return &ATTRIBUTE_INFO_ARRAY[ATTRIBUTE_INDEX_PASSWORD];
      default:
        return 0;
    }
}

// CREATORS

SocksConfiguration::SocksConfiguration(bslma::Allocator *basicAllocator)
: d_destinationIpAddr(basicAllocator)
, d_socksIpAddr(basicAllocator)
, d_username(basicAllocator)
, d_password(basicAllocator)
, d_destinationPort()
, d_socksPort()
, d_timeout()
, d_doAuthentication()
{
}

SocksConfiguration::SocksConfiguration(const SocksConfiguration& original,
                                       bslma::Allocator *basicAllocator)
: d_destinationIpAddr(original.d_destinationIpAddr, basicAllocator)
, d_socksIpAddr(original.d_socksIpAddr, basicAllocator)
, d_username(original.d_username, basicAllocator)
, d_password(original.d_password, basicAllocator)
, d_destinationPort(original.d_destinationPort)
, d_socksPort(original.d_socksPort)
, d_timeout(original.d_timeout)
, d_doAuthentication(original.d_doAuthentication)
{
}

SocksConfiguration::~SocksConfiguration()
{
}

// MANIPULATORS

SocksConfiguration&
SocksConfiguration::operator=(const SocksConfiguration& rhs)
{
    if (this != &rhs) {
        d_destinationIpAddr = rhs.d_destinationIpAddr;
        d_destinationPort = rhs.d_destinationPort;
        d_socksIpAddr = rhs.d_socksIpAddr;
        d_socksPort = rhs.d_socksPort;
        d_timeout = rhs.d_timeout;
        d_doAuthentication = rhs.d_doAuthentication;
        d_username = rhs.d_username;
        d_password = rhs.d_password;
    }
    return *this;
}

void SocksConfiguration::reset()
{
    bdeat_ValueTypeFunctions::reset(&d_destinationIpAddr);
    bdeat_ValueTypeFunctions::reset(&d_destinationPort);
    bdeat_ValueTypeFunctions::reset(&d_socksIpAddr);
    bdeat_ValueTypeFunctions::reset(&d_socksPort);
    bdeat_ValueTypeFunctions::reset(&d_timeout);
    bdeat_ValueTypeFunctions::reset(&d_doAuthentication);
    bdeat_ValueTypeFunctions::reset(&d_username);
    bdeat_ValueTypeFunctions::reset(&d_password);
}

// ACCESSORS

bsl::ostream& SocksConfiguration::print(
    bsl::ostream& stream,
    int           level,
    int           spacesPerLevel) const
{
    if (level < 0) {
        level = -level;
    }
    else {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    int levelPlus1 = level + 1;

    if (0 <= spacesPerLevel) {
        // multiline

        stream << "[\n";

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "DestinationIpAddr = ";
        bdeu_PrintMethods::print(stream, d_destinationIpAddr,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "DestinationPort = ";
        bdeu_PrintMethods::print(stream, d_destinationPort,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "SocksIpAddr = ";
        bdeu_PrintMethods::print(stream, d_socksIpAddr,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "SocksPort = ";
        bdeu_PrintMethods::print(stream, d_socksPort,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Timeout = ";
        bdeu_PrintMethods::print(stream, d_timeout,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "DoAuthentication = ";
        bdeu_PrintMethods::print(stream, d_doAuthentication,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Username = ";
        bdeu_PrintMethods::print(stream, d_username,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "Password = ";
        bdeu_PrintMethods::print(stream, d_password,
                                 -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "DestinationIpAddr = ";
        bdeu_PrintMethods::print(stream, d_destinationIpAddr,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "DestinationPort = ";
        bdeu_PrintMethods::print(stream, d_destinationPort,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "SocksIpAddr = ";
        bdeu_PrintMethods::print(stream, d_socksIpAddr,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "SocksPort = ";
        bdeu_PrintMethods::print(stream, d_socksPort,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Timeout = ";
        bdeu_PrintMethods::print(stream, d_timeout,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "DoAuthentication = ";
        bdeu_PrintMethods::print(stream, d_doAuthentication,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Username = ";
        bdeu_PrintMethods::print(stream, d_username,
                                 -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "Password = ";
        bdeu_PrintMethods::print(stream, d_password,
                                 -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}


}  // close namespace bbcn
}  // close namespace BloombergLP

// GENERATED BY BLP_BAS_CODEGEN_3.5.3 
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
