/*
r128.h: 128-bit (64.64) signed fixed-point arithmetic. Version 1.0

COMPILATION
-----------
Drop this header file somewhere in your project and include it wherever it is
needed. There is no separate .c file for this library. To get the code, in ONE
file in your project, put:

#define R128_IMPLEMENTATION

before you include this file. You may also provide a definition for R128_ASSERT
to force the library to use a custom assert macro.

COMPILER/LIBRARY SUPPORT
------------------------
This library requires a C89 with support for 64-bit integers. If your compiler
does not support the long long data type, the R128_U64, etc. macros must be set
appropriately.

The only C runtime library functionality used by this library is <assert.h>.
This can be avoided by defining an R128_ASSERT macro in your implementation
file. Since this library uses 64-bit arithmetic, this may implicitly add a
runtime library dependency on 32-bit platforms.

C++ SUPPORT
-----------
Operator overloads are supplied for C++ files that include this file. Since all
C++ functions are declared inline (or static inline), the R128_IMPLEMENTATION
file can be either C++ or C.

LICENSE
-------
Copyright (c) 2017 F. Alan Hickman

Permission is hereby granted, free of charge, to any person obtaining a copy of 
this software and associated documentation files (the "Software"), to deal in 
the Software without restriction, including without limitation the rights to 
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
of the Software, and to permit persons to whom the Software is furnished to do 
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
SOFTWARE.
*/

#ifndef H_R128_H
#define H_R128_H

#include <stddef.h>

// 64-bit integer support
// If your compiler does not have stdint.h, add appropriate defines for these macros.
#if defined(_MSC_VER) && (_MSC_VER < 1600)
#  define R128_S32 __int32
#  define R128_U32 unsigned __int32
#  define R128_S64 __int64
#  define R128_U64 unsigned __int64
#  define R128_LIT_S64(x) x##i64
#  define R128_LIT_U64(x) x##ui64
#else
#  include <stdint.h>
#  define R128_S32 int32_t
#  define R128_U32 uint32_t
#  define R128_S64 int64_t
#  define R128_U64 uint64_t
#  define R128_LIT_S64(x) x##ll
#  define R128_LIT_U64(x) x##ull
#endif

#ifdef __cplusplus
extern "C" {
#endif
 
typedef struct R128 {
   R128_U64 lo;
   R128_U64 hi;

#ifdef __cplusplus
   R128();
   R128(R128_S64);
   R128(double);
   R128(R128_U64 low, R128_U64 high);

   operator double() const;
   operator R128_S64() const;
   operator int() const;
   operator bool() const;

   bool operator!() const;
   R128 operator~() const;
   R128 operator-() const;
   R128 &operator|=(const R128 &rhs);
   R128 &operator&=(const R128 &rhs);
   R128 &operator^=(const R128 &rhs);
   R128 &operator+=(const R128 &rhs);
   R128 &operator-=(const R128 &rhs);
   R128 &operator*=(const R128 &rhs);
   R128 &operator/=(const R128 &rhs);
   R128 &operator%=(const R128 &rhs);
   R128 &operator<<=(int amount);
   R128 &operator>>=(int amount);
#endif   //__cplusplus
} R128;

// Type conversion
extern void r128FromInt(R128 *dst, R128_S64 v);
extern void r128FromFloat(R128 *dst, double v);
extern R128_S64 r128ToInt(const R128 *v);
extern double r128ToFloat(const R128 *v);

// Copy
extern void r128Copy(R128 *dst, const R128 *src);

// Negate
extern void r128Neg(R128 *dst, const R128 *src);

// Bitwise operations
extern void r128Not(R128 *dst, const R128 *src);               // ~a
extern void r128Or(R128 *dst, const R128 *a, const R128 *b);   // a | b
extern void r128And(R128 *dst, const R128 *a, const R128 *b);  // a & b
extern void r128Xor(R128 *dst, const R128 *a, const R128 *b);  // a ^ b
extern void r128Shl(R128 *dst, const R128 *src, int amount);   // shift left by amount mod 128
extern void r128Shr(R128 *dst, const R128 *src, int amount);   // shift right logical by amount mod 128
extern void r128Sar(R128 *dst, const R128 *src, int amount);   // shift right arithmetic by amount mod 128

// Arithmetic
extern void r128Add(R128 *dst, const R128 *a, const R128 *b);  // a + b
extern void r128Sub(R128 *dst, const R128 *a, const R128 *b);  // a - b
extern void r128Mul(R128 *dst, const R128 *a, const R128 *b);  // a * b
extern void r128Div(R128 *dst, const R128 *a, const R128 *b);  // a / b
extern void r128Mod(R128 *dst, const R128 *a, const R128 *b);  // a - toInt(a / b) * b

// Comparison
extern int  r128Cmp(const R128 *a, const R128 *b);  // sign of a-b
extern void r128Min(R128 *dst, const R128 *a, const R128 *b);
extern void r128Max(R128 *dst, const R128 *a, const R128 *b);
extern void r128Floor(R128 *dst, const R128 *v);
extern void r128Ceil(R128 *dst, const R128 *v);
extern int  r128IsNeg(const R128 *v); // quick check for < 0

// String conversion
//
typedef enum R128ToStringSign {
   R128ToStringSign_Default,  // no sign character for positive values
   R128ToStringSign_Space,    // leading space for positive values
   R128ToStringSign_Plus,     // leading '+' for positive values
} R128ToStringSign;

// Formatting options for use with r128ToStringOpt. The "defaults" correspond
// to a format string of "%f".
//
typedef struct R128ToStringFormat {
   // sign character for positive values. Default is R128ToStringSign_Default.
   R128ToStringSign sign;

   // minimum number of characters to write. Default is 0.
   int width;

   // place to the right of the decimal at which rounding is performed. If negative,
   // a maximum of 20 decimal places will be written, with no trailing zeroes.
   // (20 places is sufficient to ensure that r128FromString will convert back to the
   // original value.) Default is -1. NOTE: This is not the same default that the C
   // standard library uses for %f.
   int precision;

   // If non-zero, pads the output string with leading zeroes if the final result is
   // fewer than width characters. Otherwise, leading spaces are used. Default is 0.
   int zeroPad;

   // Always print a decimal point, even if the value is an integer. Default is 0.
   int decimal;

   // Left-align output if width specifier requires padding.
   // Default is 0 (right align).
   int leftAlign;
} R128ToStringFormat;

// r128ToStringOpt: convert R128 to a decimal string, with formatting.
// 
// dst and dstSize: specify the buffer to write into. At most dstSize bytes will be written
// (including null terminator). No additional rounding is performed if dstSize is not large
// enough to hold the entire string.
//
// opt: an R128ToStringFormat struct (q.v.) with formatting options.
//
// Uses the R128_decimal global as the decimal point character.
// Always writes a null terminator, even if the destination buffer is not large enough.
//
// Number of bytes that will be written (i.e. how big does dst need to be?):
// If width is specified: width + 1 bytes.
// If precision is specified: at most precision + 23 bytes.
// If neither is specified: at most 43 bytes.
//
// Returns the number of characters written, not including the final null terminator.
//
extern int r128ToStringOpt(char *dst, size_t dstSize, const R128 *v, const R128ToStringFormat *opt);

// r128ToStringf: convert R128 to a decimal string, with formatting.
// 
// dst and dstSize: specify the buffer to write into. At most dstSize bytes will be written
// (including null terminator).
//
// format: a printf-style format specifier, as one would use with floating point types.
//    e.g. "%+5.2f". (The leading % and trailing f are optional.)
//    NOTE: This is NOT a full replacement for sprintf. Any characters in the format string
//       that do not correspond to a format placeholder are ignored.
//
// Uses the R128_decimal global as the decimal point character.
// Always writes a null terminator, even if the destination buffer is not large enough.
//
// Number of bytes that will be written (i.e. how big does dst need to be?):
// If the precision field is specified: at most max(width, precision + 22) + 1 bytes
// Otherwise: at most max(width, 42) + 1 bytes.
//
// Returns the number of characters written, not including the final null terminator.
//
extern int r128ToStringf(char *dst, size_t dstSize, const char *format, const R128 *v);

// r128ToString: convert R128 to a decimal string, with default formatting.
// Equivalent to r128ToStringf(dst, dstSize, "%f", v).
//
// Uses the R128_decimal global as the decimal point character.
// Always writes a null terminator, even if the destination buffer is not large enough.
// 
// Will write at most 43 bytes (including NUL) to dst.
//
// Returns the number of characters written, not including the final null terminator.
//
extern int r128ToString(char *dst, size_t dstSize, const R128 *v);

// r128FromString: Convert string to R128.
//
// The string can be formatted either as a decimal number with optional sign
// or as hexadecimal with a prefix of 0x or 0X.
//
// endptr, if not NULL, is set to the character following the last character
//   used in the conversion.
//
extern void r128FromString(R128 *dst, const char *s, char **endptr);

// Constants
extern const R128 R128_min;      // minimum (most negative) value
extern const R128 R128_max;      // maximum (most positive) value
extern const R128 R128_smallest; // smallest positive value
extern const R128 R128_zero;     // zero
extern const R128 R128_one;      // 1.0

extern char R128_decimal;        // decimal point character used by r128From/ToString. defaults to '.'

#ifdef __cplusplus
}

#include <limits>
namespace std {
template<>
struct numeric_limits<R128>
{
   static const bool is_specialized = true;

   static R128 min() throw() { return R128_min; }
   static R128 max() throw() { return R128_max; }

   static const int digits = 127;
   static const int digits10 = 39;
   static const bool is_signed = true;
   static const bool is_integer = false;
   static const bool is_exact = false;
   static const int radix = 2;
   static R128 epsilon() throw() { return R128_smallest; }
   static R128 round_error() throw() { return R128_one; }

   static const int min_exponent = 0;
   static const int min_exponent10 = 0;
   static const int max_exponent = 0;
   static const int max_exponent10 = 0;

   static const bool has_infinity = false;
   static const bool has_quiet_NaN = false;
   static const bool has_signaling_NaN = false;
   static const float_denorm_style has_denorm = denorm_absent;
   static const bool has_denorm_loss = false;

   static R128 infinity() throw() { return R128_zero; }
   static R128 quiet_NaN() throw() { return R128_zero; }
   static R128 signaling_NaN() throw() { return R128_zero; }
   static R128 denorm_min() throw() { return R128_zero; }

   static const bool is_iec559 = false;
   static const bool is_bounded = true;
   static const bool is_modulo = true;

   static const bool traps = numeric_limits<R128_U64>::traps;
   static const bool tinyness_before = false;
   static const float_round_style round_style = round_toward_zero;
};
}  //namespace std

inline R128::R128() {}

inline R128::R128(R128_S64 v)
{
   r128FromInt(this, v);
}

inline R128::R128(double v)
{
   r128FromFloat(this, v);
}

inline R128::R128(R128_U64 low, R128_U64 high)
{
   lo = low;
   hi = high;
}

inline R128::operator double() const
{
   return r128ToFloat(this);
}

inline R128::operator R128_S64() const
{
   return r128ToInt(this);
}

inline R128::operator int() const
{
   return (int) r128ToInt(this);
}

inline R128::operator bool() const
{
   return lo || hi;
}

inline bool R128::operator!() const
{
   return !lo && !hi;
}

inline R128 R128::operator~() const
{
   R128 r;
   r128Not(&r, this);
   return r;
}

inline R128 R128::operator-() const
{
   R128 r;
   r128Neg(&r, this);
   return r;
}

inline R128 &R128::operator|=(const R128 &rhs)
{
   r128Or(this, this, &rhs);
   return *this;
}

inline R128 &R128::operator&=(const R128 &rhs)
{
   r128And(this, this, &rhs);
   return *this;
}

inline R128 &R128::operator^=(const R128 &rhs)
{
   r128Xor(this, this, &rhs);
   return *this;
}

inline R128 &R128::operator+=(const R128 &rhs)
{
   r128Add(this, this, &rhs);
   return *this;
}

inline R128 &R128::operator-=(const R128 &rhs)
{
   r128Sub(this, this, &rhs);
   return *this;
}

inline R128 &R128::operator*=(const R128 &rhs)
{
   r128Mul(this, this, &rhs);
   return *this;
}

inline R128 &R128::operator/=(const R128 &rhs)
{
   r128Div(this, this, &rhs);
   return *this;
}

inline R128 &R128::operator%=(const R128 &rhs)
{
   r128Mod(this, this, &rhs);
   return *this;
}

inline R128 &R128::operator<<=(int amount)
{
   r128Shl(this, this, amount);
   return *this;
}

inline R128 &R128::operator>>=(int amount)
{
   r128Sar(this, this, amount);
   return *this;
}

static inline R128 operator|(const R128 &lhs, const R128 &rhs)
{
   R128 r(lhs);
   return r |= rhs;
}

static inline R128 operator&(const R128 &lhs, const R128 &rhs)
{
   R128 r(lhs);
   return r &= rhs;
}

static inline R128 operator^(const R128 &lhs, const R128 &rhs)
{
   R128 r(lhs);
   return r ^= rhs;
}

static inline R128 operator+(const R128 &lhs, const R128 &rhs)
{
   R128 r(lhs);
   return r += rhs;
}

static inline R128 operator-(const R128 &lhs, const R128 &rhs)
{
   R128 r(lhs);
   return r -= rhs;
}

static inline R128 operator*(const R128 &lhs, const R128 &rhs)
{
   R128 r(lhs);
   return r *= rhs;
}

static inline R128 operator/(const R128 &lhs, const R128 &rhs)
{
   R128 r(lhs);
   return r /= rhs;
}

static inline R128 operator%(const R128 &lhs, const R128 &rhs)
{
   R128 r(lhs);
   return r %= rhs;
}

static inline R128 operator<<(const R128 &lhs, int amount)
{
   R128 r(lhs);
   return r <<= amount;
}

static inline R128 operator>>(const R128 &lhs, int amount)
{
   R128 r(lhs);
   return r >>= amount;
}

static inline bool operator<(const R128 &lhs, const R128 &rhs)
{
   return r128Cmp(&lhs, &rhs) < 0;
}

static inline bool operator>(const R128 &lhs, const R128 &rhs)
{
   return r128Cmp(&lhs, &rhs) > 0;
}

static inline bool operator<=(const R128 &lhs, const R128 &rhs)
{
   return r128Cmp(&lhs, &rhs) <= 0;
}

static inline bool operator>=(const R128 &lhs, const R128 &rhs)
{
   return r128Cmp(&lhs, &rhs) >= 0;
}

static inline bool operator==(const R128 &lhs, const R128 &rhs)
{
   return lhs.lo == rhs.lo && lhs.hi == rhs.hi;
}

static inline bool operator!=(const R128 &lhs, const R128 &rhs)
{
   return lhs.lo != rhs.lo || lhs.hi != rhs.hi;
}

#endif   //__cplusplus
#endif   //H_R128_H

#ifdef R128_IMPLEMENTATION

#define R128_SET2(x, l, h) do { (x)->lo = (R128_U64)(l); (x)->hi = (R128_U64)(h); } while(0)
#define R128_R0(x) ((R128_U32)(x)->lo)
#define R128_R2(x) ((R128_U32)(x)->hi)
#if defined(_M_IX86)
// workaround: MSVC x86's handling of 64-bit values is not great
#  define R128_SET4(x, r0, r1, r2, r3) do { \
      ((R128_U32*)&(x)->lo)[0] = (R128_U32)(r0); \
      ((R128_U32*)&(x)->lo)[1] = (R128_U32)(r1); \
      ((R128_U32*)&(x)->hi)[0] = (R128_U32)(r2); \
      ((R128_U32*)&(x)->hi)[1] = (R128_U32)(r3); \
      } while(0)
#  define R128_R1(x) (((R128_U32*)&(x)->lo)[1])
#  define R128_R3(x) (((R128_U32*)&(x)->hi)[1])
#else
#  define R128_SET4(x, r0, r1, r2, r3) do { (x)->lo = (R128_U64)(r0) | ((R128_U64)(r1) << 32); \
      (x)->hi = (R128_U64)(r2) | ((R128_U64)(r3) << 32); } while(0)
#  define R128_R1(x) ((R128_U32)((x)->lo >> 32))
#  define R128_R3(x) ((R128_U32)((x)->hi >> 32))
#endif

#if defined(_M_X64)
#  define R128_INTEL 1
#  define R128_64BIT 1
#  include <intrin.h>
#elif defined(__x86_64__)
#  define R128_INTEL 1
#  define R128_64BIT 1
#  include <x86intrin.h>
#elif defined(_M_IX86)
#  define R128_INTEL 1
#  include <intrin.h>
#elif defined(__i386__)
#  define R128_INTEL 1
#  include <x86intrin.h>
#elif defined(_M_ARM)
#  include <intrin.h>
#elif defined(_M_ARM64)
#  define R128_64BIT 1
#  include <intrin.h>
#elif defined(__aarch64__)
#  define R128_64BIT 1
#endif

#ifndef R128_INTEL
#  define R128_INTEL 0
#endif

#ifndef R128_64BIT
#  define R128_64BIT 0
#endif

#ifndef R128_ASSERT
#  include <assert.h>
#  define R128_ASSERT(x) assert(x)
#endif

#include <stdlib.h>  // for NULL

static const R128ToStringFormat R128__defaultFormat = {
   R128ToStringSign_Default,
   0,
   -1,
   0,
   0,
   0
};

const R128 R128_min = { 0, R128_LIT_U64(0x8000000000000000) };
const R128 R128_max = { R128_LIT_U64(0xffffffffffffffff), R128_LIT_U64(0x7fffffffffffffff) };
const R128 R128_smallest = { 1, 0 };
const R128 R128_zero = { 0, 0 };
const R128 R128_one = { 0, 1 };
char R128_decimal = '.';

static int r128__clz64(R128_U64 x)
{
#ifdef _M_X64
   unsigned long idx;
   if (_BitScanReverse64(&idx, x)) {
      return 63 - (int)idx;
   } else {
      return 64;
   }
#elif defined(_MSC_VER)
   unsigned long idx;
   if (_BitScanReverse(&idx, (R128_U32)(x >> 32))) {
      return 31 - (int)idx;
   } else if (_BitScanReverse(&idx, (R128_U32)x)) {
      return 63 - (int)idx;
   } else {
      return 64;
   }
#else
   return x ? __builtin_clzll(x) : 64;
#endif
}

#if !R128_64BIT
// 32*32->64
static R128_U64 r128__umul64(R128_U32 a, R128_U32 b)
{
#  if defined(_M_IX86)
   return __emulu(a, b);
#  else
   return a * (R128_U64)b;
#  endif
}

// 64/32->32
static R128_U32 r128__udiv64(R128_U32 nlo, R128_U32 nhi, R128_U32 d, R128_U32 *rem)
{
#  if defined(_M_IX86)
   __asm {
      mov eax, nlo
      mov edx, nhi
      div d
      mov ecx, rem
      mov dword ptr [ecx], edx
   }
#  elif defined(__i386__)
   R128_U32 q, r;
   __asm("divl %4"
      : "=a"(q), "=d"(r)
      : "a"(nlo), "d"(nhi), "X"(d));
   *rem = r;
   return q;
#  else
   R128_U64 n64 = ((R128_U64)nhi << 32) | nlo;
   *rem = (R128_U32)(n64 % d);
   return (R128_U32)(n64 / d);
#  endif
}
#endif   //!R128_64BIT

// 64*64->128
static void r128__umul128(R128 *dst, R128_U64 a, R128_U64 b)
{
#if defined(_M_X64)
   dst->lo = _umul128(a, b, &dst->hi);
#elif defined(__x86_64__)
   unsigned __int128 p0 = a * (unsigned __int128)b;
   dst->hi = (R128_U64)(p0 >> 64);
   dst->lo = (R128_U64)p0;
#else
   R128_U32 alo = (R128_U32)a;
   R128_U32 ahi = (R128_U32)(a >> 32);
   R128_U32 blo = (R128_U32)b;
   R128_U32 bhi = (R128_U32)(b >> 32);
   R128_U64 p0, p1, p2, p3;

   p0 = r128__umul64(alo, blo);
   p1 = r128__umul64(alo, bhi);
   p2 = r128__umul64(ahi, blo);
   p3 = r128__umul64(ahi, bhi);

   {
#if R128_INTEL
      R128_U32 r0, r1, r2, r3;
      unsigned char carry;

      r0 = (R128_U32)(p0);
      r1 = (R128_U32)(p0 >> 32);
      r2 = (R128_U32)(p1 >> 32);
      r3 = (R128_U32)(p3 >> 32);

      carry = _addcarry_u32(0, r1, (R128_U32)p1, &r1);
      carry = _addcarry_u32(carry, r2, (R128_U32)(p2 >> 32), &r2);
      _addcarry_u32(carry, r3, 0, &r3);
      carry = _addcarry_u32(0, r1, (R128_U32)p2, &r1);
      carry = _addcarry_u32(carry, r2, (R128_U32)p3, &r2);
      _addcarry_u32(carry, r3, 0, &r3);

      R128_SET4(dst, r0, r1, r2, r3);
#else
      R128_U64 carry, lo, hi;
      carry = ((R128_U64)(R128_U32)p1 + (R128_U64)(R128_U32)p2 + (p0 >> 32)) >> 32;

      lo = p0 + ((p1 + p2) << 32);
      hi = p3 + ((R128_U32)(p1 >> 32) + (R128_U32)(p2 >> 32)) + carry;

      R128_SET2(dst, lo, hi);
#endif
   }
#endif
}

// 128/64->64
#ifdef _M_X64
// MSVC x64 provides neither inline assembly nor a div intrinsic, so we do fake
// "inline assembly" to avoid long division or outline assembly.
#pragma code_seg(".text")
__declspec(allocate(".text")) static const char r128__udiv128Code[] = {
   0x48, 0x8B, 0xC1,       //mov  rax, rcx
   0x49, 0xF7, 0xF0,       //div  rax, r8
   0x49, 0x89, 0x11,       //mov  qword ptr [r9], rdx
   0xC3                    //ret
};
typedef R128_U64 (*r128__udiv128Proc)(R128_U64 nlo, R128_U64 nhi, R128_U64 d, R128_U64 *rem);
static r128__udiv128Proc r128__udiv128 = (r128__udiv128Proc)r128__udiv128Code;
#else
static R128_U64 r128__udiv128(R128_U64 nlo, R128_U64 nhi, R128_U64 d, R128_U64 *rem)
{
#ifdef __x86_64__
   R128_U64 q, r;
   __asm("divq %4"
      : "=a"(q), "=d"(r)
      : "a"(nlo), "d"(nhi), "X"(d));
   *rem = r;
   return q;
#else
   R128_U64 tmp;
   R128_U32 d0, d1;
   R128_U32 n3, n2, n1, n0;
   R128_U32 q0, q1;
   R128_U32 r;
   int shift;

   R128_ASSERT(d != 0);    //division by zero
   R128_ASSERT(nhi < d);   //overflow

   // normalize
   shift = r128__clz64(d);

   if (shift) {
      R128 tmp128;
      R128_SET2(&tmp128, nlo, nhi);
      r128Shl(&tmp128, &tmp128, shift);
      n3 = R128_R3(&tmp128);
      n2 = R128_R2(&tmp128);
      n1 = R128_R1(&tmp128);
      n0 = R128_R0(&tmp128);
      d <<= shift;
   } else {
      n3 = (R128_U32)(nhi >> 32);
      n2 = (R128_U32)nhi;
      n1 = (R128_U32)(nlo >> 32);
      n0 = (R128_U32)nlo;
   }

   d1 = (R128_U32)(d >> 32);
   d0 = (R128_U32)d;

   // first digit
   R128_ASSERT(n3 < d1);
   q1 = r128__udiv64(n2, n3, d1, &r);
refine1:
   if (r128__umul64(q1, d0) > ((R128_U64)r << 32) + n1) {
      --q1;
      if (r < ~n1 + 1) {
         r += n1;
         goto refine1;
      }
   }

   tmp = ((R128_U64)n2 << 32) + n1 - (r128__umul64(q1, d0) + (r128__umul64(q1, d1) << 32));
   n2 = (R128_U32)(tmp >> 32);
   n1 = (R128_U32)tmp;

   // second digit
   R128_ASSERT(n2 < d1);
   q0 = r128__udiv64(n1, n2, d1, &r);
refine0:
   if (r128__umul64(q0, d0) > ((R128_U64)r << 32) + n0) {
      --q0;
      if (r < ~n1 + 1) {
         r += n1;
         goto refine0;
      }
   }

   tmp = ((R128_U64)n1 << 32) + n0 - (r128__umul64(q0, d0) + (r128__umul64(q0, d1) << 32));
   n1 = (R128_U32)(tmp >> 32);
   n0 = (R128_U32)tmp;

   *rem = (((R128_U64)n1 << 32) + n0) >> shift;
   return ((R128_U64)q1 << 32) + q0;
#endif
}
#endif

static int r128__ucmp(const R128 *a, const R128 *b)
{
   if (a->hi == b->hi) {
      if (a->hi > b->hi) {
         return 1;
      } else {
         return -1;
      }
   } else if (a->lo == b->lo) {
      return 0;
   } else {
      if (a->lo > b->lo) {
         return 1;
      } else {
         return -1;
      }
   }
}

static void r128__umul(R128 *dst, const R128 *a, const R128 *b)
{
#ifdef _M_X64
   R128_U64 t0, t1;
   R128_U64 lo, hi = 0;
   unsigned char carry;

   t0 = _umul128(a->lo, b->lo, &t1);
   carry = _addcarry_u64(0, t1, t0 >> 63, &lo);
   _addcarry_u64(carry, hi, hi, &hi);

   t0 = _umul128(a->lo, b->hi, &t1);
   carry = _addcarry_u64(0, lo, t0, &lo);
   _addcarry_u64(carry, hi, t1, &hi);

   t0 = _umul128(a->hi, b->lo, &t1);
   carry = _addcarry_u64(0, lo, t0, &lo);
   _addcarry_u64(carry, hi, t1, &hi);

   t0 = _umul128(a->hi, b->hi, &t1);
   hi += t0;

   R128_SET2(dst, lo, hi);
#elif defined(__x86_64__)
   unsigned __int128 p0, p1, p2, p3;
   p0 = a->lo * (unsigned __int128)b->lo;
   p1 = a->lo * (unsigned __int128)b->hi;
   p2 = a->hi * (unsigned __int128)b->lo;
   p3 = a->hi * (unsigned __int128)b->hi;

   p0 = (p3 << 64) + p2 + p1 + (p0 >> 64) + ((R128_U64)p0 >> 63);
   dst->lo = (R128_U64)p0;
   dst->hi = (R128_U64)(p0 >> 64);
#else
   R128 p0, p1, p2, p3, round;

   r128__umul128(&p0, a->lo, b->lo);
   round.hi = 0; round.lo = p0.lo >> 63;
   p0.lo = p0.hi; p0.hi = 0; //r128Shr(&p0, &p0, 64);
   r128Add(&p0, &p0, &round);

   r128__umul128(&p1, a->hi, b->lo);
   r128Add(&p0, &p0, &p1);

   r128__umul128(&p2, a->lo, b->hi);
   r128Add(&p0, &p0, &p2);

   r128__umul128(&p3, a->hi, b->hi);
   p3.hi = p3.lo; p3.lo = 0; //r128Shl(&p3, &p3, 64);
   r128Add(&p0, &p0, &p3);

   r128Copy(dst, &p0);
#endif
}

// Shift d left until the high bit is set, and shift n left by the same amount.
// returns non-zero on overflow.
static int r128__norm(R128 *n, R128 *d, R128_U64 *n2)
{
   R128_U64 d0, d1;
   R128_U64 n0, n1;
   int shift;

   d1 = d->hi;
   d0 = d->lo;
   n1 = n->hi;
   n0 = n->lo;

   if (d1) {
      shift = r128__clz64(d1);
      if (shift) {
         d1 = (d1 << shift) | (d0 >> (64 - shift));
         d0 = d0 << shift;
         *n2 = n1 >> (64 - shift);
         n1 = (n1 << shift) | (n0 >> (64 - shift));
         n0 = n0 << shift;
      } else {
         *n2 = 0;
      }
   } else {
      shift = r128__clz64(d0);
      if (r128__clz64(n1) >= shift) {
         return 1; // overflow
      }

      if (shift) {
         d1 = d0 << shift;
         d0 = 0;
         *n2 = (n1 << shift) | (n0 >> (64 - shift));
         n1 = n0 << shift;
         n0 = 0;
      } else {
         d1 = d0;
         d0 = 0;
         *n2 = n1;
         n1 = n0;
         n0 = 0;
      }
   }

   R128_SET2(n, n0, n1);
   R128_SET2(d, d0, d1);
   return 0;
}

static void r128__udiv(R128 *quotient, const R128 *dividend, const R128 *divisor)
{
   R128 tmp;
   R128_U64 d0, d1;
   R128_U64 n1, n2, n3;
   R128 q;

   R128_ASSERT(dividend != NULL);
   R128_ASSERT(divisor != NULL);
   R128_ASSERT(quotient != NULL);
   R128_ASSERT(divisor->hi != 0 || divisor->lo != 0);  // divide by zero

   // scale dividend and normalize
   {
      R128 n, d;
      r128Copy(&n, dividend);
      r128Copy(&d, divisor);
      if (r128__norm(&n, &d, &n3)) {
         r128Copy(quotient, &R128_max);
         return;
      }

      d1 = d.hi;
      d0 = d.lo;
      n2 = n.hi;
      n1 = n.lo;
   }

   // first digit
   R128_ASSERT(n3 < d1);
   {
      R128 t0, t1;
      t0.lo = n1;
      q.hi = r128__udiv128(n2, n3, d1, &t0.hi);

refine1:
      r128__umul128(&t1, q.hi, d0);
      if (r128__ucmp(&t1, &t0) > 0) {
         --q.hi;
         if (t0.hi < ~n1 + 1) {
            t0.hi += n1;
            goto refine1;
         }
      }
   }

   {
      R128 t0, t1, t2;
      t0.hi = n2;
      t0.lo = n1;

      r128__umul128(&t1, q.hi, d0);
      r128__umul128(&t2, q.hi, d1);

      t2.hi = t2.lo; t2.lo = 0;  //r128Shl(&t2, &t2, 64);
      r128Add(&tmp, &t1, &t2);
      r128Sub(&tmp, &t0, &tmp);
   }
   n2 = tmp.hi;
   n1 = tmp.lo;

   // second digit
   R128_ASSERT(n2 < d1);
   {
      R128 t0, t1;
      t0.lo = 0;
      q.lo = r128__udiv128(n1, n2, d1, &t0.hi);

   refine0:
      r128__umul128(&t1, q.lo, d0);
      if (r128__ucmp(&t1, &t0) > 0) {
         --q.lo;
         if (t0.hi < ~n1 + 1) {
            t0.hi += n1;
            goto refine0;
         }
      }
   }

   r128Copy(quotient, &q);
}

static R128_U64 r128__umod(R128 *n, R128 *d)
{
   R128_U64 d0, d1;
   R128_U64 n3, n2, n1;
   R128_U64 q;

   R128_ASSERT(d != NULL);
   R128_ASSERT(n != NULL);
   R128_ASSERT(d->hi != 0 || d->lo != 0);  // divide by zero

   if (r128__norm(n, d, &n3)) {
      return R128_LIT_U64(0xffffffffffffffff);
   }

   d1 = d->hi;
   d0 = d->lo;
   n2 = n->hi;
   n1 = n->lo;

   R128_ASSERT(n3 < d1);
   {
      R128 t0, t1;
      t0.lo = n1;
      q = r128__udiv128(n2, n3, d1, &t0.hi);

   refine1:
      r128__umul128(&t1, q, d0);
      if (r128__ucmp(&t1, &t0) > 0) {
         --q;
         if (t0.hi < ~n1 + 1) {
            t0.hi += n1;
            goto refine1;
         }
      }
   }

   return q;
}

static int r128__format(char *dst, size_t dstSize, const R128 *v, const R128ToStringFormat *format)
{
   char buf[128];
   R128 tmp;
   R128_U64 whole;
   char *cursor, *decimal, *dstp = dst;
   int sign = 0;
   int fullPrecision = 1;
   int width, precision;
   int padCnt, trail = 0;

   R128_ASSERT(dst != NULL && dstSize > 0);
   R128_ASSERT(v != NULL);
   R128_ASSERT(format != NULL);

   r128Copy(&tmp, v);
   if (r128IsNeg(&tmp)) {
      r128Neg(&tmp, &tmp);
      sign = 1;
   }

   width = format->width;
   if (width < 0) {
      width = 0;
   }

   precision = format->precision;
   if (precision < 0) {
      // print a maximum of 20 digits
      fullPrecision = 0;
      precision = 20;
   } else if (precision > sizeof(buf) - 21) {
      trail = precision - (sizeof(buf) - 21);
      precision -= trail;
   }

   whole = tmp.hi;
   decimal = cursor = buf;

   // fractional part first in case a carry into the whole part is required
   if (tmp.lo || format->decimal) {
      while (tmp.lo || (fullPrecision && precision)) {
         if ((int)(cursor - buf) == precision) {
            if ((R128_S64)tmp.lo < 0) {
               // round up, propagate carry backwards
               char *c;
               for (c = cursor - 1; c >= buf; --c) {
                  char d = ++*c;
                  if (d <= '9') {
                     goto endfrac;
                  } else {
                     *c = '0';
                  }
               }

               // carry out into the whole part
               whole++;
            }

            break;
         }

         r128__umul128(&tmp, tmp.lo, 10);
         *cursor++ = (char)tmp.hi + '0';
      }

   endfrac:
      if (format->decimal || precision) {
         decimal = cursor;
         *cursor++ = R128_decimal;
      }
   }

   // whole part
   do {
      char digit = (char)(whole % 10);
      whole /= 10;
      *cursor++ = digit + '0';
   } while (whole);

#define R128__WRITE(c) if (dstSize-- == 1) goto finish; *dstp++ = c;

   padCnt = width - (int)(cursor - buf) - 1;

   // left padding
   if (!format->leftAlign) {
      char padChar = format->zeroPad ? '0' : ' ';
      if (format->zeroPad) {
         if (sign) {
            R128__WRITE('-');
         } else if (format->sign == R128ToStringSign_Plus) {
            R128__WRITE('+');
         } else if (format->sign == R128ToStringSign_Space) {
            R128__WRITE(' ');
         } else {
            ++padCnt;
         }
      }

      for (; padCnt > 0; --padCnt) {
         R128__WRITE(padChar);
      }
   }

   if (format->leftAlign || !format->zeroPad) {
      if (sign) {
         R128__WRITE('-');
      } else if (format->sign == R128ToStringSign_Plus) {
         R128__WRITE('+');
      } else if (format->sign == R128ToStringSign_Space) {
         R128__WRITE(' ');
      } else {
         ++padCnt;
      }
   }

   {
      char *i;

      // reverse the whole part
      for (i = cursor - 1; i >= decimal; --i) {
         R128__WRITE(*i);
      }

      // copy the fractional part
      for (i = buf; i < decimal; ++i) {
         R128__WRITE(*i);
      }
   }

   // right padding
   if (format->leftAlign) {
      char padChar = format->zeroPad ? '0' : ' ';
      for (; padCnt > 0; --padCnt) {
         R128__WRITE(padChar);
      }
   }

   // trailing zeroes for very large precision
   while (trail--) {
      R128__WRITE('0');
   }

#undef R128__WRITE

finish:
    *dstp = '\0';
    return (int)(dstp - dst);
}

void r128FromInt(R128 *dst, R128_S64 v)
{
   R128_ASSERT(dst != NULL);
   dst->lo = 0;
   dst->hi = (R128_U64)v;
}

void r128FromFloat(R128 *dst, double v)
{
   R128_ASSERT(dst != NULL);

   if (v < -9223372036854775808.0) {
      r128Copy(dst, &R128_min);
   } else if (v >= 9223372036854775808.0) {
      r128Copy(dst, &R128_max);
   } else {
      R128 r;
      int sign = 0;

      if (v < 0) {
         v = -v;
         sign = 1;
      }

      r.hi = (R128_U64)(R128_S64)v;
      v -= (R128_S64)v;
      r.lo = (R128_U64)(v * 18446744073709551616.0);

      if (sign) {
         r128Neg(&r, &r);
      }

      r128Copy(dst, &r);
   }
}

void r128FromString(R128 *dst, const char *s, char **endptr)
{
   R128_U64 lo = 0, hi = 0;
   R128_U64 base = 10;

   int sign = 0;

   R128_ASSERT(dst != NULL);
   R128_ASSERT(s != NULL);

   R128_SET2(dst, 0, 0);

   // consume whitespace
   for (;;) {
      if (*s == ' ' || *s == '\t' || *s == '\r' || *s == '\n' || *s == '\v') {
         ++s;
      } else {
         break;
      }
   }

   // sign
   if (*s == '-') {
      sign = 1;
      ++s;
   } else if (*s == '+') {
      ++s;
   }

   // parse base prefix
   if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
      base = 16;
      s += 2;
   }

   // whole part
   for (;; ++s) {
      R128_U64 digit;

      if ('0' <= *s && *s <= '9') {
         digit = *s - '0';
      } else if (base == 16 && 'a' <= *s && *s <= 'f') {
         digit = *s - 'a' + 10;
      } else if (base == 16 && 'A' <= *s && *s <= 'F') {
         digit = *s - 'A' + 10;
      } else {
         break;
      }

      hi = hi * base + digit;
   }

   // fractional part
   if (*s == R128_decimal) {
      const char *exp = ++s;

      // find the last digit and work backwards
      for (;; ++s) {
         if ('0' <= *s && *s <= '9') {
         } else if (base == 16 && ('a' <= *s && *s <= 'f')) {
         } else if (base == 16 && ('A' <= *s && *s <= 'F')) {
         } else {
            break;
         }
      }

      for (--s; s >= exp; --s) {
         R128_U64 digit, unused;

         if ('0' <= *s && *s <= '9') {
            digit = *s - '0';
         } else if ('a' <= *s && *s <= 'f') {
            digit = *s - 'a' + 10;
         } else {
            digit = *s - 'A' + 10;
         }

         lo = r128__udiv128(lo, digit, base, &unused);
      }
   }

   R128_SET2(dst, lo, hi);
   if (sign) {
      r128Neg(dst, dst);
   }

   if (endptr) {
      *endptr = (char *) s;
   }
}

R128_S64 r128ToInt(const R128 *v)
{
   R128_ASSERT(v != NULL);
   return (R128_S64)v->hi;
}

double r128ToFloat(const R128 *v)
{
   R128 tmp;
   int sign = 0;
   double d;

   R128_ASSERT(v != NULL);

   r128Copy(&tmp, v);
   if (r128IsNeg(&tmp)) {
      r128Neg(&tmp, &tmp);
      sign = 1;
   }

   d = tmp.hi + tmp.lo * (1 / 18446744073709551616.0);
   if (sign) {
      d = -d;
   }

   return d;
}

int r128ToStringOpt(char *dst, size_t dstSize, const R128 *v, const R128ToStringFormat *opt)
{
   return r128__format(dst, dstSize, v, opt);
}

int r128ToStringf(char *dst, size_t dstSize, const char *format, const R128 *v)
{
   R128ToStringFormat opts;

   R128_ASSERT(dst != NULL && dstSize);
   R128_ASSERT(format != NULL);
   R128_ASSERT(v != NULL);

   opts.sign = R128__defaultFormat.sign;
   opts.precision = R128__defaultFormat.precision;
   opts.zeroPad = R128__defaultFormat.zeroPad;
   opts.decimal = R128__defaultFormat.decimal;
   opts.leftAlign = R128__defaultFormat.leftAlign;

   if (*format == '%') {
      ++format;
   }

   // flags field
   for (;; ++format) {
      if (*format == ' ' && opts.sign != R128ToStringSign_Plus) {
         opts.sign = R128ToStringSign_Space;
      } else if (*format == '+') {
         opts.sign = R128ToStringSign_Plus;
      } else if (*format == '0') {
         opts.zeroPad = 1;
      } else if (*format == '-') {
         opts.leftAlign = 1;
      } else if (*format == '#') {
         opts.decimal = 1;
      } else {
         break;
      }
   }

   // width field
   opts.width = 0;
   for (;;) {
      if ('0' <= *format && *format <= '9') {
         opts.width = opts.width * 10 + *format++ - '0';
      } else {
         break;
      }
   }

   // precision field
   if (*format == '.') {
      opts.precision = 0;
      ++format;
      for (;;) {
         if ('0' <= *format && *format <= '9') {
            opts.precision = opts.precision * 10 + *format++ - '0';
         } else {
            break;
         }
      }
   }

   return r128__format(dst, dstSize, v, &opts);
}

int r128ToString(char *dst, size_t dstSize, const R128 *v)
{
   return r128__format(dst, dstSize, v, &R128__defaultFormat);
}

void r128Copy(R128 *dst, const R128 *src)
{
   R128_ASSERT(dst != NULL);
   R128_ASSERT(src != NULL);
   dst->lo = src->lo;
   dst->hi = src->hi;
}

void r128Neg(R128 *dst, const R128 *src)
{
   unsigned char carry = 0;

   R128_ASSERT(dst != NULL);
   R128_ASSERT(src != NULL);

#if R128_INTEL
#  if R128_64BIT
   carry = _addcarry_u64(carry, ~src->lo, 1, &dst->lo);
   carry = _addcarry_u64(carry, ~src->hi, 0, &dst->hi);
#  else
   R128_U32 r0, r1, r2, r3;
   carry = _addcarry_u32(carry, ~R128_R0(src), 1, &r0);
   carry = _addcarry_u32(carry, ~R128_R1(src), 0, &r1);
   carry = _addcarry_u32(carry, ~R128_R2(src), 0, &r2);
   carry = _addcarry_u32(carry, ~R128_R3(src), 0, &r3);
   R128_SET4(dst, r0, r1, r2, r3);
#  endif //R128_64BIT
#else
   r128Not(dst, a);
   r128Add(dst, dst, &R128_smallest);
#endif   //R128_INTEL
}

void r128Not(R128 *dst, const R128 *src)
{
   R128_ASSERT(dst != NULL);
   R128_ASSERT(src != NULL);

   dst->lo = ~src->lo;
   dst->hi = ~src->hi;
}

void r128Or(R128 *dst, const R128 *a, const R128 *b)
{
   R128_ASSERT(dst != NULL);
   R128_ASSERT(a != NULL);
   R128_ASSERT(b != NULL);

   dst->lo = a->lo | b->lo;
   dst->hi = a->hi | b->hi;
}

void r128And(R128 *dst, const R128 *a, const R128 *b)
{
   R128_ASSERT(dst != NULL);
   R128_ASSERT(a != NULL);
   R128_ASSERT(b != NULL);

   dst->lo = a->lo & b->lo;
   dst->hi = a->hi & b->hi;
}

void r128Xor(R128 *dst, const R128 *a, const R128 *b)
{
   R128_ASSERT(dst != NULL);
   R128_ASSERT(a != NULL);
   R128_ASSERT(b != NULL);

   dst->lo = a->lo ^ b->lo;
   dst->hi = a->hi ^ b->hi;
}

void r128Shl(R128 *dst, const R128 *src, int amount)
{
   R128_U64 r[4];

   R128_ASSERT(dst != NULL);
   R128_ASSERT(src != NULL);

#ifdef _M_IX86
   __asm {
      // load src
      mov ebx, dword ptr[src]
      mov ecx, amount

      mov edi, dword ptr[ebx + 12]
      mov esi, dword ptr[ebx + 8]
      mov edx, dword ptr[ebx + 4]
      mov eax, dword ptr[ebx]

      shl ecx, 3        // ch = amount / 32
      shr cl, 3         // cl = amount mod 32

      // shift mod 32
      shld edi, esi, cl
      shld esi, edx, cl
      shld edx, eax, cl
      shl eax, cl

      // clear out low 12 bytes of stack
      xor ebx, ebx
      mov dword ptr[r], ebx
      mov dword ptr[r + 4], ebx
      mov dword ptr[r + 8], ebx

      // store shifted amount offset by ch*4
      shr ecx, 6
      and ecx, 15
      mov dword ptr[r + ecx + 0], eax
      mov dword ptr[r + ecx + 4], edx
      mov dword ptr[r + ecx + 8], esi
      mov dword ptr[r + ecx + 12], edi
   }
#else

   r[0] = src->lo;
   r[1] = src->hi;

   amount &= 127;
   if (amount >= 64) {
      r[1] = r[0] << (amount - 64);
      r[0] = 0;
   } else if (amount) {
#  ifdef _M_X64
      r[1] = __shiftleft128(r[0], r[1], (char) amount);
#  else
      r[1] = (r[1] << amount) | (r[0] >> (64 - amount));
#  endif
      r[0] = r[0] << amount;
   }
#endif   //_M_IX86

   dst->lo = r[0];
   dst->hi = r[1];
}

void r128Shr(R128 *dst, const R128 *src, int amount)
{
   R128_U64 r[4];

   R128_ASSERT(dst != NULL);
   R128_ASSERT(src != NULL);

#ifdef _M_IX86
   __asm {
      // load src
      mov ebx, dword ptr[src]
      mov ecx, amount

      mov edi, dword ptr[ebx + 12]
      mov esi, dword ptr[ebx + 8]
      mov edx, dword ptr[ebx + 4]
      mov eax, dword ptr[ebx]

      shl ecx, 3        // ch = amount / 32
      shr cl, 3         // cl = amount mod 32

      // shift mod 32
      shrd eax, edx, cl
      shrd edx, esi, cl
      shrd esi, edi, cl
      shr edi, cl

      // clear out high 12 bytes of stack
      xor ebx, ebx
      mov dword ptr[r + 20], ebx
      mov dword ptr[r + 24], ebx
      mov dword ptr[r + 28], ebx

      // store shifted amount offset by -ch*4
      shr ecx, 6
      and ecx, 15
      neg ecx
      mov dword ptr[r + ecx + 16], eax
      mov dword ptr[r + ecx + 20], edx
      mov dword ptr[r + ecx + 24], esi
      mov dword ptr[r + ecx + 28], edi
   }
#else
   r[2] = src->lo;
   r[3] = src->hi;

   amount &= 127;
   if (amount >= 64) {
      r[2] = r[3] >> (amount - 64);
      r[3] = 0;
   } else if (amount) {
#ifdef _M_X64
      r[2] = __shiftright128(r[2], r[3], (char) amount);
#else
      r[2] = (r[2] >> amount) | (r[3] << (64 - amount));
#endif
      r[3] = r[3] >> amount;
   }
#endif

   dst->lo = r[2];
   dst->hi = r[3];
}

void r128Sar(R128 *dst, const R128 *src, int amount)
{
   R128_U64 r[4];

   R128_ASSERT(dst != NULL);
   R128_ASSERT(src != NULL);

#ifdef _M_IX86
   __asm {
      // load src
      mov ebx, dword ptr[src]
      mov ecx, amount

      mov edi, dword ptr[ebx + 12]
      mov esi, dword ptr[ebx + 8]
      mov edx, dword ptr[ebx + 4]
      mov eax, dword ptr[ebx]

      shl ecx, 3        // ch = amount / 32
      shr cl, 3         // cl = amount mod 32

      // shift mod 32
      shrd eax, edx, cl
      shrd edx, esi, cl
      shrd esi, edi, cl
      sar edi, cl

      // copy sign to high 12 bytes of stack
      mov ebx, edi
      sar ebx, 31
      mov dword ptr[r + 20], ebx
      mov dword ptr[r + 24], ebx
      mov dword ptr[r + 28], ebx

      // store shifted amount offset by -ch*4
      shr ecx, 6
      and ecx, 15
      neg ecx
      mov dword ptr[r + ecx + 16], eax
      mov dword ptr[r + ecx + 20], edx
      mov dword ptr[r + ecx + 24], esi
      mov dword ptr[r + ecx + 28], edi
   }
#else
   r[2] = src->lo;
   r[3] = src->hi;

   amount &= 127;
   if (amount >= 64) {
      r[2] = (R128_U64)((R128_S64)r[3] >> (amount - 64));
      r[3] = (R128_U64)((R128_S64)r[3] >> 63);
   } else if (amount) {
      r[2] = (r[2] >> amount) | (R128_U64)((R128_S64)r[3] << (64 - amount));
      r[3] = (R128_U64)((R128_S64)r[3] >> amount);
   }
#endif

   dst->lo = r[2];
   dst->hi = r[3];
}

void r128Add(R128 *dst, const R128 *a, const R128 *b)
{
   unsigned char carry = 0;
   R128_ASSERT(dst != NULL);
   R128_ASSERT(a != NULL);
   R128_ASSERT(b != NULL);

#if R128_INTEL
#  if R128_64BIT
   carry = _addcarry_u64(carry, a->lo, b->lo, &dst->lo);
   carry = _addcarry_u64(carry, a->hi, b->hi, &dst->hi);
#  else
   R128_U32 r0, r1, r2, r3;
   carry = _addcarry_u32(carry, R128_R0(a), R128_R0(b), &r0);
   carry = _addcarry_u32(carry, R128_R1(a), R128_R1(b), &r1);
   carry = _addcarry_u32(carry, R128_R2(a), R128_R2(b), &r2);
   carry = _addcarry_u32(carry, R128_R3(a), R128_R3(b), &r3);
   R128_SET4(dst, r0, r1, r2, r3);
#  endif //R128_64BIT
#else
   R128_U64 r = a->lo + b->lo;
   carry = r < a->lo;
   dst->lo = r;
   dst->hi = a->hi + b->hi + carry;
#endif   //R128_INTEL
}

void r128Sub(R128 *dst, const R128 *a, const R128 *b)
{
   unsigned char borrow = 0;
   R128_ASSERT(dst != NULL);
   R128_ASSERT(a != NULL);
   R128_ASSERT(b != NULL);

#if R128_INTEL
#  if R128_64BIT
   borrow = _subborrow_u64(borrow, a->lo, b->lo, &dst->lo);
   borrow = _subborrow_u64(borrow, a->hi, b->hi, &dst->hi);
#  else
   R128_U32 r0, r1, r2, r3;
   borrow = _subborrow_u32(borrow, R128_R0(a), R128_R0(b), &r0);
   borrow = _subborrow_u32(borrow, R128_R1(a), R128_R1(b), &r1);
   borrow = _subborrow_u32(borrow, R128_R2(a), R128_R2(b), &r2);
   borrow = _subborrow_u32(borrow, R128_R3(a), R128_R3(b), &r3);
   R128_SET4(dst, r0, r1, r2, r3);
#  endif //R128_64BIT
#else
   R128_U64 r = a->lo - b->lo;
   borrow = r > a->lo;
   dst->lo = r;
   dst->hi = a->hi - b->hi - borrow;
#endif   //R128_INTEL
}

void r128Mul(R128 *dst, const R128 *a, const R128 *b)
{
   int sign = 0;
   R128 ta, tb, tc;

   R128_ASSERT(dst != NULL);
   R128_ASSERT(a != NULL);
   R128_ASSERT(b != NULL);

   r128Copy(&ta, a);
   r128Copy(&tb, b);

   if (r128IsNeg(&ta)) {
      r128Neg(&ta, &ta);
      sign = !sign;
   }
   if (r128IsNeg(&tb)) {
      r128Neg(&tb, &tb);
      sign = !sign;
   }

   r128__umul(&tc, &ta, &tb);
   if (sign) {
      r128Neg(&tc, &tc);
   }

   r128Copy(dst, &tc);
}

void r128Div(R128 *dst, const R128 *a, const R128 *b)
{
   int sign = 0;
   R128 tn, td, tq;

   R128_ASSERT(dst != NULL);
   R128_ASSERT(a != NULL);
   R128_ASSERT(b != NULL);

   r128Copy(&tn, a);
   r128Copy(&td, b);

   if (r128IsNeg(&tn)) {
      r128Neg(&tn, &tn);
      sign = !sign;
   }

   if (td.lo == 0 && td.hi == 0) {
      // divide by zero
      if (sign) {
         r128Copy(dst, &R128_min);
      } else {
         r128Copy(dst, &R128_max);
      }
      return;
   } else if (r128IsNeg(&td)) {
      r128Neg(&td, &td);
      sign = !sign;
   }

   r128__udiv(&tq, &tn, &td);

   if (sign) {
      r128Neg(&tq, &tq);
   }

   r128Copy(dst, &tq);
}

void r128Mod(R128 *dst, const R128 *a, const R128 *b)
{
   int sign = 0;
   R128 tn, td, tq;

   R128_ASSERT(dst != NULL);
   R128_ASSERT(a != NULL);
   R128_ASSERT(b != NULL);

   r128Copy(&tn, a);
   r128Copy(&td, b);

   if (r128IsNeg(&tn)) {
      r128Neg(&tn, &tn);
      sign = !sign;
   }

   if (td.lo == 0 && td.hi == 0) {
      // divide by zero
      if (sign) {
         r128Copy(dst, &R128_min);
      } else {
         r128Copy(dst, &R128_max);
      }
      return;
   } else if (r128IsNeg(&td)) {
      r128Neg(&td, &td);
      sign = !sign;
   }

   tq.hi = r128__umod(&tn, &td);
   tq.lo = 0;

   if (sign) {
      tq.hi = ~tq.hi + 1;
   }

   r128Mul(&tq, &tq, b);
   r128Sub(dst, a, &tq);
}

int r128Cmp(const R128 *a, const R128 *b)
{
   R128_ASSERT(a != NULL);
   R128_ASSERT(b != NULL);

   if (a->hi == b->hi) {
      if (a->lo == b->lo) {
         return 0;
      } else if (a->lo > b->lo) {
         return 1;
      } else {
         return -1;
      }
   } else if ((R128_S64)a->hi > (R128_S64)b->hi) {
      return 1;
   } else {
      return -1;
   }
}

int r128IsNeg(const R128 *v)
{
   R128_ASSERT(v != NULL);

   return (R128_S64)v->hi < 0;
}

void r128Min(R128 *dst, const R128 *a, const R128 *b)
{
   R128_ASSERT(dst != NULL);
   R128_ASSERT(a != NULL);
   R128_ASSERT(b != NULL);

   if (r128Cmp(a, b) < 0) {
      r128Copy(dst, a);
   } else {
      r128Copy(dst, b);
   }
}

void r128Max(R128 *dst, const R128 *a, const R128 *b)
{
   R128_ASSERT(dst != NULL);
   R128_ASSERT(a != NULL);
   R128_ASSERT(b != NULL);

   if (r128Cmp(a, b) > 0) {
      r128Copy(dst, a);
   } else {
      r128Copy(dst, b);
   }
}

void r128Floor(R128 *dst, const R128 *v)
{
   R128_ASSERT(dst != NULL);
   R128_ASSERT(v != NULL);

   if ((R128_S64)v->hi < 0) {
      dst->hi = v->hi - (v->lo != 0);
   } else {
      dst->hi = v->hi;
   }
   dst->lo = 0;
}

void r128Ceil(R128 *dst, const R128 *v)
{
   R128_ASSERT(dst != NULL);
   R128_ASSERT(v != NULL);

   if ((R128_S64)v->hi > 0) {
      dst->hi = v->hi + (v->lo != 0);
   } else {
      dst->hi = v->hi;
   }
   dst->lo = 0;
}

#endif   //R128_IMPLEMENTATION
