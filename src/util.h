#pragma once

#include <random>
#include <string>
#include <vector>
#include <sstream>
#include <cstring>
#include <string>
#include <unordered_map>
#include <functional>
#include <utility>
#include <iostream>

#define all(x) std::begin(x), std::end(x)
#define rep(i, n) for (int i = 0; i < (n); ++i)
#define rep_all_pairs(i, j, n) for (int i = 0; i < (n) - 1; i++) for(int j = i + 1; j < (n); j++)
#define ensure(condition, message) if (!(condition)) throw runtime_error(message);


template<typename s, typename v>
s& operator<<(s& str, std::vector<v> vec) {
    str << "[";
    rep(i, vec.size()) {
        if (i != 0) str << ", ";
        str << vec[i];
    }
    str << "]";
    return str;
}

template <typename T>
void printAbbrev(const std::vector<T>& data) {
    if (data.empty()) {
        std::cout << "Empty List";
    } else if (data.size() <= 10) {
        std::cout << data;
    } else {
        std::cout << "[";
        for (int i = 0; i < 5; ++i) {
            std::cout << data[i];
            std::cout << ", ";
        }
        std::cout << "...";
        for (int i = data.size() - 5; i < data.size(); ++i) {
            std::cout << ", ";
            std::cout << data[i];
        }
        std::cout << "]";
    }
}

template <typename T>
double mean(const std::vector<T>& elements) {
    return accumulate(all(elements), 0.0) / elements.size();
}

//#define randRange(upperBound) (rand() % (upperBound))
//#define randRange(upperBound) std::uniform_int_distribution<int>(0, upperBound)(my_randomEngine)

double square(double val);


// from https://stackoverflow.com/q/2590677/4354423
template <class T>
inline void hash_combine(std::size_t& seed, const T& v) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}

// from https://stackoverflow.com/a/32685618/4354423
struct pair_hash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1,T2> &p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);

        // Mainly for demonstration purposes, i.e. works but is overly simple
        // In the real world, use sth. like boost.hash_combine
        size_t seed = 0;
        hash_combine(seed, h1);
        hash_combine(seed, h2);
        return seed;
    }
};


#define startsWith(a, b) ((a).rfind((b), 0) == 0)
#define contains(a, b) ((a).find((b), 0) != std::string::npos)


// string split and join functions from here:
// https://github.com/ekg/split


// join a vector of elements by a delimiter object.  ostream<< must be defined
// for both class S and T and an ostream, as it is e.g. in the case of strings
// and character arrays
// the optional lambda is applied to each element before outputting it.
template<class S, class T, typename E2S>
std::string join(std::vector<T>& elems, S& delim, E2S&& e2s) {
    if (elems.empty()) return "";
    std::stringstream ss;
    typename std::vector<T>::iterator e = elems.begin();
    ss << e2s(*e++);
    for (; e != elems.end(); ++e) {
        ss << delim << e2s(*e);
    }
    return ss.str();
}
template<class S, class T>
std::string join(std::vector<T>& elems, S& delim) {
    return join(elems, delim, [](const T& t) { return t; });
}
// thanks to Evan Teran, http://stackoverflow.com/questions/236129/how-to-split-a-string/236803#236803

// split a string on a single delimiter character (delim)
std::vector<std::string>& split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string>  split(const std::string &s, char delim);

// split a string on any character found in the string of delimiters (delims)
std::vector<std::string>& split(const std::string &s, const std::string& delims, std::vector<std::string> &elems);
std::vector<std::string>  split(const std::string &s, const std::string& delims);