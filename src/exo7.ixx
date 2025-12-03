module;
// Import rapide de la bibliothèque standard
import std;

// Nommage et exportation de ce module
export module exo7;

// On met toutes les classes du module dans un espace de nom
// ayant le même nom que le module pour plus de clarté et éviter
// les conflits de nommage.
// On exporte la totalité des classe de l'espace de nom.
export namespace exo7 {
    class Resource{
    public:
        Resource() {
            std::cout << "Resource::Resource()" << std::endl;
        };
        ~Resource() {
            std::cout << "~Resource()" << std::endl;
        }

        void DoSomething() {
            std::cout << "DoSomething()" << std::endl;
        }
    };

    class Application {
    public:
        Application() {
            UniquePtr();
            SharedPtr();
        }
        virtual ~Application() {}

        void UniquePtr() {
            std::unique_ptr<Resource> res = std::make_unique<Resource>();
            res->DoSomething();
        }

        void SharedPtr() {
            std::shared_ptr<Resource> res = std::make_shared<Resource>();
            std::cout << "ressource use count: " << res.use_count() << std::endl;
            UseSharedPtr(res);
            std::cout << "ressource use count: " << res.use_count() << std::endl;
        }

        void UseSharedPtr(const std::shared_ptr<Resource>/*&*/ res) {
            std::cout << "ressource use count: " << res.use_count() << std::endl;
            res->DoSomething();
            if (res.use_count() < 5) UseSharedPtr(res);
        }
    };
}