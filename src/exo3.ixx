module;
// Import rapide de la bibliothèque standard
import std;

// Nommage et exportation de ce module
export module exo3;

// On met toutes les classes du module dans un espace de nom
// ayant le même nom que le module pour plus de clarté et éviter
// les conflits de nommage.
// On exporte la totalité des classe de l'espace de nom.
export namespace exo3 {
    struct Vector {
        double x{};
        double y{};
        double z{};

        Vector (double x, double y, double z) : x (x), y (y), z (z) { std::cout << "Constructor Vector()" << std::endl; };
        Vector (const Vector& vec) : x (vec.x), y (vec.y), z (vec.z) { std::cout << "Copy Vector()" << std::endl; };

        void Homothety(double value) {
            x = x * value;
            y = y * value;
            z = z * value;
        }

        void Sum1(Vector vectorCopy) {
            x += vectorCopy.x;
            y += vectorCopy.y;
            z += vectorCopy.z;
        }

        void Sum2(const Vector& vector) {
            x += vector.x;
            y += vector.y;
            z += vector.z;
        }

        std::string ToString () const {
            return std::format("({:.2f},{:.2f},{:.2f})", x, y, z);
        }

        Vector operator+(const Vector vector) const {
            return Vector{x + vector.x, y + vector.y, z + vector.z};
        };

        Vector operator*(const double value) const {
            return Vector { x * value, y * value, z * value };
        }

        bool operator==(const Vector vector) const {
            return x == vector.x && y == vector.y && z == vector.z;
        }
    };
}