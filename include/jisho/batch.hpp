#pragma once

#include <exception>
#include <optional>
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
std::vector<std::optional<definition>> fetch_batch(const std::vector<string_type>& words,
                                                   bool threads) {
    std::vector<std::optional<definition>> out(words.size());

    if (threads) {
        std::vector<std::exception_ptr> exc;
        std::vector<std::thread> threads;
        std::size_t ix = 0;
        for (const string_type& word : words) {
            threads.emplace_back([&out, ix, &word, &exc]() {
                try {
                    out[ix] = definition(curl::session{}, word);
                }
                catch (const jisho::error&) {
                    // ignore
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
    }
    else {
        curl::session session;
        std::size_t ix = 0;
        for (const string_type& word : words) {
            try {
                out[ix] = definition(session, word);
            }
            catch (const jisho::error&) {
                // ignore
            }
            ++ix;
        }
    }
    return out;
}
}  // namespace jisho
