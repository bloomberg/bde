// bdlsta_linefit.cpp                                                 -*-C++-*-
#include <bdlsta_linefit.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

namespace BloombergLP {
namespace bdlsta {
// BDE_VERIFY pragma: -LL01 // Link is just too long
//@IMPLEMENTATION NOTES:
//
// The formulae for linear square line fit are developed from:
// https://en.wikipedia.org/wiki/Simple_linear_regression#Fitting_the_regression_line
//
// The line desired is Y = A + B*X, where A, B are the scalar parameters we
// calculate.
// A is simply deduced (once we have B) from the requirement to pass through
// the center or mass:
//..
// Xm = mean(X), Ym = mean(Y) ==> A = Ym - B*Xm
//..
// The formula for B is:
//..
// B = SUM((Xi-Xm)*(Yi-Ym)) / SUM((Xi-Xm)^2)
//..
// The denominator SUM((Xi-Xm)^2) is the 2nd moment multiplied by the count,
// which is used in the variance calculation.
//
// The numerator:
//..
// SUM((Xi-Xm)*(Yi-Ym))
// = SUM(Xi*Yi + Xm*Ym - Yi*Xm - Xi*Ym)
// = SUM(Xi*Yi) + N*Xm*Ym - Xm*SUM(Yi) - Ym*SUM(Yi)
// = SUM(Xi*Yi) + N*Xm*Ym - N*Xm*Ym - N*Ym*Xm
// = SUM(Xi*Yi) - N*Xm*Ym
//..
//
// Alternative derivation does not avoid multiplication of X and Y:
//
// Let's look for the difference between n-1 and n for the numerator (as would
// be expected for sample moments):
//..
// SUM((Xi-Xm)*(Yi-Ym)), designated as DS = SUMn - SUMn1
//..
// Let's mark Xm as mean(X) with n values, Xm1 as mean(X) with n-1 value1, Ym
// as mean(Y) with n values, Ym1 as mean(Y) with n-1 values.  Let's also mark
// the newest member in the same as Sn.
//
// We can see that DS = Sn + difference in the sum of the first n-1 members
// due to change of Xm1 to Xm, Ym1 to Ym.
//..
// Sn is trivial: (Xn - Xm) * (Yn = Ym)
//..
// The difference in the first n-1 members:
//..
// SUM((Xi-Xm)*(Yi-Ym)) - SUM((Xi-Xm1)*((Yi-Ym1))
// = SUM(Xi*Yi + Xm*Ym - Xi*Ym - Yi*Xm - Xi*Yi - Xm1*Ym1 + Xi*Ym1 + Yi * Ym1)
// = SUM(Xm*Ym - Xm1*Ym1) - SUM(Xi*(Ym - Ym1)) - SUM(Yi*(Xm - Xm1))
// = n*(Xm*Ym - Xm1*Ym1) - (Ym - Ym1)*SUM(Xi) - (Xm - Xm1)*SUM(Yi)
// BDE_VERIFY pragma: +LL01
//..
//
}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
