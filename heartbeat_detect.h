/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** heartbeat_detect.h
** Detects heartsbeats after applying smoothing and deducting a signal
**
** Author: Jens Klein
** Version: 1.0
** -------------------------------------------------------------------------*/

#include "../Microsmooth/microsmooth.h"

#ifndef _HEARBEAT_DETECT_H
#define _HEARBEAT_DETECT_H

#define HB_HEARTBEAT_RATE_MAX 180 // max beats per minute to be valid
#define HB_HEARTBEAT_RATE_MIN 40  // min beats per minute to be valid
#define HB_HEARTBEAT_TRESHOLD -3  // threshold of steepness
#define HB_HEARTBEAT_HIST_SIZE 5  // size of the detected heartbeat timestamps
#define HB_PRECISION 3            // from 1 - (HIST_SIZE -1)


/* TYPEDEFS */
typedef void (*hb_new_value_callback) ();
typedef struct hb_heartrate{
  unsigned long time_stamp;
  float rate;
};

/* FUNCTIONS */
void hb_set_new_value_callback(hb_new_value_callback);
void hb_push_sample(uint16_t, unsigned long);
const hb_heartrate *hb_get_heartrate();

#endif