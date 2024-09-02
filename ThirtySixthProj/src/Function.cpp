#include "Function.h"
#include "Stack.h"
#include "State.h"




InstructionValue::InstructionValue():
_parentInsVal(nullptr), _insSet(nullptr),
_hasFor(false), _breaked(false)
{

}


void InstructionValue::clearInsSet()
{
	if (_insSet)  {
		_insSet->clearInstructions();
	}
}

Function::Function()
{
}


Function::~Function()
{
}




Closure* Function::generateClosure(State* s)
{
	Closure* cl = new Closure(s);
	cl->setPrototype(this);
	return cl;
}





Closure::Closure(State* s)
	:_state(s), _actParamNum(0),
	_actRetNum(0), _needRetNum(0),
	_prototype(nullptr),
	_parentClosure(nullptr),
	_upTables(nullptr)
{

}

void Closure::initClosure()
{
	clearClosure();
	_nest_tables.push_back(new Table());
}

void Closure::clearClosure()
{
	_nest_tables.clear();
}

void Closure::balanceStack()
{
	if (_needRetNum != -1)  {                  //Ϊ-1ʱ����ʾ�޷�������Ҫ�ķ���ֵ��������f1(f2())
		int n = _actRetNum - _needRetNum;      //��ʵ����ֵ����-��Ҫ����ֵ����
		if (n > 0)  {                           //f() + 2,���f()��ʵ������2������������ֻ��Ҫһ������ջҪ����һ��
			while (n > 0)  {
				_state->getStack()->popValue();
				n--;
			}
		}
		else  {
			while (n < 0)  {                    //f() + 2, ���f()��ʵ����0��������������Ҫһ������Ҫѹ��ջһ����ò�Ʋ�ѹҲ�У���Ϊ���渳ֵʱ��ƽ�⣩
				_state->getStack()->Push(new Nil());
				n++;
			}
		}
	}
	
	_actRetNum = 0;
	_needRetNum = 0;
}

void Closure::addBlockTable()
{
	_nest_tables.push_back(new Table());
}

void Closure::removeBlockTable()
{
	Table* top = getTopTable();
	if (top)  {
		delete top;
	}
	_nest_tables.pop_back();
}

Closure* Closure::clone()
{
	Closure* cl = new Closure(_state);
	cl->_needRetNum = _needRetNum;
	cl->_actParamNum = _actRetNum;
	cl->_actRetNum = _actRetNum;
	cl->_parentClosure = _parentClosure;
	cl->_prototype = _prototype;
	if (_upTables)  {
		cl->_upTables = _upTables->clone();
	}
	return cl;
}

Table* Closure::getTopTable()
{
	return getLevelTable(_nest_tables.size() - 1);
}

Table* Closure::getLevelTable(unsigned int i)
{
	if (_nest_tables.size() == 0  || i >= _nest_tables.size())  {
		return nullptr;
	}
	return _nest_tables[i];
}

void Closure::setParentClosure(Closure* c)
{
	_parentClosure = c;
	if (c)  {
		Table* topTab = c->getTopTable();       //ֻ�追�������ľֲ�����,forѭ������ľֲ��������Ӻ����ǲ��ɼ���
		if (topTab)  {
			_upTables = topTab->clone();
		}
	}
}


int Closure::findInNestTables(Value* key, Value** val)
{
	int num = _nest_tables.size();
	Value* temp = nullptr;
	for (int i = num - 1; i >= 0; i--)  {
		temp = _nest_tables[i]->GetValue(key);
		if (temp)  {
			if (val) *val = temp;
			return i;
		}
	}
	return -1;
}

int Closure::findUpTables(Value* key, Value** val, Table** tab)
{
	Closure* cl = this;
	while (cl)  {
		int level = cl->findInNestTables(key, val);
		if (level != -1)  {  //�ڵ�ǰ�հ����ҵ�
			if (tab) *tab = cl->getLevelTable(level);
			return 1;
		}
		cl = cl->_parentClosure;
	}

	if (_upTables)  {      //������ֵ����
		Value* temp = _upTables->GetValue(key);
		if (temp) { 
			if (val) *val = temp;
			if (tab) *tab = _upTables;
			return 2;
		}
	}

	Table* table = getState()->getGlobalTable();  //�ѵ������
	Value* temp = table->GetValue(key);
	if (temp)  {
		if (val) *val = temp;
		if (tab) *tab = table;
		return 0;
	}
	else  {
		return -1;
	}
	return -1;
}