--[[
* @author PureMVC LUA Native Port by Ravior 
* @author Copyright(c) 2015 Gitlib, Inc., Some rights reserved.
* 
* @class puremvc.MacroCommand
* @extends puremvc.Notifier
* 
* A base command implementation that executes other commands, such as
* {@link puremvc.SimpleCommand SimpleCommand}
* or {@link puremvc.MacroCommand MacroCommand}
* subclasses.
*  
* A MacroCommand maintains an list of
* command constructor references called*SubCommands*.
* 
* When #execute is called, the MacroCommand
* instantiates and calls #execute on each of its*SubCommands* in turn.
* Each*SubCommand* will be passed a reference to the original
* {@link puremvc.Notification Notification} 
* that was passed to the MacroCommands #execute method
* 
* Unlike {@link puremvc.SimpleCommand SimpleCommand}, 
* your subclass should not override #execute but instead, should 
* override the #InitializeMacroCommand method, calling #AddSubCommand once for 
* each*SubCommand* to be executed.
]]
local Notifier = puremvc_require('observer/Notifier')
local MacroCommand = class("MacroCommand",Notifier)


--[[
* If your subclass does define a constructor, be sure to call "super" like so
* 
*     function MyMacroCommand ()
*     {
*         MacroCommand.call(this);
*     };
* @constructor
]]
function MacroCommand:ctor()
	self.subCommands = {}
	self:InitializeMacroCommand()
end


--[[
* @protected
* Initialize the MacroCommand.
* 
* In your subclass, override this method to 
* initialize the MacroCommand's*SubCommand*  
* list with command class references like 
* this:
* 
*     // Initialize MyMacroCommand
*     MyMacroCommand.prototype.InitializeMacroCommand= function ()
*     {
*         this.AddSubCommand( com.me.myapp.controller.FirstCommand );
*         this.AddSubCommand( com.me.myapp.controller.SecondCommand );
*         this.AddSubCommand( com.me.myapp.controller.ThirdCommand );
*     };
* 
* Note that*SubCommand*s may be any command implementor,
* MacroCommands or SimpleCommands are both acceptable.
* @return {void}
]]
function MacroCommand:InitializeMacroCommand() end


--[[
* @protected
* Add a*SubCommand*
* 
* The*SubCommand*s will be called in First In / First Out (FIFO) order
* @param {Function} commandClassRef
*  A reference to a subclassed SimpleCommand or MacroCommand constructor
]]
function MacroCommand:AddSubCommand(commandClassRef)
	table.insert(self.subCommands, commandClassRef)
end


--[[
* Execute this MacroCommands*SubCommands*
* 
* The*SubCommand*s will be called in First In / First Out (FIFO) order
* @param {puremvc.Notification} note
*  The Notification object to be passed to each*SubCommand*
]]
function MacroCommand:Execute(note)
	while(#self.subCommands > 0) do
        local ref = table.remove(self.subCommands)
        local cmd = ref.new()
        cmd:InitializeNotifier(self.multitonKey)
        cmd:Execute(note)
    end
end

return MacroCommand