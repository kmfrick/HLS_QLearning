#ifndef GLOBALS_H
#define GLOBALS_H

#include <ap_int.h>

#define OBJECTIVE 	195
#define N_ACTIONS 	2
#define N_BOXES     162         // Number of disjoint boxes of state space.

// Number of parallel learning agents
#define N_AGENTS 4

#define GAMMA 		0.999f

#define MAX_STEPS  100000
#define MAX_FAILURES 400         // Termination criterion.

// Parameters for simulation

#define GRAVITY 9.8f
#define MASSCART 1.0f
#define MASSPOLE 0.1f
#define TOTAL_MASS (MASSPOLE + MASSCART)
#define LENGTH 0.5f	  // actually half the pole's length
#define POLEMASS_LENGTH (MASSPOLE * LENGTH)
#define FORCE_MAG 10.0f
#define TAU 0.02f		  // seconds between state updates
#define FOURTHIRDS 1.333333333f

typedef float qvalue;
typedef qvalue qtable[N_BOXES][N_ACTIONS];
typedef ap_uint<1> bit;
typedef ap_uint<2> twobits;

short learn(volatile int *rng_state, volatile twobits *running, volatile qtable q_shared[], volatile short failures[], ap_uint<8> id);

/*----------------------------------------------------------------------
 discretize:  Given the current state, returns a number from 1 to 162
 designating the region of the state space encompassing the current state.
 Returns a value of -1 if a failure state is encountered.
 ----------------------------------------------------------------------*/

#define one_degree 		0.0174532f	// 2pi/360
#define six_degrees 	0.1047192f
#define twelve_degrees 	0.2094384f
#define fifty_degrees 	0.87266f
#define X_BOUND  2.4f

int discretize(float x, float x_dot, float theta, float theta_dot);

#endif
