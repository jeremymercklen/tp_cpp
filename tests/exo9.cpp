#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

// Importation de l'exercice
import exo9;
using namespace exo9;

// Exemple de test unitaire
TEST_CASE("Exo9-1", "[MyClass]") {
    auto tree = BinaryTree<std::string>{};

    auto vec_test = [&](std::vector<std::string> result) {
        std::vector<std::string> vec;
        tree.Traversal([&vec](const std::string i) {
            vec.push_back(i);
        });
        REQUIRE(vec == result);
    };

    vec_test({});
    tree.Insert("C++");
    vec_test({"C++"});
    tree.Insert("ASM");
    vec_test({"ASM", "C++"});
    tree.Insert("GO");
    vec_test({"ASM", "C++", "GO"});
    tree.Insert("Rust");
    vec_test({"ASM", "C++", "GO", "Rust"});
    tree.Insert("JavaScript");
    vec_test({"ASM", "C++", "GO", "JavaScript", "Rust"});
}

TEST_CASE("Exo9_2", "[Map]") {
    auto map = Map<std::string, std::uint64_t>{};
    REQUIRE(map.GetSize() == 0);
    REQUIRE_FALSE(map.Contains("Jean"));
    map.Put("Jean", 18);
    REQUIRE(map.GetSize() == 1);
    REQUIRE(map.Contains("Jean"));
    map.Put("Georges", 20);
    REQUIRE(map.GetSize() == 2);
    REQUIRE(map.Contains("Jean"));
    REQUIRE(map.Contains("Georges"));
    REQUIRE_FALSE(map.Contains("Jeanne"));
    REQUIRE(map.Get("Jean") == 18);
    REQUIRE(map.Get("Georges") == 20);
    map.Put("Jean", 8);
    REQUIRE(map.Contains("Jean"));
    REQUIRE(map.Contains("Georges"));
    REQUIRE_FALSE(map.Contains("Jeanne"));
    REQUIRE(map.Get("Jean") == 8);
    REQUIRE(map.Get("Georges") == 20);
    map.Clear();
    REQUIRE(map.GetSize() == 0);
    REQUIRE_FALSE(map.Contains("Jean"));
}