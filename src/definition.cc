#include "jisho/definition.hpp"

namespace jisho {
namespace {
std::string api_server_root = "https://jisho.org/";
}  // namespace

void set_api_root(const std::string_view& api_root) {
    api_server_root = api_root;
    if (api_server_root.back() != '/') {
        // ensure the canonical form of the api_server_root which ends in a slash
        api_server_root.push_back('/');
    }
}

pt::ptree definition::fetch_word(const curl::session& session,
                                 const std::string_view& word) {
    std::string url = api_server_root + "api/v1/search/words?keyword=";
    curl::string escaped = session.escape(word);
    url += escaped.get();
    std::stringstream raw(session.get(url));
    pt::ptree response;
    pt::read_json(raw, response);
    return response;
}

definition::definition(const pt::ptree& response) {
    pt::ptree data = response.get_child("data");
    if (!data.size()) {
        throw error("no data member in response");
    }
    pt::ptree entry = data.front().second;

    m_word = entry.get<std::string>("slug");
    m_is_common = entry.get<bool>("is_common");
    m_reading = entry.get<std::string>("japanese..reading");

    std::vector<std::string> pos;
    for (const auto& pt_sense : entry.get_child("senses")) {
        definition::sense& sense = m_senses.emplace_back();

        for (const auto& def : pt_sense.second.get_child("english_definitions")) {
            sense.add_def(def.second.get<std::string>(""));
        }

        pt::ptree pt_pos = pt_sense.second.get_child("parts_of_speech");
        if (pt_pos.size()) {
            for (const auto& pos : pt_pos) {
                sense.add_pos(pos.second.get<std::string>(""));
            }
            pos = sense.pos();
        }
        else {
            sense.pos(pos);
        }
    }
}

namespace {
std::ostream& write_quoted(std::ostream& stream, const std::string_view& view) {
    stream << '"';
    for (char c : view) {
        if (c == '"') {
            stream << '\\';
        }
        else if (c == '\\') {
            stream << '\\';
        }
        stream << c;
    }
    stream << '"';

    return stream;
}
}  // namespace

std::ostream& definition::csv_row(std::ostream& stream, char delim) const {
    write_quoted(stream, word()) << delim;
    write_quoted(stream, reading()) << delim;

    std::string flat_senses;
    for (const definition::sense& sense : senses()) {
        if (sense.pos().size()) {
            for (std::size_t ix = 0; ix < sense.pos().size() - 1; ++ix) {
                flat_senses += sense.pos()[ix];
                flat_senses += ", ";
            }
            flat_senses += sense.pos().back();
            flat_senses += ": ";
        }
        else {
            flat_senses = "<unknown>: ";
        }

        if (sense.def().size()) {
            for (std::size_t ix = 0; ix < sense.def().size() - 1; ++ix) {
                flat_senses += sense.def()[ix];
                flat_senses += "; ";
            }
            flat_senses += sense.def().back();
            flat_senses += "<br/>";
        }
        else {
            flat_senses += "<unknown><br/>";
        }
    }

    write_quoted(stream, flat_senses);
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const definition& definition) {
    stream << definition.word() << "(" << definition.reading() << ")\n";

    for (const definition::sense& sense : definition.senses()) {
        std::cout << "  ";
        if (sense.pos().size()) {
            for (std::size_t ix = 0; ix < sense.pos().size() - 1; ++ix) {
                stream << sense.pos()[ix] << ", ";
            }
            stream << sense.pos().back() << "\n";
        }
        else {
            stream << "<unknown>\n";
        }

        stream << "    ";
        if (sense.def().size()) {
            for (std::size_t ix = 0; ix < sense.def().size() - 1; ++ix) {
                stream << sense.def()[ix] << "; ";
            }
            stream << sense.def().back() << "\n";
        }
        else {
            stream << "<unknown>\n";
        }
    }

    return stream;
}

std::ostream& write_csv(std::ostream& stream,
                        const std::vector<definition>& definitions,
                        char field_delim) {
    for (const definition& definition : definitions) {
        definition.csv_row(stream, field_delim) << '\n';
    }
    return stream;
}
}  // namespace jisho
