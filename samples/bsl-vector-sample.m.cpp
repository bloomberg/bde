#include <bsl_vector.h>
#include <bsl_iostream.h>

int main(int, char *[])
{
	bsl::vector<int> v;

	v.push_back(3);
	v.push_back(2);
	v.push_back(5);

	for (bsl::vector<int>::const_iterator iter = v.begin();
	     iter != v.end();
	     ++iter) {
		bsl::cout << *iter << bsl::endl;
	}

	return 0;
}

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
