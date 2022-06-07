#include <string>
#include <stdexcept>
#include <unistd.h>
#include "utils.h"
#include "../log/log.h"

namespace utils {
    std::string get_required_env(std::string const& env) {
        char *env_value = getenv(env.c_str());
        std::string error_msg = "MISSING ENV FOR: ";

        if (env_value == NULL) throw std::runtime_error(error_msg.append(env));

        return env_value ;
    }

    std::vector<std::string> get_required_env_vs(std::string const& env) {
        std::string temp = get_required_env(env);
        std::vector<std::string> result;
        const char _sep = ',';

        size_t position = temp.find(_sep);
        size_t last_position = 0;
        while (position != std::string::npos) {
            std::string entry = temp.substr(last_position, position-last_position);

            last_position = position + 1;
            position = temp.find(_sep, last_position);

            result.push_back(entry);
        }
        result.push_back(temp.substr(last_position, temp.length()));

        return result;
    }

    bool wait(int seconds) {
        LOG::info(std::string("Waiting... ").append(std::to_string(seconds)).append("s"));
        sleep(seconds);
        return true;
    }
}
