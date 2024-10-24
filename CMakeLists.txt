# Copyright 2024 granz.fisherman@gmail.com
# https://opensource.org/license/mit

cmake_minimum_required (VERSION 3.16)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "RelWithDebinfo" CACHE STRING "Default build type")
endif ()

project(fqhex)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# hex-perf-test
add_executable(fqhex-perf-test perf/fqhex_perf_test.cpp)
target_compile_features(fqhex-perf-test PUBLIC cxx_std_20)
if (UNIX)
  target_compile_options(fqhex-perf-test PRIVATE -Wall -mavx2)
endif()

# fqhex-test-char
add_executable(fqhex-test-char test/fqhex_test.cpp)
target_compile_features(fqhex-test-char PUBLIC cxx_std_20)
if (UNIX)
  target_compile_options(fqhex-test-char PRIVATE -Wall -D_GLIBCXX_DEBUG)
endif()

# fqhex-test-wchar
add_executable(fqhex-test-wchar test/fqhex_test.cpp)
target_compile_features(fqhex-test-wchar PUBLIC cxx_std_20)
target_compile_options(fqhex-test-wchar PRIVATE -DCHAR_MODE=1632)
if (UNIX)
  target_compile_options(fqhex-test-wchar PRIVATE -Wall -D_GLIBCXX_DEBUG)
endif()

# fqhex-test-char8
add_executable(fqhex-test-char8 test/fqhex_test.cpp)
target_compile_features(fqhex-test-char8 PUBLIC cxx_std_20)
target_compile_options(fqhex-test-char8 PRIVATE -DCHAR_MODE=8)
if (UNIX)
  target_compile_options(fqhex-test-char8 PRIVATE -Wall -D_GLIBCXX_DEBUG)
endif()

# fqhex-test-char16
add_executable(fqhex-test-char16 test/fqhex_test.cpp)
target_compile_features(fqhex-test-char16 PUBLIC cxx_std_20)
target_compile_options(fqhex-test-char16 PRIVATE -DCHAR_MODE=16)
if (UNIX)
  target_compile_options(fqhex-test-char16 PRIVATE -Wall -D_GLIBCXX_DEBUG)
endif()

# fqhex-test-char32
add_executable(fqhex-test-char32 test/fqhex_test.cpp)
target_compile_features(fqhex-test-char32 PUBLIC cxx_std_20)
target_compile_options(fqhex-test-char32 PRIVATE -DCHAR_MODE=32)
if (UNIX)
  target_compile_options(fqhex-test-char32 PRIVATE -Wall -D_GLIBCXX_DEBUG)
endif()

# fqhex-test-ssse3-char
add_executable(fqhex-test-ssse3-char test/fqhex_test.cpp)
target_compile_features(fqhex-test-ssse3-char PUBLIC cxx_std_20)
target_compile_options(fqhex-test-ssse3-char PRIVATE -DHAVE_SSSE3=8)
if (UNIX)
  target_compile_options(fqhex-test-ssse3-char PRIVATE -Wall -D_GLIBCXX_DEBUG -mssse3)
endif()

# fqhex-test-ssse3-char8
add_executable(fqhex-test-ssse3-char8 test/fqhex_test.cpp)
target_compile_features(fqhex-test-ssse3-char8 PUBLIC cxx_std_20)
target_compile_options(fqhex-test-ssse3-char8 PRIVATE -DCHAR_MODE=8 -DHAVE_SSSE3=8)
if (UNIX)
  target_compile_options(fqhex-test-ssse3-char8 PRIVATE -Wall -D_GLIBCXX_DEBUG -mssse3)
endif()

# fqhex-test-ssse3-char16
add_executable(fqhex-test-ssse3-char16 test/fqhex_test.cpp)
target_compile_features(fqhex-test-ssse3-char16 PUBLIC cxx_std_20)
target_compile_options(fqhex-test-ssse3-char16 PRIVATE -DCHAR_MODE=16 -DHAVE_SSSE3=16)
if (UNIX)
  target_compile_options(fqhex-test-ssse3-char16 PRIVATE -Wall -D_GLIBCXX_DEBUG -mssse3)
endif()

# fqhex-test-ssse3-char32
add_executable(fqhex-test-ssse3-char32 test/fqhex_test.cpp)
target_compile_features(fqhex-test-ssse3-char32 PUBLIC cxx_std_20)
target_compile_options(fqhex-test-ssse3-char32 PRIVATE -DCHAR_MODE=32 -DHAVE_SSSE3=32)
if (UNIX)
  target_compile_options(fqhex-test-ssse3-char32 PRIVATE -Wall -D_GLIBCXX_DEBUG -mssse3)
endif()

# fqhex-test-avx2-char
add_executable(fqhex-test-avx2-char test/fqhex_test.cpp)
target_compile_features(fqhex-test-avx2-char PUBLIC cxx_std_20)
target_compile_options(fqhex-test-avx2-char PRIVATE -DHAVE_AVX2=8)
if (UNIX)
  target_compile_options(fqhex-test-avx2-char PRIVATE -Wall -D_GLIBCXX_DEBUG -mavx2)
endif()

# fqhex-test-avx2-char8
add_executable(fqhex-test-avx2-char8 test/fqhex_test.cpp)
target_compile_features(fqhex-test-avx2-char8 PUBLIC cxx_std_20)
target_compile_options(fqhex-test-avx2-char8 PRIVATE -DCHAR_MODE=8 -DHAVE_AVX2=8)
if (UNIX)
  target_compile_options(fqhex-test-avx2-char8 PRIVATE -Wall -D_GLIBCXX_DEBUG -mavx2)
endif()

# fqhex-test-avx2-char16
add_executable(fqhex-test-avx2-char16 test/fqhex_test.cpp)
target_compile_features(fqhex-test-avx2-char16 PUBLIC cxx_std_20)
target_compile_options(fqhex-test-avx2-char16 PRIVATE -DCHAR_MODE=16 -DHAVE_AVX2=16)
if (UNIX)
  target_compile_options(fqhex-test-avx2-char16 PRIVATE -Wall -D_GLIBCXX_DEBUG -mavx2)
endif()

# fqhex-test-avx2-char32
add_executable(fqhex-test-avx2-char32 test/fqhex_test.cpp)
target_compile_features(fqhex-test-avx2-char32 PUBLIC cxx_std_20)
target_compile_options(fqhex-test-avx2-char32 PRIVATE -DCHAR_MODE=32 -DHAVE_AVX2=32)
if (UNIX)
  target_compile_options(fqhex-test-avx2-char32 PRIVATE -Wall -D_GLIBCXX_DEBUG -mavx2)
endif()

enable_testing()
add_test(NAME generic-char COMMAND fqhex-test-char)
add_test(NAME generic-wchar_t COMMAND fqhex-test-wchar)
add_test(NAME generic-char8_t COMMAND fqhex-test-char8)
add_test(NAME generic-char16_t COMMAND fqhex-test-char16)
add_test(NAME generic-char32_t COMMAND fqhex-test-char32)
add_test(NAME ssse3-char COMMAND fqhex-test-ssse3-char)
add_test(NAME ssse3-char8_t COMMAND fqhex-test-ssse3-char8)
add_test(NAME ssse3-char16_t COMMAND fqhex-test-ssse3-char16)
add_test(NAME ssse3-char32_t COMMAND fqhex-test-ssse3-char32)
add_test(NAME avx2-char COMMAND fqhex-test-avx2-char)
add_test(NAME avx2-char8_t COMMAND fqhex-test-avx2-char8)
add_test(NAME avx2-char16_t COMMAND fqhex-test-avx2-char16)
add_test(NAME avx2-char32_t COMMAND fqhex-test-avx2-char32)
