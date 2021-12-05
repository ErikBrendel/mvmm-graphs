#pragma once

#include <random>
#include <string>
#include <vector>
#include <sstream>
#include <cstring>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <utility>
#include <iostream>
#include <algorithm>

// #define endl "\n" // broken, sadly
#define all(x) std::begin(x), std::end(x)
#define rep(i, n) for (int i = 0; i < (n); ++i)
#define rep_all_pairs(i, j, n) for (int i = 0; i < (n) - 1; i++) for(int j = i + 1; j < (n); j++)
#define all_pairs_count(n) ((((n) - 1) * (n)) / 2)
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

template <typename T>
T square(T val) {
    return val * val;
}


// from https://stackoverflow.com/q/2590677/4354423
template <class T>
inline void hash_combine(std::size_t& seed, const T& v) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}


namespace std {
// from https://stackoverflow.com/q/37007307/4354423
template<typename T>
struct hash<vector<T>> {
    typedef vector<T> argument_type;
    typedef std::size_t result_type;

    result_type operator()(argument_type const& in) const {
        size_t size = in.size();
        size_t seed = 0;
        for (size_t i = 0; i < size; i++)
            //Combine the hash of the current vector with the hashes of the previous ones
            hash_combine(seed, in[i]);
        return seed;
    }
};

// from https://stackoverflow.com/a/32685618/4354423
template<class T1, class T2>
struct hash<std::pair<T1, T2>> {
    typedef std::pair<T1, T2> argument_type;
    typedef std::size_t result_type;

    result_type operator()(argument_type const& in) const {
        auto h1 = std::hash<T1>{}(in.first);
        auto h2 = std::hash<T2>{}(in.second);
        size_t seed = 0;
        hash_combine(seed, h1);
        hash_combine(seed, h2);
        return seed;
    }
};

// from https://stackoverflow.com/a/7115547/4354423
template <typename ... TT>
struct hash<std::tuple<TT...>> {
    template<class Tuple, size_t Index = std::tuple_size<Tuple>::value - 1>
    struct HashValueImpl {
        static void apply(size_t& seed, Tuple const& tuple) {
            HashValueImpl<Tuple, Index - 1>::apply(seed, tuple);
            hash_combine(seed, std::get<Index>(tuple));
        }
    };

    template<class Tuple>
    struct HashValueImpl<Tuple, 0> {
        static void apply(size_t& seed, Tuple const& tuple) {
            hash_combine(seed, std::get<0>(tuple));
        }
    };

    size_t operator()(std::tuple<TT...> const& tt) const {
        size_t seed = 0;
        HashValueImpl<std::tuple<TT...>>::apply(seed, tt);
        return seed;
    }
};
}


bool startsWith(const std::string& bigString, const std::string& potentialStart);
#define stringContains(a, b) ((a).find((b), 0) != std::string::npos)


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

template <typename T, typename I>
void removeIndices(std::vector<T>& listToModify, std::vector<I> indicesList) {
    // removes all the elements in the list_to_modify, and fills those empty places up
    // with not-to-removable elements from the back of the list, super-fast.
    // assumes that the second parameter is way smaller than the first one
    // inspired by: https://stackoverflow.com/a/8313120/4354423
    sort(all(indicesList));
    std::unordered_set<I> indicesSet;
    indicesSet.reserve(indicesList.size());
    for (const auto& i: indicesList) {
        indicesSet.insert(i);
    }
    int backMovePointer = (int)listToModify.size() - 1;
    while (indicesSet.find(backMovePointer) != indicesSet.end()) {
        backMovePointer--;
    }
    for (const auto& indexToRemove: indicesList) {
        if (indexToRemove >= backMovePointer) {
            break;
        }
        listToModify[indexToRemove] = listToModify[backMovePointer];
        backMovePointer--;
        while (indicesSet.find(backMovePointer) != indicesSet.end()) {
            backMovePointer--;
        }
    }
    int resultLength = listToModify.size() - indicesList.size();
    listToModify.erase(listToModify.begin() + resultLength, listToModify.end()); // del list_to_modify[result_length:]
}

template <typename F>
std::vector<std::vector<F>> generateOneDistributions(int dim, int precision = 10) {
    // precision = how many steps between 0 and 1
    F decrementStep = -1.0 / precision;
    std::vector<std::vector<F>> allResults;
    std::vector<F> result(dim, 0);

    std::function<void(int, F)> fillFirst = [&](int digits, F totalValue) {
        if (digits == 1) {
            result[0] = totalValue;
            allResults.push_back(result);
        } else if (totalValue < 0.00001) {
            rep(i, digits) {
                result[i] = 0;
            }
            allResults.push_back(result);
        } else {
            for (F val = totalValue; val >= -0.000001; val += decrementStep) {
                result[digits - 1] = val < 0 ? 0 : val;
                fillFirst(digits - 1, totalValue - val);
            }
        }
    };

    fillFirst(dim, 1);

    return allResults;
}
