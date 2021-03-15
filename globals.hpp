#ifndef GLOBALS_H
#define GLOBALS_H

#define min(x, y)               ((x <= y) ? x : y)
#define max(x, y)	        	((x >= y) ? x : y)
#define random                  ((float)(rng_state = (a * rng_state + c) % m)  / (float)((1 << 31) - 1)) * (rng_state < 0 ? -1 : 1)
#define coinflip 				(rng_state = (a * rng_state + c) % m) % 2  * (rng_state < 0 ? -1 : 1)

#define OBJECTIVE 	195
#define N_ACTIONS 	2
#define N_BOXES     162         // Number of disjoint boxes of state space.
#define ALPHA_MIN	0.1
#define EPS_START	0.9
#define EPS_END		0.05
#define EPS_DECAY	200.0
#define ALPHA		max(ALPHA_MIN, min(1.0, 1.0 - hls::log10((failures + 1.0) / 25.0))) // Learning rate for the Q-table
#define EPS 		EPS_END + (EPS_START - EPS_END) * hls::exp(-1. * failures / EPS_DECAY) // Experimentation threshold
#define GAMMA 		0.999

#define MAX_FAILURES     800         // Termination criterion.
#define MAX_STEPS        100000

typedef float satable[N_BOXES][N_ACTIONS];

int learn(volatile satable q, volatile int seed);

/*----------------------------------------------------------------------
 cart_pole:  Takes an action (0 or 1) and the current values of the
 four state variables and updates their values by estimating the state
 TAU seconds later.
 ----------------------------------------------------------------------*/

// Parameters for simulation

#define GRAVITY 9.8
#define MASSCART 1.0
#define MASSPOLE 0.1
#define TOTAL_MASS (MASSPOLE + MASSCART)
#define LENGTH 0.5		  // actually half the pole's length
#define POLEMASS_LENGTH (MASSPOLE * LENGTH)
#define FORCE_MAG 10.0
#define TAU 0.02		  // seconds between state updates
#define FOURTHIRDS 1.3333333333333

void cart_pole(volatile int action, volatile float *x, volatile float *x_dot, volatile float *theta, volatile float *theta_dot);

/*----------------------------------------------------------------------
 discretize:  Given the current state, returns a number from 1 to 162
 designating the region of the state space encompassing the current state.
 Returns a value of -1 if a failure state is encountered.
 ----------------------------------------------------------------------*/

#define one_degree 0.0174532	// 2pi/360
#define six_degrees 0.1047192
#define twelve_degrees 0.2094384
#define fifty_degrees 0.87266

int discretize(float x, float x_dot, float theta, float theta_dot);

#endif
