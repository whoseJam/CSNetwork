#include "wrapping_integers.hh"

using namespace std;

// absolute seqno -> seqno
Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  return zero_point + n;
}

// seqno -> absolute seqno
uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  uint64_t val = 
    ((uint64_t) raw_value_ + (1UL<<32) - 
     (uint64_t) zero_point.raw_value_) & ((1UL<<32) - 1);
  if (checkpoint < val) return val;
  uint64_t res = checkpoint - val;
  uint64_t k = res >> 32;
  uint64_t l = val + (k<<32);
  uint64_t r = l + (1UL<<32);
  if (checkpoint - l < r - checkpoint) return l;
  return r;
}
