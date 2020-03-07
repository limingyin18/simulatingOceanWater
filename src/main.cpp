#include <exception>
#include <iostream>
#include "Scene.hpp"

using namespace std;

int main()
{
    try
    {
        Scene scene{};
        while(!scene.shouldClose())
        {
            scene.draw();
        }
    }
    catch(const exception& e)
    {
        cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}