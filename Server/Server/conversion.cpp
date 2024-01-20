#include "conversion.h"


std::string CharToString(const char* input) { 
    std::string s;
    s.assign(input, strlen(input));

    return s;
}

int CharToInt(char* input) {
    int rv = 0;
    sscanf_s(input, "%d", &rv);

    return rv;
}
