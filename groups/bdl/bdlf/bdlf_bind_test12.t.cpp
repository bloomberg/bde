// bdlf_bind_test12.t.cpp                                             -*-C++-*-

#include <bdlf_bind_test12.h>

// Count
#define BBT_n 12

// S with parameter count appended
#define BBT_C(S) S##12

// Repeat comma-separated S once per number of parameters with number appended
#define BBT_N(S) S##1,S##2,S##3,S##4,S##5,S##6,S##7,S##8,S##9,S##10,S##11,S##12

// Repeat comma-separated S once per number of parameters
#define BBT_R(S) S,   S,   S,   S,   S,   S,   S,   S,   S,   S,    S,    S

#include <bdlf_bind_testn.t.cpp>
