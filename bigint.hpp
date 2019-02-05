#ifndef __FLAW_BIGINT_HPP__
#define __FLAW_BIGINT_HPP__ 1

#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <algorithm>
#include <functional>
#include <vector>
#include <string>


namespace math
{
  class bigint
  {
    friend bigint operator - (bigint x);

    static_assert(sizeof(long long) == 8, "bigint error");

  public:

#define __BIGINT_WORDSIZE__ 32

    typedef uint32_t word_type;
    typedef uint64_t dword_type;

    typedef std::vector<word_type> number_type;
    typedef uint64_t size_type;

    static constexpr char __word_size__ = 8 * sizeof(word_type);
    static constexpr word_type __word_max__ = static_cast<word_type>(-1);

#define __math_bigint_multiply_karatsuba_threshold__ 75
#define __math_bigint_square_karatsuba_threshold__ 75 // aleatorio
#define __math_bigint_small_to_string_threshold__ 1

  private:

    bool number_sign;
    number_type number;

  private:

    void shl(size_type n);
    void shr(size_type n);

    void add(const number_type& __y);

    void subtract(const number_type& __y);
    void rsubtract(const number_type& __y);

    bigint karatsuba_lower(size_type __pos) const;
    bigint karatsuba_upper(size_type __pos) const;

    void multiply(const number_type& __y);
    void multiply_karatsuba(const bigint& __y);
    bigint square_karatsuba() const;
    bigint square_colin() const;

    static void divide_knuth(bigint& __quot, bigint* const __rem, const bigint& numer, const bigint& __denom);

    struct schoenhage
    {
      const uint8_t _start_size;
      const uint8_t _base;
      const uint8_t _digits;
      const uint64_t _maximum;
      std::string _converted;
      std::function<char(char)> to_char;
      std::vector<std::string> digitGroup;

      schoenhage(bigint x, uint8_t __base)
      : _start_size(x.number_sign),
        _base(__base),
        _digits(log(std::numeric_limits<unsigned long long>::max()) / log(__base) - 1e-6),
        _maximum(pow(_base, _digits) + 1e-9),
        _converted()
      {
        assert(x);
        assert(__base >= 2 && __base <= 36);

        if (x.number_sign)
        {
          x.number_sign = 0;
          _converted = "-";
        }

        if (__base <= 10)
          to_char = [](char c){ return c + '0'; };
        else
          to_char = [](char c){ return c + (c <= '9' ? '0' : 'a' - 10); };

        digitGroup.reserve((4 * __math_bigint_small_to_string_threshold__ + 6) / 7);
        // cout << " -- " << _maximum << ' ' << (int)_digits << endl;
        to_string(x, 0);
      }

    private:

      void to_string(bigint& x, size_type __expected);
    };

  public:

    bigint()
    : number_sign(0),
      number()
    {}

    bigint(const bigint& x)
    : number_sign(x.number_sign),
      number(x.number)
    {}

    bigint(bigint&& x)
    : number_sign(x.number_sign),
      number(std::move(x.number))
    {}

    bigint(char x)
    : number_sign(0),
      number()
    { *this = x; }

    bigint(signed char x)
    : number_sign(0),
      number()
    { *this = x; }

    bigint(unsigned char x)
    : number_sign(0),
      number()
    { *this = x; }

    bigint(short x)
    : number_sign(0),
      number()
    { *this = x; }

    bigint(unsigned short x)
    : number_sign(0),
      number()
    { *this = x; }

    bigint(int x)
    : number_sign(0),
      number()
    { *this = x; }

    bigint(unsigned int x)
    : number_sign(0),
      number()
    { *this = x; }

    bigint(long x)
    : number_sign(0),
      number()
    { *this = x; }

    bigint(unsigned long x)
    : number_sign(0),
      number()
    { *this = x; }

    bigint(long long x)
    : number_sign(0),
      number()
    { *this = x; }

    bigint(unsigned long long x)
    : number_sign(0),
      number()
    { *this = x; }

    bigint(const char* __s)
    : number_sign(0),
      number()
    { set_string(__s); }

    bigint(const char* __s, size_type n, uint8_t __base = 10)
    : number_sign(0),
      number()
    { set_string(__s, n, __base); }

    bigint(const std::string& __s, uint8_t __base = 10)
    : number_sign(0),
      number()
    { set_string(__s, __base); }

    bigint& operator = (const bigint& x)
    {
      number = x.number;
      number_sign = x.number_sign;
      return *this;
    }

    bigint& operator = (bigint&& x)
    {
      if (this == &x)
        return *this;

      number = std::move(x.number);
      number_sign = x.number_sign;
      return *this;
    }

    bigint& operator = (char x);
    bigint& operator = (signed char x);
    bigint& operator = (unsigned char x);

    bigint& operator = (short x);
    bigint& operator = (unsigned short x);

    bigint& operator = (int x);
    bigint& operator = (unsigned int x);

    bigint& operator = (long x);
    bigint& operator = (unsigned long x);

    bigint& operator = (long long x);
    bigint& operator = (unsigned long long x);

    bigint& operator = (const char* __s)
    {
      set_string(__s);
      return *this;
    }

    bigint& operator = (const std::string& __s)
    {
      set_string(__s);
      return *this;
    }

    void set_string(const char* __s)
    { set_string(__s, strlen(__s)); }

    void set_string(const char* __s, size_type n, uint8_t __base = 10);

    void set_string(const std::string& __s, uint8_t __base = 10)
    { set_string(__s.c_str(), __s.size(), __base); }

    std::string to_string(uint8_t __base = 10) const;

    int ucompare(const bigint& __big) const;

    int compare(const bigint& __big) const;

    bool operator ! () const
    { return number.empty(); }

    operator bool () const
    { return !number.empty(); }

    operator short () const
    { return static_cast<unsigned long long>(*this); }

    operator unsigned short () const
    { return static_cast<unsigned long long>(*this); }

    operator int () const
    { return static_cast<unsigned long long>(*this); }

    operator unsigned int () const
    { return static_cast<unsigned long long>(*this); }

    operator long () const
    { return static_cast<unsigned long long>(*this); }

    operator unsigned long () const
    { return static_cast<unsigned long long>(*this); }

    operator long long () const
    { return static_cast<unsigned long long>(*this); }

    operator unsigned long long () const;

    operator std::string () const
    { return to_string(); }

    size_type bit_length() const;

    bigint square() const;

    std::pair<bigint,bigint> div(const bigint& __denom) const;

    bigint& operator += (const bigint& __big);
    bigint& operator -= (const bigint& __big);
    bigint& operator *= (const bigint& __big);
    bigint& operator /= (const bigint& __big);
    bigint& operator %= (const bigint& __big);

    bigint& operator <<= (short n)
    {
      n > 0 ? shl(n) : shr(-n);
      return *this;
    }

    bigint& operator <<= (unsigned short n)
    {
      shl(n);
      return *this;
    }

    bigint& operator <<= (int n)
    {
      n > 0 ? shl(n) : shr(-n);
      return *this;
    }

    bigint& operator <<= (unsigned int n)
    {
      shl(n);
      return *this;
    }

    bigint& operator <<= (long n)
    {
      n > 0 ? shl(n) : shr(-n);
      return *this;
    }

    bigint& operator <<= (unsigned long n)
    {
      shl(n);
      return *this;
    }

    bigint& operator <<= (long long n)
    {
      n > 0 ? shl(n) : shr(-n);
      return *this;
    }

    bigint& operator <<= (unsigned long long n)
    {
      shl(n);
      return *this;
    }

    bigint& operator >>= (short n)
    {
      n > 0 ? shr(n) : shl(-n);
      return *this;
    }

    bigint& operator >>= (unsigned short n)
    {
      shr(n);
      return *this;
    }

    bigint& operator >>= (int n)
    {
      n > 0 ? shr(n) : shl(-n);
      return *this;
    }

    bigint& operator >>= (unsigned int n)
    {
      shr(n);
      return *this;
    }

    bigint& operator >>= (long n)
    {
      n > 0 ? shr(n) : shl(-n);
      return *this;
    }

    bigint& operator >>= (unsigned long n)
    {
      shr(n);
      return *this;
    }

    bigint& operator >>= (long long n)
    {
      n > 0 ? shr(n) : shl(-n);
      return *this;
    }

    bigint& operator >>= (unsigned long long n)
    {
      shr(n);
      return *this;
    }

  };  /// \class bigint

  inline bool operator == (const bigint& x, const bigint& y)
  { return (x.compare(y) == 0); }

  inline bool operator != (const bigint& x, const bigint& y)
  { return (x.compare(y) != 0); }

  inline bool operator < (const bigint& x, const bigint& y)
  { return (x.compare(y) < 0); }

  inline bool operator <= (const bigint& x, const bigint& y)
  { return (x.compare(y) <= 0); }

  inline bool operator > (const bigint& x, const bigint& y)
  { return (x.compare(y) > 0); }

  inline bool operator >= (const bigint& x, const bigint& y)
  { return (x.compare(y) >= 0); }

  inline bigint operator - (bigint x)
  {
    if (x)
      x.number_sign ^= 1;
    return std::move(x);
  }

  inline bigint operator + (bigint x)
  { return std::move(x); }

  inline bigint operator + (bigint x, const bigint& y)
  { return std::move(x += y); }

  inline bigint operator - (bigint x, const bigint& y)
  { return std::move(x -= y); }

  inline bigint operator * (bigint x, const bigint& y)
  { return std::move(x *= y); }

  inline bigint operator / (bigint x, const bigint& y)
  { return std::move(x /= y); }

  inline bigint operator % (bigint x, const bigint& y)
  { return std::move(x %= y); }

  inline bigint operator << (bigint x, short y)
  { return std::move(x <<= y); }

  inline bigint operator << (bigint x, unsigned short y)
  { return std::move(x <<= y); }

  inline bigint operator << (bigint x, int y)
  { return std::move(x <<= y); }

  inline bigint operator << (bigint x, unsigned int y)
  { return std::move(x <<= y); }

  inline bigint operator << (bigint x, long y)
  { return std::move(x <<= y); }

  inline bigint operator << (bigint x, unsigned long y)
  { return std::move(x <<= y); }

  inline bigint operator << (bigint x, long long y)
  { return std::move(x <<= y); }

  inline bigint operator << (bigint x, unsigned long long y)
  { return std::move(x <<= y); }

  inline bigint operator >> (bigint x, short y)
  { return std::move(x >>= y); }

  inline bigint operator >> (bigint x, unsigned short y)
  { return std::move(x >>= y); }

  inline bigint operator >> (bigint x, int y)
  { return std::move(x >>= y); }

  inline bigint operator >> (bigint x, unsigned int y)
  { return std::move(x >>= y); }

  inline bigint operator >> (bigint x, long y)
  { return std::move(x >>= y); }

  inline bigint operator >> (bigint x, unsigned long y)
  { return std::move(x >>= y); }

  inline bigint operator >> (bigint x, long long y)
  { return std::move(x >>= y); }

  inline bigint operator >> (bigint x, unsigned long long y)
  { return std::move(x >>= y); }

  std::istream& operator >> (std::istream& __in, bigint& x)
  {
    std::string s;
    /// corrigir para aceitar apenas números
    __in >> s;
    /// checar a base do istream
    x.set_string(s);
    return __in;
  }

  std::ostream& operator << (std::ostream& __out, const bigint& x)
  {
    /// checar a base do ostream
    return __out << x.to_string(10);
  }

} /// \namespace math

#endif // __FLAW_BIGINT_HPP__
