local i = 1
::top::
if i <= 3 then
	print("i = " .. i)
	i = i + 1
	goto top
end
print("done")
