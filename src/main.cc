#include <iostream>

#include "jisho/batch.hpp"
#include "jisho/definition.hpp"

int main(int argc, char** argv) {
    char* jisho_api = getenv("JISHO_API");
    if (jisho_api && *jisho_api) {
        jisho::jisho_api = jisho_api;
        if (jisho::jisho_api.back() != '/') {
            jisho::jisho_api.push_back('/');
        }
    }

    std::vector<std::string_view> words;
    for (int ix = 1; ix < argc; ++ix) {
        words.emplace_back(argv[ix]);
    }

    for (const jisho::definition& definition : jisho::fetch_batch(words)) {
        std::cout << definition << '\n';
    }
    return 0;
}
