local M = {}
M["LoginPage"] = {
    Path = "/Game/Game/UI/Login.Login_C",
    Order = 0
}
M["PlayerPage"] = {
    Path = "/Game/Game/UI/PlayerPage.PlayerPage_C",
    Order = 0
}
_G.PageConfig = M

local PageNames = {}
for pageName, _ in pairs(M) do
    PageNames[pageName] = pageName
end
_G.PageNames = PageNames
