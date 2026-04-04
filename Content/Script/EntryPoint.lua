
-- if UE4.UAkGameplayStatics.IsEditor() then
    require("debug/LuaPanda").start("127.0.0.1",8818)
-- end

print("EntryPoint")
require("Base.Init")
require("Util.Init")
require("Config.Init")
require("Resource.Init")
require("framework.Init")
require("Global.Init")
require("Business.Init")