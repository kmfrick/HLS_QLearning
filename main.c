#include "globals.h"

#include <time.h>

int main() {

	vector w, /* vector of action weights */
	v, /* vector of critic weights */
	e, /* vector of action weight eligibilities */
	xbar; /* vector of critic weight eligibilities */
	int i;
	volatile int periods;

	// Initialize RNG
	m = 1 << 31;
	a = 1103515245;
	c = 12345;
	rng_state = 12345; // Constant seed

	/*--- Initialize action and heuristic critic weights and traces. ---*/
	for (i = 0; i < N_BOXES; i++)
		w[i] = v[i] = xbar[i] = e[i] = 0.0;


	/*--- Iterate through the action-learn loop. ---*/
	periods = learn(w, e, xbar, v);
	if (periods == MAX_FAILURES)
		printf("Pole not balanced. Stopping after %d failures.\n", MAX_FAILURES);
	else
		printf("Pole balanced successfully for at least %d steps\n", periods);
}
