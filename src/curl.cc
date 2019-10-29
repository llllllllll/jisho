#include <cstring>

#include "jisho/curl.hpp"

namespace jisho::curl {

void global_init() {
    if (curl_global_init(CURL_GLOBAL_ALL)) {
        throw std::runtime_error{"failed to initialize curl"};
    }
}


[[nodiscard]] owned_header_list set_headers(CURL* curl,
                                            const std::vector<header>& headers) {
    curl_slist* header_list = NULL;

    for (const header& h : headers) {
        std::stringstream s;
        s << h.first << ":" << h.second;

        curl_slist* head = curl_slist_append(header_list, s.str().data());
        if (!head) {
            if (header_list) {
                curl_slist_free_all(header_list);
            }
            throw error("failed to construct request headers");
        }

        header_list = head;
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
    return owned_header_list(header_list);
}

namespace {
size_t read_callback(char* ptr, std::size_t size, std::size_t nmemb, void* inbuf) {
    std::string_view* buf = reinterpret_cast<std::string_view*>(inbuf);

    // TODO: Should we be worried about overflow in the multiplication here?
    std::size_t max;
    if (__builtin_mul_overflow(size, nmemb, &max)) {
        throw std::overflow_error("size * nmemb overflows");
    }
    std::size_t to_copy = std::min(buf->size(), max);
    buf->copy(ptr, to_copy);
    buf->remove_prefix(to_copy);
    return to_copy;
};

using write_callback_type = std::size_t (*)(char*, std::size_t, std::size_t, void*);

void set_common_request_fields(CURL* curl,
                               const std::string_view& url,
                               std::string& output_buffer,
                               std::array<char, CURL_ERROR_SIZE>& error_message_buffer) {

    curl_easy_setopt(curl, CURLOPT_URL, url.data());
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error_message_buffer.data());
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &output_buffer);
    write_callback_type write_callback =
        [](char* ptr, std::size_t size, std::size_t nmemb, void* closure) {
            reinterpret_cast<std::string*>(closure)->append(ptr, size * nmemb);
            return size * nmemb;
        };
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
}

long perform_request(CURL* curl,
                     const std::vector<header>& headers,
                     const std::array<char, CURL_ERROR_SIZE>& error_message_buffer) {

    owned_header_list headers_to_free(set_headers(curl, headers));

    CURLcode error_code = curl_easy_perform(curl);

    if (CURLE_OK != error_code) {
        // TODO: curl_easy_strerror
        throw error{error_message_buffer.data()};
    }

    long response_code;
    error_code = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    if (CURLE_OK != error_code) {
        throw error(curl_easy_strerror(error_code));
    }

    return response_code;
}

void throw_for_status(long code, const std::string_view& response_body) {
    if (200 == code) {
        return;
    }

    std::stringstream s;
    s << "request was not a 200; status was 400:\n" << response_body;
    throw http_error(s.str(), code);
}
}  // namespace

std::string session::get(const std::string_view& url,
                         const std::vector<header>& headers) const {
    std::string out;
    std::array<char, CURL_ERROR_SIZE> error_buffer;

    curl_easy_setopt(m_curl.get(), CURLOPT_HTTPGET, 1L);

    set_common_request_fields(m_curl.get(), url, out, error_buffer);

    long code = perform_request(m_curl.get(), headers, error_buffer);
    throw_for_status(code, out);

    return out;
}

std::string session::put(const std::string_view& url,
                         const std::vector<header>& headers,
                         const std::string_view& body) const {
    std::string out;
    std::array<char, CURL_ERROR_SIZE> error_buffer;
    std::string_view body_copy = body;

    curl_easy_setopt(m_curl.get(), CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(m_curl.get(), CURLOPT_READFUNCTION, &read_callback);
    curl_easy_setopt(m_curl.get(), CURLOPT_READDATA, &body_copy);
    curl_easy_setopt(m_curl.get(),
                     CURLOPT_INFILESIZE_LARGE,
                     static_cast<curl_off_t>(body.size()));

    set_common_request_fields(m_curl.get(), url, out, error_buffer);

    long code = perform_request(m_curl.get(), headers, error_buffer);
    throw_for_status(code, out);

    return out;
}

string session::escape(const std::string_view& str) const {
    if (str.size() > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
        throw std::overflow_error{"str exceeds max size"};
    }
    char* escaped = curl_easy_escape(m_curl.get(), str.data(), str.size());
    if (!escaped) {
        throw error("failed to escape string");
    }
    return string{escaped};
}
}  // namespace jisho::curl
