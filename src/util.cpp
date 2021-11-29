#include "util.h"

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    return split(s, delim, elems);
}

std::vector<std::string> &split(const std::string &s, const std::string& delimiter, std::vector<std::string> &elems) {
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
