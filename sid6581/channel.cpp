#include "channel.h"
#include "sid6581.h"
#include "wave_table.h"
#include "midi.h"

void Channel::handle_message(const MidiMessage& message)
{
    switch (message.type) {
    case MidiMessageType::NOTE_ON: {
        // data is note and pressure
        uint8_t midi_note = message.data[0];

        // get the note from our lookup table and split it
        uint16_t note = note_table[midi_note];
        uint8_t note_lo = (note & 0xff);
        uint8_t note_hi = (note >> 8);

        // write note value
        sid->poke(registers->note_lo, note_lo);
        sid->poke(registers->note_hi, note_hi);

        // turn voice output on
        Serial.print(registers->ctrl, HEX); Serial.print(" "); Serial.println(voices[voice_idx].ctrl | 0x01, HEX);
        sid->poke(registers->ctrl, (voices[voice_idx].ctrl | 0x01));

        last_note = midi_note;
        is_on = true;

        break;
    } case MidiMessageType::NOTE_OFF: {
        // data is note and 0
        uint8_t note = message.data[0];
        if (note == last_note) { // prevents prematurely killing notes when they overlap
            Serial.print("Writing off: "); Serial.println(registers->ctrl);
            sid->poke(registers->ctrl, voices[voice_idx].ctrl);
            is_on = false;
        }
        break;
    } case MidiMessageType::PATCH_CHANGE: {
        // data will be patch number
        uint8_t patch_no = message.data[0];
        if (patch_no < 16) {
            current_voice = &(voices[patch_no]);
            voice_idx = patch_no;
            reset_voice();
        }
        break;
    } default: // should never happen?
        break;
    }
}

void Channel::reset_voice()
{
    sid->poke(registers->pw_lo, voices[voice_idx].pw_lo);
    sid->poke(registers->pw_hi, voices[voice_idx].pw_hi);

    sid->poke(registers->ad, voices[voice_idx].ad);
    sid->poke(registers->sr, voices[voice_idx].sr);

    sid->poke(registers->ctrl, voices[voice_idx].ctrl & 0xFE);
}