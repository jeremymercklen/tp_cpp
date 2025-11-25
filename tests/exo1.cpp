#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>

// Importation de l'exercice
import exo1;
using namespace exo1;

// Exemple de test unitaire
TEST_CASE("Exo1", "[MyClass]") {
    Vector v1{12.34, 34.56, 56.78};
    REQUIRE(v1.GetX() == 12.34);
    REQUIRE(v1.GetY() == 34.56);
    REQUIRE(v1.GetZ() == 56.78);
    REQUIRE(v1.ToString() == "(12.34,34.56,56.78)");
    v1.Homothety(2.1);
    REQUIRE(v1.ToString() == "(25.91,72.58,119.24)");
    v1.SetX(34.12);
    v1.SetY(56.34);
    v1.SetZ(78.56);
    REQUIRE(v1.ToString() == "(34.12,56.34,78.56)");
    const Vector v2{1.1, 2.2, 3.3};
    v1.Sum(v2);
    REQUIRE(v1.ToString() == "(35.22,58.54,81.86)");
}