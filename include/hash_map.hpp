#pragma once
#include <sstream>
#include <exception>
#include <vector>
#include <memory>
namespace cs251 {

// Custom exception classes
class duplicate_key : public std::runtime_error {
	public: duplicate_key() : std::runtime_error("Duplicate key!") {} };
class nonexistent_key : public std::runtime_error {
	public: nonexistent_key() : std::runtime_error("Key does not exist!") {} };

template <typename K, typename V>
class hash_map {
public:
	class hash_map_node {
	public:
		// The key of current node.
		K m_key = {};
		// The value of current node.
		std::unique_ptr<V> m_value{};
	};

	// Return a constant reference to the hash table vector
	const std::vector<std::shared_ptr<hash_map_node>>& get_data() const;

	// Default constructor - create a hash map with an initial capacity of 1
	hash_map();
	// Constructor - create a hash map with an intial capacity of bucketCount
	hash_map(size_t bucketCount);

	// Get the hash code for a given key
	size_t hash_code(K key) const;

	// Change the size of the table to bucketCount, re-hashing all existing elements
	// bucketCount will never be 0 or less than the current number of elements
	void resize(size_t bucketCount);

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
	// Return the current capacity of the hash table
	size_t bucket_count() const;
	// Return whether the hash table is currently empty
	bool empty() const;

private:
	// The array that holds key-value pairs
	std::vector<std::shared_ptr<hash_map_node>> m_data = {};
	// The bucket count of the array
    size_t m_bucket_count = 0;
    // The size of the array
    size_t m_size = 0;
};

template <typename K, typename V>
const std::vector<std::shared_ptr<typename hash_map<K,V>::hash_map_node>>& hash_map<K,V>::get_data() const {
	return m_data;
}

template <typename K, typename V>hash_map<K,V>::hash_map() {
    std::vector<std::shared_ptr<hash_map_node>> newTable(1);
    m_data = std::move(newTable);
	m_data[0] = nullptr;
    m_size = 0;
    m_bucket_count = 1;
}

template <typename K, typename V>
hash_map<K,V>::hash_map(const size_t bucketCount) {
    std::vector<std::shared_ptr<hash_map_node>> newTable(bucketCount);
    m_data = std::move(newTable);
	for (int i = 0; i < bucketCount; i++) {
        m_data[i] = nullptr;    
    }
    m_size = 0;
    m_bucket_count = bucketCount;
}

template <typename K, typename V>
size_t hash_map<K,V>::hash_code(K key) const {
	return key % m_bucket_count;
}

template <typename K, typename V>
void hash_map<K,V>::resize(const size_t bucketCount) {
	if (bucketCount < m_size) {
        return;    
    }
    size_t oldBucketCount = m_bucket_count;
    m_bucket_count = bucketCount;
    std::vector<std::shared_ptr<hash_map_node>> newTable(bucketCount);
    for (int i = 0; i < oldBucketCount; i++) {
        if (m_data[i] != nullptr) {
            size_t index = hash_code(m_data[i]->m_key);
            while (newTable[index] != nullptr) {
                index = (index + 1) % m_bucket_count;    
            }
            newTable[index] = m_data[i];
        }
    }
    m_data = std::move(newTable);
}

template <typename K, typename V>
void hash_map<K,V>::insert(const K& key, std::unique_ptr<V> value) {
    for (int i = 0; i < m_bucket_count; i++) {
        if (m_data[i] != nullptr) {
            if (m_data[i]->m_key == key) {
                throw duplicate_key();    
            }
        }
    }
	size_t startIndex = hash_code(key);
    size_t index = startIndex;
    while (m_data[index] != nullptr) {
        index = (index + 1) % m_bucket_count;
        if (index == startIndex) {
            resize(2 * m_bucket_count);
            startIndex = hash_code(key);
            index = startIndex;
        }
    }
    m_data[index] = std::make_shared<hash_map_node>();
    m_data[index]->m_key = key;
    m_data[index]->m_value = std::move(value);
    m_size++;
}

template <typename K, typename V>
const std::unique_ptr<V>& hash_map<K,V>::peek(const K& key) {
	for (int i = 0; i < m_bucket_count; i++) {
        if (m_data[i] != nullptr) {
            if (m_data[i]->m_key == key) {
                return m_data[i]->m_value;    
            }
        }
    }
    throw nonexistent_key();
}

template <typename K, typename V>
std::unique_ptr<V> hash_map<K,V>::extract(const K& key) {
	int index = -1;
    for (int i = 0; i < m_bucket_count; i++) {
        if (m_data[i] != nullptr) {
            if (m_data[i]->m_key == key) {
                index = i;    
            }
        }
    }
    if (index == -1) {
        throw nonexistent_key();    
    }
    std::unique_ptr<V> value = std::move(m_data[index]->m_value);
    m_data[index] = nullptr;
    m_size--;
    return value;
}

template <typename K, typename V>
size_t hash_map<K,V>::size() const {
	return m_size;
}

template <typename K, typename V>
size_t hash_map<K,V>::bucket_count() const {
	return m_bucket_count;
}

template <typename K, typename V>
bool hash_map<K,V>::empty() const {
	if (m_size == 0) {
        return true;    
    }
    return false;
}

}
