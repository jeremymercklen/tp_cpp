module;
// Import rapide de la bibliothèque standard
import std;

// Nommage et exportation de ce module
export module exo2;

// On met toutes les classes du module dans un espace de nom
// ayant le même nom que le module pour plus de clarté et éviter
// les conflits de nommage.
// On exporte la totalité des classe de l'espace de nom.
export namespace exo2 {
    class Vector {
    public:
        Vector (double x, double y, double z) : x (x), y (y), z (z) { std::cout << "Constructor Vector()" << std::endl; };
        Vector (const Vector& vec) : x (vec.x), y (vec.y), z (vec.z) { std::cout << "Copy Vector()" << std::endl; };

        const double GetX() {
            return x;
        };
        const double GetY() {
            return y;
        };
        const double GetZ() {
            return z;
        };

        void SetX (double x) {
            this->x = x;
        };
        void SetY (double y) {
            this->y = y;
        };
        void SetZ (double z) {
            this->z = z;
        };

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
    private:
        double x{};
        double y{};
        double z{};
    };
}