#include "globals.h"

#include <assert.h>
#include <stdio.h>

int main() {
	volatile short periods; // Will hold periods needed to converge
	volatile led_array phase;
	volatile bit running;
	int i;
	// Iterate through the action-learn loop
	periods = learn(&running, &phase);
	printf("Took %d periods to balance the pole.\n", periods);
	return 0;
}
