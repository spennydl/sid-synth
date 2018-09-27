#include "Arduino.h"

static uint32_t frequencies[] = {
    0x892B,
    0x9153,
    0x99F7,
    0xA31F,
    0xACD2,
    0xB719,
    0xC1FC,
    0xCD85,
    0xD98D,
    0xE6B0,
    0xF467,
    0x1FFF0
};

#define C(octave) (frequencies[0] >> (7 - (octave)))
#define Db(octave) (frequencies[1] >> (7 - (octave)))
#define D(octave) (frequencies[2] >> (7 - (octave)))
#define Eb(octave) (frequencies[3] >> (7 - (octave)))
#define E(octave) (frequencies[4] >> (7 - (octave)))
#define F_(octave) (frequencies[5] >> (7 - (octave)))
#define Gb(octave) (frequencies[6] >> (7 - (octave)))
#define G(octave) (frequencies[7] >> (7 - (octave)))
#define Ab(octave) (frequencies[8] >> (7 - (octave)))
#define A(octave) (frequencies[9] >> (7 - (octave)))
#define Bb(octave) (frequencies[10] >> (7 - (octave)))
#define B(octave) (frequencies[11] >> (8 - (octave)))

static uint16_t note_table[] = {
    C(1),
    Db(1),
    D(1),
    Eb(1),
    E(1),
    F_(1),
    Gb(1),
    G(1),
    Ab(1),
    A(1),
    Bb(1),
    B(1),

    C(2),
    Db(2),
    D(2),
    Eb(2),
    E(2),
    F_(2),
    Gb(2),
    G(2),
    Ab(2),
    A(2),
    Bb(2),
    B(2),

    C(3),
    Db(3),
    D(3),
    Eb(3),
    E(3),
    F_(3),
    Gb(3),
    G(3),
    Ab(3),
    A(3),
    Bb(3),
    B(3),

    C(4),
    Db(4),
    D(4),
    Eb(4),
    E(4),
    F_(4),
    Gb(4),
    G(4),
    Ab(4),
    A(4),
    Bb(4),
    B(4),

    C(5),
    Db(5),
    D(5),
    Eb(5),
    E(5),
    F_(5),
    Gb(5),
    G(5),
    Ab(5),
    A(5),
    Bb(5),
    B(5),

    C(6),
    Db(6),
    D(6),
    Eb(6),
    E(6),
    F_(6),
    Gb(6),
    G(6),
    Ab(6),
    A(6),
    Bb(6),
    B(6),

    C(7),
    Db(7),
    D(7),
    Eb(7),
    E(7),
    F_(7),
    Gb(7),
    G(7),
    Ab(7),
    A(7),
    Bb(7),
    B(7),
};

struct wave_table_entry {
    uint8_t cr;
    uint16_t adsr;
    uint16_t note;
    bool legato;
    bool reset_cr;
};

struct inst_table_entry {
    uint8_t addr;
    uint8_t data;
    uint8_t next_instr_offset;
};

