/***************************************************************************
 *
 * 26.class.gslice.cpp - tests exercising class std::gslice
 *
 * $Id: 26.class.gslice.cpp 648752 2008-04-16 17:01:56Z faridz $
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

#include <cstdlib>    // for strtoul(), size_t
#include <valarray>   // for gslice, valarray

#include <driver.h>

/**************************************************************************/

// returns a valarray constructed from a string of comma-separated values
static std::valarray<std::size_t>
make_array (const char *s)
{
    if (0 == s)
        return std::valarray<std::size_t>();

    std::size_t buf [256];

    for (std::size_t i = 0; ; ++i) {

        char *end;
        unsigned long val = std::strtoul (s, &end, 10);

        RW_ASSERT ('\0' == *end || ',' == *end);

        buf [i] = std::size_t (val);

        if ('\0' == *end)
            return std::valarray<std::size_t>(buf, i + 1);

        s = end + 1;
    }
}

/**************************************************************************/

// returns the size of the index array represented by the genreralized slice
static std::size_t
get_array_size (const std::gslice &gsl)
{
    const std::valarray<std::size_t> sizes = gsl.size ();

    std::size_t asize = sizes.size () ? 1 : 0;

    for (std::size_t i = 0; i != sizes.size (); ++i) {
        asize *= sizes [i];
    }

    return asize;
}

/**************************************************************************/

static std::size_t
next_index (const std::gslice &gsl, std::valarray<std::size_t> &factors)
{
    const std::size_t                start    = gsl.start ();
    const std::valarray<std::size_t> asizes   = gsl.size ();
    const std::valarray<std::size_t> astrides = gsl.stride ();

    const std::size_t ndims = asizes.size ();

    std::size_t inx = ndims;

    if (0 == factors.size ()) {
        factors.resize (ndims);

        return start;
    }

    while (inx && factors [inx - 1] == asizes [inx - 1] - 1)
        --inx;

    if (0 == inx) {
        factors = 0;

        return start;
    }

    ++factors [inx - 1];

    if (inx < factors.size ()) {
        for (std::size_t i = inx; i != ndims; ++i)
            factors [i] = 0;
    }
    else
        inx = factors.size ();

    std::size_t index = start;

    for (std::size_t i = 0; i != inx; ++i)
        index += factors [i] * astrides [i];

    return index;
}

/**************************************************************************/

// returns a valarray of indices reprsented by the generalized slice
static std::valarray<std::size_t>
get_index_array (const std::gslice &gsl)
{
    const std::size_t size = get_array_size (gsl);

    std::valarray<std::size_t> indices (size);

    std::valarray<std::size_t> tmpstore;

    for (std::size_t i = 0; i != size; ++i)
        indices [i] = next_index (gsl, tmpstore);

    return indices;
}

/**************************************************************************/

static void
test_gslice (std::size_t  start,
             const char  *sizes,
             const char  *strides)
{
    const std::valarray<std::size_t> asizes (make_array (sizes));
    const std::valarray<std::size_t> astrides (make_array (strides));

    const std::gslice gsl (start, asizes, astrides);

    const std::valarray<std::size_t> indices = get_index_array (gsl);

    std::size_t maxinx = 0;

    for (std::size_t i = 0; i != indices.size (); ++i)
        if (maxinx < indices [i])
            maxinx = indices [i];

    std::valarray<std::size_t> va (maxinx + 1);
    for (std::size_t i = 0; i != va.size (); ++i)
        va [i] = i;

    for (int i = 0; i != 3; ++i) {
        // repeat each test three to verify that operator[](gslice)
        // doesn't change the observable state of its argument and
        // that the same result is obtained for a copy of gslice

        const std::valarray<std::size_t> array_slice =
            i < 2 ? va [gsl] : va [std::gslice (gsl)];

        bool equal = array_slice.size () == indices.size ();

        rw_assert (equal, 0, __LINE__,
                   "size() == %zu, got %zu\n",
                   indices.size (), array_slice.size ());

        if (equal) {
            for (std::size_t j = 0; j != array_slice.size (); ++j) {

                equal = array_slice [j] == va [indices [j]];

                rw_assert (equal, 0, __LINE__,
                           "mismatch at %u, index %u: expected %u, got %u\n",
                           j, indices [j], va [indices [j]],
                           array_slice [j]);
            }
        }
    }
}

/**************************************************************************/

static void
test_gslice (const char *sizes, const char *strides)
{
    for (std::size_t i = 0; i != 10; ++i)
        test_gslice (i, sizes, strides);
}

/**************************************************************************/

static int
run_test (int, char**)
{
    test_gslice ("1", "1");
    test_gslice ("1", "2");
    test_gslice ("2", "1");
    test_gslice ("2", "3");
    test_gslice ("3", "2");

    test_gslice ("1,1", "1,1");
    test_gslice ("1,1", "1,2");
    test_gslice ("1,1", "2,1");
    test_gslice ("1,9", "2,1");
    test_gslice ("9,1", "2,1");

    test_gslice ("1,1,1", "1,1,1");
    test_gslice ("1,2,3", "4,5,6");
    test_gslice ("6,5,4", "3,2,1");
    test_gslice ("10,11,12", "13,14,15");

    // includes example from class.gslice, p3
    test_gslice ("2,4,3", "19,4,1");

    // includes example of a degenerate gslice from p5
    test_gslice ("2,4,3", "19,4,1");

    return 0;
}

/**************************************************************************/

int main (int argc, char *argv[])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.class.gslice",
                    0 /* no comment */,
                    run_test,
                    "",
                    (void*)0   /* sentinel */);
}
