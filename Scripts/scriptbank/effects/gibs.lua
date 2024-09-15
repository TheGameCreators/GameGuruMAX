-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Gibs v16 - by Necrym,59
-- DESCRIPTION: Will randomly gib a weakened character in range when their health is low. Attach to an object and set Always Active.
-- DESCRIPTION: Ensure all named gib objects are on the map and set Physics=On, Gravity=ON, and Always Active=ON.
-- DESCRIPTION: [RANGE=1000(100,3000)]
-- DESCRIPTION: [GIB1_NAME$="gib 1"] for gib object
-- DESCRIPTION: [GIB2_NAME$="gib 2"] for gib object
-- DESCRIPTION: [GIB3_NAME$="gib 3"] for gib object
-- DESCRIPTION: [GIB4_NAME$="gib 4"] for gib object
-- DESCRIPTION: [GIB5_NAME$="gib 5"] for gib object
-- DESCRIPTION: [GIB6_NAME$="gib 6"] for gib object
-- DESCRIPTION: [SPLAT_NAME$="blood_splat"] for blood splat plain
-- DESCRIPTION: [LIFESPAN=10(1,60)] Seconds
-- DESCRIPTION: [@CLEANUP=1(1=Instant, 2=Discrete)]
-- DESCRIPTION: [GLOBAL_ENABLED!=0]
-- DESCRIPTION: <Sound0> when gibbed.

local U = require "scriptbank\\utillib"
g_GibsEnabled = {}

local lower = string.lower
local gibs 			= {}
local range 		= {}
local gib1_name 	= {}
local gib2_name 	= {}
local gib3_name 	= {}
local gib4_name 	= {}
local gib5_name 	= {}
local gib6_name 	= {}
local splat_name 	= {}
local lifespan		= {}
local cleanup		= {}

local gib1_no 		= {}
local gib2_no 		= {}
local gib3_no 		= {}
local gib4_no 		= {}
local splat_no 		= {}

local pEntno 		= {}
local allegiance 	= {}
local seenobj 		= {}
local etoclone		= {}
local gibclones		= {}
local lifetimer		= {}
local wait			= {}
local fade_level	= {}
local check_timer 	= {}
local status		= {}
local tableName 	= {}
local npclist		= {}
local giblist		= {}
local fade_level	= {}

function gibs_properties(e, range, gib1_name, gib2_name, gib3_name, gib4_name, gib5_name, gib6_name, splat_name, lifespan, cleanup, global_enabled)
	gibs[e].range = range
	gibs[e].gib1_name = lower(gib1_name)
	gibs[e].gib2_name = lower(gib2_name)
	gibs[e].gib3_name = lower(gib3_name)
	gibs[e].gib4_name = lower(gib4_name)
	gibs[e].gib5_name = lower(gib5_name)
	gibs[e].gib6_name = lower(gib6_name)	
	gibs[e].splat_name = lower(splat_name)
	gibs[e].lifespan = lifespan
	gibs[e].cleanup = cleanup
	gibs[e].global_enabled = global_enabled or 0
end

function gibs_init(e)
	gibs[e] = {}
	gibs[e].range = 1000
	gibs[e].gib1_name = ""
	gibs[e].gib2_name = ""
	gibs[e].gib3_name = ""
	gibs[e].gib4_name = ""
	gibs[e].gib5_name = ""
	gibs[e].gib6_name = ""	
	gibs[e].splat_name = ""
	gibs[e].lifespan = 10
	gibs[e].cleanup = 1
	gibs[e].global_enabled = 0

	gibs[e].gib1_no = 0
	gibs[e].gib2_no = 0
	gibs[e].gib3_no = 0
	gibs[e].gib4_no = 0
	gibs[e].gib5_no = 0
	gibs[e].gib6_no = 0	
	gibs[e].splat_no = 0

	wait[e] = math.huge
	lifetimer[e] = math.huge
	check_timer[e] = 0
	seenobj[e] = 0	
	fade_level[e] = 100		
	gibclones[e] = 0
	g_GibsEnabled = 0
	status[e] = "init"

	math.randomseed(os.time())
end

function gibs_main(e)
	if status[e] == "init" then
		if gibs[e].global_enabled == 1 then g_GibsEnabled = 1 end
		if gibs[e].gib1_name > "" and gibs[e].gib1_no == 0 then
			for m = 1, g_EntityElementMax do
				if m ~= nil and g_Entity[m] ~= nil then
					if lower(GetEntityName(m)) == gibs[e].gib1_name then
						gibs[e].gib1_no = m
						SetEntityBaseAlpha(m,100)
						SetEntityTransparency(m,1)
						Hide(m)
						CollisionOff(m)
						GravityOn(m)
						break
					end
				end
			end
		end
		if gibs[e].gib2_name > "" and gibs[e].gib2_no == 0 then
			for n = 1, g_EntityElementMax do
				if n ~= nil and g_Entity[n] ~= nil then
					if lower(GetEntityName(n)) == gibs[e].gib2_name then
						gibs[e].gib2_no = n
						SetEntityBaseAlpha(n,100)
						SetEntityTransparency(n,1)
						Hide(n)
						CollisionOff(n)
						GravityOn(n)						
						break
					end
				end
			end
		end
		if gibs[e].gib3_name > "" and gibs[e].gib3_no == 0 then
			for o = 1, g_EntityElementMax do
				if o ~= nil and g_Entity[o] ~= nil then
					if lower(GetEntityName(o)) == gibs[e].gib3_name then
						gibs[e].gib3_no = o
						SetEntityBaseAlpha(o,100)
						SetEntityTransparency(o,1)						
						Hide(o)
						CollisionOff(o)
						GravityOn(o)
						break
					end
				end
			end
		end
		if gibs[e].gib4_name > "" and gibs[e].gib4_no == 0 then
			for p = 1, g_EntityElementMax do
				if p ~= nil and g_Entity[p] ~= nil then
					if lower(GetEntityName(p)) == gibs[e].gib4_name then
						gibs[e].gib4_no = p
						SetEntityBaseAlpha(p,100)
						SetEntityTransparency(p,1)						
						Hide(p)
						CollisionOff(p)
						GravityOn(p)						
						break
					end
				end
			end
		end
		if gibs[e].gib5_name > "" and gibs[e].gib5_no == 0 then
			for q = 1, g_EntityElementMax do
				if q ~= nil and g_Entity[q] ~= nil then
					if lower(GetEntityName(q)) == gibs[e].gib5_name then
						gibs[e].gib5_no = q
						SetEntityBaseAlpha(q,100)
						SetEntityTransparency(q,1)						
						Hide(q)
						CollisionOff(q)
						GravityOn(q)
						break
					end
				end
			end
		end
		if gibs[e].gib6_name > "" and gibs[e].gib6_no == 0 then
			for r = 1, g_EntityElementMax do
				if r ~= nil and g_Entity[r] ~= nil then
					if lower(GetEntityName(r)) == gibs[e].gib6_name then
						gibs[e].gib6_no = r
						SetEntityBaseAlpha(r,100)
						SetEntityTransparency(r,1)						
						Hide(r)
						CollisionOff(r)
						GravityOn(r)						
						break
					end
				end
			end
		end
		if gibs[e].splat_name > "" and gibs[e].splat_no == 0 then
			for s = 1, g_EntityElementMax do
				if s ~= nil and g_Entity[s] ~= nil then
					if lower(GetEntityName(s)) == gibs[e].splat_name then
						gibs[e].splat_no = s
						SetEntityBaseAlpha(s,100)
						SetEntityTransparency(s,1)						
						Hide(s)
						CollisionOff(s)
						GravityOn(s)						
						break
					end
				end
			end
		end
		for r = 1, g_EntityElementMax do
			if r ~= nil and g_Entity[r] ~= nil then
				local allegiance = GetEntityAllegiance(r)
					if allegiance ~= -1 then
					table.insert(npclist,r)
				end
			end
		end
		status[e] = "check"
	end

	if status[e] == "check" then
		--- Find entity in view range ---
		if g_Time > check_timer[e] then
			seenobj[e]= U.ObjectPlayerLookingAt(gibs[e].range)
			if seenobj[e] == 0 then	g_Entity[e]['obj'] = 0 end
			if seenobj[e] > 0 then
				for a,b in pairs(npclist) do
					if g_Entity[b]['obj'] == seenobj[e] then
						if g_Entity[b]['health'] < math.random(1,3) and g_Entity[b]['active'] == 1 then
							pEntno[e] = b
							status[e] = "gib"							
							break
						end
					end
				end
			end			
			check_timer[e] = g_Time + 10
		end
	end
	
	if status[e] == "gib" then
		if gibs[e].gib1_no > 0 then
			local etoclone = gibs[e].gib1_no
			local newEntn = SpawnNewEntity(etoclone)
			local newposx = g_Entity[pEntno[e]]['x'] + math.random(-10,10)
			local newposy = g_Entity[pEntno[e]]['y'] + math.random(30,65)
			local newposz = g_Entity[pEntno[e]]['z'] + math.random(-10,10)
			ResetPosition(newEntn,newposx,newposy,newposz)
			ResetRotation(newEntn,g_Entity[etoclone]['anglex']+math.random(0,359), g_Entity[etoclone]['angley']+math.random(0,359),g_Entity[etoclone]['anglez']+math.random(0,359))
			SetEntityBaseAlpha(newEntn,100)
			SetEntityTransparency(newEntn,1)
			Show(newEntn)
			gibclones[e] = gibclones[e] + 1
			table.insert(giblist,newEntn)
			lifetimer[e] = g_Time + (gibs[e].lifespan*1000)
		end
		if gibs[e].gib2_no > 0 then
			local etoclone = gibs[e].gib2_no
			local newEntn = SpawnNewEntity(etoclone)
			local newposx = g_Entity[pEntno[e]]['x'] + math.random(-10,10)
			local newposy = g_Entity[pEntno[e]]['y'] + math.random(30,65)
			local newposz = g_Entity[pEntno[e]]['z'] + math.random(-10,10)
			ResetPosition(newEntn,newposx,newposy,newposz)
			ResetRotation(newEntn,g_Entity[etoclone]['anglex']+math.random(0,359), g_Entity[etoclone]['angley']+math.random(0,359),g_Entity[etoclone]['anglez']+math.random(0,359))
			SetEntityBaseAlpha(newEntn,100)
			SetEntityTransparency(newEntn,1)
			Show(newEntn)			
			gibclones[e] = gibclones[e] + 1
			table.insert(giblist,newEntn)
			lifetimer[e] = g_Time + (gibs[e].lifespan*1000)
		end
		if gibs[e].gib3_no > 0 then
			local etoclone = gibs[e].gib3_no
			local newEntn = SpawnNewEntity(etoclone)
			local newposx = g_Entity[pEntno[e]]['x'] + math.random(-10,10)
			local newposy = g_Entity[pEntno[e]]['y'] + math.random(30,65)
			local newposz = g_Entity[pEntno[e]]['z'] + math.random(-10,10)
			ResetPosition(newEntn,newposx,newposy,newposz)
			ResetRotation(newEntn,g_Entity[etoclone]['anglex']+math.random(0,359), g_Entity[etoclone]['angley']+math.random(0,359),g_Entity[etoclone]['anglez']+math.random(0,359))
			SetEntityBaseAlpha(newEntn,100)
			SetEntityTransparency(newEntn,1)
			Show(newEntn)
			gibclones[e] = gibclones[e] + 1
			table.insert(giblist,newEntn)
			lifetimer[e] = g_Time + (gibs[e].lifespan*1000)
		end
		if gibs[e].gib4_no > 0 then
			local etoclone = gibs[e].gib4_no
			local newEntn = SpawnNewEntity(etoclone)
			local newposx = g_Entity[pEntno[e]]['x'] + math.random(-10,10)
			local newposy = g_Entity[pEntno[e]]['y'] + math.random(30,65)
			local newposz = g_Entity[pEntno[e]]['z'] + math.random(-10,10)
			ResetPosition(newEntn,newposx,newposy,newposz)
			ResetRotation(newEntn,g_Entity[etoclone]['anglex']+math.random(0,359), g_Entity[etoclone]['angley']+math.random(0,359),g_Entity[etoclone]['anglez']+math.random(0,359))
			Show(newEntn)
			gibclones[e] = gibclones[e] + 1
			table.insert(giblist,newEntn)
			lifetimer[e] = g_Time + (gibs[e].lifespan*1000)
		end
		if gibs[e].gib5_no > 0 then
			local etoclone = gibs[e].gib5_no
			local newEntn = SpawnNewEntity(etoclone)
			local newposx = g_Entity[pEntno[e]]['x'] + math.random(-10,10)
			local newposy = g_Entity[pEntno[e]]['y'] + math.random(30,65)
			local newposz = g_Entity[pEntno[e]]['z'] + math.random(-10,10)
			ResetPosition(newEntn,newposx,newposy,newposz)
			ResetRotation(newEntn,g_Entity[etoclone]['anglex']+math.random(0,359), g_Entity[etoclone]['angley']+math.random(0,359),g_Entity[etoclone]['anglez']+math.random(0,359))
			Show(newEntn)
			gibclones[e] = gibclones[e] + 1
			table.insert(giblist,newEntn)
			lifetimer[e] = g_Time + (gibs[e].lifespan*1000)
		end
		if gibs[e].gib6_no > 0 then
			local etoclone = gibs[e].gib6_no
			local newEntn = SpawnNewEntity(etoclone)
			local newposx = g_Entity[pEntno[e]]['x'] + math.random(-10,10)
			local newposy = g_Entity[pEntno[e]]['y'] + math.random(30,65)
			local newposz = g_Entity[pEntno[e]]['z'] + math.random(-10,10)
			ResetPosition(newEntn,newposx,newposy,newposz)
			ResetRotation(newEntn,g_Entity[etoclone]['anglex']+math.random(0,359), g_Entity[etoclone]['angley']+math.random(0,359),g_Entity[etoclone]['anglez']+math.random(0,359))
			Show(newEntn)
			gibclones[e] = gibclones[e] + 1
			table.insert(giblist,newEntn)
			lifetimer[e] = g_Time + (gibs[e].lifespan*1000)
		end	
		if gibs[e].splat_no > 0 then
			local etoclone = gibs[e].splat_no
			local newEntn = SpawnNewEntity(etoclone)
			local newposx = g_Entity[pEntno[e]]['x'] + math.random(-10,10)
			local newposy = g_Entity[pEntno[e]]['y']
			local newposz = g_Entity[pEntno[e]]['z'] + math.random(-10,10)
			local newposy = GetSurfaceHeight(newposx,g_Entity[pEntno[e]]['y']-1,newposz)
			CollisionOff(newEntn)
			GravityOff(newEntn)
			ResetPosition(newEntn,newposx,newposy,newposz)
			ResetRotation(newEntn,0, g_Entity[etoclone]['angley']+math.random(0,359),0)
			SetEntityBaseAlpha(newEntn,100)			
			Show(newEntn)
			gibclones[e] = gibclones[e] + 1
			table.insert(giblist,newEntn)
			lifetimer[e] = g_Time + (gibs[e].lifespan*1000)
		end
		--------------------------------------		
		Hide(pEntno[e])
		CollisionOff(pEntno[e])
		Destroy(pEntno[e])
		PlaySound(e,0)
		pEntno[e] = 0		
		status[e] = "check"
	end
	
	if gibs[e].cleanup == 1 then
		if g_Time > lifetimer[e] then
			if gibclones[e] > 0 then
				for a,b in pairs (giblist) do
					if g_Entity[b] ~= nil then
						DeleteNewEntity(b)
						giblist[a] = nil
					end
					gibclones[e] = 0
				end
			end
		end
	end	
	if gibs[e].cleanup == 2 then
		if g_Time > lifetimer[e] then
			if gibclones[e] > 0 then
				for a,b in pairs (giblist) do
					if g_Entity[b] ~= nil then						
						if fade_level[e] > 0 then
							SetEntityBaseAlpha(b,fade_level[e])
							fade_level[e] = fade_level[e]-1
						end
						if fade_level[e] <= 0 then													
							DeleteNewEntity(b)						
							giblist[a] = nil
							gibclones[e] = gibclones[e] - 1
							fade_level[e] = 100
						end
						break
					end			
				end
			end			
		end		
	end	
end