// ======== CẤU HÌNH BUZZER =========
hw_timer_t* buzzerTimer = NULL;
portMUX_TYPE buzzerMux = portMUX_INITIALIZER_UNLOCKED;

volatile uint8_t buzzerPattern = 0;
volatile int8_t bitIndex = -1;
volatile uint32_t buzzerIntervalUs = 0;
volatile bool buzzerIsBusy = false;

uint8_t BUZZER_PIN = 13;  // Có thể thay đổi bằng Buzzer_Init()
bool buzzerInitialized = false;

// ======== ISR XỬ LÝ TIMER =========
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

// ======== HÀM KHỞI TẠO BUZZER & TIMER =========
void Buzzer_Init(uint8_t pin, uint8_t timerNum = 1, uint16_t prescaler = 80)
{
    if (buzzerInitialized)
        return;

    BUZZER_PIN = pin;
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);

    buzzerTimer = timerBegin(timerNum, prescaler, true);
    timerAttachInterrupt(buzzerTimer, &buzzer_isr, true);

    buzzerInitialized = true;
}

// ======== HÀM PHÁT BUZZER THEO PATTERN =========
void Buzzer_On(uint8_t pattern, uint32_t interval_ms)
{
    if (!buzzerInitialized || buzzerTimer == NULL)
        return;

    portENTER_CRITICAL(&buzzerMux);

    buzzerIsBusy = true;
    buzzerPattern = pattern;
    buzzerIntervalUs = interval_ms * 1000;

    // Tìm bit 1 cao nhất trong pattern
    bitIndex = -1;
    for (int8_t i = 7; i >= 0; i--)
    {
        if ((buzzerPattern >> i) & 0x01)
        {
            bitIndex = i;
            break;
        }
    }

    if (bitIndex < 0)
    {
        digitalWrite(BUZZER_PIN, LOW);
        buzzerIsBusy = false;
        portEXIT_CRITICAL(&buzzerMux);
        return;
    }

    // Phát bit đầu tiên
    bool on = (buzzerPattern >> bitIndex) & 0x01;
    digitalWrite(BUZZER_PIN, on ? HIGH : LOW);
    bitIndex--;

    timerAlarmWrite(buzzerTimer, buzzerIntervalUs, false);
    timerRestart(buzzerTimer);
    timerAlarmEnable(buzzerTimer);

    portEXIT_CRITICAL(&buzzerMux);
}
