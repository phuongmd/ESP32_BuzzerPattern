#include "BuzzerPattern.h"

hw_timer_t* buzzerTimer = NULL;
portMUX_TYPE buzzerMux = portMUX_INITIALIZER_UNLOCKED;

static uint8_t BUZZER_PIN = 13;
static uint8_t buzzerTimerNum = 1;

volatile uint8_t buzzerPattern = 0;
volatile int8_t bitIndex = -1;
volatile uint32_t buzzerIntervalUs = 0;
volatile bool buzzerIsBusy = false;

// ========== ISR ==========
void IRAM_ATTR buzzer_isr()
{
	portENTER_CRITICAL_ISR(&buzzerMux);

	if (bitIndex < 0)
	{
		digitalWrite(BUZZER_PIN, LOW);
		timerAlarmDisable(buzzerTimer);
		buzzerIsBusy = false;
	}
	else
	{
		bool on = (buzzerPattern >> bitIndex) & 0x01;
		digitalWrite(BUZZER_PIN, on ? HIGH : LOW);

		bitIndex--;
		timerAlarmWrite(buzzerTimer, buzzerIntervalUs, false);
		timerRestart(buzzerTimer);
		timerAlarmEnable(buzzerTimer);
	}

	portEXIT_CRITICAL_ISR(&buzzerMux);
}

// ========== INIT ==========
void Buzzer_Init(uint8_t pin, uint8_t timerNum)
{
	BUZZER_PIN = pin;
	buzzerTimerNum = timerNum;
	pinMode(BUZZER_PIN, OUTPUT);
	digitalWrite(BUZZER_PIN, LOW);

	if (buzzerTimer == NULL)
	{
		buzzerTimer = timerBegin(buzzerTimerNum, 80, true);
		timerAttachInterrupt(buzzerTimer, &buzzer_isr, true);
	}
}

// ========== BUZZER ON ==========
void Buzzer_On(uint8_t pattern, uint32_t interval_ms)
{
	portENTER_CRITICAL(&buzzerMux);

	buzzerIsBusy = true;
	buzzerPattern = pattern;
	buzzerIntervalUs = interval_ms * 1000;

	// ==== Tìm bit cao nhất có giá trị 1 ====
	bitIndex = -1;
	for (int8_t i = 7; i >= 0; i--)
	{
		if ((buzzerPattern >> i) & 0x01)
		{
			bitIndex = i;
			break;
		}
	}

	// Nếu không có bit nào bật, thoát sớm
	if (bitIndex < 0)
	{
		digitalWrite(BUZZER_PIN, LOW);
		buzzerIsBusy = false;
		portEXIT_CRITICAL(&buzzerMux);
		return;
	}

	// Đảm bảo timer đã khởi tạo
	if (buzzerTimer == NULL)
	{
		buzzerTimer = timerBegin(buzzerTimerNum, 80, true);
		timerAttachInterrupt(buzzerTimer, &buzzer_isr, true);
	}

	// Bật buzzer theo bit đầu tiên
	bool on = (buzzerPattern >> bitIndex) & 0x01;
	digitalWrite(BUZZER_PIN, on ? HIGH : LOW);
	bitIndex--;

	timerAlarmWrite(buzzerTimer, buzzerIntervalUs, false);
	timerRestart(buzzerTimer);
	timerAlarmEnable(buzzerTimer);

	portEXIT_CRITICAL(&buzzerMux);
}
