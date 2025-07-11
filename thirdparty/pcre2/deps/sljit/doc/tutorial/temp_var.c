#include "sljitLir.h"

#include <stdio.h>
#include <stdlib.h>

typedef sljit_sw (SLJIT_FUNC *func3_t)(sljit_sw a, sljit_sw b, sljit_sw c);

static sljit_sw SLJIT_FUNC print_arr(sljit_sw *a, sljit_sw n)
{
	sljit_sw i;
	sljit_sw sum = 0;
	for (i = 0; i < n; ++i) {
		sum += a[i];
		printf("arr[%ld] = %ld\n", (long)i, (long)a[i]);
	}
	return sum;
}

/*
  This example, we generate a function like this:

sljit_sw func(sljit_sw a, sljit_sw b, sljit_sw c)
{
	sljit_sw arr[3] = { a, b, c };
	return print_arr(arr, 3);
}
*/

static int temp_var(sljit_sw a, sljit_sw b, sljit_sw c)
{
	void *code;
	sljit_uw len;
	func3_t func;

	/* Create a SLJIT compiler */
	struct sljit_compiler *C = sljit_create_compiler(NULL);

	/* reserved space in stack for sljit_sw arr[3] */
	sljit_emit_enter(C, 0, SLJIT_ARGS3(W, W, W, W), 2, 3, 3 * sizeof(sljit_sw));
	/*                  opt arg                     R  S  local_size */

	/* arr[0] = S0, SLJIT_SP is the init address of local var */
	sljit_emit_op1(C, SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), 0, SLJIT_S0, 0);
	/* arr[1] = S1 */
	sljit_emit_op1(C, SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), 1 * sizeof(sljit_sw), SLJIT_S1, 0);
	/* arr[2] = S2 */
	sljit_emit_op1(C, SLJIT_MOV, SLJIT_MEM1(SLJIT_SP), 2 * sizeof(sljit_sw), SLJIT_S2, 0);

	/* R0 = arr; in fact SLJIT_SP is the address of arr, but can't do so in SLJIT */
	sljit_get_local_base(C, SLJIT_R0, 0, 0);	/* get the address of local variables */
	sljit_emit_op1(C, SLJIT_MOV, SLJIT_R1, 0, SLJIT_IMM, 3);	/* R1 = 3; */
	sljit_emit_icall(C, SLJIT_CALL, SLJIT_ARGS2(W, P, W), SLJIT_IMM, SLJIT_FUNC_ADDR(print_arr));
	sljit_emit_return(C, SLJIT_MOV, SLJIT_R0, 0);

	/* Generate machine code */
	code = sljit_generate_code(C, 0, NULL);
	len = sljit_get_generated_code_size(C);

	/* Execute code */
	func = (func3_t)code;
	printf("func return %ld\n", (long)func(a, b, c));

	/* dump_code(code, len); */

	/* Clean up */
	sljit_free_compiler(C);
	sljit_free_code(code, NULL);
	return 0;
}

int main()
{
	return temp_var(7, 8, 9);
}
