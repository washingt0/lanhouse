#ifndef LAN_HOUSE_LOG_H
#define LAN_HOUSE_LOG_H

#include <string>
#include <ctime>
#include <iostream>
#include <iomanip>

namespace LOG {
    void info(std::string message);
    void warn(std::string message);
    void error(std::string message);
};

#endif //LAN_HOUSE_LOG_H
