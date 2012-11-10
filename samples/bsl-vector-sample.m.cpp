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
