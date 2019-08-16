#pragma once

#include <cstring>
#include <memory>

#include <sqlite3.h>

namespace jisho::sqlite {
/** A managed sqlite3 statement.
 */
class stmt;

/** A managed sqlite3 connection.
 */
class conn : public std::enable_shared_from_this<conn> {
public:
    inline conn(sqlite3* underlying) : m_underlying(underlying) {}

    /** Open a new sqlite database.

        @param filename The path the file to open/create.
     */
    static std::shared_ptr<conn> make(const std::string& filename);

    /** Prepare a sql statement which can be bound or iterated.
     */
    std::shared_ptr<stmt> statement(const std::string_view& sql);

    /** Execute a sql statement.
     */
    void exec(const std::string& sql);

private:
    sqlite3* m_underlying;
};

namespace detail {
template<typename T>
struct bind_helper;

template<>
struct bind_helper<std::string> {
private:
    static void delete_string(void* p) {
        delete[] reinterpret_cast<char*>(p);
    }

public:
    static int f(sqlite3_stmt* underlying, int col, const std::string& data) {
        char* c_data = new char[data.size()];
        std::memcpy(c_data, data.data(), data.size());
        return sqlite3_bind_text(underlying, col, c_data, data.size(), delete_string);
    }
};

template<>
struct bind_helper<double> {
    static int f(sqlite3_stmt* underlying, int col, double d) {
        return sqlite3_bind_double(underlying, col, d);
    }
};

template<>
struct bind_helper<int> {
    static int f(sqlite3_stmt* underlying, int col, int i) {
        return sqlite3_bind_int(underlying, col, i);
    }
};

template<>
struct bind_helper<std::int64_t> {
    static int f(sqlite3_stmt* underlying, int col, std::int64_t i) {
        return sqlite3_bind_int64(underlying, col, i);
    }
};

template<>
struct bind_helper<bool> {
    static int f(sqlite3_stmt* underlying, int col, bool b) {
        return sqlite3_bind_int(underlying, col, b);
    }
};
}  // namespace detail

class constraint_violated : public std::runtime_error {
public:
    inline constraint_violated() : std::runtime_error("constraint violated") {}
};

/** A managed sqlite3 statement.
 */
class stmt : public std::enable_shared_from_this<stmt> {
public:
    inline stmt(const std::shared_ptr<conn>& db, sqlite3_stmt* underlying)
        : m_db(db), m_underlying(underlying) {}

    inline ~stmt() {
        sqlite3_finalize(m_underlying);
    }

    /** Reset a prepared statement so that it may be used again.
     */
    void reset();

    /** Step the statement once.

        @return Are there more rows in the result?
     */
    bool step();

    /** Bind a variable to a bind-parameter in the sqlite statement.

        @param col The 1-indexed column index.
        @param value The value to bind.
     */
    template<typename T>
    void bind(int col, const T& value) {
        if (detail::bind_helper<T>::f(m_underlying, col, value) != SQLITE_OK) {
            throw std::runtime_error{"failed to bind parameter"};
        }
    }

private:
    std::shared_ptr<conn> m_db;
    sqlite3_stmt* m_underlying;
};
}  // namespace jisho::sqlite
