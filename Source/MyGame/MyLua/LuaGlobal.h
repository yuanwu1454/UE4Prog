#pragma once
// 定义全局命名空间：luaGlobal

#include "UnLuaEx.h"
#include "UnLuaLegacy.h"

namespace LuaGlobal
{


	// 去掉 std::forward，纯 UE4 写法
	template<typename T>
	void PushParam(lua_State* L, int32& Count, T Param)
	{
		UnLua::Push(L, Param, false);
		Count++;
	}

	template<typename T, typename... Args>
	void PushParams(lua_State* L, int32& Count, T First, Args... Rest)
	{
		PushParam(L, Count, First);
		PushParams(L, Count, Rest...);
	}

	inline void PushParams(lua_State* L, int32& Count)
	{
	}

	//=============================================
	// 【无返回值，只管调用】
	//=============================================
	template<typename... Args>
	bool CallVoidLua(FString FuncName, Args... args)
	{
		lua_State* L = UnLua::GetState();
		if (!L) return false;

		// 1. 压入错误处理函数
		lua_pushcfunction(L, UnLua::ReportLuaCallError);
		int ErrFuncIdx = lua_gettop(L); // 记录绝对位置

		// 2. 查找Lua函数
		lua_getglobal(L, TCHAR_TO_UTF8(*FuncName));
		if (!lua_isfunction(L, -1))
		{
			UE_LOG(LogTemp, Error, TEXT("Lua函数不存在: %s"), *FuncName);
			lua_pop(L, 1);
			lua_pop(L, 1); // 清理错误函数
			return false;
		}

		// 3. 压参
		int32 ParamCount = 0;
		PushParams(L, ParamCount, args...);

		// 4. 调用（关键：必须用绝对索引，不能用-2）
		bool bSuccess = lua_pcall(L, ParamCount, 0, ErrFuncIdx) == 0;

		// 5. 清理错误处理函数
		lua_pop(L, 1);

		return bSuccess;
	}

	//=============================================
	// 【带返回值，自动类型】
	//=============================================
	template<typename RetType, typename... Args>
	RetType CallLua(FString FuncName, Args... args)
	{
		lua_State* L = UnLua::GetState();
		if (!L)
			return RetType();

		lua_pushcfunction(L, UnLua::ReportLuaCallError);
		lua_getglobal(L, TCHAR_TO_UTF8(*FuncName));

		if (!lua_isfunction(L, -1))
		{
			UE_LOG(LogTemp, Error, TEXT("Lua函数不存在: %s"), *FuncName);
			lua_pop(L, 1);
			return RetType();
		}

		int32 ParamCount = 0;
		PushParams(L, ParamCount, args...);

		if (lua_pcall(L, ParamCount, 1, -2) != 0)
		{
			UE_LOG(LogTemp, Error, TEXT("Lua执行失败: %s"), UTF8_TO_TCHAR(lua_tostring(L, -1)));
			lua_pop(L, 1);
			return RetType();
		}
		
		// =============================================
		// ✅ 只需要这一行！UnLua 原生自动处理所有类型！
		// =============================================
		// 为啥不能使用这个？ int32 明明有特化，却走了 GetPointer，是 UnLua 模板优先级的 BUG！
		// 解决方案：手动判断基础类型，绕开 UnLua 模板问题！
		// 	C++ 模板规则：
		// 如果通用模板能完全匹配，它就不会去找具体类型特化！
		// 所以 int32 明明有特化，还是被通用模板抢走了！

		// 1. 函数模板（最坑）
		// 函数模板的特化，优先级低于普通函数，也低于主模板精确匹配
		// 2. 类模板（规则更清晰）
		// 类模板只看：精确匹配主模板 → 不匹配才找特化
		// RetType Result = UnLua::Get<RetType>(L, -1, UnLua::TType<RetType>());


		RetType Result;
		// 你用的是 运行时 if，但模板是 编译期决议。
		// 也就是说：
		// 当你调用 CallLua<int32>() 时
		// 所有 else if 分支依然会参与编译！
		// 包括 Result = FString(...) 这一行
		// 于是编译器试图把 FString 赋值给 int32 → 直接报错 C2440
		if constexpr (UnLua::TIsPrimitiveType<RetType>::Value)
		{
			Result = UnLua::Get(L, -1,  UnLua::TType<RetType>());			
		}else
		{
			Result = UnLua::Get<RetType>(L, -1, UnLua::TType<RetType>());
		}

		lua_pop(L, 2);
		return Result;
	}
}
