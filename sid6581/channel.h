#ifndef CHANNEL_H
#define CHANNEL_H

#include "sid6581.h"
#include "midi.h"

struct VoiceRegisters {
    uint8_t ctrl;
    uint8_t note_lo;
    uint8_t note_hi;
    uint8_t pw_lo;
    uint8_t pw_hi;
    uint8_t ad;
    uint8_t sr;
};

struct VoiceProfile {
    uint8_t ctrl;
    uint8_t pw_lo;
    uint8_t pw_hi;
    uint8_t ad;
    uint8_t sr;
};

// so i don't actually need this?
// voiceregisters should be a member of channel
struct Voice {
    VoiceRegisters *reg;
    VoiceProfile *profile;
};

class Channel {
    public:

        Channel(Sid6581 *sid_, VoiceRegisters *registers_, VoiceProfile (&voices_)[3])
            : sid(sid_),
            registers(registers_),
            voices(voices_),
            current_voice(&voices_[0]) 
        {
            reset_voice();
        }

        void handle_message(const MidiMessage& message);

        void reset_voice();

    private:
        // this array will be in static storage and there
        // will only ever be one per channel
        VoiceProfile (&voices)[3];
        VoiceProfile *current_voice;

        VoiceRegisters *registers;

        const Sid6581 *sid;

        uint8_t last_note;

        bool is_on;
};

#endif // CHANNEL_H