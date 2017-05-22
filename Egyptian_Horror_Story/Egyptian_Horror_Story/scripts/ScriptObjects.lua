-- globals

score = 0
prefix = "add"
objects = {} -- all objects

-- CLASSES --

print = Log
ScriptObject = { key = -1, update = nil, onPlayerCollision = nil }

function ScriptObject:new(o)
	o = o or {}
	self.__index = self
	setmetatable(o, self)
	return o
end

Coin = ScriptObject:new({pointsGiven = 0, taken = false})

function Coin:update()
end

function Coin:onPlayerCollision() 
	if not self.taken then
		score = score + self.pointsGiven
		print("Score: " .. score)
		hide(self.key)
		
		self.taken = true
	end
end

NormalCoin = Coin:new{pointsGiven = 1}
Trampoline = ScriptObject:new{frameCD = 100, currCD = 0}

function Trampoline:update() 
	if (self.currCD > 0) then self.currCD = self.currCD - 1 end
end

function Trampoline:onPlayerCollision()
	if IsInAir then
		SetSpeed(50)
	end
end

-- END OF CLASSES --

-- this is a weird solution 🤢
function addNormalCoin(id, inKey)
	objects[id] = NormalCoin:new{key = inKey}
end 

function addTrampoline(id, inKey)
	objects[id] = Trampoline:new{key = inKey}
end 

function onPlayerCollision(id)
	objects[id]:onPlayerCollision()
end

function update()
	for k, v in pairs(objects) do
		v:update()
	end
end