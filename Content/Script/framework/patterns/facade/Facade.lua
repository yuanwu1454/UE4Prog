local Controller = require('framework/core/Controller')
local Model = require('framework/core/Model')
local View = require('framework/core/View')
local Notification = puremvc_require('observer/Notification')

local Facade = class("Facade")

function Facade:ctor(key)
	if Facade.instanceMap[key] ~= nil then
		error(Facade.MULTITON_MSG)
	end
	self:InitializeNotifier(key)
	self:initializeFacade()
end

function Facade:initializeFacade()
	self:initializeModel()
	self:InitializeController()
	self:InitializeView()
end

function Facade:InitializeController()
	if self.controller ~= nil then
		return
	end
	self.controller = Controller:GetInstance(self.multitonKey)
end

function Facade:initializeModel()
	if self.model ~= nil then
		return
	end
	self.model = Model:GetInstance(self.multitonKey)
end

function Facade:InitializeView()
	if self.view ~= nil then
		return
	end
	self.view = View:GetInstance(self.multitonKey)
end

function Facade:RegisterCommand(notificationName,commandClassRef)
	self.controller:RegisterCommand(notificationName,commandClassRef)
end

function Facade:RemoveCommand(notificationName)
	self.controller:RemoveCommand(notificationName)
end

function Facade:HasCommand(notificationName)
	return self.controller:HasCommand(notificationName)
end

function Facade:RegisterProxy(proxy)
	self.model:RegisterProxy(proxy)
end

function Facade:RetrieveProxy(proxyName)
	return self.model:RetrieveProxy(proxyName)
end

function Facade:RemoveProxy(proxyName)
	local proxy = nil
	if self.model ~= nil then
		proxy = self.model:RemoveProxy(proxyName)
	end
	return proxy
end

function Facade:HasProxy(proxyName)
	return self.model:HasProxy(proxyName)
end

function Facade:RegisterMediator(mediator)
	if self.view ~= nil then
		self.view:RegisterMediator(mediator)
	end
end

function Facade:RetrieveMediator(mediatorName)
	return self.view:RetrieveMediator(mediatorName)
end

function Facade:RemoveMediator(mediatorName)
	local mediator = nil
	if self.view ~= nil then
		mediator = self.view:RemoveMediator(mediatorName)
	end
	return mediator
end

function Facade:HasMediator(mediatorName)
	return self.view:HasMediator(mediatorName)
end

function Facade:SendNotification(notificationName,body,type)
	self:NotifyObservers(Notification.new(notificationName,body,type))
end

function Facade:NotifyObservers(notification)
	if self.view ~= nil then
		self.view:NotifyObservers(notification)
	end
end

function Facade:InitializeNotifier(key)
	self.multitonKey = key
end


-- static func
function Facade.GetInstance(key)
	if nil == key then 
		return nil
	end
	if Facade.instanceMap[key] == nil then
		Facade.instanceMap[key] = Facade.new(key)
	end
	return Facade.instanceMap[key]
end

function Facade.hasCore(key)
	return Facade.instanceMap[key] ~= nil
end

function Facade.removeCore(key)
	if Facade.instanceMap[key] == nil then
		return
	end

	Model:RemoveModel(key)
	View:RemoveView(key)
	Controller:RemoveController(key)
	Facade.instanceMap[key] = nil
end


Facade.instanceMap = {}

Facade.MULTITON_MSG = "Facade instance for this Multiton key already constructed!";

return Facade