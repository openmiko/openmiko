
# Video Settings JSON


A group can consist of 1 or more channels.


- One Group only supports one resolution, different resolutions need to start a new Group. A Group supports both H264 and JPEG captures
- If the specified Group already exists, return failure


Set rate control properties


Calling this API will set the rate control properties of the channel, and the next IDR will take effect. Calling this API requires that the channel already exists.


Currently, rate control supports ENC_RC_MODE_H264FIXQP and ENC_RC_MODE_H264CBR. ENC_RC_MODE_H264FIXQP supports the configuration of qp, and ENC_RC_MODE_H264CBR supports the configuration of outBitRate.
This function has no meaning for JPEG channels.
