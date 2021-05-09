#include "pole.hpp"
#include "mtwister.hpp"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
	int periods; // Will hold periods needed to converge
	static status_bits running;
	int i, j, k;
	qtable q[N_AGENTS];
	int failures[N_AGENTS];
	// Initialize RNG
	ap_uint<32> seed = 50321;
	srand(seed);
	static hls::stream<ap_uint<32> > hls_rand_stream;
	ap_uint<32> stream_length = MAX_FAILURES * OBJECTIVE;
	mtwist_core(true, seed, stream_length, hls_rand_stream);

	for (k = 0; k < N_AGENTS; k++) {
		for (i = 0; i < N_BOXES; i++) {
			for (j = 0; j < N_ACTIONS; j++) {
				q[k][i][j] = rand() / float(RAND_MAX);
			}
		}
		failures[k] = k;
	}

	// Iterate through the action-learn loop
	printf("Calling learn()\n");
	periods = learn(hls_rand_stream, running, q, failures, 0);
	assert(periods == failures[0]);
	printf("Took %d periods to balance the pole.\n", periods);
	return 0;
}
