#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

// Importation de l'exercice
import exo5;
using namespace exo5;

// Exemple de test unitaire
TEST_CASE("Exo5", "[MyClass]") {
    REQUIRE(std::format("{0:.2f}", Polygon{{1.0,2.0,3.0}}.Perimeter()) == "6.00");
    REQUIRE(std::format("{0:.2f}", Rectangle{1.0,2.0}.Perimeter()) == "6.00");
    REQUIRE(std::format("{0:.2f}", Square{1.0}.Perimeter()) == "4.00");
}