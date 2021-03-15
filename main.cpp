#include "globals.hpp"

#include <assert.h>
#include <stdio.h>

int main() {

	satable q; // State-action
	volatile int periods; // Will hold periods needed to converge
	int i, j;

	int rng_state;
	int m, a, c;
	// Initialize RNG
	// Using glibc values for initialization
	m = 1 << 31;
	a = 1103515245;
	c = 12345;
	rng_state = 250917;

	// Initialize Q-table
	for (i = 0; i < N_BOXES; i++) {
		for (j = 0; j < N_ACTIONS; j++) {
			q[i][j] = random;
		}
	}

	// Iterate through the action-learn loop
	periods = learn(q, 50321);
	printf("Took %d periods to balance the pole.\n", periods);

}
