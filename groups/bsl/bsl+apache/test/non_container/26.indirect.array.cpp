/***************************************************************************
 *
 * 26.template.indirect.array.cpp - tests exercising class template
 *                                  indirect_array
 *
 * $Id: 26.indirect.array.cpp 492025 2007-01-03 02:48:52Z sebor $
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
#include <valarray>   // for indirect_array, valarray

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

/**************************************************************************/

static void
test_indirect_array (int         line,
                     const char *array,
                     const char *inxs,
                     const char *opname,
                     const char *args,
                     const char *result,
                     int         nmatch)
                     
{
    typedef int T;

    const char tname[] = "int";

    /* */ std::valarray<T>           asrc (make_array<int>(array));
    const std::valarray<std::size_t> ainx (make_array<std::size_t> (inxs));
    const std::valarray<T>           aarg (make_array<int>(args));

    if (0 == opname)
        opname = "";

    // construct the expected result
#if RW_BUG // const valarray<T>::operator[] is supposed to return T by value but RW returns it by reference.
    const std::valarray<T> expect =
#else
    std::valarray<T> expect =
#endif
        make_array<int>(result, *opname ? 0 : asrc.size ());

    // create an indirect array object from the source array
    // and the array of indices
    const std::indirect_array<T> ia (asrc [ainx]);

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

    rw_assert (nequal == nmatch, 0, line,
               "valarray<%s>({%s})[valarray<size_t>({%s})] %s "
               "valarray<%1$s>({%s}) %c= {%{*.*Ad}}, got {%{*.*Ad}}",
               tname, array, inxs, opname, result,
               nmatch == int (asrc.size ()) ? '=' : '!',
               int (sizeof (int)), int (expect.size ()), expect_begin,
               int (sizeof (int)), int (asrc.size ()), actual_begin);
}

/**************************************************************************/

static int
run_test (int, char**)
{
#define TEST(array, inx, op, arg, res, nmatch) \
    test_indirect_array (__LINE__, array, inx, op, arg, res, nmatch)

    //////////////////////////////////////////////////////////////////
    // exercise indirect_array<T>::operator=(T), fill assignment
    rw_info (0, 0, __LINE__,
             "std::indirect_array<int>::operator=(int)");

    //    +-- source array
    //    |    +-- array of indices (indirect_array ctor argument)
    //    |    |   +-- zero for fill assignment
    //    |    |   |   +-- argument of fill assignment
    //    |    |   |   |   +-- expected result
    //    |    |   |   |   |    +-- number of matching elements
    //    |    |   |   |   |    |
    //    V    V   V   V   V    V 
    TEST ("",  "", 0, "0", "",  -1);
    TEST ("1", "", 0, "0", "1", -1);

    TEST ("1",         "0", 0, "2", "2",         -1);
    TEST ("1,2",       "0", 0, "3", "3,2",       -1);
    TEST ("1,2,3",     "0", 0, "4", "4,2,3",     -1);
    TEST ("1,2,3,4",   "0", 0, "5", "5,2,3,4",   -1);
    TEST ("1,2,3,4,5", "0", 0, "6", "6,2,3,4,5", -1);

    TEST ("1,2,3,4,5", "0,1", 0, "6", "6,6,3,4,5", -1);
    TEST ("1,2,3,4,5", "0,2", 0, "6", "6,2,6,4,5", -1);
    TEST ("1,2,3,4,5", "0,3", 0, "6", "6,2,3,6,5", -1);
    TEST ("1,2,3,4,5", "0,4", 0, "6", "6,2,3,4,6", -1);

    TEST ("1,2,3,4,5", "0,1,2", 0, "6", "6,6,6,4,5", -1);
    TEST ("1,2,3,4,5", "0,1,3", 0, "6", "6,6,3,6,5", -1);
    TEST ("1,2,3,4,5", "0,1,4", 0, "6", "6,6,3,4,6", -1);

    TEST ("1,2,3,4,5", "0,2,1", 0, "6", "6,6,6,4,5", -1);
    TEST ("1,2,3,4,5", "0,2,3", 0, "6", "6,2,6,6,5", -1);
    TEST ("1,2,3,4,5", "0,2,4", 0, "6", "6,2,6,4,6", -1);

    TEST ("1,2,3,4,5", "0,3,1", 0, "6", "6,6,3,6,5", -1);
    TEST ("1,2,3,4,5", "0,3,2", 0, "6", "6,2,6,6,5", -1);
    TEST ("1,2,3,4,5", "0,3,4", 0, "6", "6,2,3,6,6", -1);

#define INFO(op)   \
    rw_info (0, 0, __LINE__, \
             "std::indirect_array<int>::operator%s(std::valarray<int>)", op);

    INFO ("=");

    //    +-- source array
    //    |    +-- array of indices (indirect_array ctor argument)
    //    |    |    +-- assignment operators to test (0 for none)
    //    |    |    |    +-- argument of assignment (0 for none)
    //    |    |    |    |    +-- expected result
    //    |    |    |    |    |    +-- number of matching elements
    //    |    |    |    |    |    |
    //    V    V    V    V    V    V
    TEST ("",  "",  "=", "",  "",  -1 /* all */);
    TEST ("1", "",  "=", "",  "1", -1 /* all */);
    TEST ("1", "0", "=", "2", "2", -1 /* all */);
    TEST ("1", "0", "=", "3", "3", -1 /* all */);
    TEST ("2", "0", "=", "4", "1",  0 /* mismatch at offset 0 */);

    TEST ("1,2",     "",    "=", "",    "1,2",     -1);
    TEST ("1,2",     "0",   "=", "3",   "3,2",     -1);
    TEST ("1,2",     "1",   "=", "3",   "1,3",     -1);

    TEST ("1,2,3",   "",    "=", "",    "1,2,3",   -1);
    TEST ("1,2,3",   "0,1", "=", "5,4", "5,4,3",   -1);
    TEST ("1,2,3",   "0,2", "=", "5,4", "5,2,4",   -1);
    TEST ("1,2,3",   "1,0", "=", "5,4", "4,5,3",   -1);
    TEST ("1,2,3",   "1,2", "=", "5,4", "1,5,4",   -1);
    TEST ("1,2,3",   "2,0", "=", "5,4", "4,2,5",   -1);
    TEST ("1,2,3",   "2,1", "=", "5,4", "1,4,5",   -1);

    TEST ("1,2,3,4", "",  "=", "",  "1,2,3,4", -1);
    TEST ("1,2,3,4", "0", "=", "9", "9,2,3,4", -1);
    TEST ("1,2,3,4", "1", "=", "9", "1,9,3,4", -1);
    TEST ("1,2,3,4", "2", "=", "9", "1,2,9,4", -1);
    TEST ("1,2,3,4", "3", "=", "9", "1,2,3,9", -1);

    TEST ("1,2,3,4", "0,1", "=", "8,9", "8,9,3,4", -1);
    TEST ("1,2,3,4", "0,2", "=", "8,9", "8,2,9,4", -1);
    TEST ("1,2,3,4", "0,3", "=", "8,9", "8,2,3,9", -1);

    TEST ("1,2,3,4", "1,0", "=", "8,9", "9,8,3,4", -1);
    TEST ("1,2,3,4", "1,2", "=", "8,9", "1,8,9,4", -1);
    TEST ("1,2,3,4", "1,3", "=", "8,9", "1,8,3,9", -1);

    TEST ("1,2,3,4", "2,0", "=", "8,9", "9,2,8,4", -1);
    TEST ("1,2,3,4", "2,1", "=", "8,9", "1,9,8,4", -1);
    TEST ("1,2,3,4", "2,3", "=", "8,9", "1,2,8,9", -1);

    TEST ("1,2,3,4", "3,0", "=", "8,9", "9,2,3,8", -1);
    TEST ("1,2,3,4", "3,1", "=", "8,9", "1,9,3,8", -1);
    TEST ("1,2,3,4", "3,2", "=", "8,9", "1,2,9,8", -1);

    //////////////////////////////////////////////////////////////////
    // exercise indirect_array::operator*=()
    INFO ("*=");

    TEST ("",    "",    "*=", "",    "",     -1);
    TEST ("1",   "",    "*=", "",    "1",    -1);
    TEST ("1",   "0",   "*=", "9",   "9",    -1);

    TEST ("1,2", "",    "*=", "",    "1, 2", -1);
    TEST ("1,2", "0",   "*=", "9",   "9, 2", -1);
    TEST ("1,2", "1",   "*=", "9",   "1,18", -1);

    TEST ("1,2", "0,1", "*=", "8,9", "8,18", -1);
    TEST ("1,2", "1,0", "*=", "8,9", "9,16", -1);

    TEST ("1,2,3", "",    "*=", "",    "1, 2, 3", -1);
    TEST ("1,2,3", "0,1", "*=", "8,9", "8,18, 3", -1);
    TEST ("1,2,3", "0,2", "*=", "8,9", "8, 2,27", -1);

    TEST ("1,2,3", "1,0", "*=", "8,9", "9,16, 3", -1);
    TEST ("1,2,3", "1,2", "*=", "8,9", "1,16,27", -1);

    TEST ("1,2,3", "2,0", "*=", "8,9", "9, 2,24", -1);
    TEST ("1,2,3", "2,1", "*=", "8,9", "1,18,24", -1);

    TEST ("1,2,3,4", "0", "*=", "0", "0,2,3,4", -1);
    TEST ("1,2,3,4", "1", "*=", "0", "1,0,3,4", -1);
    TEST ("1,2,3,4", "2", "*=", "0", "1,2,0,4", -1);
    TEST ("1,2,3,4", "3", "*=", "0", "1,2,3,0", -1);

    TEST ("1,2,3,4", "0,1", "*=", "5,6", "5,12, 3, 4", -1);
    TEST ("1,2,3,4", "0,2", "*=", "5,6", "5, 2,18, 4", -1);
    TEST ("1,2,3,4", "0,3", "*=", "5,6", "5, 2, 3,24", -1);

    TEST ("1,2,3,4", "1,0", "*=", "5,6", "6,10, 3, 4", -1);
    TEST ("1,2,3,4", "1,2", "*=", "5,6", "1,10,18, 4", -1);
    TEST ("1,2,3,4", "1,3", "*=", "5,6", "1,10, 3,24", -1);

    TEST ("1,2,3,4", "2,0", "*=", "5,6", "6, 2,15, 4", -1);
    TEST ("1,2,3,4", "2,1", "*=", "5,6", "1,12,15, 4", -1);
    TEST ("1,2,3,4", "2,3", "*=", "5,6", "1, 2,15,24", -1);

    //////////////////////////////////////////////////////////////////
    // exercise indirect_array::operator/=()
    INFO ("/=");

    TEST ("10",    "0",   "/=", "2",   " 5",    -1);
    TEST ("10,20", "0",   "/=", "2",   " 5,20", -1);
    TEST ("10,20", "1",   "/=", "2",   "10,10", -1);

    TEST ("10,20", "0,1", "/=", "5,4", "2,5", -1);
    TEST ("10,20", "1,0", "/=", "5,4", "2,4", -1);

    TEST ("10,20,30", "0,1", "/=", "5,4", "2, 5,30", -1);
    TEST ("10,20,30", "0,2", "/=", "5,4", "2,20, 7", -1);

    TEST ("10,20,30", "1,0", "/=", "5,4", " 2,4,30", -1);
    TEST ("10,20,30", "1,2", "/=", "5,4", "10,4, 7", -1);

    TEST ("10,20,30", "2,0", "/=", "5,4", " 2,20, 6", -1);
    TEST ("10,20,30", "2,1", "/=", "5,4", "10, 5, 6", -1);

    // exercise compound assignment, i.e.,
    // ({1,2,3,4,5}[{0,2,4}] *= {6,7,8}) /= {6,7,8}
    TEST (" 1, 2, 3, 4, 5", "0,2,4", "*=,/=", "6,7,8", "1, 2, 3, 4, 5", -1);
    // ({1,2,3,4,5}[{0,2,4}] /= {6,7,8}) *= {6,7,8}
    TEST ("10,20,30,40,50", "0,2,4", "/=,*=", "9,8,7", "9,20,24,40,49", -1);

    //////////////////////////////////////////////////////////////////
    // exercise indirect_array::operator%=()
    INFO ("%=");

    TEST ("10,11,12,13,14,15", "1,3,5", "%=", "6,7,8", "10,5,12,6,14,7", -1);

    //////////////////////////////////////////////////////////////////
    // exercise indirect_array::operator+=()
    INFO ("+=");

    TEST ("1,2,3,4,5,6", "1,3,5", "+=", "7,8,9", "1,9,3,12,5,15", -1);

    // exercise compound assignment, i.e.,
    // ({1,2,3,4,5,6}[{1,3,5}] += {7,8,9}) += {7,8,9}
    TEST ("1,2,3,4,5,6", "1,3,5", "+=,+=", "7,8,9", "1,16,3,20,5,24", -1);
    // ({1,2,3,4,5,6}[{1,3,5}] += {7,8,9}) -= {7,8,9}
    TEST ("1,2,3,4,5,6", "1,3,5", "+=,-=", "7,8,9", "1,2,3,4,5,6", -1);
    // ({1,2,3,4,5,6}[{1,3,5}] += {7,8,9}) /= {7,8,9}
    TEST ("1,2,3,4,5,6", "1,3,5", "+=,/=", "7,8,9", "1,1,3,1,5,1", -1);

    //////////////////////////////////////////////////////////////////
    // exercise indirect_array::operator-=()
    INFO ("-=");

    TEST ("9,8,7,6,5,4", "0,1,2", "-=", "9,8,7", "0,0,0,6,5,4", -1);
    TEST ("9,8,7,6,5,4", "1,2,3", "-=", "8,7,6", "9,0,0,0,5,4", -1);
    TEST ("9,8,7,6,5,4", "2,3,4", "-=", "7,6,5", "9,8,0,0,0,4", -1);
    TEST ("9,8,7,6,5,4", "3,4,5", "-=", "6,5,4", "9,8,7,0,0,0", -1);
    TEST ("9,8,7,6,5,4", "0,2,4", "-=", "3,2,1", "6,8,5,6,4,4", -1);

    //////////////////////////////////////////////////////////////////
    // exercise indirect_array::operator^=()
    INFO ("^=");

    TEST ("1,3,5,7,9", "0,2,4", "^=", "2,4,5", "3,3,1,7,12", -1);

    //////////////////////////////////////////////////////////////////
    // exercise indirect_array::operator&=()
    INFO ("&=");

    TEST ("1,3,5,7,9", "0,2,4", "&=", "3,1,7", "1,3,1,7,1", -1);

    //////////////////////////////////////////////////////////////////
    // exercise indirect_array::operator|=()
    INFO ("|=");

    TEST ("1,3,5,7,9", "0,2,4", "|=", "3,1,7", "3,3,5,7,15", -1);

    //////////////////////////////////////////////////////////////////
    // exercise indirect_array::operator<<=()
    INFO ("<<=");

    TEST ("1,3,5,7,9", "0,1,2", "<<=", "1,1,1", "2,6,10, 7, 9", -1);
    TEST ("1,3,5,7,9", "1,2,3", "<<=", "1,1,1", "1,6,10,14, 9", -1);
    TEST ("1,3,5,7,9", "2,3,4", "<<=", "1,1,1", "1,3,10,14,18", -1);

    // exercise a compound assignment, i.e.,
    // ({1,3,5,7,9}[{2,3,4}] <<= {1,1,1}) >>= {1,1,1}
    TEST ("1,3,5,7,9", "2,3,4", "<<=,>>=", "1,1,1", "1,3,5,7,9", -1);

    //////////////////////////////////////////////////////////////////
    // exercise indirect_array::operator>>=()
    INFO (">>=");

    TEST ("2,6,10, 7, 9", "0,1,2", ">>=", "1,1,1", "1,3,5,7,9", -1);
    TEST ("1,6,10,14, 9", "1,2,3", ">>=", "1,1,1", "1,3,5,7,9", -1);
    TEST ("1,3,10,14,18", "2,3,4", ">>=", "1,1,1", "1,3,5,7,9", -1);

    // exercise a compound assignment, i.e.,
    // ({10,20,30,40,50}[{1,3,5}] >>= {1,1,1}) <<= {1,1,1}
    TEST ("10,11,12,13,14", "0,2,4", ">>=,<<=", "1,2,3", "10,11,12,13,8", -1);
    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    // FIXME: add command line options to enable/disable each operator
    return rw_test (argc, argv, __FILE__,
                    "valarray.indirect.array",
                    0 /* no comment */,
                    run_test,
                    "",
                    (void*)0   /* sentinel */);
}
