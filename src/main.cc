#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include <boost/program_options.hpp>

#include "jisho/batch.hpp"
#include "jisho/definition.hpp"
#include "jisho/sqlite.hpp"

namespace po = boost::program_options;

namespace {
template<typename T>
std::vector<T> remove_missing(const std::vector<std::optional<T>>& es) {
    std::vector<T> out;
    for (const auto& e : es) {
        if (e) {
            out.emplace_back(*e);
        }
    }
    return out;
}
}  // namespace

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
        ("sqlite", po::value<std::string>(), "write the words to a sqlite database")
        ("no-stdout", po::value<std::string>(), "don't write the output to stdout")
        ("sqlite-db-init", po::value<std::string>(), "initialize a sqlite database")
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
        jisho::write_csv(std::cout,
                         remove_missing(definitions),
                         vm["csv-field-delim"].as<char>());
    }
    else if (vm.count("sqlite-db-init")) {
        const std::string& db_path = vm["sqlite-db-init"].as<std::string>();
        auto db = jisho::sqlite::conn::make(db_path);
        jisho::populate_sqlite_schema(db);
    }
    else if (vm.count("sqlite")) {
        const std::string& db_path = vm["sqlite"].as<std::string>();
        auto db = jisho::sqlite::conn::make(db_path);
        jisho::write_sqlite(db, remove_missing(definitions));
    }

    if (!vm.count("no-stdout")) {
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
