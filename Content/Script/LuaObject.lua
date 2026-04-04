local LuaObject = Class()


function LuaObject:SetData(data)
    self.data = data
end


function LuaObject:GetData()
    return self.data
end

return LuaObject