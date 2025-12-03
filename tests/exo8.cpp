#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

// Importation de l'exercice
import exo8;
using namespace exo8;

// Exemple de test unitaire
TEST_CASE("Exo8", "[MyClass]") {
    auto tree = BinaryTree{};

    auto vec_test = [&](std::vector<int> result) {
        std::vector<int> vec;
        tree.Traversal([&](const int i) {
            vec.push_back(i);
        });
        REQUIRE(vec == result);
    };

    vec_test({});
    tree.Insert(10);
    vec_test({10});
    tree.Insert(5);
    vec_test({5, 10});
    tree.Insert(15);
    vec_test({5, 10, 15});
    tree.Insert(3);
    vec_test({3, 5, 10, 15});
    tree.Insert(7);
    vec_test({3, 5, 7, 10, 15});
}