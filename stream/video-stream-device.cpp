#include "../include/stream/video-stream-device.h"

VideoStreamDevice::VideoStreamDevice()
{

}

int VideoStreamDevice::getFrameSize() const
{
	return (width * height);
}

int VideoStreamDevice::getWidth() const
{
    return width;
}

void VideoStreamDevice::setWidth(int value)
{
    width = value;
}

int VideoStreamDevice::getHeight() const
{
    return height;
}

void VideoStreamDevice::setHeight(int value)
{
    height = value;
}

void VideoStreamDevice::getBuffer(struct videoStreamBuffer* vsb)
{
	if (vsb) {
		vsb->data = buffer;
		vsb->len = buffer_len;
	}
}

