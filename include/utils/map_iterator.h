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

#ifndef CPPUTILS_MAP_ITERATOR_H__
#define CPPUTILS_MAP_ITERATOR_H__

#include <std14/type_traits>
#include "meta.h"
#include "invoke.h"

#include <iterator>

namespace utils {
namespace details {
    
    /*
     * This class wraps any iterator by applying a given function to all the
     * elements of the range upon the invocation of the dereference operator
     */
    template<typename It, typename F>
    class map_iterator : private invokable_t<F>
    {
        It _it;
        
        using map_result_t = std14::result_of_t<F(typename std::iterator_traits<It>::reference)>;

        invokable_t<F>      &map()       { return *this; }
        invokable_t<F> const&map() const { return *this; }
        
    public:
        using value_type = std14::decay_t<map_result_t>;
        using pointer    = value_type *;
        using reference  = map_result_t;
        using difference_type   = typename std::iterator_traits<It>::difference_type;
        using iterator_category = typename std::iterator_traits<It>::iterator_category;
        
    public:
        map_iterator(It iterator, F map)
            : invokable_t<F>(map), _it(iterator) { }
        
        map_iterator(map_iterator const&) = default;
        map_iterator(map_iterator     &&) = default;
        
        map_iterator &operator=(map_iterator const&) = default;
        map_iterator &operator=(map_iterator     &&) = default;
        
        /*
         * The real work of this iterator type happens here
         */
        reference operator*() const {
            return map()(*_it);
        }
        
        map_iterator &operator++() {
            ++_it;
            return *this;
        }
        
        /*
         * InputIterator and ForwardIterator operations
         */
        bool operator==(map_iterator other) const {
            return _it == other._it;
        }
        
        bool operator!=(map_iterator other) const {
            return _it != other._it;
        }
        
        map_iterator operator++(int) const {
            map_iterator it = *this;
            ++_it;
            return it;
        }
        
        /*
         * BidirectionalIterator operations
         */
        template<REQUIRES(std::is_base_of<std::bidirectional_iterator_tag,
                                          iterator_category
                                         >::value)>
        map_iterator &operator--() {
            --_it;
            return *this;
        }
        
        template<REQUIRES(std::is_base_of<std::bidirectional_iterator_tag,
                                          iterator_category
                                         >::value)>
        map_iterator operator--(int) const {
            map_iterator it = *this;
            --_it;
            return it;
        }
        
        template<REQUIRES(std::is_base_of<std::random_access_iterator_tag,
                                          iterator_category
                                         >::value)>
        map_iterator &operator+=(difference_type n) {
            _it += n;
            return *this;
        }
        
        template<REQUIRES(std::is_base_of<std::random_access_iterator_tag,
                          iterator_category
                          >::value)>
        map_iterator operator+(difference_type n) const {
            map_iterator it = *this;
            it += n;
            return it;
        }
        
        template<REQUIRES(std::is_base_of<std::random_access_iterator_tag,
                                          iterator_category
                                         >::value)>
        friend
        map_iterator operator+(difference_type n, map_iterator const&it) {
            return it + n;
        }
        
        template<REQUIRES(std::is_base_of<std::random_access_iterator_tag,
                                          iterator_category
                                         >::value)>
        map_iterator &operator-=(difference_type n) {
            _it -= n;
            return *this;
        }
        
        template<REQUIRES(std::is_base_of<std::random_access_iterator_tag,
                                          iterator_category
                                         >::value)>
        map_iterator operator-(difference_type n) const {
            map_iterator it = *this;
            it -= n;
            return it;
        }
        
        template<REQUIRES(std::is_base_of<std::random_access_iterator_tag,
                                          iterator_category
                                         >::value)>
        difference_type operator-(map_iterator const&other) const {
            return _it - other._it;
        }
        
        template<REQUIRES(std::is_base_of<std::random_access_iterator_tag,
                                          iterator_category
                                         >::value)>
        reference operator[](difference_type n) const {
            return _it[n];
        }
        
        template<REQUIRES(std::is_base_of<std::random_access_iterator_tag,
                                          iterator_category
                                         >::value)>
        bool operator<(map_iterator const& it) const {
            return _it < it._it;
        }
        
        template<REQUIRES(std::is_base_of<std::random_access_iterator_tag,
                          iterator_category
                          >::value)>
        bool operator>(map_iterator const& it) const {
            return _it > it._it;
        }
        
        template<REQUIRES(std::is_base_of<std::random_access_iterator_tag,
                          iterator_category
                          >::value)>
        bool operator<=(map_iterator const& it) const {
            return _it <= it._it;
        }
        
        template<REQUIRES(std::is_base_of<std::random_access_iterator_tag,
                          iterator_category
                          >::value)>
        bool operator>=(map_iterator const& it) const {
            return _it >= it._it;
        }
        
    private:
        
    };
    
    /*
     * This is the function used to create a map_iterator deducing the arguments
     */
    template<typename It, typename F>
    map_iterator<It, F> make_map_iterator(It it, F f) {
        return map_iterator<It, F>(it, f);
    }
    
} // namespace details

using details::map_iterator;
using details::make_map_iterator;
    
} // namespace utils

#endif
