/*----------------------------------------------------------------------
 This file contains routines written by Rich Sutton and Chuck Anderson.
 Claude Sammut translated parts from Fortran to C.
 ----------------------------------------------------------------------*/
#include "hls_math.h"
#include "globals.hpp"

int argmax(volatile float q[N_ACTIONS]) {
	float max = q[0];
	int pos_max = 0;
	int i;
	for (i = 0; i < N_ACTIONS; i++) {
		if (q[i] > max) {
			max = q[i];
			pos_max = i;
		}
	}
	return pos_max;
}

int learn(volatile satable q, volatile int seed) {
#pragma HLS INTERFACE ap_memory port=q
	float p, oldp, rhat, r;
	int failures;

	int i;
	int failed;
	int steps, last_steps;
	float mov_avg; // Moving average of steps, used to calculate solvability
	int y;
	int box;
	int old_state;
	float x; 			// cart position, meters
	float x_dot;		// cart velocity
	float theta;		// pole angle, radians
	float theta_dot;	// pole angular velocity

	int rng_state;
	int m, a, c;
	// Initialize RNG
	// Using glibc values for initialization
	m = 1 << 31;
	a = 1103515245;
	c = 12345;
	rng_state = seed;
	// Starting state is (0 0 0 0)
	x = x_dot = theta = theta_dot = 0.0;

	// Find box in state space containing start state
	box = discretize(x, x_dot, theta, theta_dot);

	steps = mov_avg = 0;
	failures = 0;
	// Iterate through the action-learn loop.
	while (steps++ < MAX_STEPS && failures < MAX_FAILURES) {
		// Experiment with probability EPS
		if (random < EPS) {
			y = random < 0.5;
		} else {
			y = argmax(q[box]);
		}
		//printf("Choosing action %d\n", y);
		// Apply action to the simulated cart-pole
		cart_pole(y, &x, &x_dot, &theta, &theta_dot);
		// Get box of state space containing the resulting state.
		old_state = box;
		box = discretize(x, x_dot, theta, theta_dot);
		if (box < 0) {
			// Failure occurred.
			failed = 1;
			failures++;
			printf("Trial %d was %d steps.\n", failures, steps);
			printf("Moving average for trial %d was %.2f steps.\n", failures, mov_avg);

			mov_avg = mov_avg + (steps - mov_avg) / (failures > 100 ? float(failures) : 100.0);
			if (mov_avg > OBJECTIVE) {
				return failures;
			}

			steps = 0;

			// Reinforcement upon failure is -1
			q[old_state][y] += ALPHA * (-1 + GAMMA * q[box][argmax(q[box])] - q[old_state][y]);
			// Reset state to (0 0 0 0).  Find the box.
			x = x_dot = theta = theta_dot = 0.0;
			box = discretize(x, x_dot, theta, theta_dot);

		} else {
			// Not a failure.
			failed = 0;
			// Reinforcement is 0
			q[old_state][y] += ALPHA * (0 + GAMMA * q[box][argmax(q[box])] - q[old_state][y]);
		}

	}
	return failures;
}

void cart_pole(volatile int action, volatile float *x, volatile float *x_dot, volatile float *theta, volatile float *theta_dot) {
#pragma HLS INTERFACE ap_ctrl_hs port=return
	float xacc, thetaacc, force, costheta, sintheta, temp;

	force = (action > 0) ? FORCE_MAG : -FORCE_MAG;
	costheta = hls::cos(*theta);
	sintheta = hls::sin(*theta);

	temp = (force + POLEMASS_LENGTH * *theta_dot * *theta_dot * sintheta)
			/ TOTAL_MASS;

	thetaacc =
			(GRAVITY * sintheta - costheta * temp)
					/ (LENGTH
							* (FOURTHIRDS
									- MASSPOLE * costheta * costheta
											/ TOTAL_MASS));

	xacc = temp - POLEMASS_LENGTH * thetaacc * costheta / TOTAL_MASS;

	/*** Update the four state variables, using Euler's method. ***/

	*x += TAU * *x_dot;
	*x_dot += TAU * xacc;
	*theta += TAU * *theta_dot;
	*theta_dot += TAU * thetaacc;
}

int discretize(float x, float x_dot, float theta, float theta_dot) {
#pragma HLS INTERFACE ap_ctrl_hs port=return
	int box = 0;

	if (x < -2.4 || x > 2.4 || theta < -twelve_degrees || theta > twelve_degrees)
		return (-1); // to signal failure

	if (x < -0.8)
		box = 0;
	else if (x < 0.8)
		box = 1;
	else
		box = 2;

	if (x_dot < -0.5)
		// noop
		;
	else if (x_dot < 0.5)
		box += 3;
	else
		box += 6;

	if (theta < -six_degrees)
		// noop
		;
	else if (theta < -one_degree)
		box += 9;
	else if (theta < 0)
		box += 18;
	else if (theta < one_degree)
		box += 27;
	else if (theta < six_degrees)
		box += 36;
	else
		box += 45;

	if (theta_dot < -fifty_degrees)
		;
	else if (theta_dot < fifty_degrees)
		box += 54;
	else
		box += 108;

	return (box);
}
