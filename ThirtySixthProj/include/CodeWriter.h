#pragma once
#include "Value.h"
#include <string>
#include "Instruction.h"


class InstructionValue;

class CodeWrite  {
public:
	CodeWrite();
	~CodeWrite();
	Instruction* newInstruction();

	void* paramRW;        //�ɶ�д����
	void* paramFunc;      //�������ظ������ã���Щ�ط���Ҫ����ֵ1��n������Щ�ط�����
	void* paramArrInd;    //���index

	InstructionSet* fetchInstructionSet();

	InstructionValue* fetchInstructionVal();

private:
	InstructionSet* _insSet;
};
