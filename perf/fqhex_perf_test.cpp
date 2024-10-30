// Copyright 2024 granz.fisherman@gmail.com
// https://opensource.org/license/mit
#include <cstddef>
#include <iostream>
#if HAVE_AVX2
#    include <fqhex_avx2.hpp>
#elif HAVE_SSSE3
#    include <fqhex_ssse3.hpp>
#else
#    include <fqhex_generic.hpp>
#endif
#include "fqhex_perf_test.hpp"

template <class T>
using test = fqhex::hex_perf_test<T>;

int main()
{
#if HAVE_AVX2
    using avx8 = fqhex::hex_avx2<char, std::byte>;
    using avx16 = fqhex::hex_avx2<char16_t, std::byte>;
    using avx32 = fqhex::hex_avx2<char32_t, std::byte>;
    test<avx8>("avx2", "char    ").run();
    test<avx16>("avx2", "char16_t").run();
    test<avx32>("avx2", "char32_t").run();
#elif HAVE_SSSE3
    using sse8 = fqhex::hex_ssse3<char, std::byte>;
    using sse16 = fqhex::hex_ssse3<char16_t, std::byte>;
    using sse32 = fqhex::hex_ssse3<char32_t, std::byte>;
    test<sse8>("ssse3", "char    ").run();
    test<sse16>("ssse3", "char16_t").run();
    test<sse32>("ssse3", "char32_t").run();
#else
    using gen8 = fqhex::hex_generic<char, std::byte>;
    using gen16 = fqhex::hex_generic<char16_t, std::byte>;
    using gen32 = fqhex::hex_generic<char32_t, std::byte>;
    test<gen8>("generic", "char    ").run();
    test<gen16>("generic", "char16_t").run();
    test<gen32>("generic", "char32_t").run();
#endif
}
