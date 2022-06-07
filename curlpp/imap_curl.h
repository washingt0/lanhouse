#ifndef LAN_HOUSE_IMAP_CURL_H
#define LAN_HOUSE_IMAP_CURL_H

#include <curl/curl.h>
#include <string>
#include <map>
#include <vector>

namespace lan_house {
    const std::string SEARCH_RESPONSE_PREFIX = "* SEARCH ";
    const std::string FROM_KEY = "From";
    const std::string SUBJECT_KEY = "Subject";
    const std::string CONTENT_DISPOSITION_KEY = "Content-Disposition";
    const std::string CONTENT_DISPOSITION_ATTACHMENT = "attachment";
    const std::string FILE_PATH = "filePath";

    class IMAP {
    private:
        CURL *curl;
        std::string base_url, username, password;
        std::vector<std::string> allowed_senders;

        void perform_action();
        static size_t write_curl_callback(void *contents, size_t size, size_t nmemb, std::string *s);
        static std::vector<std::string> _parse_search_response(std::string *response);
        static std::map<std::string, std::string> _parse_fetch_message_response(std::string *response);
        void _init_curl_parameters();

    public:
        IMAP(std::string base_url, std::string username, std::string password, std::vector<std::string> allowed_senders);
        ~IMAP();
        std::vector<std::string> fetch_unseen_messages();
        std::string fetch_message(std::string uid);
    };
} // lan_house

#endif //LAN_HOUSE_IMAP_CURL_H
