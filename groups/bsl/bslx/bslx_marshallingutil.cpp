// bslx_marshallingutil.cpp                                           -*-C++-*-
#include <bslx_marshallingutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslx_marshallingutil_cpp,"$Id$ $CSID$")

namespace BloombergLP {
namespace bslx {

                        // ----------------------
                        // struct MarshallingUtil
                        // ----------------------

// CLASS METHODS

                        // *** put arrays of integral values ***

void MarshallingUtil::putArrayInt64(char                     *buffer,
                                    const bsls::Types::Int64 *values,
                                    int                       numValues)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    const bsls::Types::Int64 *end = values + numValues;
    for (; values != end; ++values) {
        putInt64(buffer, *values);
        buffer += k_SIZEOF_INT64;
    }
}

void MarshallingUtil::putArrayInt64(char                      *buffer,
                                    const bsls::Types::Uint64 *values,
                                    int                        numValues)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    const bsls::Types::Uint64 *end = values + numValues;
    for (; values != end; ++values) {
        putInt64(buffer, *values);
        buffer += k_SIZEOF_INT64;
    }
}

void MarshallingUtil::putArrayInt56(char                     *buffer,
                                    const bsls::Types::Int64 *values,
                                    int                       numValues)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    const bsls::Types::Int64 *end = values + numValues;
    for (; values != end; ++values) {
        putInt56(buffer, *values);
        buffer += k_SIZEOF_INT56;
    }
}

void MarshallingUtil::putArrayInt56(char                      *buffer,
                                    const bsls::Types::Uint64 *values,
                                    int                        numValues)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    const bsls::Types::Uint64 *end = values + numValues;
    for (; values != end; ++values) {
        putInt56(buffer, *values);
        buffer += k_SIZEOF_INT56;
    }
}

void MarshallingUtil::putArrayInt48(char                     *buffer,
                                    const bsls::Types::Int64 *values,
                                    int                       numValues)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    const bsls::Types::Int64 *end = values + numValues;
    for (; values != end; ++values) {
        putInt48(buffer, *values);
        buffer += k_SIZEOF_INT48;
    }
}

void MarshallingUtil::putArrayInt48(char                      *buffer,
                                    const bsls::Types::Uint64 *values,
                                    int                        numValues)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    const bsls::Types::Uint64 *end = values + numValues;
    for (; values != end; ++values) {
        putInt48(buffer, *values);
        buffer += k_SIZEOF_INT48;
    }
}

void MarshallingUtil::putArrayInt40(char                     *buffer,
                                    const bsls::Types::Int64 *values,
                                    int                       numValues)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    const bsls::Types::Int64 *end = values + numValues;
    for (; values != end; ++values) {
        putInt40(buffer, *values);
        buffer += k_SIZEOF_INT40;
    }
}

void MarshallingUtil::putArrayInt40(char                      *buffer,
                                    const bsls::Types::Uint64 *values,
                                    int                        numValues)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    const bsls::Types::Uint64 *end = values + numValues;
    for (; values != end; ++values) {
        putInt40(buffer, *values);
        buffer += k_SIZEOF_INT40;
    }
}

void MarshallingUtil::putArrayInt32(char      *buffer,
                                    const int *values,
                                    int        numValues)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    const int *end = values + numValues;
    for (; values != end; ++values) {
        putInt32(buffer, *values);
        buffer += k_SIZEOF_INT32;
    }
}

void MarshallingUtil::putArrayInt32(char               *buffer,
                                    const unsigned int *values,
                                    int                 numValues)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    const unsigned int *end = values + numValues;
    for (; values != end; ++values) {
        putInt32(buffer, *values);
        buffer += k_SIZEOF_INT32;
    }
}

void MarshallingUtil::putArrayInt24(char      *buffer,
                                    const int *values,
                                    int        numValues)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    const int *end = values + numValues;
    for (; values != end; ++values) {
        putInt24(buffer, *values);
        buffer += k_SIZEOF_INT24;
    }
}

void MarshallingUtil::putArrayInt24(char               *buffer,
                                    const unsigned int *values,
                                    int                 numValues)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    const unsigned int *end = values + numValues;
    for (; values != end; ++values) {
        putInt24(buffer, *values);
        buffer += k_SIZEOF_INT24;
    }
}

void MarshallingUtil::putArrayInt16(char        *buffer,
                                    const short *values,
                                    int          numValues)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    const short *end = values + numValues;
    for (; values != end; ++values) {
        putInt16(buffer, *values);
        buffer += k_SIZEOF_INT16;
    }
}

void MarshallingUtil::putArrayInt16(char                 *buffer,
                                    const unsigned short *values,
                                    int                   numValues)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    const unsigned short *end = values + numValues;
    for (; values != end; ++values) {
        putInt16(buffer, *values);
        buffer += k_SIZEOF_INT16;
    }
}

                        // *** put arrays of floating-point values ***

void MarshallingUtil::putArrayFloat64(char         *buffer,
                                      const double *values,
                                      int           numValues)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    const double *end = values + numValues;
    for (; values < end; ++values) {
        putFloat64(buffer, *values);
        buffer += k_SIZEOF_FLOAT64;
    }
}

void MarshallingUtil::putArrayFloat32(char        *buffer,
                                      const float *values,
                                      int          numValues)
{
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(values);
    BSLS_ASSERT(0 <= numValues);

    const float *end = values + numValues;
    for (; values < end; ++values) {
        putFloat32(buffer, *values);
        buffer += k_SIZEOF_FLOAT32;
    }
}

                        // *** get arrays of integral values ***

void MarshallingUtil::getArrayInt64(bsls::Types::Int64 *variables,
                                    const char         *buffer,
                                    int                 numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= numVariables);

    const bsls::Types::Int64 *end = variables + numVariables;
    for (; variables != end; ++variables) {
        getInt64(variables, buffer);
        buffer += k_SIZEOF_INT64;
    }
}

void MarshallingUtil::getArrayUint64(bsls::Types::Uint64 *variables,
                                     const char          *buffer,
                                     int                  numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= numVariables);

    const bsls::Types::Uint64 *end = variables + numVariables;
    for (; variables != end; ++variables) {
        getUint64(variables, buffer);
        buffer += k_SIZEOF_INT64;
    }
}

void MarshallingUtil::getArrayInt56(bsls::Types::Int64 *variables,
                                    const char         *buffer,
                                    int                 numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= numVariables);

    const bsls::Types::Int64 *end = variables + numVariables;
    for (; variables != end; ++variables) {
        getInt56(variables, buffer);
        buffer += k_SIZEOF_INT56;
    }
}

void MarshallingUtil::getArrayUint56(bsls::Types::Uint64 *variables,
                                     const char          *buffer,
                                     int                  numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= numVariables);

    const bsls::Types::Uint64 *end = variables + numVariables;
    for (; variables != end; ++variables) {
        getUint56(variables, buffer);
        buffer += k_SIZEOF_INT56;
    }
}

void MarshallingUtil::getArrayInt48(bsls::Types::Int64 *variables,
                                    const char         *buffer,
                                    int                 numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= numVariables);

    const bsls::Types::Int64 *end = variables + numVariables;
    for (; variables != end; ++variables) {
        getInt48(variables, buffer);
        buffer += k_SIZEOF_INT48;
    }
}

void MarshallingUtil::getArrayUint48(bsls::Types::Uint64 *variables,
                                     const char          *buffer,
                                     int                  numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= numVariables);

    const bsls::Types::Uint64 *end = variables + numVariables;
    for (; variables != end; ++variables) {
        getUint48(variables, buffer);
        buffer += k_SIZEOF_INT48;
    }
}

void MarshallingUtil::getArrayInt40(bsls::Types::Int64 *variables,
                                    const char         *buffer,
                                    int                 numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= numVariables);

    const bsls::Types::Int64 *end = variables + numVariables;
    for (; variables != end; ++variables) {
        getInt40(variables, buffer);
        buffer += k_SIZEOF_INT40;
    }
}

void MarshallingUtil::getArrayUint40(bsls::Types::Uint64 *variables,
                                     const char          *buffer,
                                     int                  numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= numVariables);

    const bsls::Types::Uint64 *end = variables + numVariables;
    for (; variables != end; ++variables) {
        getUint40(variables, buffer);
        buffer += k_SIZEOF_INT40;
    }
}

void MarshallingUtil::getArrayInt32(int        *variables,
                                    const char *buffer,
                                    int         numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= numVariables);

    const int *end = variables + numVariables;
    for (; variables != end; ++variables) {
        getInt32(variables, buffer);
        buffer += k_SIZEOF_INT32;
    }
}

void MarshallingUtil::getArrayUint32(unsigned int *variables,
                                     const char   *buffer,
                                     int           numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= numVariables);

    const unsigned int *end = variables + numVariables;
    for (; variables != end; ++variables) {
        getUint32(variables, buffer);
        buffer += k_SIZEOF_INT32;
    }
}

void MarshallingUtil::getArrayInt24(int        *variables,
                                    const char *buffer,
                                    int         numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= numVariables);

    const int *end = variables + numVariables;
    for (; variables != end; ++variables) {
        getInt24(variables, buffer);
        buffer += k_SIZEOF_INT24;
    }
}

void MarshallingUtil::getArrayUint24(unsigned int *variables,
                                     const char   *buffer,
                                     int           numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= numVariables);

    const unsigned int *end = variables + numVariables;
    for (; variables != end; ++variables) {
        getUint24(variables, buffer);
        buffer += k_SIZEOF_INT24;
    }
}

void MarshallingUtil::getArrayInt16(short      *variables,
                                    const char *buffer,
                                    int         numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= numVariables);

    const short *end = variables + numVariables;
    for (; variables != end; ++variables) {
        getInt16(variables, buffer);
        buffer += k_SIZEOF_INT16;
    }
}

void MarshallingUtil::getArrayUint16(unsigned short *variables,
                                     const char     *buffer,
                                     int             numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= numVariables);

    const unsigned short *end = variables + numVariables;
    for (; variables != end; ++variables) {
        getUint16(variables, buffer);
        buffer += k_SIZEOF_INT16;
    }
}

                        // *** get arrays of floating-point variables ***

void MarshallingUtil::getArrayFloat64(double     *variables,
                                      const char *buffer,
                                      int         numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= numVariables);

    const double *end = variables + numVariables;
    for (; variables != end; ++variables) {
        getFloat64(variables, buffer);
        buffer += k_SIZEOF_FLOAT64;
    }
}

void MarshallingUtil::getArrayFloat32(float      *variables,
                                      const char *buffer,
                                      int         numVariables)
{
    BSLS_ASSERT(variables);
    BSLS_ASSERT(buffer);
    BSLS_ASSERT(0 <= numVariables);

    const float *end = variables + numVariables;
    for (; variables != end; ++variables) {
        getFloat32(variables, buffer);
        buffer += k_SIZEOF_INT32;
    }
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
