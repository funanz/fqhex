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
    requires (sizeof(CharT) == 1 && sizeof(ByteT) == 1)
    class hex8_ssse3
    {
    public:
        using char_type = CharT;
        using byte_type = ByteT;

    private:
        struct sub_if_result {
            __m128i result;
            __m128i mask;
        };
 
        static sub_if_result sub_if(CharT min, CharT max, __m128i x, CharT value) {
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

        static bool is_out_of_range(__m128i mask1, __m128i mask2, __m128i mask3) {
            auto mask = _mm_or_si128(_mm_or_si128(mask1, mask2), mask3);
            auto mask_hi = _mm_shuffle_epi32(mask, _MM_SHUFFLE(3, 2, 3, 2));
            auto mask64 = _mm_and_si128(mask, mask_hi);

            uint64_t u64;
            _mm_storeu_si64(&u64, mask64);
            return ~u64 != 0;
        }

        static __m128i to_bytes(__m128i x) {
            auto [x1, m1] = sub_if(0x30, 0x39, x, 0x30);
            auto [x2, m2] = sub_if(0x41, 0x46, x1, 0x41 - 10);
            auto [x3, m3] = sub_if(0x61, 0x66, x2, 0x61 - 10);

            if (is_out_of_range(m1, m2, m3))
                throw std::invalid_argument("invalid hex character");

            return gather(x3);
        }

        static __m128i to_string(__m128i x) {
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

        static void to_bytes_64(std::span<const CharT> in, std::span<ByteT> out) {
            auto x = _mm_loadu_si128(reinterpret_cast<const __m128i*>(in.data()));
            _mm_storeu_si64(out.data(), to_bytes(x));
        }

        static void to_string_64(std::span<const ByteT> in, std::span<CharT> out) {
            auto x = _mm_loadu_si64(in.data());
            auto s = to_string(x);
            _mm_storeu_si128(reinterpret_cast<__m128i*>(out.data()), s);
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
