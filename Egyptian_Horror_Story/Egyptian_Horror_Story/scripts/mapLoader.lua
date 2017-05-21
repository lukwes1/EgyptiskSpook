print = Log

-- General Functions

function templateNewMethod(self, o)
	o = o or {}
	self.__index = self -- because inheritence
	setmetatable(o, self)
	return o
end

function createObject(inName, class)
	if (class == "ScriptCollider") then
		return ScriptCollider:new{name = inName}
	elseif (class == "Solid") then
		return Solid:new{name = inName}
	end
end

function loadObject(line, currentObject)
	_, _, var = line:find("%s*(%a+%d*)")
	memberVar = currentObject[var]

	if getmetatable(memberVar) == Vector then -- Vector
		currentObject[var] = Vector:createFromFile(line)
	else -- string
		_, _, currentObject[var] = line:find("%s+\"(.*)\"")
	end
end

-- Vector Class

Vector = {x = 0, y = 0, z = 0}

function Vector:__tostring()
	return self.x .. " " .. self.y .. " " .. self.z
end	

function Vector.filePattern()
	return "%s+(%d+)%s+(%d+)%s+(%d+)"
end	

function Vector:createFromFile(line)
	_, _, inX, inY, inZ = line:find(self.filePattern())
	return self:new{x = inX, y = inY, z = inZ}
end

Vector.new = templateNewMethod

-- Loadable Object Class

LoadableObject = {name = "", class = ""}
LoadableObject.new = templateNewMethod

function LoadableObject.baseString(obj)
	return obj.name .. " : " .. obj.class
end

-- A Solid Class 
Solid = LoadableObject:new{position = Vector:new(), size = Vector:new(), class = "Solid"}

function Solid:__tostring()
	return self:baseString() ..
	"\nposition " .. tostring(self.position) ..
	"\nsize " .. tostring(self.size) ..
	"\nend"
end	

Solid.new = templateNewMethod

-- ScriptCollider class, a coded collider, will run a script on collision, that's pretty much it
ScriptCollider = LoadableObject:new{position = Vector:new(), size = Vector:new(), script = "", class = "ScriptCollider"}

function ScriptCollider:__tostring()
	return self:baseString() .. 
		   "\nscript " .. self.script ..
		   "\nposition " .. tostring(self.position) ..
		   "\nsize " .. tostring(self.size) ..
		   "\nend"
end	

ScriptCollider.new = templateNewMethod

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
		io.write(tostring(v))
	end
	
	io.close()
end

function loadObjects()
	for k, v in pairs(allObjects) do
		print(tostring(v))
	end
end