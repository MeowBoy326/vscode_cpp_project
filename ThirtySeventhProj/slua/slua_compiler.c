/*
  Copyright (c) 2009 Robert G. Jakabosky
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.

  MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "slua_compiler.h"
#include "codegen.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "lua_vm_ops.h"

#include "lopcodes.h"
#include "lobject.h"
#include "lstate.h"
#include "ldo.h"
#include "lmem.h"

static unsigned int OptLevel = 3;
static bool DisableOpt = false;
static bool OptLevelO1 = false;
static bool OptLevelO2 = false;
static bool OptLevelO3 = false;

#define BRANCH_COND -1
#define BRANCH_NONE -2

const char *lua_vm_get_var_type(val_t type, hint_t hints) {
	switch(type) {
	case VAR_T_VOID:
		return "void";
	case VAR_T_INT:
	case VAR_T_ARG_A:
	case VAR_T_ARG_B:
	case VAR_T_ARG_BK:
	case VAR_T_ARG_Bx:
	case VAR_T_ARG_Bx_NUM_CONSTANT:
	case VAR_T_ARG_B_FB2INT:
	case VAR_T_ARG_sBx:
	case VAR_T_ARG_C:
	case VAR_T_ARG_CK:
	case VAR_T_ARG_C_NUM_CONSTANT:
	case VAR_T_ARG_C_NEXT_INSTRUCTION:
	case VAR_T_ARG_C_FB2INT:
	case VAR_T_PC_OFFSET:
	case VAR_T_INSTRUCTION:
	case VAR_T_NEXT_INSTRUCTION:
		return "uint32_t";
	case VAR_T_LUA_STATE_PTR:
		return "lua_State *";
	case VAR_T_K:
		return "TValue *";
	case VAR_T_CL:
		return "LClosure *";
	case VAR_T_OP_VALUE_0:
	case VAR_T_OP_VALUE_1:
	case VAR_T_OP_VALUE_2:
		if(hints & HINT_USE_LONG) {
			return "uint64_t";
		}
		return "double";
	default:
		fprintf(stderr, "Error: missing var_type=%d\n", type);
		exit(1);
		break;
	}
	return NULL;
}

/**
 *
 * OPFunc: Lua VM op functions.
 *
 */

struct OPFunc {
	const vm_func_info *info;
	CFuncProto *proto;
	OPFunc     *next;
};

static OPFunc *new_OPFunc(const vm_func_info *info, OPFunc *next) {
	const char *ret_type;
	CValue val = { .type = VOID };
	char buf[128];
	int x;
	OPFunc *func = (OPFunc *)malloc(sizeof(OPFunc));

	func->info = info;
	func->next = next;
	ret_type = lua_vm_get_var_type(info->ret_type, info->hint);
	func->proto = new_CFuncProto(ret_type, info->name, false);
	for(x = 0; info->params[x] != VAR_T_VOID; x++) {
		snprintf(buf, 128, "%d", x);
		CFuncProto_create_param(func->proto, &val, lua_vm_get_var_type(info->params[x], info->hint), buf);
	}
	CValue_void(&val);
	return func;
}

static void free_OPFunc(OPFunc *func) {
	OPFunc *next;
	if(func == NULL) return;
	next = func->next;
	free_CFuncProto(func->proto);
	free(func);
	free_OPFunc(next);
}

//===----------------------------------------------------------------------===//
// Lua bytecode to C code compiler
//===----------------------------------------------------------------------===//

static void get_proto_constant(CValue *val, TValue *constant) {
	switch(ttype(constant)) {
	case LUA_TBOOLEAN:
		CValue_boolean(val, !l_isfalse(constant));
		break;
	case LUA_TNUMBER:
		CValue_double(val, nvalue(constant));
		break;
	case LUA_TSTRING:
		/* not used. */
		CValue_void(val);
		break;
	case LUA_TNIL:
	default:
		CValue_void(val);
		break;
	}
}

static OPFunc *vm_op_funcs[NUM_OPCODES];
static int strip_code = false;
static CFuncProto *vm_next_OP_proto = NULL;
static CFuncProto *vm_count_OP_proto = NULL;
static CFuncProto *vm_print_OP_proto = NULL;
static CFuncProto *vm_mini_vm_proto = NULL;
static CFuncProto *vm_get_current_closure_proto = NULL;
static CFuncProto *vm_get_current_constants_proto = NULL;
static CFuncProto *vm_get_number_proto = NULL;
static CFuncProto *vm_set_number_proto = NULL;
static CFuncProto *vm_get_long_proto = NULL;
static CFuncProto *vm_set_long_proto = NULL;

int slua_compiler_main() {
	return 0;
}

SLuaCompiler *slua_new_compiler(lua_State *L, FILE *file, int strip) {
	const vm_func_info *func_info;
	int opcode;
	int i;
	SLuaCompiler *compiler = (SLuaCompiler *)malloc(sizeof(SLuaCompiler));
	compiler->L = L;
	compiler->file = file;
	compiler->strip = strip;

	// set OptLevel
	if(OptLevelO1) OptLevel = 1;
	if(OptLevelO2) OptLevel = 2;
	if(OptLevelO3) OptLevel = 3;
	if(DisableOpt) OptLevel = 0;
	strip_code = strip;

	// define extern vm_next_OP
	vm_next_OP_proto = build_CFuncProto("void", "vm_next_OP", true,
		"lua_State *","L", "LClosure *","cl", "int ","pc_offset", NULL);
	vm_count_OP_proto = build_CFuncProto("void", "vm_count_OP", true,
		"uint32_t","i", NULL);

	vm_print_OP_proto = build_CFuncProto("void", "vm_print_OP", true,
		"lua_State *","L", "LClosure *","cl", "uint32_t","i", "int","pc_offset", NULL);

	vm_mini_vm_proto = build_CFuncProto("void", "vm_mini_vm", true,
		"lua_State *","L", "LClosure *","cl", "int","count", "int","pseudo_ops_offset", NULL);

	vm_get_current_closure_proto = build_CFuncProto("LClosure *", "vm_get_current_closure", true,
		"lua_State *","L", NULL);

	vm_get_current_constants_proto = build_CFuncProto("TValue *", "vm_get_current_constants", true,
		"LClosure *","cl", NULL);

	vm_get_number_proto = build_CFuncProto("double", "vm_get_number", true,
		"lua_State *","L", "int","idx", NULL);
	vm_set_number_proto = build_CFuncProto("void", "vm_set_number", true,
		"lua_State *","L", "int","idx", "double","num", NULL);

	vm_get_long_proto = build_CFuncProto("uint64_t", "vm_get_long", true,
		"lua_State *","L", "int","idx", NULL);
	vm_set_long_proto = build_CFuncProto("void", "vm_set_long", true,
		"lua_State *","L", "int","idx", "uint64_t","num", NULL);

	// create prototype for vm_* functions.
	for(i = 0; i < NUM_OPCODES; i++) vm_op_funcs[i] = NULL; // clear list.
	for(i = 0; true; i++) {
		func_info = &vm_op_functions[i];
		opcode = func_info->opcode;
		if(opcode < 0) break;
		vm_op_funcs[opcode] = new_OPFunc(func_info, vm_op_funcs[opcode]);
	}
	return compiler;
}

void slua_free_compiler(SLuaCompiler *compiler) {
	int i;
	for(i = 0; i < NUM_OPCODES; i++) {
		if(vm_op_funcs[i]) free_OPFunc(vm_op_funcs[i]);
	}
	free_CFuncProto(vm_next_OP_proto);
	free_CFuncProto(vm_count_OP_proto);
	free_CFuncProto(vm_print_OP_proto);
	free_CFuncProto(vm_mini_vm_proto);
	free_CFuncProto(vm_get_current_closure_proto);
	free_CFuncProto(vm_get_current_constants_proto);
	free_CFuncProto(vm_get_number_proto);
	free_CFuncProto(vm_set_number_proto);
	free_CFuncProto(vm_get_long_proto);
	free_CFuncProto(vm_set_long_proto);
	free(compiler);
}

/*
 * Pre-Compile all loaded functions.
 */
void slua_compiler_compile_all(SLuaCompiler *compiler, Proto *parent) {
	int i;
	/* pre-compile parent */
	slua_compiler_compile(compiler, parent);
	/* pre-compile all children */
	for(i = 0; i < parent->sizep; i++) {
		slua_compiler_compile_all(compiler, parent->p[i]);
	}
}

#define MAX_FUNC_NAME_LEN 33
int slua_compiler_proto_name(Proto *p, char *buf, int buf_len) {
	int len = 0;
	int rc;
	int i;

	assert(buf_len > MAX_FUNC_NAME_LEN);
	// convert proto source info into function name.
	strncpy(buf, getstr(p->source), MAX_FUNC_NAME_LEN);
	// replace non-alphanum characters with '_'
	for(i = 0; i < MAX_FUNC_NAME_LEN; i++) {
		char c = buf[i];
		if(c == '\0') break;
		if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) {
			continue;
		}
		if(c == '\n' || c == '\r') {
			buf[i] = '\0';
			break;
		}
		buf[i] = '_';
	}
	buf[i] = '\0';
	len = i;
	rc = snprintf(buf + len, buf_len - len ,"_%d_%d",p->linedefined, p->lastlinedefined);
	if(rc <= 0) return -1;
	len += rc;

	return len;
}

#define BUF_LEN 512
void slua_compiler_compile(SLuaCompiler *compiler, Proto *p) {
	lua_State *L = compiler->L;
	Instruction *code=p->code;
	TValue *k=p->k;
	int code_len=p->sizecode;
	OPFunc *opfunc;
	CFunc *func = NULL;
	CodeBlock *true_block=NULL;
	CodeBlock *false_block=NULL;
	CodeBlock *current_block=NULL;
	CodeBlock *entry_block=NULL;
	CodeBlock *ip=NULL;
	CValue *brcond=NULL;
	CValue func_L = { .type = VOID };
	CValue func_cl = { .type = VOID };
	CValue func_k = { .type = VOID };
	const vm_func_info *func_info;
	CValues *args = NULL;
	CValue call = { .type = VOID };
	CValue val = { .type = VOID };
	CValue val2 = { .type = VOID };
	CValue val3 = { .type = VOID };
	char buf[BUF_LEN];
	//char locals[LUAI_MAXVARS];
	int strip_ops=0;
	int branch;
	Instruction op_intr;
	int opcode;
	int mini_op_repeat=0;
	int i;
	int x;
	int len;
	hint_t *op_hints = NULL;
	CValues **op_values = NULL;
	CodeBlock **op_blocks = NULL;
	bool *need_op_block = NULL;

	// create function.
	len = slua_compiler_proto_name(p, buf, BUF_LEN);
	func = new_CFunc("int", buf, false);
	CFunc_create_param(func, &func_L, "lua_State *", "L");
	args = new_CValues(0);
	// entry block
	entry_block = CFunc_create_block(func, "entry");
	ip = entry_block;
	// pre-create basic blocks.
	op_hints      = (hint_t *)calloc(code_len, sizeof(hint_t));
	op_values     = (CValues **)calloc(code_len, sizeof(CValues *));
	op_blocks     = (CodeBlock **)calloc(code_len, sizeof(CodeBlock *));
	need_op_block = (bool *)calloc(code_len, sizeof(bool));
	// get LClosure & constants.
	CodeBlock_call(ip, &func_cl, "cl", vm_get_current_closure_proto, &(func_L), NULL);
	CodeBlock_call(ip, &func_k, "k", vm_get_current_constants_proto, &(func_cl), NULL);

	// find all jump/branch destinations and create a new basic block at that opcode.
	// also build hints for some opcodes.
	for(i = 0; i < code_len; i++) {
		op_intr=code[i];
		opcode = GET_OPCODE(op_intr);
		// combind simple ops into one function call.
		if(is_mini_vm_op(opcode)) {
			mini_op_repeat++;
		} else {
			if(mini_op_repeat >= 3 && OptLevel > 1) {
				op_hints[i - mini_op_repeat] |= HINT_MINI_VM;
			}
			mini_op_repeat = 0;
		}
		switch (opcode) {
			case OP_LOADBOOL:
				branch = i+1;
				// check C operand if C!=0 then skip over the next op_block.
				if(GETARG_C(op_intr) != 0)
					++branch;
				need_op_block[branch] = true;
				break;
			case OP_LOADK: {
				// check if arg Bx is a number constant.
				TValue *rb = k + INDEXK(GETARG_Bx(op_intr));
				if(ttisnumber(rb)) op_hints[i] |= HINT_Bx_NUM_CONSTANT;
				break;
			}
			case OP_JMP:
				// always branch to the offset stored in operand sBx
				branch = i + 1 + GETARG_sBx(op_intr);
				need_op_block[branch] = true;
				break;
			case OP_TAILCALL:
				branch = i+1;
				need_op_block[0] = true; /* branch to start of function if this is a recursive tail-call. */
				need_op_block[branch] = true; /* branch to return instruction if not recursive. */
				break;
			case OP_EQ:
			case OP_LT:
			case OP_LE:
				// check if arg C is a number constant.
				if(ISK(GETARG_C(op_intr))) {
					TValue *rc = k + INDEXK(GETARG_C(op_intr));
					if(ttisnumber(rc)) op_hints[i] |= HINT_C_NUM_CONSTANT;
				}
				if(GETARG_A(op_intr) == 1) {
					op_hints[i] |= HINT_NOT;
				}
				// fall-through
			case OP_TEST:
			case OP_TESTSET:
			case OP_TFORLOOP:
				branch = ++i + 1;
				op_intr=code[i];
				need_op_block[branch + GETARG_sBx(op_intr)] = true; /* inline JMP op. */
				need_op_block[branch] = true;
				break;
			case OP_FORLOOP:
				branch = i+1;
				need_op_block[branch] = true;
				branch += GETARG_sBx(op_intr);
				need_op_block[branch] = true;
				break;
			case OP_FORPREP:
				branch = i + 1 + GETARG_sBx(op_intr);
				need_op_block[branch] = true;
				need_op_block[branch + 1] = true;
				// test if init/plimit/pstep are number constants.
				if(OptLevel > 1 && i >= 3) {
					lua_Number nums[3];
					bool found_val[3] = { false, false , false };
					bool is_const_num[3] = { false, false, false };
					bool all_longs=true;
					int found=0;
					CValues *vals;
					int forprep_ra = GETARG_A(op_intr);
					bool no_jmp_end_point = true; // don't process ops after finding a jmp end point.
					vals = new_CValues(4);
					// find & load constants for init/plimit/pstep
					for(x = 1; x < 6 && found < 3 && no_jmp_end_point; ++x) {
						const TValue *tmp_k;
						Instruction op_intr2;
						int ra;

						if((i - x) < 0) break;
						op_intr2 = code[i - x];
						// get 'a' register.
						ra = GETARG_A(op_intr2);
						ra -= forprep_ra;
						// check for jmp end-point.
						no_jmp_end_point = !(need_op_block[i - x]);
						// check that the 'a' register is for one of the value we are interested in.
						if(ra < 0 || ra > 2) continue;
						// only process this opcode if we haven't seen this value before.
						if(found_val[ra]) continue;
						found_val[ra] = true;
						found++;
						if(GET_OPCODE(op_intr2) == OP_LOADK) {
							tmp_k = k + GETARG_Bx(op_intr2);
							if(ttisnumber(tmp_k)) {
								lua_Number num=nvalue(tmp_k);
								nums[ra] = num;
								// test if number is a whole number
								all_longs &= (floor(num) == num);
								CValue_double(&val, num);
								CValues_set(vals, ra, &val);
								is_const_num[ra] = true;
								op_hints[i - x] |= HINT_SKIP_OP;
								continue;
							}
						}
						all_longs = false;
					}
					all_longs &= (found == 3);
					// create for_idx OP_FORPREP will inialize it.
					op_hints[branch] = HINT_FOR_N_N_N;
					if(all_longs) {
						CFunc_var(func, &val, "uint64_t", "for_idx", NULL);
						CValues_set(vals, 3,  &val);
						op_hints[branch] |= HINT_USE_LONG;
					} else {
						CFunc_var(func, &val, "double", "for_idx", NULL);
						CValues_set(vals, 3,  &val);
					}
					op_values[branch] = vals;
					// check if step, init, limit are constants
					if(is_const_num[2]) {
						// step is a constant
						if(is_const_num[0]) {
							// init & step are constants.
							if(is_const_num[1]) {
								// all are constants.
								op_hints[i] = HINT_FOR_N_N_N;
							} else {
								// limit is variable.
								op_hints[i] = HINT_FOR_N_M_N;
							}
							op_values[i] = new_CValues(3);
							CValues_set(op_values[i], 0, CValues_get(vals, 0));
							CValues_set(op_values[i], 2, CValues_get(vals, 2));
						} else if(is_const_num[1]) {
							// init is variable, limit & step are constants.
							op_hints[i] = HINT_FOR_M_N_N;
							op_values[i] = new_CValues(3);
							CValues_set(op_values[i], 1, CValues_get(vals, 1));
							CValues_set(op_values[i], 2, CValues_get(vals, 2));
						}
						// check the direct of step.
						if(nums[2] > 0) {
							op_hints[branch] |= HINT_UP;
						} else {
							op_hints[branch] |= HINT_DOWN;
						}
					}
					if(op_hints[i] == HINT_NONE) {
						// don't skip LOADK ops, since we are not inlining them.
						for(x=i-3; x < i; x++) {
							if(op_hints[x] & HINT_SKIP_OP) op_hints[x] &= ~(HINT_SKIP_OP);
						}
					}
					if(all_longs) {
						for(x = 0; x < 3; ++x) {
							CValue_integer(&val, (lua_Long)nums[x]);
							CValues_set(vals, x, &val);
						}
					}
					// make sure OP_FORPREP doesn't subtract 'step' from 'init'
					op_hints[i] |= HINT_NO_SUB;
				}
				break;
			case OP_SETLIST:
				// if C == 0, then next code value is count value.
				if(GETARG_C(op_intr) == 0) {
					i++;
				}
				break;
			case OP_ADD:
			case OP_SUB:
			case OP_MUL:
			case OP_DIV:
			case OP_MOD:
			case OP_POW:
				// check if arg C is a number constant.
				if(ISK(GETARG_C(op_intr))) {
					TValue *rc = k + INDEXK(GETARG_C(op_intr));
					if(ttisnumber(rc)) op_hints[i] |= HINT_C_NUM_CONSTANT;
				}
				break;
			default:
				break;
		}
		// update local variable type hints.
		//vm_op_hint_locals(locals, p->maxstacksize, k, op_intr);
	}
	for(i = 0; i < code_len; i++) {
		if(need_op_block[i]) {
			op_intr=code[i];
			opcode = GET_OPCODE(op_intr);
			snprintf(buf,128,"op_block_%s_%d",luaP_opnames[opcode],i);
			op_blocks[i] = CFunc_create_block(func, buf);
		} else {
			op_blocks[i] = NULL;
		}
	}
	// branch "entry" to first block.
	if(need_op_block[0]) {
		CodeBlock_jump(ip, op_blocks[0]);
	} else {
		current_block = entry_block;
	}
	// gen op calls.
	for(i = 0; i < code_len; i++) {
		if(op_blocks[i] != NULL) {
			if(current_block) {
				// add branch to new block.
				CodeBlock_jump(ip, op_blocks[i]);
			}
			ip = op_blocks[i];
			current_block = op_blocks[i];
		}
		// skip dead unreachable code.
		if(current_block == NULL) {
			if(strip_code) strip_ops++;
			continue;
		}
		branch = i+1;
		op_intr=code[i];
		opcode = GET_OPCODE(op_intr);
		opfunc = vm_op_funcs[opcode];
		// combind multiple simple ops into one call.
		if(op_hints[i] & HINT_MINI_VM) {
			int op_count = 1;
			// count mini ops and check for any branch end-points.
			while(is_mini_vm_op(GET_OPCODE(code[i + op_count])) &&
					(op_hints[i + op_count] & HINT_SKIP_OP) == 0) {
				// branch end-point in middle of mini ops block.
				if(need_op_block[i + op_count]) {
					op_hints[i + op_count] |= HINT_MINI_VM; // mark start of new mini vm ops.
					break;
				}
				op_count++;
			}
			if(op_count >= 3) {
				CValue_integer(&val2, op_count);
				CValue_integer(&val3, (i - strip_ops));
				CodeBlock_call_void(ip, vm_mini_vm_proto, &(func_L), &(func_cl), &(val2), &(val3), NULL);
				if(strip_code && strip_ops > 0) {
					while(op_count > 0) {
						code[i - strip_ops] = code[i];
						i++;
						op_count--;
					}
				} else {
					i += op_count;
				}
				i--;
				continue;
			} else {
				// mini ops block too small.
				op_hints[i] &= ~(HINT_MINI_VM);
			}
		}
		// find op function with matching hint.
		while(opfunc->next != NULL && (opfunc->info->hint & op_hints[i]) != opfunc->info->hint) {
			opfunc = opfunc->next;
		}
		//fprintf(stderr, "%d: '%s' (%d) = 0x%08X, hint=0x%X\n", i, luaP_opnames[opcode], opcode, op_intr, op_hints[i]);
		//fprintf(stderr, "%d: func: '%s', func hints=0x%X\n", i, opfunc->info->name,opfunc->info->hint);
		if(op_hints[i] & HINT_SKIP_OP) {
			if(strip_code) strip_ops++;
			continue;
		}
		if(strip_code) {
			// strip all opcodes.
			strip_ops++;
			if(strip_ops > 0 && strip_ops < (i+1)) {
				// move opcodes we want to keep to new position.
				code[(i+1) - strip_ops] = op_intr;
			}
		}
		// setup arguments for opcode function.
		func_info = opfunc->info;
		if(func_info == NULL) {
			fprintf(stderr, "Error missing vm_OP_* function for opcode: %d\n", opcode);
			goto cleanup;
		}
		// special handling of OP_FORLOOP
		if(opcode == OP_FORLOOP) {
			CodeBlock *loop_test;
			CodeBlock *prep_block;
			CodeBlock *incr_block;
			const CValue *init,*step,*idx_var;
			//*cur_idx,*next_idx;
			CValues *vals;

			vals=op_values[i];
			if(vals != NULL) {
				// get init value from forprep block
				init = CValues_get(vals, 0);
				// get for loop 'idx' variable.
				step = CValues_get(vals, 2);
				idx_var = CValues_get(vals, 3);
				assert(idx_var != NULL);
				incr_block = current_block;
				CodeBlock_add(ip, idx_var, idx_var, step);
				CValues_set(vals, 0, idx_var);
				// create extra BasicBlock for vm_OP_FORLOOP_*
				snprintf(buf,128,"op_block_%s_%d_loop_test",luaP_opnames[opcode],i);
				loop_test = CFunc_create_block(func, buf);
				// create unconditional jmp from current block to loop test block
				CodeBlock_jump(ip, loop_test);
				// create unconditional jmp from forprep block to loop test block
				prep_block = op_blocks[branch + GETARG_sBx(op_intr) - 1];
				ip = prep_block;
				CodeBlock_jump(ip, loop_test);
				// set current_block to loop_test block
				current_block = loop_test;
				ip = current_block;
			}
		}
		clear_CValues(args);
		for(x = 0; func_info->params[x] != VAR_T_VOID ; x++) {
			CValue_void(&val);
			switch(func_info->params[x]) {
			case VAR_T_ARG_A:
				CValue_integer(&val, GETARG_A(op_intr));
				break;
			case VAR_T_ARG_C:
				CValue_integer(&val, GETARG_C(op_intr));
				break;
			case VAR_T_ARG_C_FB2INT:
				CValue_integer(&val, luaO_fb2int(GETARG_C(op_intr)));
				break;
			case VAR_T_ARG_Bx_NUM_CONSTANT:
				get_proto_constant(&val, k + INDEXK(GETARG_Bx(op_intr)));
				break;
			case VAR_T_ARG_C_NUM_CONSTANT:
				get_proto_constant(&val, k + INDEXK(GETARG_C(op_intr)));
				break;
			case VAR_T_ARG_C_NEXT_INSTRUCTION: {
				int c = GETARG_C(op_intr);
				// if C == 0, then next code value is used as ARG_C.
				if(c == 0) {
					if((i+1) < code_len) {
						c = code[i+1];
						if(strip_code) strip_ops++;
					}
				}
				CValue_integer(&val, c);
				break;
			}
			case VAR_T_ARG_B:
				CValue_integer(&val, GETARG_B(op_intr));
				break;
			case VAR_T_ARG_B_FB2INT:
				CValue_integer(&val, luaO_fb2int(GETARG_B(op_intr)));
				break;
			case VAR_T_ARG_Bx:
				CValue_integer(&val, GETARG_Bx(op_intr));
				break;
			case VAR_T_ARG_sBx:
				CValue_integer(&val, GETARG_sBx(op_intr));
				break;
			case VAR_T_PC_OFFSET:
				CValue_integer(&val, i + 1 - strip_ops);
				break;
			case VAR_T_INSTRUCTION:
				CValue_integer(&val, op_intr);
				break;
			case VAR_T_NEXT_INSTRUCTION:
				CValue_integer(&val, code[i+1]);
				break;
			case VAR_T_LUA_STATE_PTR:
				CValue_set(&val, &(func_L));
				break;
			case VAR_T_K:
				CValue_set(&val, &(func_k));
				break;
			case VAR_T_CL:
				CValue_set(&val, &(func_cl));
				break;
			case VAR_T_OP_VALUE_0:
				if(op_values[i] != NULL) CValue_set(&val, CValues_get(op_values[i], 0));
				break;
			case VAR_T_OP_VALUE_1:
				if(op_values[i] != NULL) CValue_set(&val, CValues_get(op_values[i], 1));
				break;
			case VAR_T_OP_VALUE_2:
				if(op_values[i] != NULL) CValue_set(&val, CValues_get(op_values[i], 2));
				break;
			default:
				fprintf(stderr, "Error: not implemented!\n");
				goto cleanup;
			case VAR_T_VOID:
				fprintf(stderr, "Error: invalid value type!\n");
				goto cleanup;
			}
			if(val.type == VOID) {
				fprintf(stderr, "Error: Missing parameter '%d' for this opcode(%d) function=%s!\n", x,
					opcode, func_info->name);
				exit(1);
			}
			CValues_push_back(args, &val);
		}
		// create call to opcode function.
		if(func_info->ret_type != VAR_T_VOID) {
			CodeBlock_call_args(ip, &call, "retval", opfunc->proto, args);
		} else {
			CodeBlock_call_args(ip, NULL, NULL, opfunc->proto, args);
		}
		// handle retval from opcode function.
		switch (opcode) {
			case OP_LOADBOOL:
				// check C operand if C!=0 then skip over the next op_block.
				if(GETARG_C(op_intr) != 0) branch += 1;
				else branch = BRANCH_NONE;
				break;
			case OP_LOADK:
			case OP_LOADNIL:
			case OP_GETGLOBAL:
			case OP_GETTABLE:
			case OP_SETGLOBAL:
			case OP_SETTABLE:
			case OP_NEWTABLE:
			case OP_SELF:
			case OP_ADD:
			case OP_SUB:
			case OP_MUL:
			case OP_DIV:
			case OP_MOD:
			case OP_POW:
			case OP_UNM:
			case OP_NOT:
			case OP_LEN:
			case OP_CONCAT:
			case OP_GETUPVAL:
			case OP_MOVE:
				branch = BRANCH_NONE;
				break;
			case OP_CLOSE:
			case OP_SETUPVAL:
				branch = BRANCH_NONE;
				break;
			case OP_VARARG:
			case OP_CALL:
				branch = BRANCH_NONE;
				break;
			case OP_TAILCALL:
				CValue_integer(&val, PCRTAILRECUR);
				CodeBlock_cmp_ne(ip, &val2, "brcond", &call, &val);
				brcond=&val2;
				i++; // skip return opcode.
				false_block = op_blocks[0]; // branch to start of function if this is a recursive tail-call.
				true_block = op_blocks[i]; // branch to return instruction if not recursive.
				CodeBlock_cond_jump(ip, brcond, true_block, false_block);
				ip = op_blocks[i];
				CodeBlock_ret(ip, &call);
				current_block = NULL; // have terminator
				branch = BRANCH_NONE;
				break;
			case OP_JMP:
				// always branch to the offset stored in operand sBx
				branch += GETARG_sBx(op_intr);
				// call vm_OP_JMP just in case luai_threadyield is defined.
				break;
			case OP_EQ:
			case OP_LT:
			case OP_LE:
			case OP_TEST:
			case OP_TESTSET:
			case OP_TFORLOOP:
				brcond=&call;
				false_block=op_blocks[branch+1];
				/* inlined JMP op. */
				branch = ++i + 1;
				if(strip_code) {
					strip_ops++;
					if(strip_ops > 0 && strip_ops < (i+1)) {
						// move opcodes we want to keep to new position.
						code[(i+1) - strip_ops] = code[i];
					}
				}
				op_intr=code[i];
				branch += GETARG_sBx(op_intr);
				true_block=op_blocks[branch];
				branch = BRANCH_COND; // do conditional branch
				break;
			case OP_FORLOOP: {
				CFuncProto *set_func=vm_set_number_proto;
				CValues *vals;

				brcond=&call;
				true_block=op_blocks[branch + GETARG_sBx(op_intr)];
				false_block=op_blocks[branch];
				branch = BRANCH_COND; // do conditional branch

				// update external index if needed.
				vals=op_values[i];
				if(vals != NULL) {
					CodeBlock *idx_block;
					if(op_hints[i] & HINT_USE_LONG) {
						set_func = vm_set_long_proto;
					}
					// create extra BasicBlock
					snprintf(buf,128,"op_block_%s_%d_set_for_idx",luaP_opnames[opcode],i);
					idx_block = CFunc_create_block(func, buf);
					ip = idx_block;
					// copy idx value to Lua-stack.
					CValue_integer(&val, (GETARG_A(op_intr) + 3));
					CodeBlock_call_void(ip, set_func, &(func_L), &(val), CValues_get(vals, 3), NULL);
					// create jmp to true_block
					CodeBlock_jump(ip, true_block);
					true_block = idx_block;
					ip = current_block;
				}
				break;
			}
			case OP_FORPREP: {
				CFuncProto *get_func=vm_get_number_proto;
				const CValue *idx_var,*init;
				CValue call2 = { .type = VOID };
				CValues *vals;

				op_blocks[i] = current_block;
				branch += GETARG_sBx(op_intr);
				vals=op_values[branch];
				// if no saved value, then use slow method.
				if(vals == NULL) break;
				if(op_hints[branch] & HINT_USE_LONG) {
					get_func = vm_get_long_proto;
				}
				// get non-constant init from Lua stack.
				if(CValues_get(vals, 0) == NULL) {
					CValue_integer(&val, (GETARG_A(op_intr) + 0));
					CodeBlock_call(ip, &call2, "for_init", get_func, &(func_L), &(val), NULL);
					CValues_set(vals, 0,  &call2);
				}
				init = CValues_get(vals, 0);
				// get non-constant limit from Lua stack.
				if(CValues_get(vals, 1) == NULL) {
					CValue_integer(&val, (GETARG_A(op_intr) + 1));
					CodeBlock_call(ip, &call2, "for_limit", get_func, &(func_L), &(val), NULL);
					CValues_set(vals, 1,  &call2);
				}
				// get non-constant step from Lua stack.
				if(CValues_get(vals, 2) == NULL) {
					CValue_integer(&val, (GETARG_A(op_intr) + 2));
					CodeBlock_call(ip, &call2, "for_step", get_func, &(func_L), &(val), NULL);
					CValues_set(vals, 2,  &call2);
				}
				// get for loop 'idx' variable.
				assert(CValues_get(vals, 3) != NULL);
				idx_var = CValues_get(vals, 3);
				CodeBlock_store(ip, idx_var, init); // store 'for_init' value.
				current_block = NULL; // have terminator
				branch = BRANCH_NONE;
				break;
			}
			case OP_SETLIST:
				// if C == 0, then next code value is used as ARG_C.
				if(GETARG_C(op_intr) == 0) {
					i++;
				}
				branch = BRANCH_NONE;
				break;
			case OP_CLOSURE: {
				Proto *p2 = p->p[GETARG_Bx(op_intr)];
				int nups = p2->nups;
				if(strip_code && strip_ops > 0) {
					while(nups > 0) {
						i++;
						code[i - strip_ops] = code[i];
						nups--;
					}
				} else {
					i += nups;
				}
				branch = BRANCH_NONE;
				break;
			}
			case OP_RETURN: {
				CodeBlock_ret(ip, &call);
				branch = BRANCH_NONE;
				current_block = NULL; // have terminator
				break;
			}
			default:
				fprintf(stderr, "Bad opcode: opcode=%d\n", opcode);
				break;
		}
		// branch to next block.
		if(branch >= 0 && branch < code_len) {
			CodeBlock_jump(ip, op_blocks[branch]);
			current_block = NULL; // have terminator
		} else if(branch == BRANCH_COND) {
			CodeBlock_cond_jump(ip, brcond, true_block, false_block);
			current_block = NULL; // have terminator
		}
	}
	// strip Lua bytecode and debug info.
	if(strip_code && strip_ops > 0) {
		code_len -= strip_ops;
		luaM_reallocvector(L, p->code, p->sizecode, code_len, Instruction);
		p->sizecode = code_len;
		luaM_reallocvector(L, p->lineinfo, p->sizelineinfo, 0, int);
		p->sizelineinfo = 0;
		luaM_reallocvector(L, p->locvars, p->sizelocvars, 0, LocVar);
		p->sizelocvars = 0;
		luaM_reallocvector(L, p->upvalues, p->sizeupvalues, 0, TString *);
		p->sizeupvalues = 0;
	}
	/* dump functions C code. */
	CFunc_dump(func, compiler->file);
cleanup:
	CValue_void(&func_L);
	CValue_void(&func_cl);
	CValue_void(&func_k);
	CValue_void(&call);
	CValue_void(&val);
	CValue_void(&val2);
	CValue_void(&val3);
	if(op_values) {
		for(i = 0; i < code_len; i++) {
			if(op_values[i]) {
				free_CValues(op_values[i]);
			}
		}
		free(op_values);
	}
	if(op_hints) free(op_hints);
	if(op_blocks) free(op_blocks);
	if(need_op_block) free(need_op_block);

	if(func) free_CFunc(func);
	if(args) free_CValues(args);
}

