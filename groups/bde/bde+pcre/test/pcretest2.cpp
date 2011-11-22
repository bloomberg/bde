#include <cstring>

extern int auxiliary_main(int argc, char **argv);

const char *TEST_INPUT_FILE =
                    TEST_LOCATION "/testinput2";

int main(int argc, char **argv)
{
    if (1 == argc) {
        char *localArgv[2];
        localArgv[0] = new char[std::strlen(argv[0]) + 1];
        std::strcpy(localArgv[0], argv[0]);
        localArgv[1] = new char[std::strlen(TEST_INPUT_FILE) + 1];
        std::strcpy(localArgv[1], TEST_INPUT_FILE);
        int ret = auxiliary_main(argc + 1, localArgv);
        delete [] localArgv[0];
        delete [] localArgv[1];
        return ret;
    }
    else {
        return auxiliary_main(argc, argv);
    }
}
