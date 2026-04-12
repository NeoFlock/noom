-- i love build scripts i love build scripts i love build scripts scripts build love i script build love me i love script build i script love build

-- TODO: script the build

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

runCommand('rm -r build')
runCommand('mkdir build')

local files = {
	'src/lexer.c',
	'src/helper.c',
	'src/main.c',
}

local objects = {}

for i = 1,#files do
	local fname = files[i]
	local out = "build/" .. filename(fname) .. '.o'

	runCommand('clang -c -o ' .. out .. ' ' .. fname)

	objects[#objects+1] = out;
end

runCommand('clang -o noom ' .. table.concat(objects, ' '))
