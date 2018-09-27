#ifndef WAVE_H
#define WAVE_H
#include "Arduino.h"

#include "sid6581.h"

class WaveForm {
public:

    uint8_t get_control_reg() const noexcept;
    uint8_t get_pwm() const noexcept;
    uint8_t get_ad() const noexcept;
    uint8_t get_sr() const noexcept;
    void set_pwm(uint16_t val);

    static WaveForm noise(uint8_t a, uint8_t d, uint8_t s, uint8_t r);
    static WaveForm square(uint8_t a, uint8_t d, uint8_t s, uint8_t r, uint16_t pwm);
    static WaveForm saw(uint8_t a, uint8_t d, uint8_t s, uint8_t r);
    static WaveForm triangle(uint8_t a, uint8_t d, uint8_t s, uint8_t r);

    void set_square();
    void set_noise();
    void set_triangle();
    void set_saw();

private:

    WaveForm(uint8_t a, uint8_t d, uint8_t s, uint8_t r);

    uint8_t ad, sr;
    uint8_t control_reg;
    uint16_t pwm;
};

struct VoiceProfile {
    uint8_t control;
    uint8_t freq_lo;
    uint8_t freq_hi;
    uint8_t pw_low;
    uint8_t pw_hi;
    uint8_t ad;
    uint8_t sr;
};

class Voice {
public:
    Voice(WaveForm w, VoiceProfile p) : wave(w), profile(p)
    {
    }

    void init(Sid6581& sid, bool on = false)
    {
        sid.poke(profile.ad, wave.get_ad());
        sid.poke(profile.sr, wave.get_sr());

        uint16_t pwm = wave.get_pwm();
        uint8_t pwm_lo = pwm & 0xff;
        uint8_t pwm_hi = pwm >> 8;
        sid.poke(profile.pw_low, pwm_lo);
        sid.poke(profile.pw_hi, pwm_hi);

        uint8_t cr = wave.get_control_reg();
        if (on)
            cr |= 0x01;

        sid.poke(profile.control, cr);
    }

    void note_on(Sid6581& sid, uint16_t note)
    {
        uint8_t cr = wave.get_control_reg();
        cr |= 0x01;
        uint8_t note_lo = (note & 0xff);
        uint8_t note_hi = (note >> 8);
        sid.poke(profile.freq_lo, note_lo);
        sid.poke(profile.freq_hi, note_hi);
        sid.poke(profile.control, cr);
    }

    void note_off(Sid6581& sid)
    {
        sid.poke(profile.control, wave.get_control_reg());
    }

    void set_wave_form(WaveForm& w) 
    {
        wave = wave;
    }

    WaveForm& get_wave() { return wave; }

private:
    WaveForm wave;
    VoiceProfile profile;
};

#endif
