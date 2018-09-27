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
        sid->poke(registers->ctrl, current_voice->ctrl | 0x01);

        last_note = note;
        is_on = true;

        break;
    } case MidiMessageType::NOTE_OFF: {
        // data is note and 0
        uint8_t note = message.data[0];
        if (note != last_note) { // prevents prematurely killing notes when they overlap
            sid->poke(registers->ctrl, current_voice->ctrl);
            is_on = false;
        }
        break;
    } case MidiMessageType::PATCH_CHANGE: {
        // data will be patch number
        uint8_t patch_no;
        if (patch_no > 2) {
            break; // only support up to 3 
        }

        current_voice = &voices[patch_no];
        reset_voice();
        break;
    } default: // should never happen?
        break;
    }
}

void Channel::reset_voice()
{
    sid->poke(registers->pw_lo, current_voice->pw_lo);
    sid->poke(registers->pw_hi, current_voice->pw_hi);

    sid->poke(registers->ad, current_voice->ad);
    sid->poke(registers->sr, current_voice->sr);

    uint8_t ctrl = (is_on) ? current_voice->ctrl : current_voice->ctrl | 0x01;
    sid->poke(registers->ctrl, ctrl);
}