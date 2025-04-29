# sid-synth

This repository holds the code for a sid6581-based synthesizer I made several years ago.
The project has since perished on the healing bench. It may one day be resurrected, but for now
I've uploaded the code for reference and posterity.

## Brief overview

The synthesizer was capable of bitbanging the sid6581 and driving all 3 of its voices. Some
features such as ring modulation went unsupported in the software.

I ran the code in the `sid6581` directory on an Arduino MKR1000. One of the serial interfaces
on the arduino was connected to a MIDI circuit, and a GPIO pin was connected to two shift registers
that served as a serial-to-parallel buffer between the arduino and the sid6581.

The `aldats` directory appears to be the beginning of an interpreter for a text music specification
file format? I assume my intention was, at some point, to play `.a` files on the synth, but I
have zero recollection of writing it. It's included more for posterity than reference.

There is much I would do differently were I to rewrite this project. One big thing that comes to mind would
be to compute note frequencies rather than store a huge table in the very limited available memory; as
I found out while writing the FM synth for Three's a Chorus there's a simple formula for that.
