#ifndef MICROPHONE_H
#define MICROPHONE_H

#include "MP3EncoderLAME.h"
#include <WiFiUdp.h>
using namespace liblame;

class Microphone {
public:
    Microphone(int bclk, int ws, int data, int sampleRate, int bufLen, int bufferSize, int udpPort, const char *udpIp);
    static void dataCallbackStatic(unsigned char *mp3_data, unsigned int len);
    void dataCallback(unsigned char *mp3_data, unsigned int len) const;
    void begin() const;
    void loop() const;


private:
    int _bclk;
    int _ws;
    int _data;
    int _sampleRate;
    int _bufLen;
    int _bufferSize;
    int _udpPort;
    const char* _udpIp;
    int16_t* _buffer;
};

#endif
