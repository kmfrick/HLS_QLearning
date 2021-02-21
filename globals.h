#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define min(x, y)               ((x <= y) ? x : y)
#define max(x, y)	        	((x >= y) ? x : y)
#define prob_push_right(s)      (1.0 / (1.0 + exp(-max(-50.0, min(s, 50.0)))))
#define random                  ((float)(rng_state = (a * rng_state + c) % m)  / (float)((1 << 31) - 1)) * (rng_state < 0 ? -1 : 1)

#define N_BOXES     162         /* Number of disjoint boxes of state space. */
#define ALPHA		1000        /* Learning rate for action weights, w. */
#define BETA		0.5         /* Learning rate for critic weights, v. */
#define GAMMA		0.95        /* Discount factor for critic. */
#define LAMBDAw		0.9         /* Decay rate for w eligibility trace. */
#define LAMBDAv		0.8         /* Decay rate for v eligibility trace. */

#define MAX_FAILURES     100         /* Termination criterion. */
#define MAX_STEPS        100000

typedef float vector[N_BOXES];

int rng_state;
int m, a, c; // Using glibc values for initialization

int learn(volatile vector w, volatile vector e, volatile vector xbar, volatile vector v);

/*----------------------------------------------------------------------
 cart_pole:  Takes an action (0 or 1) and the current values of the
 four state variables and updates their values by estimating the state
 TAU seconds later.
 ----------------------------------------------------------------------*/

/*** Parameters for simulation ***/

#define GRAVITY 9.8
#define MASSCART 1.0
#define MASSPOLE 0.1
#define TOTAL_MASS (MASSPOLE + MASSCART)
#define LENGTH 0.5		  /* actually half the pole's length */
#define POLEMASS_LENGTH (MASSPOLE * LENGTH)
#define FORCE_MAG 10.0
#define TAU 0.02		  /* seconds between state updates */
#define FOURTHIRDS 1.3333333333333

void cart_pole(volatile int action, volatile float *x, volatile float *x_dot, volatile float *theta, volatile float *theta_dot);

/*----------------------------------------------------------------------
 discretize:  Given the current state, returns a number from 1 to 162
 designating the region of the state space encompassing the current state.
 Returns a value of -1 if a failure state is encountered.
 ----------------------------------------------------------------------*/

#define one_degree 0.0174532	/* 2pi/360 */
#define six_degrees 0.1047192
#define twelve_degrees 0.2094384
#define fifty_degrees 0.87266

int discretize(float x, float x_dot, float theta, float theta_dot);

#endif
