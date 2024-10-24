// Copyright 2024 granz.fisherman@gmail.com
// https://opensource.org/license/mit
#pragma once
#include <span>
#include <cstdint>
#include <stdexcept>
#include <immintrin.h>
#include "fqhex_generic.hpp"

namespace fqhex
{
    template <class CharT, class ByteT>
    requires (sizeof(CharT) == 2 && sizeof(ByteT) == 1)
    class hex16_avx2
    {
    public:
        using char_type = CharT;
        using byte_type = ByteT;

    private:
        struct sub_if_result {
            __m256i result;
            __m256i mask;
        };
 
        static sub_if_result sub_if(CharT min, CharT max, __m256i x, CharT value) {
            auto mask_min = _mm256_cmpgt_epi16(x, _mm256_set1_epi16(min - 1));
            auto mask_max = _mm256_cmpgt_epi16(_mm256_set1_epi16(max + 1), x);
            auto mask = _mm256_and_si256(mask_min, mask_max);
            auto mask_value = _mm256_and_si256(mask, _mm256_set1_epi16(value));
            return { _mm256_sub_epi16(x, mask_value), mask };
        }

        static __m128i gather(__m256i x) {
            auto shuffle = _mm256_set_epi8(
                -1, -1, -1, -1, -1, -1, -1, -1, 12, 8, 4, 0, 14, 10, 6, 2,
                -1, -1, -1, -1, -1, -1, -1, -1, 12, 8, 4, 0, 14, 10, 6, 2);
            auto lo = _mm256_shuffle_epi8(x, shuffle);
            auto hi = _mm256_srli_epi64(lo, 28);
            auto b32x2 = _mm256_or_si256(lo, hi);
            auto b32lo = _mm256_extracti128_si256(b32x2, 0);
            auto b32hi = _mm256_extracti128_si256(b32x2, 1);
            return _mm_unpacklo_epi32(b32lo, b32hi);
        }

        static bool is_out_of_range(__m256i mask1, __m256i mask2, __m256i mask3) {
            auto mask = _mm256_or_si256(_mm256_or_si256(mask1, mask2), mask3);
            auto not_mask = _mm256_xor_si256(mask, _mm256_set1_epi8(0xff));
            return !_mm256_testz_si256(not_mask, _mm256_set1_epi8(0xff));
        }

        static __m128i to_bytes(__m256i x) {
            auto [x1, m1] = sub_if(0x30, 0x39, x, 0x30);
            auto [x2, m2] = sub_if(0x41, 0x46, x1, 0x41 - 10);
            auto [x3, m3] = sub_if(0x61, 0x66, x2, 0x61 - 10);

            if (is_out_of_range(m1, m2, m3))
                throw std::invalid_argument("invalid hex character");

            return gather(x3);
        }

        static __m256i to_string(__m128i x) {
            auto ex = _mm256_cvtepu8_epi32(x);
            auto lo = _mm256_and_si256(_mm256_srli_epi64(ex, 4), _mm256_set1_epi8(0x0f));
            auto hi = _mm256_and_si256(_mm256_slli_si256(ex, 2), _mm256_set1_epi8(0x0f));
            auto v = _mm256_or_si256(hi, lo);

            auto mask = _mm256_cmpgt_epi16(v, _mm256_set1_epi16(9));
            auto mask_0x27 = _mm256_and_si256(mask, _mm256_set1_epi16(0x27));
            auto add_0x30 = _mm256_add_epi16(v, _mm256_set1_epi16(0x30));
            auto s = _mm256_add_epi16(add_0x30, mask_0x27);

            return s;
        }

        static void to_bytes_64(std::span<const CharT> in, std::span<ByteT> out) {
            auto x = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(in.data()));
            auto b = to_bytes(x);
            _mm_storeu_si64(out.data(), b);
        }

        static void to_string_64(std::span<const ByteT> in, std::span<CharT> out) {
            auto x = _mm_loadu_si64(in.data());
            auto s = to_string(x);
            _mm256_storeu_si256(reinterpret_cast<__m256i*>(out.data()), s);
        }

    public:
        static void to_bytes(std::span<const CharT> in, std::span<ByteT> out) {
            if (out.size() * 2 < in.size())
                throw std::invalid_argument("output span size is small");

            auto count = in.size() / 16;
            for (size_t i = 0; i < count; i++)
                to_bytes_64(in.subspan(i * 16), out.subspan(i * 8));

            if (in.size() % 16 >= 2)
                hex_generic<CharT, ByteT>::to_bytes(in.subspan(count * 16), out.subspan(count * 8));
        }

        static constexpr void to_string(std::span<const ByteT> in, std::span<CharT> out) {
            if (out.size() < in.size() * 2)
                throw std::invalid_argument("output span size is small");

            auto count = in.size() / 8;
            for (size_t i = 0; i < count; i++)
                to_string_64(in.subspan(i * 8), out.subspan(i * 16));

            if (in.size() % 8 > 0)
                hex_generic<CharT, ByteT>::to_string(in.subspan(count * 8), out.subspan(count * 16));
        }
    };
}
