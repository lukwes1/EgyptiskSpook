print = Log

-- General Functions

function templateNewMethod(self, o)
	o = o or {}
	self.__index = self -- because inheritence
	setmetatable(o, self)
	return o
end

function createObject(inName, class)
	if (class == ScriptCollider.class) then
		return ScriptCollider:new{name = inName}
	elseif (class == Solid.class) then
		return Solid:new{name = inName}
	end
end

function loadObject(line, currentObject)
	_, _, var = line:find("%s*(%a+%d*)")
	memberVar = currentObject[var]

	if getmetatable(memberVar) == Vector then -- Vector
		currentObject[var] = Vector:createFromFile(line)
	elseif line:find("%s+%d+") then
		_, _, currentObject[var] = line:find("%s+(%d+)")
	else -- string
		_, _, currentObject[var] = line:find("%s+\"(.*)\"")
	end
end

-- Vector Class

Vector = {x = 0.0, y = 0.0, z = 0.0}

function Vector:__tostring()
	return self.x .. " " .. self.y .. " " .. self.z
end	

function Vector:__add(vec)
	if getmetatable(vec) == Vector and getmetatable(self) == Vector then
		return Vector:new{x = self.x + vec.x, y = self.y + vec.y, z = self.z + vec.z}
	else
		return Vector:new() -- Just returns default
	end
end

function Vector:__mul(val)
	if getmetatable(self) == Vector and type(val) == "number" then
		return Vector:new{x = self.x * val, y = self.y * val, z = self.z * val}
	elseif type(self) == "number" then -- Self is the number and val is the vector so switch
		return val * self
	end
end

function Vector.filePattern()
	return "%s+(%d+%p?%d?)%s+(%d+%p?%d?)%s+(%d+%p?%d?)"
end	

function Vector:createFromFile(line)
	local numbers = {}
	local i = 1
	
	for l in line:gmatch("%S+") do
		numbers[i] = l
		i = i + 1
	end
	
	return Vector:new{x = tonumber(numbers[2]),
					  y = tonumber(numbers[3]),
					  z = tonumber(numbers[4])}
end

Vector.new = templateNewMethod

-- Loadable Object Class

LoadableObject = {name = "", class = "", texture = 0}
LoadableObject.new = templateNewMethod

function LoadableObject:baseString()
	return self.name .. " : " .. self.class ..
	"\n\ttexture " .. tostring(self.texture)
end

-- A Solid Class 
Solid = LoadableObject:new{position = Vector:new(), size = Vector:new(), class = "Solid", texture}

function Solid:__tostring()
	return self:baseString() ..
	"\n\tposition " .. tostring(self.position) ..
	"\n\tsize " .. tostring(self.size) ..
	"\nend"
end	

-- ScriptCollider class, a scripted object
ScriptCollider = LoadableObject:new{position = Vector:new(), size = Vector:new(), class = "ScriptCollider", id = "0", texture}

function ScriptCollider:__tostring()
	return self:baseString() .. 
		   "\n\tid \"" .. self.id .. "\"" ..
		   "\n\tposition " .. tostring(self.position) ..
		   "\n\tsize " .. tostring(self.size) ..
		   "\nend"
end	

-- Functions C++ should call
allObjects = {}

function loadMap(path)
	io.input(path)
	
	local inObj = false
	local currentObject
	for line in io.lines() do
		if line:find("end") then
			inObj = false
			table.insert(allObjects, currentObject)
		else
			if inObj then
				loadObject(line, currentObject)
			elseif string.find(line, ":") then
				local _, _, name, class = line:find("(%a+%d*)%s*.%s*(%a+%d*)") -- name,any nr of space,any char,any nr of space,class
				currentObject = createObject(name, class)
				
				inObj = true
			end
		end
	end
	
	loadObjects()
	io.close()
	saveMap(path)
end

function saveMap(path)
	io.output(path)
	
	for k, v in pairs(allObjects) do
		if v.name ~= "" then
			io.write(tostring(v), "\n\n")
		end
	end
	
	io.close()
end

function loadObjects()
	for k, v in pairs(allObjects) do
		if getmetatable(v) == ScriptCollider then
			local id = DrawBlock(v.position, v.size, false, v.texture)
			AddCollider(v, id)
		else
			DrawBlock(v.position, v.size, true, v.texture)
		end
	end
end

-- Map Builder
range = 10
objectType = 1

initObject = {
	function(norm, pos) -- Add Platform
		return Solid:new{position = pos + norm * range,
						size = Vector:new{x = 25, y = 0.3, z = 25},
						texture = 0,
						name = "platform"}
	end,
	function(norm, pos) -- Add Normal Coin
		return ScriptCollider:new{position =  pos + norm * range,
								 size = Vector:new{x = 5, y = 5, z = 0.2},
								 texture = 1,
								 id = #allObjects,
								 name = "NormalCoin"}
	end,
	function(norm, pos) -- Add Normal Coin
		return ScriptCollider:new{position =  pos + norm * range,
								 size = Vector:new{x = 0.2, y = 5, z = 5},
								 texture = 1,
								 id = #allObjects,
								 name = "NormalCoin"}
	end
}

buildObject = {}

buildObject["platform"] = function(obj) -- Add platform
		DrawBlock(obj.position, obj.size, true, obj.texture)
end

buildObject["NormalCoin"] = function(obj) -- Add Normal Coint
	local id = DrawBlock(obj.position, obj.size, false, obj.texture)
	AddCollider(obj, id)
end

function increaseRange()
	range = range + 0.25
end

function decreaseRange()
	range = range - 0.25
end

function switchObjectType()
	objectType = objectType + 1
	if (objectType > #initObject) then
		objectType = 1
	end
end

-- Called when in building gamestate
function onBuildingUpdate(inX, inY, inZ, pX, pY, pZ)
	local normal = Vector:new{x = inX, y = inY, z = inZ}
	local playerPosition = Vector:new{x = pX, y = pY, z = pZ}
	
	local object = initObject[objectType](normal, playerPosition)
	DrawBuildingBlock(object.position, object.size, object.texture)
end

function placeObject(inX, inY, inZ, pX, pY, pZ)
	local normal = Vector:new{x = inX, y = inY, z = inZ}
	local playerPosition = Vector:new{x = pX, y = pY, z = pZ}
	
	local object = initObject[objectType](normal, playerPosition)
	table.insert(allObjects, object)
	
	buildObject[object.name](object)
end