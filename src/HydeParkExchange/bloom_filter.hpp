#ifndef BLOOM_FILTER_HPP
#define BLOOM_FILTER_HPP

#include <vector>
#include <bitset>
#include <concepts>
#include <functional>

// just needs the requires clause to not have an error
template<typename T>
concept Hashable = requires(const T& t) {
    { std::hash<T>{}(t) } -> std::convertible_to<std::size_t>;
};


template<Hashable T>
class BloomFilter {
private:
    std::vector<bool> bits;
    // use a vector of std functions to store those lambdas that we make
    std::vector<std::function<std::size_t(const T&)>> hash_functions;
    std::size_t num_hash_functions;
    
    // This specific choice of changing the hash function is heavily studied
    // You essentially XOR w/ an integer representation of the fractional part of the golden ratio
    // Then do some offsets to shuffle things around a bit
    std::size_t calculateHash(const T& element, std::size_t i) {
        std::size_t hash = std::hash<T>{}(element);
        hash = hash^(i + 0x9e3779b9 + (hash << 6) - (hash >> 2));
        return hash;
    }

public:
    BloomFilter(std::size_t size, std::size_t numHashFunctions) : bits(size), num_hash_functions(numHashFunctions) {
        for (std::size_t i = 0; i < numHashFunctions; i++) {
            hash_functions.push_back(
                [this, i](const T& element)
                        {return calculateHash(element, i); }
                    );
        }
    }
    void insert(const T& element) {
        //set true for all hash functions
        for (const auto& hash_function : hash_functions) {
            std::size_t index = hash_function(element) % bits.size();
            bits[index] = true;
        }
    }

    bool mayContain(const T& element) const {
        for (const auto& hash_function : hash_functions) {
            std::size_t index = hash_function(element) % bits.size();
            if (!bits[index])
                return false;
        }
        return true;
    }

};

#endif  // BLOOM_FILTER_HPP
