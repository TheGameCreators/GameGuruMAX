-- Swap v5 by Necrym59
-- DESCRIPTION: A global behavior that will hide all the same named objects in-game and swap them out for another set of same named objects.
-- DESCRIPTION: Attach to an object Physics=ON. Activate by a linked switch or zone.
-- DESCRIPTION: NPC's can only be swapped once.
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
local posx				= {}
local posy				= {}
local posz				= {}
local angx				= {}
local angy				= {}
local angz				= {}
local isnpc				= {}

function swap_properties(e, object_name1, object_name2, navmesh_block)
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
	posx[e] = 0
	posy[e] = 0
	posz[e] = 0
	angx[e] = 0
	angy[e] = 0
	angz[e] = 0
	isnpc[e] = 0
end

function swap_main(e)
		
	if g_Entity[e]['activated'] == 1 and status[e] == "swapout" then

		if status[e] == "swapout" then
			if doonce[e] == 0 then
				PlaySound(e,0)
				doonce[e] = 1
			end
			for n = 1, g_EntityElementMax do
				if n ~= nil and g_Entity[n] ~= nil then
					if lower(GetEntityName(n)) == swap[e].object_name1 and g_Entity[n]['health'] > 0 then
						posx[e],posy[e],posz[e],angx[e],angy[e],angz[e] = GetEntityPosAng(n)						
						Hide(n)
						CollisionOff(n)
						if GetEntityAllegiance(n) ~= -1 then
							isnpc[e] = 1
							SetEntityActive(n,1)
						end	
					end	 
				end
			end
			for m = 1, g_EntityElementMax do
				if m ~= nil and g_Entity[m] ~= nil then
					if lower(GetEntityName(m)) == swap[e].object_name2 and g_Entity[m]['health'] > 0 then
						Spawn(m)
						ResetPosition(m,posx[e],posy[e],posz[e])
						ResetRotation(m,angx[e],angy[e],angz[e])						
						Show(m)
						CollisionOn(m)
						if GetEntityAllegiance(m) ~= -1 then
							isnpc[e] = 1
							SetEntityActive(m,1)
						end						
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
					end
				end
			end			
			status[e] = "swapback"
		end
		doonce[e] = 0
		SetActivated(e,0)
	end
	
	if g_Entity[e]['activated'] == 1 and status[e] == "swapback" and isnpc[e] == 0 then

		if status[e] == "swapback" then
			if doonce[e] == 0 then
				PlaySound(e,0)
				doonce[e] = 1
			end		
			for n = 1, g_EntityElementMax do
				if n ~= nil and g_Entity[n] ~= nil then
					if lower(GetEntityName(n)) == swap[e].object_name2 and g_Entity[n]['health'] > 0 then	
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
						posx[e],posy[e],posz[e],angx[e],angy[e],angz[e] = GetEntityPosAng(n)
						Hide(n)
						CollisionOff(n)
						SetEntityActive(n,1)
					end			
				end
			end
			for m = 1, g_EntityElementMax do
				if m ~= nil and g_Entity[m] ~= nil then
					if lower(GetEntityName(m)) == swap[e].object_name1 and g_Entity[m]['health'] then
						Spawn(m)						
						ResetPosition(m,posx[e],posy[e],posz[e])
						ResetRotation(m,angx[e],angy[e],angz[e])
						Show(m)
						CollisionOn(m)
						SetEntityActive(m,1)
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
					end
				end
			end
			status[e] = "swapout"
		end
		doonce[e] = 0
		SetActivated(e,0)
	end	
end