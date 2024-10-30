# Fast? Hex Parser

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    $ make test
    $ ./fqhex-perf-test
     0:01.000790524	152.78 M op/s	to_bytes	char    	32	generic
     0:01.001119847	269.90 M op/s	to_bytes	char    	16	generic
     0:01.000606484	541.17 M op/s	to_bytes	char    	8	generic


# Features
- Converts hexadecimal strings to byte arrays.
- Converts byte arrays to hexadecimal strings.
- Performs error detection for hexadecimal strings.
- Supports x86 SSSE3 and AVX2.
- Supports multiple string types (char, wchar_t, char8_t, char16_t, char32_t).
- Supports multiple byte types (uint8_t, std::byte).

# Results

Linux x86_64 (WSL2) gcc-14.2.1

| Method    | Type     | Size |  Generic code |    SSSE3 code |     AVX2 code |
|:----------|:---------|-----:|--------------:|--------------:|--------------:|
| to_bytes  | char     |   32 | 152.78 M op/s | 288.41 M op/s | 559.99 M op/s |
| to_bytes  | char     |   16 | 269.90 M op/s | 575.16 M op/s | 599.47 M op/s |
| to_bytes  | char     |    8 | 541.17 M op/s | 534.44 M op/s | 561.72 M op/s |
| to_bytes  | char     |    4 | 903.34 M op/s | 953.44 M op/s | 960.23 M op/s |
| to_string | char     |   32 | 157.61 M op/s | 812.16 M op/s | 912.00 M op/s |
| to_string | char     |   16 | 273.84 M op/s |   1.27 G op/s |   1.35 G op/s |
| to_string | char     |    8 | 496.36 M op/s | 504.67 M op/s | 506.16 M op/s |
| to_string | char     |    4 | 806.37 M op/s | 827.95 M op/s | 833.72 M op/s |
| to_bytes  | char16_t |   32 | 119.46 M op/s | 174.31 M op/s | 296.53 M op/s |
| to_bytes  | char16_t |   16 | 229.89 M op/s | 348.88 M op/s | 354.35 M op/s |
| to_bytes  | char16_t |    8 | 419.43 M op/s | 401.87 M op/s | 430.86 M op/s |
| to_bytes  | char16_t |    4 | 827.13 M op/s | 856.45 M op/s | 926.53 M op/s |
| to_string | char16_t |   32 | 165.46 M op/s | 605.32 M op/s | 736.10 M op/s |
| to_string | char16_t |   16 | 362.85 M op/s |   1.10 G op/s |   1.10 G op/s |
| to_string | char16_t |    8 | 613.98 M op/s | 612.71 M op/s | 614.69 M op/s |
| to_string | char16_t |    4 |   1.68 G op/s |   1.70 G op/s |   1.71 G op/s |
| to_bytes  | char32_t |   32 | 114.73 M op/s | 142.35 M op/s | 254.93 M op/s |
| to_bytes  | char32_t |   16 | 232.22 M op/s | 288.28 M op/s | 296.54 M op/s |
| to_bytes  | char32_t |    8 | 409.29 M op/s | 430.50 M op/s | 430.14 M op/s |
| to_bytes  | char32_t |    4 | 790.89 M op/s | 804.85 M op/s | 815.09 M op/s |
| to_string | char32_t |   32 | 166.87 M op/s | 358.42 M op/s | 401.39 M op/s |
| to_string | char32_t |   16 | 364.78 M op/s | 709.05 M op/s | 709.01 M op/s |
| to_string | char32_t |    8 | 612.78 M op/s | 615.90 M op/s | 615.63 M op/s |
| to_string | char32_t |    4 |   1.68 G op/s |   1.65 G op/s |   1.72 G op/s |

# Example

```C++
#include <cstdint>
#include <iostream>
#include <string>
#include <fqhex.hpp>

void example()
{
    std::string s = "7c86ef21711c5d5e3a8408b5b305b49c";

    // parse
    std::array<uint8_t, 16> a;
    try {
        fqhex::hex_avx2<char, uint8_t>::to_bytes(s, a);
    }
    catch (std::invalid_argument& e) {
        std::cerr << e.what() << std::endl; // parse error
    }

    // to string
    std::wstring ws = fqhex::hex_avx2<wchar_t, uint8_t>::to_string(a);

    // to char array (non null terminated)
    std::array<char32_t, 32> u32;
    fqhex::hex_avx2<char32_t, uint8_t>::to_string(a, u32);
}
```
