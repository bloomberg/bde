// bdlf_bind_test2.t.cpp                                              -*-C++-*-

#include <bdlf_bind_test2.h>

// Count
#define BBT_n 2

// S with parameter count appended
#define BBT_C(S) S##2

// Repeat comma-separated S once per number of parameters with number appended
#define BBT_N(S) S##1,S##2

// Repeat comma-separated S once per number of parameters
#define BBT_R(S) S,   S

#include <bdlf_bind_testn.t.cpp>
