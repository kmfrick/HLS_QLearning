#include "globals.h"

int learn(int y, vector w,
		int box, vector e, vector xbar, float oldp, vector v, float x,
		float x_dot, float theta, float theta_dot, float r, float p,
		float rhat) {
	int failures;
	int i;
	int failed;
	int steps;
	steps = 0;
	failures = 0;
	/*--- Iterate through the action-learn loop. ---*/
	while (steps++ < MAX_STEPS && failures < MAX_FAILURES) {
		/*--- Choose action randomly, biased by current weight. ---*/
		y = (random < prob_push_right(w[box]));
		/*--- Update traces. ---*/
		e[box] += (1.0 - LAMBDAw) * (y - 0.5);
		xbar[box] += (1.0 - LAMBDAv);
		/*--- Remember prediction of failure for current state ---*/
		oldp = v[box];
		/*--- Apply action to the simulated cart-pole ---*/
		cart_pole(y, &x, &x_dot, &theta, &theta_dot);
		/*--- Get box of state space containing the resulting state. ---*/
		box = get_box(x, x_dot, theta, theta_dot);
		if (box < 0) {
			/*--- Failure occurred. ---*/
			failed = 1;
			failures++;
			printf("Trial %d was %d steps.\n", failures, steps);
			steps = 0;
			/*--- Reset state to (0 0 0 0).  Find the box. ---*/
			x = x_dot = theta = theta_dot = 0.0;
			box = get_box(x, x_dot, theta, theta_dot);
			/*--- Reinforcement upon failure is -1. Prediction of failure is 0. ---*/
			r = -1.0;
			p = 0.;
		} else {
			/*--- Not a failure. ---*/
			failed = 0;
			/*--- Reinforcement is 0. Prediction of failure given by v weight. ---*/
			r = 0;
			p = v[box];
		}
		/*--- Heuristic reinforcement is:   current reinforcement
		 + gamma * new failure prediction - previous failure prediction ---*/
		rhat = r + GAMMA * p - oldp;
		for (i = 0; i < N_BOXES; i++) {
			/*--- Update all weights. ---*/
			w[i] += ALPHA * rhat * e[i];
			v[i] += BETA * rhat * xbar[i];
			if (v[i] < -1.0)
				v[i] = v[i];

			if (failed) {
				/*--- If failure, zero all traces. ---*/
				e[i] = 0.;
				xbar[i] = 0.;
			} else {
				/*--- Otherwise, update (decay) the traces. ---*/
				e[i] *= LAMBDAw;
				xbar[i] *= LAMBDAv;
			}
		}
	}
	if (failures == MAX_FAILURES) {
		return -1;
	} else {
	return steps;
	}
}

void cart_pole(int action, float *x, float *x_dot, float *theta, float *theta_dot)
	 {
	float xacc, thetaacc, force, costheta, sintheta, temp;

	force = (action > 0) ? FORCE_MAG : -FORCE_MAG;
	costheta = cos(*theta);
	sintheta = sin(*theta);

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



int get_box(float x, float x_dot, float theta, float theta_dot)
	{
	int box = 0;

	if (x < -2.4 || x > 2.4 || theta < -twelve_degrees || theta > twelve_degrees)
		return (-1); /* to signal failure */

	if (x < -0.8)
		box = 0;
	else if (x < 0.8)
		box = 1;
	else
		box = 2;

	if (x_dot < -0.5)
		;
	else if (x_dot < 0.5)
		box += 3;
	else
		box += 6;

	if (theta < -six_degrees)
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
