// bdlf_bind_test4.t.cpp                                              -*-C++-*-

#include <bdlf_bind_test4.h>

// Count
#define BBT_n 4

// S with parameter count appended
#define BBT_C(S) S##4

// Repeat comma-separated S once per number of parameters with number appended
#define BBT_N(S) S##1,S##2,S##3,S##4

// Repeat comma-separated S once per number of parameters
#define BBT_R(S) S,   S,   S,   S

#include <bdlf_bind_testn.t.cpp>
