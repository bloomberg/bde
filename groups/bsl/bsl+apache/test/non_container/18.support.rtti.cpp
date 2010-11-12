/***************************************************************************
 *
 * 18.support.rtti.cpp - test exercising 18.5 [lib.support.rtti]
 *
 * $Id: 18.support.rtti.cpp 648752 2008-04-16 17:01:56Z faridz $
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
 * Copyright 2001-2008 Rogue Wave Software.
 * 
 **************************************************************************/

#include <rw/_defs.h> 
#if defined (__IBMCPP__) && !defined (_RWSTD_NO_IMPLICIT_INCLUSION)
// Disable implicit inclusion to work around 
// a limitation in IBM's VisualAge 5.0.2.0 (see PR#26959) 

#  define _RWSTD_NO_IMPLICIT_INCLUSION 
#endif 

/**************************************************************************/


// verifies that typeid is correctly declared for MSVC (see PR #25603)
void foo ()
{
    _THROW (0);   // must appear before #include <typeinfo>
}

/**************************************************************************/


#include <typeinfo>
#include <driver.h>
#include <valcmp.h>

// polymorphic classes (10.3, p1) used in tests below
struct B { virtual ~B () { } };
struct D1: B { };
struct D2: B { };

/**************************************************************************/

static int
run_test (int, char* [])
{
    if (1) {
        // exercise 18.5, the synopsis of <typeinfo>

        std::type_info  *ti = 0;
        std::bad_cast   *bc = 0;
        std::bad_typeid *bt = 0;

        _RWSTD_UNUSED (ti);
        _RWSTD_UNUSED (bc);
        _RWSTD_UNUSED (bt);
    }

    if (1) {
        // exercise 18.5.1, class type_info interface

        // 18.5.1, p2
        bool (std::type_info::*p_op_eq)(const std::type_info&) const =
            &std::type_info::operator==;

        // 18.5.1, p3
        bool (std::type_info::*p_op_neq)(const std::type_info&) const =
            &std::type_info::operator!=;

        // 18.5.1, p5
        bool (std::type_info::*p_before)(const std::type_info&) const =
            &std::type_info::before;

        // 18.5.1, p7
        const char* (std::type_info::*p_name)() const = &std::type_info::name;

        _RWSTD_UNUSED (p_op_eq);
        _RWSTD_UNUSED (p_op_neq);
        _RWSTD_UNUSED (p_before);
        _RWSTD_UNUSED (p_name);
    }

    if (1) {
        // exercise 18.5.1, class type_info functionality
        D1 d1;
        D2 d2;

        const std::type_info &ti_D1 = typeid (D1);
        const std::type_info &ti_D2 = typeid (D2);

        const std::type_info &ti_d1 = typeid (d1);
        const std::type_info &ti_d2 = typeid (d2);


        const char *D1_name = ti_D1.name () ? ti_D1.name () : "(D1 null)";
        const char *D2_name = ti_D2.name () ? ti_D2.name () : "(D2 null)";
        const char *d1_name = ti_d1.name () ? ti_d1.name () : "(d1 null)";
        const char *d2_name = ti_d2.name () ? ti_d2.name () : "(d2 null)";

        // 18.5.1, p2
        rw_assert (ti_D1 == ti_D1, 0, __LINE__,
                   "std::type_info::operator==(): \"%s\" != \"%s\"",
                   D1_name, D1_name);
        rw_assert (ti_D1 == ti_d1, 0, __LINE__,
                   "std::type_info::operator==(): \"%s\" != \"%s\"",
                   D1_name, d1_name);
        rw_assert (!(ti_D1 == ti_D2), 0, __LINE__,
                   "std::type_info::operator==(): \"%s\" == \"%s\"",
                   D1_name, D2_name);
        rw_assert (ti_d1 == ti_d1, 0, __LINE__,
                   "std::type_info::operator==(): \"%s\" != \"%s\"",
                   d1_name, d1_name);
        rw_assert (ti_d1 == ti_D1, 0, __LINE__,
                   "std::type_info::operator==(): \"%s\" != \"%s\"",
                   d1_name, D1_name);
        rw_assert (!(ti_d1 == ti_d2), 0, __LINE__,
                   "std::type_info::operator==(): \"%s\" == \"%s\"",
                   d1_name, d2_name);

        // 18.5.1, p3
        rw_assert (ti_D1 != ti_D2, 0, __LINE__,
                   "std::type_info::operator!=(): \"%s\" == \"%s\"",
                   D1_name, D2_name);
        rw_assert (ti_D1 != ti_d2, 0, __LINE__,
                   "std::type_info::operator!=(): \"%s\" == \"%s\"",
                   D1_name, d2_name);
        rw_assert (!(ti_D1 != ti_D1), 0, __LINE__,
                   "std::type_info::operator!=(): \"%s\" != \"%s\"",
                   D1_name, D1_name);
        rw_assert (ti_d1 != ti_d2, 0, __LINE__,
                   "std::type_info::operator!=(): \"%s\" == \"%s\"",
                   d1_name, d2_name);
        rw_assert (ti_d1 != ti_D2, 0, __LINE__,
                   "std::type_info::operator!=(): \"%s\" == \"%s\"",
                   d1_name, D2_name);
        rw_assert (!(ti_d1 != ti_d1), 0, __LINE__,
                   "std::type_info::operator!=(): \"%s\" != \"%s\"",
                   d1_name, d1_name);

        // 18.5.1, p5
        rw_assert (!ti_D1.before (ti_D1) && !ti_D2.before (ti_D2),
                   0, __LINE__, "std::type_info::before ()");
        rw_assert (ti_D1.before (ti_D2) || ti_D2.before (ti_D1),
                   0, __LINE__, "std::type_info::before ()");
        rw_assert (ti_d1.before (ti_d2) || ti_d2.before (ti_d1),
                   0, __LINE__, "std::type_info::before ()");
        rw_assert (!ti_d1.before (ti_d1) && !ti_d2.before (ti_d2),
                   0, __LINE__, "std::type_info::before ()");

        // 18.5.1, p7
        rw_assert (0 == rw_strncmp (D1_name, d1_name), 0, __LINE__,
                   "std::type_info::name (): \"%s\" != \"%s\"",
                   D1_name, d1_name);
        rw_assert (0 != rw_strncmp (D1_name, D2_name), 0, __LINE__,
                   "std::type_info::name (): \"%s\" == \"%s\"",
                   D1_name, D2_name);
        rw_assert (0 == rw_strncmp (D2_name, d2_name), 0, __LINE__,
                   "std::type_info::name (): \"%s\" != \"%s\"",
                   D2_name, d2_name);
        rw_assert (0 != rw_strncmp (d1_name, d2_name), 0, __LINE__,
                   "std::type_info::name (): \"%s\" == \"%s\"",
                   d1_name, d2_name);
    }

    if (1) {
        // exercise 18.5.2, class bad_cast interface

        // std::bad_cast must publicly derive from std::exception
        const std::bad_cast  *pbc = 0;
        const std::exception *pe  = pbc;

        // 18.5.2, p2
        std::bad_cast bc;

        // 18.5.2, p4 - copy ctor
        std::bad_cast bc2 (bc);

        // 18.5.2, p4 - assignment
        std::bad_cast& (std::bad_cast::*p_op_assign)(const std::bad_cast&) 
            _PTR_THROWS (()) = &std::bad_cast::operator=;

        // 18.5.2, p5
        const char* (std::bad_cast::*p_what)() const _PTR_THROWS (()) =
            &std::bad_cast::what;

        _RWSTD_UNUSED (pbc);
        _RWSTD_UNUSED (pe);
        _RWSTD_UNUSED (bc);
        _RWSTD_UNUSED (bc2);
        _RWSTD_UNUSED (p_op_assign);
        _RWSTD_UNUSED (p_what);
    }

    if (1) {
        // exercise 18.5.2, class bad_cast functionality

#ifndef _RWSTD_NO_EXCEPTIONS
#  ifndef _RWSTD_NO_DYNAMIC_CAST

        const char *caught = "no exception";

        D1 d1;
        B &b = d1;

        try {
            D2 &d2 = dynamic_cast<D2&>(b);

            _RWSTD_UNUSED (d2);
        }
        catch (const std::bad_cast &bc) {
            caught = "std::bad_cast";

            // 18.5.2, p2
            std::bad_cast bc2;

            // 18.5.2, p4 - copy ctor
            std::bad_cast bc3 (bc);

            const char* const bc_what  = bc.what ();
            const char*       bc2_what = bc2.what ();
            const char* const bc3_what = bc3.what ();

            if (0 == bc2_what)
                rw_assert (false, 0, __LINE__, "bad_cast().what() != 0");

            if (0 == bc3_what)
                rw_assert (false, 0, __LINE__,
                           "bad_cast::what() != 0 failed "
                           "for a copy of a caught exception object");

            if (bc2_what && bc3_what)
                rw_warn (0 == rw_strncmp (bc2_what, bc3_what),
                         0, __LINE__,
                         "bad_cast::bad_cast (const bad_cast&): "
                         "\"%s\" != \"%s\"", bc_what, bc3_what);

            // 18.5.2, p4 - assignment
            bc2 = bc;

            bc2_what = bc2.what ();

            if (0 == bc_what)
                rw_assert (false, 0, __LINE__,
                           "bad_cast::what() != 0 failed "
                           "for a caught exception object");

            if (0 == bc2_what)
                rw_assert (false, 0, __LINE__,
                           "bad_cast::what() != 0 failed "
                           "for an assigned exception object");

            if (bc_what && bc2_what)
                rw_warn (0 == rw_strncmp (bc_what, bc2_what),
                         0, __LINE__,
                         "bad_cast::operator=(const bad_cast&): "
                         "\"%s\" != \"%s\"", bc_what, bc2_what);

            // 18.5.2, p5
            if (bc_what)
                rw_assert (0 == rw_strncmp (bc.what (), bc.what ()),
                           0, __LINE__, 
                           "bad_cast::what() const: \"%s\" != \"%s\"",
                           bc.what (), bc.what ());
        }
        catch (const std::exception&) {
            caught = "std::exception";
        }
        catch (...) {
            caught = "unknown exception";
        }

#if    !defined (_RWSTD_NO_STD_BAD_CAST) \
    || !defined (_RWSTD_NO_RUNTIME_IN_STD)

       const char expect[] = "std::bad_cast";

#else

       const char expect[] = "std::bad_cast (alias for ::bad_cast)";

#endif   // !NO_STD_BAD_CAST || !NO_RUNTIME_IN_STD

        rw_assert (0 == rw_strncmp (caught, "std::bad_cast"),
                   0, __LINE__,
                   "dynamic_cast<>() threw %s, expected %s; this suggests "
                   "that bad_cast might be defined in the wrong namespace",
                   caught, expect);

        _RWSTD_UNUSED (d1);
        _RWSTD_UNUSED (b);

#  endif   // _RWSTD_NO_DYNAMIC_CAST
#endif   // _RWSTD_NO_EXCEPTIONS
    }

    if (1) {
        // exercise 18.5.3, class bad_typeid interface

        // std::bad_cast must publicly derive from std::exception
        const std::bad_typeid *pbt = 0;
        const std::exception  *pe  = pbt;

        // 18.5.2, p2
        std::bad_typeid bt;

        // 18.5.2, p4 - copy ctor
        std::bad_typeid bt2 (bt);

        // 18.5.2, p4 - assignment
        std::bad_typeid& (std::bad_typeid::*p_op_assign)(const std::bad_typeid&)
            _PTR_THROWS (()) = &std::bad_typeid::operator=;

        // 18.5.2, p5
        const char* (std::bad_typeid::*p_what)() const _PTR_THROWS (()) =
            &std::bad_typeid::what;

        _RWSTD_UNUSED (pbt);
        _RWSTD_UNUSED (pe);
        _RWSTD_UNUSED (bt);
        _RWSTD_UNUSED (bt2);
        _RWSTD_UNUSED (p_op_assign);
        _RWSTD_UNUSED (p_what);
    }

    if (1) {
        // exercise 18.5.3, class bad_typeid functionality

#ifndef _RWSTD_NO_EXCEPTIONS

        const char *caught = "no exception";

        try {

#if !defined (__GNUG__) || __GNUG__ > 2

            B *b = 0;

            // 5.2.8, p2 - typeid(0) throws std::bad_typeid
            const std::type_info &ti = typeid (*b);

            _RWSTD_UNUSED (b);
            _RWSTD_UNUSED (ti);

#else

            // working around a gcc 2.x bug
            caught = "SIGSEGV (program dumps core)";

#endif   // gcc < 3.0

        }
        catch (const std::bad_typeid &bt) {
            caught = "std::bad_typeid";

            // 18.5.2, p2
            std::bad_typeid bt2;

            // 18.5.2, p4 - copy ctor
            std::bad_typeid bt3 (bt);

            // verify that what() returns the same string
            // after copy construction
            rw_warn (0 == rw_strncmp (bt.what (), bt3.what ()),
                     0, __LINE__,
                     "std::bad_typeid::bad_typeid (const bad_typeid&): "
                     "\"%s\" != \"%s\"", bt.what (), bt3.what ());

            // 18.5.2, p4 - assignment
            bt2 = bt;

            // verify that what() returns the same string
            // after assignment
            rw_warn (0 == rw_strncmp (bt.what (), bt2.what ()),
                     0, __LINE__,
                     "std::bad_typeid::operator=(const bad_typeid&): "
                     "\"%s\" != \"%s\"", bt.what (), bt2.what ());

            // 18.5.2, p5
            rw_assert (0 == rw_strncmp (bt.what (), bt.what ()),
                       0, __LINE__,
                       "std::bad_typeid::what() const: "
                       "\"%s\" != \"%s\"", bt.what (), bt.what ());
        }
        catch (const std::exception&) {
            caught = "std::exception";
        }
        catch (...) {
            caught = "unknown exception";
        }

#if    !defined (_RWSTD_NO_STD_BAD_TYPEID) \
    || !defined (_RWSTD_NO_RUNTIME_IN_STD)

       const char expect[] = "std::bad_typeid";

#else

       const char expect[] = "std::bad_typeid (alias for ::bad_typeid)";

#endif   // !NO_STD_BAD_TYPEID || !NO_RUNTIME_IN_STD


        rw_assert (0 == rw_strncmp (caught, "std::bad_typeid"),
                   0, __LINE__,
                   "typeid ((T*)0) threw %s, expected %s; this suggests "
                   "that bad_typeid might be defined in the wrong namespace",
                   caught, expect);

#endif   // _RWSTD_NO_EXCEPTIONS
    }

    return 0;
}

int main (int argc, char* argv [])
{
    return rw_test (argc, argv, __FILE__,
                    "lib.support.rtti",
                    0 /* no comment */,
                    run_test,
                    "",
                    (void*)0);
}
