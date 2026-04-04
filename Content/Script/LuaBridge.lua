require "UnLua"

-- local OnLuaBridgePostInit = _G.OnLuaBridgePostInit
-- local TickGlobal = _G.TickGlobal
-- local LogDebug = _G.LogDebug
-- local LogError = _G.LogError
-- 所有需要同C++交互的事件在这里配置
-- LuaBridge的某些行为被重写需要，比如__eq，使用起来要非常慎重
local LuaBridge = Class()

-- 需要第一时间自动注册的事件代理
local autoRegistDelegates = {}

-- 委托绑定统计
local Delegates = {}

-- local function PrintDelegatesNum()
--     local count = 0
--     for key, value in pairs(Delegates) do
--         count = count + value
--     end
--     UE4.UKismetSystemLibrary.PrintString(LuaGetWorld(),"//DelegateMgr-当前委托数量 = " .. count,true,false,UE4.FLinearColor(0,1,0,1),5)
-- end

-- local function CheckDelegate(delegate, num)
--     if Delegates[delegate] and num > 0 then
--         LogDebug('ShowLuaCallDelegateCnt', '//委托重复绑定！！！')
--         return
--     end

--     if not Delegates[delegate] and num < 0 then
--         LogDebug('ShowLuaCallDelegateCnt', '//委托已经解绑！！！')
--         return
--     end

--     if Delegates[delegate] then
--         Delegates[delegate] = nil
--         LogDebug('ShowLuaCallDelegateCnt', '//委托 -1')
--     else
--         Delegates[delegate] = num
--         LogDebug('ShowLuaCallDelegateCnt', '//委托 +1')
--     end

--     --PrintDelegatesNum()
-- end

-- local function CheckMulticastDelegate(delegate, num)
--     if not Delegates[delegate] and num <= 0 then
--         LogDebug('lua', '//多播委托已经解绑！！！')
--         return
--     end

--     if Delegates[delegate]  then
--         Delegates[delegate] = Delegates[delegate] + num
--     else
--         Delegates[delegate] = num
--     end

--     if num == 0 or Delegates[delegate] == 0 then
--         Delegates[delegate] = nil
--     end

--     local str = (num == 0 and '清零' or string.format("%+d", num)) --//-code parse
--     LogDebug('ShowLuaCallDelegateCnt', '//多播委托 %s', str)

--     --PrintDelegatesNum()
-- end

-- function LuaBridge:ShowDelegates()
--     PrintDelegatesNum()
-- end


function LuaBridge:Initialize()
    print("LuaBridge:Initialize()")
    _G.g_LuaBridgeObject = self
    GameFacade:SetupGameProxy()
    GameFacade:Setup()
end

-- 转换绑定方法的名称
local function GetBindFuncName(ins, funcKey)
    return ins.__cname .. '_' .. funcKey -- function name
end

-- 默认的clazz引用了默认的绑定对象，运行时可以将绑定对象换成其它
local function ReplaceUpvalue(func, ins)
    local idx = 1
    repeat
        local name, value = debug.getupvalue(func, idx)
        if (ins.__cname == value.__cname) then
            LogDebug('lua', 'auto replace binded func upvalue %s clazz to %s', tostring(value), tostring(ins))
            debug.setupvalue(func, idx, ins)
        end
        idx = idx + 1
    until(value == nil)
end

function LuaBridge:TickLuaBridge(deltaTime)
    TickGlobal(deltaTime)
end

--[[
    可以传不同类型参数
    obj: table funcName :string  则function=obj[funcName]
    obj: function funcName: Nil or string 
    obj: table funcName: function
]]
function LuaBridge:CreateWrapDelegateFunc(obj, funcName)
    -- 用于获取全局唯一的名称
    -- obj为function funcName:为string类型的名字
    local registName, func, contextObj
    if (type(obj) == 'function') then
        registName = string.format('%s_%s', (obj.__cname or self:GetTempName('o')), (funcName or '0'))
        func = obj
    elseif (type(obj) == 'table') then
        if (type(funcName) == 'function') then
            func = funcName
            registName = string.format('%s_%s', (obj.__cname or self:GetTempName('o')), self:GetTempName('f'))
        else
            local f = obj[funcName]
            if (type(f) == 'function') then
                func = f
                registName = string.format('%s_%s', (obj.__cname or self:GetTempName('o')), funcName)
            end
        end
        contextObj = obj
    end
    -- 参数错误
    if (not func or not registName) then
        LogError('LuaBridge', 'can\'t wrap for delegate with params %s.%s ', tostring(obj), tostring(funcName))
        return
    end

    -- 第一个参数必然为LuaBridge对象，需要改为相应upvalue（对象实际对象的值），如果没有ctx则只传参数
    local newFunc = function(_, ...)
        if (contextObj) then
            func(contextObj, ...)
        else
            func(...)
        end
    end
    return newFunc
end

-- ------------------ 管理全局及单局的委托代理------------------------
function LuaBridge:BindDelegate(dynamicDelegate, obj, funcName)
    local handleFunc = self:CreateWrapDelegateFunc(obj, funcName)
    dynamicDelegate:Bind(self, handleFunc)

    -- if WITH_EDITOR then
    --     CheckDelegate(dynamicDelegate, 1);
    -- end

    return handleFunc
end

function LuaBridge:UnbindDelegate(dynamicDelegate, handle)
    dynamicDelegate:Unbind()

    -- if WITH_EDITOR then
    --     CheckDelegate(dynamicDelegate, -1);
    -- end
end

function LuaBridge:AddDelegate(dynamicMulticastDelegate, obj, funcName)
    local handleFunc = self:CreateWrapDelegateFunc(obj, funcName)
    dynamicMulticastDelegate:Add(self, handleFunc)

    -- if WITH_EDITOR then
    --     CheckMulticastDelegate(dynamicMulticastDelegate, 1);
    -- end

    return handleFunc
end

function LuaBridge:RemoveDelegate(dynamicMulticastDelegate, handle)
    dynamicMulticastDelegate:Remove(self, handle)

    -- if WITH_EDITOR then
    --     CheckMulticastDelegate(dynamicMulticastDelegate, -1);
    -- end
end

function LuaBridge:ClearDelegate(dynamicMulticastDelegate)
    dynamicMulticastDelegate:Clear()

    -- if WITH_EDITOR then
    --     CheckMulticastDelegate(dynamicMulticastDelegate, 0);
    -- end
end

local function GetBridgeObject()
    if (not _G.g_LuaBridgeObject) then
        LogError('DelegateMgr', 'LuaBridgeObject havent initailized')
    end
    return _G.g_LuaBridgeObject
end

_G.GetBridgeObject = GetBridgeObject

return LuaBridge
