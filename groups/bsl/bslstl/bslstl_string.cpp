// bslstl_string.cpp                                                  -*-C++-*-
#include <bslstl_string.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

// IMPLEMENTATION NOTES:
// This string class implements a "short string optimization" which optimizes
// the handling of short strings (strings shorter than a certain length) by
// putting them into an internal short string buffer.  The short string buffer
// is a part of the 'string' object and it doesn't require any memory
// allocations.  This avoids memory allocations/deallocations on operations on
// short strings such as: construction, copy-construction, copy-assignment,
// destruction.  Those operations are much cheaper now than they used to be
// when they required memory allocations.
//
// There are also some side-effects of the short string optimization.  The
// footprint of the 'string' object is larger now than it used to be due to the
// short string buffer.  And the default-constructed object may now have a
// non-zero capacity.  But the default constructor of 'string' still doesn't
// require any memory and cannot fail.

#ifndef INCLUDED_BSLSTL_STRING_CPP
#define INCLUDED_BSLSTL_STRING_CPP

#ifdef BSLS_COMPILERFEATURES_SUPPORT_EXTERN_TEMPLATE
template class bsl::String_Imp<char, bsl::string::size_type>;
template class bsl::String_Imp<wchar_t, bsl::wstring::size_type>;
template class bsl::basic_string<char>;
template class bsl::basic_string<wchar_t>;
#endif


std::size_t bsl::hashBasicString(const string& str)
{
    return hashBasicString<char, char_traits<char>, allocator<char> >(str);
}

std::size_t bsl::hashBasicString(const wstring& str)
{
    return hashBasicString<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >(
                                                                          str);
}

int bsl::stoi(const string& str, std::size_t* pos, int base){
    char* ptr;
    long value = std::strtol(str.c_str(), &ptr, base);
    if (errno == ERANGE){
        errno = 0;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange("stoi");
    }
    else if (ptr == str.c_str()){
        BloombergLP::bslstl::StdExceptUtil::throwInvalidArgument("stoi");
    }
    *pos = ptr - str.c_str();
    return value;
}

int bsl::stoi(const wstring& str, std::size_t* pos, int base){
    wchar_t* ptr;
    long value = std::wcstol(str.c_str(), &ptr, base);

    if (errno == ERANGE){
        errno = 0;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange("stoi");
    }
    else if (ptr == str.c_str()){
        BloombergLP::bslstl::StdExceptUtil::throwInvalidArgument("stoi");
    }

    *pos = ptr - str.c_str();
    return value;
}

long bsl::stol(const string& str, std::size_t* pos, int base){
    char* ptr;
    long value = std::strtol(str.c_str(), &ptr, base);

    if (errno == ERANGE){
        errno = 0;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange("stol");
    }
    else if (ptr == str.c_str()){
        BloombergLP::bslstl::StdExceptUtil::throwInvalidArgument("stol");
    }

    *pos = ptr - str.c_str();
    return value;
}

long bsl::stol(const wstring& str, std::size_t* pos, int base){
    wchar_t* ptr;
    long value = std::wcstol(str.c_str(), &ptr, base);

    if (errno == ERANGE){
        errno = 0;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange("stol");
    }
    else if (ptr == str.c_str()){
        BloombergLP::bslstl::StdExceptUtil::throwInvalidArgument("stol");
    }

    *pos = ptr - str.c_str();
    return value;
}

unsigned long bsl::stoul(const string& str, std::size_t* pos, int base){
    char* ptr;
    unsigned long value = std::strtoul(str.c_str(), &ptr, base);

    if (errno == ERANGE){
        errno = 0;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange("stoul");
    }
    else if (ptr == str.c_str()){
        BloombergLP::bslstl::StdExceptUtil::throwInvalidArgument("stoul");
    }

    *pos = ptr - str.c_str();
    return value;
}

unsigned long bsl::stoul(const wstring& str,
                                                   std::size_t* pos, int base){
    wchar_t* ptr;
    unsigned long value = std::wcstoul(str.c_str(), &ptr, base);

    if (errno == ERANGE){
        errno = 0;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange("stoul");
    }
    else if (ptr == str.c_str()){
        BloombergLP::bslstl::StdExceptUtil::throwInvalidArgument("stoul");
    }

    *pos = ptr - str.c_str();
    return value;
}
#if __cplusplus >= 201103L
long long bsl::stoll(const string& str, std::size_t* pos, int base){
    char* ptr;
    long long value = std::strtoll(str.c_str(), &ptr, base);

    if (errno == ERANGE){
        errno = 0;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange("stoll");
    }
    else if (ptr == str.c_str()){
        BloombergLP::bslstl::StdExceptUtil::throwInvalidArgument("stoll");
    }

    *pos = ptr - str.c_str();
    return value;
}
long long bsl::stoll(const wstring& str, std::size_t* pos, int base){
    wchar_t* ptr;
    long long value = std::wcstoll(str.c_str(), &ptr, base);

    if (errno == ERANGE){
        errno = 0;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange("stoll");
    }
    else if (ptr == str.c_str()){
        BloombergLP::bslstl::StdExceptUtil::throwInvalidArgument("stoll");
    }

    *pos = ptr - str.c_str();
    return value;
}
unsigned long long bsl::stoull(const string& str, std::size_t* pos,
                                                                     int base){
    char* ptr;
    unsigned long long value = std::strtoull(str.c_str(), &ptr, base);

    if (errno == ERANGE){
        errno = 0;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange("stoull");
    }
    else if (ptr == str.c_str()){
        BloombergLP::bslstl::StdExceptUtil::throwInvalidArgument("stoull");
    }

    *pos = ptr - str.c_str();
    return value;
}
unsigned long long bsl::stoull(const wstring& str, std::size_t* pos,
                                                                     int base){
    wchar_t* ptr;
    unsigned long long value = std::wcstoull(str.c_str(), &ptr, base);

    if (errno == ERANGE){
        errno = 0;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange("stoull");
    }
    else if (ptr == str.c_str()){
        BloombergLP::bslstl::StdExceptUtil::throwInvalidArgument("stoull");
    }

    *pos = ptr - str.c_str();
    return value;
}
#endif

float bsl::stof(const string& str, std::size_t* pos){
    char* ptr;
    float value = std::strtod(str.c_str(), &ptr);

    if (errno == ERANGE){
        errno = 0;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange("stof");
    }
    else if (ptr == str.c_str()){
        BloombergLP::bslstl::StdExceptUtil::throwInvalidArgument("stof");
    }

    *pos = ptr - str.c_str();
    return value;
}
float bsl::stof(const wstring& str, std::size_t* pos){
    wchar_t* ptr;
    float value = std::wcstod(str.c_str(), &ptr);

    if (errno == ERANGE){
        errno = 0;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange("stof");
    }
    else if (ptr == str.c_str()){
        BloombergLP::bslstl::StdExceptUtil::throwInvalidArgument("stof");
    }

    *pos = ptr - str.c_str();
    return value;
}
double bsl::stod(const string& str, std::size_t* pos){
    char* ptr;
    double value = std::strtod(str.c_str(), &ptr);

    if (errno == ERANGE){
        errno = 0;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange("stod");
    }
    else if (ptr == str.c_str()){
        BloombergLP::bslstl::StdExceptUtil::throwInvalidArgument("stod");
    }

    *pos = ptr - str.c_str();
    return value;
}
double bsl::stod(const wstring& str, std::size_t* pos){
    wchar_t* ptr;
    double value = std::wcstod(str.c_str(), &ptr);

    if (errno == ERANGE){
        errno = 0;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange("stod");
    }
    else if (ptr == str.c_str()){
        BloombergLP::bslstl::StdExceptUtil::throwInvalidArgument("stod");
    }

    *pos = ptr - str.c_str();
    return value;
}
#if __cplusplus >= 201103L
long double bsl::stold(const string& str, std::size_t* pos){
    char* ptr;
    long double value = std::strtold(str.c_str(), &ptr);

    if (errno == ERANGE){
        errno = 0;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange("stold");
    }
    else if (ptr == str.c_str()){
        BloombergLP::bslstl::StdExceptUtil::throwInvalidArgument("stold");
    }

    *pos = ptr - str.c_str();
    return value;
}
long double bsl::stold(const wstring& str, std::size_t* pos){
    wchar_t* ptr;
    long double value = std::wcstold(str.c_str(), &ptr);

    if (errno == ERANGE){
        errno = 0;
        BloombergLP::bslstl::StdExceptUtil::throwOutOfRange("stold");
    }
    else if (ptr == str.c_str()){
        BloombergLP::bslstl::StdExceptUtil::throwInvalidArgument("stold");
    }

    *pos = ptr - str.c_str();
    return value;
}
#endif

bsl::string bsl::to_string(int value) {
    bsl::string str;
    sprintf(str.dataPtr(), "%d", value);
    str.d_length = strlen(str.dataPtr());
    return str;
}

bsl::string bsl::to_string(unsigned value) {
    bsl::string str;
    sprintf(str.dataPtr(), "%u", value);
    str.d_length = strlen(str.dataPtr());
    return str;
}

bsl::string bsl::to_string(long value)
{
    bsl::string str;
    sprintf(str.dataPtr(), "%ld", value);
    str.d_length = strlen(str.dataPtr());
    return str;
}

bsl::string bsl::to_string(unsigned long value)
{
    bsl::string str;
    sprintf(str.dataPtr(), "%lu", value);
    str.d_length = strlen(str.dataPtr());
    return str;
}

bsl::string bsl::to_string(long long value)
{
    bsl::string str;
    sprintf(str.dataPtr(), "%lld", value);
    str.d_length = strlen(str.dataPtr());
    return str;
}

bsl::string bsl::to_string(unsigned long long value)
{
    bsl::string str;
    sprintf(str.dataPtr(), "%llu", value);
    str.d_length = strlen(str.dataPtr());
    return str;
}

bsl::string bsl::to_string(float value)
{
    char tempBuf[e_MAX_FLOAT_STRLEN10];
    sprintf(tempBuf, "%f", value);
    string str(tempBuf);
    return str;

}

bsl::string bsl::to_string(double value)
{
    char tempBuf[e_MAX_DOUBLE_STRLEN10];
    sprintf(tempBuf, "%f", value);
    string str(tempBuf);
    return str;
}

bsl::string bsl::to_string(long double value)
{
    char tempBuf[e_MAX_LONGDOUBLE_STRLEN10];
    sprintf(tempBuf, "%Lf", value);
    string str(tempBuf);
    return str;
}

bsl::wstring bsl::to_wstring(int value)
{
    wchar_t tempBuf[e_MAX_INT_STRLEN10];
    swprintf(tempBuf, sizeof tempBuf / sizeof *tempBuf, L"%d", value);
    wstring str(tempBuf);
    return str;
}

bsl::wstring bsl::to_wstring(long value)
{
    wchar_t tempBuf[e_MAX_INT64_STRLEN10];
    swprintf(tempBuf, sizeof tempBuf / sizeof *tempBuf, L"%ld", value);
    wstring str(tempBuf);
    return str;
}
bsl::wstring bsl::to_wstring(long long value)
{
    wchar_t tempBuf[e_MAX_INT64_STRLEN10];
    swprintf(tempBuf, sizeof tempBuf / sizeof *tempBuf, L"%lld", value);
    wstring str(tempBuf);
    return str;
}

bsl::wstring bsl::to_wstring(unsigned value)
{
    wchar_t tempBuf[e_MAX_INT_STRLEN10];
    swprintf(tempBuf, sizeof tempBuf / sizeof *tempBuf, L"%u", value);
    wstring str(tempBuf);
    return str;
}

bsl::wstring bsl::to_wstring(unsigned long value)
{
    wchar_t tempBuf[e_MAX_INT64_STRLEN10];
    swprintf(tempBuf, sizeof tempBuf / sizeof *tempBuf, L"%lu", value);
    wstring str(tempBuf);
    return str;
}

bsl::wstring bsl::to_wstring(unsigned long long value)
{
    wchar_t tempBuf[e_MAX_INT64_STRLEN10];
    swprintf(tempBuf, sizeof tempBuf / sizeof *tempBuf, L"%llu", value);
    wstring str(tempBuf);
    return str;
}

bsl::wstring bsl::to_wstring(float value)
{
    wchar_t tempBuf[e_MAX_FLOAT_STRLEN10];
    swprintf(tempBuf, sizeof tempBuf / sizeof *tempBuf, L"%f", value);
    wstring wstr(tempBuf);
    return wstr;
}

bsl::wstring bsl::to_wstring(double value)
{
    wchar_t tempBuf[e_MAX_DOUBLE_STRLEN10];
    swprintf(tempBuf, sizeof tempBuf / sizeof *tempBuf, L"%f", value);
    wstring wstr(tempBuf);
    return wstr;
}

bsl::wstring bsl::to_wstring(long double value)
{
    wchar_t tempBuf[e_MAX_LONGDOUBLE_STRLEN10];
    swprintf(tempBuf, sizeof tempBuf / sizeof *tempBuf, L"%Lf", value);
    wstring wstr(tempBuf);
    return wstr;
}



#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
