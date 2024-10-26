// Copyright 2024 granz.fisherman@gmail.com
// https://opensource.org/license/mit
#pragma once
#include <cstdint>
#include <span>
#include <stdexcept>
#include <string>
#include <immintrin.h>
#include "fqhex_generic.hpp"

namespace fqhex::detail
{
    class avx2
    {
        struct sub_if_result {
            __m256i result;
            __m256i mask;
        };

        static sub_if_result sub_if(uint8_t min, uint8_t max, __m256i x, uint8_t value) {
            auto mask_min = _mm256_cmpgt_epi8(x, _mm256_set1_epi8(min - 1));
            auto mask_max = _mm256_cmpgt_epi8(_mm256_set1_epi8(max + 1), x);
            auto mask = _mm256_and_si256(mask_min, mask_max);
            auto mask_value = _mm256_and_si256(mask, _mm256_set1_epi8(value));
            return { _mm256_sub_epi8(x, mask_value), mask };
        }

        static __m128i gather(__m256i x) {
            auto shuffle = _mm256_set_epi8(
                14, 12, 10,  8,  6,  4,  2,  0, 15, 13, 11,  9,  7,  5,  3,  1,
                14, 12, 10,  8,  6,  4,  2,  0, 15, 13, 11,  9,  7,  5,  3,  1);
            auto lo = _mm256_shuffle_epi8(x, shuffle);
            auto hi = _mm256_slli_epi64(_mm256_srli_si256(lo, 8), 4);
            auto b64x2 = _mm256_or_si256(hi, lo);
            auto b128 = _mm256_permute4x64_epi64(b64x2, 0b11'01'10'00);
            return _mm256_extracti128_si256(b128, 0);
        }

        static bool has_invalid_chars(__m256i mask1, __m256i mask2, __m256i mask3) {
            auto mask = _mm256_or_si256(_mm256_or_si256(mask1, mask2), mask3);
            uint32_t m = _mm256_movemask_epi8(mask);
            return m != 0xffff'ffff;
        }

    public:
        static __m128i char8_to_bytes(__m256i x) {
            auto [x1, m1] = sub_if(0x30, 0x39, x, 0x30);
            auto [x2, m2] = sub_if(0x41, 0x46, x1, 0x41 - 10);
            auto [x3, m3] = sub_if(0x61, 0x66, x2, 0x61 - 10);

            if (has_invalid_chars(m1, m2, m3))
                throw std::invalid_argument("invalid hex character");

            return gather(x3);
        }

        static __m256i bytes_to_char8(__m128i x) {
            auto ex = _mm256_cvtepu8_epi16(x);
            auto lo = _mm256_and_si256(_mm256_srli_epi64(ex, 4), _mm256_set1_epi8(0x0f));
            auto hi = _mm256_and_si256(_mm256_slli_si256(ex, 1), _mm256_set1_epi8(0x0f));
            auto v = _mm256_or_si256(hi, lo);

            auto mask = _mm256_cmpgt_epi8(v, _mm256_set1_epi8(9));
            auto mask_0x27 = _mm256_and_si256(mask, _mm256_set1_epi8(0x27));
            auto add_0x30 = _mm256_add_epi8(v, _mm256_set1_epi8(0x30));
            auto s = _mm256_add_epi8(add_0x30, mask_0x27);

            return s;
        }
    };

    template <class CharT, class ByteT>
    requires (sizeof(CharT) == 1 && sizeof(ByteT) == 1)
    class hex8_avx2
    {
    public:
        static void to_bytes_128(const CharT in[32], ByteT out[16]) {
            auto x = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(in));
            auto b = avx2::char8_to_bytes(x);
            _mm_storeu_si128(reinterpret_cast<__m128i*>(out), b);
        }

        static void to_string_128(const ByteT in[16], CharT out[32]) {
            auto x = _mm_loadu_si128(reinterpret_cast<const __m128i*>(in));
            auto s = avx2::bytes_to_char8(x);
            _mm256_storeu_si256(reinterpret_cast<__m256i*>(out), s);
        }
    };

    template <class CharT, class ByteT>
    requires (sizeof(CharT) == 2 && sizeof(ByteT) == 1)
    class hex16_avx2
    {
        static bool has_invalid_chars(__m256i s) {
            auto mask = _mm256_cmpgt_epi16(s, _mm256_set1_epi16(0xff));
            return _mm256_movemask_epi8(mask) != 0;
        }

        static __m256i char16to8(const CharT in[16]) {
            auto s = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(in));

            if (has_invalid_chars(s))
                throw std::invalid_argument("invalid hex character");

            auto shuffle = _mm256_set_epi8(
                -1, -1, -1, -1, -1, -1, -1, -1, 14, 12, 10, 8, 6, 4, 2, 0,
                -1, -1, -1, -1, -1, -1, -1, -1, 14, 12, 10, 8, 6, 4, 2, 0);
            return _mm256_shuffle_epi8(s, shuffle);
        }

    public:
        static void to_bytes_128(const CharT in[32], ByteT out[16]) {
            auto s0 = char16to8(&in[0]);
            auto s1 = char16to8(&in[16]);
            auto sx = _mm256_or_si256(_mm256_slli_si256(s1, 8), s0);
            auto s = _mm256_permute4x64_epi64(sx, 0b11'01'10'00);
            auto b = avx2::char8_to_bytes(s);
            _mm_storeu_si128(reinterpret_cast<__m128i*>(out), b);
        }

        static void to_string_128(const ByteT in[16], CharT out[32]) {
            auto x = _mm_loadu_si128(reinterpret_cast<const __m128i*>(in));
            auto s = avx2::bytes_to_char8(x);
            auto s0 = _mm256_cvtepu8_epi16(_mm256_extracti128_si256(s, 0));
            auto s1 = _mm256_cvtepu8_epi16(_mm256_extracti128_si256(s, 1));
            _mm256_storeu_si256(reinterpret_cast<__m256i*>(&out[0]), s0);
            _mm256_storeu_si256(reinterpret_cast<__m256i*>(&out[16]), s1);
        }
    };

    template <class CharT, class ByteT>
    requires (sizeof(CharT) == 4 && sizeof(ByteT) == 1)
    class hex32_avx2
    {
        static bool has_invalid_chars(__m256i s) {
            auto mask = _mm256_cmpgt_epi32(s, _mm256_set1_epi32(0xff));
            return _mm256_movemask_epi8(mask) != 0;
        }

        static __m256i char32to8(const CharT in[8]) {
            auto s = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(in));

            if (has_invalid_chars(s))
                throw std::invalid_argument("invalid hex character");

            auto shuffle = _mm256_set_epi8(
                -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 12, 8, 4, 0,
                -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 12, 8, 4, 0);
            return _mm256_shuffle_epi8(s, shuffle);
        }

    public:
        static void to_bytes_128(const CharT in[32], ByteT out[16]) {
            auto s0 = char32to8(&in[0]);
            auto s1 = char32to8(&in[8]);
            auto s2 = char32to8(&in[16]);
            auto s3 = char32to8(&in[24]);
            auto sx = _mm256_or_si256(s0, _mm256_slli_si256(s1, 4));
            sx = _mm256_or_si256(sx, _mm256_slli_si256(s2, 8));
            sx = _mm256_or_si256(sx, _mm256_slli_si256(s3, 12));

            auto idx = _mm256_set_epi32(7, 3, 6, 2, 5, 1, 4, 0);
            auto s = _mm256_permutevar8x32_epi32(sx, idx);
            auto b = avx2::char8_to_bytes(s);
            _mm_storeu_si128(reinterpret_cast<__m128i*>(out), b);
        }

        static void to_string_128(const ByteT in[16], CharT out[32]) {
            auto x = _mm_loadu_si128(reinterpret_cast<const __m128i*>(in));
            auto s = avx2::bytes_to_char8(x);

            auto s_lo = _mm256_extracti128_si256(s, 0);
            auto s_hi = _mm256_extracti128_si256(s, 1);
            auto s0 = _mm256_cvtepu8_epi32(s_lo);
            auto s1 = _mm256_cvtepu8_epi32(_mm_srli_si128(s_lo, 8));
            auto s2 = _mm256_cvtepu8_epi32(s_hi);
            auto s3 = _mm256_cvtepu8_epi32(_mm_srli_si128(s_hi, 8));

            _mm256_storeu_si256(reinterpret_cast<__m256i*>(&out[0]), s0);
            _mm256_storeu_si256(reinterpret_cast<__m256i*>(&out[8]), s1);
            _mm256_storeu_si256(reinterpret_cast<__m256i*>(&out[16]), s2);
            _mm256_storeu_si256(reinterpret_cast<__m256i*>(&out[24]), s3);
        }
    };
}
namespace fqhex
{
    template <class CharT, class ByteT>
    requires ((sizeof(CharT) == 1 || sizeof(CharT) == 2 || sizeof(CharT) == 4)
              && sizeof(ByteT) == 1)
    class hex_avx2
    {
    public:
        using char_type = CharT;
        using byte_type = ByteT;

    private:
        static void to_bytes_128(const CharT in[32], ByteT out[16]) {
            if constexpr (sizeof(CharT) == 1)
                detail::hex8_avx2<CharT, ByteT>::to_bytes_128(in, out);
            else if constexpr (sizeof(CharT) == 2)
                detail::hex16_avx2<CharT, ByteT>::to_bytes_128(in, out);
            else if constexpr (sizeof(CharT) == 4)
                detail::hex32_avx2<CharT, ByteT>::to_bytes_128(in, out);
        }

        static void to_string_128(const ByteT in[16], CharT out[32]) {
            if constexpr (sizeof(CharT) == 1)
                detail::hex8_avx2<CharT, ByteT>::to_string_128(in, out);
            else if constexpr (sizeof(CharT) == 2)
                detail::hex16_avx2<CharT, ByteT>::to_string_128(in, out);
            else if constexpr (sizeof(CharT) == 4)
                detail::hex32_avx2<CharT, ByteT>::to_string_128(in, out);
        }

    public:
        static void to_bytes(std::span<const CharT> in, std::span<ByteT> out) {
            if (out.size() * 2 < in.size())
                throw std::invalid_argument("output span size is small");

            auto count = in.size() / 32;
            for (size_t i = 0; i < count; i++)
                to_bytes_128(&in[i * 32], &out[i * 16]);

            if (in.size() % 32 >= 2)
                hex_generic<CharT, ByteT>::to_bytes(in.subspan(count * 32), out.subspan(count * 16));
        }

        static void to_string(std::span<const ByteT> in, std::span<CharT> out) {
            if (out.size() < in.size() * 2)
                throw std::invalid_argument("output span size is small");

            auto count = in.size() / 16;
            for (size_t i = 0; i < count; i++)
                to_string_128(&in[i * 16], &out[i * 32]);

            if (in.size() % 8 > 0)
                hex_generic<CharT, ByteT>::to_string(in.subspan(count * 16), out.subspan(count * 32));
        }

        static auto to_string(std::span<const ByteT> in) {
            std::basic_string<CharT> s(in.size() * 2, 0);
            to_string(in, s);
            return s;
        }
    };
}
