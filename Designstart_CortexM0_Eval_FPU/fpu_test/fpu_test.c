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

float fpu_add(float op1, float op2){
	// Access the FPU IP on its base address
	FPU_TypeDef * fpu_ip = ((FPU_TypeDef *) APB_FPU_ADDR); //FPU_ADDR);
	float fpu_op_res;
	//printf("In the following usage example, we are using two static floating values as case scenario.\n");
	//printf(" User first waits until the component is ready polling the READY bit in the status register.\n");
	//printf(" Then, user loads the operands and selects the addition as operation.\n ");
	//printf(" Finally, the IP module is enabled and the computation starts.\n");

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
	//printf("\nFPU IP result: %f\n", fpu_op_res);

    return fpu_op_res;
}


int main (void)
{
UartStdOutInit();
//app_fpu();

float v1 = 4.55;
float v2 = 5.55;
float result;

result = fpu_add(v1, v2);

printf("\n Here is main! Operation Result is %f ~~\n",result);
UartEndSimulation();

return 0 ;
}
