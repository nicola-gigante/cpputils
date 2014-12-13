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

#ifndef CPPUTILS_COMPRESSED_PAIR_H__
#define CPPUTILS_COMPRESSED_PAIR_H__

#include <type_traits>

namespace utils {
    
    namespace details {
        
        // The tag parameter is to allow compressed_pair<T, T>
        template<unsigned Tag, typename T, typename = void>
        class holder
        {
            T _value;
        public:
            // Default constructible, copiable, movable, etc...
            holder() = default;
            holder(holder const&) = default;
            holder(holder     &&) = default;
            
            holder &operator=(holder const&) = default;
            holder &operator=(holder     &&) = default;
            
            // Constructor
            holder(T const&value) : _value(value) { }
            
            // Getters
            T      &value()       { return _value; }
            T const&value() const { return _value; }
        };
        
        
        template<unsigned Tag, typename T>
        class holder<Tag, T,
            typename std::enable_if<std::is_empty<T>::value>::type>
            : private T
        {
        public:
            // Default constructible, copiable, movable, etc...
            holder() = default;
            holder(holder const&) = default;
            holder(holder     &&) = default;
            
            holder &operator=(holder const&) = default;
            holder &operator=(holder     &&) = default;

            // Constructor
            holder(T const&value) : T(value) { }
            
            // Getters
            T      &value()       { return static_cast<T      &>(*this); }
            T const&value() const { return static_cast<T const&>(*this); }
        };
        
        template<typename T1, typename T2>
        class compressed_pair : private holder<1, T1>, private holder<2, T2>
        {
        public:
            // Default constructible, copiable, movable, etc...
            compressed_pair() = default;
            compressed_pair(compressed_pair const&) = default;
            compressed_pair(compressed_pair     &&) = default;
            
            compressed_pair &operator=(compressed_pair const&) = default;
            compressed_pair &operator=(compressed_pair     &&) = default;
            
            compressed_pair(T1 const&v1, T2 const&v2)
                : holder<1, T1>(v1), holder<2, T2>(v2) { }
            
            T1      &first()        { return holder<1, T1>::value(); }
            T1 const&first()  const { return holder<1, T1>::value(); }
            
            T2      &second()       { return holder<2, T2>::value(); }
            T2 const&second() const { return holder<2, T2>::value(); }
        };
    }
    
    using details::compressed_pair;
}

#endif
