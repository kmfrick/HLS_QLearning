/******************************************************************************
 *
 * Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * Use of the Software is limited solely to applications:
 * (a) running on a Xilinx device, or
 * (b) that interact with a Xilinx device through a bus or interconnect.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Except as contained in this notice, the name of the Xilinx shall not be used
 * in advertising or otherwise to promote the sale, use or other dealings in
 * this Software without prior written authorization from Xilinx.
 *
 ******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xlearn.h"
#include "xil_io.h"
#include <unistd.h>

#define N_ACTIONS 2
#define N_BOXES 162
#define N_AGENTS 4

typedef float qvalue;
typedef qvalue qtable[N_BOXES][N_ACTIONS];

int main() {
	int i, j;
	static qtable q_temp = { { 0.16693, 0.82352 },
				{ 0.60380, 0.73938 }, { 0.00527, 0.91625 }, { 0.03561, 0.48119 }, {
						0.48185, 0.15833 }, { 0.97689, 0.00340 },
				{ 0.98688, 0.79428 }, { 0.22181, 0.05893 }, { 0.21510, 0.55064 }, {
						0.77491, 0.89019 }, { 0.06811, 0.79147 },
				{ 0.96843, 0.13522 }, { 0.91285, 0.27476 }, { 0.69299, 0.67094 }, {
						0.80979, 0.97636 }, { 0.13255, 0.99244 },
				{ 0.64998, 0.59888 }, { 0.94168, 0.67694 }, { 0.49501, 0.81046 }, {
						0.38784, 0.77923 }, { 0.83293, 0.19460 },
				{ 0.34535, 0.73849 }, { 0.67474, 0.82311 }, { 0.89637, 0.38814 }, {
						0.44195, 0.10909 }, { 0.68256, 0.59326 },
				{ 0.69290, 0.49161 }, { 0.09064, 0.78637 }, { 0.67068, 0.77822 }, {
						0.54974, 0.59527 }, { 0.59990, 0.74944 },
				{ 0.34761, 0.62142 }, { 0.30565, 0.70947 }, { 0.41494, 0.00983 }, {
						0.68338, 0.65138 }, { 0.80152, 0.29476 },
				{ 0.06288, 0.17366 }, { 0.78465, 0.59617 }, { 0.87106, 0.58495 }, {
						0.48286, 0.93033 }, { 0.68580, 0.52997 },
				{ 0.88053, 0.31911 }, { 0.70468, 0.38093 }, { 0.70165, 0.70192 }, {
						0.40257, 0.33748 }, { 0.28631, 0.25016 },
				{ 0.57040, 0.16429 }, { 0.80881, 0.70956 }, { 0.73026, 0.22844 }, {
						0.30753, 0.83788 }, { 0.25934, 0.23632 },
				{ 0.97462, 0.60742 }, { 0.72308, 0.72550 }, { 0.39475, 0.56076 }, {
						0.41888, 0.91833 }, { 0.55480, 0.61641 },
				{ 0.33831, 0.18018 }, { 0.45061, 0.88761 }, { 0.62608, 0.47842 }, {
						0.25375, 0.80228 }, { 0.70586, 0.92487 },
				{ 0.52636, 0.04564 }, { 0.12485, 0.40130 }, { 0.91509, 0.84611 }, {
						0.69587, 0.69702 }, { 0.56596, 0.08913 },
				{ 0.39941, 0.07662 }, { 0.68770, 0.66374 }, { 0.72564, 0.22748 }, {
						0.34765, 0.24612 }, { 0.66531, 0.59128 },
				{ 0.66875, 0.30697 }, { 0.35377, 0.51638 }, { 0.65678, 0.69966 }, {
						0.91013, 0.36123 }, { 0.66945, 0.68613 },
				{ 0.69312, 0.67771 }, { 0.22500, 0.55947 }, { 0.57306, 0.81683 }, {
						0.67870, 0.52154 }, { 0.27807, 0.13410 },
				{ 0.55151, 0.38494 }, { 0.59544, 0.68927 }, { 0.76706, 0.91656 }, {
						0.63448, 0.48558 }, { 0.67956, 0.69537 },
				{ 0.20350, 0.77393 }, { 0.21355, 0.48859 }, { 0.70142, 0.68295 }, {
						0.91473, 0.18095 }, { 0.86941, 0.41947 },
				{ 0.54753, 0.68513 }, { 0.31992, 0.81967 }, { 0.03726, 0.22576 }, {
						0.65609, 0.67725 }, { 0.30333, 0.59342 },
				{ 0.05502, 0.55783 }, { 0.70055, 0.69610 }, { 0.64799, 0.04940 }, {
						0.55543, 0.10699 }, { 0.07503, 0.40492 },
				{ 0.71168, 0.69337 }, { 0.33693, 0.88708 }, { 0.39916, 0.64291 }, {
						0.40680, 0.77653 }, { 0.92737, 0.50812 },
				{ 0.72362, 0.73015 }, { 0.74184, 0.63033 }, { 0.19499, 0.29837 }, {
						0.82832, 0.82599 }, { 0.92127, 0.69806 },
				{ 0.12932, 0.62219 }, { 0.43760, 0.86742 }, { 0.64360, 0.04046 }, {
						0.21597, 0.71444 }, { 0.34446, 0.00613 },
				{ 0.07234, 0.13728 }, { 0.33228, 0.87835 }, { 0.68850, 0.69633 }, {
						0.97920, 0.52803 }, { 0.33799, 0.13452 },
				{ 0.42716, 0.72684 }, { 0.61029, 0.77823 }, { 0.06185, 0.09625 }, {
						0.86877, 0.09578 }, { 0.23823, 0.87503 },
				{ 0.97215, 0.89741 }, { 0.62756, 0.65941 }, { 0.92605, 0.03710 }, {
						0.48920, 0.24458 }, { 0.62218, 0.74240 },
				{ 0.97698, 0.41618 }, { 0.46766, 0.06744 }, { 0.85467, 0.41553 }, {
						0.85990, 0.92194 }, { 0.38600, 0.56660 },
				{ 0.55414, 0.66431 }, { 0.88044, 0.25949 }, { 0.77023, 0.22318 }, {
						0.72834, 0.74119 }, { 0.88602, 0.34907 },
				{ 0.58275, 0.81837 }, { 0.21856, 0.97523 }, { 0.98813, 0.37141 }, {
						0.61837, 0.45898 }, { 0.40316, 0.61683 },
				{ 0.91377, 0.04563 }, { 0.00920, 0.32570 }, { 0.49108, 0.62825 }, {
						0.10381, 0.69457 }, { 0.11907, 0.25434 },
				{ 0.67597, 0.76236 }, { 0.70853, 0.63887 }, { 0.41924, 0.54647 }, {
						0.02075, 0.13632 }, { 0.83512, 0.93108 },
				{ 0.45032, 0.67657 }, { -0.01265, 0.11104 }, { 0.60695, 0.45009 }, {
						0.35380, 0.39453 }, { 0.16838, 0.66728 },
				{ 0.18779, 0.65427 }};

	u32 isDone, isIdle, isReady, interruptOn, interruptEnable;
	u32 return_val;
	useconds_t sleeping_time_us = 5000000;
	init_platform();
	XLearn InstancePtr[N_AGENTS];
	u16 DeviceId = 0;
	int error;
	u32 rng_seeds[N_AGENTS] = { 50321, 54321, 12345, 11111 };
	char failures[N_AGENTS];
	u8 allAreDone;
	XLearn_LookupConfig(DeviceId);
	for (DeviceId = 0; DeviceId < N_AGENTS; DeviceId++) {
		error = XLearn_Initialize(&InstancePtr[DeviceId], DeviceId);
		if (!error) {
			printf("Init OK!\n");
		} else {
			printf("Init failed!\n");
		}
		XLearn_Set_rng_state(&InstancePtr[DeviceId], rng_seeds[DeviceId]);
		printf("RNG seed for agent %d = %d\n", DeviceId,
				XLearn_Get_rng_state(&InstancePtr[DeviceId]));

		XLearn_Write_q_shared_Bytes(&InstancePtr[DeviceId], 0, q_temp, sizeof(q_temp));
		XLearn_Write_failures_Bytes(&InstancePtr[DeviceId], 0, DeviceId, sizeof(char));
	}

	allAreDone = 0;
	while (!allAreDone) {
		allAreDone = 1;
		for (DeviceId = 0; DeviceId < N_AGENTS; DeviceId++) {
			isReady = XLearn_IsReady(&InstancePtr[DeviceId]);
			if (!isReady) {
				allAreDone = 0;
			} else {
				XLearn_InterruptGlobalEnable(&InstancePtr[DeviceId]);
			}
			printf("Agent %d ready = %d \n", DeviceId, isReady);
		}
	}

	for (DeviceId = 0; DeviceId < N_AGENTS; DeviceId++) {
		XLearn_DisableAutoRestart(&InstancePtr[DeviceId]);
		XLearn_Start(&InstancePtr[DeviceId]);
		printf("Agent %d starting\n", DeviceId);
	}

	allAreDone = 0;
	usleep(sleeping_time_us);
	XLearn_Read_q_shared_Bytes(&InstancePtr[0], 0, q_temp, sizeof(q_temp));
	while (!allAreDone) {
		allAreDone = 1;
		for (DeviceId = 0; DeviceId < N_AGENTS; DeviceId++) {
			isDone = XLearn_IsDone(&InstancePtr[DeviceId]);
			isIdle = XLearn_IsIdle(&InstancePtr[DeviceId]);
			XLearn_Read_failures_Words(&InstancePtr[DeviceId], 0, &failures[DeviceId], 1);
			interruptOn = XLearn_InterruptGetStatus(&InstancePtr[DeviceId]);
			interruptEnable = XLearn_InterruptGetEnabled(&InstancePtr[DeviceId]);
			printf("Agent %d done = %ld\n", DeviceId, isDone);
			printf("Agent %d idle = %ld\n", DeviceId, isIdle);
			printf("Agent %d failures = %ld\n", DeviceId, failures[DeviceId]);
			printf("Agent %d interrupt = %ld\n", DeviceId, interruptOn);
			printf("Agent %d interrupt EN = %ld\n", DeviceId, interruptEnable);
			return_val = XLearn_Get_return(&InstancePtr[DeviceId]);
			printf("return = %d\n", return_val);
			if (!isDone) {
				allAreDone = 0;
			}
		}
		usleep(sleeping_time_us);
	}
	/*printf("{ ");
	for (i = 0; i < N_BOXES; i++) {
		for (j = 0; j < N_ACTIONS; j++) {
			if (j == 0) {
				printf("{ ");
			}
			printf("%.3f ", q_temp[i][j]);
			if (j < N_ACTIONS - 1) {
				printf(", ");
			} else {
				printf("},\n");
			}
		}

	}
	printf("};\n");*/


	printf("Goodbye!\n");
	cleanup_platform();
	return 0;
}
