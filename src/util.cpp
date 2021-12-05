#include "util.h"

bool startsWith(const std::string& bigString, const std::string& potentialStart) {
    if (potentialStart.length() > bigString.length()) {
        return false;
    }
    // beginning at the back for an expected earlier abort
    for (int i = (int)potentialStart.length() - 1; i >= 0; --i) {
        if (potentialStart[i] != bigString[i]) {
            return false;
        }
    }
    return true;
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::string delim_s(1, delim);
    return split(s, delim_s, elems);
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    return split(s, delim, elems);
}

std::vector<std::string> &split(const std::string &s, const std::string& delimiter, std::vector<std::string> &elems) {
    if (s.empty()) {
        return elems; // do not return a single empty element, but instead no splitting elements at all!
    }
    // see https://stackoverflow.com/a/14266139/4354423 that answers first comment
    size_t last = 0;
    size_t next = 0;
    while ((next = s.find(delimiter, last)) != std::string::npos) {
        elems.push_back(s.substr(last, next - last));
        last = next + 1;
    }
    elems.push_back(s.substr(last));
    return elems;
}

std::vector<std::string> split(const std::string &s, const std::string& delimiter) {
    std::vector<std::string> elems;
    return split(s, delimiter, elems);
}
