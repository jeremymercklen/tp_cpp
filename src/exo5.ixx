module;
// Import rapide de la bibliothèque standard
import std;

// Nommage et exportation de ce module
export module exo5;

// On met toutes les classes du module dans un espace de nom
// ayant le même nom que le module pour plus de clarté et éviter
// les conflits de nommage.
// On exporte la totalité des classe de l'espace de nom.
export namespace exo5 {
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

    class Polygon : public Shape {
    protected:
        std::vector<double> sides;
    public:
        Polygon(std::initializer_list<double> sides) : sides(sides) {};
        virtual double Perimeter() const override {
            return std::accumulate(sides.begin(), sides.end(), 0.0);
        }

        virtual std::string ToString() const override {
            std::string toString{};
            for (double side : sides) {
                toString += std::format("{:.2f},", side);
            }
            toString.pop_back();
            return toString;
        }
    };

    class Rectangle : public Polygon {
    public:
        Rectangle(double height, double length) : Polygon({height, length}) {};

        virtual double Perimeter() const override {
            return std::accumulate(sides.begin(), sides.end(), 0.0) * 2;
        }
        virtual std::string ToString() const override {
            return Polygon::ToString();
        }
    };

    class Square : public Rectangle {
    public:
        Square(double width) : Rectangle({width, width}) {};

        virtual double Perimeter() const override {
            return Rectangle::Perimeter();
        };
        virtual std::string ToString() const override {
            return Polygon::ToString();
        };
    };
}