#include "RF24L01Stream.h"

RF24L01Stream::RF24L01Stream(uint8_t _cepin, uint8_t _cspin, uint32_t spispeed):
  radio(_cepin, _cspin, spispeed)
{

}

RF24L01Stream::RF24L01Stream(uint8_t _cepin, uint8_t _cspin):
  radio(_cepin, _cspin)
{

}

void RF24L01Stream::begin(uint8_t role)
{
  radio.begin();
  //uint8_t addr[] = {0xC6, 0xA1};
  uint8_t addr[] = {0, 120, 0, 0};
  //uint8_t addr[] = {0, 0, 120, 1};
  radio.setAddressWidth(sizeof(addr) / sizeof(addr[0]));
  //RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH or RF24_PA_MAX
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(57);
  //radio.setPayloadSize(?)
  radio.setRetries(15, 1);
  //RF24_CRC_8 or RF24_CRC_16
  radio.setCRCLength(RF24_CRC_16);

  if (role == PipeType::Write) {
    radio.openWritingPipe(addr);
  } else {
    radio.openReadingPipe(0, addr);
    radio.startListening();
  }

  radio.printDetails();
}

int RF24L01Stream::available()
{
  return radio.available();
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
  radio.flush_tx();
  data = 0x0;
}

size_t RF24L01Stream::write(uint8_t byte0)
{
  radio.stopListening();
  return radio.write(&byte0, sizeof(byte0));
}

