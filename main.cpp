#include <iostream>
#include "curlpp/imap_curl.h"
#include "cupspp/print_cups.h"
#include "utils/utils.h"
#include "log/log.h"

using namespace std;
using namespace lan_house;

void message_processing_loop(IMAP* imap, printing::PrintingService* printingService) {
    do {
        LOG::info("Starting fetch of messages from IMAP server");
        vector<string> unseen_messages = imap->fetch_unseen_messages();
        LOG::info(std::string("Fetched messages from IMAP server: ").append(to_string(unseen_messages.size())));
        for (auto& uid: unseen_messages) {
            std::string attachment_file_path = imap->fetch_message(uid);
            if (!attachment_file_path.empty())
                printingService->print_file(attachment_file_path);
        }
    } while (utils::wait(60));
}

int main() {
    const string USERNAME = utils::get_required_env("LANHOUSE_MAIL_USERNAME");
    const string PASSWORD = utils::get_required_env("LANHOUSE_MAIL_PASSWORD");
    const string BASE_URL = utils::get_required_env("LANHOUSE_MAIL_SERVER");
    vector<string> allowed_senders = utils::get_required_env_vs("LANHOUSE_ALLOWED_SENDERS");

    IMAP imap = IMAP(BASE_URL, USERNAME, PASSWORD, allowed_senders);
    printing::PrintingService printingService = printing::PrintingService();

    message_processing_loop(&imap, &printingService);

    return 0;
}

