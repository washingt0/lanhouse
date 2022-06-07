#ifndef LAN_HOUSE_PRINT_CUPS_H
#define LAN_HOUSE_PRINT_CUPS_H

#include <cups/cups.h>
#include <string>

namespace printing {
    class PrintingService {
    private:
        cups_dest_t* destination = NULL;
        cups_dinfo_t* destination_info = NULL;
        std::string server_name;

        void init();
        void find_default_destination();
        static int destination_discovery_callback(PrintingService*, unsigned , cups_dest_t*);
        void get_destination_info();

    public:
        PrintingService();
        explicit PrintingService(const char*);

        void print_file(std::string path);
    };
}

#endif //LAN_HOUSE_PRINT_CUPS_H

