
#include "Application.hpp"
#include <csignal>
#include <functional>
#include <iostream>


std::function<void(int)> terminationHandler;
void signalHandler(int signal)
{
    std::cout << "Shutdown initiated, please wait up to 1 minute." << std::endl;
    terminationHandler(signal);
    exit(0);
}

int main()
{
    Application app;
    app.start();
    terminationHandler = [&](int dummy) { app.stop(); };
    signal(SIGINT, signalHandler);
    signal(SIGABRT, signalHandler);
    sleep (1000);
    return 0;
}
