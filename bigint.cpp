#ifndef __FLAW_BIGINT_CPP__
#define __FLAW_BIGINT_CPP__ 1


namespace math
{
  bigint& bigint::bigint::operator = (char x)
  {
    if (x == 0)
    {
      number_sign = 0;
      number.clear();
    }
    else
    {
      if ((number_sign = x < 0))
        x = -x;
      number.assign(1, x);
    }

    return *this;
  }

  bigint& bigint::bigint::operator = (signed char x)
  {
    if (x == 0)
    {
      number_sign = 0;
      number.clear();
    }
    else
    {
      if ((number_sign = x < 0))
        x = -x;
      number.assign(1, x);
    }

    return *this;
  }

  bigint& bigint::operator = (unsigned char x)
  {
    number_sign = 0;
    if (x == 0)
      number.clear();
    else
      number.assign(1, x);

    return *this;
  }

  bigint& bigint::bigint::operator = (short x)
  {
    if (x == 0)
    {
      number_sign = 0;
      number.clear();
    }
    else
    {
      if ((number_sign = x < 0))
        x = -x;
      number.assign(1, x);
    }

    return *this;
  }

  bigint& bigint::operator = (unsigned short x)
  {
    number_sign = 0;
    if (x == 0)
      number.clear();
    else
      number.assign(1, x);

    return *this;
  }

  bigint& bigint::operator = (int x)
  {
    if (x == 0)
    {
      number_sign = 0;
      number.clear();
    }
    else
    {
      if ((number_sign = x < 0))
        x = -x;
      number.assign(1, x);
    }

    return *this;
  }

  bigint& bigint::operator = (unsigned int x)
  {
    number_sign = 0;
    if (x == 0)
      number.clear();
    else
      number.assign(1, x);

    return *this;
  }

  bigint& bigint::operator = (long x)
  {
    if (x == 0)
    {
      number_sign = 0;
      number.clear();
    }
    else
    {
      if ((number_sign = x < 0))
        x = -x;
#if LONG_MAX == 0x7fffffff || __BIGINT_WORDSIZE__ == 64
      number.assign(1, x);
#else
      if (x <= __word_max__)
        number.assign(1, x);
      else
      {
        number.resize(2);
        number[0] = x & 0xffffffff;
        number[1] = x >> 32;
      }
#endif
    }

    return *this;
  }

  bigint& bigint::operator = (unsigned long x)
  {
    number_sign = 0;
    if (x == 0)
      number.clear();
    else
    {
#if ULONG_MAX == 0xffffffff || __BIGINT_WORDSIZE__ == 64
      number.assign(1, x);
#else
      if (x <= __word_max__)
        number.assign(1, x);
      else
      {
        number.resize(2);
        number[0] = x & 0xffffffff;
        number[1] = x >> 32;
      }
#endif
    }

    return *this;
  }

  bigint& bigint::operator = (long long x)
  {
    if (x == 0)
    {
      number_sign = 0;
      number.clear();
    }
    else
    {
      if ((number_sign = x < 0))
        x = -x;
#if __BIGINT_WORDSIZE__ == 64
      number.assign(1, x);
#else
      if (x <= __word_max__)
        number.assign(1, x);
      else
      {
        number.resize(2);
        number[0] = x & 0xffffffff;
        number[1] = x >> 32;
      }
#endif
    }

    return *this;
  }

  bigint& bigint::operator = (unsigned long long x)
  {
    number_sign = 0;
    if (x == 0)
      number.clear();
    else
    {
#if __BIGINT_WORDSIZE__ == 64
      number.assign(1, x);
#else
      if (x <= __word_max__)
        number.assign(1, x);
      else
      {
        number.resize(2);
        number[0] = x & 0xffffffff;
        number[1] = x >> 32;
      }
#endif
    }

    return *this;
  }

  void bigint::set_string(const char* __s, size_type n, uint8_t __base)
  {
    assert(__s != nullptr);
    assert(__base >= 2 && __base <= 36);

    size_type i = 0;

    // check for sign
    if (__s[0] == '-')
    {
      number_sign = 1;
      ++i;
    }
    else
    {
      number_sign = 0;
      if (__s[0] == '+')
        ++i;
    }

    // skip leading zeros
    while (i < n && __s[i] == '0')
      ++i;

    // special case when s = "0"
    number.clear();
    if (i == n)
    {
      number_sign = 0;
      return;
    }

    // compute upper bound for expected size
    number.reserve((static_cast<size_type>((n - i) * log2(__base))) / __word_size__ + 1);

    // calculate the number of digits that fit in 64 bits
    const int digits_word = log(__word_max__) / log(__base) - 1e-6;
    const word_type super = pow(__base, digits_word) + 1e-9;

    assert(super > 0);
    assert((dword_type)__base * super > __word_max__);

    // define function to faster convert string to one word
    std::function<word_type(size_type, int)> convert;
    if (__base <= 10)
      convert = [__s, __base](size_type __begin, int n)
      {
        const char* s = __s + __begin;
        word_type w = 0;
        for (int i = 0; i < n; ++i)
          w = __base * w + s[i] - '0';
        return w;
      };
    else
      convert = [__s, __base](size_type __begin, int n)
      {
        const char* s = __s + __begin;
        word_type w = 0;
        for (int i = 0; i < n; ++i)
          w = __base * w + (s[i] <= '9' ? s[i] - '0' : (s[i] | 32) - 'a');
        return w;
      };

    // first
    size_type first = (n - i) % digits_word;
    if (first == 0)                     // to avoid multipĺication
      first = digits_word;

    number.push_back(convert(i, first));
    i += first;

    // remaining
    while (i < n)
    {
      *this *= super;
      *this += convert(i, digits_word);
      i += digits_word;
    }
  }

  void bigint::schoenhage::to_string(bigint& x, size_type num_digits)
  {
    if (x.number.size() <= __math_bigint_small_to_string_threshold__)
    {
      std::string s;

      if (!x)
        s = "0";
      else
      {
        assert(digitGroup.empty());

        // Translate number to string, a digit group at a time
        while (x)
        {
          std::pair<bigint,bigint> qr = x.div(_maximum);
          x = qr.first;

          auto rem = (uint64_t)qr.second;
          std::string rem_str;
          if (rem == 0)
            rem_str = "0";
          else
          {
            while (rem > 0)
            {
              rem_str.push_back(to_char(rem % _base));
              rem /= _base;
            }
            reverse(rem_str.begin(), rem_str.end());
          }
          digitGroup.push_back(std::move(rem_str));
        }

        s.append(digitGroup.back());
        digitGroup.pop_back();
        while (!digitGroup.empty())
        {
          s.append(_digits - digitGroup.back().size(), '0');
          s.append(digitGroup.back());
          digitGroup.pop_back();
        }
      }
      if (s.size() < num_digits && _converted.size() > _start_size)
        _converted.append(num_digits - s.size(), '0');
      _converted.append(s);
      return;
    }

    // cache[i][k] = i ** (2 ** k), for 2 <= i <= 36
    static std::vector<bigint> cache[] = {
      std::vector<bigint>({  }), std::vector<bigint>({  }), std::vector<bigint>({ 2}),
      std::vector<bigint>({ 3}), std::vector<bigint>({ 4}), std::vector<bigint>({ 5}),
      std::vector<bigint>({ 6}), std::vector<bigint>({ 7}), std::vector<bigint>({ 8}),
      std::vector<bigint>({ 9}), std::vector<bigint>({10}), std::vector<bigint>({11}),
      std::vector<bigint>({12}), std::vector<bigint>({13}), std::vector<bigint>({14}),
      std::vector<bigint>({15}), std::vector<bigint>({16}), std::vector<bigint>({17}),
      std::vector<bigint>({18}), std::vector<bigint>({19}), std::vector<bigint>({20}),
      std::vector<bigint>({21}), std::vector<bigint>({22}), std::vector<bigint>({23}),
      std::vector<bigint>({24}), std::vector<bigint>({25}), std::vector<bigint>({26}),
      std::vector<bigint>({27}), std::vector<bigint>({28}), std::vector<bigint>({29}),
      std::vector<bigint>({30}), std::vector<bigint>({31}), std::vector<bigint>({32}),
      std::vector<bigint>({33}), std::vector<bigint>({34}), std::vector<bigint>({35}),
      std::vector<bigint>({36})
    };

    // Schoenhage base conversion
    size_type n = (size_type)(log2(x.bit_length() * M_LN2 / log(_base)) - 0.5);

    std::vector<bigint> &cache_line = cache[_base];
    while (cache_line.size() <= n)
      cache_line.push_back(cache_line.back().square());

    size_type expected = 1 << n;        // expected digits
    std::pair<bigint,bigint> qr = x.div(cache_line[n]);

    // build the two halves of each number
    to_string(qr.first, (num_digits > expected ? num_digits - expected : 0));
    to_string(qr.second, expected);
  }

  std::string bigint::to_string(uint8_t __base) const
  {
    if (__base < 2 || __base > 36)
      throw "invalid base";
    if (!*this)
      return std::string("0");

    return schoenhage(*this, __base)._converted;
  }

  void bigint::shl(size_type n)
  {
    if (!n || !*this)
      return;

    size_type shl_bits = n % __word_size__;
    size_type shl_words = n / __word_size__;

    if (shl_bits > 0)
    {
      size_type shr_bits = __word_size__ - shl_bits;
      word_type over = number.back() >> shr_bits;
      for (size_type i = number.size() - 1; i > 0; --i)
        number[i] = (number[i] << shl_bits) | (static_cast<dword_type>(number[i - 1]) >> shr_bits);
      number[0] <<= shl_bits;
      if (over > 0)
        number.push_back(over);
    }
    if (shl_words > 0)
      number.insert(number.begin(), shl_words, 0);
  }

  void bigint::shr(size_type n)
  {
    if (!n || !*this)
      return;
    if (number_sign && number.size() == 1 && number[0] == 1)
      return;

    size_type shr_bits = n % __word_size__;
    size_type shr_words = n / __word_size__;

    if (shr_words >= number.size())
    {
      *this = number_sign ? -1 : 0;
      return;
    }

    bool flag = false;
    if (shr_words > 0)
    {
      if (number_sign)
        for (size_type i = 0; i < shr_words; ++i)
          if (number[i])
          {
            flag = true;
            break;
          }
      number.erase(number.begin(), number.begin() + shr_words);
    }
    if (shr_bits > 0)
    {
      flag |= number[0] & ((static_cast<word_type>(1) << shr_bits) - 1);

      size_type shl_bits = __word_size__ - shr_bits;
      for (size_type i = 0; i < number.size() - 1; ++i)
        number[i] = (number[i] >> shr_bits) | (static_cast<dword_type>(number[i + 1]) << shl_bits);
      number.back() >>= shr_bits;

      if (number.back() == 0)
        number.pop_back();
    }

    if (number_sign && flag)
      *this -= 1;
  }

  void bigint::add(const number_type& y)
  {
    number_type& x = number;
    size_type up = std::min(x.size(), y.size());
    x.reserve(std::max(x.size(), y.size()) + 1);

    size_type i;
    dword_type carry = 0;

    // add common part
    for (i = 0; i < up; ++i)
      x[i] = carry = (carry >> __word_size__) + x[i] + y[i];

    // add remainder
    if (i < y.size())
    {
      if (carry >> __word_size__)
      {
        size_type begin = i;
        for (; i < y.size() && y[i] + 1 == 0; ++i);
        x.insert(x.end() - 1, i - begin, 0);

        if (i == y.size())
        {
          x.push_back(1);
          return;
        }
        x.push_back(y[i] + 1);
        ++i;
      }

      for (; i < y.size(); ++i)
        x.push_back(y[i]);
    }
    else
    {
      if (carry >> __word_size__)
      {
        for (; i < x.size() && ++x[i] == 0; ++i);
        if (i == x.size())
          x.push_back(1);
      }
    }
  }

  void bigint::subtract(const number_type& __y)
  {
    assert(number.size() > 0 && __y.size() > 0);
    assert(number.size() >= __y.size());

    size_type up = __y.size();  // value.size() >= this.size()
    constexpr char carry_sign = (2 * __word_size__ - 1);  // shift to take sign of dword

    // subtract common part
    size_type i;
    dword_type carry = 0;
    for (i = 0; i < up; ++i)  /// unsigned subtrai errado!
      number[i] = carry = (dword_type)number[i] - __y[i] - (carry >> carry_sign);

    // subtract carry, if needed
    if (carry >> carry_sign)
    {
      while (number[i]-- == 0) // vai remover o carry
        ++i;
    }
    if (number.back() == 0)
      number.pop_back();
    // print(number);

    assert(number.back() != 0);
  }

  void bigint::rsubtract(const number_type& __y)
  {
    assert(number.size() > 0 && __y.size() > 0);
    assert(number.size() <= __y.size());

    constexpr char carry_sign = (2 * __word_size__ - 1);  // shift to take sign of dword

    number.reserve(__y.size() + 1);
    size_type up = number.size();  // this.size() <= value.size()
    size_type i;
    dword_type carry = 0;

    // subtract common part
    for (i = 0; i < up; ++i)  /// unsigned subtrai errado!
      number[i] = carry = (dword_type)__y[i] - number[i] - (carry >> carry_sign);

    // subtract carry, if needed
    if (carry >> carry_sign)
    {
      for (; __y[i] == 0; ++i) // vai remover o carry
        number.push_back(static_cast<word_type>(-1));
      number.push_back(__y[i] - 1);
      ++i;
    }
    for (; i < __y.size(); ++i)
      number.push_back(__y[i]);

    assert(number.back() != 0);
  }

  void bigint::multiply(const number_type& y)
  {
    number_type x = number;
    const size_type n = x.size();
    const size_type m = y.size();
    number.assign(n + m, 0);

    assert(n > 0 && m > 0);

    for (size_type i = 0; i < n; ++i)
    {
      dword_type carry = 0;
      const word_type xi = x[i];
      for (size_type j = 0; j < m; ++j)
        number[i + j] = carry = number[i + j] + static_cast<dword_type>(xi) * y[j] + (carry >> __word_size__);
      number[i + m] = carry >> __word_size__;
    }
    if (number.back() == 0)
      number.pop_back();
    assert(number.back() != 0);
  }

  void bigint::multiply_karatsuba(const bigint& y)
  {
    const size_type half = std::max(number.size(), y.number.size()) / 2;
    bigint xl = karatsuba_lower(half);
    bigint xu = karatsuba_upper(half);
    bigint yl = y.karatsuba_lower(half);
    bigint yu = y.karatsuba_upper(half);

    const size_type k = half * __word_size__;
    bigint p1 = xu * yu;
    bigint p2 = xl * yl;

    // (p1 * 2**k + (xl + xu) * (yl + yu) - p1 - p2) * 2**k + p2
    *this = xl;
    *this += xu;
    *this *= yl + yu;
    *this += p1 << k;
    *this -= p1;
    *this -= p2;
    *this <<= k;
    *this += p2;

    assert(number.back() != 0);
  }

  int bigint::ucompare(const bigint& __big) const
  {
    const number_type& other = __big.number;
    if (number.size() != other.size())
      return number.size() < other.size() ? -1 : 1;

    size_type i;
    for (i = number.size(); i--; )
      if (number[i] != other[i])
        return (number[i] < other[i] ? -1 : 1);

    return 0;
  }

  int bigint::compare(const bigint& __big) const
  {
    if (number_sign != __big.number_sign)
      return -number_sign + __big.number_sign;
    return ucompare(__big);
  }

  bigint::operator unsigned long long () const
  {
    typedef unsigned long long ull;

    if (!*this)
      return 0;
#if __BIGINT_WORDSIZE__ == 64
    if (!number_sign)
      return static_cast<ull>(number[0]);
    if (number[0] == 0)
      return ~static_cast<ull>(0);
    return static_cast<ull>(-number[0]);
#else
    if (!number_sign)
    {
      if (number.size() == 1)
        return static_cast<ull>(number[0]);
      return (static_cast<ull>(number[1]) << 32) | static_cast<ull>(number[0]);
    }
    if (number.size() == 1)
      return static_cast<ull>(-number[0]);
    if (number[0] == 0)
      return static_cast<ull>(-number[1]) << 32;
    return (static_cast<ull>(~number[1]) << 32) | static_cast<ull>(-number[0]);
#endif
  }

  bigint::size_type bigint::bit_length() const
  {
    if (!*this)
      return 0;

    size_type n = number.size() * __word_size__ - __builtin_clz(number.back());
    if (!number_sign)
      return n;

    // if is a power of two, subtract 1
    if (__builtin_popcountll(number.back()) != 1)
      return n;
    if (number.size() == 1)
      return n - 1;

    size_type i;
    for (i = number.size() - 2; i && number[i] == 0; --i);
    return number[i] != 0 ? n : n - 1;
  }

  bigint bigint::karatsuba_lower(size_type n) const
  {
    bigint temp;
    if(n >= number.size())
      temp.number = number;
    else
    {
      while (n > 0 && number[n - 1] == 0)
        --n;
      temp.number.assign(number.begin(), number.begin() + n);
      assert(temp.number.empty() || temp.number.back() != 0);
    }
    return temp;
  }

  bigint bigint::karatsuba_upper(size_type n) const
  {
    bigint temp;
    if (n < number.size())
      temp.number.assign(number.begin() + n, number.end());
    return temp;
  }

  bigint bigint::square_karatsuba() const
  {
    size_type half = number.size() / 2;
    bigint l = karatsuba_lower(half);
    bigint u = karatsuba_upper(half);

    // cerr << " >> " << half << ' ' << number << ' ' << l.number << ' ' << u.number << '\n';

    bigint ls = l.square();
    bigint us = u.square();

    // (((u^2 << 32) + (l+u)^2 - (u^2 + l^2)) << 32) + l^2
    bigint t = us;
    t <<= half * __word_size__;
    t += (l + u).square();
    t -= us;
    t -= ls;
    t <<= half * __word_size__;
    t += ls;
    return t;
  }

  bigint bigint::square_colin() const
  {
    bigint sq;
    const number_type& x = number;
    number_type& y = sq.number;

    size_type n = x.size();
    y.resize(2 * n);

    // store the squares, divided by 2
    word_type last = 0;                 // last product low word
    for (size_type i = n - 1, k = 2 * n; k > 0; --i)
    {
      dword_type product = static_cast<dword_type>(x[i]) * x[i];
      y[--k] = (last << (__word_size__ - 1)) | static_cast<word_type>(product >> (__word_size__ + 1));
      y[--k] = static_cast<word_type>(product >> 1);
      last = static_cast<word_type>(product);
    }

    for (size_type i = 0, j; i < n; ++i)
    {
      word_type w = x[i];
      dword_type carry = 0;
      for (j = i + 1; j < n; ++j)
        y[i + j] = carry = (carry >> 32) + y[i + j] + static_cast<dword_type>(w) * x[j];
      if (carry >>= __word_size__)
      {
        j += i;
        y[j] = carry += y[j];
        if (carry >> 32)
          while (++y[++j] == 0);
      }
    }

    while (y.back() == 0)
      y.pop_back();

    sq <<= 1;
    y[0] |= x[0] & 1;

    return sq;
  }

  bigint bigint::square() const
  {
    if (!*this)
      return *this;
    if (number.size() == 1 && number[0] == 1)
      return 1;
    if (number.size() <= __math_bigint_square_karatsuba_threshold__)
      return square_colin();
    return square_karatsuba();
  }

  void bigint::divide_knuth(bigint& __quot, bigint* const __rem,
      const bigint& __numer, const bigint& __denom)
  {
    const number_type& u = __numer.number;
    const number_type& v = __denom.number;
    number_type& q = __quot.number;

    size_type m = u.size();
    size_type n = v.size();

    assert(v.back() != 0);
    assert(m >= n && n > 0);

    q.assign(m - n + 1, 0);

    // Take care of the case of a single-digit divisor here.
    if (n == 1)
    {
      word_type v0 = v[0];
      dword_type k = 0;
      for (size_type j = m - 1; j != static_cast<size_type>(-1); --j)
      {
        k = (k << __word_size__) + u[j];
        q[j] = k / v0;
        k %= v0;
      }
      if (q.back() == 0)
        q.pop_back();
      if (__rem != nullptr)
        *__rem = k;

      assert(!q.empty() && q.back() != 0);
      assert(__rem == nullptr || __rem->number.empty() || __rem->number.back() != 0);
      return;
    }

    /*
     * Normalized form of u, v: normalize by shifting v left just enough
     * so that its high-order bit is on, and shift u left the same amount.
     */
    const char shift = __builtin_clz(v.back());
    assert(0 <= shift && shift < __word_size__);
    bigint u_norm = __numer << shift;
    bigint v_norm = __denom << shift;
    number_type& un = u_norm.number;
    number_type& vn = v_norm.number;

    const word_type v1 = vn[n - 1];
    const word_type v2 = vn[n - 2];

    if (un.size() == m)
      un.push_back(0);

    // Main loop.
    for (size_type j = m - n; j != static_cast<size_type>(-1); --j)
    {
      // estimate qhat of q[j].
      dword_type tmp = (static_cast<dword_type>(un[j + n]) << __word_size__) + un[j + n - 1];
      dword_type qhat = tmp / v1;
      dword_type rhat = tmp % v1;

      while ((qhat > __word_max__ || qhat * v2 > (rhat << __word_size__) + un[j + n - 2]) && rhat <= __word_max__)
      {
        --qhat;
        rhat += v1;
      }

      if (qhat == 0)
        continue;

      // Multiply and subtract.
      dword_type k = 0;
      dword_type t;

      constexpr char shr_sign = 2 * __word_size__ - 1;

      for (size_type i = 0; i < n; ++i)
      {
        dword_type p = qhat * vn[i];
        un[i + j] = t = un[i + j] - k - (p & __word_max__);

        // (t >> __word_size__) is unsigned and must be signed
        /// \see Hacker’s delight / Henry S. Warren, Jr. -- 2nd ed. Section 2-7.
        dword_type x = -(t >> shr_sign);
        k = (p >> __word_size__) - (((x ^ t) >> __word_size__) ^ x);
      }
      un[j + n] = t = un[j + n] - k;

      // If we subtracted too much, add back.
      if (t >> shr_sign)
      {
        --qhat;
        t = 0;
        for (size_type i = 0; i < n; ++i)
          un[i + j] = t = static_cast<dword_type>(un[i + j]) + vn[i] + (t >> __word_size__);
        un[j + n] += t >> __word_size__;
      }
      q[j] = qhat;          // Store quotient digit.
    }

    if (q.back() == 0)
      q.pop_back();

    // If the caller wants the remainder, unnormalize it and pass it back.
    if (__rem != nullptr)
    {
      while (!un.empty() && un.back() == 0)
        un.pop_back();
      *__rem = u_norm >> shift;
    }

    assert(q.back() != 0);
    assert(__rem == nullptr || __rem->number.empty() || __rem->number.back() != 0);
  }

  std::pair<bigint,bigint> bigint::div(const bigint& __denom) const
  {
    if (!__denom)
      throw "zero denom";
    if (!*this)
      return std::pair<bigint,bigint>(0, 0);
    // if (__denom == 1)
      // return pair<bigint,bigint>(*this, 0);

    int cmp = ucompare(__denom);
    if (cmp < 0)
      return std::pair<bigint,bigint>(0, *this);

    bool sign = number_sign ^ __denom.number_sign;
    if (cmp == 0)
      return std::pair<bigint,bigint>((sign ? -1 : 1), 0);

    std::pair<bigint,bigint> qr;
    divide_knuth(qr.first, &qr.second, *this, __denom);
    qr.first.number_sign = sign;
    qr.second.number_sign = (number_sign && (bool)qr.second);

    assert(!qr.first.number.empty() && qr.first.number.back() != 0);
    assert(qr.first.number_sign == 0 || !qr.first.number.empty());
    assert(qr.second.number.empty() || qr.second.number.back() != 0);
    assert(qr.second.number_sign == 0 || !qr.second.number.empty());
    return qr;
  }

  bigint& bigint::operator += (const bigint& __big)
  {
    if (!__big)
      return *this;
    if (!*this)
      return *this = __big;

    if (number_sign == __big.number_sign)
      add(__big.number);
    else
    {
      int cmp = this->ucompare(__big);
      if (cmp == 0)
        return *this = 0;
      if (cmp > 0)
        subtract(__big.number);
      else
      {
        rsubtract(__big.number);
        number_sign ^= 1;
      }
    }

    return *this;
  }

  bigint& bigint::operator -= (const bigint& __big)
  {
    if (!__big)
      return *this;
    if (!*this)
      return *this = -__big;

    if (number_sign != __big.number_sign)
      add(__big.number);
    else
    {
      int cmp = this->ucompare(__big);
      if (cmp == 0)
        return *this = 0;
      if (cmp > 0)
        subtract(__big.number);
      else
      {
        rsubtract(__big.number);
        number_sign ^= 1;
      }
    }

    return *this;
  }

  bigint& bigint::operator *= (const bigint& __big)
  {
    if (!*this)
      return *this;
    if (!__big)
      return *this = 0;

    bool sign = number_sign ^ __big.number_sign;
    if (number.size() <= __math_bigint_multiply_karatsuba_threshold__
                || __big.number.size() <= __math_bigint_multiply_karatsuba_threshold__)
      multiply(__big.number);
    else
      multiply_karatsuba(__big);

    number_sign = sign;
    assert(number.back() != 0);
    return *this;
  }

  bigint& bigint::operator /= (const bigint& __big)
  {
    if (!__big)
      throw "div 0";
    if (!*this)
      return *this;

    int cmp = ucompare(__big);
    if(cmp < 0)
      return *this = 0;

    bool sign = number_sign ^ __big.number_sign;
    if(cmp == 0)
      return *this = (sign ? -1 : 1);

    // por enquanto...
    divide_knuth(*this, nullptr, bigint(*this), __big);
    number_sign = sign;
    return *this;
  }

  bigint& bigint::operator %= (const bigint& __big)
  {
    if (!__big)
      throw "rem 0";
    if (!*this)
      return *this;

    int cmp = ucompare(__big);
    if(cmp < 0)
      return *this;
    if(cmp == 0)
      return *this = 0;

    // por enquanto...
    *this = div(__big).second;
    return *this;
  }

} /// \namespace math

#endif // __FLAW_BIGINT_CPP__
