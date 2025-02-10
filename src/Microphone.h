#ifndef MICROPHONE_H
#define MICROPHONE_H

#include <driver/i2s.h>
#include <WiFiUdp.h>

class Microphone {
public:
    Microphone(int bclk, int ws, int data, int sampleRate, int bufferSize);
    void begin() const;
    void readAndSend(WiFiUDP& udp, const char* udpAddress, int udpPort) const;

private:
    int _bclk;
    int _ws;
    int _data;
    int _sampleRate;
    int _bufferSize;
    int16_t* _buffer;
};

#endif // MICROPHONE_H
