# Copyright 2024 granz.fisherman@gmail.com
# https://opensource.org/license/mit

# Performance test executable (Generic)
add_executable(fqhex-perf-test-generic fqhex_perf_test.cpp)
target_compile_features(fqhex-perf-test-generic PUBLIC cxx_std_20)
if (UNIX)
  target_compile_options(fqhex-perf-test-generic PRIVATE -Wall)
endif()

# Performance test executable (SSSE3)
add_executable(fqhex-perf-test-ssse3 fqhex_perf_test.cpp)
target_compile_features(fqhex-perf-test-ssse3 PUBLIC cxx_std_20)
target_compile_options(fqhex-perf-test-ssse3 PRIVATE -DHAVE_SSSE3=1)
if (UNIX)
  target_compile_options(fqhex-perf-test-ssse3 PRIVATE -Wall -mssse3)
endif()

# Performance test executable (AVX2)
add_executable(fqhex-perf-test-avx2 fqhex_perf_test.cpp)
target_compile_features(fqhex-perf-test-avx2 PUBLIC cxx_std_20)
target_compile_options(fqhex-perf-test-avx2 PRIVATE -DHAVE_AVX2=1)
if (UNIX)
  target_compile_options(fqhex-perf-test-avx2 PRIVATE -Wall -mavx2)
endif()
