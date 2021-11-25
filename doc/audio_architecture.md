

The ingenic_videocap project has a thread in capture.c for audio.

This thread is responsible for reading audio frames using the Ingenic API.
It then puts those frames into an ALSA audio loopback provider PCM encoded.

