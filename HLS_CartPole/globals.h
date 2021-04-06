#ifndef GLOBALS_H
#define GLOBALS_H

#include <ap_cint.h>
#include <math.h>

#define OBJECTIVE 	195
#define N_ACTIONS 	2
#define N_BOXES     162         // Number of disjoint boxes of state space.

#define min(x, y)               ((x <= y) ? x : y)
#define max(x, y)	        	((x >= y) ? x : y)
#define random(rng_state)       ((float)(rng_state = (a * rng_state + c) % m)  / (float)((1 << 31) - 1)) * (rng_state < 0 ? -1 : 1)
#define random_action(rng_state) (rng_state = (a * rng_state + c) % m) % N_ACTIONS  * (rng_state < 0 ? -1 : 1)

// Number of parallel learning agents
#define N_AGENTS 4

#define GAMMA 		0.999f

#define MAX_STEPS        100000
#define PHASES 7
#define FAILURES_PER_PHASE 50
#define MAX_FAILURES PHASES * FAILURES_PER_PHASE         // Termination criterion.

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
typedef uint1 bit;

void learn(volatile int *rng_state, volatile bit *running, volatile qtable q_shared[], volatile short failures[], uint8 id);

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
