/********************************************************************************
 * sound_fx_mixer
 * main.c
 * Note: this code uses snippets from pico audio example from raspberrypi.org
 * rev 1 - shabaz - Jan 2024
 ********************************************************************************/

#include <stdio.h>
#include <math.h>
#include "hardware/gpio.h"

// Pins 13,14,15 are used for I2S
#define PICO_AUDIO_I2S_DATA_PIN 13
#define PICO_AUDIO_I2S_CLOCK_PIN_BASE 14
#include "hardware/clocks.h"
#include "hardware/structs/clocks.h"
#include "pico/stdlib.h"
#include "pico/audio_i2s.h"
#include "pico/binary_info.h"
bi_decl(bi_3pins_with_names(PICO_AUDIO_I2S_DATA_PIN, "I2S DIN", PICO_AUDIO_I2S_CLOCK_PIN_BASE, "I2S BCK", PICO_AUDIO_I2S_CLOCK_PIN_BASE + 1, "I2S LRCK"));

// defines
#define COUNT_OF(x) ((sizeof(x) / sizeof(0 [x])) / ((size_t)(!(sizeof(x) % sizeof(0 [x])))))
// number of concurrent sounds that can be played out
#define NUM_VOICES 8
#define LED_PIN PICO_DEFAULT_LED_PIN
#define LED_ON gpio_put(LED_PIN, 1)
#define LED_OFF gpio_put(LED_PIN, 0)

/********* extern declarations for all the sound arrays *********/
extern const int16_t strike[13312];
extern const int16_t snip[10975];
extern const int16_t singlebeep[2280];
extern const int16_t needy_warning[60928];
extern const int16_t needy_activated[17823];
extern const int16_t doublebeep[35772];
extern const int16_t doublebeep_1_25[32817];
extern const int16_t alarm_clock_beep[88998];

// array of pointers to the sound arrays
const int16_t *sound_array[8] = {
    strike,           /*   0   */
    snip,             /*   1   */
    singlebeep,       /*   2   */
    needy_warning,    /*   3   */
    needy_activated,  /*   4   */
    doublebeep,       /*   5   */
    doublebeep_1_25,  /*   6   */
    alarm_clock_beep, /*   7   */
};

// definitions for convenience
#define STRIKE 0
#define SNIP 1
#define SINGLEBEEP 2
#define NEEDY_WARNING 3
#define NEEDY_ACTIVATED 4
#define DOUBLEBEEP 5
#define DOUBLEBEEP_1_25 6
#define ALARM_CLOCK_BEEP 7

// array of lengths of the sound arrays
const int sound_array_len[21] = {
    COUNT_OF(strike), COUNT_OF(snip), COUNT_OF(singlebeep), COUNT_OF(needy_warning), COUNT_OF(needy_activated),
    COUNT_OF(doublebeep), COUNT_OF(doublebeep_1_25), COUNT_OF(alarm_clock_beep)};

// map the buttons to particular sounds
// (it is OK to have multiple buttons mapped to the same sound if desired)
const int button_sound_map[8] = {STRIKE, SNIP, SINGLEBEEP, NEEDY_WARNING, NEEDY_ACTIVATED, DOUBLEBEEP, DOUBLEBEEP_1_25, ALARM_CLOCK_BEEP};

// button array
// GPIO numbers
const uint button_gpio[8] = {2, 3, 4, 5, 6, 7, 8, 9};

#define SAMPLES_PER_BUFFER 256

// ******** global variables ********
uint8_t last_button_state = 0;
// the voice_sound_index array stores a value from 0 to 20, which is the sound array index
// a value of -1 means the voice is not playing.
int voice_sound_index[NUM_VOICES];
uint32_t voice_sound_pos[NUM_VOICES]; // stores the current position in the sound array

// init_audio() function
struct audio_buffer_pool *init_audio()
{
    static audio_format_t audio_format = {
        .format = AUDIO_BUFFER_FORMAT_PCM_S16,
        .sample_freq = 24000,
        .channel_count = 1,
    };
    static struct audio_buffer_format producer_format = {
        .format = &audio_format,
        .sample_stride = 2};
    struct audio_buffer_pool *producer_pool = audio_new_producer_pool(&producer_format, 3, SAMPLES_PER_BUFFER); // todo correct size
    bool __unused ok;
    const struct audio_format *output_format;

    struct audio_i2s_config config = {
        .data_pin = PICO_AUDIO_I2S_DATA_PIN,
        .clock_pin_base = PICO_AUDIO_I2S_CLOCK_PIN_BASE,
        .dma_channel = 0,
        .pio_sm = 0,
    };
    output_format = audio_i2s_setup(&audio_format, &config);
    if (!output_format)
    {
        panic("PicoAudio: Unable to open audio device.\n");
    }
    ok = audio_i2s_connect(producer_pool);
    assert(ok);
    audio_i2s_set_enabled(true);

    return producer_pool;
}

void board_init(void)
{
    int i;
    // setup buttons
    for (i = 0; i < 8; i++)
    {
        gpio_init(button_gpio[i]);
        gpio_set_dir(button_gpio[i], GPIO_IN);
        gpio_pull_up(button_gpio[i]); // pullup enabled
    }

    // setup LED
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    LED_OFF;

    // initialize arrays
    for (i = 0; i < NUM_VOICES; i++)
    {
        voice_sound_index[i] = -1;
        voice_sound_pos[i] = 0;
    }

    sleep_ms(100); // let any attached hardware settle
}

// returns a byte with the current button level (1 = pressed)
uint8_t get_button_level(void)
{
    uint8_t button_level = 0;
    int i;
    for (i = 0; i < 8; i++)
    {
        if (gpio_get(button_gpio[i]) == false)
        { // button is pressed!
            button_level |= (1 << i);
        }
    }
    if (button_level != 0)
    {
        // printf("get_button_level: %02x, last_button_state: %02x\n", button_level, last_button_state);
    }
    return button_level;
}

// returns a byte with only new pressed buttons (1 = pressed)
uint8_t get_new_button_press(void)
{
    uint8_t button_state = get_button_level();
    uint8_t button_press = button_state & ~last_button_state;
    last_button_state = button_state;
    if (button_state != 0)
    {
        printf("get_new_button_press: %02x\n", button_press);
    }
    return button_press;
}

bool repeating_timer_callback(struct repeating_timer *t)
{
    // find a free voice
    for (int k = 0; k < NUM_VOICES; k++)
    {
        if (voice_sound_index[k] == -1)
        {
            // found a free voice
            voice_sound_index[k] = button_sound_map[DOUBLEBEEP];
            voice_sound_pos[k] = 0;
            break;
        }
    }

    return true;
}

int main()
{
    uint8_t buttons;
    int j;

    stdio_init_all();
    board_init();

    struct audio_buffer_pool *ap = init_audio();

    struct repeating_timer timer;
    // Negative delay so means we will call repeating_timer_callback, and call it again
    // 500ms later regardless of how long the callback took to execute
    add_repeating_timer_ms(-1000, repeating_timer_callback, NULL, &timer);

    while (true)
    {
        // forever loop
        LED_OFF;
        // check if any buttons have been pressed
        buttons = get_new_button_press();
        if (buttons)
        {
            // a button has been pressed
            for (j = 0; j < 8; j++)
            {
                if (buttons & (1 << j))
                {
                    // a button has been pressed
                    LED_ON;
                    // find a free voice
                    for (int k = 0; k < NUM_VOICES; k++)
                    {
                        if (voice_sound_index[k] == -1)
                        {
                            // found a free voice
                            voice_sound_index[k] = button_sound_map[j];
                            voice_sound_pos[k] = 0;
                            break;
                        }
                    }
                }
            }
        }

        // fill the audio sample buffer
        struct audio_buffer *buffer = take_audio_buffer(ap, true);
        int16_t *samples = (int16_t *)buffer->buffer->bytes;
        for (uint i = 0; i < buffer->max_sample_count; i++)
        {
            // create a sample by adding all the voices together
            samples[i] = 0;
            for (j = 0; j < NUM_VOICES; j++)
            {
                if (voice_sound_index[j] != -1)
                {
                    // add one eighth of the amplitude to the sample
                    samples[i] += (sound_array[voice_sound_index[j]][voice_sound_pos[j]]) >> 3;
                    voice_sound_pos[j]++;
                    // if the sound has finished, set the index to -1
                    if (voice_sound_pos[j] >= sound_array_len[voice_sound_index[j]])
                    {
                        voice_sound_index[j] = -1;
                    }
                }
            }
        }
        buffer->sample_count = buffer->max_sample_count;
        give_audio_buffer(ap, buffer);
    }
    puts("\n");
    return 0;
}
