#pragma once
#include "DirectX.h"

//ħ����һ��С������
struct Cube
{
	Vetrex v[24];	//0-3���棬4-7���棬8-11���棬12-15���棬16-19���棬20-23����
	LPDIRECT3DTEXTURE9 top, bottom, left, right, front, rear;	//�����������
	bool rotating;	//���������Ƿ�����ת
};

//ħ��
struct Rubik
{
	Cube cube[3][3][3];

	POINT position[3][4][4];	//0Ϊ���� 1Ϊ���� 2Ϊ����
	int degree;			//��ת�ĽǶ�
	int shuffle_times;	//��ʼ��ʱ���Ҵ���
	bool YZ, XZ, XY;	//��ת��������
	bool complete;		//ħ���Ƿ����

	void reset();			//����
	void update();			//����
	void show();			//��ʾ
	void check_complete(HWND window);		//����Ƿ����

	void rotate_YZ(int index, bool inverse);	//YZ����ת��indexΪ0ʱת�����棬1ת���м䣬2ת������
	void rotate_XZ(int index, bool inverse);	//XZ����ת��indexΪ0ʱת�����棬1ת���м䣬2ת������
	void rotate_XY(int index, bool inverse);	//YZ����ת��indexΪ0ʱת�����棬1ת���м䣬2ת������

	
	void view_rotate_X(bool inverse);			//��X����ת
	void view_rotate_Y(bool inverse);			//��Y����ת
	void view_rotate_Z(bool inverse);			//��Z����ת

	Rubik();	//�����ӿڶ�������

};



