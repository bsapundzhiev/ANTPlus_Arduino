#include "RF24L01Stream.h"

RF24L01Stream::RF24L01Stream(uint8_t _cepin, uint8_t _cspin, uint32_t spispeed ):
  radio(_cepin, _cspin, spispeed )
{

}

RF24L01Stream::RF24L01Stream(uint8_t _cepin, uint8_t _cspin):
  radio(_cepin, _cspin)
{

}

int RF24L01Stream::available()
{
    return 0;
}

int RF24L01Stream::read()
{
    return 0;
}
int RF24L01Stream::peek()
{
    return 0;
}

void RF24L01Stream::flush()
{
}

size_t RF24L01Stream::write(uint8_t bytes) {
  return 0;
}

