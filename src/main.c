#include <avr/io.h>
#include <util/delay.h>
#include "../lib/spi/spi.h"
#include "../lib/millis/millis.h"

#define MAX7219_DECODE_MODE_REG 0x09
#define MAX7219_INTENSITY_REG 0x0a
#define MAX7219_SCAN_LIMIT_REG 0x0b
#define MAX7219_SHUTDOWN_REG 0x0c
#define MAX7219_DISPLAY_TEST_REG 0x0f
#define LATCH_PIN PB2

#define BTN1 PD0
#define BTN2 PD1

void send_to_max7219(uint8_t addr, uint8_t data);
void setup_max7219_led_matrix(void);

uint8_t smileEmoji[8] = {
0b00000000,
0b00100100,
0b00100100,
0b00100100,
0b00000000,
0b01000010,
0b00111100,
0b00000000
};
uint8_t sadEmoji[8] = {
0b00000000,
0b00100100,
0b00100100,
0b00100100,
0b00000000,
0b00111100,
0b01000010,
0b00000000
};
int main(void) {
    DDRD &= ~(1 << BTN1);
    DDRD &= ~(1 << BTN2);
    DDRB |= 1 << LATCH_PIN;

    PORTD |= 1 << BTN1;
    PORTD |= 1 << BTN2;

    spi_init();
    setup_max7219_led_matrix();
    millis_init();

    uint8_t btn1_state = 1;
    uint8_t btn2_state = 1;

    uint8_t btn1_last_raw = 1;
    uint8_t btn2_last_raw = 1;

    uint32_t last_debounce_time1 = 0;
    uint32_t last_debounce_time2 = 0;


    while(1){
        uint8_t read1 = (PIND & (1 << BTN1)) != 0;
        uint8_t read2 = (PIND & (1 << BTN2)) != 0;

        uint32_t now = millis();

        if (read1 != btn1_last_raw) {
            last_debounce_time1 = now;
        }

        if ((now - last_debounce_time1) >= 30) {
            if (read1 != btn1_state) {
                btn1_state = read1; // Оновлюємо стабільний стан

                if (btn1_state == 0) {
                    for(int i = 0; i < 8; i++) {
                        send_to_max7219(i + 1, smileEmoji[i]);
                    }
                }
            }
        }
        btn1_last_raw = read1;

        if (read2 != btn2_last_raw) {
            last_debounce_time2 = now;
        }

        if ((now - last_debounce_time2) >= 30) {
            if (read2 != btn2_state) {
                btn2_state = read2; 

                if (btn2_state == 0) {
                    for(int i = 0; i < 8; i++) {
                        send_to_max7219(i + 1, sadEmoji[i]);
                    }
                }
            }
        }
        btn2_last_raw = read2;
    }
        
}
void send_to_max7219(uint8_t addr, uint8_t data) {
spi_transfer(addr);
spi_transfer(data);
PORTB |= 1 << LATCH_PIN;
_delay_us(1);
PORTB &= ~(1 << LATCH_PIN);
_delay_us(1);
}
void setup_max7219_led_matrix(void) {
send_to_max7219(MAX7219_DECODE_MODE_REG, 0);
send_to_max7219(MAX7219_SCAN_LIMIT_REG, 0xf);
send_to_max7219(MAX7219_INTENSITY_REG, 0x07);
send_to_max7219(MAX7219_DISPLAY_TEST_REG, 0);
send_to_max7219(MAX7219_SHUTDOWN_REG, 1);
}