local GameFacade = PureMVC.Facade:GetInstance('GameFacade')
print("GameFacade", GameFacade)
-- 全局
local GlobalModuleInits = {
	"Business/Common/Init",
	-- "Business/ResUpdate/Init",
	"Business/Login/Init",
}

function GameFacade:SetupGameProxy()
	-- GameFacade:RegisterProxy(require('Business/Common/Proxies/GameProxy').new(ProxyNames.GameProxy))
	-- GameFacade:RegisterCommand(ND.ClearAllProxy, require("Business/Common/Commands/ClearAllProxyCmd"))
end

-- 局外
function GameFacade:Setup()
	LogInfo('GameFacade', 'GameFacade SetupGlobal...')
	self:SetupModule(GlobalModuleInits)
	LogInfo('GameFacade', 'GameFacade SetupLobby...')
	-- self:SetupModule(LobbyModuleInits)
	LogInfo('GameFacade', 'GameFacade Setup End')
end

function GameFacade:Uninstall()
	LogInfo('GameFacade', 'GameFacade UninstallGlobal...')
	self:UninstallModule(GlobalModuleInits)
	-- LogInfo('GameFacade', 'GameFacade UninstallLobby...')
	-- self:UninstallModule(LobbyModuleInits)
	-- LogInfo('GameFacade', 'GameFacade UninstallGame...')
	-- self:UninstallModule(GameModuleInits)
	LogInfo('GameFacade', 'GameFacade Uninstall End')
end

-- 局内
function GameFacade:SetupGame()
	-- LogInfo('GameFacade', 'GameFacade UninstallLobby...')
	-- self:UninstallModule(LobbyModuleInits)
	-- LogInfo('GameFacade', 'GameFacade SetupGame...')
	-- self:SetupModule(GameModuleInits)
	LogInfo('GameFacade', 'GameFacade SetupGame End')
end

function GameFacade:UninstallGame()
	-- LogInfo('GameFacade', 'GameFacade UninstallGame...')
	-- self:UninstallModule(GameModuleInits)
	-- LogInfo('GameFacade', 'GameFacade SetupLobby...')
	-- self:SetupModule(LobbyModuleInits)
	-- LogInfo('GameFacade', 'GameFacade UninstallGame End')
end

function GameFacade:SetupModule(moduleInits)
	local bSuccess, errorMsg, ClassObject
	for k, v in pairs(moduleInits) do
		LogInfo('GameFacade', 'GameFacade SetupModule Path = %s', v)
		bSuccess, errorMsg = pcall(require, v)
		if bSuccess then
			ClassObject = require(v)
			ClassObject:Init()
		else
			LogError("GameFacade:SetupModule", "File require error !!! \n Error = %s", errorMsg)
		end
	end
end

function GameFacade:UninstallModule(moduleInits)
	local bSuccess, errorMsg, ClassObject
	for k, v in pairs(moduleInits) do
		LogInfo('GameFacade', 'GameFacade UninstallModule Path = %s', v)
		if pcall(require, v) then
			ClassObject = require(v)
			ClassObject:Clear()
		end
	end	
end


_G.GameFacade = GameFacade