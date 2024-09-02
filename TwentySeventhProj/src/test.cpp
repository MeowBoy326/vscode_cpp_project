// test.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <cstdio>


int sample1 ( );
int sample2 ( );
int sample3 ( );
int sample4 ( );
int sample5 ( );
int sample6 ( );

#include <lua.hpp>
#include "lua_tinker.h"
// int _tmain ( int argc, _TCHAR* argv[] )
int main (void)
{
   lua_State* L = luaL_newstate();
   luaopen_base(L);
   lua_tinker::def(L,"sample1",sample1);
   lua_tinker::def(L,"sample2",sample2);
   lua_tinker::def(L,"sample3",sample3);
   lua_tinker::def(L,"sample4",sample4);
   lua_tinker::def(L,"sample5",sample5);
   lua_tinker::def(L,"sample6",sample6);

   // ����ȫ�ֱ���
   // ִ�нű�
   lua_tinker::dofile ( L, "samples.lua" );
   lua_tinker::call<int> ( L, "run" );

   getchar();

     // ����ȫ�ֱ���
   // ִ�нű�
   lua_tinker::dofile ( L, "sample1.lua" );
   getchar();
   lua_tinker::dofile ( L, "sample2.lua" );
   getchar();
   lua_tinker::dofile ( L, "sample3.lua" );
   getchar();
   lua_tinker::dofile ( L, "sample4.lua" );
   getchar();
   lua_tinker::dofile ( L, "sample5.lua" );
   lua_tinker::call<int> ( L, "test_error" );
   getchar();
   lua_tinker::dofile ( L, "sample6.lua" );
     lua_tinker::call<int> ( L, "ThreadTest" );
   getchar();

   lua_close ( L );

   return 0;
}

