// bdlf_bind_test1.t.cpp                                              -*-C++-*-

#include <bdlf_bind_test1.h>

// Count
#define BBT_n 1

// S with parameter count appended
#define BBT_C(S) S##1

// Repeat comma-separated S once per number of parameters with number appended
#define BBT_N(S) S##1

// Repeat comma-separated S once per number of parameters
#define BBT_R(S) S

#include <bdlf_bind_testn.t.cpp>
