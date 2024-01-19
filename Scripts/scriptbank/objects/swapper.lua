-- SWAPPER v8 by Necrym59
-- DESCRIPTION: A one time behavior to swap same named dynamic objects in-game and swap them out for another set of same named dynamic objects when key is pressed.
-- DESCRIPTION: Attach to an object Physics=ON.
-- DESCRIPTION: [PICKUP_RANGE = 80]
-- DESCRIPTION: [USE_PROMPT$ = "X to Use"]
-- DESCRIPTION: [OBJECT_NAME1$=""] to swap out
-- DESCRIPTION: [OBJECT_NAME2$=""] to swap in
-- DESCRIPTION: [!NAVMESH_BLOCK=0]
-- DESCRIPTION: <Sound0> when used

local U = require "scriptbank\\utillib"
local NAVMESH = require "scriptbank\\navmeshlib"
local rad = math.rad

local lower = string.lower
local swapper 			= {}
local pickup_range		= {}
local use_prompt		= {}
local object_name1 		= {}
local object_name2 		= {}

local status			= {}
local terrainh			= {}
local doonce			= {}
local playonce			= {}

function swapper_properties(e, pickup_range, use_prompt, object_name1, object_name2, navmesh_block)
	swapper[e] = g_Entity[e]
	swapper[e].pickup_range = pickup_range
	swapper[e].use_prompt = use_prompt
	swapper[e].object_name1 = lower(object_name1)
	swapper[e].object_name2 = lower(object_name2)
	swapper[e].navmesh_block = navmesh_block
end

function swapper_init(e)
	swapper[e] = {}
	swapper[e].pickup_range = 80
	swapper[e].use_prompt = "X to Use"
	swapper[e].object_name1 = ""
	swapper[e].object_name2 = ""
	swapper[e].navmesh_block = 1
	status[e] = "collect"
	doonce[e] = 0
	SetActivated(e,0)
end

function swapper_main(e)
	swapper[e] = g_Entity[e]

	if status[e] == "collect" then
		local PlayerDist = GetPlayerDistance(e)
		if PlayerDist < swapper[e].pickup_range then
			local LookingAt = GetPlrLookingAtEx(e,1)
			if LookingAt == 1 and GetEntityVisibility(e) == 1 then
				PromptLocal(e,swapper[e].use_prompt)
				if GetInKey() == "x" or GetInKey() == "X" then
					PromptLocal(e,"")
					status[e] = "swapout"
					SetActivated(e,1)
				end
			end
		end
	end

	if g_Entity[e]['activated'] == 1 then
		if status[e] == "swapout" then
			if doonce[e] == 0 then
				PlaySound(e,0)
				ActivateIfUsed(e)
				PerformLogicConnections(e)
				doonce[e] = 1
			end
			for n = 1, g_EntityElementMax do
				if n ~= nil and g_Entity[n] ~= nil then
					if lower(GetEntityName(n)) == swapper[e].object_name1 then
						Hide(n)
						CollisionOff(n)
						SetActivated(n,0)
						Destroy(n)
					end
				end
			end
			for m = 1, g_EntityElementMax do
				if m ~= nil and g_Entity[m] ~= nil then
					if lower(GetEntityName(m)) == swapper[e].object_name2 then
						Spawn(m)
						Show(m)
						CollisionOn(m)
						if swapper[e].navmesh_block == 1 then
							local x,y,z = GetEntityPosAng(m)
							y = RDGetYFromMeshPosition(x,y,z)
							local xmin, ymin, zmin, xmax, ymax, zmax = GetObjectColBox(g_Entity[m]['obj'])
							local sx, sy, sz = GetObjectScales(g_Entity[m]['obj'])
							local w, h, l = (xmax - xmin) * sx, (ymax - ymin) * sy, (zmax - zmin) * sz
							local sizex = w
							local sizez = l
							local blockmode = 1
							local angle = GetEntityAngleY(m)
							RDBlockNavMeshWithShape(x,y,z,w,1,l,angle)
						end
						SetActivated(m,1)
					end
				end
			end
		end
		status[e] = "end"
	end
	if status[e] == "end" then
		SetActivated(e,1)
		doonce[e] = 0
		Destroy(e)
	end
end