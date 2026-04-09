local allProtoFiles = {
    -- "activity_cfg.proto",
    "test.pb",
    -- "dir_svr_cs.proto",
    -- "errcode_cs.proto",
    -- "ncmd_cs.proto",
    -- "ncmd_ds.proto",
    -- "s_dsa.proto",
    -- "public.proto",
}

local ProtobufMgr = {}

function ProtobufMgr:LoadPb(pbName)
    LogDebug('[LuaProto]', 'begin load %s', pbName)
    local load_path = string.format('LuaProto/%s', pbName)
    local data = UE4.LuaBridge.LoadPbFile(load_path)
    local b, i = pb.load(data)
    LogDebug('[LuaProto]', 'bool, int = %s %s', tostring(b), tostring(i))
end

function ProtobufMgr:LoadAllPb()
    LogDebug('[LuaProto]', 'begin load lua proto')
    for _, v in ipairs(allProtoFiles) do
        self:LoadPb(v)
    end
    LogDebug('[LuaProto]', 'finish load lua proto')
end



local structMeta = {}
structMeta.__index = structMeta
function structMeta.__call(cls, tempObj)
	local obj = tempObj or {}
	for _, field in ipairs(cls.__fields) do
		local key, val, ctor = field[1], field[2], field[3]
		if obj[key] == nil then
			if not val and ctor then
				ctor = Proto.Msg[ctor]
				val = ctor and ctor()
			end
			obj[key] = val
		end
	end
	return setmetatable(obj, cls)
end

---@return string bytes
function structMeta.Encode(self)
	return pb.encode(self.__pbName, self)
end

---@param bytes string
---@return table
function structMeta.Decode(self, bytes)
	return pb.decode(self.__pbName, bytes)
end

function structMeta.Send(self, responseHandleFunc, target, flag)
	if not self.__bindCmdID then
		LogError("ProtobufMgr", "Send proto request failed, %s is not a request", self.__pbName)
		return
	end
	local lobbyService = GetLobbyServiceHandle()
	if not lobbyService then return end
	local dataBytes = self:Encode()
	if not dataBytes then
		LogError("ProtobufMgr", "Send proto request failed, cannot encode [cmdID:%s]%s data", tostring(self.__bindCmdID), self.__pbName)
		return
	end
	local responseCmdID, funcSlot
	if responseHandleFunc then
		if self.__resCmdID then
			responseCmdID = self.__resCmdID
			funcSlot = FuncSlot(responseHandleFunc, target)
		else
			LogWarn("ProtobufMgr", "Send with handler function will never call, because cannot find response cmdID:%s", self.__pbName)
		end
	end
	if IsLogDebugEnable() then
		LogDebug('LuaPb', '[SendCmd:%d][%s]\n%s', self.__bindCmdID, self.__pbName, TableToString(self))
	end
	lobbyService:SendRequest(self.__bindCmdID, dataBytes, responseCmdID, funcSlot, flag)
end

function structMeta.Reg(self, responseHandleFunc, target)
	if not self.__bindCmdID then
		LogError("ProtobufMgr", "Reg proto msg failed, %s is not a cmd message", self.__pbName)
		return
	end
	local lobbyService = GetLobbyServiceHandle()
	if lobbyService then
		if self.__isNtf then
			lobbyService:RegistNty(self.__bindCmdID, FuncSlot(responseHandleFunc, target))
		else
			lobbyService:RegistResponse(self.__bindCmdID, FuncSlot(responseHandleFunc, target))
		end
	end
end

function structMeta.UnReg(self, responseHandleFunc, target)
	if not self.__bindCmdID then
		LogError("ProtobufMgr", "UnReg proto msg failed, %s is not a cmd message", self.__pbName)
		return
	end
	local lobbyService = GetLobbyServiceHandle()
	if lobbyService then
		lobbyService:UnregistNty(self.__bindCmdID, FuncSlot(responseHandleFunc, target))
	end
end

function structMeta:GetBindCmdID()
	return self.__bindCmdID
end


function ProtobufMgr.DefineProtoStruct(pbName, fields, bindCmdID, resCmdIdOrIsNtf)
	local structCls = {__pbName=pbName, __fields = fields, 
					   __bindCmdID = bindCmdID}
	if resCmdIdOrIsNtf == true then
		structCls.__isNtf = true
	else
		structCls.__resCmdID = resCmdIdOrIsNtf
	end
	
	structCls.__index = structCls
	if bindCmdID then
		Proto.Msg.__cmdIDDict[bindCmdID] = structCls
	end
	return setmetatable(structCls, structMeta)
end

_G.ProtobufMgr = ProtobufMgr
ProtobufMgr:LoadAllPb()