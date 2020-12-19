
# Release 0.0.6

A new `videocapture_settings.json` format is now in use. If you have an older one replace the one you have in `/config/overlay/etc` with the one here: https://raw.githubusercontent.com/openmiko/openmiko/master/overlay_minimal/etc/videocapture_settings.json

This new settings file has sections for frame sources, encoders, and bindings. The configuration by default provides 3 output streams:
- 1920x1080 H264
- 1920x1080 JPG
- 640x360 H264

There are some jagged edges on the 640x360 stream that I haven't been able to figure out yet.

The streams can be accessed using the following URLs:

- rtps://IP:8554/video3/unicast
- rtps://IP:8554/video5/unicast
- http://IP:8080/?action=stream
- http://IP:8080/?action=snapshot
