#include "pole.hpp"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifndef NO_HLS
#include "mtwister.hpp"
#endif

int main() {
	int periods; // Will hold periods needed to converge
	static status_bits running;
	int i, j, k;
	qtable q[N_AGENTS];
	// Initialize RNG
#ifdef NO_HLS
	int seed = 50321;
#else
	ap_uint<32> seed = 50321;
#endif
	srand(seed);
#ifdef NO_HLS
	int hls_rand_stream = 0;
#else
	static hls::stream<ap_uint<32> > hls_rand_stream;
	ap_uint<32> stream_length = MAX_FAILURES * OBJECTIVE;
	mtwist_core(true, seed, stream_length, hls_rand_stream);
#endif

	for (k = 0; k < N_AGENTS; k++) {
		for (i = 0; i < N_BOXES; i++) {
			for (j = 0; j < N_ACTIONS; j++) {
				q[k][i][j] = 1+(rand()%100)/100.0;
			}
		}
	}

	// Iterate through the action-learn loop
	printf("Calling learn()\n");
	periods = learn(hls_rand_stream, running, q, 0);
	printf("Took %d periods to balance the pole.\n", periods);
	return 0;
}

