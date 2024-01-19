-- Swap v4 by Necrym59
-- DESCRIPTION: A global behavior that will hide all the same named objects in-game and swap them out for another set of same named objects.
-- DESCRIPTION: Attach to an object Physics=ON. Activate by a linked switch or zone.
-- DESCRIPTION: [OBJECT_NAME1$=""] to swap out
-- DESCRIPTION: [OBJECT_NAME2$=""] to swap in
-- DESCRIPTION: [!NAVMESH_BLOCK=0]
-- DESCRIPTION: <Sound0> when swapping

local lower = string.lower
local swap 				= {}
local object_name1 		= {}
local object_name2 		= {}
local navmesh_block		= {}

local status			= {}
local doonce			= {}

function swap_properties(e, object_name1, object_name2, navmesh_block)
	swap[e] = g_Entity[e]
	swap[e].object_name1 = lower(object_name1)
	swap[e].object_name2 = lower(object_name2)
	swap[e].navmesh_block = navmesh_block or 0
end 

function swap_init(e)
	swap[e] = {}
	swap[e].object_name1 = ""
	swap[e].object_name2 = ""
	swap[e].navmesh_block = 0	
	status[e] = "swapout"
	doonce[e] = 0
end

function swap_main(e)
	swap[e] = g_Entity[e]
	
	if g_Entity[e]['activated'] == 1 and status[e] == "swapout" then
	
		if status[e] == "swapout" then
			if doonce[e] == 0 then
				PlaySound(e,0)
				doonce[e] = 1
			end
			for n = 1, g_EntityElementMax do
				if n ~= nil and g_Entity[n] ~= nil then
					if lower(GetEntityName(n)) == swap[e].object_name1 then
						Hide(n)
						CollisionOff(n)
						g_Entity[n]['activated'] = 0
					end			
				end
			end
			for m = 1, g_EntityElementMax do
				if m ~= nil and g_Entity[m] ~= nil then
					if lower(GetEntityName(m)) == swap[e].object_name2 then												
						Spawn(m)
						Show(m)
						CollisionOn(m)
						if swap[e].navmesh_block == 1 then
							local x,y,z = GetEntityPosAng(m)
							y = RDGetYFromMeshPosition(x,y,z)
							local xmin, ymin, zmin, xmax, ymax, zmax = GetObjectColBox(g_Entity[m]['obj'])
							local sx, sy, sz = GetObjectScales(g_Entity[m]['obj'])
							local w, h, l = (xmax - xmin) * sx, (ymax - ymin) * sy, (zmax - zmin) * sz
							local sizex = w
							local sizez = l
							local angle = GetEntityAngleY(m)
							local blockmode = 1					
							RDBlockNavMeshWithShape(x,y,z,w,1,l,angle)
						end	
						g_Entity[m]['activated'] = 1
					end
				end
			end
			status[e] = "swapback"			
		end
		doonce[e] = 0
		SetActivated(e,0)
	end
	
	if g_Entity[e]['activated'] == 1 and status[e] == "swapback" then
	
		if status[e] == "swapback" then
			if doonce[e] == 0 then
				PlaySound(e,0)
				doonce[e] = 1
			end		
			for n = 1, g_EntityElementMax do
				if n ~= nil and g_Entity[n] ~= nil then
					if lower(GetEntityName(n)) == swap[e].object_name2 then	
						if swap[e].navmesh_block == 1 then
							local x,y,z = GetEntityPosAng(n)
							y = RDGetYFromMeshPosition(x,y,z)
							local xmin, ymin, zmin, xmax, ymax, zmax = GetObjectColBox(g_Entity[n]['obj'])
							local sx, sy, sz = GetObjectScales(g_Entity[n]['obj'])
							local w, h, l = (xmax - xmin) * sx, (ymax - ymin) * sy, (zmax - zmin) * sz
							local sizex = w
							local sizez = l
							local angle = GetEntityAngleY(n)
							local blockmode = 0
							RDBlockNavMeshWithShape(x,y,z,w,blockmode,l,angle)
						end					
						Hide(n)
						CollisionOff(n)
						SetActivated(n,0)
					end			
				end
			end
			for m = 1, g_EntityElementMax do
				if m ~= nil and g_Entity[m] ~= nil then
					if lower(GetEntityName(m)) == swap[e].object_name1 then
						Spawn(m)
						Show(m)
						CollisionOn(m)	
						if swap[e].navmesh_block == 1 then
							local x,y,z = GetEntityPosAng(m)
							y = RDGetYFromMeshPosition(x,y,z)
							local xmin, ymin, zmin, xmax, ymax, zmax = GetObjectColBox(g_Entity[m]['obj'])
							local sx, sy, sz = GetObjectScales(g_Entity[m]['obj'])
							local w, h, l = (xmax - xmin) * sx, (ymax - ymin) * sy, (zmax - zmin) * sz
							local sizex = w
							local sizez = l
							local angle = GetEntityAngleY(m)
							local blockmode = 1					
							RDBlockNavMeshWithShape(x,y,z,w,blockmode,l,angle)
						end
						SetActivated(m,1)
					end
				end
			end
			status[e] = "swapout"			
		end
		doonce[e] = 0
		SetActivated(e,0)
	end
end