#!/usr/bin/env lua
local files = {
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
usage: lua build.lua [commands...] [options]

commands:
	(none)|build	compile and link noom
	run		compile, link and run noom
	files		print source files separated by spaces (for shell use)
			e.g. clang -O3 $(lua build.lua files) -o noom
	clean		clean the build cache
	help		show this!!! hello :DDDDD!!!!!

options:
	-O<level>	optimisation level passed to clang (e.g. -O2, -O3)
			defaults to -O0
	-j		parallel compilation (UNIX only!!!!)
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

-- might fail on MacOS and other *NIXes, haven't tried
-- prob can be get functional there with just stat -c shit || ...
local function getTime(path)
	if windows then return 0 end
	local handle = assert(io.popen('stat -c %Y "' .. path .. '" 2>/dev/null'))
	local result = handle:read("*a")
	handle:close()
	return tonumber(result) or 0
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

local commands = {}
local optLevel = nil
local parallel = false

local shi = { build=true, clean=true, run=true, files=true, help=true }

for i = 1, #arg do
	local a = arg[i]
	if shi[a] then
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
		runCommand(exe)
	end
end
