#include <stdexcept>

#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity )
  : capacity_( capacity ), buffer_( capacity ), error_( false ), closed_( false ), total_pushed_( 0 ), total_poped_( 0 )
{}

void Writer::push( string data )
{
  for ( auto i : data ) {
    if ( total_pushed_ - total_poped_ < capacity_ ) {
      buffer_[total_pushed_%capacity_] = i;
      total_pushed_++;
    } else {
      break;
    }
  }
}

void Writer::close()
{
  closed_ = true;
}

void Writer::set_error()
{
  error_ = true;
}

bool Writer::is_closed() const
{
  return closed_;
}

uint64_t Writer::available_capacity() const
{
  return capacity_ - (total_pushed_ - total_poped_);
}

uint64_t Writer::bytes_pushed() const
{
  return total_pushed_;
}

string_view Reader::peek() const
{
  return string_view(&buffer_[total_poped_%capacity_], 1);
}

bool Reader::is_finished() const
{
  return total_poped_ == total_pushed_ && closed_;
}

bool Reader::has_error() const
{
  return error_;
}

void Reader::pop( uint64_t len )
{
  len = min( len, bytes_buffered() );
  for ( uint64_t i = 0; i < len; i++ ) {
    total_poped_++;
  }
}

uint64_t Reader::bytes_buffered() const
{
  return total_pushed_ - total_poped_;
}

uint64_t Reader::bytes_popped() const
{
  return total_poped_;
}
