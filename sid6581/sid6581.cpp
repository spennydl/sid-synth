#include "sid6581.h"
#include "Arduino.h"

Sid6581::Sid6581()
{
    //start with a reset
    digitalWrite(reset_pin, LOW);

    // set up serial out
    pinMode(sr_data_out, OUTPUT);

    // set up shift register pins
    pinMode(shift_clk, OUTPUT);
    pinMode(store_clk, OUTPUT);
    pinMode(sr_reset, OUTPUT);
    //pinMode(sr_oe, OUTPUT);

    // reset the shift register just for kicks
    digitalWrite(sr_reset, LOW);
    delay(20);
    digitalWrite(sr_reset, HIGH);

    // finish sr setup, turn on output
    //digitalWrite(sr_oe, LOW);

    // chip select is active low and allows us to write
    pinMode(cs_pin, OUTPUT);
    digitalWrite(cs_pin, HIGH);

    // read/write will always be low, we don't need to read
    pinMode(rw_pin, OUTPUT);
    digitalWrite(rw_pin, LOW);

    //finish the reset
    delay(200);
    digitalWrite(reset_pin, HIGH);
    delay(200);

    // time to write data!
    // zero everything
    for (int i = 0; i < 0x1C; i++) {
        poke(i, 0x00);
    }
}

void Sid6581::poke(uint8_t addr, uint8_t data) const
{
    digitalWrite(store_clk, LOW);
    shiftOut(sr_data_out, shift_clk, MSBFIRST, addr);
    shiftOut(sr_data_out, shift_clk, MSBFIRST, data);
    digitalWrite(store_clk, HIGH);

    // data should be on the bus, send to chip
    cycle_cs();
}

void Sid6581::cycle_cs() const
{
    digitalWrite(cs_pin, LOW);
    delayMicroseconds(200);
    digitalWrite(cs_pin, HIGH);
}

void Sid6581::set_master_volume(uint8_t vol) const
{
    uint16_t val = vol & 0x0f;
    poke(0x18, val);
}

void Sid6581::reset() const
{
    digitalWrite(reset_pin, LOW);
    delay(1000);
    digitalWrite(reset_pin, HIGH);
}
