/*************************************************** 
  This is a library for the Adafruit VS1053 Codec Breakout

  Designed specifically to work with the Adafruit VS1053 Codec Breakout 
  ----> https://www.adafruit.com/products/1381

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/
#ifndef ADAFRUIT_VS1053_H
#define ADAFRUIT_VS1053_H

#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
 #include <pins_arduino.h>
#endif

#include "pins_arduino.h"
#include "wiring_private.h"
#include <SPI.h> 
#include <SD.h>

#ifdef __SAM3X8E__
typedef volatile RwReg PortReg;
typedef uint32_t PortMask;
#else
typedef volatile uint8_t PortReg;
typedef uint8_t PortMask;
#endif


#define VS1053_FILEPLAYER_TIMER0_INT 255 // allows useInterrupt to accept pins 0 to 254
#define VS1053_FILEPLAYER_PIN_INT 5

#define VS1053_SCI_READ 0x03
#define VS1053_SCI_WRITE 0x02

#define VS1053_REG_MODE  0x00
#define VS1053_REG_STATUS 0x01
#define VS1053_REG_CLOCKF 0x03
#define VS1053_REG_DECODETIME 0x04
#define VS1053_REG_WRAM 0x06
#define VS1053_REG_WRAMADDR 0x07
#define VS1053_REG_VOLUME 0x0B

#define VS1053_MODE_SM_RESET 0x0004
#define VS1053_MODE_SM_CANCEL 0x0008
#define VS1053_MODE_SM_SDINEW 0x0800
#define VS1053_MODE_SM_LINE1 0x4000


#define VS1053_DATABUFFERLEN 32


class Adafruit_VS1053 {
 public:
  Adafruit_VS1053(int8_t mosi, int8_t miso, int8_t clk, 
		  int8_t rst, int8_t cs, int8_t dcs, int8_t dreq);
  Adafruit_VS1053(int8_t rst, int8_t cs, int8_t dcs, int8_t dreq);
  uint8_t begin(void);
  void reset(void);
  void softReset(void);
  uint16_t sciRead(uint8_t addr);
  void sciWrite(uint8_t addr, uint16_t data);
  void sineTest(uint8_t n, uint16_t ms);
  void spiwrite(uint8_t d);
  uint8_t spiread(void);

	void setVolume(uint8_t left, uint8_t right);

	void playData(uint8_t *buffer, uint8_t buffsiz);
  boolean readyForData(void);

	uint8_t mp3buffer[VS1053_DATABUFFERLEN];

 protected:
  uint8_t  _dreq;
 private:
  int8_t _mosi, _miso, _clk, _reset, _cs, _dcs;
  boolean useHardwareSPI;
};


class Adafruit_VS1053_FilePlayer : public Adafruit_VS1053 {
 public:
  Adafruit_VS1053_FilePlayer (int8_t mosi, int8_t miso, int8_t clk, 
			      int8_t rst, int8_t cs, int8_t dcs, int8_t dreq,
			      int8_t cardCS);
  Adafruit_VS1053_FilePlayer (int8_t rst, int8_t cs, int8_t dcs, int8_t dreq,
			      int8_t cardCS);
  Adafruit_VS1053_FilePlayer (int8_t cs, int8_t dcs, int8_t dreq,
			      int8_t cardCS);

  boolean begin(void);
  boolean useInterrupt(uint8_t type);
  File currentTrack;
  volatile boolean playingMusic;
  void feedBuffer(void);
  boolean startPlayingFile(const char *trackname);
  boolean playFullFile(const char *trackname);
  void stopPlaying(void);
  boolean paused(void);
  boolean stopped(void);
  void pausePlaying(boolean pause);

 private:
  uint8_t _cardCS;
};

#endif // ADAFRUIT_VS1053_H
