#!/usr/bin/env lua
local files = {
	'src/noom.c',
	'src/error.c',
	'src/helper.c',
	'src/lexer.c',
	'src/parser.c',
	'src/compiler.c',
	'src/vm.c',
	'src/gc.c',
	'src/main.c',
}

-- i love build scripts i love build scripts i love build scripts scripts build love i script build love me i love script build i script love build
local isBlendi = os.getenv("USER") == "blendi"
local separator = package.config:sub(1, 1)
-- win mean and lean!!!!
local windows = separator == '\\'

local help = [[
usage: lua build.lua [commands...] [options] -- args...

commands:
    (none)|build    compile and link noom
    run     compile, link and run noom
    test    run tests
    files   print source files separated by spaces (for shell use)
                e.g. clang -O3 $(lua build.lua files) -o noom
    clean   clean the build cache
    help    show this!!! hello :DDDDD!!!!!

options:
    -O<level>    optimisation level passed to clang (e.g. -O2, -O3)
                     defaults to -O0
    -j           parallel compilation (UNIX only!!!!)
    -- args...	 anything after -- is forwarded to noom when running it
]]

local function filename(path)
	local s, e = 1, #path
	for i = 1, #path do
		if path:sub(i, i) == '/' then s = i + 1 end;
		if path:sub(i, i) == '.' then e = i - 1 end;
	end

	return path:sub(s, e)
end

local function fixPath(path)
	if not windows then return path end
	return path:gsub('/', '\\')
end

local function runCommand(cmd, ignore_fail)
	print("> " .. cmd)
	local result = os.execute(cmd)
	if (result ~= true and result ~= 0) and (not ignore_fail) then
		os.exit(1)
	end
end

local function preadall(cmd)
    local handle = io.popen(cmd)
    if handle == nil then return end
    local result = handle:read("*a")
    handle:close()
	return result
end
-- might fail on MacOS and other *NIXes, haven't tried
-- prob can be get functional there with just stat -c shit || ...
local function getTime(path)
	if windows then return 0 end
	return preadall('stat -c %Y "' .. path .. '" 2>/dev/null') or 0
end

local function needsRebuild(src, obj)
	if windows then return true end -- TODO?
	return getTime(src) > getTime(obj)
end

local function dirExists(path)
	if windows then return false end -- always recreate on windows duh, TODO
	local ok, _, code = os.execute('test -d "' .. path .. '"')
	return (ok == true) or (code == 0)
end

local function ensureDir(path)
	path = fixPath(path)
	if dirExists(path) then return end
	if windows then
		runCommand('mkdir "' .. path .. '" 2>nul', true)
	else
		runCommand('mkdir -p "' .. path .. '"')
	end
end

local function rmdir(path)
	path = fixPath(path)
	if windows then
		runCommand('rmdir /s /q "' .. path .. '" 2>nul', true)
	else
		runCommand('rm -rf "' .. path .. '"')
	end
end

-- Good enough posix-ish shell quoting for forwarded args ehhh
local function shellQuote(s)
	if windows then
		return '"' .. s:gsub('"', '\\"') .. '"'
	end
	return "'" .. s:gsub("'", "'\\''") .. "'"
end

local commands = {}
local optLevel = nil
local parallel = false
local forwardedArgs = {}

local shi = { build = true, clean = true, run = true, files = true, help = true, test = true }

for i = 1, #arg do
	local a = arg[i]
	if a == '--' then
		for j = i + 1, #arg do
			table.insert(forwardedArgs, arg[j])
		end
		break
	elseif shi[a] then
		if commands[a] then
			io.stderr:write("warning: duplicate command '" .. a .. "'\n")
		end
		commands[a] = true
	elseif a:match("^%-O") then
		optLevel = a
	elseif a == "-j" then
		parallel = true
	else
		io.stderr:write("mmm no i don't know what is " .. a .. "\n")
		os.exit(1)
	end
end

if parallel and windows then
	parallel = false -- fuck you
end

optLevel = optLevel or "-O0"

if not next(commands) then commands.build = true end
if commands.run then commands.build = true end
if commands.test then commands.build = true end

if commands.help then
	print(help)
	return
end

if commands.files then
	print(table.concat(files, " "))
	return
end

if commands.clean then
	rmdir("build")
end

local interpreters = {
	["51"] = { lua = "lua5.1", noom = "." .. (windows and "\\" or "/") .. "noom -l51" },
	["52"] = { lua = "lua5.2", noom = "." .. (windows and "\\" or "/") .. "noom -l52" },
	["53"] = { lua = "lua5.3", noom = "." .. (windows and "\\" or "/") .. "noom -l53" },
	["54"] = { lua = "lua5.4", noom = "." .. (windows and "\\" or "/") .. "noom -l54" }
}

if commands.build then
	ensureDir("build")

	local coolArgs = { optLevel }

	-- Why does blendi even need that bro 😭🙏 Do not run noom on your RISC-V shi
	if not isBlendi then table.insert(coolArgs, '-fsanitize=undefined,address') end
	local coolerArgs = table.concat(coolArgs, ' ')

	local objects = {}
	local needsLinking = false

	local toCompile = {}

	for _, fname in ipairs(files) do
		fname = fixPath(fname)
		local out = fixPath("build/" .. filename(fname) .. ".o")
		table.insert(objects, out)

		if needsRebuild(fname, out) then
			needsLinking = true
			local cmd = 'clang -g -c -o ' .. out .. ' ' .. fname .. ' ' .. coolerArgs
			if parallel then
				table.insert(toCompile, cmd)
			else
				runCommand(cmd)
			end
		end
	end

	if next(toCompile) then
		local shellScript = table.concat(toCompile, " &\n") .. " &\nwait"
		runCommand(shellScript)
	end

	local exe = windows and ".\\noom.exe" or "./noom"

	if needsLinking then
		runCommand('clang -g -o ' .. exe .. ' ' .. table.concat(objects, ' ') .. ' ' .. coolerArgs)
	end

	if commands.run then
		local parts = { exe }
		for _, a in ipairs(forwardedArgs) do
			table.insert(parts, shellQuote(a))
		end
		local final = table.concat(parts, ' ')

		if not windows and os.execute('command -v rlwrap > /dev/null 2>&1') == true then
			final = 'rlwrap ' .. final
		end

		runCommand(final)
	end

	if commands.test then
		if windows then
			io.stderr:write("womp womp this doesn't work here\n")
			os.exit(1)
		end
		local handle = io.popen("ls -d tests/* 2>/dev/null")
		if handle == nil then
			io.stderr:write("why does your computer not have ls")
			os.exit(1)
		end
		local things_to_test = {}
		for thing_to_test in handle:lines() do
			table.insert(things_to_test, thing_to_test)
		end
		for i = 1, #things_to_test do
			local interpreter = interpreters[things_to_test[i]:match("%d+")] or { lua = "lua", noom = "noom" }
			io.write("[" .. i .. "/" .. #things_to_test .. "] " .. things_to_test[i] .. " ")
			local luaOut = preadall(interpreter["lua"] .. " " .. things_to_test[i])
			local noomOut = preadall(interpreter["noom"] .. " " .. things_to_test[i])
			if (luaOut == noomOut) then io.write("+") 
			else io.write("-\nLua output:\n" .. luaOut .. "\nNoom output:\n" .. noomOut) end
			io.write("\n")
		end
		handle:close()
	end
end
