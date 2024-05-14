#pragma once
#include <vector>
#include <memory>
#include "splay_tree.hpp"
namespace cs251 {

template <typename K, typename V>
class adaptive_hash_map {
public:
	// Return a constant reference to the hash table vector
	const std::vector<splay_tree<K,V>>& get_data() const;

	// Default constructor - construct a hash table with a capacity of 1
	adaptive_hash_map();
	// Constructor - create a hash table with a capacity of bucketCount
	adaptive_hash_map(size_t bucketCount);

	// Get the hash code for a given key
	size_t hash_code(K key) const;

	// Insert the key/value pair into the table, if the key doesn't already exist
	// Throw duplicate_key if the key already exists
	void insert(const K& key, std::unique_ptr<V> value);
	// Return a const reference to the value associated with the given key
	// Throw nonexistent_key if the key is not in the hash table
	const std::unique_ptr<V>& peek(const K& key);
	// Remove and return the key-value pair associated with the given key
	// Throw nonexistent_key if the key is not in the hash table
	std::unique_ptr<V> extract(const K& key);

	// Return the current number of elements in the hash table
	size_t size() const;
	// Return the capacity of the hash table
	size_t bucket_count() const;
	// Return whether the hash table is currently empty
	bool empty() const;

private:
	// The hash table array of splay trees
	std::vector<splay_tree<K,V>> m_data {};
    // Bucket count for the adaptive hash table
    size_t m_bucket_count = 0;
    // Size of the adaptive hash table
    size_t m_size = 0;
};

template <typename K, typename V>
const std::vector<splay_tree<K,V>>& adaptive_hash_map<K,V>::get_data() const {
	return m_data;
}

template <typename K, typename V>
adaptive_hash_map<K,V>::adaptive_hash_map() {
    std::vector<splay_tree<K,V>> newTable(1);
    m_data = std::move(newTable);
    m_size = 0;
    m_bucket_count = 1;
}

template <typename K, typename V>
adaptive_hash_map<K,V>::adaptive_hash_map(const size_t bucketCount) {
    std::vector<splay_tree<K,V>> newTable(bucketCount);
    m_data = std::move(newTable);
    m_size = 0;
    m_bucket_count = bucketCount;
}

template <typename K, typename V>
size_t adaptive_hash_map<K,V>::hash_code(K key) const {
	return key % m_bucket_count;
}

template <typename K, typename V>void adaptive_hash_map<K,V>::insert(const K& key, std::unique_ptr<V> value) {
    size_t code = hash_code(key);
    m_data[code].insert(key, std::move(value));
    m_size++;
}

template <typename K, typename V>
const std::unique_ptr<V>& adaptive_hash_map<K,V>::peek(const K& key) {
    size_t code = hash_code(key);
    return m_data[code].peek(key);
}

template <typename K, typename V>
std::unique_ptr<V> adaptive_hash_map<K,V>::extract(const K& key) {
    size_t code = hash_code(key);
    std::unique_ptr<V> value = m_data[code].extract(key);
    m_size--;
    return value;
}

template <typename K, typename V>
size_t adaptive_hash_map<K,V>::size() const {
    return m_size;
}

template <typename K, typename V>
size_t adaptive_hash_map<K,V>::bucket_count() const {
    return m_bucket_count;
}

template <typename K, typename V>
bool adaptive_hash_map<K,V>::empty() const {
    if (m_size == 0) {
        return true;    
    }
    return false;
}

}
