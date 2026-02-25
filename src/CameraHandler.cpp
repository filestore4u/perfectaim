/**
 * ESP32 Sniper Addon System
 * Created by Kaustav Ray
 * Date: 25 Feb, 2026
 * Copyright (c) 2026 Kaustav Ray. All Rights Reserved.
 */

#include "CameraHandler.h"
#include "Config.h"

// Initialize the camera
bool CameraHandler::begin() {
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG; // JPEG for streaming

    if (psramFound()) {
        config.frame_size = FRAMESIZE_UXGA; // High resolution
        config.jpeg_quality = 10;
        config.fb_count = 2;
    } else {
        config.frame_size = FRAMESIZE_SVGA;
        config.jpeg_quality = 12;
        config.fb_count = 1;
    }

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed with error 0x%x", err);
        return false;
    }

    // Use the aliased type from header to avoid conflict with Adafruit_Sensor
    esp_sensor_t * s = esp_camera_sensor_get();
    // Drop down frame size for higher initial frame rate
    if (s->id.PID == OV3660_PID) {
        s->set_vflip(s, 1); // Flip vertically if needed (common for OV3660)
        s->set_brightness(s, 1); // Up the brightness
        s->set_saturation(s, -2); // Lower saturation
    }
    // Setup typical sensor settings
    s->set_framesize(s, FRAMESIZE_VGA);

    return true;
}

camera_fb_t* CameraHandler::capture() {
    return esp_camera_fb_get();
}

void CameraHandler::release(camera_fb_t* fb) {
    if (fb) {
        esp_camera_fb_return(fb);
    }
}

bool CameraHandler::detectMotion(camera_fb_t* fb, int threshold) {
    if (!fb) return false;

    // Simple motion detection based on frame size variance (for JPEG)
    // A significant change in JPEG size often indicates movement or lighting change.
    // Ideally, we'd decode and compare pixels, but that is heavy on ESP32.

    if (prevFrameLen == 0) {
        prevFrameLen = fb->len;
        return false;
    }

    long diff = abs((long)fb->len - (long)prevFrameLen);
    bool motion = diff > (long)(prevFrameLen * threshold / 100); // Threshold as percentage

    prevFrameLen = fb->len;
    return motion;
}

String CameraHandler::detectGender(camera_fb_t* fb) {
    // Placeholder for facial recognition / gender detection.
    // In a real implementation, you would use ESP-WHO or ESP-DL.
    // Example: dl_matrix3du_t *image_matrix = dl_matrix3du_alloc(1, fb->width, fb->height, 3);
    // fmt2rgb888(fb->buf, fb->len, fb->format, image_matrix->item);
    // float *prediction = model->predict(image_matrix);

    // For now, return "Analyzing..." or a random guess if motion is detected to simulate processing.
    return "Analyzing...";
}
