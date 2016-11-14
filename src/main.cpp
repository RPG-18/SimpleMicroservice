#include <iostream>
#include "3rdparty/easylogging/easylogging++.h"

#include "Application.h"

INITIALIZE_EASYLOGGINGPP

int main(int argc, char *argv[])
{
    START_EASYLOGGINGPP(argc, argv);
    el::Configurations defaultConf;
    defaultConf.setGlobally(
        el::ConfigurationType::Format, "%datetime [%level] %msg");
    el::Loggers::reconfigureLogger("default", defaultConf);


    MicroService::Application application;
    if (argc != 2) {
        std::cerr << "The configuration file is not set" << std::endl;
        std::cerr << "  MicroService path-to-config" << std::endl;
        return 1;
    }
    else {
        application.readConfiguration(argv[1]);
    }

    if (!application.openLocalStorage()) {
        std::cerr << "Error opening database" << std::endl;
        return 1;
    }

    application.run();

    return 0;
}