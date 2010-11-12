// bdema_buffermanager.cpp                                            -*-C++-*-
#include <bdema_buffermanager.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdema_buffermanager_cpp,"$Id$ $CSID$")

namespace BloombergLP {

                        // -------------------------
                        // class bdema_BufferManager
                        // -------------------------

int bdema_BufferManager::expand(void *address, int size)
{
    BSLS_ASSERT(address);
    BSLS_ASSERT(0 < size);
    BSLS_ASSERT(d_buffer_p);
    BSLS_ASSERT(0 <= d_cursor);
    BSLS_ASSERT(d_cursor <= d_bufferSize);

    if (static_cast<char *>(address) + size == d_buffer_p + d_cursor) {
        const int newSize = size + d_bufferSize - d_cursor;
        d_cursor = d_bufferSize;

        return newSize;                                               // RETURN
    }

    return size;
}

int bdema_BufferManager::truncate(void *address, int originalSize, int newSize)
{
    BSLS_ASSERT(address);
    BSLS_ASSERT(0 <= newSize);
    BSLS_ASSERT(newSize <= originalSize);
    BSLS_ASSERT(d_buffer_p);
    BSLS_ASSERT(0 <= d_cursor);
    BSLS_ASSERT(d_cursor <= d_bufferSize);

    if (static_cast<char *>(address) + originalSize == d_buffer_p + d_cursor) {
        d_cursor -= originalSize - newSize;
        return newSize;                                               // RETURN
    }

    return originalSize;
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
