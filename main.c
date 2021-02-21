#include "globals.h"

#include <time.h>

int main() {

	vector w; // vector of action weights
	vector v; // vector of critic weights
	vector e; // vector of action weight eligibilities
	vector xbar; // vector of critic weight eligibilities
	volatile int periods; // Will hold periods needed to converge
	int i;

	// Initialize RNG
	m = 1 << 31;
	a = 1103515245;
	c = 12345;
	rng_state = 54321; // Constant seed

	// Initialize action and heuristic critic weights and traces
	for (i = 0; i < N_BOXES; i++)
		w[i] = v[i] = xbar[i] = e[i] = 0.0;

	// Iterate through the action-learn loop
	periods = learn(w, e, xbar, v);
	if (periods == MAX_FAILURES) {
		printf("Pole not balanced. Stopping after %d failures.\n", MAX_FAILURES);
	} else {
		printf("Pole balanced successfully after %d periods\n", periods);
	}
}
