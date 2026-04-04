local M	= class("Init", PureMVC.ModuleInit)

ProxyNames.Login = "Login"
CD.LoginCommand = "LoginCommand"
ND.LoginShowTestA = "LoginShowTestA"
CD.CharacterCommand = "CharacterCommand"
ND.CharacterShowTestA = "CharacterShowTestA"

M.Proxys = {
    {
		Name = ProxyNames.Login,
		Path = "Business/Login/Proxies/LoginProxy",
	}
}

M.Commands = {
	{
		Name = CD.LoginCommand,
		Path = "Business/Login/Commands/LoginCommand",
	},
	{
		Name = CD.CharacterCommand,
		Path = "Business/Login/Commands/CharacterCommand",
	},
}

return M
