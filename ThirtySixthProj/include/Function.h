#pragma once
#include <vector>
#include "Instruction.h"
#include "Value.h"


class State;
class Closure;
class Stack;


class InstructionValue : public Value
{
public:
	InstructionValue();
	InstructionValue(InstructionSet* is) : InstructionValue() { _insSet = is; }

	virtual std::string Name() const { return "instruction_val"; }
	virtual int Type() const { return TYPE_INSTRUCTVAL; }
	virtual std::size_t GetHash() const  { return std::hash<const InstructionValue *>()(this); }
	virtual bool IsEqual(const Value *other) const  { return this == other; }


public:
	InstructionSet* getInstructionSet() { return _insSet; }
	void setInstructionSet(InstructionSet* s){ _insSet = s; }
	void clearInsSet();
	void setParent(InstructionValue* v) { _parentInsVal = v; }
	InstructionValue* getParent() { return _parentInsVal; }

	void setFor(bool b) { _hasFor = b; }
	bool getFor() { return _hasFor; }
	void setBreaked(bool b)  { _breaked = b; }
	bool getBreaked() { return _breaked; }
	
private:
	InstructionSet* _insSet;
	InstructionValue* _parentInsVal;
	bool _hasFor;
	bool _breaked;
};




class Function : public InstructionValue
{
public:
	Function();
	~Function();

public:
	virtual std::string Name() const { return "function"; }
	virtual int Type() const { return TYPE_FUNCTION; }
	virtual std::size_t GetHash() const  {  return std::hash<const Function *>()(this);}
	virtual bool IsEqual(const Value *other) const  {  return this == other; }

	Closure* generateClosure(State* s);

};

class Closure  : public Value
{
public:
	Closure(State*);

	Function * getPrototype() const { return _prototype; }
	void setPrototype(Function *prototype) { _prototype = prototype; }
	State* getState() { return _state; }

	virtual std::string Name() const { return "closure"; }
	virtual int Type() const { return TYPE_CLOSURE; }
	virtual std::size_t GetHash() const  { return _prototype->GetHash(); }
	virtual bool IsEqual(const Value *other) const  { return this == other; }

	void setParentClosure(Closure* c);
	Closure* getParentClosure()  { return _parentClosure; }
	Table* getTopTable();
	Table* getLevelTable(unsigned int i);
	int findUpTables(Value* key, Value** val, Table** table);
	void initClosure();
	void clearClosure();
	void balanceStack();

	Closure* clone();

	void addBlockTable();
	void removeBlockTable();

	void setActRetNum(int n) { _actRetNum = n; }
	int getRealRetNum()  { return _actRetNum; }
	void setNeedRetNum(int n)  { _needRetNum = n; }
	int getNeedRetNum()  { return _needRetNum; }
	void setActParamNum(int n)  { _actParamNum = n; }
	int getActParamNum()  { return _actParamNum; }

private:
	int findInNestTables(Value* key, Value** val);
private:
	int _needRetNum;      //������������Ҫ�ķ���ֵ����������f()Ϊ0����f() + 1 ��Ϊ1��
	int _actRetNum;       //����ִ��ʱȷ���ķ���ֵ���������Ҳ�Ƕ�̬�ģ�������if�����
	int _actParamNum;     //����ִ��ʱʵ�ʴ���Ĳ�������
	State* _state;
	Function* _prototype;
	Closure* _parentClosure;

	typedef std::vector<Table *> NestTables;
	NestTables _nest_tables;     //һ�������п����ж��Ƕ��block
	Table* _upTables;
};


class NativeFunc : public Value
{
public:
	typedef int(*Fun)(State*, void*);

	NativeFunc(Fun f) { _func = f; }
	virtual std::string Name() const { return "native_function"; }
	virtual int Type() const { return TYPE_NATIVE_FUNCTION; }
	virtual std::size_t GetHash() const  { return std::hash<const NativeFunc *>()(this); }
	virtual bool IsEqual(const Value *other) const  { return this == other; }

	int doCall(State* s, void* p) { return _func(s, p); }

private:
	Fun _func;

};

