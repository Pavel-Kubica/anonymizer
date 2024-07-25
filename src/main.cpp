
#include "Application.hpp"
#include <csignal>
#include <functional>


std::function<void(int)> terminationHandler;
void signalHandler(int signal)
{
    terminationHandler(signal);
}

int main()
{
    Application app;
    app.start();
    terminationHandler = [&](int dummy) { app.stop(); };
    signal(SIGINT, signalHandler);
    sleep (1000);
    return 0;
}
