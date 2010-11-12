/***************************************************************************
 *
 * 26.template.mask.array.cpp - tests exercising class template
 *                              mask_array
 *
 * $Id: 26.mask.array.cpp 493625 2007-01-07 00:46:13Z sebor $
 *
 ***************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  "License"); you may  not use  this file
 * except in  compliance with the License.   You may obtain  a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the  License is distributed on an  "AS IS" BASIS,
 * WITHOUT  WARRANTIES OR CONDITIONS  OF ANY  KIND, either  express or
 * implied.   See  the License  for  the  specific language  governing
 * permissions and limitations under the License.
 *
 **************************************************************************/

#include <cstdlib>    // for strtol(), size_t
#include <valarray>   // for mask_array, valarray

#include <driver.h>

/**************************************************************************/

// returns a valarray constructed from a string of comma-separated values
// if there are fewer than n values in the string the remaining values are
// taken to be the same as the last one, i.e., the call
//   make_array("1,2,3", 5) returns valarray({1,2,3,3})
template <class T>
std::valarray<T>
make_array (const char *s, std::size_t n = 0)
{
    if (0 == s || '\0' == *s)
        return n ? std::valarray<T>(T (), n) : std::valarray<T>();

    T buf [256];
    std::memset(buf, 0, 256);

    for (std::size_t i = 0; ; ++i) {

        char *end;
        long val = std::strtol (s, &end, 0);

        RW_ASSERT (s < end);
        RW_ASSERT ('\0' == *end || ',' == *end);

        buf [i] = T (val);

        if ('\0' == *end) {
            while (++i < n)
                buf [i] = buf [i - 1];

            return std::valarray<T>(buf, i);
        }

        s = end + 1;
    }
}


std::valarray<bool>
make_bool_array (const char *s, std::size_t n = 0)
{
    typedef std::valarray<bool> BoolArray;

    if (0 == s || '\0' == *s)
        return n ? BoolArray(false, n) : BoolArray ();

    bool buf [256];
    // Initialize buf to 0.
    std::memset(buf, 0, 256);

    for (std::size_t i = 0; ; ++i) {

        RW_ASSERT ('0' == *s || '1' == *s);

        buf [i] = '1' == *s++;

        if ('\0' == *s) {
            while (++i < n)
//                buf [i] = buf [i - 1];
                buf [i] = false;

            return BoolArray (buf, i);
        }
    }
}

/**************************************************************************/

static void
test_mask_array (int         line,
                 const char *array,
                 const char *mask,
                 const char *opname,
                 const char *args,
                 const char *result,
                 int         nmatch)
                     
{
    typedef int T;

    const char tname[] = "int";

    /* */ std::valarray<T>    asrc (make_array<int>(array));
    // size of the mask must be same as 'asrc', else bad things happens.
    const std::valarray<bool> amask (make_bool_array (mask, asrc.size()));
    // length of 'aargs' must be same as length of 'asrc', as defined by
    // standard 26.3.2.2 P1.
    const std::valarray<T>    aarg (make_array<int>(args, asrc.size()));

    if (0 == opname)
        opname = "";

    // construct the expected result
#if RW_BUG // Standard requires that operator[]() const return rvalue, not ref
    const std::valarray<T> expect =
#else
    std::valarray<T> expect =
#endif
        make_array<int>(result, *opname ? 0 : asrc.size ());

    // create an mask array object from the source array
    // and the array of indices
    const std::mask_array<T> ia (asrc [amask]);

    // iterate over the series of assignment operators, invoking
    // each in sequence on the source array, modifying it in place
    for (const char *op = opname; *op; ) {

        // skip optional whitespace
        for ( ; ' ' == *op; ++op);

        switch (*op) {
            // invoke fill assignment embedded within a series
            // of compound assignments, e.g., "*=,,+=" meaning
            //   ia *= aarg; ia = aarg [0]; ia += aarg
        case ',': ia = aarg.size () ? aarg [0] : 0; break;

            // invoke ordinary assignment
        case '=': ia = aarg; op += 1; break;

            // invoke computed assignment
        case '*': ia  *= aarg; RW_ASSERT ('=' == op [1]); op += 2; break;
        case '/': ia  /= aarg; RW_ASSERT ('=' == op [1]); op += 2; break;
        case '%': ia  %= aarg; RW_ASSERT ('=' == op [1]); op += 2; break;
        case '+': ia  += aarg; RW_ASSERT ('=' == op [1]); op += 2; break;
        case '-': ia  -= aarg; RW_ASSERT ('=' == op [1]); op += 2; break;
        case '^': ia  ^= aarg; RW_ASSERT ('=' == op [1]); op += 2; break;
        case '&': ia  &= aarg; RW_ASSERT ('=' == op [1]); op += 2; break;
        case '|': ia  |= aarg; RW_ASSERT ('=' == op [1]); op += 2; break;
        case '<': ia <<= aarg; RW_ASSERT ('=' == op [2]); op += 3; break;
        case '>': ia >>= aarg; RW_ASSERT ('=' == op [2]); op += 3; break;
        default:
            RW_ASSERT (!"unknown operator");
        }

        // skip optional whitespace
        for ( ; ' ' == *op; ++op);

        // assignment operators are separated by commas
        if (',' == *op)
            ++op;
        else
            break;
    }

    if ('\0' == *opname) {
        // fill assignment
        ia = aarg.size () ? aarg [0] : 0;
    }

    int nequal = 0;
    for (std::size_t i = 0; i != asrc.size (); ++i, ++nequal) {
        if (asrc [i] != expect [i])
            break;
    }

    if (-1 == nmatch)
        nmatch = int (asrc.size ());

    const int* const expect_begin = expect.size () ? &expect [0] : 0;
    const int* const actual_begin = asrc.size () ? &asrc [0] : 0;

    if ('=' == opname [0] && '\0' == opname [1]) {
        rw_assert (nequal == nmatch, 0, line,
                   "valarray<%s>({%s})[valarray<bool>({%s})] %s "
                   "%d %c= {%{*.*Ad}}, got {%{*.*Ad}}",
                   tname, array, mask, opname,
                   aarg.size () ? aarg [0] : 0,
                   nmatch == int (asrc.size ()) ? '=' : '!',
                   int (sizeof (int)), int (expect.size ()), expect_begin,
                   int (sizeof (int)), int (asrc.size ()), actual_begin);
    }
    else {
        rw_assert (nequal == nmatch, 0, line,
                   "valarray<%s>({%s})[valarray<bool>({%s})] %s "
                   "valarray<%1$s>({%s}) %c= {%{*.*Ad}}, got {%{*.*Ad}}",
                   tname, array, mask, opname, result,
                   nmatch == int (asrc.size ()) ? '=' : '!',
                   int (sizeof (int)), int (expect.size ()), expect_begin,
                   int (sizeof (int)), int (asrc.size ()), actual_begin);
    }
}

/**************************************************************************/

static int
run_test (int, char**)
{
#define TEST(array, mask, op, arg, res) \
    test_mask_array (__LINE__, array, mask, op, arg, res, -1)

    //////////////////////////////////////////////////////////////////
    // exercise mask_array<T>::operator=(T), fill assignment
    rw_info (0, 0, __LINE__,
             "std::mask_array<int>::operator=(int)");

    //    +-- source array
    //    |    +-- mask array (bitset of 0's and 1's)
    //    |    |   +-- zero for fill assignment
    //    |    |   |   +-- argument of fill assignment
    //    |    |   |   |   +-- expected result
    //    |    |   |   |   |
    //    |    |   |   |   |
    //    V    V   V   V   V
    TEST ("",  "", 0, "0", "");
    TEST ("1", "", 0, "0", "1");

    TEST ("1",         "1",     0, "2", "2");
    TEST ("1,2",       "10",    0, "3", "3,2");
    TEST ("1,2,3",     "100",   0, "4", "4,2,3");
    TEST ("1,2,3,4",   "1000",  0, "5", "5,2,3,4");
    TEST ("1,2,3,4,5", "10000", 0, "6", "6,2,3,4,5");

    TEST ("1,2,3,4,5", "11000", 0, "6", "6,6,3,4,5");
    TEST ("1,2,3,4,5", "10100", 0, "6", "6,2,6,4,5");
    TEST ("1,2,3,4,5", "10010", 0, "6", "6,2,3,6,5");
    TEST ("1,2,3,4,5", "10001", 0, "6", "6,2,3,4,6");

    TEST ("1,2,3,4,5", "11100", 0, "6", "6,6,6,4,5");
    TEST ("1,2,3,4,5", "11010", 0, "6", "6,6,3,6,5");
    TEST ("1,2,3,4,5", "11001", 0, "6", "6,6,3,4,6");

    TEST ("1,2,3,4,5", "10110", 0, "6", "6,2,6,6,5");
    TEST ("1,2,3,4,5", "10101", 0, "6", "6,2,6,4,6");

    TEST ("1,2,3,4,5", "11010", 0, "6", "6,6,3,6,5");
    TEST ("1,2,3,4,5", "10110", 0, "6", "6,2,6,6,5");
    TEST ("1,2,3,4,5", "10011", 0, "6", "6,2,3,6,6");

#define INFO(op)   \
    rw_info (0, 0, __LINE__, \
             "std::mask_array<int>::operator%s(std::valarray<int>)", op);

    INFO ("=");

    //    +-- source array
    //    |    +-- array of indices (mask_array ctor argument)
    //    |    |    +-- assignment operators to test (0 for none)
    //    |    |    |    +-- argument of assignment (0 for none)
    //    |    |    |    |    +-- expected result
    //    |    |    |    |    |
    //    |    |    |    |    |
    //    V    V    V    V    V
    TEST ("",  "",  "=", "",  "");
    TEST ("1", "",  "=", "",  "1");
    TEST ("1", "1", "=", "2", "2");
    TEST ("1", "1", "=", "3", "3");
    TEST ("2", "1", "=", "4", "4");

    TEST ("1,2",     "",     "=", "",      "1,2");
    TEST ("1,2",     "01",   "=", "3",     "1,3");
    TEST ("1,2",     "10",   "=", "3",     "3,2");
    TEST ("1,2",     "11",   "=", "3,4",   "3,4");

    TEST ("1,2,3",   "",     "=", "",      "1,2,3");
    TEST ("1,2,3",   "000",  "=", "",      "1,2,3");
    TEST ("1,2,3",   "001",  "=", "6",     "1,2,6");
    TEST ("1,2,3",   "010",  "=", "6",     "1,6,3");
    TEST ("1,2,3",   "011",  "=", "6,5",   "1,6,5");
    TEST ("1,2,3",   "100",  "=", "6",     "6,2,3");
    TEST ("1,2,3",   "101",  "=", "6,5",   "6,2,5");
    TEST ("1,2,3",   "110",  "=", "6,5",   "6,5,3");
    TEST ("1,2,3",   "111",  "=", "6,5,4", "6,5,4");

    TEST ("1,2,3,4", "",     "=", "",      "1,2,3,4");
    TEST ("1,2,3,4", "0",    "=", "",      "1,2,3,4");
    TEST ("1,2,3,4", "1",    "=", "5",     "5,2,3,4");
    TEST ("1,2,3,4", "00",   "=", "",      "1,2,3,4");
    TEST ("1,2,3,4", "01",   "=", "5",     "1,5,3,4");
    TEST ("1,2,3,4", "10",   "=", "5",     "5,2,3,4");
    TEST ("1,2,3,4", "11",   "=", "5,6",   "5,6,3,4");

    TEST ("1,2,3,4", "000",  "=", "",      "1,2,3,4");
    TEST ("1,2,3,4", "001",  "=", "5",     "1,2,5,4");
    TEST ("1,2,3,4", "010",  "=", "5",     "1,5,3,4");
    TEST ("1,2,3,4", "011",  "=", "5,6",   "1,5,6,4");
    TEST ("1,2,3,4", "100",  "=", "5",     "5,2,3,4");
    TEST ("1,2,3,4", "101",  "=", "5,6",   "5,2,6,4");
    TEST ("1,2,3,4", "110",  "=", "5,6",   "5,6,3,4");
    TEST ("1,2,3,4", "111",  "=", "5,6,7", "5,6,7,4");

    TEST ("1,2,3,4", "0000", "=", "",      "1,2,3,4");
    TEST ("1,2,3,4", "0001", "=", "5",     "1,2,3,5");
    TEST ("1,2,3,4", "0010", "=", "5",     "1,2,5,4");
    TEST ("1,2,3,4", "0011", "=", "5,6",   "1,2,5,6");
    TEST ("1,2,3,4", "0100", "=", "5",     "1,5,3,4");
    TEST ("1,2,3,4", "0101", "=", "5,6",   "1,5,3,6");
    TEST ("1,2,3,4", "0110", "=", "5,6",   "1,5,6,4");
    TEST ("1,2,3,4", "0111", "=", "5,6,7", "1,5,6,7");

    TEST ("1,2,3,4", "1000", "=", "5",       "5,2,3,4");
    TEST ("1,2,3,4", "1001", "=", "5,6",     "5,2,3,6");
    TEST ("1,2,3,4", "1010", "=", "5,6",     "5,2,6,4");
    TEST ("1,2,3,4", "1011", "=", "5,6,7",   "5,2,6,7");
    TEST ("1,2,3,4", "1100", "=", "5,6",     "5,6,3,4");
    TEST ("1,2,3,4", "1101", "=", "5,6,7",   "5,6,3,7");
    TEST ("1,2,3,4", "1110", "=", "5,6,7",   "5,6,7,4");
    TEST ("1,2,3,4", "1111", "=", "5,6,7,8", "5,6,7,8");

    //////////////////////////////////////////////////////////////////
    // exercise mask_array::operator*=()
    INFO ("*=");

    TEST ("",  "",  "*=", "",  "");
    TEST ("1", "",  "*=", "",  "1");
    TEST ("1", "0", "*=", "",  "1");
    TEST ("1", "0", "*=", "5", "1");
    TEST ("1", "1", "*=", "5", "5");

    TEST ("1,2",     "",     "*=", "",        "1, 2");
    TEST ("1,2",     "0",    "*=", "",        "1, 2");
    TEST ("1,2",     "00",   "*=", "",        "1, 2");
    TEST ("1,2",     "01",   "*=", "5",       "1,10");
    TEST ("1,2",     "10",   "*=", "5",       "5, 2");
    TEST ("1,2",     "11",   "*=", "5,6",     "5,12");

    TEST ("1,2,3",   "",     "*=", "",        "1, 2, 3");
    TEST ("1,2,3",   "0",    "*=", "",        "1, 2, 3");
    TEST ("1,2,3",   "00",   "*=", "",        "1, 2, 3");
    TEST ("1,2,3",   "000",  "*=", "",        "1, 2, 3");
    TEST ("1,2,3",   "001",  "*=", "5",       "1, 2,15");
    TEST ("1,2,3",   "010",  "*=", "5",       "1,10, 3");
    TEST ("1,2,3",   "011",  "*=", "5,6",     "1,10,18");
    TEST ("1,2,3",   "100",  "*=", "5",       "5, 2, 3");
    TEST ("1,2,3",   "101",  "*=", "5,6",     "5, 2,18");
    TEST ("1,2,3",   "110",  "*=", "5,6",     "5,12, 3");
    TEST ("1,2,3",   "111",  "*=", "5,6,7",   "5,12,21");

    TEST ("1,2,3,4", "",     "*=", "",        "1, 2, 3, 4");
    TEST ("1,2,3,4", "0",    "*=", "",        "1, 2, 3, 4");
    TEST ("1,2,3,4", "00",   "*=", "",        "1, 2, 3, 4");
    TEST ("1,2,3,4", "000",  "*=", "",        "1, 2, 3, 4");
    TEST ("1,2,3,4", "0000", "*=", "",        "1, 2, 3, 4");
    TEST ("1,2,3,4", "0001", "*=", "5",       "1, 2, 3,20");
    TEST ("1,2,3,4", "0010", "*=", "5",       "1, 2,15, 4");
    TEST ("1,2,3,4", "0011", "*=", "5,6",     "1, 2,15,24");
    TEST ("1,2,3,4", "0100", "*=", "5",       "1,10, 3, 4");
    TEST ("1,2,3,4", "0101", "*=", "5,6",     "1,10, 3,24");
    TEST ("1,2,3,4", "0110", "*=", "5,6",     "1,10,18, 4");
    TEST ("1,2,3,4", "0111", "*=", "5,6,7",   "1,10,18,28");
    TEST ("1,2,3,4", "1001", "*=", "5,6",     "5, 2, 3,24");
    TEST ("1,2,3,4", "1010", "*=", "5,6",     "5, 2,18, 4");
    TEST ("1,2,3,4", "1011", "*=", "5,6,7",   "5, 2,18,28");
    TEST ("1,2,3,4", "1100", "*=", "5,6",     "5,12, 3, 4");
    TEST ("1,2,3,4", "1101", "*=", "5,6,7",   "5,12, 3,28");
    TEST ("1,2,3,4", "1110", "*=", "5,6,7",   "5,12,21, 4");
    TEST ("1,2,3,4", "1111", "*=", "5,6,7,8", "5,12,21,32");

    // exercise compound assignment, i.e.,
    // ({1,2,3,4,5,6}[{0,1,0,1,0,1}] *= {0,1,2}) *= {0,1,2}
    TEST ("1,2,3,4,5,6", "010101", "*=,*=", "0,1,2", "1,0,3,4,5,24");

    //////////////////////////////////////////////////////////////////
    // exercise mask_array::operator/=()
    INFO ("/=");

    TEST ("",  "",  "/=", "",  "");
    TEST ("10", "",  "/=", "",  "10");
    TEST ("10", "0", "/=", "",  "10");
    TEST ("10", "0", "/=", "5", "10");
    TEST ("10", "1", "/=", "5", " 2");

    TEST ("10,20",     "",     "/=", "",          "10,20");
    TEST ("10,20",     "0",    "/=", "",          "10,20");
    TEST ("10,20",     "00",   "/=", "",          "10,20");
    TEST ("10,20",     "01",   "/=", "5",         "10, 4");
    TEST ("10,20",     "10",   "/=", "5",         " 2,20");
    TEST ("10,20",     "11",   "/=", "5,6",       " 2, 3");

    TEST ("10,20,30",   "",     "/=", "",         "10,20,30");
    TEST ("10,20,30",   "0",    "/=", "",         "10,20,30");
    TEST ("10,20,30",   "00",   "/=", "",         "10,20,30");
    TEST ("10,20,30",   "000",  "/=", "",         "10,20,30");
    TEST ("10,20,30",   "001",  "/=", "5",        "10,20, 6");
    TEST ("10,20,30",   "010",  "/=", "5",        "10, 4,30");
    TEST ("10,20,30",   "011",  "/=", "5,6",      "10, 4, 5");
    TEST ("10,20,30",   "100",  "/=", "5",        " 2,20,30");
    TEST ("10,20,30",   "101",  "/=", "5,6",      " 2,20, 5");
    TEST ("10,20,30",   "110",  "/=", "5,6",      " 2, 3,30");
    TEST ("10,20,30",   "111",  "/=", "5,6,7",    " 2, 3, 4");

    TEST ("10,20,30,40", "",     "/=", "",        "10,20,30,40");
    TEST ("10,20,30,40", "0",    "/=", "",        "10,20,30,40");
    TEST ("10,20,30,40", "00",   "/=", "",        "10,20,30,40");
    TEST ("10,20,30,40", "000",  "/=", "",        "10,20,30,40");
    TEST ("10,20,30,40", "0000", "/=", "",        "10,20,30,40");
    TEST ("10,20,30,40", "0001", "/=", "5",       "10,20,30, 8");
    TEST ("10,20,30,40", "0010", "/=", "5",       "10,20, 6,40");
    TEST ("10,20,30,40", "0011", "/=", "5,6",     "10,20, 6, 6");
    TEST ("10,20,30,40", "0100", "/=", "5",       "10, 4,30,40");
    TEST ("10,20,30,40", "0101", "/=", "5,6",     "10, 4,30, 6");
    TEST ("10,20,30,40", "0110", "/=", "5,6",     "10, 4, 5,40");
    TEST ("10,20,30,40", "0111", "/=", "5,6,7",   "10, 4, 5, 5");
    TEST ("10,20,30,40", "1000", "/=", "5",       " 2,20,30,40");
    TEST ("10,20,30,40", "1001", "/=", "5,6",     " 2,20,30, 6");
    TEST ("10,20,30,40", "1010", "/=", "5,6",     " 2,20, 5,40");
    TEST ("10,20,30,40", "1011", "/=", "5,6,7",   " 2,20, 5, 5");
    TEST ("10,20,30,40", "1100", "/=", "5,6",     " 2, 3,30,40");
    TEST ("10,20,30,40", "1101", "/=", "5,6,7",   " 2, 3,30, 5");
    TEST ("10,20,30,40", "1110", "/=", "5,6,7",   " 2, 3, 4,40");
    TEST ("10,20,30,40", "1111", "/=", "5,6,7,8", " 2, 3, 4, 5");

    // exercise compound assignment, i.e.,
    // ({1,2,3,4,5,6}[{0,1,0,1,0,1}] *= {7,8,9}) /= {7,8,9}
    TEST ("1,2,3,4,5,6", "010101", "*=,/=", "7,8,9", "1,2,3,4,5,6");
    // ({1,20,3,40,5,60}[{0,1,0,1,0,1}] /= {7,8,9}) *= {7,8,9}
    TEST ("1,20,3,40,5,60", "010101", "/=,*=", "7,8,9", "1,14,3,40,5,54");

    //////////////////////////////////////////////////////////////////
    // exercise mask_array::operator%=()
    INFO ("%=");

    TEST ("1, 2,3, 4,5, 6", "010101", "%=", "3,5,7", "1,2,3,4,5,6");
    TEST ("1,11,2,13,4,15", "010101", "%=", "6,7,8", "1,5,2,6,4,7");

    //////////////////////////////////////////////////////////////////
    // exercise mask_array::operator+=()
    INFO ("+=");

    TEST ("1,2,3,4,5,6", "010101", "+=",    "7,8,9", "1,9,3,12,5,15");
    TEST ("1,2,3,4,5,6", "010101", "+=,+=", "1,2,3", "1,4,3, 8,5,12");

    //////////////////////////////////////////////////////////////////
    // exercise mask_array::operator-=()
    INFO ("-=");

    TEST ("9,8,7,6,5,4", "111000", "-=", "9,8,7", "0,0,0,6,5,4");
    TEST ("9,8,7,6,5,4", "011100", "-=", "8,7,6", "9,0,0,0,5,4");
    TEST ("9,8,7,6,5,4", "001110", "-=", "7,6,5", "9,8,0,0,0,4");
    TEST ("9,8,7,6,5,4", "000111", "-=", "6,5,4", "9,8,7,0,0,0");
    TEST ("9,8,7,6,5,4", "010101", "-=", "3,2,1", "9,5,7,4,5,3");

    //////////////////////////////////////////////////////////////////
    // exercise mask_array::operator^=()
    INFO ("^=");

    TEST ("1,3,5,7,9", "10101", "^=", "2,4,5", "3,3,1,7,12");

    //////////////////////////////////////////////////////////////////
    // exercise mask_array::operator&=()
    INFO ("&=");

    TEST ("1,3,5,7,9", "10101", "&=", "3,1,7", "1,3,1,7,1");
    TEST ("1,2,3,4,5", "01010", "&=", "1,1",   "1,0,3,0,5");

    //////////////////////////////////////////////////////////////////
    // exercise mask_array::operator|=()
    INFO ("|=");

    TEST ("1,3,5,7,9", "10101", "|=", "3,1,7", "3,3,5,7,15");
    TEST ("1,2,3,4,5", "01010", "|=", "1,1",   "1,3,3,5, 5");

    //////////////////////////////////////////////////////////////////
    // exercise mask_array::operator<<=()
    INFO ("<<=");

    TEST ("1,3,5,7,9", "11100", "<<=", "1,1,1",     " 2, 6,10, 7, 9");
    TEST ("1,3,5,7,9", "01110", "<<=", "1,1,1",     " 1, 6,10,14, 9");
    TEST ("1,3,5,7,9", "00111", "<<=", "1,1,1",     " 1, 3,10,14,18");
    TEST ("1,3,5,7,9", "11111", "<<=", "5,4,3,2,1", "32,48,40,28,18");

    // exercise a compound assignment, i.e.,
    // ({1,3,5,7,9}[{2,3,4}] <<= {1,1,1}) >>= {1,1,1}
    TEST ("1,3,5,7,9", "00111", "<<=,>>=", "1,1,1", "1,3,5,7,9");

    //////////////////////////////////////////////////////////////////
    // exercise mask_array::operator>>=()
    INFO (">>=");

    TEST ("2,6,10, 7, 9", "11100", ">>=", "1,1,1", "1,3,5,7,9");
    TEST ("1,6,10,14, 9", "01110", ">>=", "1,1,1", "1,3,5,7,9");
    TEST ("1,3,10,14,18", "00111", ">>=", "1,1,1", "1,3,5,7,9");

    // exercise a compound assignment, i.e.,
    // ({10,20,30,40,50}[{1,3,5}] >>= {1,1,1}) <<= {1,1,1}
    TEST ("10,11,12,13,14", "10101", ">>=,<<=", "1,2,3", "10,11,12,13,8");

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    // FIXME: add command line options to enable/disable each operator
    return rw_test (argc, argv, __FILE__,
                    "valarray.mask.array",
                    0 /* no comment */,
                    run_test,
                    "",
                    (void*)0   /* sentinel */);
}
