/**************************************************************************
 *
 * 20.specialized.stdcxx-390.cpp - regression test for STDCXX-390
 *
 * http://issues.apache.org/jira/browse/STDCXX-390
 *
 * $Id: 20.specialized.stdcxx-390.cpp 580483 2007-09-28 20:55:52Z sebor $
 *
 **************************************************************************
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

#include <memory>

int main ()
{
    int i = 0;
    const int ci = 0;
#if TEST_RW_EXTENSIONS // the standard does not require that volatile work here
    volatile int vi = 0;
    const volatile int cvi = 0;
#else // !TEST_RW_EXTENSIONS
    int vi = 0;
    const int cvi = 0;
#endif // !TEST_RW_EXTENSIONS

    std::uninitialized_copy (&i, &i, &i);
    std::uninitialized_copy (&i, &i, &vi);

    std::uninitialized_copy (&ci, &ci, &i);
    std::uninitialized_copy (&ci, &ci, &vi);

    std::uninitialized_copy (&vi, &vi, &i);
    std::uninitialized_copy (&vi, &vi, &vi);

    std::uninitialized_copy (&cvi, &cvi, &i);
    std::uninitialized_copy (&cvi, &cvi, &vi);
}
