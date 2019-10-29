#include "jisho/definition.hpp"

namespace jisho::sqlite {
std::shared_ptr<conn> conn::make(const std::string& filename) {
    sqlite3* underlying;
    if (sqlite3_open(filename.data(), &underlying) != SQLITE_OK) {
        throw std::runtime_error{"failed to open db"};
    }
    return std::make_shared<conn>(underlying);
}

std::shared_ptr<stmt> conn::statement(const std::string_view& sql) {
    if (sql.size() > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
        throw std::overflow_error{"sql statement size exceeds max int"};
    }
    const char* tail;
    sqlite3_stmt* underlying;
    int res =
        sqlite3_prepare_v3(m_underlying, sql.data(), sql.size(), 0, &underlying, &tail);
    if (res != SQLITE_OK) {
        std::stringstream ss;
        ss << "failed to prepare statement: " << sql
           << "\nreason: " << sqlite3_errstr(res);
        throw std::runtime_error{ss.str()};
    }

    return std::make_shared<stmt>(shared_from_this(), underlying);
}

void conn::exec(const std::string& sql) {
    char* err;
    if (sqlite3_exec(m_underlying, sql.data(), nullptr, nullptr, &err) != SQLITE_OK) {
        std::stringstream ss;
        ss << "failed to execute statement: " << sql << "\reason: " << err;
        sqlite3_free(err);
    }
}

void stmt::reset() {
    sqlite3_reset(m_underlying);
}

bool stmt::step() {
    int res = sqlite3_step(m_underlying);
    if (!(res == SQLITE_DONE || res == SQLITE_ROW)) {
        if (res == SQLITE_CONSTRAINT) {
            throw constraint_violated{};
        }

        std::stringstream ss;
        ss << "failed to step\nreason: " << sqlite3_errstr(res);
        throw std::runtime_error{ss.str()};
    }
    return res == SQLITE_ROW;
}
}  // namespace jisho::sqlite
