#include "reassembler.hh"

using namespace std;

using Info = Reassembler::info;

Info::info( uint64_t l, uint64_t r, string&& data ) : l_( l ), r_( r ), data_( move( data ) ) {}

Info::info( Info&& other ) : l_( other.l_ ), r_( other.r_ ), data_( move( other.data_ ) ) {}

Info::info( const Info& other ) : l_( other.l_ ), r_( other.r_ ), data_( other.data_ ) {}

bool Info::operator<( const Info& other ) const
{
  if ( l_ != other.l_ )
    return l_ < other.l_;
  return r_ < other.r_;
}

bool Info::in_range( uint64_t x )
{
  return l_ <= x && x < r_;
}

Reassembler::Reassembler() : memory_(), bytes_pending_( 0 ), eof_index_( 0 ), eof_flag_( false ) {}

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring, Writer& output )
{
  uint64_t capacity = output.available_capacity();
  uint64_t byte_pushed = output.bytes_pushed();
  uint64_t l = max( first_index, byte_pushed );
  uint64_t r = min( first_index + data.length(), byte_pushed + capacity );
  if ( is_last_substring ) {
    eof_flag_ = true;
    eof_index_ = first_index + data.length();
  }

  if ( l < r ) {
    Info ii = { l, r, data.substr( l - first_index, r - l ) };
    bytes_pending_ += ii.data_.length();
    auto iter = memory_.insert( move( ii ) ).first;
    if ( iter != memory_.begin() ) {
      auto prev = iter;
      prev--;
      if ( iter->l_ <= prev->r_ )
        iter = prev;
    }
    Info i { move( *iter ) };
    bytes_pending_ -= iter->data_.length();
    iter = memory_.erase( iter );
    while ( iter != memory_.end() && iter->l_ <= i.r_ ) {
      int start_index = i.r_ - iter->l_;
      int length = iter->data_.length() - start_index;
      if ( length > 0 )
        i.data_.insert( i.data_.length(), iter->data_.substr( start_index, length ) );
      i.r_ = max( i.r_, iter->r_ );
      bytes_pending_ -= iter->data_.length();
      iter = memory_.erase( iter );
    }
    bytes_pending_ += i.data_.length();
    memory_.insert( move( i ) );
  }

  auto iter = memory_.begin();
  while ( iter != memory_.end() && iter->l_ == output.bytes_pushed() ) {
    output.push( iter->data_ );
    bytes_pending_ -= iter->data_.length();
    iter = memory_.erase( iter );
  }
  if ( output.bytes_pushed() == eof_index_ && eof_flag_ )
    output.close();
}

uint64_t Reassembler::bytes_pending() const
{
  return bytes_pending_;
}
