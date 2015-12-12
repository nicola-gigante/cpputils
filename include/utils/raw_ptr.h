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

#ifndef CPPUTILS_RAW_PTR_H
#define CPPUTILS_RAW_PTR_H

#include "meta.h"

#include <utility>
#include <std14/type_traits>

namespace utils {
namespace details {
    
    template<typename T>
    class ptr_base
    {
    public:
        // Default constructor
        ptr_base() = default;
        
        // Constructor from raw pointers
        explicit ptr_base(T *ptr) noexcept : _ptr(ptr) { }
        ptr_base(std::nullptr_t) noexcept { }
        
        // Copy operations are defaulted
        ptr_base(ptr_base const&) = default;
        ptr_base &operator=(ptr_base const&) = default;

        // Moves are implemented in terms of swap(), unlike raw pointers
        // This helps the applicationo of the rule of zero
        ptr_base(ptr_base &&other) noexcept {
            std::swap(_ptr, other._ptr);
        }
        
        ptr_base &operator=(ptr_base &&other) noexcept {
            std::swap(_ptr, other._ptr);
            return *this;
        }
        
        bool operator==(ptr_base other) noexcept {
            return _ptr == other._ptr;
        }
        
        bool operator!=(ptr_base other) noexcept {
            return _ptr != other._ptr;
        }
        
        // Getter for the raw pointer
        T *get() const noexcept { return _ptr; }
        
        // Setter for the raw pointer
        void reset(T *ptr) noexcept { _ptr = ptr; }
        
        // Conversion to bool and logical operators
        explicit operator bool() const noexcept {
            return bool(_ptr);
        }
        
        bool operator!() const noexcept {
            return !_ptr;
        }
        
    protected:
        T *_ptr = nullptr;
    };
    
    // Derived class. Main template is for simple pointers
    template<typename T>
    class ptr : public ptr_base<T>
    {
        using base_t = ptr_base<T>;
        using base_t::_ptr;
        
        template<typename>
        friend class ptr;
        
    public:
        using base_t::base_t;
        
        ptr() = default;
        
        ptr(ptr const&) = default;
        ptr(ptr     &&) = default;
        
        ptr &operator=(ptr const&) = default;
        ptr &operator=(ptr     &&) = default;
        
        // The specialization for pointers to single elements have also a
        // conversion constructors and assignment operators from any
        // convertible pointer type
        template<typename U,
                 REQUIRES(std::is_convertible<U *, T *>())>
        ptr(ptr<U> const&other) : base_t(other._ptr) { }
        
        template<typename U>
        ptr(ptr<U> &&other) {
            std::swap(_ptr, other._ptr);
        }
        
        // We explicitly forbid arrays to be used to initialize this pointer
        template<unsigned N>
        ptr(T (&array)[N]) = delete;
        
        // Dereference operators
        T &operator*() const noexcept {
            return *_ptr;
        }
        
        T *operator->() const noexcept {
            return _ptr;
        }
    };
    
    // Specialization for pointers to arrays of unknown size
    template<typename T>
    class ptr<T[]> {
        using base_t = ptr_base<T>;
        using base_t::_ptr;
        
        template<typename>
        friend class ptr;
        
    public:
        using base_t::base_t;
        
        ptr(ptr const&) = default;
        ptr(ptr     &&) = default;
        
        ptr &operator=(ptr const&) = default;
        ptr &operator=(ptr     &&) = default;
        
        // The specialization for pointers to arrays allows only
        // non-const -> const conversions
        template<REQUIRES(neg(std::is_const<T>()))>
        ptr(ptr<const T> const&other) : base_t(other._ptr) { }
        
        template<REQUIRES(neg(std::is_const<T>()))>
        ptr(ptr<const T> &&other) {
            std::swap(_ptr, other._ptr);
        }
        
        // The array version adds an indexing operator 
        T &operator[](size_t i) const {
            return _ptr[i];
        }
    };
    
    // Specialization for pointers to arrays of known size: forbidden
    template<typename T, unsigned N>
    class ptr<T[N]>;
    
} // namespace details
    
using details::ptr;
    
} // namespace utils

#endif


















