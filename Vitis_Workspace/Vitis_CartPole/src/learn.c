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

#define N_AGENTS 4

int main() {
	u32 isDone, isIdle;
	u32 return_val;
	useconds_t sleeping_time_us = 5000000;
	init_platform();
	XLearn InstancePtr[N_AGENTS];
	u16 DeviceId = 0;
	int error;
	u32 rng_seeds[N_AGENTS] = { 50321, 54321, 12345, 11111 };
	XLearn_LookupConfig(DeviceId);
	for (DeviceId = 0; DeviceId < N_AGENTS; DeviceId++) {
		error = XLearn_Initialize(&InstancePtr[DeviceId], DeviceId);
		if (!error) {
			printf("Init OK!\n");
		} else {
			printf("Init failed!\n");
		}
		XLearn_Set_rng_state(&InstancePtr[DeviceId], rng_seeds[DeviceId]);
		printf("RNG seed for agent %ud = %ud", DeviceId,
				XLearn_Get_rng_state(&InstancePtr[DeviceId]));
	}

	for (DeviceId = 0; DeviceId < N_AGENTS; DeviceId++) {
		XLearn_Start(&InstancePtr[DeviceId]);
		printf("Agent %d starting\n", DeviceId);
	}

	while (!XLearn_IsDone(&InstancePtr)) {
		for (DeviceId = 0; DeviceId < N_AGENTS; DeviceId++) {
			isDone = XLearn_IsDone(&InstancePtr[DeviceId]);
			isIdle = XLearn_IsIdle(&InstancePtr[DeviceId]);
			printf("Agent %ud done = %ld\n", isDone);
			printf("Agent %ud idle = %ld\n", isIdle);
		}
		usleep(sleeping_time_us);
	}
	for (DeviceId = 0; DeviceId < N_AGENTS; DeviceId++) {
		return_val = XLearn_Get_return(&InstancePtr[DeviceId]);
		printf("return = %d\n", return_val);
	}

	printf("Goodbye!\n");
	cleanup_platform();
	return 0;
}
