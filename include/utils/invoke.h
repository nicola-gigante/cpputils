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

#ifndef CPPUTILS_INVOKE_H__
#define CPPUTILS_INVOKE_H__

#include "meta.h"

#include <std14/utility>
#include <type_traits>


namespace utils {
namespace details {

    /*
     * The invoke function comes at hand to call functions, function pointers,
     * pointers to members and pointers to member functions all with a uniform
     * syntax.
     */
    
    /*
     * Easy macro to use the decltype/return pair without repetitions
     */
    #define declreturn(...) \
    -> decltype(__VA_ARGS__) { \
        return (__VA_ARGS__); \
    }
    
    /*
     * Generic version
     */
    template<typename F, typename ...Args>
    auto invoke(F&& f, Args&& ...args)
        declreturn( std::forward<F>(f)(std::forward<Args>(args)...) )
    
    /*
     * Pointers to member functions
     */
    template<typename R, typename C, typename Obj,
             typename ...FArgs, typename ...Args>
    auto invoke(R(C::*f)(FArgs...), Obj&& obj, Args&& ...args)
        declreturn( (std::forward<Obj>(obj).*f)(std::forward<Args>(args)...) )
    
    /*
     * Pointers to member const functions
     */
    template<typename R, typename C, typename Obj,
    typename ...FArgs, typename ...Args>
    auto invoke(R(C::*f)(FArgs...) const, Obj&& obj, Args&& ...args)
        declreturn( (std::forward<Obj>(obj).*f)(std::forward<Args>(args)...) )
    
    /*
     * Pointers to member functions, callable with a pointer to the instance
     */
    template<typename R, typename C, typename Obj,
             typename ...FArgs, typename ...Args>
    auto invoke(R(C::*f)(FArgs...), Obj&& obj, Args&& ...args)
        declreturn( ((*std::forward<Obj>(obj)).*f)(std::forward<Args>(args)...))
    
    /*
     * Pointers to member const functions, callable with a pointer to 
     * the instance
     */
    template<typename R, typename C, typename Obj,
    typename ...FArgs, typename ...Args>
    auto invoke(R(C::*f)(FArgs...) const, Obj&& obj, Args&& ...args)
        declreturn( ((*std::forward<Obj>(obj)).*f)(std::forward<Args>(args)...))
    
    /*
     * Pointers to members
     */
    template<typename T, typename C, typename Obj>
    auto invoke(T (C::*m), Obj&& obj)
        declreturn( std::forward<Obj>(obj).*m )
        
    /*
     * Pointers to members
     */
    template<typename T, typename C, typename Obj>
    auto invoke(T (C::*m), Obj* obj)
        declreturn( obj->*m )
    
    
    template<typename T>
    class holder {
        T _obj;
        
    public:
        holder(T t) : _obj(t) { }
        
        template<typename ...Args>
        auto operator()(Args&& ...args) const
            declreturn( invoke(_obj, std::forward<Args>(args)...) )

        template<typename ...Args>
        auto operator()(Args&& ...args)
            declreturn( invoke(_obj, std::forward<Args>(args)...) )
    };

    template<typename T>
    class invokable_type {
        using U = typename std::decay<T>::type;
        
    public:
        using type = typename
                     std::conditional<std::is_class<U>::value,
                                      U,
                                      holder<U>
                                     >::type;
    };
    
    template<typename T>
    using invokable_t = typename invokable_type<T>::type;
    
    /*
     * The invokable function wraps anything that is invokable by invoke()
     * into a function object, paying attention to preserve the emptyness of
     * empty function objects (like no-capture lambdas).
     */
    template<typename T>
    auto invokable(T&& t)
        declreturn( invokable_t<T>( std::forward<T>(t) ) )
    
    
    /*
     * The apply function does the same things as invoke() but accepts arguments
     * from an std::tuple
     */
    template<typename F, typename Tuple, size_t ...Idx>
    auto apply_impl(F&& f, Tuple&& tuple, std14::index_sequence<Idx...>)
        declreturn(invoke(std::forward<F>(f),
                          std::get<Idx>(std::forward<Tuple>(tuple))...))
    
    template<typename F, typename Tuple>
    auto apply(F&& f, Tuple&& tuple)
        declreturn(apply_impl(std::forward<F>(f),
                              std::forward<Tuple>(tuple),
                              std14::make_index_sequence<
                                std::tuple_size<Tuple>::value
                              >()))
    
    #undef declreturn
    
} // namespace details
    
using details::invokable;
using details::invokable_t;
using details::invoke;
using details::apply;
    
} // namespace utils

#endif
