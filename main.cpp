#include "globals.hpp"

#include <assert.h>
#include <stdio.h>

int main() {

	satable q; // State-action
	volatile int periods; // Will hold periods needed to converge

	// Iterate through the action-learn loop
	periods = learn(50321);
	printf("Took %d periods to balance the pole.\n", periods);

}
