#include <deque>
#include <vector>
#include <functional>

using namespace std;

#define RADIX_WIDTH 8
#define RADIX_SIZE 256
#define RADIX_MASK 255

template<typename object_type>
vector<deque<object_type>> create_radix() {
    return vector<deque<object_type>>(RADIX_SIZE);
}

template <
        class iterator,
        typename object_type = typename iterator_traits<iterator>::value_type>
void radix_insert(iterator begin, iterator end, vector<deque<object_type>>& b, function<unsigned long(const object_type&)> key) {
    for (auto it = begin; it != end; it++) {
        const object_type e = *it;
        b[key(e) & RADIX_MASK].push_back(move(e));
    }
}

template<typename object_type>
void radix_step(vector<deque<object_type>>& a, vector<deque<object_type>>& b, size_t shift, function<unsigned long(const object_type&)> key) {
    for (size_t i = 0; i < RADIX_SIZE; i++) {
        auto& q = a[i];
        for (auto e : q) {
            b[(key(e) >> shift) & RADIX_MASK].push_back(move(e));
        }
        q.clear();
    }
}

template <
        class iterator,
        typename object_type = typename iterator_traits<iterator>::value_type>
void radix_extract(vector<deque<object_type>>& a, iterator b) {
    for (size_t i = 0; i < RADIX_SIZE; i++) {
        const auto& q = a[i];
        for (auto& e : q) {
            *(b++) = move(e);
        }
    }
}

template <
        class iterator,
        typename object_type = typename iterator_traits<iterator>::value_type>
void radix_sort(iterator begin, iterator end, function<unsigned long(const object_type&)> key) {
    auto qa = create_radix<object_type>();
    auto qb = create_radix<object_type>();
    radix_insert(begin, end, qa, key);
    radix_step(qa, qb, 8, key);
    radix_step(qb, qa, 16, key);
    radix_step(qa, qb, 24, key);
    radix_step(qb, qa, 32, key);
    radix_step(qa, qb, 40, key);
    radix_step(qb, qa, 48, key);
    radix_step(qa, qb, 56, key);

    radix_extract(qb, begin);
}
