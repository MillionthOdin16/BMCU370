#pragma once

#include "main.h"

extern void Motion_control_init();
extern void Motion_control_set_PWM(uint8_t CHx, int PWM);
extern void Motion_control_run(int error);

// Adaptive pressure control functions
extern void pressure_sensor_calibrate_channel(int channel);
extern void pressure_sensor_auto_calibrate();
extern void pressure_sensor_reset_calibration(int channel);
extern float get_dynamic_pressure_threshold_high(int channel);
extern float get_dynamic_pressure_threshold_low(int channel);
extern bool is_pressure_in_deadband(int channel, float pressure);

// Automatic direction learning functions
extern void start_direction_learning(int channel, int commanded_direction);
extern void update_direction_learning(int channel, float movement_delta);
extern void complete_direction_learning(int channel);
extern bool get_direction_learning_status(int channel, float* confidence, int* samples, bool* complete);
extern void reset_direction_learning(int channel);
extern void reset_all_learned_directions();

// Loading direction detection functions
extern void start_loading_direction_detection(int channel);
extern void update_loading_direction_detection(int channel);
extern void complete_loading_direction_detection(int channel);