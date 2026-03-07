#include <cstdint>
#include <iostream>
#include <functional>

struct UInt128 {
  uint64_t hi;
  uint64_t lo;

  constexpr UInt128() : hi(0), lo(0) {}
  constexpr UInt128(uint64_t low) : hi(0), lo(low) {}
  constexpr UInt128(uint64_t high, uint64_t low) : hi(high), lo(low) {}

  friend constexpr bool operator==(const UInt128 &a, const UInt128 &b) {
    return a.hi == b.hi && a.lo == b.lo;
  }

  friend constexpr bool operator!=(const UInt128 &a, const UInt128 &b) {
    return !(a == b);
  }

  friend constexpr bool operator<(const UInt128 &a, const UInt128 &b) {
    return (a.hi < b.hi) || (a.hi == b.hi && a.lo < b.lo);
  }

  friend constexpr bool operator>(const UInt128 &a, const UInt128 &b) {
    return b < a;
  }

  friend constexpr bool operator<=(const UInt128 &a, const UInt128 &b) {
    return !(b < a);
  }

  friend constexpr bool operator>=(const UInt128 &a, const UInt128 &b) {
    return !(a < b);
  }

  friend constexpr UInt128 operator+(const UInt128 &a, const UInt128 &b) {
    UInt128 r;
    r.lo = a.lo + b.lo;
    r.hi = a.hi + b.hi + (r.lo < a.lo);
    return r;
  }

  friend constexpr UInt128 operator-(const UInt128 &a, const UInt128 &b) {
    UInt128 r;
    r.lo = a.lo - b.lo;
    r.hi = a.hi - b.hi - (a.lo < b.lo);
    return r;
  }

  friend constexpr UInt128 operator&(const UInt128 &a, const UInt128 &b) {
    return UInt128(a.hi & b.hi, a.lo & b.lo);
  }

  friend constexpr UInt128 operator|(const UInt128 &a, const UInt128 &b) {
    return UInt128(a.hi | b.hi, a.lo | b.lo);
  }

  friend constexpr UInt128 operator^(const UInt128 &a, const UInt128 &b) {
    return UInt128(a.hi ^ b.hi, a.lo ^ b.lo);
  }

  friend constexpr UInt128 operator~(const UInt128 &a) {
    return UInt128(~a.hi, ~a.lo);
  }

  friend constexpr UInt128 operator<<(const UInt128 &a, unsigned s) {
    if (s >= 128)
      return UInt128(0, 0);

    if (s >= 64)
      return UInt128(a.lo << (s - 64), 0);

    return UInt128(
        (a.hi << s) | (a.lo >> (64 - s)),
        a.lo << s);
  }

  friend constexpr UInt128 operator>>(const UInt128 &a, unsigned s) {
    if (s >= 128)
      return UInt128(0, 0);

    if (s >= 64)
      return UInt128(0, a.hi >> (s - 64));

    return UInt128(
        a.hi >> s,
        (a.lo >> s) | (a.hi << (64 - s)));
  }

  UInt128 &operator++() {
    *this = *this + 1;
    return *this;
  }

  UInt128 operator++(int) {
    UInt128 tmp = *this;
    ++(*this);
    return tmp;
  }

  UInt128 &operator--() {
    *this = *this - 1;
    return *this;
  }

  UInt128 operator--(int) {
    UInt128 tmp = *this;
    --(*this);
    return tmp;
  }
};

namespace std {
  template <>
  struct hash<UInt128> {
    size_t operator()(const UInt128 &v) const noexcept {
      size_t h1 = std::hash<uint64_t>{}(v.hi);
      size_t h2 = std::hash<uint64_t>{}(v.lo);
      return h1 ^ (h2 << 1);
    }
  };
}

// std::ostream &operator<<(std::ostream &os, const UInt128 &v) {
//   os << "0x" << std::hex << v.hi << v.lo << std::dec;
//   return os;
// }