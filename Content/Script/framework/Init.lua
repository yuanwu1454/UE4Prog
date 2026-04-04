local puremvc_load_path = "framework/patterns/"

local function puremvc_require(module)
   return require(puremvc_load_path .. module)
end

local PureMVC = {}
_G.puremvc_require = puremvc_require
_G.PureMVC= PureMVC

PureMVC.Facade = puremvc_require('facade/Facade')
PureMVC.Mediator = puremvc_require('mediator/Mediator')
PureMVC.Proxy = puremvc_require('proxy/Proxy')
PureMVC.Command = puremvc_require('command/SimpleCommand')
PureMVC.MacroCommand = puremvc_require('command/MacroCommand')
PureMVC.Notifier = puremvc_require('observer/Notifier')
PureMVC.Notification = puremvc_require('observer/Notification')
PureMVC.Observer = puremvc_require('observer/Observer')