/* capture.h - BR_CAPTURE: the pictures to a lossless video (capture.cpp) */
#pragma once

#include <stdint.h>

void capture_init(void);                                            /* after the options are read */
void capture_frame(const uint32_t *fb, int stride, int w, int h);   /* each runner frame's picture */
