//
// Created by wsantos on 6/6/22.
//

#include "log.h"

#include <utility>

namespace LOG {
    void _log(std::string kind, std::string message) {
        time_t t = std::time(nullptr);
        tm tm = *std::localtime(&t);
        std::cout
            << std::put_time(&tm, "%d-%m-%Y %H-%M-%S")
            << " [" << kind << "] - "
            << message
            << std::endl;
    }

    void info(std::string message) {
        _log("INFO", std::move(message));
    }

    void warn(std::string message) {
        _log("WARNING", std::move(message));
    }

    void error(std::string message) {
        _log("ERROR", std::move(message));
    }
}
