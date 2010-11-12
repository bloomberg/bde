/***************************************************************************
 *
 * 21.string.find.first.of.cpp - 
 *      string test exercising lib.string.find.first.of
 *
 * $Id: 21.string.find.first.of.cpp 590052 2007-10-30 12:44:14Z faridz $
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
 * Copyright 2006 Rogue Wave Software.
 * 
 **************************************************************************/

#include <string>           // for string
#include <cstdlib>          // for free(), size_t
#include <stdexcept>        // for length_error

#include <21.strings.h>     // for StringMembers
#include <driver.h>         // for rw_test()
#include <rw_allocator.h>   // for UserAlloc
#include <rw_char.h>        // for rw_expand()

/**************************************************************************/

// for convenience and brevity
#define FindFirstOf(sig)          StringIds::find_first_of_ ## sig

static const char* const exceptions[] = {
    "unknown exception", "out_of_range", "length_error",
    "bad_alloc", "exception"
};

/**************************************************************************/

// used to exercise 
// find_first_of (const value_type*)
static const StringTestCase 
cptr_test_cases [] = {

#undef TEST
#define TEST(str, arg, res)                 \
    { __LINE__, -1, -1, -1, -1, -1,         \
      str, sizeof str - 1, arg,             \
      sizeof arg - 1, 0, res, 0             \
    }

    //    +----------------------------------- controlled sequence
    //    |             +--------------------- sequence to be found
    //    |             |                +---- expected result 
    //    |             |                |                               
    //    |             |                |                     
    //    V             V                V        
    TEST ("ab",         "a",             0),   

    TEST ("",           "",           NPOS),   
    TEST ("",           "\0",         NPOS),    
    TEST ("",           "a",          NPOS),   

    TEST ("\0",         "",           NPOS),    
    TEST ("\0",         "\0",         NPOS),   
    TEST ("\0",         "a",          NPOS),   

    TEST ("bbcdefghij", "a",          NPOS),    
    TEST ("abcdefghij", "a",             0),  
    TEST ("abcdefghij", "f",             5),  
    TEST ("abcdefghij", "j",             9),  

    TEST ("edfcbbhjig", "cba",           3),     
    TEST ("ecdfcbahca", "cba",           1),   
    TEST ("bacbahjicg", "cba",           0),  
    TEST ("efhijhjieg", "cba",        NPOS), 

    TEST ("e\0cb\0\0g", "b\0\0g",        3),    
    TEST ("e\0cb\0\0g", "cbe",           0),    
    TEST ("\0cb\0\0ge", "\0\0ge",     NPOS),   
    TEST ("\0cb\0\0be", "b\0c",          2),   
    TEST ("e\0gbg\0\0", "bg",            2),    
    TEST ("e\0dg\0\0a", "cba",           6),  
    TEST ("a\0b",       "e\0gbg\0\0", NPOS),    
    TEST ("a\0b",       "eb\0gg\0\0",    2), 
    TEST ("a\0b",       "e\0gg\0\0a", NPOS),

    TEST ("bcbedfbjih", "a",          NPOS),   
    TEST ("bcaedfajih", "a",             2),    
    TEST ("bcedfaajih", "a",             5),    
    TEST ("bcaaedfaji", "a",             2),    

    TEST ("aaaaaaaaaa", "aaaaaaaaaa",    0),     
    TEST ("aaaaabaaaa", "cccccccccb",    5),   
    TEST ("aabaabaaaa", "cccccccccb",    2),
    TEST ("bbb",        "aaaaaaaaba",    0),        
    TEST ("bbb",        "aaaaaaaaaa", NPOS),    

    TEST ("x@4096",     "",           NPOS),     
    TEST ("x@4096",     "a",          NPOS),  
    TEST ("x@4096",     "x",             0),       
    TEST ("x@4096",     "axa",           0),   
    TEST ("abc",        "x@4096",     NPOS),    
    TEST ("axbc",       "x@4096",        1), 
    TEST ("x@2048axxx", "y@4096a",    2048), 

    TEST ("abcdefghij", 0,               0),      
    TEST ("\0cb\0\0ge", 0,            NPOS),       
    TEST ("x@4096",     0,               0),

    TEST ("last test",  "test",          2)       
};

/**************************************************************************/

// used to exercise 
// find_first_of (const basic_string&)
static const StringTestCase 
cstr_test_cases [] = {

#undef TEST     
#define TEST(str, arg, res)                 \
    { __LINE__, -1, -1, -1, -1, -1,         \
      str, sizeof str - 1, arg,             \
      sizeof arg - 1, 0, res, 0             \
    }

    //    +------------------------------------ controlled sequence
    //    |             +---------------------- sequence to be found
    //    |             |                +----- expected result 
    //    |             |                |                               
    //    |             |                |                     
    //    V             V                V        
    TEST ("ab",         "a",             0),   

    TEST ("",           "",           NPOS),   
    TEST ("",           "\0",         NPOS),    
    TEST ("",           "a",          NPOS),   

    TEST ("\0",         "",           NPOS),    
    TEST ("\0",         "\0",            0),   
    TEST ("\0",         "a",          NPOS),   

    TEST ("bbcdefghij", "a",          NPOS),    
    TEST ("abcdefghij", "a",             0),  
    TEST ("abcdefghij", "f",             5),  
    TEST ("abcdefghij", "j",             9),  

    TEST ("edfcbbhjig", "cba",           3),     
    TEST ("ecdfcbahca", "cba",           1),   
    TEST ("bacbahjicg", "cba",           0),  
    TEST ("efhijhjieg", "cba",        NPOS), 

    TEST ("e\0cb\0\0g", "ad",         NPOS),  
    TEST ("e\0cb\0\0g", "b\0\0g",        1),    
    TEST ("e\0cb\0\0g", "cbe",           0),    
    TEST ("\0cb\0\0ge", "\0\0ge",        0),   
    TEST ("\0cb\0\0be", "b\0c",          0),  
    TEST ("\0cb\0\0be", "bc",            1),
    TEST ("\0ab\0\0be", "bc",            2),
    TEST ("e\0gbg\0\0", "bg",            2),    
    TEST ("e\0dg\0\0a", "cba",           6),  
    TEST ("a\0b",       "e\0gbg\0\0",    1),    
    TEST ("a\0b",       "eb\0gg\0\0",    1), 
    TEST ("a\0b",       "e\0gg\0\0a",    0),

    TEST ("bcbedfbjih", "a",          NPOS),   
    TEST ("bcaedfajih", "a",             2),    
    TEST ("bcedfaajih", "a",             5),    
    TEST ("bcaaedfaji", "a",             2),    

    TEST ("aaaaaaaaaa", "aaaaaaaaaa",    0),     
    TEST ("aaaaabaaaa", "cccccccccb",    5),   
    TEST ("aabaabaaaa", "cccccccccb",    2),
    TEST ("bbb",        "aaaaaaaaba",    0),        
    TEST ("bbb",        "aaaaaaaaaa", NPOS),    

    TEST ("x@4096",     "",           NPOS),     
    TEST ("x@4096",     "a",          NPOS),  
    TEST ("x@4096",     "x",             0),       
    TEST ("x@4096",     "axa",           0),   
    TEST ("abc",        "x@4096",     NPOS),    
    TEST ("axbc",       "x@4096",        1), 
    TEST ("x@2048axxx", "y@4096a",    2048),

    TEST ("abcdefghij", 0,               0),      
    TEST ("\0cb\0\0ge", 0,               0),       
    TEST ("x@4096",     0,               0),

    TEST ("last test",  "test",          2)      
};

/**************************************************************************/

// used to exercise 
// find_first_of (const value_type*, size_type)
static const StringTestCase 
cptr_size_test_cases [] = {

#undef TEST
#define TEST(str, arg, off, res)            \
    { __LINE__, off, -1, -1, -1, -1,        \
      str, sizeof str - 1, arg,             \
      sizeof arg - 1, 0, res, 0             \
    }

    //    +------------------------------------- controlled sequence
    //    |            +------------------------ sequence to be found
    //    |            |               +-------- find_first_of() off argument 
    //    |            |               |     +-- expected result  
    //    |            |               |     |                             
    //    |            |               |     |             
    //    V            V               V     V                 
    TEST ("ab",        "a",            0,    0),   

    TEST ("",           "",            0, NPOS),    
    TEST ("",           "\0",          0, NPOS),    
    TEST ("",           "a",           0, NPOS),    

    TEST ("\0",         "",            0, NPOS),    
    TEST ("\0",         "\0",          0, NPOS),   
    TEST ("\0",         "\0",          1, NPOS),    
    TEST ("\0",         "a",           0, NPOS),    

    TEST ("bbcdefghij", "a",           0, NPOS),    
    TEST ("abcdefghij", "a",           0,    0),   
    TEST ("abcdefghij", "a",           2, NPOS),    
    TEST ("abcdefghij", "f",           2,    5),   
    TEST ("abcdefghij", "f",           7, NPOS),    
    TEST ("abcdefghij", "j",           9,    9),  

    TEST ("edfcbbhjig", "cba",         0,    3),    
    TEST ("edfcbahjig", "cba",         4,    4),     
    TEST ("edfcbahjig", "cba",         6, NPOS),    
    TEST ("edfcbahcba", "cba",         9,    9),       
    TEST ("cbacbahjig", "cba",         7, NPOS),    

    TEST ("e\0cb\0\0g", "b\0\0g",      0,    3),    
    TEST ("e\0cb\0\0g", "b\0\0g",      4, NPOS),    
    TEST ("e\0cb\0\0g", "cbe",         0,    0),     
    TEST ("\0cb\0\0ge", "\0\0ge",      0, NPOS),  
    TEST ("\0cb\0\0ge", "cb\0",        6, NPOS),     
    TEST ("e\0gbg\0\0", "bg",          1,    2),   
    TEST ("e\0dg\0\0a", "cba",         0,    6),    
    TEST ("a\0b",       "e\0gbg\0\0",  0, NPOS),    
    TEST ("a\0b",       "eb\0gg\0\0",  1,    2), 
    TEST ("a\0b",       "e\0gg\0\0a",  3, NPOS),

    TEST ("bcbedfbjih", "a",           0, NPOS),    
    TEST ("bcaedfajih", "a",           1,    2),     
    TEST ("bcedfaajih", "a",           6,    6),    
    TEST ("bcaaedfaji", "a",           5,    7),    

    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  0,    0),    
    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  4,    4),    
    TEST ("aaaaabaaaa", "cccccccccb",  1,    5),   
    TEST ("aabaabaaaa", "cccccccccb",  0,    2),  
    TEST ("aabaabaaaa", "cccccccccb",  6, NPOS), 
    TEST ("bbb",        "aaaaaaaaba",  2,    2),  
    TEST ("bab",        "aaaaaaaaba",  0,    0),
    TEST ("bbb",        "aaaaaaaaaa",  0, NPOS),     

    TEST ("x@4096",     "",            0, NPOS),    
    TEST ("x@4096",     "a",           0, NPOS),     
    TEST ("x@4096",     "x",           0,    0),     
    TEST ("x@4096",     "xxx",        10,   10),    
    TEST ("x@4096",     "axa",        10,   10),     
    TEST ("abc",        "x@4096",      2, NPOS),    
    TEST ("xxxxxxxxxx", "x@4096",      0,    0),
    TEST ("x@2048axxx", "y@4096a",    10, 2048),

    TEST ("x@4096",     "xxx",      4093, 4093), 
    TEST ("x@4096",     "xxx",      4096, NPOS),     

    TEST ("abcdefghij", 0,             0,    0),    
    TEST ("abcdefghij", 0,             1,    1),   
    TEST ("\0cb\0\0ge", 0,             5, NPOS),    
    TEST ("x@4096",     0,             0,    0),  
    TEST ("x@4096",     0,             7,    7),    

    TEST ("",           "",            1, NPOS),  
    TEST ("abcdefghij", "abc",        10, NPOS),   
    TEST ("abcdefghij", "cba",        10, NPOS), 

    TEST ("last test", "test",         0,    2)      
};

/**************************************************************************/

// used to exercise 
// find_first_of (const value_type*, size_type, size_type)
static const StringTestCase 
cptr_size_size_test_cases [] = {

#undef TEST
#define TEST(str, arg, off, size, res, bthrow)      \
    { __LINE__, off, size, -1, -1, -1,              \
      str, sizeof str - 1, arg,                     \
      sizeof arg - 1, 0, res, bthrow                \
    }

    //    +-------------------------------------- controlled sequence
    //    |            +------------------------- sequence to be found
    //    |            |               +--------- find_first_of() off argument
    //    |            |               |   +----- find_first_of() n argument 
    //    |            |               |   |     +- expected result  
    //    |            |               |   |     |  exception info 
    //    |            |               |   |     |  |   0 - no exception        
    //    |            |               |   |     |  |   1 - length_error  
    //    |            |               |   |     |  |                           
    //    |            |               |   |     |  +-----+             
    //    V            V               V   V     V        V             
    TEST ("ab",        "a",            0,  1,    0,       0),

    TEST ("",           "",            0,  0, NPOS,       0),
    TEST ("",           "\0",          0,  0, NPOS,       0),
    TEST ("",           "\0",          0,  1, NPOS,       0),
    TEST ("",           "a",           0,  0, NPOS,       0),
    TEST ("",           "a",           0,  1, NPOS,       0),

    TEST ("\0",         "",            0,  0, NPOS,       0),
    TEST ("\0",         "\0",          0,  1,    0,       0),
    TEST ("\0",         "\0",          1,  1, NPOS,       0),
    TEST ("\0\0",       "\0\0",        1,  1,    1,       0),
    TEST ("\0",         "a",           0,  1, NPOS,       0),

    TEST ("edfcbbhjig", "cba",         0,  3,    3,       0),
    TEST ("edfcbbhjig", "bac",         0,  2,    4,       0),
    TEST ("edfcbahjig", "cba",         7,  3, NPOS,       0),
    TEST ("ebcfeahjig", "cba",         3,  2, NPOS,       0),
    TEST ("ebcfeahjig", "cab",         3,  2,    5,       0),
    TEST ("edfcbahcba", "bac",         6,  3,    7,       0),
    TEST ("edfcbehcba", "abc",         1,  2,    4,       0),
    TEST ("edfcbahcba", "cba",         5,  1,    7,       0),
    TEST ("cbacbahjig", "cba",         6,  3, NPOS,       0),
    TEST ("cbacbahjcg", "cba",         5,  1,    8,       0),

    TEST ("e\0cb\0\0g", "b\0\0g",      0,  4,    1,       0),
    TEST ("e\0cb\0\0g", "b\0\0g",      4,  4,    4,       0),
    TEST ("e\0cb\0\0b", "b\0\0g",      4,  1,    6,       0),
    TEST ("\0b\0\0gb\0","bg\0",        2,  2,    4,       0),
    TEST ("\0b\0\0gb\0","bg\0",        0,  2,    1,       0),
    TEST ("\0b\0\0gb\0","bg\0",        0,  3,    0,       0),
    TEST ("e\0cb\0\0g", "afe",         0,  2, NPOS,       0),
    TEST ("\0cb\0\0ge", "\0\0ge",      7,  4, NPOS,       0),
    TEST ("\0cb\0\0ge", "\0\0ge",      6,  0, NPOS,       0),
    TEST ("a\0b",       "e\0gbg\0\0",  0,  1, NPOS,       0),    
    TEST ("a\0b",       "eb\0gg\0\0",  1,  2,    2,       0), 
    TEST ("a\0b",       "e\0gg\0\0a",  3,  6, NPOS,       0),
    TEST ("a\0b",       "e\0gg\0\0a",  0,  7,    0,       0),
    TEST ("a\0b",       "e\0gg\0\0a",  0,  4,    1,       0),

    TEST ("e\0a\0",     "e\0a\0\0",    0,  4,    0,       0),
    TEST ("e\0a\0",     "b\0c\0\0",    0,  5,    1,       0),
    TEST ("ee\0a\0",    "b\0c\0\0",    1,  4,    2,       0),
    TEST ("be\0a\0",    "fdbcb\0a",    1,  5, NPOS,       0),

    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  0, 10,    0,       0),
    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  1, 10,    1,       0),
    TEST ("aaaaabaaaa", "cccccccccc",  0, 10, NPOS,       0),
    TEST ("aaaaabaaaa", "cccbcccccc",  1,  4,    5,       0),
    TEST ("aaaabaaaaa", "cccca",       0,  5,    0,       0),
    TEST ("aabaabaaaa", "cbccccccca",  0,  5,    2,       0), 
    TEST ("bbb",        "aaaaaaaaba",  2,  2, NPOS,       0),  
    TEST ("bbb",        "aaaaaaaaaa",  0,  9, NPOS,       0),
    TEST ("bab",        "aaaaaaaaba",  0,  0, NPOS,       0),
    TEST ("bab",        "ccccccccba",  1, 10,    1,       0),

    TEST ("x@4096",     "",            0,  0, NPOS,       0),
    TEST ("x@4096",     "a",           0,  1, NPOS,       0),
    TEST ("x@4096",     "x",           0,  1,    0,       0),
    TEST ("x@4096",     "xxx",        10,  3,   10,       0),
    TEST ("x@4096",     "axx",        10,  1, NPOS,       0),
    TEST ("x@4096",     "xxa",        10,  0, NPOS,       0),
    TEST ("abc",        "x@4096",      2, 10, NPOS,       0),
    TEST ("xxxxxxxxxx", "x@4096",      0, 4096,  0,       0),
    TEST ("xxxxxxxxxx", "x@4096",      2,  4,    2,       0),
    TEST ("x@2048axyx", "y@4096a",    10, 4097, 2048,     0),

    TEST ("x@4096",     "xxx",      4093,  3, 4093,       0),
    TEST ("x@4096",     "xxx",      4094,  0, NPOS,       0),
    TEST ("x@4096",     "xxx",      4095,  1, 4095,       0),

    TEST ("abcdefghij", 0,             0,  9,    0,       0),
    TEST ("abcdefghij", 0,             1,  9,    1,       0),
    TEST ("\0cb\0\0ge", 0,             5,  7,    5,       0),
    TEST ("\0cb\0\0ge", 0,             5,  5, NPOS,       0),
    TEST ("\0cb\0ge\0", 0,             6,  1,    6,       0),
    TEST ("x@4096",     0,             0, 4096,  0,       0),
    TEST ("x@4096",     0,          4096, 4096, NPOS,     0),
    TEST ("x@4096",     0,          4095,  1, 4095,       0),

    TEST ("",           "",            1,  0, NPOS,       0),
    TEST ("abcdefghij", "abc",        10,  3, NPOS,       0),
    TEST ("abcdefghij", "cba",        10,  1, NPOS,       0),

    TEST ("",           "cba",         0, -1, NPOS,       0),
    TEST ("abcdefghij", "cba",         0, -1,    0,       0),
    TEST ("x@4096",     "xxx",         0, -1,    0,       0),
    TEST ("abcdefghij", "ax@4096",     0, -1,    0,       0),

    TEST ("last test", "test",         0,  4,    2,       0)
};

/**************************************************************************/

// used to exercise 
// find_first_of (const basic_string&, size_type)
static const StringTestCase 
cstr_size_test_cases [] = {

#undef TEST
#define TEST(str, arg, off, res)            \
    { __LINE__, off, -1, -1, -1, -1,        \
      str, sizeof str - 1, arg,             \
      sizeof arg - 1, 0, res, 0             \
    }

    //    +-------------------------------------- controlled sequence
    //    |             +------------------------ sequence to be found
    //    |             |              +--------- find_first_of() off argument
    //    |             |              |     +--- expected result 
    //    |             |              |     |                          
    //    |             |              |     |           
    //    V             V              V     V                
    TEST ("ab",         "a",           0,    0),     

    TEST ("",           "",            0, NPOS),    
    TEST ("",           "\0",          0, NPOS),    
    TEST ("",           "a",           0, NPOS),    

    TEST ("\0",         "",            0, NPOS),    
    TEST ("\0",         "\0",          0,    0),   
    TEST ("\0",         "\0",          1, NPOS),    
    TEST ("\0",         "a",           0, NPOS),    

    TEST ("bbcdefghij", "a",           0, NPOS),    
    TEST ("abcdefghij", "a",           0,    0),   
    TEST ("abcdefghij", "a",           2, NPOS),    
    TEST ("abcdefghij", "f",           2,    5),   
    TEST ("abcdefghij", "f",           7, NPOS),    
    TEST ("abcdefghij", "j",           9,    9),  

    TEST ("edfcbbhjig", "cba",         0,    3),    
    TEST ("edfcbahjig", "cba",         4,    4),     
    TEST ("edfcbahjig", "cba",         6, NPOS),    
    TEST ("edfcbahcba", "cba",         9,    9),       
    TEST ("cbacbahjig", "cba",         7, NPOS),    

    TEST ("e\0cb\0\0g", "b\0\0g",      0,    1),    
    TEST ("e\0cb\0\0g", "b\0\0g",      4,    4),    
    TEST ("e\0cb\0\0g", "cbe",         0,    0),     
    TEST ("\0cb\0\0ge", "\0\0ge",      0,    0),  
    TEST ("\0cb\0\0ge", "cb\0",        6, NPOS),     
    TEST ("e\0gbg\0\0", "bg",          1,    2),   
    TEST ("e\0gbg\0\0", "\0bg",        5,    5), 
    TEST ("e\0dg\0\0a", "cba",         0,    6),    
    TEST ("a\0b",       "e\0gbg\0\0",  0,    1),    
    TEST ("a\0b",       "eb\0gg\0\0",  1,    1), 
    TEST ("a\0b",       "e\0gg\0\0b",  2,    2),
    TEST ("a\0b",       "e\0gg\0\0a",  3, NPOS),

    TEST ("bcbedfbjih", "a",           0, NPOS),    
    TEST ("bcaedfajih", "a",           1,    2),     
    TEST ("bcedfaajih", "a",           6,    6),    
    TEST ("bcaaedfaji", "a",           5,    7),    

    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  0,    0),    
    TEST ("aaaaaaaaaa", "aaaaaaaaaa",  4,    4),    
    TEST ("aaaaabaaaa", "cccccccccb",  1,    5),   
    TEST ("aabaabaaaa", "cccccccccb",  0,    2),  
    TEST ("aabaabaaaa", "cccccccccb",  6, NPOS), 
    TEST ("bbb",        "aaaaaaaaba",  2,    2),  
    TEST ("bab",        "aaaaaaaaba",  0,    0),
    TEST ("bbb",        "aaaaaaaaaa",  0, NPOS),     

    TEST ("x@4096",     "",            0, NPOS),    
    TEST ("x@4096",     "a",           0, NPOS),     
    TEST ("x@4096",     "x",           0,    0),     
    TEST ("x@4096",     "xxx",        10,   10),    
    TEST ("x@4096",     "axa",        10,   10),     
    TEST ("abc",        "x@4096",      2, NPOS),    
    TEST ("xxxxxxxxxx", "x@4096",      0,    0), 
    TEST ("x@2048axxx", "y@4096a",    10, 2048),

    TEST ("x@4096",     "xxx",      4093, 4093), 
    TEST ("x@4096",     "xxx",      4096, NPOS),     

    TEST ("abcdefghij", 0,             0,    0),    
    TEST ("abcdefghij", 0,             1,    1),   
    TEST ("\0cb\0\0ge", 0,             5,    5),    
    TEST ("\0cb\0\0ge", 0,             4,    4),  
    TEST ("x@4096",     0,             0,    0),  
    TEST ("x@4096",     0,             7,    7),    

    TEST ("",           "",            1, NPOS),  
    TEST ("abcdefghij", "abc",        10, NPOS),   
    TEST ("abcdefghij", "cba",        10, NPOS), 

    TEST ("last test",  "test",        0,    2)     
};

/**************************************************************************/

// used to exercise 
// find_first_of (value_type)
static const StringTestCase 
val_test_cases [] = {

#undef TEST
#define TEST(str, val, res)             \
    { __LINE__, -1, -1, -1, -1,         \
      val, str, sizeof str - 1,         \
      0, 0, 0, res, 0                   \
    }

    //    +----------------------------- controlled sequence
    //    |              +-------------- character to be found
    //    |              |       +------ expected result  
    //    |              |       |                           
    //    |              |       |                
    //    V              V       V                 
    TEST ("ab",          'a',    0),   

    TEST ("",            'a', NPOS),  
    TEST ("",           '\0', NPOS),   

    TEST ("\0",         '\0',    0),   
    TEST ("\0\0",       '\0',    0),  
    TEST ("\0",          'a', NPOS),   

    TEST ("e\0cb\0\0g", '\0',    1),    
    TEST ("e\0cb\0\0g",  'b',    3),    
    TEST ("e\0cb\0\0g",  'a', NPOS),   
    TEST ("\0cbge\0\0", '\0',    0),   
    TEST ("\0cbge\0\0",  'b',    2),   
    TEST ("\0cbge\0\0",  'a', NPOS),   

    TEST ("x@4096",      'x',    0),  
    TEST ("x@4096",     '\0', NPOS),   
    TEST ("x@4096",      'a', NPOS),
    TEST ("x@2048axxx",  'a', 2048),

    TEST ("last test",   't',    3)    
};

/**************************************************************************/

// used to exercise 
// find_first_of (value_type, size_type)
static const StringTestCase 
val_size_test_cases [] = {

#undef TEST
#define TEST(str, val, off, res)        \
    { __LINE__, off, -1, -1, -1,        \
      val, str, sizeof str - 1,         \
      0, 0, 0, res, 0                   \
    }

    //    +------------------------------ controlled sequence
    //    |              +--------------- character to be found
    //    |              |     +--------- find_first_of() off argument
    //    |              |     |     +--- expected result  
    //    |              |     |     |                              
    //    |              |     |     |               
    //    V              V     V     V                   
    TEST ("ab",          'a',  0,    0),    

    TEST ("",            'a',  0, NPOS),   
    TEST ("",           '\0',  0, NPOS),    

    TEST ("\0",         '\0',  1, NPOS),    
    TEST ("\0",          'a',  0, NPOS),   
    TEST ("\0\0",       '\0',  1,    1),    
    TEST ("\0\0",        'a',  3, NPOS),   
    TEST ("\0\0",       '\0',  3, NPOS),    

    TEST ("e\0cb\0\0g", '\0',  1,    1),    
    TEST ("e\0cb\0\0g", '\0',  2,    4),    
    TEST ("e\0cb\0\0g", '\0',  6, NPOS),    
    TEST ("e\0cb\0\0g",  'b',  1,    3),    
    TEST ("e\0cb\0\0g",  'b',  4, NPOS),    
    TEST ("e\0cb\0\0g",  'a',  0, NPOS),   
    TEST ("\0cbge\0\0", '\0',  0,    0),   
    TEST ("\0cbge\0\0", '\0',  1,    5),  
    TEST ("\0cbge\0\0", '\0',  9, NPOS),    
    TEST ("\0cbge\0\0",  'b',  0,    2),  
    TEST ("\0bgeb\0\0",  'b',  2,    4),   
    TEST ("\0cbge\0\0",  'a',  1, NPOS),    

    TEST ("x@4096",      'x',  0,    0),   
    TEST ("x@4096",      'x',  5,    5),   
    TEST ("x@4096",     '\0',  0, NPOS),    
    TEST ("x@4096",      'a',  3, NPOS),     
    TEST ("x@4096",      'x', 4096, NPOS),
    TEST ("x@4096",      'x', 4095, 4095),
    TEST ("x@2048axxx",  'a', 10, 2048),

    TEST ("last test",   't',  0,    3)      
};

/**************************************************************************/

template <class charT, class Traits, class Allocator>
void test_find_first_of (charT, Traits*, Allocator*,  
                         const StringFunc    &func,
                         const StringTestCase &tcase)
{
    typedef std::basic_string <charT, Traits, Allocator> String;

    static const std::size_t BUFSIZE = 256;

    static charT wstr_buf [BUFSIZE];
    static charT warg_buf [BUFSIZE];

    std::size_t str_len = sizeof wstr_buf / sizeof *wstr_buf;
    std::size_t arg_len = sizeof warg_buf / sizeof *warg_buf;

    charT* wstr = rw_expand (wstr_buf, tcase.str, tcase.str_len, &str_len);
    charT* warg = rw_expand (warg_buf, tcase.arg, tcase.arg_len, &arg_len);

    // construct the string object and the argument string
    const String  s_str (wstr, str_len);
    const String  s_arg (warg, arg_len);

    if (wstr != wstr_buf)
        delete[] wstr;

    if (warg != warg_buf)
        delete[] warg;

    wstr = 0;
    warg = 0;

    // save the state of the string object before the call
    // to detect wxception safety violations (changes to
    // the state of the object after an exception)
    const StringState str_state (rw_get_string_state (s_str));

    const charT* const arg_ptr = tcase.arg ? s_arg.c_str () : s_str.c_str ();
    const String&      arg_str = tcase.arg ? s_arg : s_str;
    const charT        arg_val = make_char (char (tcase.val), (charT*)0);

    std::size_t size = tcase.size >= 0 ? tcase.size : s_arg.max_size () + 1;

#ifndef _RWSTD_NO_EXCEPTIONS

    // is some exception expected ?
    const char* expected = 0;
    if (1 == tcase.bthrow)
        expected = exceptions [2];

    const char* caught = 0;

#else   // if defined (_RWSTD_NO_EXCEPTIONS)

    if (tcase.bthrow)
        return;

#endif   // _RWSTD_NO_EXCEPTIONS

    try {
        std::size_t res = 0;

        switch (func.which_) {

        case FindFirstOf (cptr):
            res = s_str.find_first_of (arg_ptr);
            break;

        case FindFirstOf (cstr):
            res = s_str.find_first_of (arg_str);
            break;

        case FindFirstOf (cptr_size):
            res = s_str.find_first_of (arg_ptr, tcase.off);
            break;

        case FindFirstOf (cptr_size_size):
            res = s_str.find_first_of (arg_ptr, tcase.off, size);
            break;

        case FindFirstOf (cstr_size):
            res = s_str.find_first_of (arg_str, tcase.off);
            break;

        case FindFirstOf (val):
            res = s_str.find_first_of (arg_val);
            break;

        case FindFirstOf (val_size):
            res = s_str.find_first_of (arg_val, tcase.off);
            break;

        default:
            RW_ASSERT ("test logic error: unknown find_first_of overload");
            return;
        }

        const std::size_t exp_res =
            NPOS != tcase.nres ? tcase.nres : String::npos;

        // verify the returned value
        rw_assert (exp_res == res, 0, tcase.line,
                   "line %d. %{$FUNCALL} == %{?}%zu%{;}%{?}npos%{;}, "
                   "got %{?}%zu%{;}%{?}npos%{;}", 
                   __LINE__, NPOS != tcase.nres, exp_res, NPOS == tcase.nres, 
                   String::npos != res, res, String::npos == res);
    }

#ifndef _RWSTD_NO_EXCEPTIONS

    catch (const std::length_error &ex) {
        caught = exceptions [2];
        rw_assert (caught == expected, 0, tcase.line,
                   "line %d. %{$FUNCALL} %{?}expected %s,%{:}"
                   "unexpectedly%{;} caught std::%s(%#s)",
                   __LINE__, 0 != expected, expected, caught, ex.what ());
    }
    catch (const std::exception &ex) {
        caught = exceptions [4];
        rw_assert (0, 0, tcase.line,
                   "line %d. %{$FUNCALL} %{?}expected %s,%{:}"
                   "unexpectedly%{;} caught std::%s(%#s)",
                   __LINE__, 0 != expected, expected, caught, ex.what ());
    }
    catch (...) {
        caught = exceptions [0];
        rw_assert (0, 0, tcase.line,
                   "line %d. %{$FUNCALL} %{?}expected %s,%{:}"
                   "unexpectedly%{;} caught %s",
                   __LINE__, 0 != expected, expected, caught);
    }

#endif   // _RWSTD_NO_EXCEPTIONS

    if (caught) {
        // verify that an exception thrown during allocation
        // didn't cause a change in the state of the object
        str_state.assert_equal (rw_get_string_state (s_str),
                                __LINE__, tcase.line, caught);
    }
    else if (-1 != tcase.bthrow) {
        rw_assert (caught == expected, 0, tcase.line,
                   "line %d. %{$FUNCALL} %{?}expected %s, caught %s"
                   "%{:}unexpectedly caught %s%{;}",
                   __LINE__, 0 != expected, expected, caught, caught);
    }
}

/**************************************************************************/

DEFINE_STRING_TEST_DISPATCH (test_find_first_of);

int main (int argc, char** argv)
{
    static const StringTest
    tests [] = {

#undef TEST
#define TEST(sig) {                                             \
        FindFirstOf (sig), sig ## _test_cases,                  \
        sizeof sig ## _test_cases / sizeof *sig ## _test_cases  \
    }

        TEST (cptr),
        TEST (cstr),
        TEST (cptr_size),
        TEST (cptr_size_size),
        TEST (cstr_size),
        TEST (val),
        TEST (val_size)
    };

    const std::size_t test_count = sizeof tests / sizeof *tests;

    return rw_run_string_test (argc, argv, __FILE__,
                               "lib.string.find.first.of",
                               test_find_first_of, tests, test_count);
}
