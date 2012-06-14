/***************************************************************************
 *
 * 23.set.stdcxx-216.cpp - regression test for STDCXX-216
 *
 * $Id: 23.set.stdcxx-216.cpp 640522 2008-03-24 19:00:16Z vitek $
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
 * Copyright 1994-2008 Rogue Wave Software.
 *
 **************************************************************************/

#include <set>
#include <cassert>

struct Value
{
    static const unsigned magic = 0x12344321;

    Value (unsigned value = 0)
        : value (value)
        , valid (magic)
    {
    }

    Value (const Value& key)
        : value (key.value)
        , valid (key.magic)
    {
    }

    Value& operator= (const Value& rhs)
    {
        // assignment only valid from valid values
        assert (rhs.is_valid ());

        value = rhs.value;
        valid = rhs.valid;
        return *this;
    }

    ~Value ()
    {
        // destruction only allowed for valid values
        assert (is_valid ());

        valid = 0;
    }

    bool is_valid () const
    {
        return valid == magic;
    }

    friend
    bool operator< (const Value& lhs, const Value& rhs)
    {
        // comparing against an invalid value is forbidden
        assert (lhs.is_valid ());
        assert (rhs.is_valid ());

        const int lhs_is_odd = lhs.value & 1;
        const int rhs_is_odd = rhs.value & 1;

        // sort all even numbers in ascending order
        // followed by odd numbers in ascending order
        return   lhs_is_odd != rhs_is_odd
               ? lhs_is_odd  < rhs_is_odd
               : lhs.value   < rhs.value;
    }

    unsigned value;
    unsigned valid;
};

//#include <iostream>
//
//void dump (const std::set<Value>& s)
//{
//    std::set<Value>::const_iterator b = s.begin ();
//    std::set<Value>::const_iterator e = s.end   ();
//
//    for (/**/; b != e; ++b)
//        std::cout << b->value << ' ';
//    std::cout << std::endl;
//}

int main ()
{
    // insert at begin
    {
        std::set<Value> s;

        std::set<Value>::iterator i (s.begin ());
        for (unsigned n = 0; n < 10; ++n)
            s.insert (i, n);
    }

    // insert after last
    {
        std::set<Value> s;

        std::set<Value>::iterator i (s.end ());
        for (unsigned n = 0; n < 10; ++n)
            i = s.insert (i, n);
    }

    // insert at end
    {
        std::set<Value> s;

        std::set<Value>::iterator i (s.end ());
        for (unsigned n = 0; n < 10; ++n)
            s.insert (i, n);
    }

    return 0;
}

