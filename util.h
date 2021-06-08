#pragma once

#include<random>
#include <string>
#include <vector>
#include <sstream>
#include <cstring>
#include <string>

#define all(x) std::begin(x), std::end(x)
#define rep(i, n) for (unsigned int i = 0; i < (n); ++i)
#define rep_all_pairs(i, j, n) for (unsigned int i = 0; i < (n) - 1; i++) for(unsigned int j = i + 1; j < (n); j++)


//#define randRange(upperBound) (rand() % (upperBound))
//#define randRange(upperBound) std::uniform_int_distribution<int>(0, upperBound)(my_randomEngine)

double square(double val);


#define startsWith(a, b) ((a).rfind((b), 0) == 0)


// string split and join functions from here:
// https://github.com/ekg/split


// join a vector of elements by a delimiter object.  ostream<< must be defined
// for both class S and T and an ostream, as it is e.g. in the case of strings
// and character arrays
template<class S, class T>
std::string join(std::vector<T>& elems, S& delim) {
    std::stringstream ss;
    typename std::vector<T>::iterator e = elems.begin();
    ss << *e++;
    for (; e != elems.end(); ++e) {
        ss << delim << *e;
    }
    return ss.str();
}
// thanks to Evan Teran, http://stackoverflow.com/questions/236129/how-to-split-a-string/236803#236803

// split a string on a single delimiter character (delim)
std::vector<std::string>& split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string>  split(const std::string &s, char delim);

// split a string on any character found in the string of delimiters (delims)
std::vector<std::string>& split(const std::string &s, const std::string& delims, std::vector<std::string> &elems);
std::vector<std::string>  split(const std::string &s, const std::string& delims);
