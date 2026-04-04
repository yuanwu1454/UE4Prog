
local ModuleInit					= class("ModuleInit")


--[[
	{
		Name = "Name",
		Path = "Path",
	},
--]]
ModuleInit.Proxys = {}
ModuleInit.Commands = {}

local PrivateInitCommandsFunc = function(dirName, self)
	if self.CommandsRequireParentPath == nil then
		return 
	end

	self.privateCommands = self.privateCommands or {}

	if #self.privateCommands > 0 then
		-- 已经加入了 就不用重复加入了
		return
	end

	local CommandMap = {}
	for i, v in ipairs(self.Commands) do
		CommandMap[v.Name] = true
	end

	local fileparent = UE4.UBlueprintPathsLibrary.ProjectContentDir()
    local realDir = fileparent .. "/Script/" .. dirName

    local bExists, Arr      = UE4.UFlibAssetManageHelper.FindFilesRecursive(realDir)
    local CommandsFileArr = {}
    if bExists then
        for i = 1, Arr:Length() do
            local value = Arr:Get(i)
            local s, e = string.find(value, dirName)
            local a = string.sub(value, s, -1)
            CommandsFileArr[#CommandsFileArr+1] = a
        end
    end

    for i, v in ipairs(CommandsFileArr) do
        local ext = UE4.UBlueprintPathsLibrary.GetExtension(v)
        local reqPath
        if ext == "lua" then
            local filename = UE4.UBlueprintPathsLibrary.GetBaseFilename(v)
            reqPath = string.sub(v, 1, -5)

			local cmd
            local bSuccess = xpcall(function() 
				cmd = require(reqPath)
			end, function (err)
				print(err)
			end)

			if bSuccess then
				local value = {
					Name = filename,
					Path = reqPath
				}

				if cmd.NotifyName then
					value.Name = cmd.NotifyName
				end

				NotificationDefines[#NotificationDefines + 1] = cmd.NotifyName
				if CommandMap[value.Name] == nil then
					self.Commands[#self.Commands+1] = value
					CommandMap[value.Name] = true
				else
					LogInfo(" ModuleInit:Init-Proxys ", "Commands " .. tostring(value.Name) .. " is alerady exist, dont repeat add");
				end
			else
				LogError("ModuleInit:Init-Proxys", "require %s is error", reqPath)
			end
        end
    end
end


function ModuleInit:Init()
	local GameFacade = GameFacade
	if not GameFacade then
		return
	end

	local bSuccess, errorMsg, ClassObject

	for k, v in pairs(self.Proxys) do
		if v.Path then
			bSuccess, errorMsg = pcall(require, v.Path)
			if bSuccess then
				ClassObject = require(v.Path)
			else
				LogError("ModuleInit:Init-Proxys", "File require error !!! \n Error = %s", errorMsg)
			end				
		end
		if ClassObject then
			LogInfo(" ModuleInit ", tostring(ClassObject.ProxyName ));
		end
		if v.Name == nil and ClassObject and ClassObject.ProxyName then
			v.Name = ClassObject.ProxyName
		end 

		if v.Path and v.Name then
            local obj =  ClassObject.new(v.Name)
            print("obj", tostring(obj), ClassObject, obj.initializeNotifier)
			GameFacade:RegisterProxy(obj)
		else
			LogError("ModuleInit:Init-Proxys", "Name Or Path not config !!! Index=%s, Name=%s, Path=%s", k, v.Name, v.Path)
		end
	end

	-- PrivateInitCommandsFunc(self.CommandsRequireParentPath, self)
	
	for k, v in pairs(self.Commands) do
		if v.Path then
			bSuccess, errorMsg = pcall(require, v.Path)
			if bSuccess then
				ClassObject = require(v.Path)
			else
				LogError("ModuleInit:Init-Commands", "File require error !!! \n Error = %s", errorMsg)
			end				
		end

		if v.Name == nil and ClassObject.NotifyName then
			v.Name = ClassObject.NotifyName
		end 

		if v.Path and v.Name then
			GameFacade:RegisterCommand(v.Name, ClassObject)
		else
			LogError("ModuleInit:Init-Commands", "Name Or Path not config !!! Index=%s, Name=%s, Path=%s", k, v.Name, v.Path)
		end
	end
end

function ModuleInit:Clear()
	LogInfo(" ModuleInit ", "Clear");
	
	local GameFacade = GameFacade
	if not GameFacade then
		return
	end
	
	local err_handle = function(err, name, path)
		LogError("ModuleInit:Clear-Commands", "Name=%s, Path=%s, error is %s",name, path, err)
	end

	for i, v in pairs(self.Proxys) do
		if v.Name and v.Path then
			xpcall(function ()
				GameFacade:RemoveProxy(v.Name)
			end, function (err)
				err_handle(err, v.Name, v.Path)
			end)
		end
	end

	
	for i, v in pairs(self.Commands) do
		if v.Name and v.Path then
			GameFacade:RemoveCommand(v.Name)
		end
	end
end

return ModuleInit