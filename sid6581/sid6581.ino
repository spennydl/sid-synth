#include "sid6581.h"
#include "wave_table.h"


//#define DEBUG


constexpr static int clock_pin = 6;

struct VoiceProfile {
    uint8_t control;
    uint8_t freq_lo;
    uint8_t freq_hi;
    uint8_t pw_low;
    uint8_t pw_hi;
    uint8_t ad;
    uint8_t sr;
};

VoiceProfile voice_one = {
    .control = 0x04,
    .freq_lo = 0x00,
    .freq_hi = 0x01,
    .pw_low  = 0x02,
    .pw_hi   = 0x03,
    .ad      = 0x05,
    .sr      = 0x06
};

VoiceProfile voice_two = {
    .control = 0x0B,
    .freq_lo = 0x07,
    .freq_hi = 0x08,
    .pw_low  = 0x09,
    .pw_hi   = 0x0A,
    .ad      = 0x0C,
    .sr      = 0x0D
};

Sid6581 *sid;

void clk_setup()
{
    //pinMode(clock_pin, OUTPUT);

    /*
    GCLK->GENDIV.reg = GCLK_GENDIV_DIV(8) |          // Divide the 48MHz clock source by divisor 48: 48MHz/48=1MHz
                        GCLK_GENDIV_ID(4);            // Select Generic Clock (GCLK) 4
    while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

    GCLK->GENCTRL.reg = GCLK_GENCTRL_GENEN |         // Enable GCLK4
                        GCLK_GENCTRL_SRC_OSC8M |   // Set the 48MHz clock source
                        GCLK_GENCTRL_ID(4) |
                        GCLK_GENCTRL_RUNSTDBY |
                        GCLK_GENCTRL_OE;
    while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

    */
    
    REG_GCLK_GENDIV = GCLK_GENDIV_DIV(8) | GCLK_GENDIV_ID(4);
    while (GCLK->STATUS.bit.SYNCBUSY);

    REG_GCLK_GENCTRL = GCLK_GENCTRL_OE |    // Enable the GCLK output
                       GCLK_GENCTRL_IDC |   // Set the duty cycle to 50/50 HIGH/LOW
                       GCLK_GENCTRL_GENEN | // Enable GCLK 4
                       GCLK_GENCTRL_RUNSTDBY |
                       GCLK_GENCTRL_SRC_OSC8M | // Set the clock source to the external 32.768kHz
                       GCLK_GENCTRL_ID(4);      // Set clock source on GCLK 4
    while (GCLK->STATUS.bit.SYNCBUSY)
        ; // Wait for synchronization

    //REG_GCLK_CLKCTRL = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK4;
    //while (GCLK->STATUS.bit.SYNCBUSY)
        //;

    // Enable the port multiplexer on digital pin 6
    PORT->Group[g_APinDescription[6].ulPort].PINCFG[g_APinDescription[6].ulPin].bit.PMUXEN = 1;

    // Switch the port multiplexer to peripheral H (GCLK_IO[4])
    PORT->Group[g_APinDescription[6].ulPort].PMUX[g_APinDescription[6].ulPin >> 1].reg |= PORT_PMUX_PMUXE_H;

    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(4) |
                        GCLK_CLKCTRL_CLKEN |
                        GCLK_CLKCTRL_GEN_GCLK4;
    while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization
}


void setup()
{
    Serial.begin(9600);

    //while (!Serial)
        //continue;

    //if (Serial) Serial.println("Serial conn established");

    clk_setup();
    Serial1.begin(31250, SERIAL_8N1);

    while (!Serial1)
        continue;

    //Serial.println("Clock is set up");


    //Serial.println("Hardware serial established");

    sid = new Sid6581();

    for (uint8_t i = 0; i < 0x19; i++)
    {
        sid->poke(i, 0x00);
    }
    sid->reset();

    sid->set_master_volume(0x0f);

    sid->poke(voice_one.pw_low, 0x00);
    sid->poke(voice_one.pw_hi, 0x05);

    sid->poke(voice_one.ad, 0x96);
    sid->poke(voice_one.sr, 0x98);

    sid->poke(voice_two.pw_low, 0x00);
    sid->poke(voice_two.pw_hi, 0x05);

    sid->poke(voice_two.ad, 0x35);
    sid->poke(voice_two.sr, 0x94);
}

static wave_table_entry score_voice_one[] = {

    {.cr = 0x41, .adsr = 0x349a, .note = A(2), .legato = true, .reset_cr = false},
    {.cr = 0x40, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},

    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},

    {.cr = 0x81, .adsr = 0x1144, .note = C(6), .legato = false, .reset_cr = false},
    {.cr = 0x80, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},

    {.cr = 0x41, .adsr = 0x349a, .note = A(2), .legato = true, .reset_cr = false},
    {.cr = 0x40, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},

    {.cr = 0x41, .adsr = 0x00, .note = G(2), .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x40, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},

    {.cr = 0x41, .adsr = 0x00, .note = A(2), .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x40, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},

    {.cr = 0x81, .adsr = 0x1144, .note = C(6), .legato = false, .reset_cr = false},
    {.cr = 0x80, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},

    {.cr = 0x41, .adsr = 0x349a, .note = F_(2), .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},

    {.cr = 0x40, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},

    {.cr = 0x41, .adsr = 0x00, .note = F_(2), .legato = false, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x40, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},

    {.cr = 0x81, .adsr = 0x1144, .note = C(6), .legato = false, .reset_cr = false},
    {.cr = 0x80, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},

    {.cr = 0x41, .adsr = 0x349a, .note = F_(2), .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},

    {.cr = 0x40, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},

    {.cr = 0x41, .adsr = 0x00, .note = G(2), .legato = false, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x40, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},

    {.cr = 0x81, .adsr = 0x1144, .note = C(6), .legato = false, .reset_cr = false},
    {.cr = 0x80, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},

    {.cr = 0x41, .adsr = 0x349a, .note = G(2), .legato = false, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x40, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
};

static wave_table_entry score_voice_two[] = {

    {.cr = 0x41, .adsr = 0x33Fa, .note = C(5), .legato = true, .reset_cr = true},
    {.cr = 0x00, .adsr = 0x00, .note = E(4), .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = C(5), .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = E(4), .legato = true, .reset_cr = false},

    {.cr = 0x00, .adsr = 0x00, .note = C(5), .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = E(4), .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = C(5), .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = E(4), .legato = true, .reset_cr = false},

    {.cr = 0x00, .adsr = 0x00, .note = C(5), .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = E(4), .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = C(5), .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = E(4), .legato = true, .reset_cr = false},

    {.cr = 0x00, .adsr = 0x00, .note = C(5), .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = E(4), .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = C(5), .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = E(4), .legato = true, .reset_cr = false},

    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},

    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},

    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},

    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},

    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},

    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},

    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},

    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},

    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},

    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},

    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},

    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
    {.cr = 0x00, .adsr = 0x00, .note = 0x00, .legato = true, .reset_cr = false},
};

double bpm = 140.0;

double bps = bpm / 60.0;
unsigned long delay_rate = (1000.0 / bps) / 8; // sixteenth notes
uint8_t last_cr_v1 = 0;
uint8_t last_cr_v2 = 0;

uint16_t pwm = 0x0040;
uint16_t pwm_voice2 = 0x0040;

int pos = 0;
int end = 64;

uint8_t last_note = 0;
void loop()
{
    /*
    if (pos++ == 0) {
        uint8_t note_lo = (score_voice_one[0].note & 0xff);
        uint8_t note_hi = (score_voice_one[0].note >> 8);
        sid->poke(voice_one.freq_lo, note_lo);
        sid->poke(voice_one.freq_hi, note_hi);

        sid->poke(voice_one.control, score_voice_one[0].cr);

        return;
    }
    //{.cr = 0x41, .adsr = 0x349a, .note = A(2), .legato = true, .reset_cr = false},
    uint16_t notev1 = A(4);
    uint8_t note_lo = (notev1 & 0xff);
    uint8_t note_hi = (notev1 >> 8);

    uint16_t notev2 = E(5);
    uint8_t note_lo2 = (notev2 & 0xff);
    uint8_t note_hi2 = (notev2 >> 8);

    sid->poke(voice_one.freq_lo, note_lo);
    sid->poke(voice_one.freq_hi, note_hi);
    sid->poke(voice_one.control, 0x41);

    sid->poke(voice_two.freq_lo, note_lo2);
    sid->poke(voice_two.freq_hi, note_hi2);
    sid->poke(voice_two.control, 0x41);
    delay(300);
    sid->poke(voice_one.control, 0x40);
    sid->poke(voice_two.control, 0x40);
    delay(2000);
    */


    while (Serial1.available()) {
        uint8_t status = Serial1.read();
        //Serial.print("Got status: "); Serial.println(status, DEC);

        uint8_t upper_nibble = status & 0xF0;
        uint8_t lower_nibble = status & 0x0f;
        //Serial.print("upper nibble: "); Serial.println(upper_nibble, HEX);
        if (upper_nibble == 0x90) { // NOTE ON
            // read 2 more bytes
            while (!Serial1.available()) continue;
            uint8_t note = Serial1.read();

            while(!Serial1.available()) continue;
            uint8_t pressure = Serial1.read();

            //Serial.print("Note: "); Serial.println(note, DEC);
            //Serial.print("pressure: "); Serial.println(pressure, DEC);

            uint16_t noteval = note_table[note];
            uint8_t note_lo = (noteval & 0xff);
            uint8_t note_hi = (noteval >> 8);

            sid->poke(voice_one.freq_lo, note_lo);
            sid->poke(voice_one.freq_hi, note_hi);
            sid->poke(voice_one.control, 0x41);

            last_note = note;
        } else if (upper_nibble == 0x80) { // NOTE OFF
            // read 2 bytes
            while (!Serial1.available()) continue;
            uint8_t note = Serial1.read(); // don't need?

            while (!Serial1.available()) continue;
            uint8_t p = Serial1.read(); // throw away

            if (note == last_note) {
                sid->poke(voice_one.control, 0x40);
            }
        }
    }

    // v2 - TODO: not this

/*
    if (score_voice_one[pos].note != 0)
    {
        uint8_t note_lo = (score_voice_one[pos].note & 0xff);
        uint8_t note_hi = (score_voice_one[pos].note >> 8);

        if ((last_cr_v1 & 1) && !(score_voice_one[pos].legato))
        {
            sid->poke(voice_one.control, last_cr_v1 ^ 0x01);
        }

        sid->poke(voice_one.freq_lo, note_lo);
        sid->poke(voice_one.freq_hi, note_hi);

        if ((last_cr_v1 & 1) && !(score_voice_one[pos].legato))
        {
            sid->poke(voice_one.control, last_cr_v1);
        }
    }

    if (score_voice_one[pos].adsr != 0)
    {
        uint8_t sr = (score_voice_one[pos].adsr & 0xff);
        uint8_t ad = (score_voice_one[pos].adsr >> 8);
        sid->poke(voice_one.ad, ad);
        sid->poke(voice_one.sr, sr);
    }

    if (score_voice_one[pos].cr != 0 || score_voice_one[pos].reset_cr)
    {
        sid->poke(voice_one.control, score_voice_one[pos].cr);
        last_cr_v1 = score_voice_one[pos].cr;
    }

    uint8_t pwlo = pwm & 0xff;
    uint8_t pwhi = (pwm >> 8);
    sid->poke(voice_one.pw_low, pwlo);
    sid->poke(voice_one.pw_hi, pwhi);

    pwm += 16;
    if (pwm > 0x0ff0)
    {
        pwm = 0x000F;
    }

    if (score_voice_one[pos].note != 0)
    {
        Serial.println("VOICE TWO NOTE");
        uint8_t note_lo = (score_voice_one[pos].note & 0xff);
        uint8_t note_hi = (score_voice_one[pos].note >> 8);

        Serial.print("High: "); Serial.println(note_hi);
        Serial.print("Low: "); Serial.println(note_lo);
        if ((last_cr_v2 & 1) && !(score_voice_one[pos].legato))
        {
            sid->poke(voice_two.control, last_cr_v2 ^ 0x01);
        }

        sid->poke(voice_two.freq_lo, note_lo);
        sid->poke(voice_two.freq_hi, note_hi);

        if ((last_cr_v2 & 1) && !(score_voice_one[pos].legato))
        {
            sid->poke(voice_two.control, last_cr_v2);
        }
    }

    if (score_voice_one[pos].adsr != 0)
    {
        uint8_t sr = (score_voice_one[pos].adsr & 0xff);
        uint8_t ad = (score_voice_one[pos].adsr >> 8);
        sid->poke(voice_two.ad, ad);
        sid->poke(voice_two.sr, sr);
    }

    if (score_voice_one[pos].cr != 0 || score_voice_one[pos].reset_cr)
    {
        sid->poke(voice_two.control, score_voice_two[pos].cr);
        last_cr_v2 = score_voice_one[pos].cr;
    }

    uint8_t pwlo2 = pwm_voice2 & 0xff;
    uint8_t pwhi2 = (pwm_voice2 >> 8);
    //sid->poke(voice_two.pw_low, pwlo2);
    //sid->poke(voice_two.pw_hi, pwhi2);

    pwm_voice2 += 64;
    if (pwm_voice2 > 0x0ff0)
    {
        pwm_voice2 = 0x000F;
    }

    if (++pos == end)
    {
        pos = 0;
    }

    unsigned long elapsed = millis() - start;
    delay(delay_rate - elapsed);
    */
}
