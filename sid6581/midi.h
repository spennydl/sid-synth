#ifndef MIDI_H
#define MIDI_H

#include "Arduino.h"

enum class MidiMessageType
{
    NOTE_ON,
    NOTE_OFF,
    PATCH_CHANGE,
    NOSUP //not supported
};

struct MidiMessage
{
    uint8_t channel;
    MidiMessageType type;
    uint8_t data[2];
};


class Midi {
    public:
        void open() const;
        MidiMessage get_next_message() const;
        MidiMessageType parse_message_type(uint8_t raw) const;
    private:

};

#endif // MIDI_H