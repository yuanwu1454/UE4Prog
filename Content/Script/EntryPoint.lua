
-- if UE4.UAkGameplayStatics.IsEditor() then
    require("debug/LuaPanda").start("127.0.0.1",8818)
-- end
print("entrypoint")

function GlobalFunc()
    ViewMgr:OpenPage(LuaGetWorld(), "SettingPage", nil, {a = 1,b = 2})
end
function NDCall(...)
    print("NDCall xcvxcv")
    local args = {...}
    local num = #args
    local NDName = args[1]
    table.remove(args, 1)
    
    print("NDCall xcvxcv", NDName)
    print("NDCall xcvxcv", ND.ManaNotify)
    print("NDCall xcvxcv", ND[NDName])    
    GameFacade:SendNotification(ND[NDName], args)
end


function ShowCommonTip()
    print("ShowCommontip")
end

function GetNumber()
    print("GetNumber")
    return 5
end

function AddNumber(a, b)
    print("AddNumber")
    return a+b
end

print("EntryPoint")
require("Base.Init")
require("Util.Init")
require("Config.Init")
require("Resource.Init")
require("framework.Init")
require("Global.Init")
require("Business.Init")