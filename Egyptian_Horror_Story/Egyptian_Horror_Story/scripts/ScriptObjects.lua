-- globals

score = 0
prefix = "add"
objects = {} -- all objects

-- CLASSES --

print = Log
ScriptObject = { update = nil, onPlayerCollision = nil }

function ScriptObject:new(o)
	o = o or {}
	self.__index = self
	setmetatable(o, self)
	return o
end

coin = ScriptObject:new({pointsGiven = 0})

function coin.update()
	print ("hello")
end

function coin.onPlayerCollision() 
	print("Coin Wow!")
	score = score + pointsGiven
end

normalCoin = coin:new({pointsGiven = 1})

-- END OF CLASSES --

-- this is a weird solution 🤢
function addNormalCoin(id)
	objects[id] = normalCoin:new()
end 

function onPlayerCollision(id)
	objects[id]:onPlayerCollision()
end

function update()
	for k, v in pairs(objects) do
		v.update()
	end
end