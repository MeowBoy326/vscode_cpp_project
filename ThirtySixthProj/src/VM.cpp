#include "VM.h"
#include <assert.h>
#include "Value.h"
#include "State.h"
#include "Function.h"
#include "Stack.h"
#include "BaseLib.h"






VM::VM(State* state)
	: _state(state),
	_stackClosure(new Stack(20)),
	_curInsVal(nullptr)
{
	_stack = _state->getStack();
	_state->setVM(this);
}


VM::~VM()
{
}

void VM::execute()
{
}


void VM::execute_frame()
{

}


int VM::runCode(InstructionValue* insSetVal)
{
	//local a = 1  һ�����͵ĳ�ʼ����������ָ����3����OpCode_Push(1) OpCode_SetLocalVar (a)  OpCode_Assign (=)
	//a = 1  һ�����͵ĸ�ֵ��������ָ����3����OpCode_Push(1) OpCode_SetLocalVar (a)  OpCode_InitLocalVar (=)
	//һ�������������Ҳ��3��ָ�OpCode_GenerateClosure(1) OpCode_SetLocalVar (funcName)  OpCode_InitLocalVar (=)
	//����һ��ֵ��������OpCode_GetLocalVar������ѹ��ñ�����ֵ(�ӱհ���tableֵ�в���)��ջ��
	//�������ؼ���ֵ���ͻ�ѹ�뼸��ֵ��ջ�У�OpCode_Retָ��ֻ�Ǹ��߱հ�����ֵ�ĸ���
	/*���ú�������ʱ��û���ر��ָ�ֻ��OpCode_GetLocalVarѹ��ֵ��ջ��ִ���в����ĺ���ʱ�����Ⱦ���OpCode_SetLocalVarѹ���������
	  Ȼ�����OpCode_PassFunParamָ���������ֵΪջ�ϵ�ֵ
	*/

	//TODO
	/*�������յĻ���˼·���ǣ��հ��˳�ʱ��������ǰ��table,����Щ����ջ�ϵľ�delete�������������и����⣬�����һ����������һ�������������Ļ����հ�����ʱ���������Ҳ��delete����
	  ���Լ�һ����������հ��еı����Ƿ�����������š���Ȼ�հ��ڲ����õ����ı����ǲ�������������ģ���Ϊ��ʱ��ֵ�����ڵ�ǰ�հ�table�м�¼
	*/
	//���ܲ��ܼ�OpCode_Push(1),OpCode_SetLocalVar(a),OpCode_Assign(=)Ϊ����ָ��
	insSetVal->setParent(_curInsVal);
	_curInsVal = insSetVal;
	auto vtIns = insSetVal->getInstructionSet()->toVtInstructions();
	for (auto it = vtIns.begin(); it != vtIns.end(); ++it)  {
		if (insSetVal->getBreaked())  {
			printf("breaked!!!\n");
			return 0;
		}
		Instruction* ins = *it;
		switch (ins->op_code)
		{
		case Instruction::OpCode_AddGlobalTable:
			add_global_table();
			break;
		case Instruction::OpCode_EnterClosure:         //���뺯��ʱ����
			enterClosure();
			break;
		case Instruction::OpCode_QuitClosure:
			quitClosure();
			break;
		case Instruction::OpCode_InitLocalVar:         //�ֲ���������
			initLocalVar(ins);
			break;
		case Instruction::OpCode_GetLocalVar:             
			getLoacalVar(ins);
			break;
		case Instruction::OpCode_SetLocalVar:             
			setLoacalVar(ins);
			break;
		case Instruction::OpCode_GenerateClosure:
			generateClosure(ins);
			break;
		case Instruction::OpCode_PassFunParam:
			passFunParam(ins);
			break;
		case Instruction::OpCode_Call:
			call(ins);
			break;
		case Instruction::OpCode_Ret:
			funcionRet(ins);
			break;
		case Instruction::OpCode_Push:
			pushValue(ins);
			break;
		case Instruction::OpCode_Assign:
			assignOperate(ins);
			break;
		case Instruction::OpCode_Plus:
		case Instruction::OpCode_Minus:
		case Instruction::OpCode_Multiply:
		case Instruction::OpCode_Divide:
			operateNum(ins);
			break;
		case Instruction::OpCode_If:
			ifCompare(ins);
			break;
		case Instruction::OpCode_NumericFor:
			numericFor(ins);
			break;
		case Instruction::OpCode_GenericFor:
			genericFor(ins);
			break;
		case Instruction::OpCode_Break:
			return -1;
		case Instruction::OpCode_Less:
		case Instruction::OpCode_Greater:
		case Instruction::OpCode_LessEqual:
		case Instruction::OpCode_GreaterEqual:
		case Instruction::OpCode_NotEqual:
		case Instruction::OpCode_Equal:
			operateLogic(ins);
			break;
		case Instruction::OpCode_EnterBlock:
			enterBlock(ins);
			break;
		case Instruction::OpCode_QuitBlock:
			quitBlock(ins);
			break;
		case Instruction::OpCode_GenerateBlock:
			generateBlock(ins);
			break;
		case Instruction::OpCode_TableDefine:
			tableDefine(ins);
			break;
		case Instruction::OpCode_TableMemAccess:
			tableAccess(ins);
			break;
		case Instruction::OpCode_TableArrIndex:
			tableArrIndex(ins);
			break;
		case Instruction::OpCode_Negative:
			negNumber(ins);
			break;
		case Instruction::OpCode_Length:
			lenOfVale(ins);
			break;
		default:
			break;
		}
	}
	return 0;
}

void VM::generateClosure(Instruction* ins)
{
	assert(ins->param.value->Type() == Value::TYPE_FUNCTION);
	Function *func = static_cast<Function *>(ins->param.value);
	Closure *cl = func->generateClosure(_state) ;
	cl->setParentClosure(getCurrentClosure());
	_stack->Push(cl);
}

void VM::add_global_table()
{

}

void VM::call(Instruction* ins)
{
	int paramNum = ins->param.counter.counter1;                  //ʵ�θ���
	Value* callee = _stack->popValue();

	if (callee->Type() == Value::TYPE_NIL)  {
		printf("error, call a nil val");
	}

	if (callee->Type() == Value::TYPE_NATIVE_FUNCTION)  {
		((NativeFunc*)callee)->doCall(_state, (void*)paramNum);
	}
	else if (callee->Type() == Value::TYPE_CLOSURE)  {
		Closure* cl = static_cast<Closure*>(callee)->clone();    //��������¡������Ϊ�еݹ���ú���
		_stackClosure->Push(cl);
		cl->setActParamNum(paramNum);
		int needRetNum = ins->param.counter.counter2;
		cl->setNeedRetNum(needRetNum);
		runCode(cl->getPrototype());
	}
}

void VM::enterClosure()
{
	getCurrentClosure()->initClosure();
}

void VM::quitClosure()
{
	Closure* cl = getCurrentClosure();
	cl->clearClosure();
	cl->balanceStack();
	_stackClosure->popValue();
}

Closure* VM::getCurrentClosure()
{
	if (_stackClosure->Size() == 0)  {
		return nullptr;
	}
	Closure* cl = (Closure*)_stackClosure->Top()->param.value;
	assert(cl->Type() == Value::TYPE_CLOSURE);
	return cl;
}

void VM::passFunParam(Instruction* ins)                      //��ʵ���Ҳ������OpCode_InitLocalVar����
{
	int needParamNum = ins->param.counter.counter1;
	int actParamNum = getCurrentClosure()->getActParamNum();
	
	assignVals(needParamNum, actParamNum, 0);               //���ѹ���ֵ�Ȳ����࣬����ᴦ���
}

void VM::assignVals(int num_key, int num_val, int type)      //�������ô������ʱҲ���������
{
	Table* tab = getCurrentClosure()->getTopTable();
	std::list<Value*> listKeys;
	std::list<Value*> listVals;
	for (int i = 0; i < num_key; i++)  {
		listKeys.push_front(_stack->popValue());
	}
	if (num_key > num_val)  {                   //ֵ����key���п����Ǻ��������a��b =f(),��൯��key�ĸ�������Ϊջ�Ͽ����������ط���ֵ
		num_val = (unsigned int)num_key < _stack->Size() ? num_key : _stack->Size();
	}
	for (int i = 0; i < num_val; i++)    {      //Ҫ��ʣ�µ�ֵȫ������
		Value* val = _stack->popValue();
		listVals.push_front(val);
	}
	num_val = listVals.size();
	if (num_key > num_val)  {     //a,b,c = 1, 2
		for (int i = 0; i < num_key - num_val; i++)  {
			listVals.push_back(new Nil());
		}
	}

	while (!listKeys.empty() && !listVals.empty())  {     //�п����ж���Ĳ���������
		Value* key = listKeys.front();
		Value* val = listVals.front();
		if (type == 1)  {   
			if (getCurrentClosure()->findUpTables(key, nullptr, &tab) == -1)  {      //��ֵʱû���ҵ��ͷ���ȫ�ֱ���
				tab = _state->getGlobalTable();
			}
		}  
		tab->Assign(key, val);
		listKeys.pop_front();
		listVals.pop_front();
	}
}

void VM::initLocalVar(Instruction* ins)
{
	int num_key = ins->param.counter.counter1;
	int num_val = ins->param.counter.counter2;
	if (num_key == 1 && num_val == 1)  {
		assignSimple(0);
	}
	else  {
		assignVals(num_key, num_val, 0);
	}
}

void VM::assignOperate(Instruction* ins)
{
	int num_key = ins->param.counter.counter1;
	int num_val = ins->param.counter.counter2;
	if (num_key == 1 && num_val == 1)  {
		assignSimple(1);
	}
	else  {
		assignVals(num_key, num_val, 1);
	}
}

void VM::assignSimple(int type)
{
	Table* tab = getCurrentClosure()->getTopTable();
	Value* key = _stack->popValue();
	Value* val = _stack->popValue();
	if (key->Type() == Value::TYPE_STRING)  {
		if (type == 1 && getCurrentClosure()->findUpTables(key, nullptr, &tab) == -1)  {      //��ֵʱû���ҵ��ͷ���ȫ�ֱ���
			tab = _state->getGlobalTable();
		}
		tab->Assign(key, val);
	}
	else if (key->Type() == Value::TYPE_TABLEVAL) {
		((TableValue*)key)->SetValue(val);
	}
	
}


void VM::setLoacalVar(Instruction* ins)
{
	_stack->Push(ins->param.name);
}

void VM::getLoacalVar(Instruction* ins)
{
	Value* key = ins->param.name;
	Value* val = nullptr;
	if (getCurrentClosure()->findUpTables(key, &val, nullptr) != -1)  {
		_stack->Push(val);
	}
	else  {
		val = new Nil();
		_stack->Push(val);
	}
}

void VM::pushValue(Instruction* ins)
{
	if (ins->type == InstructionParam::InstructionParamType_Name)  {
		_stack->Push(ins->param.name);
	}  
	else if (ins->type == InstructionParam::InstructionParamType_Value)  {
		_stack->Push(ins->param.value);
	}
}

void VM::operateNum(Instruction* ins)
{
	Value* num1 = _stack->popValue();
	Value* num2 = _stack->popValue();
	
	if (num1->Type() == Value::TYPE_NIL ||
		num2->Type() == Value::TYPE_NIL)  {
		printf("error, operate on a nil value\n");
	}

	Value* ret = nullptr;
	if (num1->Type() == Value::TYPE_STRING &&
		num2->Type() == Value::TYPE_STRING)  {
		ret = ((String*)num2)->concat((String*)num1);
	}
	else if (num1->Type() == Value::TYPE_NUMBER &&
		num2->Type() == Value::TYPE_NUMBER)  {
		double num = 0;
		if (ins->op_code == Instruction::OpCode_Plus)  {
			num = ((Number*)num2)->Get() + ((Number*)num1)->Get();
		}
		else if (ins->op_code == Instruction::OpCode_Minus)  {
			num = ((Number*)num2)->Get() - ((Number*)num1)->Get();
		}
		else if (ins->op_code == Instruction::OpCode_Multiply)  {
			num = ((Number*)num2)->Get() * ((Number*)num1)->Get();
		}
		else if (ins->op_code == Instruction::OpCode_Divide)  {
			num = ((Number*)num2)->Get() / ((Number*)num1)->Get();
		}

		ret = new Number(num);
	}
	else  {
		printf("error, attempt to perform arithmetic on a type that cannot be operated\n");
	}
	_stack->Push(ret);
}

void VM::operateLogic(Instruction* ins)
{
	Number* right = (Number*)_stack->popValue();
	Number* left = (Number*)_stack->popValue();
	BoolValue* retLogic = new BoolValue();
	double num1 = left->Get();
	double num2 = right->Get();
	if (ins->op_code == Instruction::OpCode_Less)  {
		retLogic->setLogicVal(num1 < num2);
	}
	else if (ins->op_code == Instruction::OpCode_Greater)  {
		retLogic->setLogicVal(num1 > num2);
	}
	else if (ins->op_code == Instruction::OpCode_LessEqual)  {
		retLogic->setLogicVal(num1 <= num2);
	}
	else if (ins->op_code == Instruction::OpCode_GreaterEqual)  {
		retLogic->setLogicVal(num1 >= num2);
	}
	else if (ins->op_code == Instruction::OpCode_Equal)  {
		retLogic->setLogicVal(num1 == num2);
	}
	else if (ins->op_code == Instruction::OpCode_NotEqual)  {
		retLogic->setLogicVal(num1 != num2);
	}
	_stack->Push(retLogic);
}

void VM::funcionRet(Instruction* ins)
{
 	int num = ins->param.counter.counter1;
	Closure* cl = getCurrentClosure();
	cl->setActRetNum(num);
}

void VM::ifCompare(Instruction* ins)
{
	Value* logic = _stack->popValue();
	InstructionValue* leftBlock = (InstructionValue*)_stack->popValue();

	InstructionValue* rightBlock = nullptr;
	if (ins->param.counter.counter1 > 0)  {
		rightBlock = (InstructionValue*)_stack->popValue();
	}
	bool runLeft = true;                //����nil��false����ȫΪtrue
	if (logic->Type() == Value::TYPE_NIL)  {
		runLeft = false;
	}
	else {
		if (logic->Type() == Value::TYPE_BOOL)  {
			runLeft = ((BoolValue*)logic)->getLogicVal();
		}
	}
	if (runLeft)  {
		runBlockCode(leftBlock);
	}
	else  {
		if (rightBlock)  {
			runBlockCode(rightBlock);
		}
	}
}

void VM::numericFor(Instruction* ins)
{
	Value* valStart = _stack->popValue();      //�����Ʊ�������ѹ����ջ
	Table* top = getCurrentClosure()->getTopTable();
	Number* numStart = (Number*)top->GetValue(valStart);
	int iStart = ((Number*)top->GetValue(valStart))->GetInteger();
	int iEnd = ((Number*)_stack->popValue())->GetInteger();
	int iStep = 1;
	if (ins->param.counter.counter1 > 0)  {
		iStep = ((Number*)_stack->popValue())->GetInteger();
	}
	InstructionValue* block = (InstructionValue*)ins->param.value;
	block->setFor(true);

	auto xInc = [](int i, int end) {return i <= end; };
	auto xDec = [](int i, int end) {return i >= end; };
	typedef bool(*Cmp)(int, int);
	Cmp xCmp = xInc;
	if (iStep < 0) {
		xCmp = xDec;
	}
	for (int i = iStart; xCmp(i, iEnd); i += iStep)  {
		numStart->SetNumber(i);                    //�ı��ڲ���i��ֵ
		top->Assign(valStart, numStart);
		runBlockCode(block);
		if (block->getBreaked())  {
			break;
		}
	}
}

void VM::genericFor(Instruction* ins)
{
	Value* ctrlKey = nullptr;
	std::vector<Value*> vtKeys;
	int num = ins->param.counter.counter1;
	for (int i = 0; i < num; i++)  {
		vtKeys.push_back(_stack->popValue());
	}
	ctrlKey = vtKeys[vtKeys.size() - 1];
	while (_stack->Size() > 3)  {      //ֻҪ��������
		_stack->popValue();
	}
	Value* ctrlVal = _stack->popValue();
	Value* data    = _stack->popValue();
	Value* func = _stack->popValue();
	
	do  {
		_stack->Push(data);       //��һ������
		_stack->Push(ctrlVal);    //�ڶ�������
		_stack->Push(func);       //��������

		Instruction insCall;
		insCall.param.counter.counter1 = 2;
		insCall.param.counter.counter2 = num;
		call(&insCall);

		for (int i = num - 1; i >=0; i--)  {
			_stack->Push(vtKeys[i]);
		}

		assignVals(num, num, 0);

		ctrlVal = getCurrentClosure()->getTopTable()->GetValue(ctrlKey);
		if (ctrlVal->Type() == Value::TYPE_NIL)  {  //����ֵΪnil
			break;
		}
		InstructionValue* block = (InstructionValue*)ins->param.value;
		block->setFor(true);
		runBlockCode(ins->param.value);
	} while (true);
}

void VM::breakFor(Instruction* ins)
{

}

void VM::enterBlock(Instruction* ins)
{
	getCurrentClosure()->addBlockTable();
}

void VM::quitBlock(Instruction* ins)
{
	getCurrentClosure()->removeBlockTable();
}

void VM::runBlockCode(Value* val)
{
	if (val)  {
		assert(val->Type() == Value::TYPE_INSTRUCTVAL);
		getCurrentClosure()->addBlockTable();
		int ret = runCode((InstructionValue*)val);
		getCurrentClosure()->removeBlockTable();
		if (ret == -1)  {
			((InstructionValue*)val)->setBreaked(true);
			InstructionValue* p = (InstructionValue*)val;
			while (p)  {
				p = p->getParent();
				if (p)  {
					p->setBreaked(true);
					if (p->getFor())  {
						break;
					}
				}
			}
			if (!p)  {    //�ҵ�ͷ�˶�û���ҵ�ѭ��
				printf("run error, break in no loop block!!!\n");
			}
		}
	}
}

void VM::generateBlock(Instruction* ins)
{
	Value* val = ins->param.value;
	_stack->Push(val);
}

void VM::tableDefine(Instruction* ins)
{
	Table* tab = new Table();
	for (int i = 0; i < ins->param.counter.counter1; i++)  {
		Value* key = _stack->popValue();
		Value* val = _stack->popValue();
		tab->Assign(key, val);
	}
	_stack->Push(tab);
}

void VM::tableArrIndex(Instruction* ins)
{

}

void VM::tableAccess(Instruction* ins)
{
	Value* tabName = _stack->popValue();
	Value* member = _stack->popValue();
	Value* tab = nullptr;
	Value* val = nullptr;
	std::string stFiled = ((String*)(ins->param.value))->Get();

	if (tabName->Type() == Value::TYPE_TABLE)  {    //a.b.c ��ôa.b����table
		val = ((Table*)tabName)->GetValue(member);
	}
	else if (tabName->Type() == Value::TYPE_NUMBER)  {
		printf("attempt to index a number val (filed \'%s\') \n", stFiled.c_str());
	}
	else  {
		if (getCurrentClosure()->findUpTables(tabName, &tab, nullptr) != -1)  {
			if (tab->Type() != Value::TYPE_TABLE)  {
				printf("%s is not a table\n", ((String*)tabName)->Get().c_str());
			}
			val = ((Table*)tab)->GetValue(member);
			if (!val)  {
				((Table*)tab)->Assign(member, new Nil());
				val = ((Table*)tab)->GetTableValue(member);   //����table��ֵ t.a = 1
			}
		}
		else  {
			if (tabName->Type() == Value::TYPE_NUMBER)  {
				printf("attempt to index a number val (filed \'%s\') \n", stFiled.c_str());
			}
			else if (tabName->Type() == Value::TYPE_NIL)  {
				printf("attempt to index a nil val (filed \'%s\') \n", stFiled.c_str());
			}
		}
	}
	if (!val)  {       //a.b �Ҳ���a��b����Ϊ��
		val = new Nil();
	}
	
	_stack->Push(val);
}

void VM::negNumber(Instruction* ins)
{
	Number* val = (Number*)_stack->popValue();
	double num = val->Get() * -1;
	val->SetNumber(num);
	_stack->Push(val);
}

void VM::lenOfVale(Instruction* ins)
{
	BaseLib::len(_state, nullptr);	
}