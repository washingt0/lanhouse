//
// Created by wsantos on 6/6/22.
//

#ifndef LAN_HOUSE_B64_H
#define LAN_HOUSE_B64_H


#include <string>

namespace encoding {
    static const std::string base64_chars =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789+/";

    class B64 {
    private:
        static bool is_base64(unsigned char c);

    public:
        static std::string decode(std::string const& input);
    };
}


#endif //LAN_HOUSE_B64_H
