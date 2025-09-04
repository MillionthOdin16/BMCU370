#pragma once

#include "main.h"

extern void Motion_control_init();
extern void Motion_control_set_PWM(uint8_t CHx, int PWM);
extern void Motion_control_run(int error);

// Automatic direction learning functions
extern void start_direction_learning(int channel, int commanded_direction);
extern void update_direction_learning(int channel, float movement_delta);
extern void complete_direction_learning(int channel);
extern bool get_direction_learning_status(int channel, float* confidence, int* samples, bool* complete);
extern void reset_direction_learning(int channel);
extern void reset_all_learned_directions();