/************************************************************************
 *
 * 23.list.insert.stdcxx-331.cpp - test case from STDCXX-331 issue
 *
 * $Id: 23.list.insert.stdcxx-331.cpp 550991 2007-06-26 23:58:07Z sebor $
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

#include <list>
#include <cassert>

static int throw_inx = -1;

class ListItem
{
public:
    static int count_;

    void test ()
    {
        if (throw_inx == count_)
            throw count_;

        ++count_;
    }

    ListItem () { test (); }

    ListItem (const ListItem&) { test (); }

    ~ListItem () { --count_; }
};

int ListItem::count_ = 0;

int main (int argc, char* argv[])
{
    typedef std::list<ListItem> List;
    ListItem items [20];
    List lst (1);

    bool thrown = false;
    throw_inx = 10;

    List::iterator it = lst.begin ();
    ListItem & ref = *it;

    try {
        ListItem::count_ = 0;
        lst.insert (it, 20, items [0]);
    } catch (...) {
        thrown = true;
    }

    assert (thrown);
#if TEST_RW_EXTENSIONS
    // Should have no effect only if inserting a single item.
    // 23.1 - item 10
    assert (it == lst.begin ());
    assert (&ref == &*lst.begin ());
    assert (1 == lst.size ());
    assert (0 == ListItem::count_);
#endif

    try {
        ListItem::count_ = 0;
        lst.insert (it, items, items + 20);
    } catch (...) {
        thrown = true;
    }

    assert (thrown);
#if TEST_RW_EXTENSIONS
    // Should have no effect only if inserting a single item.
    // 23.1 - item 10
    assert (it == lst.begin ());
    assert (&ref == &*lst.begin ());
    assert (1 == lst.size ());
    assert (0 == ListItem::count_);
#endif

    return 0;
}
