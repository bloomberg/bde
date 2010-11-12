// baenet_httpresponseheader.cpp  -*-C++-*-
#include <baenet_httpresponseheader.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baenet_httpresponseheader_cpp,"$Id$ $CSID$")

#include <bsl_ostream.h>

#include <bdeu_print.h>
#include <bdeu_printmethods.h>

#include <baenet_httpparserutil.h>

namespace BloombergLP {

namespace {

                            // ===================
                            // class AddFieldProxy
                            // ===================

class AddFieldProxy {
    // Proxy class to filter out INFO_TYPE before passing to
    // 'baenet_HttpParserUtilAddField'.

    // PRIVATE DATA MEMBERS
    baenet_HttpParserUtilAddField d_addField;

    // NOT IMPLEMENTED
    AddFieldProxy(const AddFieldProxy&);
    AddFieldProxy& operator=(const AddFieldProxy&);

  public:
    // CREATORS
    explicit AddFieldProxy(const bdeut_StringRef *fieldValue)
    : d_addField(fieldValue)
    {
    }

    ~AddFieldProxy()
    {
    }

    // MANIPULATORS
    template <typename TYPE>
    int operator()(TYPE *object)
    {
        return d_addField(object);
    }

    template <typename TYPE, typename INFO_TYPE>
    int operator()(TYPE *object, const INFO_TYPE&)
    {
        return d_addField(object);
    }
};

}  // close unnamed namespace

                         // -------------------------
                         // baenet_HttpResponseHeader
                         // -------------------------

// ACCESSORS

bsl::ostream& baenet_HttpResponseHeader::print(
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
        stream << "BasicFields = ";
        bdeu_PrintMethods::print(stream, d_basicFields,
                             -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "ResponseFields = ";
        bdeu_PrintMethods::print(stream, d_responseFields,
                             -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, levelPlus1, spacesPerLevel);
        stream << "ExtendedFields = ";
        bdeu_PrintMethods::print(stream, d_extendedFields,
                             -levelPlus1, spacesPerLevel);

        bdeu_Print::indent(stream, level, spacesPerLevel);
        stream << "]\n";
    }
    else {
        // single line

        stream << '[';

        stream << ' ';
        stream << "BasicFields = ";
        bdeu_PrintMethods::print(stream, d_basicFields,
                             -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "ResponseFields = ";
        bdeu_PrintMethods::print(stream, d_responseFields,
                             -levelPlus1, spacesPerLevel);

        stream << ' ';
        stream << "ExtendedFields = ";
        bdeu_PrintMethods::print(stream, d_extendedFields,
                             -levelPlus1, spacesPerLevel);

        stream << " ]";
    }

    return stream << bsl::flush;
}

// MANIPULATORS (FROM 'baenet_HttpHeader')

int baenet_HttpResponseHeader::addField(const bdeut_StringRef& fieldName,
                                        const bdeut_StringRef& fieldValue)
{
    const bdeat_AttributeInfo *info;

    AddFieldProxy addField(&fieldValue);

    info = baenet_HttpBasicHeaderFields::lookupAttributeInfo(
                                                           fieldName.data(),
                                                           fieldName.length());

    if (info) {
        return d_basicFields.manipulateAttribute(addField, info->id());
    }

    info = baenet_HttpResponseHeaderFields::lookupAttributeInfo(
                                                           fieldName.data(),
                                                           fieldName.length());

    if (info) {
        return d_responseFields.manipulateAttribute(addField, info->id());
    }

    baenet_HttpExtendedHeaderFields::FieldValueIterator it
                                   = d_extendedFields.addFieldValue(fieldName);

    return addField(&(*it));
}

}  // close namespace BloombergLP;

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
