#pragma once
#include <sstream>
#include <exception>
#include <memory>
namespace cs251 {

// Custom exception classes
class duplicate_key : public std::runtime_error {
	public: duplicate_key() : std::runtime_error("Duplicate key!") {} };
class nonexistent_key : public std::runtime_error {
	public: nonexistent_key() : std::runtime_error("Key does not exist!") {} };
class empty_tree : public std::runtime_error {
	public: empty_tree() : std::runtime_error("Tree is empty!") {} };

template <typename K, typename V>
class splay_tree {
public:
	struct splay_tree_node {
		// Pointer to the left child
		std::shared_ptr<splay_tree_node> m_left {};
		// Pointer to the right child
		std::shared_ptr<splay_tree_node> m_right {};
		// Weak pointer to the parent
		std::weak_ptr<splay_tree_node> m_parent {};

		// The key of this element
		K m_key {};
		// Pointer to the value of this element
		std::unique_ptr<V> m_value {};
	};

	// Return a pointer to the root of the tree
	std::shared_ptr<splay_tree_node> get_root() const;

	// Default constructor - create an empty splay tree
	splay_tree();
    
    // Splays the input node to the root
    void splay(std::shared_ptr<splay_tree_node> node);
    
    // Helper function for splay, rotates left
    void rotate_left(std::shared_ptr<splay_tree_node> node);
    
    // Helper function for splay, rotates right
    void rotate_right(std::shared_ptr<splay_tree_node> node);

	// Insert the key/value pair into the tree, if the key doesn't already exist
	// Throw duplicate_key if the key already exists
	void insert(const K& key, std::unique_ptr<V> value);
	// Return a const reference to the value associated with the given key
	// Throw nonexistent_key if the key is not in the splay tree
	const std::unique_ptr<V>& peek(const K& key);
	// Remove and return the key-value pair associated with the given key
	// Throw nonexistent_key if the key is not in the splay tree
	std::unique_ptr<V> extract(const K& key);

	// Return the minimum key in the splay tree, and splay the node
	// Throw empty_tree if the tree is empty
	K minimum_key();
	// Return the maximum key in the splay tree, and splay the node
	// Throw empty_tree if the tree is empty
	K maximum_key();

	// Return the current number of elements in the splay tree
	bool empty() const;
	// Return whether the splay tree is currently empty
	size_t size() const;

private:
	// Pointer to the root node of the splay tree
	std::shared_ptr<splay_tree_node> m_root {};
    // Current size of the splay tree
	size_t m_size = 0;
};

template <typename K, typename V>
std::shared_ptr<typename splay_tree<K,V>::splay_tree_node> splay_tree<K,V>::get_root() const {
	return m_root;
}

template <typename K, typename V>
splay_tree<K,V>::splay_tree() {
	m_root = nullptr;
}
    
template <typename K, typename V>
void splay_tree<K,V>::splay(std::shared_ptr<splay_tree_node> node) {
    while (node != m_root) {
        std::shared_ptr<splay_tree_node> parent = node->m_parent.lock();
        std::shared_ptr<splay_tree_node> grandparent = parent->m_parent.lock();
        if (parent == m_root) {
            if (parent->m_left == node) {
                rotate_right(parent);
            } else {
                rotate_left(parent);
            }
        } else {
            if (grandparent->m_left == parent && parent->m_left == node) {
                rotate_right(grandparent);
                rotate_right(parent);
            } else if (grandparent->m_right == parent && parent->m_right == node) {
                rotate_left(grandparent);
                rotate_left(parent);
            } else {
                if (parent->m_right == node) {
                    rotate_left(parent);
                    rotate_right(grandparent);
                } else {
                    rotate_right(parent);
                    rotate_left(grandparent);
                }
            }
        }
    }
    m_root = node;
}

template <typename K, typename V>
void splay_tree<K,V>::rotate_left(std::shared_ptr<splay_tree_node> node) {
    std::shared_ptr<splay_tree_node> parent = node->m_parent.lock();
    std::shared_ptr<splay_tree_node> rightChild = node->m_right;
    if (node != m_root) {
        rightChild->m_parent = parent;
        if (rightChild->m_key < parent->m_key) {
            parent->m_left = rightChild;    
        } else {
            parent->m_right = rightChild;    
        }
    } else {
        rightChild->m_parent.reset();
        m_root = rightChild;
    }
    if (rightChild->m_left != nullptr) {
        node->m_right = rightChild->m_left;
        rightChild->m_left->m_parent = node;
    } else {
        node->m_right = nullptr;    
    }
    rightChild->m_left = node;
    node->m_parent = rightChild;
}

template <typename K, typename V>
void splay_tree<K,V>::rotate_right(std::shared_ptr<splay_tree_node> node) {
    std::shared_ptr<splay_tree_node> parent = node->m_parent.lock();
    std::shared_ptr<splay_tree_node> leftChild = node->m_left;
    if (node != m_root) {
        leftChild->m_parent = parent;
        if (leftChild->m_key < parent->m_key) {
            parent->m_left = leftChild;    
        } else {
            parent->m_right = leftChild;    
        }
    } else {
        leftChild->m_parent.reset();
        m_root = leftChild;
    }
    if (leftChild->m_right != nullptr) {
        node->m_left = leftChild->m_right;
        leftChild->m_right->m_parent = node;
    } else {
        node->m_left = nullptr;    
    }
    leftChild->m_right = node;
    node->m_parent = leftChild;
}

template <typename K, typename V>
void splay_tree<K,V>::insert(const K& key, std::unique_ptr<V> value) {
    if (m_root == nullptr) {
        m_root = std::make_shared<splay_tree_node>();
        m_root->m_key = key;
        m_root->m_value = std::move(value);
        m_root->m_parent.reset();
        m_size++;
        return;
    } else {
        std::shared_ptr<splay_tree_node> current = m_root;
        std::shared_ptr<splay_tree_node> parent = current;
        while (current != nullptr) {
            parent = current;
            if (key < current->m_key) {
                current = current->m_left;    
            } else if (key > current->m_key) {
                current = current->m_right;    
            } else {
                throw duplicate_key();    
            }  
        }
        std::shared_ptr<splay_tree_node> newNode = std::make_shared<splay_tree_node>();
        newNode->m_key = key;
        newNode->m_value = std::move(value);
        newNode->m_parent = parent;
        if (key < parent->m_key) {
            parent->m_left = newNode;    
        } else {
            parent->m_right = newNode;
        }
        m_size++;
        splay(newNode);
    }
}

template <typename K, typename V>
const std::unique_ptr<V>& splay_tree<K,V>::peek(const K& key) {
    if (m_root == nullptr) {
        throw nonexistent_key();
    } else {
        std::shared_ptr<splay_tree_node> current = m_root;
        while (current != nullptr) {
            if (key < current->m_key) {
                current = current->m_left;    
            } else if (key > current->m_key) {
                current = current->m_right;    
            } else {
                if (current != m_root) {
                    splay(current);
                }
                return current->m_value;
            }  
        }
        throw nonexistent_key();
    }
}

template <typename K, typename V>
std::unique_ptr<V> splay_tree<K,V>::extract(const K& key) {
    if (m_root == nullptr) {
        throw nonexistent_key();
    } else {
        std::shared_ptr<splay_tree_node> current = m_root;
        while (current != nullptr) {
            if (key < current->m_key) {
                current = current->m_left;    
            } else if (key > current->m_key) {
                current = current->m_right;    
            } else {
                if (current == m_root && current->m_left == nullptr && current->m_right == nullptr) {
                    m_root = nullptr;
                    m_size--;
                    return std::move(current->m_value);
                } 
                if (current != m_root) {
                    splay(current);
                }
                current = m_root;
                if (current->m_right == nullptr) {
                    m_root = current->m_left;
                    current->m_left->m_parent.reset();
                    m_size--;
                    return std::move(current->m_value);
                } else if (current->m_left == nullptr) {
                    m_root = current->m_right;
                    current->m_right->m_parent.reset();
                    m_size--;
                    return std::move(current->m_value);
                } else {
                    std::shared_ptr<splay_tree_node> successor = current->m_right;
                    while (successor->m_left != nullptr) {
                        successor = successor->m_left;
                    }
                    if (successor == current->m_right) {
                        successor->m_left = current->m_left;
                        current->m_left->m_parent = successor;
                        successor->m_parent.reset();
                        m_root = successor;
                        m_size--;
                        return std::move(current->m_value);
                    }
                    std::shared_ptr<splay_tree_node> successorParent = successor->m_parent.lock();
                    if (successor->m_right != nullptr) {
                        successor->m_right->m_parent = successorParent;
                        successorParent->m_left = successor->m_right;   
                    } else {
                        successorParent->m_left = nullptr;    
                    }
                    successor->m_left = current->m_left;
                    successor->m_right = current->m_right;
                    current->m_left->m_parent = successor;
                    current->m_right->m_parent = successor;
                    successor->m_parent.reset();
                    m_root = successor;
                    m_size--;
                    return std::move(current->m_value);
                }
            }  
        }
        throw nonexistent_key();
    }
}

template <typename K, typename V>
K splay_tree<K,V>::minimum_key() {
    if (m_root == nullptr) {
        throw empty_tree();
    } else {
        std::shared_ptr<splay_tree_node> current = m_root;
        while (current->m_left != nullptr) {
            current = current->m_left;    
        }
        splay(current);
        return current->m_key;
    }
}

template <typename K, typename V>
K splay_tree<K,V>::maximum_key() {
    if (m_root == nullptr) {
        throw empty_tree();
    } else {
        std::shared_ptr<splay_tree_node> current = m_root;
        while (current->m_right != nullptr) {
            current = current->m_right;    
        }
        splay(current);
        return current->m_key;
    }
}

template <typename K, typename V>
bool splay_tree<K,V>::empty() const {
	if (m_root == nullptr) {
        return true;    
    }
    return false;
}

template <typename K, typename V>
size_t splay_tree<K,V>::size() const {
	return m_size;
}

}
