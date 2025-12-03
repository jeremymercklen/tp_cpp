module;
// Import rapide de la bibliothèque standard
import std;

// Nommage et exportation de ce module
export module exo9;
//#include <memory>

// On met toutes les classes du module dans un espace de nom
// ayant le même nom que le module pour plus de clarté et éviter
// les conflits de nommage.
// On exporte la totalité des classe de l'espace de nom.
export namespace exo9 {
    template<typename T>
    struct Node {
        T value{};
        std::unique_ptr<Node> left{nullptr};
        std::unique_ptr<Node> right{nullptr};
    };
    template<typename T>
    class BinaryTree {
    private:
        std::unique_ptr<Node<T>> rootNode{nullptr};
        std::uint64_t size{0};

        void InsertRecursive(const T& value, std::unique_ptr<Node<T>>& node) {
            if (node == nullptr) {
                node = std::make_unique<Node<T>>(value);
            }
            else if (value < node->value) {
                InsertRecursive(value, node->left);
            }
            else if (value > node->value) {
                InsertRecursive(value, node->right);
            }
        }

        void TraversalRecursive(const std::unique_ptr<Node<T>>& node, const std::function<void(T& value)> func) const {
            if (!node) return;
            TraversalRecursive(node->left, func);
            func(node->value);
            TraversalRecursive(node->right, func);
        }
    public:
        void Insert(const T value) {
            if (rootNode == nullptr) {
                rootNode = std::make_unique<Node<T>>(value);
            }
            else {
                InsertRecursive(value, rootNode);
            }
            ++size;
        }

        void Traversal(const std::function<void(T& data)> func) const {
            TraversalRecursive(rootNode, func);
        }

        void Clear() {
            rootNode = nullptr;
            size = 0;
        }

        std::uint64_t GetSize() const {
            return size;
        };

        bool Contains(const T& value) const {
            if (rootNode == nullptr) {
                return false;
            }
            bool found{false};
            Traversal([&](const T& v) {
                if (value == v) {
                    found = true;
                }
            });
            return found;
        }
    };

    template<typename T, typename U>
    struct MapEntry {
        T key;
        U value;

        friend bool operator<(MapEntry lhs, MapEntry rhs) {
            return lhs.key < rhs.key;
        }
        friend bool operator<=(MapEntry lhs, MapEntry rhs) {
            return lhs.key <= rhs.key;
        }
        friend bool operator>(MapEntry lhs, MapEntry rhs) {
            return lhs.key > rhs.key;
        }
        friend bool operator>=(MapEntry lhs, MapEntry rhs) {
            return lhs.key >= rhs.key;
        }
        friend bool operator==(MapEntry lhs, MapEntry rhs) {
            return lhs.key == rhs.key;
        }
        friend bool operator!=(MapEntry lhs, MapEntry rhs) {
            return lhs.key != rhs.key;
        }
    };

    template<typename T, typename U>
    struct Map {
        BinaryTree<MapEntry<T, U>> tree{};

    public:
        void Clear() {
            tree.Clear();
        }

        std::uint64_t GetSize() const {
            return tree.GetSize();
        }

        bool Contains(const T& key) const {
            return tree.Contains({key});
        }

        U& Get(const T& key) {
            if (!tree.Contains({key})) {
                throw std::out_of_range("Key not found");
            }
            MapEntry<T, U> *result;
            tree.Traversal([&](MapEntry<T, U>& entry) {
                if (entry.key == key) {
                    result = &entry;
                    return true;
                }
                return false;
            });
            return result->value;
        }

        void Put (const T& key, const U& value) {
            if (!tree.Contains({key})) {
                tree.Insert(MapEntry<T, U>{key, value});
            }
            else {
                tree.Traversal([&](MapEntry<T, U>& entry) {
                if (entry.key == key) {
                    entry.value = value;
                    return true;
                }
                return false;
            });
            }
        }
    };
}