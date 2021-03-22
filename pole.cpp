/*----------------------------------------------------------------------
 This file contains routines written by Rich Sutton and Chuck Anderson.
 Claude Sammut translated parts from Fortran to C.
 ----------------------------------------------------------------------*/
#include "hls_math.h"
#include "globals.hpp"

short learn(volatile short seed) {
	float p, oldp, rhat, r;
	int failures;
	satable q;

	int i, j;
	int failed;
	int steps, last_steps;
	int action;
	int new_state;
	int state;
	float q_max;		// used to argmax over actions
	float x; 			// cart position, meters
	float x_dot;		// cart velocity
	float theta;		// pole angle, radians
	float theta_dot;	// pole angular velocity
	float xacc, thetaacc, force, costheta, sintheta, temp;

	int rng_state;
	int m, a, c;

	// Initialize RNG
	// Using glibc values for initialization
	m = 1 << 31;
	a = 1103515245;
	c = 12345;
	rng_state = seed;

	// Initialize Q-table
	for (i = 0; i < N_BOXES; i++) {
		for (j = 0; j < N_ACTIONS; j++) {
			q[i][j] = random;
		}
	}
	// Starting state is (0 0 0 0)
	x = x_dot = theta = theta_dot = 0.0;

	// Find box in state space containing start state
	new_state = discretize(x, x_dot, theta, theta_dot);

	steps = 0;
	failures = 0;
	// Iterate through the action-learn loop.
	while (steps++ < MAX_STEPS && failures < MAX_FAILURES) {
		// Experiment with probability EPS
		if (random <= EPS) {
			action = coinflip;
		} else {
			// Begin action argmax
			action = 0;
			q_max = q[new_state][0];
			for (i = 0; i < N_ACTIONS; i++) {
				if (q[new_state][i] > q_max) {
					q_max = q[new_state][i];
					action = i;
				}
			}
			// End action argmax
		}

		// Begin cart-pole simulation
		// Takes an action (0 or 1) and the current values of the
		// four state variables and updates their values by estimating the state
		// TAU seconds later.

		force = (action > 0) ? FORCE_MAG : -FORCE_MAG;
		costheta = hls::cos(theta);
		sintheta = hls::sin(theta);

		temp = (force + POLEMASS_LENGTH * theta_dot * theta_dot * sintheta)
				/ TOTAL_MASS;

		thetaacc = (GRAVITY * sintheta - costheta * temp)
				/ (LENGTH
						* (FOURTHIRDS
								- MASSPOLE * costheta * costheta / TOTAL_MASS));

		xacc = temp - POLEMASS_LENGTH * thetaacc * costheta / TOTAL_MASS;

		// Update the four state variables, using Euler's method.

		x += TAU * x_dot;
		x_dot += TAU * xacc;
		theta += TAU * theta_dot;
		theta_dot += TAU * thetaacc;
		// End cart-pole simulation

		// Get box of state space containing the resulting state.
		state = new_state;
		new_state = discretize(x, x_dot, theta, theta_dot);
		if (new_state < 0) {
			// Failure occurred.
			failed = 1;
			failures++;
			//printf("Trial %d was %d steps.\n", failures, steps);
			//printf("EPS = %.3f, ALPHA = %.3f\n", EPS, ALPHA);

			// Stop learning if the objective is reached
			if (steps > OBJECTIVE) {
				return failures;
			}
			// Reset state to (0 0 0 0).  Find the box.
			x = x_dot = theta = theta_dot = 0.0;
			new_state = discretize(x, x_dot, theta, theta_dot);
			steps = 0;

			// Begin argmax
			q_max = q[new_state][0];
			for (i = 0; i < N_ACTIONS; i++) {
				if (q[new_state][i] > q_max) {
					q_max = q[new_state][i];
				}
			}
			// End argmax
			// Reinforcement upon failure is -1
			q[state][action] += ALPHA * (-1 + GAMMA * q_max - q[state][action]);

		} else {
			// Not a failure.
			failed = 0;
			// Begin argmax
			q_max = q[new_state][0];
			for (i = 0; i < N_ACTIONS; i++) {
				if (q[new_state][i] > q_max) {
					q_max = q[new_state][i];
				}
			}
			// End argmax
			// Reinforcement is 0
			q[state][action] += ALPHA * (0 + GAMMA * q_max - q[state][action]);
		}
	}
	return failures;
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
