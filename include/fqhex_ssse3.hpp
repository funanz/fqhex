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
    class ssse3
    {
        struct sub_if_result {
            __m128i result;
            __m128i mask;
        };

        static sub_if_result sub_if(uint8_t min, uint8_t max, __m128i x, uint8_t value) {
            auto mask_min = _mm_cmpgt_epi8(x, _mm_set1_epi8(min - 1));
            auto mask_max = _mm_cmplt_epi8(x, _mm_set1_epi8(max + 1));
            auto mask = _mm_and_si128(mask_min, mask_max);
            auto mask_value = _mm_and_si128(mask, _mm_set1_epi8(value));
            return { _mm_sub_epi8(x, mask_value), mask };
        }

        static __m128i gather(__m128i x) {
            auto shuffle = _mm_set_epi8(14, 12, 10, 8, 6, 4, 2, 0, 15, 13, 11, 9, 7, 5, 3, 1);
            auto lo = _mm_shuffle_epi8(x, shuffle);
            auto hi = _mm_slli_epi64(_mm_srli_si128(lo, 8), 4);
            return  _mm_or_si128(hi, lo);
        }

        static bool has_invalid_chars(__m128i mask1, __m128i mask2, __m128i mask3) {
            auto mask = _mm_or_si128(_mm_or_si128(mask1, mask2), mask3);
            return _mm_movemask_epi8(mask) != 0xffff;
        }

    public:
        static __m128i char8_to_bytes(__m128i x) {
            auto [x1, m1] = sub_if(0x30, 0x39, x, 0x30);
            auto [x2, m2] = sub_if(0x41, 0x46, x1, 0x41 - 10);
            auto [x3, m3] = sub_if(0x61, 0x66, x2, 0x61 - 10);

            if (has_invalid_chars(m1, m2, m3))
                throw std::invalid_argument("invalid hex character");

            return gather(x3);
        }

        static __m128i bytes_to_char8(__m128i x) {
            auto shuffle = _mm_set_epi8(
                -1, 7, -1, 6, -1, 5, -1, 4, -1, 3, -1, 2, -1, 1, -1, 0);
            auto ex = _mm_shuffle_epi8(x, shuffle);
            auto lo = _mm_and_si128(_mm_srli_epi64(ex, 4), _mm_set1_epi8(0x0f));
            auto hi = _mm_and_si128(_mm_slli_si128(ex, 1), _mm_set1_epi8(0x0f));
            auto v = _mm_or_si128(hi, lo);

            auto mask = _mm_cmpgt_epi8(v, _mm_set1_epi8(9));
            auto mask_0x27 = _mm_and_si128(mask, _mm_set1_epi8(0x27));
            auto add_0x30 = _mm_add_epi8(v, _mm_set1_epi8(0x30));
            auto s = _mm_add_epi8(add_0x30, mask_0x27);

            return s;
        }
    };

    template <class CharT, class ByteT>
    requires (sizeof(CharT) == 1 && sizeof(ByteT) == 1)
    class hex8_ssse3
    {
    public:
        static void to_bytes_64(const CharT in[16], ByteT out[8]) {
            auto x = _mm_loadu_si128(reinterpret_cast<const __m128i*>(in));
            auto b = ssse3::char8_to_bytes(x);
            _mm_storeu_si64(out, b);
        }

        static void to_string_64(const ByteT in[8], CharT out[16]) {
            auto x = _mm_loadu_si64(in);
            auto s = ssse3::bytes_to_char8(x);
            _mm_storeu_si128(reinterpret_cast<__m128i*>(out), s);
        }
    };

    template <class CharT, class ByteT>
    requires (sizeof(CharT) == 2 && sizeof(ByteT) == 1)
    class hex16_ssse3
    {
        static bool has_invalid_chars(__m128i s) {
            auto mask = _mm_cmpgt_epi16(s, _mm_set1_epi16(0xff));
            return _mm_movemask_epi8(mask) != 0;
        }

        static __m128i char16to8(const CharT in[8]) {
            auto s = _mm_loadu_si128(reinterpret_cast<const __m128i*>(in));

            if (has_invalid_chars(s))
                throw std::invalid_argument("invalid hex character");

            auto shuffle = _mm_set_epi8(
                -1, -1, -1, -1, -1, -1, -1, -1, 14, 12, 10, 8, 6, 4, 2, 0);
            return _mm_shuffle_epi8(s, shuffle);
        }

    public:
        static void to_bytes_64(const CharT in[16], ByteT out[8]) {
            auto s0 = char16to8(&in[0]);
            auto s1 = char16to8(&in[8]);
            auto s = _mm_or_si128(_mm_slli_si128(s1, 8), s0);
            auto b = ssse3::char8_to_bytes(s);
            _mm_storeu_si64(out, b);
        }

        static void to_string_64(const ByteT in[8], CharT out[16]) {
            auto x = _mm_loadu_si64(in);
            auto s = ssse3::bytes_to_char8(x);
            auto s0 = _mm_unpacklo_epi8(s, _mm_setzero_si128());
            auto s1 = _mm_unpacklo_epi8(_mm_srli_si128(s, 8), _mm_setzero_si128());
            _mm_storeu_si128(reinterpret_cast<__m128i*>(&out[0]), s0);
            _mm_storeu_si128(reinterpret_cast<__m128i*>(&out[8]), s1);
        }
    };

    template <class CharT, class ByteT>
    requires (sizeof(CharT) == 4 && sizeof(ByteT) == 1)
    class hex32_ssse3
    {
        static bool has_invalid_chars(__m128i s) {
            auto mask = _mm_cmpgt_epi32(s, _mm_set1_epi32(0xff));
            return _mm_movemask_epi8(mask) != 0;
        }

        static __m128i char32to8(const CharT in[4]) {
            auto s = _mm_loadu_si128(reinterpret_cast<const __m128i*>(in));

            if (has_invalid_chars(s))
                throw std::invalid_argument("invalid hex character");

            auto shuffle = _mm_set_epi8(
                -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 12, 8, 4, 0);
            return _mm_shuffle_epi8(s, shuffle);
        }

    public:
        static void to_bytes_64(const CharT in[16], ByteT out[8]) {
            auto s0 = char32to8(&in[0]);
            auto s1 = char32to8(&in[4]);
            auto s2 = char32to8(&in[8]);
            auto s3 = char32to8(&in[12]);
            auto s = _mm_or_si128(s0, _mm_slli_si128(s1, 4));
            s = _mm_or_si128(s, _mm_slli_si128(s2, 8));
            s = _mm_or_si128(s, _mm_slli_si128(s3, 12));

            auto b = ssse3::char8_to_bytes(s);
            _mm_storeu_si64(out, b);
        }

        static void to_string_64(const ByteT in[8], CharT out[16]) {
            auto x = _mm_loadu_si64(in);
            auto s = ssse3::bytes_to_char8(x);

            auto s16lo = _mm_unpacklo_epi8(s, _mm_setzero_si128());
            auto s16hi = _mm_unpacklo_epi8(_mm_srli_si128(s, 8), _mm_setzero_si128());
            auto s0 = _mm_unpacklo_epi16(s16lo, _mm_setzero_si128());
            auto s1 = _mm_unpacklo_epi16(_mm_srli_si128(s16lo, 8), _mm_setzero_si128());
            auto s2 = _mm_unpacklo_epi16(s16hi, _mm_setzero_si128());
            auto s3 = _mm_unpacklo_epi16(_mm_srli_si128(s16hi, 8), _mm_setzero_si128());

            _mm_storeu_si128(reinterpret_cast<__m128i*>(&out[0]), s0);
            _mm_storeu_si128(reinterpret_cast<__m128i*>(&out[4]), s1);
            _mm_storeu_si128(reinterpret_cast<__m128i*>(&out[8]), s2);
            _mm_storeu_si128(reinterpret_cast<__m128i*>(&out[12]), s3);
        }
    };
}
namespace fqhex
{
    template <class CharT, class ByteT>
    requires ((sizeof(CharT) == 1 || sizeof(CharT) == 2 || sizeof(CharT) == 4)
              && sizeof(ByteT) == 1)
    class hex_ssse3
    {
    public:
        using char_type = CharT;
        using byte_type = ByteT;

    private:
        static void to_bytes_64(const CharT in[16], ByteT out[8]) {
            if constexpr (sizeof(CharT) == 1)
                detail::hex8_ssse3<CharT, ByteT>::to_bytes_64(in, out);
            else if constexpr (sizeof(CharT) == 2)
                detail::hex16_ssse3<CharT, ByteT>::to_bytes_64(in, out);
            else if constexpr (sizeof(CharT) == 4)
                detail::hex32_ssse3<CharT, ByteT>::to_bytes_64(in, out);
        }

        static void to_string_64(const ByteT in[8], CharT out[16]) {
            if constexpr (sizeof(CharT) == 1)
                detail::hex8_ssse3<CharT, ByteT>::to_string_64(in, out);
            else if constexpr (sizeof(CharT) == 2)
                detail::hex16_ssse3<CharT, ByteT>::to_string_64(in, out);
            else if constexpr (sizeof(CharT) == 4)
                detail::hex32_ssse3<CharT, ByteT>::to_string_64(in, out);
        }

    public:
        static void to_bytes(std::span<const CharT> in, std::span<ByteT> out) {
            if (out.size() * 2 < in.size())
                throw std::invalid_argument("output span size is small");

            auto count = in.size() / 16;
            for (size_t i = 0; i < count; i++)
                to_bytes_64(&in[i * 16], &out[i * 8]);

            if (in.size() % 16 >= 2)
                hex_generic<CharT, ByteT>::to_bytes(in.subspan(count * 16), out.subspan(count * 8));
        }

        static void to_string(std::span<const ByteT> in, std::span<CharT> out) {
            if (out.size() < in.size() * 2)
                throw std::invalid_argument("output span size is small");

            auto count = in.size() / 8;
            for (size_t i = 0; i < count; i++)
                to_string_64(&in[i * 8], &out[i * 16]);

            if (in.size() % 8 > 0)
                hex_generic<CharT, ByteT>::to_string(in.subspan(count * 8), out.subspan(count * 16));
        }

        static auto to_string(std::span<const ByteT> in) {
            std::basic_string<CharT> s(in.size() * 2, 0);
            to_string(in, s);
            return s;
        }
    };
}
