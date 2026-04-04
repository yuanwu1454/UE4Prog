--[[
* @author PureMVC LUA Native Port by Ravior 
* @author Copyright(c) 2015 Gitlib, Inc., Some rights reserved.
* 
* @class puremvc.Facade
* Facade exposes the functionality of the Controller, Model and View
* actors to client facing code. 
* 
* This Facade implementation is a Multiton, so you should not call the 
* constructor directly, but instead call the static Factory method, 
* passing the unique key for this instance to #getInstance
]]



local Controller = require('framework/core/Controller')
local Model = require('framework/core/Model')
local View = require('framework/core/View')
local Notification = puremvc_require('observer/Notification')

local Facade = class("Facade")


--[[
* @constructor
* @param {string} key
* 	The multiton key to use to retrieve the Facade instance.
* @throws {Error} 
*  If an attempt is made to instantiate Facade directly
]]
function Facade:ctor(key)
	if Facade.instanceMap[key] ~= nil then
		error(Facade.MULTITON_MSG)
	end
	self:InitializeNotifier(key)
	Facade.instanceMap[key] = self
	self:initializeFacade()
end


--[[
* Initialize the Multiton Facade instance.
* 
* Called automatically by the constructor. Override in your subclass to any
* subclass specific initializations. Be sure to call the 'super' 
* initializeFacade method, though
* 
*     MyFacade.prototype.initializeFacade= function ()
*     {
*         Facade.call(this);
*     };
* @protected
* @return {void}
]]
function Facade:initializeFacade()
	self:initializeModel()
	self:InitializeController()
	self:InitializeView()
end


--[[
* Facade Multiton Factory method. 
* Note that this method will return null if supplied a
* null or undefined multiton key.
* 
* @param {string} key
* 	The multiton key use to retrieve a particular Facade instance
* @return {puremvc.Facade}
]]
function Facade:GetInstance(key)
	if nil == key then 
		return nil
	end

	if Facade.instanceMap[key] == nil then
		Facade.instanceMap[key] = Facade.new(key)
	end

	return Facade.instanceMap[key]
end


--[[
* Initialize the {@link puremvc.Controller Controller}.
* 
* Called by the #initializeFacade method.
* 
* Override this method in your subclass of Facade
* if one or both of the following are true:
* - You wish to initialize a different Controller
* - You have 
* {@link puremvc.SimpleCommand SimpleCommand}s
* or {@link puremvc.MacroCommand MacroCommand}s
* to register with the Controllerat startup.   
* 
* If you don't want to initialize a different Controller, 
* call the 'super' initializeControlle method at the beginning of your
* method, then register commands.
* 
*     MyFacade.prototype.InitializeController= function ()
*     {
*         Facade.prototype.InitializeController.call(this);
*         this.RegisterCommand(AppConstants.A_NOTE_NAME, ABespokeCommand)
*     }
* 
* @protected
* @return {void}
]]
function Facade:InitializeController()
	if self.controller ~= nil then
		return
	end

	self.controller = Controller:GetInstance(self.multitonKey)
end


--[[
* @protected
* Initialize the {@link puremvc.Model Model};
* 
* Called by the #initializeFacade method.
* Override this method in your subclass of Facade if one of the following are
* true:
* 
* - You wish to initialize a different Model.
* 
* - You have {@link puremvc.Proxy Proxy}s to 
*   register with the Model that do not retrieve a reference to the Facade at 
*   construction time.
* 
* If you don't want to initialize a different Model
* call 'super' #initializeModel at the beginning of your method, then register 
* Proxys.
* 
* Note: This method is*rarely* overridden; in practice you are more
* likely to use a command to create and RegisterProxys with the Model>, 
* since Proxys with mutable data will likely
* need to send Notifications and thus will likely want to fetch a reference to 
* the Facade during their construction. 
* 
* @return {void}
]]
function Facade:initializeModel()
	if self.model ~= nil then
		return
	end

	self.model = Model:GetInstance(self.multitonKey)
end


--[[
* @protected
* 
* Initialize the {@link puremvc.View View}.
* 
* Called by the #initializeFacade method.
* 
* Override this method in your subclass of Facade if one or both of the 
* following are true:
*
* - You wish to initialize a different View.
* - You have Observers to register with the View
* 
* If you don't want to initialize a different View 
* call 'super' #initializeView at the beginning of your
* method, then register Mediator instances.
* 
*     MyFacade.prototype.initializeView= function ()
*     {
*         Facade.prototype.initializeView.call(this);
*         this.RegisterMediator(new MyMediator());
*     };
* 
* Note: This method is*rarely* overridden; in practice you are more
* likely to use a command to create and register Mediators
* with the View, since Mediator instances will need to send 
* Notifications and thus will likely want to fetch a reference 
* to the Facade during their construction. 
* @return {void}
]]
function Facade:InitializeView()
	if self.view ~= nil then
		return
	end

	self.view = View:GetInstance(self.multitonKey)
end


--[[
* Register a command with the Controller by Notification name
* @param {string} notificationName
*  The name of the Notification to associate the command with
* @param {Function} commandClassRef
*  A reference ot the commands constructor.
* @return {void}
]]
function Facade:RegisterCommand(notificationName,commandClassRef)
	self.controller:RegisterCommand(notificationName,commandClassRef)
end


--[[
* Remove a previously registered command to Notification mapping from the
* {@link puremvc.Controller#RemoveCommand Controller}
* @param {string} notificationName
*  The name of the the Notification to remove from the command mapping for.
* @return {void}
]]
function Facade:RemoveCommand(notificationName)
	self.controller:RemoveCommand(notificationName)
end


--[[
* Check if a command is registered for a given notification.
* 
* @param {string} notificationName
*  A Notification name
* @return {boolean}
*  Whether a comman is currently registered for the given notificationName
]]
function Facade:HasCommand(notificationName)
	return self.controller:HasCommand(notificationName)
end


--[[
* Register a Proxy with the {@link puremvc.Model#RegisterProxy Model}
* by name.
* 
* @param {puremvc.Proxy} proxy
*  The Proxy instance to be registered with the Model.
* @return {void}
]]
function Facade:RegisterProxy(proxy)
	self.model:RegisterProxy(proxy)
end


--[[
* Retrieve a Proxy from the Model
* 
* @param {string} proxyName
* @return {puremvc.Proxy}
]]
function Facade:RetrieveProxy(proxyName)
	return self.model:RetrieveProxy(proxyName)
end


--[[
* Remove a Proxy from the Model by name
* @param {string} proxyName
*  The name of the Proxy
* @return {puremvc.Proxy}
*  The Proxy that was removed from the Model
]]
function Facade:RemoveProxy(proxyName)
	local proxy = nil
	if self.model ~= nil then
		proxy = self.model:RemoveProxy(proxyName)
	end

	return proxy
end


--[[
* Check it a Proxy is registered.
* @param {string} proxyName
*  A Proxy name
* @return {boolean}
*  Whether a Proxy is currently registered with the given proxyName
]]
function Facade:HasProxy(proxyName)
	return self.model:HasProxy(proxyName)
end


--[[
* Register a Mediator with with the View.
* 
* @param {puremvc.Mediator} mediator
*  A reference to the Mediator to register
* @return {void}
]]
function Facade:RegisterMediator(mediator)
	if self.view ~= nil then
		self.view:RegisterMediator(mediator)
	end
end


--[[
* Retrieve a Mediator from the View by name
* 
* @param {string} mediatorName
*  The Mediators name
* @return {puremvc.Mediator}
*  The retrieved Mediator
]]
function Facade:RetrieveMediator(mediatorName)
	return self.view:RetrieveMediator(mediatorName)
end


--[[
* Remove a Mediator from the View.
* 
* @param {string} mediatorName
*  The name of the Mediator to remove.
* @return {puremvc.Mediator}
*  The removed Mediator
]]
function Facade:RemoveMediator(mediatorName)
	local mediator = nil
	if self.view ~= nil then
		mediator = self.view:RemoveMediator(mediatorName)
	end

	return mediator
end


--[[
* Check if a Mediator is registered or not.
* 
* @param {string} mediatorName
*  A Mediator name
* @return {boolean}
*  Whether a Mediator is registered with the given mediatorName
]]
function Facade:HasMediator(mediatorName)
	return self.view:HasMediator(mediatorName)
end


--[[
* Create and send a 
* {@link puremvc.Notification Notification}
* 
* Keeps us from having to construct new Notification instances in our
* implementation
* 
* @param {string} notificationName
*  The name of the Notification to send
* @param {Object} [body]
*  The body of the notification
* @param {string} [type]
*  The type of the notification
* @return {void}
]]
function Facade:SendNotification(notificationName,body,type)
	self:NotifyObservers(Notification.new(notificationName,body,type))
end


--[[
* Notify {@link puremvc.Observer Observer}s
* 
* This method is left public mostly for backward compatibility, and to allow
* you to send custom notification classes using the facade.
* 
* Usually you should just call sendNotification and pass the parameters, never 
* having to construct the notification yourself.
* 
* @param {puremvc.Notification} notification
*  The Notification to send
* @return {void}
]]
function Facade:NotifyObservers(notification)
	if self.view ~= nil then
		self.view:NotifyObservers(notification)
	end
end


--[[
* Initialize the Facades Notifier capabilities by setting the Multiton key for 
* this facade instance.
* 
* Not called directly, but instead from the constructor when #getInstance is 
* invoked. It is necessary to be public in order to implement Notifier
* 
* @param {string} key
* @return {void}
]]
function Facade:InitializeNotifier(key)
	self.multitonKey = key
end


--[[
* Check if a*Core* is registered or not
*
* @static
* @param {string} key
*  The multiton key for the*Core* in question
* @return {boolean}
*  Whether a*Core* is registered with the given key
]]
function Facade:hasCore(key)
	return Facade.instanceMap[key] ~= nil
end


--[[
* Remove a*Core* 
* 
* Remove the Model, View, Controller and Facade for a given key.
*
* @static
* @param {string} key
* @return {void}
]]
function Facade:removeCore(key)
	if Facade.instanceMap[key] == nil then
		return
	end

	Model:RemoveModel(key)
	View:RemoveView(key)
	Controller:RemoveController(key)
	Facade.instanceMap[key] = nil
end


--[[
* @ignore
* The Multiton Facade instance map.
* @static
* @protected
* @type Array
]]
Facade.instanceMap = {}


--[[
* @ignore
* Message Constants
* @protected
* @type {string}
* @const
* @static
]]
Facade.MULTITON_MSG = "Facade instance for this Multiton key already constructed!";

return Facade