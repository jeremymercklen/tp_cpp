module;
// Import rapide de la bibliothèque standard
import std;

// Nommage et exportation de ce module
export module exo4;

// On met toutes les classes du module dans un espace de nom
// ayant le même nom que le module pour plus de clarté et éviter
// les conflits de nommage.
// On exporte la totalité des classe de l'espace de nom.
export namespace exo4 {
    class Shape {
    public:
        virtual ~Shape() = default;

        virtual double Perimeter() const = 0;
        virtual std::string ToString() const = 0;
    };

    class Circle : public Shape {
    private:
        double radius{};
    public:
        Circle(double radius) : radius(radius) {};
        virtual double Perimeter() const override {
            return 2* std::numbers::pi * radius;
        };
        virtual std::string ToString() const override {
            return std::format("({:.2f}", radius);
        };
    };
}