/************************************************************************
 *
 * 23.associative.stdcxx-16.cpp - regression test for STDCXX-16
 *   http://issues.apache.org/jira/browse/STDCXX-16
 *
 * $Id: 23.associative.stdcxx-16.cpp 580483 2007-09-28 20:55:52Z sebor $
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

#include <set>
#include <map>
#include <cassert>

template <class T>
struct RuntimeCmp
{
    enum cmp_mode {normal, reverse};

    cmp_mode mode;

    RuntimeCmp (cmp_mode m = normal) : mode (m) { }

    bool operator() (const T& t1, const T& t2) const {
        return mode == normal ? t1 < t2 : t2 < t1;
    }
};

template <class T>
bool operator== (const RuntimeCmp <T>& lc, const RuntimeCmp <T>& rc) {
    return lc.mode == rc.mode;
}

typedef std::set <int, RuntimeCmp <int> > IntSet;
typedef std::map <int, int, RuntimeCmp <int> > IntMap;

int main ()
{
    RuntimeCmp <int> reverse_order (RuntimeCmp<int>::reverse);

    IntSet set1;
    IntSet set2 (reverse_order);
    set1 = set2;
    assert (set1.key_comp () == set2.key_comp ());
    assert (set1.value_comp () == set2.value_comp ());

    IntMap map1;
    IntMap map2 (reverse_order);
    map1 = map2;
    assert (map1.key_comp () == map2.key_comp ());

    return 0;
}
