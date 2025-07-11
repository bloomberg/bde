#include "sljitLir.h"

#include <stdio.h>
#include <stdlib.h>

struct point_st {
	sljit_sw x;
	sljit_s32 y;
	sljit_s16 z;
	sljit_s8 d;
};

typedef sljit_sw (SLJIT_FUNC *point_func_t)(struct point_st *point);

static sljit_sw SLJIT_FUNC print_num(sljit_sw a)
{
	printf("a = %ld\n", (long)a);
	return a + 1;
}

/*
  This example, we generate a function like this:

sljit_sw func(struct point_st *point)
{
	print_num(point->x);
	print_num(point->y);
	print_num(point->z);
	print_num(point->d);
	return point->x;
}
*/

static int struct_access()
{
	void *code;
	sljit_uw len;
	point_func_t func;

	struct point_st point = {
		-5, -20, 5, 'a'
	};

	/* Create a SLJIT compiler */
	struct sljit_compiler *C = sljit_create_compiler(NULL);

	sljit_emit_enter(C, 0, SLJIT_ARGS1(W, W), 1, 1, 0);
	/*                  opt arg               R  S  local_size */

	/* S0->x --> R0; print_num(R0) */
	sljit_emit_op1(C, SLJIT_MOV, SLJIT_R0, 0, SLJIT_MEM1(SLJIT_S0), SLJIT_OFFSETOF(struct point_st, x));
	sljit_emit_icall(C, SLJIT_CALL, SLJIT_ARGS1(W, P), SLJIT_IMM, SLJIT_FUNC_ADDR(print_num));

	/* S0->y --> R0; print_num(R0) */
	sljit_emit_op1(C, SLJIT_MOV_S32, SLJIT_R0, 0, SLJIT_MEM1(SLJIT_S0), SLJIT_OFFSETOF(struct point_st, y));
	sljit_emit_icall(C, SLJIT_CALL, SLJIT_ARGS1(W, P), SLJIT_IMM, SLJIT_FUNC_ADDR(print_num));

	/* S0->z --> R0; print_num(R0) */
	sljit_emit_op1(C, SLJIT_MOV_S16, SLJIT_R0, 0, SLJIT_MEM1(SLJIT_S0), SLJIT_OFFSETOF(struct point_st, z));
	sljit_emit_icall(C, SLJIT_CALL, SLJIT_ARGS1(W, P), SLJIT_IMM, SLJIT_FUNC_ADDR(print_num));

	/* S0->d --> R0; print_num(R0) */
	sljit_emit_op1(C, SLJIT_MOV_S8, SLJIT_R0, 0, SLJIT_MEM1(SLJIT_S0), SLJIT_OFFSETOF(struct point_st, d));
	sljit_emit_icall(C, SLJIT_CALL, SLJIT_ARGS1(W, P), SLJIT_IMM, SLJIT_FUNC_ADDR(print_num));

	/* return S0->x */
	sljit_emit_return(C, SLJIT_MOV, SLJIT_MEM1(SLJIT_S0), SLJIT_OFFSETOF(struct point_st, x));

	/* Generate machine code */
	code = sljit_generate_code(C, 0, NULL);
	len = sljit_get_generated_code_size(C);

	/* Execute code */
	func = (point_func_t)code;
	printf("func return %ld\n", (long)func(&point));

	/* dump_code(code, len); */

	/* Clean up */
	sljit_free_compiler(C);
	sljit_free_code(code, NULL);
	return 0;
}

int main()
{
	return struct_access();
}
