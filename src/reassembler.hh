#pragma once

#include "byte_stream.hh"

#include <set>
#include <string>

#define REASSEMBLER_0

class Reassembler
{
public:
  Reassembler();
  /*
   * Insert a new substring to be reassembled into a ByteStream.
   *   `first_index`: the index of the first byte of the substring
   *   `data`: the substring itself
   *   `is_last_substring`: this substring represents the end of the stream
   *   `output`: a mutable reference to the Writer
   *
   * The Reassembler's job is to reassemble the indexed substrings (possibly out-of-order
   * and possibly overlapping) back into the original ByteStream. As soon as the Reassembler
   * learns the next byte in the stream, it should write it to the output.
   *
   * If the Reassembler learns about bytes that fit within the stream's available capacity
   * but can't yet be written (because earlier bytes remain unknown), it should store them
   * internally until the gaps are filled in.
   *
   * The Reassembler should discard any bytes that lie beyond the stream's available capacity
   * (i.e., bytes that couldn't be written even if earlier gaps get filled in).
   *
   * The Reassembler should close the stream after writing the last byte.
   */
  void insert( uint64_t first_index, std::string data, bool is_last_substring, Writer& output );

  // How many bytes are stored in the Reassembler itself?
  uint64_t bytes_pending() const;

#ifdef REASSEMBLER_0
  struct info
  {
    uint64_t l_, r_;
    std::string data_;
    info( uint64_t l, uint64_t r, std::string&& data );
    info( info&& other );
    info( const info& other );
    bool in_range( uint64_t x );
    bool operator<( const info& other ) const;
  };
private:
  std::multiset<info> memory_;
  uint64_t bytes_pending_;
  uint64_t eof_index_;
  bool eof_flag_;
#endif

#ifdef REASSEMBLER_1
  
private:
  std::vector<uint16_t> buffer_;
  uint64_t capacity_;
  uint64_t bytes_pending_;
  uint64_t start_index_;
  uint64_t eof_index_;
  bool eof_flag_;
#endif
};
