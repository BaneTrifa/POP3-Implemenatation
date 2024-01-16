#include "conversion.h"


std::string char_to_string(const char* input) { 
    std::string s;
    s.assign(input, strlen(input));

    return s;
}

int char_to_int(char* input) {
    int rv = 0;
    sscanf(input, "%d", &rv);

    return rv;
}

std::string int_to_string(int input) {
	// stringstream object
    std::stringstream stream;
    
    // insertion of integer variable to stream
    stream << input;
    
    // variable to hold the new variable from the stream
    std::string input_as_string;
    
    // extraction of string type of the integer variable
    stream >> input_as_string;

	return input_as_string;
}