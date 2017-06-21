-- globals

score = 0
prefix = "add"
objects = {} -- all objects

VICTORY_POINTS = 15

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
		hide(self.key)
		
		self.taken = true
	end
	
	if score > VICTORY_POINTS then
		victory() -- just a test victory screen
	end
end

NormalCoin = Coin:new{pointsGiven = 1}
SuperCoin = Coin:new{pointsGiven = 5, up = false}
Trampoline = ScriptObject:new{frameCD = 100, currCD = 0}

function SuperCoin:update()
	local x, y, z = GetPosition(self.key)
	if up then
		SetPosition(self.key, x, y + 0.01, z)
		if y > self.startY + 5 then
			up = not up
		end
	else
		SetPosition(self.key, x, y - 0.01, z)
		if y < self.startY - 5 then
			up = not up
		end
	end
end

function Trampoline:update() 
	if (self.currCD > 0) then self.currCD = self.currCD - 1 end
end

function Trampoline:onPlayerCollision()
	if IsInAir() and self.currCD <= 0 then
		SetSpeed(50)
		self.currCD = frameCD
	end
end

-- END OF CLASSES --

-- this is a weird solution 🤢
function addNormalCoin(id, inKey)
	objects[id] = NormalCoin:new{key = inKey}
end 

function addSuperCoin(id, inKey)
	objects[id] = SuperCoin:new{key = inKey}
	local x, y, z = GetPosition(inKey)
	objects[id].startY = y
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