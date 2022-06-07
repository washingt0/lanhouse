#ifndef LAN_HOUSE_UTILS_H
#define LAN_HOUSE_UTILS_H

#include <vector>

namespace utils {
    std::string get_required_env(std::string const& env);
    std::vector<std::string> get_required_env_vs(std::string const& env);
    bool wait(int seconds);
}

#endif //LAN_HOUSE_UTILS_H
