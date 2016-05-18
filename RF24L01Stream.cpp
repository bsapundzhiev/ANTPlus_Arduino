#include "RF24L01Stream.h"

RF24L01Stream::RF24L01Stream(uint8_t _cepin, uint8_t _cspin, uint32_t spispeed):
  radio(_cepin, _cspin, spispeed)
{

}

RF24L01Stream::RF24L01Stream(uint8_t _cepin, uint8_t _cspin):
  radio(_cepin, _cspin)
{

}

void RF24L01Stream::begin(uint8_t *pipe, int role)
{
  radio.begin();
  radio.setPALevel(RF24_PA_LOW); //RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH or RF24_PA_MAX
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(57); //Above most Wifi Channels

  if (role == PipeType::Write) {
    radio.openWritingPipe(pipe);
  } else {
    radio.openReadingPipe(1, pipe);
    radio.startListening();
  }

  radio.printDetails();
  address = pipe;
}

int RF24L01Stream::available()
{
    return radio.available(address);
}

int RF24L01Stream::read()
{
    if(available()) {
      radio.read(&data, sizeof(data));
      return data;
    }
    return -1;
}

int RF24L01Stream::peek()
{
    return data;
}

void RF24L01Stream::flush()
{
  data = 0x0;
}

size_t RF24L01Stream::write(uint8_t bytes)
{
  //radio.stopListening();
  return radio.write(&bytes, sizeof(bytes));
}

