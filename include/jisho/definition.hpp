#pragma once

#include <ostream>
#include <string_view>
#include <vector>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "jisho/curl.hpp"

namespace jisho {
namespace pt = boost::property_tree;

extern std::string jisho_api;

enum class jlpt : char {
    n5 = (1 << 5),
    n4 = (1 << 4),
    n3 = (1 << 3),
    n2 = (1 << 2),
    n1 = (1 << 1),
};

class error : public std::runtime_error {
public:
    explicit error(const std::string& message) : std::runtime_error(message) {}
};

class definition {
public:
    class sense {
    private:
        std::vector<std::string> m_pos;
        std::vector<std::string> m_def;

    protected:
        friend definition;

        inline void add_pos(const std::string_view& pos) {
            m_pos.emplace_back(pos);
        }

        inline void add_def(const std::string_view& def) {
            m_def.emplace_back(def);
        }

        inline void pos(const std::vector<std::string>& pos) {
            m_pos = pos;
        }

    public:
        inline const std::vector<std::string> pos() const {
            return m_pos;
        }

        inline const std::vector<std::string> def() const {
            return m_def;
        }
    };

    definition() = default;
    definition(const pt::ptree& api_result);

private:
    static pt::ptree fetch_word(const curl::session& session,
                                const std::string_view& word);

public:
    /** Fetch a word definition from jisho.

        @param session The curl session to make the request with.
        @word The word to query for.
    */
    inline definition(const curl::session& session, const std::string_view& word)
        : definition(fetch_word(session, word)) {}

    inline const std::string& word() const {
        return m_word;
    }

    inline const std::string& reading() const {
        return m_reading;
    }

    inline const std::vector<sense>& senses() const {
        return m_senses;
    }

    inline int jlpt_mask() const {
        return m_jlpt_mask;
    }

    inline bool n5() const {
        return m_jlpt_mask & static_cast<char>(jlpt::n5);
    }

    inline bool n4() const {
        return m_jlpt_mask & static_cast<char>(jlpt::n4);
    }

    inline bool n3() const {
        return m_jlpt_mask & static_cast<char>(jlpt::n3);
    }

    inline bool n2() const {
        return m_jlpt_mask & static_cast<char>(jlpt::n2);
    }

    inline bool n1() const {
        return m_jlpt_mask & static_cast<char>(jlpt::n1);
    }

    inline bool is_common() const {
        return m_is_common;
    }

private:
    std::string m_word;
    std::string m_reading;
    std::vector<sense> m_senses;
    char m_jlpt_mask;
    bool m_is_common;
};

std::ostream& operator<<(std::ostream& stream, const definition& definition);
}  // namespace jisho
