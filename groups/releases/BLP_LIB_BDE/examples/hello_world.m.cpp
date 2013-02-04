#include <bdet_string.h>
#include <bdescm_version.h>
#include <iostream>

using namespace BloombergLP;
using namespace std;

int main()
{
  bdet_String text = "Hello world from ";
  text += bdescm_Version::version();
  text += '!';

  cout << text << endl;
  return 0;
}
