#ifndef BUZZER_PATTERN_H
#define BUZZER_PATTERN_H

#include <Arduino.h>

// ===================== KHAI BÁO BIẾN =====================
extern hw_timer_t* buzzerTimer;
extern portMUX_TYPE buzzerMux;

extern volatile uint8_t buzzerPattern;
extern volatile int8_t bitIndex;
extern volatile uint32_t buzzerIntervalUs;
extern volatile bool buzzerIsBusy;

extern uint8_t BUZZER_PIN;
extern bool buzzerInitialized;

// ===================== HÀM GỌI TỪ NGOÀI =====================

/**
 * @brief Khởi tạo buzzer và timer hardware.
 * 
 * @param pin        Chân buzzer.
 * @param timerNum   Số timer (0–3), mặc định là 1.
 * @param prescaler  Bộ chia xung timer, mặc định 80 (tương ứng 1us).
 */
void Buzzer_Init(uint8_t pin, uint8_t timerNum = 1, uint16_t prescaler = 80);

/**
 * @brief Phát âm thanh buzzer theo pattern nhị phân.
 * 
 * @param pattern     Mẫu bit (bit cao trước), 1 là ON, 0 là OFF.
 * @param interval_ms Khoảng thời gian mỗi bit (ms).
 */
void Buzzer_On(uint8_t pattern, uint32_t interval_ms);

/**
 * @brief Ngắt Timer xử lý phát pattern buzzer. 
 *        Không nên gọi trực tiếp từ ngoài.
 */
void IRAM_ATTR buzzer_isr();

#endif // BUZZER_PATTERN_H

