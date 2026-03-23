#include <forestguard_chainsaw_detection.h> // rename to match your Edge Impulse export
#include "driver/i2s.h"

#define CONFIDENCE_THRESHOLD 0.85
#define SAMPLE_RATE          16000

#define I2S_MIC_SCK   GPIO_NUM_7
#define I2S_MIC_WS    GPIO_NUM_6
#define I2S_MIC_SD    GPIO_NUM_2

// Must match EI_CLASSIFIER_RAW_SAMPLE_COUNT from your model
static signed short sampleBuffer[EI_CLASSIFIER_RAW_SAMPLE_COUNT];

void setup() {
    Serial.begin(115200);
    Serial.println("ForestGuard: Initializing...");

    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 64
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_MIC_SCK,
        .ws_io_num  = I2S_MIC_WS,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num  = I2S_MIC_SD
    };

    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);
    Serial.println("ForestGuard: Ready.");
}

int get_signal_data(size_t offset, size_t length, float *out_ptr) {
    for (size_t i = 0; i < length; i++) {
        out_ptr[i] = (float)sampleBuffer[offset + i];
    }
    return 0;
}

void sendAlert(float confidence) {
    Serial.print("ALERT: Chainsaw detected! Confidence: ");
    Serial.println(confidence);
    // Add ESP-NOW / WiFi alert + GPS location here
}

void loop() {
    // Capture exactly EI_CLASSIFIER_RAW_SAMPLE_COUNT samples
    size_t bytes_read;
    i2s_read(
        I2S_NUM_0,
        sampleBuffer,                                      // fixed: no &
        EI_CLASSIFIER_RAW_SAMPLE_COUNT * sizeof(signed short),
        &bytes_read,
        portMAX_DELAY
    );

    signal_t signal;
    signal.total_length = EI_CLASSIFIER_RAW_SAMPLE_COUNT; // fixed: use EI constant
    signal.get_data = &get_signal_data;

    ei_impulse_result_t result = { 0 };
    EI_IMPULSE_ERROR err = run_classifier(&signal, &result, false);

    if (err != EI_IMPULSE_OK) {
        Serial.print("Classifier error: ");
        Serial.println(err);
        return;
    }

    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
        if (strcmp(result.classification[ix].label, "chainsaw") == 0) {
            float score = result.classification[ix].value;
            Serial.print("Chainsaw score: ");
            Serial.println(score);
            if (score > CONFIDENCE_THRESHOLD) {
                sendAlert(score);
            }
        }
    }
}
