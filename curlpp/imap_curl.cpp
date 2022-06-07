#include <curl/curl.h>
#include <mimetic/mimetic.h>
#include "imap_curl.h"
#include "../b64/b64.h"
#include <string>
#include <utility>
#include <chrono>

#define curl_set(option, parameter) curl_easy_setopt(this->curl, option, parameter)
#define mss std::map<std::string, std::string>
#define vs std::vector<std::string>

std::string drop_crlf(std::string const& input) {
    std::stringstream ss;

    for (char i : input) {
        if (i != '\n' && i != '\r')
            ss << i;
    }

    return ss.str();
}

namespace lan_house {
    // public
    IMAP::IMAP(std::string base_url, std::string username, std::string password, vs allowed_senders) {
        this->base_url = std::move(base_url);
        this->username = std::move(username);
        this->password = std::move(password);
        this->allowed_senders = std::move(allowed_senders);

        this->_init_curl_parameters();
    }

    IMAP::~IMAP() {
        curl_easy_cleanup(this->curl);
    }

    std::vector<std::string> IMAP::fetch_unseen_messages() {
        std::string response;

        curl_set(CURLOPT_URL, (this->base_url + "/INBOX").c_str());
        curl_set(CURLOPT_CUSTOMREQUEST, "SEARCH UNSEEN");

        curl_set(CURLOPT_WRITEFUNCTION, IMAP::write_curl_callback);
        curl_set(CURLOPT_WRITEDATA, &response);

        this->perform_action();

        return _parse_search_response(&response);
    }

     std::string IMAP::fetch_message(std::string uid) {
        std::string response;

        curl_set(CURLOPT_URL, (this->base_url + "/INBOX;UID=" + uid).c_str());
        curl_set(CURLOPT_WRITEFUNCTION, IMAP::write_curl_callback);
        curl_set(CURLOPT_WRITEDATA, &response);

        this->perform_action();

        mss results = this->_parse_fetch_message_response(&response);

        for (auto& sender: this->allowed_senders) {
            if (results[FROM_KEY].find(sender) != std::string::npos)
                return results[FILE_PATH];
        }

        return std::string();
    }

    // private
    void IMAP::_init_curl_parameters() {
        this->curl = curl_easy_init();

        curl_set(CURLOPT_USERNAME, this->username.c_str());
        curl_set(CURLOPT_PASSWORD, this->password.c_str());
        curl_set(CURLOPT_VERBOSE, false);
        curl_set(CURLOPT_LOGIN_OPTIONS, "AUTH=PLAIN");
    }

    void IMAP::perform_action() {
        CURLcode res = CURLE_OK;

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::string error = std::string("FAILED: ") + curl_easy_strerror(res);
            throw std::runtime_error(error);
        }

        curl_easy_cleanup(this->curl);
        this->_init_curl_parameters();
    }

    size_t IMAP::write_curl_callback(void *contents, size_t size, size_t nmemb, std::string *s) {
        size_t newLength = size*nmemb;
        try {
            s->append((char*)contents, newLength);
        } catch(std::bad_alloc &e) {
            return 0;
        }
        return newLength;
    }

    vs IMAP::_parse_search_response(std::string *response) {
        vs result = vs();

        std::size_t last_position = 0;
        std::size_t position = response->find("\r\n");
        while (position != std::string::npos) {
            std::string line = response->substr(last_position, position-last_position);

            last_position = position + 2;
            position = response->find("\r\n", last_position);

            if (line.length() <= SEARCH_RESPONSE_PREFIX.length()) continue;

            result.push_back(line.substr(9, line.length()-1));
        }

        return result;
    }

    std::string writeTemporaryFile(mimetic::MimeEntity *part) {
        // TODO build random
        double time_now = std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();
        std::string path = "/tmp/" + std::to_string(time_now);

        std::ofstream file;

        file.open(path, std::ios::out | std::ios::binary);
        file << encoding::B64::decode(drop_crlf(part->body()));
        file.close();

        return path;
    }

    mss IMAP::_parse_fetch_message_response(std::string *response) {
        mss results = mss();
        mimetic::MimeEntity me = mimetic::MimeEntity(response->begin(), response->end());

        for (auto& field: me.header()) {
            results[field.name()] = field.value();
        }

        bool found = false;
        for (auto& part: me.body().parts()) {
            for (auto& field: part->header()) {
                if (field.name() == CONTENT_DISPOSITION_KEY &&
                    field.value().find(CONTENT_DISPOSITION_ATTACHMENT, 0) != std::string::npos) {
                    found = true;
                    break;
                }
            }

            if (found) {
                results[FILE_PATH] = writeTemporaryFile(part);
                break;
            }
        }

        return results;
    }
};