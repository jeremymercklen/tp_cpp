module;
// Import rapide de la bibliothèque standard
import std;

// Nommage et exportation de ce module
export module exo6;

// On met toutes les classes du module dans un espace de nom
// ayant le même nom que le module pour plus de clarté et éviter
// les conflits de nommage.
// On exporte la totalité des classe de l'espace de nom.
export namespace exo6 {
    class VectorUtils {
    public:
        static void Print (const std::vector<int>& vec) {
            std::for_each(vec.begin(), vec.end(), [](int val) {
                std::cout << val << " ";
            });
            std::cout << std::endl;
        }
        static void SortAndRemoveDuplicates (std::vector<int>& vec) {
            std::sort(vec.begin(), vec.end(), std::less<int>());
            auto last = std::unique(vec.begin(), vec.end());
            vec.erase(last, vec.end());
        }
        static int CountGreatersThan(const std::vector<int>& vec, int treshold) {
            return std::count_if(vec.begin(), vec.end(), [treshold](int val) {
                return val > treshold;
            });
        }
    };
}