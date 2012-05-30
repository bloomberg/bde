/***********************************************************************
 *
 * 24.istream.iterator.cpp - test exercising [istream.iterator]
 *
 * $Id: 24.istream.iterator.cpp 648752 2008-04-16 17:01:56Z faridz $
 *
 ***********************************************************************
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
 ***********************************************************************/

#include <sstream>      // for stringstream
#include <iterator>     // for istream_iterator

#include <cstddef>      // for ptrdiff_t

#include <rw_char.h>
#include <rw_printf.h>
#include <driver.h>

/***********************************************************************/

// names of template parameters
struct ParamNames {
    const char *tname;    // name of the T parameter
    const char *tfmt;     // rw_printf()-style format for T
    const char *cname;    // name of the CharT parameter
    const char *trname;   // name of the Traits parameter
    const char *dname;    // name of the Distance parameter
};


// format the template-id of the iterator
void fmtnam (const ParamNames &pn)
{
    // omit names of unspecified (default) template arguments
    const char* const id = pn.cname ? pn.trname ? pn.dname ?
        "<%s, %s, %s, %s>" : "<%s, %s, %s>" : "<%s, %s>" : "<%s>";

    rw_fprintf (0, "%{$ITER!:@}",
                "istream_iterator%{@}",
                id, pn.tname, pn.cname, pn.trname, pn.dname);
}

/***********************************************************************/

// for convenience
template <class T, class CharT, class Traits, class Dist>
struct TempParams { ParamNames names; };


// exercise istream_iterator ctors
template <class T, class CharT, class Traits, class Dist>
void test_ctors (const TempParams<T, CharT, Traits, Dist> &params)
{
    typedef std::istream_iterator<T, CharT, Traits, Dist> Iterator;
    typedef std::basic_stringstream<CharT, Traits>        StringStream;

    const Iterator eos1;
    const Iterator eos2;

    // verify [reverse.iter], p3, i.e., that two end-of-istream
    // always compare equal, and that no end-of-stream iterator
    // compares equal to a non-end-of-stream iterator

    fmtnam (params.names);

    rw_assert (eos1 == eos2, 0, __LINE__,
               "%{$ITER}::istream_iterator() == %{$ITER}::istream_iterator()");

    StringStream strm1;

    // make sure the stream object's locale contains the expected
    // specialization of the num_get facet (in case CharT isn't
    // char or wchar_t, or Traits isn't std::char_traits)
    typedef std::istreambuf_iterator<CharT, Traits> StreambufIterator;
    typedef std::num_get<CharT, StreambufIterator>  NumGet;

    if (false == std::has_facet<NumGet>(strm1.getloc ())) {
        const std::locale loc (std::locale::classic (), new NumGet);
        strm1.imbue (loc);
    }

    // insert a couple of items into the stream, one for each iterator
    // below, to prevent the iterator ctor from creating an end-of-stream
    // iterator (in case it doesn't delay extracting a value until it's
    // needed)
    strm1 << "1 2 ";

    // verify [reverse.iter], p3, i.e., that two istream_iterators
    // constructed from the same stream always compare equal
    const Iterator iter1 (strm1);
    const Iterator iter2 (strm1);

    rw_assert (iter1 != eos1, 0, __LINE__,
               "%{$ITER}::istream_iterator(basic_istream&) != "
               "%{$ITER}()");

    rw_assert (iter1 == iter2, 0, __LINE__,
               "%{$ITER}::istream_iterator(basic_istream&) == "
               "%{$ITER}::istream_iterator(basic_istream&)");

    StringStream strm2;
    strm2.imbue (strm1.getloc ());

    strm2 << "2 ";

    const Iterator iter3 (strm2);

    rw_assert (iter3 != eos1, 0, __LINE__,
               "%{$ITER}::istream_iterator(basic_istream&) != "
               "%{$ITER}()");

    // verify that iterators into different streams compare unequal
    rw_assert (iter3 != iter1, 0, __LINE__,
               "%{$ITER}::istream_iterator(basic_istream& [@%#p]) != "
               "%{$ITER}::istream_iterator(basic_istream& [@%#p])",
               &strm2, &strm1);
}

/***********************************************************************/

// worker to exercise operator*()
template <class T, class CharT, class Traits, class Dist>
void test_ops (const TempParams<T, CharT, Traits, Dist> &params,
               int                                       line,
               const char                               *input,
               T                                         expect,
               bool                                      is_eos,
               int                                       state)
{
    typedef std::istream_iterator<T, CharT, Traits, Dist> Iterator;
    typedef std::basic_stringstream<CharT, Traits>        StringStream;

    fmtnam (params.names);

    StringStream strm;

    typedef std::istreambuf_iterator<CharT, Traits> StreambufIterator;
    typedef std::num_get<CharT, StreambufIterator>  NumGet;

    if (false == std::has_facet<NumGet>(strm.getloc ())) {
        const std::locale loc (std::locale::classic (), new NumGet);
        strm.imbue (loc);
    }

    if (input && *input)
        strm << input;

    const Iterator eos;

    const Iterator it (strm);

    rw_assert (is_eos ^ (it != eos), 0, line,
               "line %d: %{$ITER}::operator() != %{$ITER}()", __LINE__);

    if (0 == (strm.rdstate () & (strm.badbit | strm.failbit))) {
        // operator*() is defined only for non-eos iterators
        // avoid calling it on a bad or failed stream too

        const T val = *it;

        rw_assert (val == expect, 0, line,
                   "line %d: %{$ITER}::operator*() == %{@}, got %{@}",
                   __LINE__, params.names.tfmt, expect, params.names.tfmt, val);
    }

    rw_assert (strm.rdstate () == state, 0, line,
               "line %d: %{$ITER}::operator*(), rdstate() == %{Is}, got %{Is}",
               __LINE__, state, strm.rdstate ());
}

/***********************************************************************/

// exercise operator*()
template <class T, class CharT, class Traits, class Dist>
void test_ops (const TempParams<T, CharT, Traits, Dist> &params)
{
    // const int Bad  = std::ios::badbit;
    const int Eof  = std::ios::eofbit;
    const int Fail = std::ios::failbit;
    const int Good = std::ios::goodbit;

#undef TEST
#define TEST(input, expect, is_eos, state) \
    test_ops (params, __LINE__, input,  T (expect), is_eos,  state)

    volatile T one  = T (1);
    volatile T zero = one - one;
    volatile T two  = one + one;

    const bool is_char   = sizeof (T) == sizeof (char);
    const bool is_signed = zero - one < zero;
    const bool is_exact  = one / two == zero;

    // see LWG issue 788
    // an istream_iterator object becomes an end-of-stream iterator
    // after a failed extraction (i.e., when the call to fail() on
    // the associated stream returns true)

    if (is_char) {

        //    +-- controlled sequence
        //    |     +-- extracted value
        //    |     |    +-- iterator equal to EOS after extraction
        //    |     |    |      +-- stream state after extraction
        //    |     |    |      |
        //    V     V    V      V
        TEST ("",  '\0', true,  Eof | Fail);
        TEST ("1", '1',  false, Good);
        TEST ("2", '2',  false, Good);
    }
    else {
        TEST ("",      0, true,  Eof | Fail);
        TEST ("1",     1, false, Eof);
        TEST ("1 ",    1, false, Good);
        TEST ("+1",    1, false, Eof);
        TEST (" 1",    1, false, Eof);
        TEST (" 1 ",   1, false, Good);
        TEST (" +1",   1, false, Eof);
        TEST ("2",     2, false, Eof);
        TEST ("+2",    2, false, Eof);
        TEST (" 2",    2, false, Eof);
        TEST (" +2",   2, false, Eof);
        TEST ("99",   99, false, Eof);
        TEST ("+99",  99, false, Eof);
        TEST (" 99",  99, false, Eof);
        TEST (" +99", 99, false, Eof);

        TEST ("+",     0, true,  Eof | Fail);
        TEST ("-",     0, true,  Eof | Fail);
        TEST (" +",    0, true,  Eof | Fail);
        TEST (" -",    0, true,  Eof | Fail);
        TEST ("++",    0, true,  Fail);
        TEST ("--",    0, true,  Fail);
        TEST (" ++",   0, true,  Fail);
        TEST (" --",   0, true,  Fail);
        TEST ("*",     0, true,  Fail);
        TEST (" *",    0, true,  Fail);

        if (is_signed) {
            TEST ("-1",    -1, false, Eof);
            TEST (" -1",   -1, false, Eof);
            TEST ("-2",    -2, false, Eof);
            TEST (" -2",   -2, false, Eof);
            TEST ("-99",  -99, false, Eof);
            TEST (" -99", -99, false, Eof);
        }

        if (is_exact) {
            TEST ("1.2",   1, false, Good);
            TEST ("2.3 ",  2, false, Good);
            TEST (" 3.4",  3, false, Good);
            TEST (" +4.",  4, false, Good);
        }
        else {
            TEST (".1",          0.1,      false, Eof);
            TEST ("+.2",         0.2,      false, Eof);
            TEST ("-.3",        -0.3,      false, Eof);
            TEST (" +.4",        0.4,      false, Eof);
            TEST (" -.5",       -0.5,      false, Eof);
            TEST ("1..",         1.0,      false, Good);

            TEST ("1.234",       1.234,    false, Eof);
            TEST ("+1.235",     +1.235,    false, Eof);
            TEST ("-1.236",     -1.236,    false, Eof);
            TEST (" +1.237",    +1.237,    false, Eof);
            TEST (" -1.238",    -1.238,    false, Eof);
            TEST ("1.239e1",     1.239e1,  false, Eof);
            TEST ("1.240e+1",    1.240e+1, false, Eof);
            TEST ("-1.241e+2",  -1.241e+2, false, Eof);
            TEST ("-1.242e-3 ", -1.242e-3, false, Good);

            TEST ("1.3e",        0,        true,  Eof | Fail);
            TEST ("1.4e+",       0,        true,  Eof | Fail);
            TEST ("1.5e-",       0,        true,  Eof | Fail);
            TEST ("1.6e+ ",      0,        true,  Fail);
        }
    }
}

/***********************************************************************/

enum TestId { TestCtors, TestOps };

// dispatch to test_ctors to test_ops
template <class T, class CharT, class Traits, class Dist>
void do_test (TestId      id,
              const char *tname,
              const char *tfmt,
              const char *cname,
              const char *trname,
              const char *dname)
{
    const ParamNames names = { tname, tfmt, cname, trname, dname };
    const TempParams<T, CharT, Traits, Dist> params = { names };

    if (TestCtors == id) {
        test_ctors (params);
    }
    else if (TestOps == id) {
        test_ops (params);
    }
}

/***********************************************************************/

int opt_char;
int opt_wchar_t;
int opt_UserTraits;
int opt_short;
int opt_int;
int opt_long;
int opt_double;


// exercise different specializations of T
template <class CharT, class Traits, class Dist>
void do_test (TestId      id,
              const char *cname,
              const char *trname,
              const char *dname)
{
#undef TEST
#define TEST(T, fmt)   \
    if (opt_ ## T)     \
        do_test<T, CharT, Traits, Dist>(id, #T, fmt, cname, trname, dname)

    // cannot instantiate basic_istream<CharT>::operator>>(char&)
    // for any CharT other than char
    // TEST (char,  "%{hhi}");

    TEST (short,  "%hi");
    TEST (int, "   %i");
    TEST (long,   "%li");
    TEST (double, "%g");
}


/***********************************************************************/

// exercise different specializations of Distance
template <class CharT, class Traits>
void do_test (TestId id, const char *cname, const char *trname)
{
    do_test<CharT, Traits, short>(id, cname, trname, "short");
    do_test<CharT, Traits, std::ptrdiff_t>(id, cname, trname, 0);
}

/***********************************************************************/

static int
run_test (int, char*[])
{
    static const TestId test_ids [] = { TestCtors, TestOps };

    if (opt_char < 0)
        rw_note (0, 0, __LINE__, "tests of char specialization disabled");

    if (opt_wchar_t < 0)
        rw_note (0, 0, __LINE__, "tests of wchar_t specialization disabled");

    if (opt_UserTraits < 0)
        rw_note (0, 0, __LINE__, "tests of UserTraits specialization disabled");

    typedef std::char_traits<char> Traits;
    typedef UserTraits<char>       UsrTraits;

    for (unsigned i = 0; i != sizeof test_ids / sizeof *test_ids; ++i) {

        const TestId id = test_ids [i];

        if (0 <= opt_char) {

            // call these helper functions directly to avoid instantiating
            // basic_istream<CharT>::operator>>(char&) on any CharT other
            // than char
            do_test<char, char, Traits, std::ptrdiff_t>(id, "char", "%{hhi}",
                                                        0, 0, 0);

            if (0 <= opt_UserTraits)
                do_test<char, char, UsrTraits, std::ptrdiff_t>(id, "char",
                    "%{hhi}", 0, "UserTraits<char>", "std::ptrdiff_t");

            do_test<char, Traits>(id, 0, 0);

            if (0 <= opt_UserTraits)
                do_test<char, UsrTraits>(id, "char", "UserTraits<char>");
        }

#ifndef _RWSTD_NO_WCHAR_T

        if (0 <= opt_wchar_t) {

            typedef std::char_traits<wchar_t> WTraits;
            typedef UserTraits<wchar_t>       WUsrTraits;

            do_test<wchar_t, WTraits>(id, 0, 0);

            if (0 <= opt_UserTraits)
                do_test<wchar_t, WUsrTraits>(id, "wchar_t",
                                             "UserTraits<wchar_t>");

        }

#endif   // _RWSTD_NO_WCHAR_T

    }

    return 0;
}


/***********************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.istream.iterator",
                    0 /* no comment */,
                    run_test,
                    "|-char~ "
                    "|-wchar_t~ "
                    "|-UserTraits~ "
                    "|-short~ "
                    "|-int~ "
                    "|-long~ "
                    "|-double~ ",
                    &opt_char,
                    &opt_wchar_t,
                    &opt_UserTraits,
                    &opt_short,
                    &opt_int,
                    &opt_long,
                    &opt_double,
                    (void*)0   /* sentinel */);
}
