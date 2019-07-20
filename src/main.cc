#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include <boost/program_options.hpp>

#include "jisho/batch.hpp"
#include "jisho/definition.hpp"

namespace po = boost::program_options;

int main(int argc, char** argv) {
    // clang-format off
    po::options_description desc("Options");
    desc.add_options()
        ("help,h", "show this message and exit")
        ("api-server", po::value<std::string>(), "the jisho api server to request from")
        ("word", po::value<std::vector<std::string>>(), "the words to read")
        ("csv", "format the output as a csv")
        ("csv-field-delim", po::value<char>()->default_value('\t'),
         "csv field delimiter, default to tab (for anki)")
        ;
    // clang-format on

    po::positional_options_description positional;
    positional.add("word", -1);

    po::variables_map vm;
    try {
        po::store(po::command_line_parser(argc, argv)
                      .options(desc)
                      .positional(positional)
                      .run(),
                  vm);
    }
    catch (const po::error& e) {
        std::cerr << e.what() << "\n\n" << desc;
        return 1;
    }

    if (vm.count("help")) {
        std::cout << desc << '\n';
        return 1;
    }
    po::notify(vm);

    if (vm.count("api-server")) {
        jisho::set_api_root(vm["api-server"].as<std::string>());
    }
    else if (char* jisho_api = getenv("JISHO_API")) {
        jisho::set_api_root(jisho_api);
    }

    std::vector<std::string> words;
    std::vector<std::optional<jisho::definition>> definitions;

    if (vm.count("word")) {
        words = vm["word"].as<std::vector<std::string>>();
        definitions = jisho::fetch_batch(words);
    }

    if (vm.count("csv")) {
        std::vector<jisho::definition> valid_definitions;
        for (std::optional<jisho::definition>& maybe : definitions) {
            if (maybe) {
                valid_definitions.emplace_back(std::move(*maybe));
            }
        }
        jisho::write_csv(std::cout, valid_definitions, vm["csv-field-delim"].as<char>());
    }
    else {
        std::size_t ix = 0;
        for (const std::optional<jisho::definition>& definition : definitions) {
            if (definition) {
                std::cout << *definition << '\n';
            }
            else {
                std::cout << "no definition for: " << words[ix] << '\n';
            }
            ++ix;
        }
    }

    return 0;
}
