// Copyright (C) 2026 atyxeut
//
// This library is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

// clang-format off

#if defined(_MSC_VER)
  #define FMIA_WCONVERSION_PUSH() __pragma(warning(push))          \
                                  __pragma(warning(disable: 4018)) \
                                  __pragma(warning(disable: 4267)) \
                                  __pragma(warning(disable: 4389))

  #define FMIA_WCONVERSION_POP() __pragma(warning(pop))
#elif defined(__GNUC__) || defined (__clang__)
  #define FMIA_WCONVERSION_PUSH() _Pragma("GCC diagnostic push")                          \
                                  _Pragma("GCC diagnostic ignored \"-Wsign-conversion\"") \
                                  _Pragma("GCC diagnostic ignored \"-Wsign-compare\"")    \
                                  _Pragma("GCC diagnostic ignored \"-Wconversion\"")

  #define FMIA_WCONVERSION_POP() _Pragma("GCC diagnostic pop")
#else
  #define FMIA_WCONVERSION_PUSH()
  #define FMIA_WCONVERSION_POP()
#endif

// clang-format on
