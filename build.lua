-- i love build scripts i love build scripts i love build scripts scripts build love i script build love me i love script build i script love build

local isBlendi = os.getenv("USER") == "blendi"
local seperator = package.config:sub(1,1)

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
		if ch == '/' then new = new .. seperator
		else new = new .. ch end
	end
	return new
end

if seperator == '\\' then
	runCommand('rmdir /S /Q build')
else
	runCommand('rm -rf build')
end

runCommand('mkdir build')

local files = {
	'src/helper.c',
	'src/lexer.c',
	'src/parser.c',
	'src/compiler.c',
	'src/main.c',
}

local objects = {}

local coolArgs = {}
if not isBlendi then table.insert(coolArgs, '-fsanitize=undefined,address') end

for i = 1,#files do
	local fname = files[i]
	local out = "build/" .. filename(fname) .. '.o'

	fname = fixPath(fname)
	out = fixPath(out)

	runCommand('clang -c -o ' .. out .. ' ' .. fname .. ' ' .. table.concat(coolArgs, ' '))

	objects[#objects+1] = out;
end

local exe = seperator == '\\' and "noom.exe" or "noom"

runCommand('clang -o ' .. exe .. ' ' .. table.concat(objects, ' ') .. ' ' .. table.concat(coolArgs, ' '))
