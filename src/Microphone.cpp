#include "Microphone.h"

Microphone::Microphone(const int bclk, const int ws, const int data, const int sampleRate, const int bufferSize)
    : _bclk(bclk), _ws(ws), _data(data), _sampleRate(sampleRate), _bufferSize(bufferSize) {
    _buffer = new int16_t[_bufferSize];
}

void Microphone::begin() const {
    Serial.print("MIC: ");
    const i2s_config_t i2s_config = {
            .mode = static_cast<i2s_mode_t>(I2S_MODE_MASTER | I2S_MODE_RX),
            .sample_rate = static_cast<uint32_t>(_sampleRate),
            .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
            .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
            .communication_format = I2S_COMM_FORMAT_STAND_I2S,
            .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
            .dma_buf_count = 4,
            .dma_buf_len = _bufferSize
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
    Serial.println("Success");
}

void Microphone::readAndSend(WiFiUDP& udp, const char* udpAddress, const int udpPort) const {
    size_t bytesRead = 0;
    i2s_read(I2S_NUM_1, _buffer, _bufferSize * sizeof(int16_t), &bytesRead, portMAX_DELAY);

    udp.beginPacket(udpAddress, udpPort);
    udp.write(reinterpret_cast<uint8_t *>(_buffer), bytesRead);
    if (udp.endPacket() == 0) {
        Serial.println();
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
}
