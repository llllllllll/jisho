#include <iostream>

#include "jisho/curl.hpp"
#include "jisho/jisho.hpp"

int main(int argc, char** argv) {
    if (argc == 1) {
        std::cerr << "usage: " << argv[0] << " WORD [WORD...]\n";
        return -1;
    }

    char* jisho_api = getenv("JISHO_API");
    if (jisho_api && *jisho_api) {
        jisho::jisho_api = jisho_api;
        if (jisho::jisho_api.back() != '/') {
            jisho::jisho_api.push_back('/');
        }
    }

    jisho::curl::session session;
    for (int ix = 1; ix < argc; ++ix) {
        std::cout << jisho::definition(session, argv[ix]) << '\n';
    }
    return 0;
}
