

# Local Recording

Local video recording is done using ffmpeg.







ffmpeg -nostats -nostdin -rtsp_transport tcp -i rtsp://openmiko:password@127.0.0.1:8554/video4/unicast \
-c:v copy -f segment -segment_time 600 -segment_format mp4 -segment_wrap 20 -reset_timestamps 1 -strftime 0 \
-movflags frag_keyframe \
-c copy -map 0 segment-%04d.mp4 > /dev/null 2>&1 &

