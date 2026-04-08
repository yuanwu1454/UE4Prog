local M = {}

M.Test = function()
    print("luaproto test")

    local pb = require "pb"
    print(pb)

    -- 1. 获取项目 Content 目录（打包后自动对应游戏的Content目录）
    local ContentDir = UE4.UKismetSystemLibrary.GetProjectContentDirectory()
    -- 2. 拼接你的 pb 文件路径
    local FullPath = ContentDir .. "Script/LuaProto/test.pb"
    -- local FullProtoPath = ContentDir .. "Script/LuaProto/test.proto"
    -- print("full:", FullPath)

    -- 只能弄源码
    assert(pb.loadfile(FullPath))  -- 二进制

    -- -- 编码
    local player_data = {
        id = 2025,
        name = "unlua_user",
        level = 66
    }

    local pb_bytes = pb.encode("Test.Player", player_data)

    -- 解码
    local result = pb.decode("Test.Player", pb_bytes)

    print("id = ", result.id)
    print("name = ", result.name)
    print("level = ", result.level)
end

-- protoc --descriptor_set_out test.pb test.proto

return M

