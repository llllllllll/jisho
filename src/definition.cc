#include "jisho/definition.hpp"
#include "jisho/map.hpp"

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

namespace {
template<typename T, typename C>
std::ostream& intersperse(std::ostream& stream, const T& sep, const C& container) {
    auto it = container.begin();
    auto end = container.end();
    if (it == end) {
        return stream;
    }

    stream << *it;
    ++it;
    for (; it != end; ++it) {
        stream << sep << *it;
    }
    return stream;
}

std::array<std::pair<jlpt, std::string_view>, 5> jlpt_parse_strings = {{
    {jlpt::n5, "jlpt-n5"},
    {jlpt::n4, "jlpt-n4"},
    {jlpt::n3, "jlpt-n3"},
    {jlpt::n2, "jlpt-n2"},
    {jlpt::n1, "jlpt-n1"},
}};

std::uint8_t parse_jlpt_mask(const pt::ptree& tags) {
    std::uint8_t out = 0;
    for (const auto& [_, entry] : tags) {
        const std::string& as_str = entry.get<std::string>("");

        bool parsed = false;
        for (const auto& [mask, parse_str] : jlpt_parse_strings) {
            if (as_str == parse_str) {
                out |= static_cast<std::uint8_t>(mask);
                parsed = true;
            }
        }
        if (!parsed) {
            std::stringstream ss;
            ss << "unknown jlpt level string " << as_str << ", valid options are: "
               << "{";
            intersperse(ss, ", ", util::map(jlpt_parse_strings, [](const auto& p) {
                            return p.second;
                        }));
            ss << '}';
            throw std::runtime_error(ss.str());
        }
    }
    return out;
}

std::string usually_written_using_kana_alone = "Usually written using kana alone";

bool parse_usually_written_using_kana_alone(const pt::ptree& tags) {
    for (const auto& [_, entry] : tags) {
        const std::string& as_str = entry.get<std::string>("");

        if (as_str == usually_written_using_kana_alone) {
            return true;
        }
    }
    return false;
}
}  // namespace

definition::definition(const pt::ptree& response) {
    pt::ptree data = response.get_child("data");
    if (!data.size()) {
        throw error("no data member in response");
    }
    pt::ptree entry = data.front().second;

    m_word = entry.get<std::string>("slug");
    m_is_common = entry.get<bool>("is_common");
    m_reading = entry.get<std::string>("japanese..reading");
    m_jlpt_mask = parse_jlpt_mask(entry.get_child("jlpt"));

    std::vector<std::string> pos;
    for (const auto& pt_sense : entry.get_child("senses")) {
        definition::sense& sense = m_senses.emplace_back(
            parse_usually_written_using_kana_alone(pt_sense.second.get_child("tags")));

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

namespace {
std::array<std::pair<jlpt, std::string_view>, 5> jlpt_formats = {{{jlpt::n5, "n5"},
                                                                  {jlpt::n4, "n4"},
                                                                  {jlpt::n3, "n3"},
                                                                  {jlpt::n2, "n2"},
                                                                  {jlpt::n1, "n1"}}};
}

std::ostream& operator<<(std::ostream& stream, const definition& definition) {
    stream << definition.word() << '(' << definition.reading() << ')';
    if (definition.jlpt_mask()) {
        std::vector<std::string_view> strings;
        for (const auto& [level, str] : jlpt_formats) {
            if (definition.jlpt_mask() & static_cast<std::uint8_t>(level)) {
                strings.emplace_back(str);
            }
        }
        stream << "; jlpt={";
        intersperse(stream, ", ", strings);
        stream << '}';
    }
    if (definition.is_common()) {
        stream << "; common";
    }

    for (const definition::sense& sense : definition.senses()) {
        stream << "\n  ";
        if (sense.pos().size()) {
            intersperse(stream, ", ", sense.pos());
        }
        else {
            stream << "<unknown>";
        }
        if (sense.usually_written_using_kana_alone()) {
            stream << " (usually written using kana alone)";
        }
        stream << '\n';

        stream << "    ";
        if (sense.def().size()) {
            intersperse(stream, "; ", sense.def());
        }
        else {
            stream << "<unknown>";
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

namespace {
template<typename C>
void make_table(const std::shared_ptr<sqlite::conn>& db,
                const std::string_view& name,
                const C& columns) {
    std::stringstream ss;
    ss << "create table " << name << " (";
    intersperse(ss, ", ", util::map(columns, [](const auto& p) {
                    std::string col(p[0]);
                    col += ' ';
                    col += p[1];
                    col += p[2];
                    return col;
                }));
    ss << ')';
    db->exec(ss.str());
}
}  // namespace

void populate_sqlite_schema(const std::shared_ptr<sqlite::conn>& db) {
    std::array<std::array<std::string_view, 3>, 4> words_columns = {{
        {"word", "text", " primary key"},
        {"reading", "text", ""},
        {"jlpt_mask", "integer", ""},
        {"is_common", "boolean", ""},
    }};
    make_table(db, "words", words_columns);

    std::array<std::array<std::string_view, 3>, 4> senses_columns = {{
        {"word", "text"},
        {"pos", "text"},
        {"def", "text"},
        {"usually_written_using_kana_alone", "boolean"},
    }};
    make_table(db, "senses", senses_columns);
}

void write_sqlite(const std::shared_ptr<sqlite::conn>& db,
                  const std::vector<definition>& definitions) {
    auto words_statement = db->statement("insert into words values (?, ?, ?, ?)");
    auto senses_statement = db->statement("insert into senses values (?, ?, ?, ?)");

    for (const definition& def : definitions) {
        words_statement->reset();
        words_statement->bind(1, def.word());
        words_statement->bind(2, def.reading());
        words_statement->bind<int>(3, def.jlpt_mask());
        words_statement->bind(4, def.is_common());
        try {
            words_statement->step();
        }
        catch (const sqlite::constraint_violated&) {
            continue;
        }

        for (const definition::sense& sense : def.senses()) {
            senses_statement->reset();
            senses_statement->bind(1, def.word());
            std::stringstream pos;
            intersperse(pos, ", ", sense.pos());
            senses_statement->bind(2, pos.str());
            std::stringstream def;
            intersperse(def, "; ", sense.def());
            senses_statement->bind(3, def.str());
            senses_statement->bind(4, sense.usually_written_using_kana_alone());
            senses_statement->step();
        }
    }
}
}  // namespace jisho
