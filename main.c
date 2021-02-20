#include "globals.h"

#include <time.h>

int main() {
	float x, /* cart position, meters */
	x_dot, /* cart velocity */
	theta, /* pole angle, radians */
	theta_dot; /* pole angular velocity */
	vector w, /* vector of action weights */
	v, /* vector of critic weights */
	e, /* vector of action weight eligibilities */
	xbar; /* vector of critic weight eligibilities */
	float p, oldp, rhat, r;
	int box, i, y;
	int steps;

	m = 1 << 31;
	a = 1103515245;
	c = 12345;
	rng_state = time(NULL); 

	/*--- Initialize action and heuristic critic weights and traces. ---*/
	for (i = 0; i < N_BOXES; i++)
		w[i] = v[i] = xbar[i] = e[i] = 0.0;

	/*--- Starting state is (0 0 0 0) ---*/
	x = x_dot = theta = theta_dot = 0.0;

	/*--- Find box in state space containing start state ---*/
	box = get_box(x, x_dot, theta, theta_dot);

	/*--- Iterate through the action-learn loop. ---*/
	steps = learn(y, w, box, e, xbar, oldp, v, x,
			x_dot, theta, theta_dot, r, p, rhat);
	if (steps < 0)
		printf("Pole not balanced. Stopping after %d failures.\n", MAX_FAILURES);
	else
		printf("Pole balanced successfully for at least %d steps\n", steps);
}
