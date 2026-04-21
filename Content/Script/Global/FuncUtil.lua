
local FuncUtil = {}
function FuncUtil:CombinVec3(x, y, z)
    local vec3
    if x and not y and not z then
        vec3 = x
    else
        vec3 = UE4.FVector(x, y, z)
    end
    return vec3
end

function FuncUtil:PrintArray(arr)
    print(arr:Length())
    for i = 1, arr:Length() do
        print(arr:Get(i))
    end
end

function FuncUtil:GetObjectName(object)
    if object and object.GetName and object.IsValid and object:IsValid() then
        return object:GetName()
    end
    return tostring(object)
end

function FuncUtil:GetKeyName(key)
    return UE4.UKismetInputLibrary.Key_GetDisplayName(key)
end

function FuncUtil:GetClassByObjPath(objPath)
    return tostring(objPath).. "_C"
end

local function hasSuffix_C(str)
    return string.sub(str, -2) == "_C"
end
function FuncUtil:LoadClass(classPath)
    if not hasSuffix_C(classPath) then
        classPath = classPath.. "_C"
    end
    local class = UE4.UClass.Load(classPath)
    return class
end

function FuncUtil:FindObject(classPath, world)
    local class = FuncUtil:LoadClass(classPath)
    local Arr = UE4.UGameplayStatics.GetAllActorsOfClass(world, class)
    if Arr:Length() > 0 then
        local actor = Arr:Get(1)
        if actor then
            return actor
        end
    end
end

_G.FuncUtil = FuncUtil