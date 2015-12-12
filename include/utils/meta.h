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

#ifndef CPPUTILS_META_H
#define CPPUTILS_META_H

#include <type_traits>

#define CPPUTILS_CONCAT__(x, y) CPPUTILS_CONCAT_2__(x,y)
#define CPPUTILS_CONCAT_2__(x, y) x ## y

#define CPPUTILS_REQUIRES_FRESH CPPUTILS_CONCAT__(UNFULFILLED_TEMPLATE_REQUIREMENT_, __LINE__)


// Note: this must stay on the same line.
#define REQUIRES(...) \
typename CPPUTILS_REQUIRES_FRESH = void, typename std::enable_if<::utils::details::true_t<CPPUTILS_REQUIRES_FRESH>::value && ::utils::details::all(__VA_ARGS__), int>::type = 0

namespace utils {
    
    namespace details
    {
        template<typename T>
        struct true_t : std::true_type { };
        
        template<typename T>
        using void_t = void;

        template<typename T, bool Tb = T::value>
        constexpr bool metapredicate() {
            return Tb;
        }
        
        template<bool B>
        constexpr bool metapredicate() {
            return B;
        }
        
        constexpr bool all() { return true; }
        
        template<typename ...Args>
        constexpr
        bool all(bool b, Args ...args)
        {
            return b && all(args...);
        }
        
        template<typename T, typename ...Args, bool Tb = T::value>
        constexpr
        bool all(T, Args ...args)
        {
            return Tb && all(args...);
        }
        
        template<typename T, typename ...Args>
        constexpr bool same_type() {
            return all(std::is_same<T, Args>()...);
        }
        
        constexpr bool neg(bool b) { return not b; }
        
        template<typename T, bool Tb = T::value>
        constexpr bool neg(T) { return not Tb; }
        
        template<typename ...Args>
        constexpr bool any(Args ...args) {
            return not all(neg(args)...);
        }
    }
    
    using details::true_t;
    using details::void_t;
    
    using details::neg;
    using details::all;
    using details::any;
    
    using details::same_type;
}

#endif
