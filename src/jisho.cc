#include "jisho/jisho.hpp"

namespace jisho {
std::string jisho_api = "https://jisho.org/";

pt::ptree definition::fetch_word(const curl::session& session,
                                 const std::string_view& word) {
    std::string url = jisho_api + "api/v1/search/words?keyword=";
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
            stream << "(unknown)\n";
        }

        stream << "    ";
        if (sense.def().size()) {
            for (std::size_t ix = 0; ix < sense.def().size() - 1; ++ix) {
                stream << sense.def()[ix] << ';';
            }
            stream << sense.def().back() << "\n";
        }
        else {
            stream << "(unknown)\n";
        }
    }

    return stream;
}
}  // namespace jisho
