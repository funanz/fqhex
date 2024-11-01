// Copyright 2024 granz.fisherman@gmail.com
// https://opensource.org/license/mit
#if HAVE_AVX2
#   include <fqhex_avx2.hpp>
#   define HEX_ENGINE fqhex::hex_avx2
#elif HAVE_SSSE3
#   include <fqhex_ssse3.hpp>
#   define HEX_ENGINE fqhex::hex_ssse3
#else
#   include <fqhex_generic.hpp>
#   define HEX_ENGINE fqhex::hex_generic
#endif

#if CHAR_MODE == 0
#   define CHAR_T char
#   define S(s) s
#elif CHAR_MODE == 1632
#   define CHAR_T wchar_t
#   define HAVE_UNICODE 1
#   define S(s) L##s
#elif CHAR_MODE == 8
#   define CHAR_T char8_t
#   define HAVE_UNICODE 1
#   define S(s) u8##s
#elif CHAR_MODE == 16
#   define CHAR_T char16_t
#   define HAVE_UNICODE 1
#   define S(s) u##s
#elif CHAR_MODE == 32
#   define CHAR_T char32_t
#   define HAVE_UNICODE 1
#   define S(s) U##s
#endif

#if BYTE_MODE == 0
#   define BYTE_T std::byte
#elif BYTE_MODE == 2
#   define BYTE_T uint8_t
#endif

#define B(n) static_cast<ByteT>(n)

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <random>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

using CharT = CHAR_T;
using ByteT = BYTE_T;
using Hex = HEX_ENGINE<CharT, ByteT>;
using String = std::basic_string<CharT>;

#define runtime_assert(expr, msg) [](bool ok){ if (!ok) throw std::runtime_error(msg " " #expr); }(expr)

static String to_lower(const String& s)
{
    String result = s;
    for (auto& c : result) {
        if (0x41 <= c && c <= 0x46)
            c += 0x20;
    }
    return result;
}

static void test_16_seq()
{
    String s1 = S("0123456789abcdef0123456789ABCDEF");

    std::array<ByteT, 16> b1 {
        B(0x01), B(0x23), B(0x45), B(0x67), B(0x89), B(0xab), B(0xcd), B(0xef),
        B(0x01), B(0x23), B(0x45), B(0x67), B(0x89), B(0xab), B(0xcd), B(0xef),
    };

    std::array<ByteT, 16> b2;
    Hex::to_bytes(s1, b2);

    String s2 = Hex::to_string(b1);

    runtime_assert(b1 == b2, "test_16_seq #1");
    runtime_assert(to_lower(s1) == s2, "test_16_seq #2");
}

static void test_16()
{
    String s1 = S("7c86ef21711c5d5e3a8408b5b305b49c");

    std::array<ByteT, 16> b1 {
        B(0x7c), B(0x86), B(0xef), B(0x21), B(0x71), B(0x1c), B(0x5d), B(0x5e),
        B(0x3a), B(0x84), B(0x08), B(0xb5), B(0xb3), B(0x05), B(0xb4), B(0x9c),
    };

    std::array<ByteT, 16> b2;
    Hex::to_bytes(s1, b2);

    String s2(b1.size() * 2, S('*'));
    Hex::to_string(b1, s2);

    runtime_assert(b1 == b2, "test_16_rand #1");
    runtime_assert(to_lower(s1) == s2, "test_16_rand #2");
}

static void test_31()
{
    String s1 = S("7c86ef21711c5d5e3a8408b5b305b49cAAAAAAAAAAAAAAAAbbbbbbbbCCCCdd");

    std::array<ByteT, 16+8+4+2+1> b1 {
        B(0x7c), B(0x86), B(0xef), B(0x21), B(0x71), B(0x1c), B(0x5d), B(0x5e),
        B(0x3a), B(0x84), B(0x08), B(0xb5), B(0xb3), B(0x05), B(0xb4), B(0x9c),
        B(0xaa), B(0xaa), B(0xaa), B(0xaa), B(0xaa), B(0xaa), B(0xaa), B(0xaa),
        B(0xbb), B(0xbb), B(0xbb), B(0xbb),
        B(0xcc), B(0xcc),
        B(0xdd),
    };

    std::array<ByteT, 16+8+4+2+1> b2;
    Hex::to_bytes(s1, b2);

    String s2(b1.size() * 2, S('*'));
    Hex::to_string(b1, s2);

    runtime_assert(b1 == b2, "test_31 #1");
    runtime_assert(to_lower(s1) == s2, "test_31 #2");
}

static void test_8()
{
    String s1 = S("7c86ef21711c5d5e");

    std::array<ByteT, 8> b1 {
        B(0x7c), B(0x86), B(0xef), B(0x21), B(0x71), B(0x1c), B(0x5d), B(0x5e),
    };

    std::array<ByteT, 8> b2;
    Hex::to_bytes(s1, b2);

    String s2(b1.size() * 2, S('*'));
    Hex::to_string(b1, s2);

    runtime_assert(b1 == b2, "test_8 #1");
    runtime_assert(to_lower(s1) == s2, "test_8 #2");
}

static void test_4()
{
    String s1 = S("7c86ef21");
    std::array<ByteT, 4> b1 { B(0x7c), B(0x86), B(0xef), B(0x21) };

    std::array<ByteT, 4> b2;
    Hex::to_bytes(s1, b2);

    String s2(b1.size() * 2, S('*'));
    Hex::to_string(b1, s2);

    runtime_assert(b1 == b2, "test_4 #1");
    runtime_assert(to_lower(s1) == s2, "test_4 #2");
}

static void test_2()
{
    String s1 = S("7c86");
    std::array<ByteT, 2> b1 { B(0x7c), B(0x86) };

    std::array<ByteT, 2> b2;
    Hex::to_bytes(s1, b2);

    String s2(b1.size() * 2, S('*'));
    Hex::to_string(b1, s2);

    runtime_assert(b1 == b2, "test_2 #1");
    runtime_assert(to_lower(s1) == s2, "test_2 #2");
}

static void test_16_unicode()
{
#if HAVE_UNICODE
    String s1 = S("\u0037c86ef21711c5d5e3a8408b5b305b49\U00000063");

    std::array<ByteT, 16> b1 {
        B(0x7c), B(0x86), B(0xef), B(0x21), B(0x71), B(0x1c), B(0x5d), B(0x5e),
        B(0x3a), B(0x84), B(0x08), B(0xb5), B(0xb3), B(0x05), B(0xb4), B(0x9c),
    };

    std::array<ByteT, 16> b2;
    Hex::to_bytes(s1, b2);

    String s2(b1.size() * 2, S('*'));
    Hex::to_string(b1, s2);

    runtime_assert(b1 == b2, "test_16_unicode #1");
    runtime_assert(to_lower(s1) == s2, "test_16_unicode #2");
#endif
}

static void test_parse_error()
{
    std::vector<String> v {
        S("!7302db4e9efb4e9a68d622b5fe406a0"),
        S("c@302db4e9efb4e9a68d622b5fe406a0"),
        S("c7#02db4e9efb4e9a68d622b5fe406a0"),
        S("c73$2db4e9efb4e9a68d622b5fe406a0"),
        S("c730%db4e9efb4e9a68d622b5fe406a0"),
        S("c7302^b4e9efb4e9a68d622b5fe406a0"),
        S("c7302d&4e9efb4e9a68d622b5fe406a0"),
        S("c7302db*e9efb4e9a68d622b5fe406a0"),
        S("c7302db4(9efb4e9a68d622b5fe406a0"),
        S("c7302db4e)efb4e9a68d622b5fe406a0"),
        S("c7302db4e9-fb4e9a68d622b5fe406a0"),
        S("c7302db4e9e=b4e9a68d622b5fe406a0"),
        S("c7302db4e9ef[4e9a68d622b5fe406a0"),
        S("c7302db4e9efb]e9a68d622b5fe406a0"),
        S("c7302db4e9efb4;9a68d622b5fe406a0"),
        S("c7302db4e9efb4e'a68d622b5fe406a0"),
        S("c7302db4e9efb4e9,68d622b5fe406a0"),
        S("c7302db4e9efb4e9a.8d622b5fe406a0"),
        S("c7302db4e9efb4e9a6/d622b5fe406a0"),
        S("c7302db4e9efb4e9a68_622b5fe406a0"),
        S("c7302db4e9efb4e9a68d+22b5fe406a0"),
        S("c7302db4e9efb4e9a68d6{2b5fe406a0"),
        S("c7302db4e9efb4e9a68d62}b5fe406a0"),
        S("c7302db4e9efb4e9a68d622|5fe406a0"),
        S("c7302db4e9efb4e9a68d622b:fe406a0"),
        S("c7302db4e9efb4e9a68d622b5<e406a0"),
        S("c7302db4e9efb4e9a68d622b5f>406a0"),
        S("c7302db4e9efb4e9a68d622b5fe?06a0"),
        S("c7302db4e9efb4e9a68d622b5fe4`6a0"),
        S("c7302db4e9efb4e9a68d622b5fe40~a0"),
        S(R"(c7302db4e9efb4e9a68d622b5fe406\0)"),
        S(R"(c7302db4e9efb4e9a68d622b5fe406a")"),
    };

    std::array<ByteT, 16> b;
    for (auto& s : v) {
        try {
            Hex::to_bytes(s, b);
            runtime_assert(false, "test_parse_error #1");
        }
        catch (std::invalid_argument&) {}
    }
}

static void test_parse_error_unicode()
{
#if HAVE_UNICODE
    std::vector<String> v {
        S("c7302db4e9efb4e9a68d622b5fe406a\u0130"),
        S("c7302db4e9efb4e9a68d622b5fe406a\U00000130"),
        S("\u00637302db4e9efb4e9a68d622b5fe406a\u0130"),
        S("\U000000637302db4e9efb4e9a68d622b5fe406a\U00000130"),
    };

    std::array<ByteT, 16> b;
    for (auto& s : v) {
        try {
            Hex::to_bytes(s, b);
            runtime_assert(false, "test_parse_error_unicode #1");
        }
        catch (std::invalid_argument&) {}
    }
#endif
}

static void test_32_random()
{
    std::mt19937_64 rng;
    std::array<uint64_t, 4> r;
    std::array<ByteT, 32> b1;
    std::array<ByteT, 32> b2;
    std::array<CharT, 64> s1;
    std::array<CharT, 64> s2;

    for (int i = 0; i < 100'000; i++) {
        std::ranges::generate(r, std::ref(rng));
        memcpy(b1.data(), r.data(), b1.size());

        Hex::to_string(b1, s1);
        Hex::to_bytes(s1, b2);
        Hex::to_string(b2, s2);

        runtime_assert(b1 == b2, "test_random #1");
        runtime_assert(s1 == s2, "test_random #2");
    }
}

static void test_16_range_error()
{
    String s = S("7c86ef21711c5d5e3a8408b5b305b49c");
    std::array<ByteT, 16> b {
        B(0x7c), B(0x86), B(0xef), B(0x21), B(0x71), B(0x1c), B(0x5d), B(0x5e),
        B(0x3a), B(0x84), B(0x08), B(0xb5), B(0xb3), B(0x05), B(0xb4), B(0x9c),
    };

    try {
        std::array<ByteT,15> a;
        Hex::to_bytes(s, a);
        runtime_assert(false, "test_range #1");
    }
    catch (std::invalid_argument&) {}

    try {
        std::array<CharT,31> a;
        Hex::to_string(b, a);
        runtime_assert(false, "test_range #2");
    }
    catch (std::invalid_argument&) {}
}

#define V_TO_S(v) #v
#define TO_S(m) V_TO_S(m)

int main()
{
    try {
        test_16_seq();
        test_16();
        test_31();
        test_8();
        test_4();
        test_2();
        test_16_unicode();
        test_parse_error();
        test_parse_error_unicode();
        test_32_random();
        test_16_range_error();

        std::cout << "All tests successful.\t"
                  << TO_S(HEX_ENGINE) << "<"
                  << TO_S(CHAR_T) << ", "
                  << TO_S(BYTE_T) << ">"
                  << std::endl;
        return 0;
    }
    catch (std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
}
