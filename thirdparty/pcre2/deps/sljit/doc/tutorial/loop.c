#include "sljitLir.h"

#include <stdio.h>
#include <stdlib.h>

typedef sljit_sw (SLJIT_FUNC *func2_t)(sljit_sw a, sljit_sw b);

/*
  This example, we generate a function like this:

sljit_sw func(sljit_sw a, sljit_sw b)
{
	sljit_sw i;
	sljit_sw ret = 0;
	for (i = 0; i < a; ++i) {
		ret += b;
	}
	return ret;
}
*/

static int loop(sljit_sw a, sljit_sw b)
{
	void *code;
	sljit_uw len;
	func2_t func;

	struct sljit_label *loopstart;
	struct sljit_jump *out;

	/* Create a SLJIT compiler */
	struct sljit_compiler *C = sljit_create_compiler(NULL);

	/* 2 arg, 2 temp reg, 2 saved reg */
	sljit_emit_enter(C, 0, SLJIT_ARGS2(W, W, W), 2, 2, 0);

	/* R0 = 0 */
	sljit_emit_op2(C, SLJIT_XOR, SLJIT_R1, 0, SLJIT_R1, 0, SLJIT_R1, 0);
	/* RET = 0 */
	sljit_emit_op1(C, SLJIT_MOV, SLJIT_RETURN_REG, 0, SLJIT_IMM, 0);
	/* loopstart: */
	loopstart = sljit_emit_label(C);
	/* R1 >= a --> jump out */
	out = sljit_emit_cmp(C, SLJIT_GREATER_EQUAL, SLJIT_R1, 0, SLJIT_S0, 0);
	/* RET += b */
	sljit_emit_op2(C, SLJIT_ADD, SLJIT_RETURN_REG, 0, SLJIT_RETURN_REG, 0, SLJIT_S1, 0);
	/* R1 += 1 */
	sljit_emit_op2(C, SLJIT_ADD, SLJIT_R1, 0, SLJIT_R1, 0, SLJIT_IMM, 1);
	/* jump loopstart */
	sljit_set_label(sljit_emit_jump(C, SLJIT_JUMP), loopstart);
	/* out: */
	sljit_set_label(out, sljit_emit_label(C));

	/* return RET */
	sljit_emit_return(C, SLJIT_MOV, SLJIT_RETURN_REG, 0);

	/* Generate machine code */
	code = sljit_generate_code(C, 0, NULL);
	len = sljit_get_generated_code_size(C);

	/* Execute code */
	func = (func2_t)code;
	printf("func return %ld\n", (long)func(a, b));

	/* dump_code(code, len); */

	/* Clean up */
	sljit_free_compiler(C);
	sljit_free_code(code, NULL);
	return 0;
}

int main()
{
	return loop(4, 5);
}
