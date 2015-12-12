//===-- llvm/Support/Compiler.h - Compiler abstraction support --*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef CPPUTILS_SUPPORT_H
#define CPPUTILS_SUPPORT_H

#include <cstdio>
#include <cstdlib>

/*
 * These are compiler compatibility macros, imported from LLVM Compiler.h
 */

#ifndef __has_feature
# define __has_feature(x) 0
#endif

#ifndef __has_extension
# define __has_extension(x) 0
#endif

#ifndef __has_attribute
# define __has_attribute(x) 0
#endif

#ifndef __has_builtin
# define __has_builtin(x) 0
#endif

/*
 * UTILS_GNUC_PREREQ(x, y, z) expands to true if we're on GCC and the version
 * is at least x.y.z
 */
#ifndef UTILS_GNUC_PREREQ
# if defined(__GNUC__) && defined(__GNUC_MINOR__) && defined(__GNUC_PATCHLEVEL__)
#  define UTILS_GNUC_PREREQ(maj, min, patch) \
     ((__GNUC__ << 20) + (__GNUC_MINOR__ << 10) + __GNUC_PATCHLEVEL__ >= \
        ((maj) << 20) + ((min) << 10) + (patch))
# elif defined(__GNUC__) && defined(__GNUC_MINOR__)
#  define UTILS_GNUC_PREREQ(maj, min, patch) \
     ((__GNUC__ << 20) + (__GNUC_MINOR__ << 10) >= ((maj) << 20) + ((min) << 10))
# else
#  define UTILS_GNUC_PREREQ(maj, min, patch) 0
# endif
#endif

#if __has_builtin(__builtin_unreachable) || UTILS_GNUC_PREREQ(4, 5, 0)
# define UTILS_BUILTIN_UNREACHABLE __builtin_unreachable()
#endif

/*
 * code_unreachable() marks a given execution path as unreachable
 */
#ifdef UTILS_BUILTIN_UNREACHABLE
#  define code_unreachable() UTILS_BUILTIN_UNREACHABLE
#else
#  define code_unreachable() ::utils::details::unreachable(__FILE__, __LINE__, __func__);
#endif

namespace utils {
namespace details {
    
    [[noreturn]]
    inline
    void unreachable(const char *file, unsigned line, const char *func) {
        fprintf(stderr,
                "Execution reached code_unreachable() at %s:%d in function %s",
                file, line, func);
        abort();
    }
    
} // namespace details
} // namespace utils

#endif
