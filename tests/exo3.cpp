#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

// Importation de l'exercice
import exo3;
using namespace exo3;

// Exemple de test unitaire
TEST_CASE("Exo3", "[MyClass]") {
    const auto v1 = Vector{12.34, 56.78, 90.12};
    REQUIRE(v1.ToString() == "(12.34,56.78,90.12)");
    REQUIRE((v1 * 2.0).ToString() == "(24.68,113.56,180.24)");
    REQUIRE((v1 + Vector{1.1,2.2,3.3}).ToString() == "(13.44,58.98,93.42)");
    const auto v2 = v1 + Vector{34.12,78.56,12.90};
    REQUIRE(!(v1 == v2));
    REQUIRE(v1 == v1);
}