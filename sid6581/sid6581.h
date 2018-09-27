#ifndef SID_6581_H
#define SID_6581_H
#include "Arduino.h"

class Sid6581 {
public:
    Sid6581();

    void poke(uint8_t addr, uint8_t data);
    void cycle_cs();
    void set_master_volume(uint8_t vol);
    void reset();

private:
    const int rw_pin = 7;
    const int cs_pin = 8;
    const int reset_pin = 9;

    const int sr_data_out = 2;
    const int shift_clk = 4;
    const int store_clk = 3;
    const int sr_reset = 5;
};

#endif

