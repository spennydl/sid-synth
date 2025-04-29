#include "sid6581.h"
#include "wave_table.h"
#include "channel.h"
#include "midi.h"

//#define DEBUG

constexpr static int clock_pin = 6;

//----------------------------------------------------------------------------
// voice register config
//----------------------------------------------------------------------------
VoiceRegisters voice_one_reg = {
    .ctrl    = 0x04,
    .note_lo = 0x00,
    .note_hi = 0x01,
    .pw_lo   = 0x02,
    .pw_hi   = 0x03,
    .ad      = 0x05,
    .sr      = 0x06
};

VoiceRegisters voice_two_reg = {
    .ctrl    = 0x0B,
    .note_lo = 0x07,
    .note_hi = 0x08,
    .pw_lo   = 0x09,
    .pw_hi   = 0x0A,
    .ad      = 0x0C,
    .sr      = 0x0D
};

VoiceRegisters voice_three_reg = {
    .ctrl    = 0x12,
    .note_lo = 0x0E,
    .note_hi = 0x0F,
    .pw_lo   = 0x10,
    .pw_hi   = 0x11,
    .ad      = 0x13,
    .sr      = 0x14
};

//----------------------------------------------------------------------------
// voice profile config
//----------------------------------------------------------------------------
VoiceProfile channel_one_voices[16] = {
    // squares
{
    .ctrl  = 0x40,
    .pw_lo = 0x60,
    .pw_hi = 0x00,
    .ad    = 0x38,
    .sr    = 0xA9
},

{
    .ctrl  = 0x40,
    .pw_lo = 0x80,
    .pw_hi = 0x00,
    .ad    = 0x38,
    .sr    = 0xC6
},

{
    .ctrl  = 0x40,
    .pw_lo = 0x10,
    .pw_hi = 0x00,
    .ad    = 0x38,
    .sr    = 0xFA
},

{
    .ctrl  = 0x40,
    .pw_lo = 0x00,
    .pw_hi = 0x04,
    .ad    = 0x38,
    .sr    = 0xFA
},

// saws
{
    .ctrl  = 0x20,
    .pw_lo = 0x00,
    .pw_hi = 0x00,
    .ad    = 0x38,
    .sr    = 0xA9
},

{
    .ctrl  = 0x20,
    .pw_lo = 0x00,
    .pw_hi = 0x00,
    .ad    = 0x38,
    .sr    = 0xC6
},

{
    .ctrl  = 0x20,
    .pw_lo = 0x00,
    .pw_hi = 0x00,
    .ad    = 0x38,
    .sr    = 0xFA
},

{
    .ctrl  = 0x20,
    .pw_lo = 0x00,
    .pw_hi = 0x04,
    .ad    = 0x38,
    .sr    = 0xFA
},

// noise
{
    .ctrl  = 0x80,
    .pw_lo = 0x10,
    .pw_hi = 0x00,
    .ad    = 0x22,
    .sr    = 0x86
},

{
    .ctrl  = 0x80,
    .pw_lo = 0x10,
    .pw_hi = 0x00,
    .ad    = 0x27,
    .sr    = 0xAA
},

{
    .ctrl  = 0x80,
    .pw_lo = 0x10,
    .pw_hi = 0x00,
    .ad    = 0xA5,
    .sr    = 0x89
},

{
    .ctrl  = 0x80,
    .pw_lo = 0x10,
    .pw_hi = 0x00,
    .ad    = 0xA5,
    .sr    = 0x99
},
{
    .ctrl  = 0x10,
    .pw_lo = 0x60,
    .pw_hi = 0x00,
    .ad    = 0x38,
    .sr    = 0xA9
},

{
    .ctrl  = 0x10,
    .pw_lo = 0x80,
    .pw_hi = 0x00,
    .ad    = 0x38,
    .sr    = 0xC6
},

{
    .ctrl  = 0x10,
    .pw_lo = 0x10,
    .pw_hi = 0x00,
    .ad    = 0x38,
    .sr    = 0xFA
},

{
    .ctrl  = 0x10,
    .pw_lo = 0x00,
    .pw_hi = 0x04,
    .ad    = 0x38,
    .sr    = 0xFA
},
// sine
};

Sid6581 *sid;

Channel *channels[3];

Midi *midi;

void clk_setup()
{
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
    
    // no waiting unless we need to, the clocks are weird. -_-
    // uncomment if you need it
    //while (Serial.available())
        //continue;

    Serial.println("On");

    // set up the clock
    clk_setup();

    // set up midi
    midi = new Midi();
    midi->open();
    
    // make the sid
    sid = new Sid6581();
    sid->reset();
    sid->set_master_volume(0x0f);

    // cheeky filter
    sid->poke(0x16, 0x92);
    sid->poke(0x17, 0xF3);
    sid->poke(0x18, 0x5F);

    // set up channels
    channels[0] = new Channel(sid, &voice_one_reg, channel_one_voices);
    channels[1] = new Channel(sid, &voice_two_reg, channel_one_voices);
    channels[2] = new Channel(sid, &voice_three_reg, channel_one_voices);
    Serial.println("End of setup");
}

void loop()
{
    MidiMessage message = midi->get_next_message();

    if (message.channel < 3)
        channels[message.channel]->handle_message(message);
}
