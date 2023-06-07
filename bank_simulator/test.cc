#include <iostream>
#include <string>

class AbstractFunctor {
    public:
        virtual void specific() = 0;

        void operator()() {
            std::cout << "Inside MyFunctor: " << std::endl;
            specific();
        }
};

class NewFunctor : public AbstractFunctor {
    private:
        std::string test;

    public:
        void specific() override {
            std::cout << "Specific: " << test << std::endl;
        }

        NewFunctor(std::string test) : test(test){}
};

int main() {
    NewFunctor functor("test");
    functor();  // Call the functor like a function
    return 0;
}