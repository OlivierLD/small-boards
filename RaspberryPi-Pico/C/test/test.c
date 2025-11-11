#include <stdio.h>
#include <stdlib.h>  // For rand()
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/binary_info.h"

#define TRUE 1
#define FALSE 0

#define ON 1
#define OFF 0

const uint LED_PIN = 25;

int main() {

    // Like the "setup"...
    bi_decl(bi_program_description("This is a test binary."));
    bi_decl(bi_1pin_with_name(LED_PIN, "On-board LED"));

    stdio_init_all();char destination[50];
                     sprintf(destination, "Hello %s!", "World");
                     printf("%s", destination);

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Like the "loop"...
    while (TRUE) {
        gpio_put(LED_PIN, OFF);
        sleep_ms(250);
        gpio_put(LED_PIN, ON);
        int r = rand();
        char to_display[50];
        // sprintf(to_display, "Hello %d!\n", r);
        sprintf(to_display, "Hello %d !", r); // No CR
        // printf("%s", to_display);
        puts(to_display);
        sleep_ms(1000);
    }
}