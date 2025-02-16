#include "Microphone.h"
#include "driver/i2s.h"


WiFiUDP udp;
static Microphone* micInstance = nullptr;

Microphone::Microphone(const int bclk, const int ws, const int data, const int sampleRate,
                       const int bufLen, const int bufferSize, const int udpPort, const char* udpIp)
    : _bclk(bclk), _ws(ws), _data(data), _sampleRate(sampleRate),
      _bufLen(bufLen), _bufferSize(bufferSize), _udpPort(udpPort), _udpIp(udpIp)
{
    _buffer = new int16_t[_bufferSize];
    micInstance = this; // Сохраняем указатель на экземпляр
}

void Microphone::dataCallback(unsigned char *mp3_data, unsigned int len) const {
    udp.beginPacket(_udpIp, _udpPort);
    udp.write(mp3_data, len);
    udp.endPacket();
}

void Microphone::dataCallbackStatic(unsigned char *mp3_data, unsigned int len) {
    if(micInstance) {
        micInstance->dataCallback(mp3_data, len);
    }
}

MP3EncoderLAME mp3(Microphone::dataCallbackStatic);
AudioInfo info;

void Microphone::begin() const {
    udp.begin(_udpPort);
    Serial.print("MIC: ");
    const i2s_config_t i2s_config = {
        .mode = static_cast<i2s_mode_t>(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = static_cast<uint32_t>(_sampleRate),
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = 0,
        .dma_buf_count = 4,
        .dma_buf_len = _bufLen
    };

    const i2s_pin_config_t pin_config = {
        .bck_io_num = _bclk,
        .ws_io_num = _ws,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = _data
    };

    i2s_driver_install(I2S_NUM_1, &i2s_config, 0, nullptr);
    i2s_set_pin(I2S_NUM_1, &pin_config);
    i2s_zero_dma_buffer(I2S_NUM_1);

    info.channels = 1;
    info.sample_rate = _sampleRate;
    info.quality = 9;
    mp3.begin(info);
    Serial.println("Success");
}

void Microphone::loop() const {
    int16_t pcmBuffer[_bufferSize];
    size_t bytesRead = 0;

    const esp_err_t ret = i2s_read(I2S_NUM_1, pcmBuffer, _bufferSize * sizeof(int16_t), &bytesRead, portMAX_DELAY);
    if (ret != ESP_OK) {
        Serial.println("I2S read error");
        return;
    }
    mp3.write(pcmBuffer, bytesRead);
}
