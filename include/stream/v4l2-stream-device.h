/*
 * v4l2-stream-device.h
 *
 *  Created on: 02.04.2017
 *      Author: sefo
 */

#ifndef INCLUDE_V4L2_STREAM_DEVICE_H_
#define INCLUDE_V4L2_STREAM_DEVICE_H_

#include <linux/videodev2.h>
#include <iostream>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>

#include "video-stream-device.h"

class V4LStreamDevice : public VideoStreamDevice
{
public:
    V4LStreamDevice(std::string vdev = "/dev/video0", int width = 640, int height = 480);
    ~V4LStreamDevice();
    int grabOneFrame();
    int connectToDevice();
private:
    void initialize_v4l2_device(std::string dev_name, int* fd, struct v4l2_buffer* buf_info,  struct v4l2_capability* caps, char** buff, int width, int height);
    struct v4l2_capability cap;
    int fd;
    struct v4l2_buffer bufferinfo;
    struct v4l2_capability caps;
};




#endif /* INCLUDE_V4L2_STREAM_DEVICE_H_ */
