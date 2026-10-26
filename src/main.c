#include <avr/io.h>
#include <util/delay.h>
#include "../lib/spi/spi.h"
#include "../lib/millis/millis.h"

#define LATCH_PIN_DISPLAY PB2
#define LATCH_PIN_BUTTONS PB1

#define SPI_MOSI PB3
#define SPI_MISO PB4
#define SPI_SCK  PB5

const uint8_t digit_map[10] = {
 0b00111111, // 0
 0b00000110, // 1
 0b01011011, // 2
 0b01001111, // 3
 0b01100110, // 4
 0b01101101, // 5
 0b01111101, // 6
 0b00000111, // 7
 0b01111111, // 8
 0b01101111 // 9
};

void display_digit(uint8_t segments, uint8_t mask);
uint8_t read_shift_register();

int main() {
    DDRB |= (1 << LATCH_PIN_DISPLAY) | (1 << LATCH_PIN_BUTTONS);
    spi_init();
    millis_init();

    int count = 99;
    uint8_t prev_input = 0xFF;
    uint32_t last_button_check = 0;

    while (1) {
        uint8_t input = read_shift_register();
        uint32_t current_time = millis();
        if(current_time - last_button_check >= 50){
            if(!(input & (1 << 0)) && prev_input & (1 << 0)){
                count -= 1;
            } 
            else if(!(input & (1 << 1)) && prev_input & (1 << 1)){
                count -= 10;
            }
            else if(!(input & (1 << 2)) && prev_input & (1 << 2)){
                count = 99;
            }
            if(count < 0){
                count = 0;
            }
            last_button_check = current_time;
            prev_input = input;
        }
        display_digit(count / 10, 0b11111110);
        _delay_ms(50);
        display_digit(count % 10, 0b11111101);
        _delay_ms(50);
    }
}

void display_digit(uint8_t segments, uint8_t mask) {
  PORTB &= ~(1 << LATCH_PIN_DISPLAY);
  spi_transfer(mask);
  spi_transfer(digit_map[segments]);
  PORTB |= (1 << LATCH_PIN_DISPLAY);
}

uint8_t read_shift_register() {
    uint8_t data;
    PORTB &= ~(1 << LATCH_PIN_BUTTONS);
    _delay_us(1);
    PORTB |= (1 << LATCH_PIN_BUTTONS);
    data = spi_read();
    return data;
}
