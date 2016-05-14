/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** heartbeat_detect.cpp
** Detects heartsbeats after applying smoothing and deducting a signal
**
** Author: Jens Klein
** Version: 1.0
** -------------------------------------------------------------------------*/


#include "heartbeat_detect.h" 

// current heartrate structure 
//containing timestamp and hearrate
hb_heartrate heartrate;

// Function that gets called when heartbeat is detected
hb_new_value_callback new_hb_value_callback;

// buffer 
uint16_t *raw_sample_history = ms_init(SMA);
uint16_t prev_filtered_sample = 0;


/*
 * Sets the callback function when heartbeat was detected
 */
void hb_set_new_value_callback(hb_new_value_callback callback){
  new_hb_value_callback = callback;
}

/*
 * Function that analyses the Heartbeat and when detected, calls the
 * callback function
 * sample: deducted signal from sensor
 * timestamp: Pointer to the timestamp value matching the sample
 */
void analyze_heartbeat(int8_t sample, unsigned long timestamp){

  // holds the time of the last heartbeats
  static unsigned long beats[HB_HEARTBEAT_HIST_SIZE];
  static int8_t prev_sample;

  // Two states: 
  // (1) Listening for a certain steepness in the signal (defined by the threshold)
  // (2) Listening for the zerocrossing, giving the maximum turning
  // point of the signal.
  static bool listen_for_zerocrossing;

  if(sample <= HB_HEARTBEAT_TRESHOLD && !listen_for_zerocrossing){
    listen_for_zerocrossing = true;
    return;
  }

  if(listen_for_zerocrossing && prev_sample < 0 && sample >= 0){

    listen_for_zerocrossing = false;
    uint8_t i = 0;

    for(i = HB_HEARTBEAT_HIST_SIZE-1; i > 0; i--){
      beats[i] = beats[i-1];
    }
    beats[0] =  timestamp;

    uint8_t valid_rates = 0;
    float sum = 0;

    for(i = 0; i < HB_HEARTBEAT_HIST_SIZE-1 ; i++){
      float rate = (1.0 / (beats[i] - beats[i+1]) * 1000) * 60;
      if(rate < HB_HEARTBEAT_RATE_MAX && rate > HB_HEARTBEAT_RATE_MIN){
        sum += rate;
        valid_rates++;
      }      
    }
    
    // If found more valid heartrates then the defined
    // precision, a heartrate is outputted
    if(valid_rates > HB_PRECISION){
      heartrate.rate = sum/valid_rates;
      heartrate.time_stamp = timestamp;
      new_hb_value_callback ();
    }
  } 

  // saving the previous sample
  prev_sample = sample;
}

/*
 * Gets called when a new sample of the signal was read
 * sample: sampled signal at constant rate
 */
void hb_push_sample(uint16_t sample, unsigned long timestamp) { 
  
  static bool isFirstRead = true;

  if( !isFirstRead ) { 
    sample = sma_filter(sample, raw_sample_history);

    // send out to analyze heartbeat
    analyze_heartbeat( sample - prev_filtered_sample, timestamp );    
  }
  isFirstRead = false;
  
  sample - prev_filtered_sample;

  // save previous filtered sample
  prev_filtered_sample = sample;
}

/*
 * Getter function that is callable outside the library
 * Can be called when new_value_callback was called.
 * Returns the pointer to the last detected heartbeat struct
 */
const hb_heartrate *hb_get_heartrate(){
  return &heartrate;
}