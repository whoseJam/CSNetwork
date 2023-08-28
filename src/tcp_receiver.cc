#include "tcp_receiver.hh"
#include <iostream>

using namespace std;

TCPReceiver::TCPReceiver() : zero_point_(0), syn_is_set_(false) {
}

void TCPReceiver::receive( TCPSenderMessage message, Reassembler& reassembler, Writer& inbound_stream )
{
  if (!syn_is_set_) {
    if (!message.SYN) return;
    zero_point_ = message.seqno;
    syn_is_set_ = true;
  };
  uint64_t checkpoint = inbound_stream.bytes_pushed() + 1;
  uint64_t absolute_seqno = message.seqno.unwrap(zero_point_, checkpoint);
  uint64_t stream_index = absolute_seqno - (message.SYN^1);
  reassembler.insert(
    stream_index,
    message.payload.release(), 
    message.FIN, 
    inbound_stream);
}

TCPReceiverMessage TCPReceiver::send( const Writer& inbound_stream ) const
{
  uint64_t capacity = inbound_stream.available_capacity();
  uint16_t window_size = UINT16_MAX < capacity ? UINT16_MAX : capacity;
  if (!syn_is_set_) return {{}, window_size};
  uint64_t next_stream_index = inbound_stream.bytes_pushed() + 1 + inbound_stream.is_closed();
  Wrap32 seqno = Wrap32::wrap(next_stream_index, zero_point_);
  return {seqno, window_size};
}
