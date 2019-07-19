#pragma once

#include <exception>
#include <thread>
#include <vector>

#include "jisho/curl.hpp"
#include "jisho/definition.hpp"

namespace jisho {
/** Fetch multiple words in batch from jisho.

    @param words The container of strings or string views to fetch.
    @return The definition for each word in the order it appears in `words`.
 */
template<typename string_type>
std::vector<definition> fetch_batch(const std::vector<string_type>& words) {
    std::vector<definition> out(words.size());

    std::vector<std::exception_ptr> exc;
    std::vector<std::thread> threads;
    std::size_t ix = 0;
    for (const string_type& word : words) {
        threads.emplace_back([&out, ix, &word, &exc]() {
            try {
                out[ix] = definition(curl::session{}, word);
            }
            catch (const std::exception&) {
                exc.emplace_back(std::current_exception());
            }
        });
        ++ix;
    }

    for (std::thread& thread : threads) {
        thread.join();
    }
    for (const std::exception_ptr& e : exc) {
        std::rethrow_exception(e);
    }
    return out;
}
}  // namespace jisho
