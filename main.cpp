#include "globals.hpp"

#include <assert.h>
#include <stdio.h>

int main() {

	vector w; // vector of action weights
	vector v; // vector of critic weights
	vector e; // vector of action weight eligibilities
	vector xbar; // vector of critic weight eligibilities
	volatile int periods; // Will hold periods needed to converge
	int i;

	// Initialize action and heuristic critic weights and traces
	for (i = 0; i < N_BOXES; i++)
		w[i] = v[i] = xbar[i] = e[i] = 0.0;

	// Iterate through the action-learn loop
	periods = learn(w, e, xbar, v, 54321);
	printf("%d\n", periods);

}
