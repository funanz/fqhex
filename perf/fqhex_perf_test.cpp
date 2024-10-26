// Copyright 2024 granz.fisherman@gmail.com
// https://opensource.org/license/mit
#include <cstddef>
#include <iostream>
#include <fqhex.hpp>
#include "fqhex_perf_test.hpp"

using hex8 = fqhex::hex_generic<char, std::byte>;
using hex16 = fqhex::hex_generic<char16_t, std::byte>;
using hex32 = fqhex::hex_generic<char32_t, std::byte>;
using sse8 = fqhex::hex_ssse3<char, std::byte>;
using sse16 = fqhex::hex_ssse3<char16_t, std::byte>;
using sse32 = fqhex::hex_ssse3<char32_t, std::byte>;
using avx8 = fqhex::hex_avx2<char, std::byte>;
using avx16 = fqhex::hex_avx2<char16_t, std::byte>;
using avx32 = fqhex::hex_avx2<char32_t, std::byte>;

template <class T>
using test = fqhex::hex_perf_test<T>;

int main()
{
    test<hex8> ("char").run();
    test<hex16>("char16_t").run();
    test<hex32>("char32_t").run();
    test<sse8> ("char    \tssse3").run();
    test<sse16>("char16_t\tssse3").run();
    test<sse32>("char32_t\tssse3").run();
    test<avx8> ("char    \tavx2").run();
    test<avx16>("char16_t\tavx2").run();
    test<avx32>("char32_t\tavx2").run();
}
