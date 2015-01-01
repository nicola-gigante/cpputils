/*
 * Copyright 2014 Nicola Gigante
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CPPUTILS_STRING_SWITCH_H__
#define CPPUTILS_STRING_SWITCH_H__

#include <cstdint>
#include <std14/experimental/string_view>

/*
 * This header declares a little facility to be able to make switch statements
 * on strings.
 * Code implements a constexpr version of the FNV-1a hashing algorithm, which
 * should be good enough for this use case. Of course don't use this if a
 * collision would cause a security problem.
 *
 * The code is inspired by https://github.com/elanthis/constexpr-hash-demo
 *
 * Note that I've not included a different runtime version of the function,
 * since it's not needed. The recursive version is clean enough, and by being
 * tail-recursive, any optimizing compiler would return turn it into the
 * iterative version anyway.
 *
 * Note also that the function accepts a string_view, so you can use string
 * literals or std::string or whatever.
 *
 * You can use the str_switch() function or the ""_match user defined literal at you
 * choice.
 */

namespace utils {
namespace details
{
    using std14::experimental::string_view;
    
    // FNV-1a constants
    static constexpr uint64_t basis = 14695981039346656037ULL;
    static constexpr uint64_t prime = 1099511628211ULL;
    
    // hash function
    constexpr uint64_t hash(const char *str, size_t size, uint64_t value)
    {
        return size == 0 ? value : hash(str + 1, size - 1,
                                        (value ^ *str) * prime);
    }
    
    constexpr uint64_t hash(const char *str, size_t size) {
        return hash(str, size, basis);
    }
    
    constexpr size_t cstrlen(const char *str) {
        return *str == 0 ? 0 : 1 + cstrlen(str + 1);
    }
    
    /*
     * Constexpr version for string literals
     */
    constexpr uint64_t str_switch(const char *str) {
        return hash(str, cstrlen(str));
    }
    
    /*
     * General flexible version
     */
    uint64_t str_switch(string_view str) {
        return hash(str.data(), str.size());
    }
    
    namespace literals {
        constexpr uint64_t operator ""_match(const char *str, size_t len) {
            return hash(str, len);
        }
    }
} // namespace details

using details::str_switch;
namespace literals = details::literals;
    
} // namespace utils



#endif
