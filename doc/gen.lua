local doc = io.open("verbs.txt", "wb")

local V = "!#$%&*+,-.<=>?@^_|~"
for i=0,18 do
    local s = string.format("%02d", i)
    local impl = io.open("../src/verbs/"..s..".c")

    local anydoc = false
    for l in impl:lines() do
        if l:sub(1,3) == "//@" then
            if anydoc then
                doc:write("   ", l:sub(4), "\n")
            else
                doc:write(s, " ", l:sub(4), "\n")
                anydoc = true
            end
        end
    end

    if not anydoc then
        doc:write(s, "  ", V:sub(i+1,i+1), "  nyi\n")
    end
end

doc:write("num is impl filename\n")
doc:close()
