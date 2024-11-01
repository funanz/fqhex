# Copyright 2024 granz.fisherman@gmail.com
# https://opensource.org/license/mit

# Unit test settings
set(CHAR_TYPES char wchar_t char8_t char16_t char32_t)

set(CHAR_MODE_char 0)
set(CHAR_MODE_wchar_t 1632)
set(CHAR_MODE_char8_t 8)
set(CHAR_MODE_char16_t 16)
set(CHAR_MODE_char32_t 32)

# Unit test executable (Generic)
foreach(TYPE IN LISTS CHAR_TYPES)
  add_executable(fqhex-test-generic-${TYPE} fqhex_test.cpp)
  target_compile_features(fqhex-test-generic-${TYPE} PUBLIC cxx_std_20)
  target_compile_options(fqhex-test-generic-${TYPE} PRIVATE -DCHAR_MODE=${CHAR_MODE_${TYPE}})
  if (UNIX)
    target_compile_options(fqhex-test-generic-${TYPE} PRIVATE -Wall -D_GLIBCXX_DEBUG)
  endif()
  add_test(NAME generic-${TYPE} COMMAND fqhex-test-generic-${TYPE})
endforeach()

# Unit test executable (SSSE3)
foreach(TYPE IN LISTS CHAR_TYPES)
  add_executable(fqhex-test-ssse3-${TYPE} fqhex_test.cpp)
  target_compile_features(fqhex-test-ssse3-${TYPE} PUBLIC cxx_std_20)
  target_compile_options(fqhex-test-ssse3-${TYPE} PRIVATE -DCHAR_MODE=${CHAR_MODE_${TYPE}} -DHAVE_SSSE3=1)
  if (UNIX)
    target_compile_options(fqhex-test-ssse3-${TYPE} PRIVATE -Wall -D_GLIBCXX_DEBUG -mssse3)
  endif()
  add_test(NAME ssse3-${TYPE} COMMAND fqhex-test-ssse3-${TYPE})
endforeach()

# Unit test executable (AVX2)
foreach(TYPE IN LISTS CHAR_TYPES)
  add_executable(fqhex-test-avx2-${TYPE} fqhex_test.cpp)
  target_compile_features(fqhex-test-avx2-${TYPE} PUBLIC cxx_std_20)
  target_compile_options(fqhex-test-avx2-${TYPE} PRIVATE -DCHAR_MODE=${CHAR_MODE_${TYPE}} -DHAVE_AVX2=1)
  if (UNIX)
    target_compile_options(fqhex-test-avx2-${TYPE} PRIVATE -Wall -D_GLIBCXX_DEBUG -mavx2)
  endif()
  add_test(NAME avx2-${TYPE} COMMAND fqhex-test-avx2-${TYPE})
endforeach()
