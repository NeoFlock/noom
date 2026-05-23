-- i love build scripts i love build scripts i love build scripts scripts build love i script build love me i love script build i script love build

local isBlendi = os.getenv("USER") == "blendi"
local separator = package.config:sub(1,1)

local function filename(path)
	local s,e = 1, #path
	for i = 1,#path do
		if path:sub(i,i) == '/' then s = i+1 end;
		if path:sub(i,i) == '.' then e = i-1 end;
	end

	return path:sub(s,e)
end

local function runCommand(cmd)
	print("> " .. cmd)
	return os.execute(cmd)
end

local function fixPath(path)
	local new = ""
	for i = 1,#path do
		local ch = path:sub(i,i)
		if ch == '/' then new = new .. separator
		else new = new .. ch end
	end
	return new
end

if separator == '\\' then
	runCommand('mkdir build 2>nul')
else
	runCommand('mkdir -p build')
end

local files = {
	'src/helper.c',
	'src/lexer.c',
	'src/parser.c',
	'src/compiler.c',
	'src/main.c',
}

local objects = {}

local coolArgs = {}

local function getTime(path)
	local handle = io.popen('stat -c %Y "' .. path .. '" 2>/dev/null')
	local result = handle:read("*a")
	handle:close()
	return tonumber(result) or 0
end

local function needsRebuild(src, obj)
	if separator == '\\' then return true end
	local srcTime = getTime(src)
	local objTime = getTime(obj)
	return srcTime > objTime
end

if not isBlendi then table.insert(coolArgs, '-fsanitize=undefined,address') end

local needsLinking = false

for i = 1,#files do
	local fname = files[i]
	local out = "build/" .. filename(fname) .. '.o'

	fname = fixPath(fname)
	out = fixPath(out)

	if needsRebuild(fname, out) then
		needsLinking = true
		runCommand('clang -c -o ' .. out .. ' ' .. fname .. ' ' .. table.concat(coolArgs, ' '))
	end

	objects[#objects+1] = out;
end

local exe = separator == '\\' and "noom.exe" or "noom"

if needsLinking then
	runCommand('clang -o ' .. exe .. ' ' .. table.concat(objects, ' ') .. ' ' .. table.concat(coolArgs, ' '))
end
