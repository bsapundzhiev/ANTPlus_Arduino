
#include <nRF24L01.h>
//#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

class RF24L01Stream: public Stream   {

  private:
    RF24 radio;

  public:
    RF24L01Stream(uint8_t _cepin, uint8_t _cspin, uint32_t spispeed );
    RF24L01Stream(uint8_t _cepin, uint8_t _cspin);

    int available();

    int read();

    int peek();

    void flush();

     size_t write(uint8_t bytes);
};
