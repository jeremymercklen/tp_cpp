module;
// Import rapide de la bibliothèque standard
import std;

// Nommage et exportation de ce module
export module exo8;
//#include <memory>

// On met toutes les classes du module dans un espace de nom
// ayant le même nom que le module pour plus de clarté et éviter
// les conflits de nommage.
// On exporte la totalité des classe de l'espace de nom.
export namespace exo8 {
    struct Node {
        int value{};
        std::unique_ptr<Node> left{nullptr};
        std::unique_ptr<Node> right{nullptr};
    };

    class BinaryTree {
    private:
        std::unique_ptr<Node> rootNode{nullptr};

        void InsertRecursive(const int& value, std::unique_ptr<Node>& node) {
            if (node == nullptr) {
                node = std::make_unique<Node>(value);
            }
            else if (value < node->value) {
                InsertRecursive(value, node->left);
            }
            else if (value > node->value) {
                InsertRecursive(value, node->right);
            }
        }

        void TraversalRecursive(const std::unique_ptr<Node>& node, const std::function<void(int value)>& func) {
            if (!node) return;
            TraversalRecursive(node->left, func);
            func(node->value);
            TraversalRecursive(node->right, func);
        }
    public:
        void Insert(const int value) {
            if (rootNode == nullptr) {
                rootNode = std::make_unique<Node>(value);
            }
            else {
                InsertRecursive(value, rootNode);
            }
        }
        void Traversal(const std::function<void(int data)>& func) {
            TraversalRecursive(rootNode, func);
        }
    };
}