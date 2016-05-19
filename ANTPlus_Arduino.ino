
#include <printf.h>
#include "RF24L01Stream.h"
#include "ANTPlus.h"

#define SERIAL_DEBUG_PRINT(x)       (Serial.print(x))
#define SERIAL_DEBUG_PRINTLN(x)     (Serial.println(x))
#define SERIAL_DEBUG_PRINT_F(x)     (Serial.print(F(x)))
#define SERIAL_DEBUG_PRINTLN_F(x)   (Serial.println(F(x)))
#define SERIAL_DEBUG_PRINT2(x,y)    (Serial.print(x,y))
#define SERIAL_DEBUG_PRINTLN2(x,y)  (Serial.println(x,y))

//The ANT+ network keys are not allowed to be published so they are stripped from here.
//They are available in the ANT+ docs at thisisant.com
//or from https://github.com/smysnk/garmin-heart-rate-monitor/blob/master/global.h

#define FREQ      0x39  // Garmin radio frequency
//#define FREQ    0x41; // Suunto radio frequency
#define PERIOD    0x1F86  
#define ANT_SENSOR_HR_KEY   {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}

//Arduino Pro Mini pins to the nrf24AP2 modules pinouts
static const int RTS_PIN      = 2; //!< RTS on the nRF24AP2 module
static const int RTS_PIN_INT  = 0; //!< The interrupt equivalent of the RTS_PIN

static RF24L01Stream RF24Stream = RF24L01Stream(1,1);

static ANTPlus        antplus   = ANTPlus(RTS_PIN, 3/*SUSPEND*/, 4/*SLEEP*/, 5/*RESET*/ );
//ANT Channel config for HRM
static ANT_Channel hrm_channel =
{
  0, //Channel Number
  PUBLIC_NETWORK,
  DEVCE_TIMEOUT,
  DEVCE_TYPE_HRM,
  DEVCE_SENSOR_FREQ,
  DEVCE_HRM_LOWEST_RATE,
  ANT_SENSOR_HR_KEY,
  ANT_CHANNEL_ESTABLISH_PROGRESSING,
  FALSE,
  0, //state_counter
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  RF24Stream.begin(RF24L01Stream::PipeType::Read);
  antplus.begin(RF24Stream);
}

void loop()
{
  byte packet_buffer[ANT_MAX_PACKET_LEN];
  ANT_Packet * packet = (ANT_Packet *) packet_buffer;
  MESSAGE_READ ret_val = MESSAGE_READ_NONE;
  
 /* if(rts_ant_received == 1)
  {
    SERIAL_DEBUG_PRINTLN_F("Received RTS Interrupt. ");
    antplus.rTSHighAssertion();
    //Clear the ISR flag
    rts_ant_received = 0;
  }*/

  //Read messages until we get a none
  while( (ret_val = antplus.readPacket(packet, ANT_MAX_PACKET_LEN, 0 )) != MESSAGE_READ_NONE )
  {
    if((ret_val == MESSAGE_READ_EXPECTED) || (ret_val == MESSAGE_READ_OTHER))
    {
      SERIAL_DEBUG_PRINT_F( "ReadPacket success = " );
      if( (ret_val == MESSAGE_READ_EXPECTED) )
      {
        SERIAL_DEBUG_PRINTLN_F( "Expected packet" );
      }
      else
      if( (ret_val == MESSAGE_READ_OTHER) )
      {
        SERIAL_DEBUG_PRINTLN_F( "Other packet" );
      }
      process_packet(packet);
    }
    else
    {
      SERIAL_DEBUG_PRINT_F( "ReadPacket Error = " );
      SERIAL_DEBUG_PRINTLN( ret_val );
      if(ret_val == MESSAGE_READ_ERROR_MISSING_SYNC)
      {
        //Nothing -- allow a re-read to get back in sync
      }
      else
      if(ret_val == MESSAGE_READ_ERROR_BAD_CHECKSUM)
      {
        //Nothing -- fully formed package just bit errors
      }
      else
      {
        break;
      }
    }
  }


  if(hrm_channel.channel_establish != ANT_CHANNEL_ESTABLISH_COMPLETE)
  {
    antplus.progress_setup_channel( &hrm_channel );
    if(hrm_channel.channel_establish == ANT_CHANNEL_ESTABLISH_COMPLETE)
    {
      SERIAL_DEBUG_PRINT( hrm_channel.channel_number );
      SERIAL_DEBUG_PRINTLN_F( " - Established." );
    }
    else
    if(hrm_channel.channel_establish == ANT_CHANNEL_ESTABLISH_PROGRESSING)
    {
      SERIAL_DEBUG_PRINT( hrm_channel.channel_number );
      SERIAL_DEBUG_PRINTLN_F( " - Progressing." );
    }
    else
    {
      SERIAL_DEBUG_PRINT( hrm_channel.channel_number );
      SERIAL_DEBUG_PRINTLN_F( " - ERROR!" );
    }
  }
}

void process_packet( ANT_Packet * packet )
{
//#if defined(USE_SERIAL_CONSOLE) && defined(ANTPLUS_DEBUG)
  //This function internally uses Serial.println
  //Only use it if the console is available and if the ANTPLUS library is in debug mode
  antplus.printPacket( packet, false );
//#endif //defined(USE_SERIAL_CONSOLE) && defined(ANTPLUS_DEBUG)
   
  switch ( packet->msg_id )
  {
    case MESG_BROADCAST_DATA_ID:
    {
      const ANT_Broadcast * broadcast = (const ANT_Broadcast *) packet->data;
      SERIAL_DEBUG_PRINT_F( "CHAN " );
      SERIAL_DEBUG_PRINT( broadcast->channel_number );
      SERIAL_DEBUG_PRINT_F( " " );
      const ANT_DataPage * dp = (const ANT_DataPage *) broadcast->data;
      
      //Update received data
      if( broadcast->channel_number == hrm_channel.channel_number )
      {
        hrm_channel.data_rx = true;
        //To determine the device type -- and the data pages -- check channel setups
        if(hrm_channel.device_type == DEVCE_TYPE_HRM)
        {
            switch(dp->data_page_number)
            {
              case DATA_PAGE_HEART_RATE_0:
              case DATA_PAGE_HEART_RATE_0ALT:
              case DATA_PAGE_HEART_RATE_1:
              case DATA_PAGE_HEART_RATE_1ALT:
              case DATA_PAGE_HEART_RATE_2:
              case DATA_PAGE_HEART_RATE_2ALT:
              case DATA_PAGE_HEART_RATE_3:
              case DATA_PAGE_HEART_RATE_3ALT:
              case DATA_PAGE_HEART_RATE_4:
              case DATA_PAGE_HEART_RATE_4ALT:
              {
                //As we only care about the computed heart rate
                // we use a same struct for all HRM pages
                const ANT_HRMDataPage * hrm_dp = (const ANT_HRMDataPage *) dp;
                SERIAL_DEBUG_PRINT_F( "HR[any_page] : BPM = ");
                SERIAL_DEBUG_PRINTLN( hrm_dp->computed_heart_rate );
              }
              break;
  
              default:
                  SERIAL_DEBUG_PRINT_F(" HRM DP# ");
                  SERIAL_DEBUG_PRINTLN( dp->data_page_number );
                break;
            }
        }
    }
    break;
    }
    
    default:
      SERIAL_DEBUG_PRINTLN_F("Non-broadcast data received.");
      break;
  }
}
