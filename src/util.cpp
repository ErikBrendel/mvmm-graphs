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

std::vector<std::string> &split(const std::string &s, const std::string& delims, std::vector<std::string> &elems) {
    char* tok;
    char* cstr = new char[s.size() + 1];
    memcpy(cstr, s.c_str(), s.size() + 1);
    tok = strtok(cstr, delims.c_str());
    while (tok != nullptr) {
        elems.emplace_back(tok);
        tok = strtok(nullptr, delims.c_str());
    }
    delete[] cstr;
    return elems;
}

std::vector<std::string> split(const std::string &s, const std::string& delims) {
    std::vector<std::string> elems;
    return split(s, delims, elems);
}
