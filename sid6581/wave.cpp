#include "Arduino.h"
#include "sid6581.h"
#include "wave.h"

#include <stdint.h>

WaveForm::WaveForm(uint8_t a, uint8_t d, uint8_t s, uint8_t r)
{
    ad = (a << 4) | d;
    sr = (s << 4) | r;
}

WaveForm WaveForm::noise(uint8_t a, uint8_t d, uint8_t s, uint8_t r)
{
    WaveForm ret(a, d, s, r);
    ret.control_reg = 0x80;
    return ret;
}

WaveForm WaveForm::square(uint8_t a, uint8_t d, uint8_t s, uint8_t r, uint16_t pwm)
{
    WaveForm ret(a, d, s, r);
    ret.control_reg = 0x40;
    ret.pwm = pwm;
    return ret;
}

WaveForm WaveForm::saw(uint8_t a, uint8_t d, uint8_t s, uint8_t r)
{
    WaveForm ret(a, d, s, r);
    ret.control_reg = 0x20;
    return ret;
}

WaveForm WaveForm::triangle(uint8_t a, uint8_t d, uint8_t s, uint8_t r)
{
    WaveForm ret(a, d, s, r);
    ret.control_reg = 0x10;
    return ret;
}

uint8_t WaveForm::get_control_reg() const noexcept 
{ 
    return control_reg;
}

uint8_t WaveForm::get_pwm() const noexcept 
{ 
    return pwm;
}

uint8_t WaveForm::get_ad() const noexcept
{
    return ad;
}

uint8_t WaveForm::get_sr() const noexcept
{
    return sr;
}

void WaveForm::set_pwm(uint16_t val)
{
    pwm = val;
}

void WaveForm::set_square()
{
    control_reg = 0x40;
}

void WaveForm::set_noise()
{
    control_reg = 0x80;
}

void WaveForm::set_triangle()
{
    control_reg = 0x10;
}

void WaveForm::set_saw()
{
    control_reg = 0x20;
}
