//
// Created by ospen_000 on 03.06.2016.
// ospennikovlev@gmail.com

// This is implementation of  bidirectional map based on two unordered_map. The map takes o(n) memory
// All keys and values should be unique and should have hash function.
// Functions roughly equivalent to simple unordered_map.
#ifndef HASH_BIMAP
#define HASH_BIMAP

#include <unordered_map>
#include <assert.h>
#include <set>
#include <iostream>
#include <memory>


template<typename key_type, typename value_type>
struct bimap {
    typedef std::pair<key_type, value_type> pair_type;

    //Subclass iterator is based on iterator key_to_value unordered map
    struct iterator {
        typedef typename std::unordered_map<key_type, std::shared_ptr<pair_type>>::iterator it_type;

        iterator(bimap &m) {
            base_it = m->key_to_value_map.begin();
        }

        iterator(iterator const &other) : base_it(other.base_it) { }

        iterator &operator++() {
            base_it++;
            return (*this);
        }

        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        iterator &operator=(iterator const &it) {

            this->base_it = it.base_it;
            return *this;
        };

        friend bool operator==(iterator const &a, iterator const &b) {
            return (a.base_it == b.base_it);
        };

        friend bool operator!=(iterator const &a, iterator const &b) {
            return !((a.base_it == b.base_it));
        };


        const pair_type &operator*() {
            return (*(*base_it).second);
        };

        friend struct bimap;

        iterator(it_type it) {
            base_it = it;
        };


    private:
        it_type base_it;

    };


    bimap() {
        ssize = 0;
    };

    ~bimap() {
    };

    bimap(bimap &other) {
        ssize = 0;
        for (auto it = other.begin(); it != other.end();) {
            this->insert((*it));
            it++;
        }

    };

    bimap &operator=(bimap &other) {
        if (this == &other) {
            return *this;
        }
        this->clear();
        for (auto it = other.begin(); it != other.end();) {
            this->insert((*it));
            it++;
        }
        return (*this);
    };


    bool empty() const {
        return ssize == 0;
    };

    size_t size() const {
        return ssize;
    };


    std::pair<iterator, bool> insert(pair_type const &pair) {
        pair_type copy = pair;
        std::shared_ptr<pair_type> p_ptr = std::make_shared<pair_type>(copy);
        auto it1 = find_with_value(pair.second);
        if (it1 != this->end()) {
            return std::make_pair(find_with_key(pair.first), false);
        }
        auto insert_pair = key_to_value_map.insert(std::make_pair(pair.first, p_ptr));
        if (!insert_pair.second) {
            return (std::make_pair(iterator(insert_pair.first), false));
        }
        value_to_key_map.insert(std::make_pair(pair.second, p_ptr));
        ssize++;
        return std::make_pair(iterator(insert_pair.first), true);
    }

    std::pair<iterator, bool> insert(key_type const &key, value_type const &value) {
        return insert(std::make_pair(key, value));
    }

    iterator erase_with_key(key_type const &key) {
        auto pair = key_to_value_map.find(key);
        if (pair != key_to_value_map.end()) {
            return erase_with_pair(*pair->second, true, false);
        } else {
            return this->end();
        }
    }

    iterator erase_with_value(key_type const &value) {
        auto pair = value_to_key_map.find(value);
        if (pair != value_to_key_map.end()) {
            return erase_with_pair(*pair->second, false, true);
        } else {
            return this->end();
        }
    }

    iterator erase(iterator it) {
        if (it != this->end()) {
            pair_type pair = (*it);
            return erase_with_pair(pair, true, true);
        } else {
            return this->end();
        }
    }


    iterator find_with_key(key_type const &key) {
        auto pair = key_to_value_map.find(key);
        if (pair != key_to_value_map.end()) {
            return iterator(pair);
        } else {
            return this->end();
        }
    };

    iterator find_with_value(key_type const &value) {
        auto pair = value_to_key_map.find(value);
        if (pair != value_to_key_map.end()) {
            return find_with_key(pair->second->first);
        } else {
            return this->end();
        }
    };

    iterator begin() noexcept {
        return iterator(key_to_value_map.begin());
    };

    iterator end() noexcept {
        return iterator(key_to_value_map.end());
    };

    void clear() {
        key_to_value_map.clear();
        value_to_key_map.clear();
        ssize = 0;
    };


private:

    std::unordered_map<key_type, std::shared_ptr<pair_type> > key_to_value_map;
    std::unordered_map<value_type, std::shared_ptr<pair_type> > value_to_key_map;

    iterator erase_with_pair(pair_type pair, bool have_key, bool have_value) {
        if (!have_key) {
            auto it = find_with_key(pair.first);
            if (it == this->end()) {
                return this->end();
            }
        }
        if (!have_value) {
            auto it = find_with_value(pair.second);
            if (it == this->end()) {
                return this->end();
            }
        }

        ssize--;
        auto iter = key_to_value_map.erase(key_to_value_map.find(pair.first));
        value_to_key_map.erase(pair.second);
        return (iterator(iter));
    }

    size_t ssize;
};

#endif //HASH_BIMAP