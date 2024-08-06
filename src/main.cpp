
#include "Application.hpp"
#include <csignal>
#include <iostream>


int main()
{
    Application app;
    app.start();
    char c;
    std::cin >> c;
    std::cout << "Stopping" << std::endl;
    app.stop();
    return 0;
}
