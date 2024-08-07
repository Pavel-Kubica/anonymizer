
#include "Application.hpp"

Application::Application() : queue(), fileManager(), consumer(&queue), transformer(&queue, &fileManager),
                             dbManager("http://localhost:8124"), sender(&fileManager, &dbManager)
{}

void Application::start()
{
    consumer.start();
    transformer.start();
    sender.start();
}

void Application::stop()
{
    sender.stop();
    consumer.stop();
    transformer.stop();
}