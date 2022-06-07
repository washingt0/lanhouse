#include <stdexcept>
#include "print_cups.h"
#include "../log/log.h"
#include <iostream>
#include <cstdio>

namespace printing {
    PrintingService::PrintingService() {
        this->server_name = "cups default";
        this->init();
    }

    PrintingService::PrintingService(const char* server_name) {
        this->server_name = server_name;
        cupsSetServer(this->server_name.c_str());
        this->init();
    }

    void PrintingService::init() {
        this->find_default_destination();

        if (this->destination == NULL) {
            throw std::runtime_error(std::string("No default destionation found on server ").append(server_name));
        }

        this->get_destination_info();
    }

    void PrintingService::get_destination_info() {
        this->destination_info = cupsCopyDestInfo(CUPS_HTTP_DEFAULT, this->destination);
    }

    void PrintingService::find_default_destination() {
        if(!cupsEnumDests(
                CUPS_DEST_FLAGS_NONE,
                1000,
                NULL,
                CUPS_PRINTER_LOCAL | CUPS_PRINTER_REMOTE,
                0,
                (cups_dest_cb_t)printing::PrintingService::destination_discovery_callback,
                this)) {
            cupsFreeDests(1, this->destination);
            throw std::runtime_error(std::string("Error while fetching destinations: ").append(cupsLastErrorString()).c_str());
        }
    }

    int PrintingService::destination_discovery_callback(
            PrintingService *data, unsigned  flags, cups_dest_t *destination) {
        if (destination->is_default) {
            cupsCopyDest(destination, 0, &data->destination);
            return 0;
        }

        return 1;
    }

    void PrintingService::print_file(std::string path) {
        int job_id = -1;
        int num_options = 0;
        cups_option_t *options = NULL;

        num_options = cupsAddOption(CUPS_COPIES, "1", num_options, &options);
        num_options = cupsAddOption(CUPS_MEDIA, CUPS_MEDIA_A4, num_options, &options);
        num_options = cupsAddOption(CUPS_SIDES, CUPS_SIDES_ONE_SIDED, num_options, &options);

        if (cupsCreateDestJob(CUPS_HTTP_DEFAULT, this->destination, this->destination_info,
                              &job_id, "AUTO PRINTING FROM MAIL", num_options, options) == IPP_STATUS_OK) {
            LOG::info((std::stringstream() << "Job #" << job_id << " created").str());
        } else {
            throw std::runtime_error(cupsLastErrorString());
        }

        FILE *fp = fopen(path.c_str(), "rb");

        size_t bytes;
        char buffer[65536];

        if (cupsStartDestDocument(CUPS_HTTP_DEFAULT, this->destination, this->destination_info,
                                  job_id, path.c_str(), CUPS_FORMAT_PDF, 0, NULL, 1) == HTTP_STATUS_CONTINUE) {
            while ((bytes = fread(buffer, 1, sizeof(buffer), fp)) > 0)
                if (cupsWriteRequestData(CUPS_HTTP_DEFAULT, buffer, bytes) != HTTP_STATUS_CONTINUE) break;

            if (cupsFinishDestDocument(CUPS_HTTP_DEFAULT, this->destination, this->destination_info) == IPP_STATUS_OK)
                LOG::info((std::stringstream() << "Job #" << job_id << " finished successfully").str());
            else
                throw std::runtime_error(std::string("Failed to print document: ").append(cupsLastErrorString()));
        }

        fclose(fp);
    }
}
