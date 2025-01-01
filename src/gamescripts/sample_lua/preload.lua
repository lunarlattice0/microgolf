--Declare variables pre-loop in this file.
--buf = buffer.create(99)

--[[
function InLoop()
    ImGuiLuaBridge.Begin("Test")
    ImGuiLuaBridge.InputTextMultiline("TestBuffer", buf)
    if (ImGuiLuaBridge.Button("Run Arbitrary Code")) then
        pcall(function()
            CompileAndRun(buffer.tostring(buf))
        end)
    end
    ImGuiLuaBridge.End()
end
--]]
