
//###Engine Include
#include "Blueprint/UserWidget.h"
#include "Chaos/ChaosPerfTest.h"
#include "Logging/LogVerbosity.h"
#include "Misc/FileHelper.h"
#include "CoreTypes.h"

//###Plugins Include
#include "LuaCore.h"
#include "UnLua.h"
#include "UnLuaEx.h"
// lua 头文件顺序不能出错，会导致依赖错误，
// 进而导致找不到指定的库？ 头文件顺序也很重要的。

//###Project Include
#include "BaseUserWidget.h"
#include "MyViewManager.h"

static bool SucCheckViewManager(UObject* WorldObject)
{
	UMyViewManager* ViewManager = UMyViewManager::Get(WorldObject);
	if (!ViewManager)
	{
		// 上报一次Lua堆栈
		// ReportSystem::ReportLuaStack(WorldObject ? *GetNameSafe(WorldObject) : TEXT("NullObj"));
		return false;
	}
	return true;
}

// 打开PageForLua para1:WorldObject为当前的Widget para2:UIName para3:NewInstance para4:luatable
static int32 Bridge_OpenPage(lua_State* L)
{
	int ParamsCount = lua_gettop(L);
	if (ParamsCount < 2)
	{
		return 0;
	}
	auto* WorldObject = UnLua::GetUObject(L, 1);
	if (!SucCheckViewManager(WorldObject))
	{
		return 0;
	}
	if (!IsValid(WorldObject))
	{
		UE_LOG(LogTemp, Error, TEXT("[LuaLib_Bridge.Bridge_OpenPage] WorldObject is not valide"));
		return 0;
	}
	const char* PageNameAnsi = luaL_checkstring(L, 2);
	const TCHAR* PageName = ANSI_TO_TCHAR(PageNameAnsi);
	bool bNewInstance = false; 
	if (ParamsCount > 2 && !lua_isnil(L, 3))
	{
		bNewInstance = lua_toboolean(L, 3) != 0;
	}
	
	UPageOpenData* OpenData = nullptr;
	int OpenDataRef = -1;
	if (ParamsCount > 3 && !lua_isnil(L, 4))
	{		
		lua_pushvalue(L, 4);
		OpenDataRef = luaL_ref(L, LUA_REGISTRYINDEX);
		OpenData = NewObject<UPageOpenData>();
		OpenData->LuaOpenDataRef = OpenDataRef;
	}
	if (const auto ViewManager = UMyViewManager::Get(WorldObject))
	{
		ViewManager->OpenPage(FName(PageName), bNewInstance, OpenData);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[LuaLib_Bridge.Bridge_OpenPage] Get ViewManager failed from %s"), *WorldObject->GetName());
	}
	return 0;
}

// 关闭Page para1:WorldObject一般为Widget para2: PageName
static int32 Bridge_ClosePage(lua_State* L)
{
	int ParamsCount = lua_gettop(L);
	if (ParamsCount < 2)
	{
		return 0;
	}
	auto* WorldObject = UnLua::GetUObject(L, 1);
	if (!SucCheckViewManager(WorldObject))
	{
		return 0;
	}
	UMyViewManager* ViewManager = UMyViewManager::Get(WorldObject);
	if (!ViewManager)
	{
		return 0;
	}
	if (!WorldObject && IsValid(WorldObject))
	{
		UE_LOG(LogTemp, Error, TEXT("[LuaLib_Bridge.Bridge_ClosePage] WorldObject is not valide"));
		return 0;
	}
	if (lua_type(L, 2) == LUA_TSTRING)
	{
		const char* UINameCStr = luaL_checkstring(L, 2);
		if (UINameCStr)
		{
			const TCHAR* TStr = ANSI_TO_TCHAR(UINameCStr);
			ViewManager->ClosePage(FName(TStr));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[LuaLib_Bridge.Bridge_ClosePage] At Wrong Time!"));
		}
	}
	else
	{
		// 第一个参数转成UserWidget
		auto* UserWidget = Cast<UBaseUserWidget>(WorldObject);
		if (!UserWidget)
		{
			UserWidget = Cast<UBaseUserWidget>(UnLua::GetUObject(L, 2));
		}
		if (UserWidget)
		{
			ViewManager->ClosePage(UserWidget->UIName);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[LuaLib_Bridge.Bridge_ClosePage] At Wrong Time Or Param Error"));
		}
	}
	return 0;
}

// 关闭Page para1:WorldObject一般为Widget para2: PageName
static int32 Bridge_HidePage(lua_State* L)
{
	int ParamsCount = lua_gettop(L);
	if (ParamsCount < 2)
	{
		return 0;
	}
	auto* WorldObject = UnLua::GetUObject(L, 1);
	if(!SucCheckViewManager(WorldObject))
	{
		return 0;
	}
	if (!WorldObject && IsValid(WorldObject))
	{
		UE_LOG(LogTemp, Error, TEXT("[LuaLib_Bridge.Bridge_HidePage] WorldObject is not valide"));
		return 0;
	}
	if (lua_type(L, 2) == LUA_TSTRING)
	{
		const char* UINameCStr = luaL_checkstring(L, 2);
		if (UINameCStr)
		{
			const TCHAR* TStr = ANSI_TO_TCHAR(UINameCStr);
			UMyViewManager::Get(WorldObject)->HidePage(FName(TStr));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[LuaLib_Bridge.Bridge_HidePage] At Wrong Time!"));
		}
	}
	return 0;
}


// 获取Ref并返回，同时unref
// Usage1：获取Lua的OpenData，可为任意值
static int32 Bridge_Unref(lua_State* L)
{
	int Pos = lua_tointeger(L, 1);
	if (Pos > 0)
	{
		lua_geti(L, LUA_REGISTRYINDEX, Pos);
		luaL_unref(L, LUA_REGISTRYINDEX, Pos);
		return 1;
	}
	return 0;
}

static int32 Bridge_GetUniqIdByPointer(lua_State* L)
{
	int ParamCount = lua_gettop(L);
	if (ParamCount > 0)
	{
		const void* Ptr = lua_topointer(L, 1);
		FString PtrStr = FString::Printf(TEXT("%p"), Ptr);
		UnLua::Push(L, PtrStr);
		return 1;
	}
	return 0;
}


static const luaL_Reg LuaBridgeLib[] =
{
    // { "LoadPbFile", Bridge_LoadPbFile},
    { "LuaOpenPage", Bridge_OpenPage},
    { "LuaClosePage", Bridge_ClosePage},
    { "LuaHidePage", Bridge_HidePage},
	{ "LuaGetRefObject", Bridge_Unref},
    { "LuaGetUniqIdByPointer", Bridge_GetUniqIdByPointer},
    { nullptr, nullptr }
};

EXPORT_UNTYPED_CLASS(LuaBridge, false, LuaBridgeLib)
IMPLEMENT_EXPORTED_CLASS(LuaBridge)
