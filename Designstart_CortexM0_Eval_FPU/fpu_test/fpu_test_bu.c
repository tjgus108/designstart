#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "CMSDK_CM0.h"
#include "CMSDK_driver.h"
#include "fpu_test.h"
#include "soc.h"

#define LOOP_ITER 	30
#define INPUT_RANGE 100

#include "uart_stdout.h"



// --------------------------------------------------------------------
// Main FPU Application
// --------------------------------------------------------------------
void app_fpu() {
	uint32_t i;
	uint32_t hw_cycles = 0;
	uint32_t time_start = 0;
	uint32_t time_end = 0;
	float cpu_res;
	float input_a[LOOP_ITER];
	float input_b[LOOP_ITER];

	// Inputs generation
	for (i = 0; i < LOOP_ITER; i++) {
		input_a[i] = (float) rand() / (float) (RAND_MAX / INPUT_RANGE);
		input_b[i] = (float) rand() / (float) (RAND_MAX / INPUT_RANGE);
	}

	printf("\n\n#### FPU AHB vs CM0 Cycle AHB Count with Timer 0! ####");
	CMSDK_timer_Init_IntClock(CMSDK_TIMER0, 0, 0);

	//
	//	FLOATING POINT ADD
	//
	hw_cycles = fpu_op_cycle_t0(AHB_FPU_ADDR, input_a, input_b, FPU_ADD, LOOP_ITER);
	printf("\nHardware AHB FP ADD clock cycles: %d", hw_cycles);

	hw_cycles = fpu_op_cycle_t0(APB_FPU_ADDR, input_a, input_b, FPU_ADD, LOOP_ITER);
	printf("\nHardware APB FP ADD clock cycles: %d", hw_cycles);

	CMSDK_timer_SetValue(CMSDK_TIMER0, 0xFFFFFFFF);
	CMSDK_timer_StartTimer(CMSDK_TIMER0);
	time_start = CMSDK_timer_GetValue(CMSDK_TIMER0);

	for (i = 0; i < LOOP_ITER; i++)
		cpu_res = input_a[i] + input_b[i];

	time_end = CMSDK_timer_GetValue(CMSDK_TIMER0);
	CMSDK_timer_StopTimer(CMSDK_TIMER0);
	printf("\nSoftware CPU FP ADD clock cycles: %d", (time_start - time_end));

	//
	//	FLOATING POINT MULT
	//
	hw_cycles = fpu_op_cycle_t0(AHB_FPU_ADDR, input_a, input_b, FPU_MULT, LOOP_ITER);
	printf("\nHardware AHB FP MUL clock cycles: %d", hw_cycles);

	hw_cycles = fpu_op_cycle_t0(APB_FPU_ADDR, input_a, input_b, FPU_MULT, LOOP_ITER);
	printf("\nHardware APB FP MUL clock cycles: %d", hw_cycles);

	CMSDK_timer_StartTimer(CMSDK_TIMER0);
	time_start = CMSDK_timer_GetValue(CMSDK_TIMER0);

	for (i = 0; i < LOOP_ITER; i++)
		cpu_res = input_a[i] * input_b[i];

	time_end = CMSDK_timer_GetValue(CMSDK_TIMER0);
	CMSDK_timer_StopTimer(CMSDK_TIMER0);
	printf("\nSoftware CPU FP MUL clock cycles: %d", (time_start - time_end));

	//
	//	FLOATING POINT DIV
	//
	hw_cycles = fpu_op_cycle_t0(AHB_FPU_ADDR, input_a, input_b, FPU_DIV, LOOP_ITER);
	printf("\nHardware AHB FP DIV clock cycles: %d", hw_cycles);

	hw_cycles = fpu_op_cycle_t0(APB_FPU_ADDR, input_a, input_b, FPU_DIV, LOOP_ITER);
	printf("\nHardware APB FP DIV clock cycles: %d", hw_cycles);

	CMSDK_timer_StartTimer(CMSDK_TIMER0);
	time_start = CMSDK_timer_GetValue(CMSDK_TIMER0);

	for (i = 0; i < LOOP_ITER; i++)
		cpu_res = input_a[i] / input_b[i];

	time_end = CMSDK_timer_GetValue(CMSDK_TIMER0);
	CMSDK_timer_StopTimer(CMSDK_TIMER0);
	printf("\nSoftware CPU FP DIV clock cycles: %d", (time_start - time_end));
}

// --------------------------------------------------------------------
// Internal Functions
// --------------------------------------------------------------------
uint32_t fpu_op_cycle_t0(const uint32_t address, const float* a, const float* b, fpu_op_t op, int N) {
	FPU_TypeDef * fpu = ((FPU_TypeDef *) address);
	uint32_t time_start, time_end;
	uint32_t i;
	float fpu_res;

	fpu->OP = (uint32_t) op;
	fpu->RMODE = RM_NEAR;

	CMSDK_timer_Init_IntClock(CMSDK_TIMER0, 0, 0);
	CMSDK_timer_SetValue(CMSDK_TIMER0, 0xFFFFFFFF);
	CMSDK_timer_StartTimer(CMSDK_TIMER0);

	time_start = CMSDK_timer_GetValue(CMSDK_TIMER0);
	for (i = 0; i < N; i++) {
		while ((fpu->STATUS_REG & (1ul << FPU_READY_BIT)) == 0);
		fpu->DATA_A = a[i];
		fpu->DATA_B = b[i];

		fpu->STATUS_REG = (1ul << FPU_VALID_BIT);
		fpu_res = fpu->RESULT;
	}
	time_end = CMSDK_timer_GetValue(CMSDK_TIMER0);

	return (time_start - time_end);
}

// --------------------------------------------------------------------
// Usage example Function
// 		In the following usage example, we are using two static floating values as case scenario.
//		User first waits until the component is ready polling the READY bit in the status register.
//		Then, user loads the operands and selects the addition as operation.
//		Finally, the IP module is enabled and the computation starts.
// --------------------------------------------------------------------


float fpu_add(float op1, float op2){
	// Access the FPU IP on its base address
	FPU_TypeDef * fpu_ip = ((FPU_TypeDef *) AHB_FPU_ADDR); //FPU_ADDR);
	float fpu_op_res;
	printf("In the following usage example, we are using two static floating values as case scenario.\n");
	printf(" User first waits until the component is ready polling the READY bit in the status register.\n");
	printf(" Then, user loads the operands and selects the addition as operation.\n ");
	printf(" Finally, the IP module is enabled and the computation starts.\n");

	// Wait ready bit
	while ((fpu_ip->STATUS_REG & (1ul << FPU_READY_BIT)) == 0);

	//Set the input data writing to the C structure
	fpu_ip->DATA_A 	= (float) op1; // Static Value A
	fpu_ip->DATA_B 	= (float) op2; // Static Value B
	fpu_ip->RMODE 	= RM_NEAR;  // Selecting the rounding mode
	fpu_ip->OP 		= FPU_ADD; // Selecting floating point operation, addiction

	// Enable and start the module
	fpu_ip->STATUS_REG = (1ul << FPU_VALID_BIT);

	// Fetch the result without checking the ready bit
	fpu_op_res 		= fpu_ip->RESULT; // Reading the final result

	//Print the result
	printf("\nFPU IP result: %f\n", fpu_op_res);

    return fpu_op_res;
}


int main (void)
{
app_fpu();

float v1 = 4.55;
float v2 = 5.55;
float result;

result = fpu_add(v1, v2);

UartStdOutInit();
printf("\n Here is main! Operation Result is %f ~~\n",result);
UartEndSimulation();

return 0 ;
}
