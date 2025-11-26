#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

// Importation de l'exercice
import exo6;
using namespace exo6;

// Exemple de test unitaire
TEST_CASE("Exo6", "[MyClass]") {
    auto vec = std::vector<int>{1, 4, 5, 2, 1, 7, 8, 5, 8, 2, 9};
    VectorUtils::SortAndRemoveDuplicates(vec);
    const auto result = std::vector<int>{1, 2, 4, 5, 7, 8, 9};
    REQUIRE(vec == result);
    const auto vecConst = vec;
    REQUIRE(VectorUtils::CountGreatersThan(vecConst, 5) == 3);
}