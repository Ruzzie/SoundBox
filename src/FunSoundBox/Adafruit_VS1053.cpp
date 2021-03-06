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

#include "Adafruit_VS1053.h"

static Adafruit_VS1053_FilePlayer *myself;

#ifndef _BV
#define _BV(x) (1<<(x))
#endif

#if defined(__AVR__)
SIGNAL(TIMER0_COMPA_vect) {
    myself->feedBuffer();
}

#endif

static void feeder(void) {
    myself->feedBuffer();
}

#define VS1053_CONTROL_SPI_SETTING  SPISettings(250000,  MSBFIRST, SPI_MODE0)
#define VS1053_DATA_SPI_SETTING     SPISettings(8000000, MSBFIRST, SPI_MODE0)


static const uint8_t dreqinttable[] = {
#if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined (__AVR_ATmega328__) || defined(__AVR_ATmega8__)
        2, 0,
        3, 1,
#elif defined(__AVR_ATmega1281__) || defined(__AVR_ATmega2561__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__)
2, 0,
3, 1,
21, 2,
20, 3,
19, 4,
18, 5,
#elif  defined(__AVR_ATmega32U4__) && defined(CORE_TEENSY)
5, 0,
6, 1,
7, 2,
8, 3,
#elif  defined(__AVR_AT90USB1286__) && defined(CORE_TEENSY)
0, 0,
1, 1,
2, 2,
3, 3,
36, 4,
37, 5,
18, 6,
19, 7,
#elif  defined(__arm__) && defined(CORE_TEENSY)
0, 0, 1, 1, 2, 2, 3, 3, 4, 4,
5, 5, 6, 6, 7, 7, 8, 8, 9, 9,
10, 10, 11, 11, 12, 12, 13, 13, 14, 14,
15, 15, 16, 16, 17, 17, 18, 18, 19, 19,
20, 20, 21, 21, 22, 22, 23, 23, 24, 24,
25, 25, 26, 26, 27, 27, 28, 28, 29, 29,
30, 30, 31, 31, 32, 32, 33, 33,
#elif  defined(__AVR_ATmega32U4__)
3, 0,
2, 1,
0, 2,
1, 3,
7, 4,
#elif defined(__AVR_ATmega256RFR2__)
4, 0,
5, 1,
#elif  defined(__SAM3X8E__)
  0, 0, 1, 1, 2, 2, 3, 3, 4, 4,
  5, 5, 6, 6, 7, 7, 8, 8, 9, 9,
  10, 10, 11, 11, 12, 12, 13, 13, 14, 14,
  15, 15, 16, 16, 17, 17, 18, 18, 19, 19,
  20, 20, 21, 21, 22, 22, 23, 23, 24, 24,
  25, 25, 26, 26, 27, 27, 28, 28, 29, 29,
  30, 30, 31, 31, 32, 32, 33, 33,

#endif
};

boolean Adafruit_VS1053_FilePlayer::useInterrupt(uint8_t type) {
    myself = this;  // oy vey

    if (type == VS1053_FILEPLAYER_TIMER0_INT) {
#if defined(__AVR__)
        OCR0A = 0xAF;
        TIMSK0 |= _BV(OCIE0A);
        return true;
#elif defined(__arm__) && defined(CORE_TEENSY)
        IntervalTimer *t = new IntervalTimer();
        return (t && t->begin(feeder, 1024)) ? true : false;
#else
        return false;
#endif
    }
    if (type == VS1053_FILEPLAYER_PIN_INT) {
        for (uint8_t i = 0; i < sizeof(dreqinttable); i += 2) {
            //Serial.println(dreqinttable[i]);
            if (_dreq == dreqinttable[i]) {
#ifdef SPI_HAS_TRANSACTION
                SPI.usingInterrupt(dreqinttable[i + 1]);
#endif
                attachInterrupt(dreqinttable[i + 1], feeder, CHANGE);
                return true;
            }
        }
    }
    return false;
}

Adafruit_VS1053_FilePlayer::Adafruit_VS1053_FilePlayer(
        uint8_t rst, uint8_t cs, uint8_t dcs, uint8_t dreq,
        uint8_t cardcs)
        : Adafruit_VS1053(rst, cs, dcs, dreq) {

    playingMusic = false;

    // Set the card to be disabled while we get the VS1053 up
    pinMode(_cardCS, OUTPUT);
    digitalWrite(_cardCS, HIGH);
}

Adafruit_VS1053_FilePlayer::Adafruit_VS1053_FilePlayer(
        uint8_t cs, uint8_t dcs, uint8_t dreq,
        uint8_t cardcs)
        : Adafruit_VS1053(-1, cs, dcs, dreq) {

    playingMusic = false;

    // Set the card to be disabled while we get the VS1053 up
    pinMode(_cardCS, OUTPUT);
    digitalWrite(_cardCS, HIGH);
}

Adafruit_VS1053_FilePlayer::Adafruit_VS1053_FilePlayer(
        uint8_t mosi, uint8_t miso, uint8_t clk,
        uint8_t rst, uint8_t cs, uint8_t dcs, uint8_t dreq,
        uint8_t cardcs)
        : Adafruit_VS1053(mosi, miso, clk, rst, cs, dcs, dreq) {

    playingMusic = false;

    // Set the card to be disabled while we get the VS1053 up
    pinMode(_cardCS, OUTPUT);
    digitalWrite(_cardCS, HIGH);
}

boolean Adafruit_VS1053_FilePlayer::begin(void) {
    uint8_t v = Adafruit_VS1053::begin();

    //dumpRegs();
    //Serial.print("Version = "); Serial.println(v);
    return (v == 4);
}

boolean Adafruit_VS1053_FilePlayer::playFullFile(const char *trackname) {
    if (!startPlayingFile(trackname)) return false;

    while (playingMusic) {
        // twiddle thumbs
        feedBuffer();
    }
    // music file finished!
    return true;
}

void Adafruit_VS1053_FilePlayer::stopPlaying(void) {
    // cancel all playback
    sciWrite(VS1053_REG_MODE, VS1053_MODE_SM_LINE1 | VS1053_MODE_SM_SDINEW | VS1053_MODE_SM_CANCEL);

    // wrap it up!
    playingMusic = false;
    currentTrack.close();
}

boolean Adafruit_VS1053_FilePlayer::stopped(void) {
    return (!playingMusic && !currentTrack);
}


boolean Adafruit_VS1053_FilePlayer::startPlayingFile(const char *trackname) {
    // reset playback
    sciWrite(VS1053_REG_MODE, VS1053_MODE_SM_LINE1 | VS1053_MODE_SM_SDINEW);
    // resync
    sciWrite(VS1053_REG_WRAMADDR, 0x1e29);
    sciWrite(VS1053_REG_WRAM, 0);

    currentTrack = SD.open(trackname);
    if (!currentTrack) {
        return false;
    }

    // As explained in datasheet, set twice 0 in REG_DECODETIME to set time back to 0
    sciWrite(VS1053_REG_DECODETIME, 0x00);
    sciWrite(VS1053_REG_DECODETIME, 0x00);

    playingMusic = true;

    // wait till its ready for data
    while (!readyForData());


    // fill it up!
    while (playingMusic && readyForData())
        feedBuffer();

//  Serial.println("Ready");

    return true;
}

void Adafruit_VS1053_FilePlayer::feedBuffer(void) {
    static uint8_t running = 0;
    uint8_t sregsave;

#ifndef __SAM3X8E__
    // Do not allow 2 copies of this code to run concurrently.
    // If an interrupt causes feedBuffer() to run while another
    // copy of feedBuffer() is already running in the main
    // program, havoc can occur.  "running" detects this state
    // and safely returns.
    sregsave = SREG;
    cli();
    if (running) {
        SREG = sregsave;
        return;  // return safely, before touching hardware!  :-)
    } else {
        running = 1;
        SREG = sregsave;
    }
#endif

    if (!playingMusic) {
        running = 0;
        return; // paused or stopped
    }
    if (!currentTrack) {
        running = 0;
        return;
    }
    if (!readyForData()) {
        running = 0;
        return;
    }

    // Feed the hungry buffer! :)
    while (readyForData()) {
        //UDR0 = '.';

        // Read some audio data from the SD card file
        uint8_t bytesRead = (uint8_t) currentTrack.read(mp3buffer, VS1053_DATABUFFERLEN);

        if (bytesRead == 0) {
            // must be at the end of the file, wrap it up!
            playingMusic = false;
            currentTrack.close();
            running = 0;
            return;
        }
        playData(mp3buffer, bytesRead);
    }
    running = 0;
    return;
}


/***************************************************************/

/* VS1053 'low level' interface */
static volatile PortReg *clkportreg, *misoportreg, *mosiportreg;
static PortMask clkpin, misopin, mosipin;

Adafruit_VS1053::Adafruit_VS1053(uint8_t mosi, uint8_t miso, uint8_t clk,
                                 uint8_t rst, uint8_t cs, uint8_t dcs, uint8_t dreq) {
    _mosi = mosi;
    _miso = miso;
    _clk = clk;
    _reset = rst;
    _cs = cs;
    _dcs = dcs;
    _dreq = dreq;

    useHardwareSPI = false;

    clkportreg = portOutputRegister(digitalPinToPort(_clk));
    clkpin = digitalPinToBitMask(_clk);
    misoportreg = portInputRegister(digitalPinToPort(_miso));
    misopin = digitalPinToBitMask(_miso);
    mosiportreg = portOutputRegister(digitalPinToPort(_mosi));
    mosipin = digitalPinToBitMask(_mosi);
}

Adafruit_VS1053::Adafruit_VS1053(uint8_t rst, uint8_t cs, uint8_t dcs, uint8_t dreq) {
    _mosi = 0;
    _miso = 0;
    _clk = 0;
    useHardwareSPI = true;
    _reset = rst;
    _cs = cs;
    _dcs = dcs;
    _dreq = dreq;
}

boolean Adafruit_VS1053::readyForData(void) {
    return digitalRead(_dreq);
}

void Adafruit_VS1053::playData(uint8_t *buffer, uint8_t bufferSize) {
#ifdef SPI_HAS_TRANSACTION
    if (useHardwareSPI) SPI.beginTransaction(VS1053_DATA_SPI_SETTING);
#endif
    digitalWrite(_dcs, LOW);
    for (uint8_t i = 0; i < bufferSize; i++) {
        spiWrite(buffer[i]);
    }
    digitalWrite(_dcs, HIGH);
#ifdef SPI_HAS_TRANSACTION
    if (useHardwareSPI) SPI.endTransaction();
#endif
}

void Adafruit_VS1053::setVolume(uint8_t left, uint8_t right) {
    uint16_t v;
    v = left;
    v <<= 8;
    v |= right;

    noInterrupts(); //cli();
    sciWrite(VS1053_REG_VOLUME, v);
    interrupts();  //sei();
}

void Adafruit_VS1053::softReset(void) {
    sciWrite(VS1053_REG_MODE, VS1053_MODE_SM_SDINEW | VS1053_MODE_SM_RESET);
    delay(100);
}

void Adafruit_VS1053::reset() {
    // TODO: http://www.vlsi.fi/player_vs1011_1002_1003/modularplayer/vs10xx_8c.html#a3
    // hardware reset
    if (_reset >= 0) {
        digitalWrite(_reset, LOW);
        delay(100);
        digitalWrite(_reset, HIGH);
    }
    digitalWrite(_cs, HIGH);
    digitalWrite(_dcs, HIGH);
    delay(100);
    softReset();
    delay(100);

    sciWrite(VS1053_REG_CLOCKF, 0x6000);

    setVolume(40, 40);
}

uint8_t Adafruit_VS1053::begin(void) {
    if (_reset >= 0) {
        pinMode(_reset, OUTPUT);
        digitalWrite(_reset, LOW);
    }

    pinMode(_cs, OUTPUT);
    digitalWrite(_cs, HIGH);
    pinMode(_dcs, OUTPUT);
    digitalWrite(_dcs, HIGH);
    pinMode(_dreq, INPUT);

    if (!useHardwareSPI) {
        pinMode(_mosi, OUTPUT);
        pinMode(_clk, OUTPUT);
        pinMode(_miso, INPUT);
    } else {
        SPI.begin();
        SPI.setDataMode(SPI_MODE0);
        SPI.setBitOrder(MSBFIRST);
        SPI.setClockDivider(SPI_CLOCK_DIV128);
    }

    reset();

    return (sciRead(VS1053_REG_STATUS) >> 4) & 0x0F;
}

uint16_t Adafruit_VS1053::sciRead(uint8_t addr) {
    uint16_t data;

#ifdef SPI_HAS_TRANSACTION
    if (useHardwareSPI) SPI.beginTransaction(VS1053_CONTROL_SPI_SETTING);
#endif
    digitalWrite(_cs, LOW);
    spiWrite(VS1053_SCI_READ);
    spiWrite(addr);
    delayMicroseconds(10);
    data = spiRead();
    data <<= 8;
    data |= spiRead();
    digitalWrite(_cs, HIGH);
#ifdef SPI_HAS_TRANSACTION
    if (useHardwareSPI) SPI.endTransaction();
#endif

    return data;
}

void Adafruit_VS1053::sciWrite(uint8_t addr, uint16_t data) {
#ifdef SPI_HAS_TRANSACTION
    if (useHardwareSPI) SPI.beginTransaction(VS1053_CONTROL_SPI_SETTING);
#endif
    digitalWrite(_cs, LOW);
    spiWrite(VS1053_SCI_WRITE);
    spiWrite(addr);
    spiWrite(data >> 8);
    spiWrite(data & 0xFF);
    digitalWrite(_cs, HIGH);
#ifdef SPI_HAS_TRANSACTION
    if (useHardwareSPI) SPI.endTransaction();
#endif
}

uint8_t Adafruit_VS1053::spiRead(void) {
    uint8_t i, x;
    x = 0;

    // MSB first, clock low when inactive (CPOL 0), data valid on leading edge (CPHA 0)
    // Make sure clock starts low

    if (useHardwareSPI) {
        x = SPI.transfer(0x00);
    } else {
        for (i = 7; i >= 0; i--) {
            if ((*misoportreg) & misopin)
                x |= (1 << i);
            *clkportreg |= clkpin;
            *clkportreg &= ~clkpin;
            //    asm("nop; nop");
        }
        // Make sure clock ends low
        *clkportreg &= ~clkpin;
    }
    return x;
}

void Adafruit_VS1053::spiWrite(uint8_t c) {
    // MSB first, clock low when inactive (CPOL 0), data valid on leading edge (CPHA 0)
    // Make sure clock starts low

    if (useHardwareSPI) {
        SPI.transfer(c);

    } else {
        for (int8_t i = 7; i >= 0; i--) {
            *clkportreg &= ~clkpin;
            if (c & (1 << i)) {
                *mosiportreg |= mosipin;
            } else {
                *mosiportreg &= ~mosipin;
            }
            *clkportreg |= clkpin;
        }
        *clkportreg &= ~clkpin;   // Make sure clock ends low
    }
}

void Adafruit_VS1053::sineTest(uint8_t n, uint16_t ms) {
    reset();

    uint16_t mode = sciRead(VS1053_REG_MODE);
    mode |= 0x0020;
    sciWrite(VS1053_REG_MODE, mode);

    while (!digitalRead(_dreq));
    //  delay(10);

#ifdef SPI_HAS_TRANSACTION
    if (useHardwareSPI) SPI.beginTransaction(VS1053_DATA_SPI_SETTING);
#endif
    digitalWrite(_dcs, LOW);
    spiWrite(0x53);
    spiWrite(0xEF);
    spiWrite(0x6E);
    spiWrite(n);
    spiWrite(0x00);
    spiWrite(0x00);
    spiWrite(0x00);
    spiWrite(0x00);
    digitalWrite(_dcs, HIGH);
#ifdef SPI_HAS_TRANSACTION
    if (useHardwareSPI) SPI.endTransaction();
#endif

    delay(ms);

#ifdef SPI_HAS_TRANSACTION
    if (useHardwareSPI) SPI.beginTransaction(VS1053_DATA_SPI_SETTING);
#endif
    digitalWrite(_dcs, LOW);
    spiWrite(0x45);
    spiWrite(0x78);
    spiWrite(0x69);
    spiWrite(0x74);
    spiWrite(0x00);
    spiWrite(0x00);
    spiWrite(0x00);
    spiWrite(0x00);
    digitalWrite(_dcs, HIGH);
#ifdef SPI_HAS_TRANSACTION
    if (useHardwareSPI) SPI.endTransaction();
#endif
}