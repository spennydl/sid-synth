#include "midi.h"
#include "Arduino.h"

void Midi::open() const
{
    //TODO: abstractionnnn
    Serial1.begin(31250);
    
    while (!Serial1) {
        continue;
    }
}

MidiMessage Midi::get_next_message() const
{
    while (!Serial1.available())
        ; // block for next message

    uint8_t status = Serial1.read();

    uint8_t raw_message_type = status & 0xF0;
    uint8_t channel = status & 0x0f;

    MidiMessageType message_type = parse_message_type(raw_message_type);

    switch (message_type) {
        case MidiMessageType::NOTE_ON:
        case MidiMessageType::NOTE_OFF: {
            // need to read 2 more bytes
            while (!Serial1.available())
                continue;
            uint8_t note = Serial1.read(); // sure wish this blocked :/

            while (!Serial1.available())
                continue;
            uint8_t pressure = Serial1.read();

            return {
                .channel = channel,
                .type = message_type,
                .data = {note, pressure}};

        } case MidiMessageType::PATCH_CHANGE: {
            // expect one byte
            while (!Serial1.available())
                continue;
            uint8_t voice_number = Serial1.read(); // sure wish this blocked :/

            return {
                .channel = channel,
                .type = message_type,
                .data = {voice_number}};

        } case MidiMessageType::NOSUP:
        default:
            return
            {
                .channel = channel,
                .type = message_type,
                .data = {0}
            };
        }
}

MidiMessageType Midi::parse_message_type(uint8_t raw) const
{
    switch (raw) {
        case 0x90:
            return MidiMessageType::NOTE_ON;
        case 0x80:
            return MidiMessageType::NOTE_OFF;
        case 0xC0:
            return MidiMessageType::PATCH_CHANGE;
        default:
            return MidiMessageType::NOSUP;
        }
}