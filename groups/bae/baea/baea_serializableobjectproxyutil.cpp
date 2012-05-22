// baea_serializableobjectproxyutil.cpp                               -*-C++-*-
#include <baea_serializableobjectproxyutil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baea_serializableobjectproxyutil_cpp,"$Id$ $CSID$")

#include <baea_testmessages.h>  // for testing only

namespace BloombergLP {
namespace baea {

namespace {

void voidAccessorFn(SerializableObjectProxy*,
                    const SerializableObjectProxy&,
                    int)
    // This function should never be called.  NUM_ATTRIBUTES is 0.
{
    BSLS_ASSERT_OPT(!"UNREACHABLE");
}

}  // close unnamed namespace


void SerializableObjectProxyUtil::makeProxyForEmptySequence(
                                            SerializableObjectProxy *proxy,
                                            const char              *className)
{
    proxy->loadSequence(0, 0, 0, className, &voidAccessorFn);
}

void SerializableObjectProxyUtil::makeEncodeProxy(
                                            SerializableObjectProxy *proxy,
                                            bsl::vector<char>       *object,
                                            bdeat_TypeCategory::Array)
{
    proxy->loadArray(object->size(), sizeof(char), object, 0);
}

}  // close namespace baea
}  // close namespace BloombergLP
// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
