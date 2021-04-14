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
#include <stdlib.h>
#include "platform.h"
#include "xlearn.h"
#include "xil_io.h"
#include <unistd.h>

#define N_ACTIONS 2
#define N_BOXES 162
#define N_AGENTS 2

int main() {
	int i, j, k;
	u32 is_done, return_val, all_done;
	u16 device_id = 0;
	XLearn instance_ptr[N_AGENTS];
	int error;
	int failures;
	useconds_t sleeping_time_us = 5000000;

	init_platform();

	srand(50321);
	for (device_id = 0; device_id < N_AGENTS; device_id++) {
		XLearn_LookupConfig(device_id);
		error = XLearn_Initialize(&instance_ptr[device_id], device_id);
		if (!error) {
			printf("\nInit %d OK! Base addr = %p\n", device_id,
					instance_ptr[device_id].Axilites_BaseAddress);
		} else {
			printf("\nInit %d failed!\n", device_id);
		}
	}

	int zero = 0;
	for (k = 0; k < N_AGENTS; k++) {
		XLearn_Write_failures_Bytes(&instance_ptr[k], 0, &zero,
								sizeof(int));
		for (i = 0; i < N_BOXES; i++) {
			for (j = 0; j < N_ACTIONS; j++) {
				float r = (rand() % 100) / 100.0f;
				if (i == 0 && j == 0) {
					printf("q%d00 = %.3f\n", k, r);
				}
					XLearn_Write_q_shared_Words(&instance_ptr[k],
						k * N_ACTIONS * N_BOXES + i * N_ACTIONS + j,
						(int*) (&r), 1);
			}
		}
	}
	printf("\nfailures, q initialized\n");

	for (device_id = 0; device_id < N_AGENTS; device_id++) {
		XLearn_Start(&instance_ptr[device_id]);
		printf("Agent %d starting\n", device_id);
	}
	all_done = 0;
	while (!all_done) {
		usleep(sleeping_time_us);
		all_done = 1;
		for (device_id = 0; device_id < N_AGENTS; device_id++) {
			is_done = XLearn_IsDone(&instance_ptr[device_id]);
			if (!is_done) {
				all_done = 0;
			}
			printf("Agent %d done = %lu\n", device_id, is_done);
			//is_done  = XLearn_ReadReg(instance_ptr->Axilites_BaseAddress, XLEARN_AXILITES_ADDR_AP_CTRL);
			//printf("AP_CTRL = %lu\n", is_done);
			float r;
			XLearn_Read_q_shared_Words(&instance_ptr[device_id], 0, &r, 1);
			printf("q%d00 = %.3f\n", device_id, r);
		}
	}
	printf("\nPrinting return values\n");
	for (device_id = 0; device_id < N_AGENTS; device_id++) {
		return_val = XLearn_Get_return(&instance_ptr[device_id]);
		printf("return = %lu\n", return_val);
	}
	printf("\nPrinting periods\n");
	for (device_id = 0; device_id < N_AGENTS; device_id++) {
		for (i = 0; i < N_AGENTS; i++) {
			XLearn_Read_failures_Bytes(&instance_ptr[device_id],
					i * sizeof(int), &failures, sizeof(int));
			printf("Agent %d failures = %d\n", i, failures);
		}
	}

	printf("\nGoodbye!\n");
	cleanup_platform();
	return 0;
}
