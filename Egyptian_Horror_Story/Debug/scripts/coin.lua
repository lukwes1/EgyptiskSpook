-- Script should have update and on collision methods 
print = Log
name
taken = false

function update()
end

function onPlayerCollision()
	if not taken then
		hide(name)
		addScore(1)
	end
end