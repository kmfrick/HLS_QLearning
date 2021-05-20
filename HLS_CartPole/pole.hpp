#ifndef GLOBALS_H
#define GLOBALS_H

#include <ap_int.h>
#include <hls_stream.h>

const int OBJECTIVE = 195;
const int N_ACTIONS = 2;
const int N_BOXES = 162;	// Number of disjoint boxes of state space
const int N_AGENTS = 1;	// Number of parallel learning agents

const float GAMMA = 0.999f;

const int MAX_STEPS = 100000;
const int MAX_FAILURES = 800;         // Termination criterion

// Parameters for simulation

const float GRAVITY = 9.8f;
const float MASSCART = 1.0f;
const float MASSPOLE = 0.1f;
const float TOTAL_MASS = (MASSPOLE + MASSCART);
const float LENGTH = 0.5f;	  // actually half the pole's length
const float POLEMASS_LENGTH = (MASSPOLE * LENGTH);
const float FORCE_MAG = 10.0f;
const float TAU = 0.02f;	  // seconds between state updates
const float FOURTHIRDS = 1.333333333f;

typedef float qvalue;
typedef qvalue qtable[N_BOXES][N_ACTIONS];
typedef ap_uint<1> bit;
typedef ap_uint<4> status_bits;
const status_bits STATUS_START = 0x01;
const status_bits STATUS_INIT_DONE = 0x03;
const status_bits STATUS_FIRST_FAILURE = 0x07;
const status_bits STATUS_DONE = 0x0F;

int learn(hls::stream<ap_uint<32> > &,
		volatile status_bits &, volatile qtable [N_AGENTS],
		volatile int [N_AGENTS], ap_uint<8>);

/*----------------------------------------------------------------------
 discretize:  Given the current state, returns a number from 1 to N_BOXES
 designating the region of the state space encompassing the current state.
 Returns a value of -1 if a failure state is encountered.
 ----------------------------------------------------------------------*/

const float one_degree = 0.0174532f; // 2pi/360
const float six_degrees = 0.1047192f;
const float twelve_degrees = 0.2094384f;
const float fifty_degrees = 0.87266f;
const float X_BOUND = 2.4f;

int discretize(float, float, float, float);
void update_state(int, float &, float &, float &, float &);

#endif
