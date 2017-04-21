-- @author LW

walkSpeed = 0.05
runSpeed = 0.09
prevWaypoint = 0
currentWaypoint = 1
waypoints = { -- c = connections, length beetwen gets calc in c++
				{x = -6, y = 0, z = 6, c = {2, 4}},
				{x = -6, y = 0, z = -47, c = {1, 7, 5}},
				{x = 45, y = 0, z = -47, c = {7, 4, 6}},
				{x = 45, y = 0, z = 6, c = {1, 3}},
				{x = -6, y = 0, z = -74, c = {2, 6}},
				{x = 45, y = 0, z = -74, c = {5, 3}},
				{x = 17, y = 0, z = -47, c = {2, 3, 8}},
				{x = 17, y = 0, z = -61, c = {7}},
			}

function onStart()
	SetEnemySpeed(walkSpeed)
	SetEnemyWaypoint(waypoints[currentWaypoint])
end

function onReachingWaypoint()
	Log("Reached Waypoint")
	local toCon = math.random(1, #waypoints[currentWaypoint].c)
	local temp = waypoints[currentWaypoint].c[toCon]
	
	if (#waypoints[currentWaypoint].c > 1 and temp == prevWaypoint)
		then return onReachingWaypoint() end
		
	prevWaypoint = currentWaypoint
	currentWaypoint = temp
	SetEnemyWaypoint(waypoints[currentWaypoint])
end

function update()
	local lSeesPlayer = SeesPlayer()
	if lSeesPlayer then
		SetEnemySpeed(runSpeed)
		pathToPlayer()
	end
end

function onReachingPathEnd()
	StopPathing()
	SetEnemyWaypoint(waypoints[currentWaypoint]) -- should get closest waypoint in line of sight
end

function pathToPlayer()
	SetCurrentPathNode(0)
	LoadPathToPlayer()
	StartPathing()
end