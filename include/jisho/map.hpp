#include <type_traits>
#include <utility>

namespace jisho::util {
namespace detail {
template<typename T>
using remove_cvref = std::remove_cv_t<std::remove_reference_t<T>>;

template<typename F, typename T>
class mapper {
private:
    F m_func;
    T m_range;

    template<typename U>
    class generic_iterator {
    private:
        F m_func;
        U m_iterator;

    protected:
        friend class mapper;

        generic_iterator(F func, U iterator) : m_func(func), m_iterator(iterator) {}

    public:
        bool operator!=(const generic_iterator& other) const {
            return m_iterator != other.m_iterator;
        }

        bool operator==(const generic_iterator& other) const {
            return m_iterator == other.m_iterator;
        }

        generic_iterator& operator++() {
            ++m_iterator;
            return *this;
        }

        auto operator*() {
            return m_func(*m_iterator);
        }
    };

public:
    using iterator = generic_iterator<remove_cvref<decltype(std::declval<T>().begin())>>;
    using const_iterator =
        generic_iterator<remove_cvref<decltype(std::declval<const T>().begin())>>;

    mapper(F&& func, T&& range)
        : m_func(std::forward<F>(func)), m_range(std::forward<T>(range)) {}

    iterator begin() {
        return {m_func, m_range.begin()};
    }

    iterator end() {
        return {m_func, m_range.end()};
    }

    const_iterator begin() const {
        return {m_func, m_range.begin()};
    }

    const_iterator end() const {
        return {m_func, m_range.end()};
    }
};
}  // namespace detail

/** Create a range that lazily applies `f` to every element of `range`.

    @param range The range to apply the function to.
    @param f The function to apply.

    # Notes
    ```
    for (auto v : map(it, f)) {
        // ...
    }
    ```

    behaves the same as:

    ```
    for (auto& underlying : it) {
        auto v = f(underlying);
        // ...
    }
    ```
 */
template<typename T, typename F>
auto map(T&& range, F&& f) {
    return detail::mapper<F, T>(std::forward<F>(f), std::forward<T>(range));
}
}  // namespace jisho::util
