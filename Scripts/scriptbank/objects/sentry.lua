-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Sentry v18
-- DESCRIPTION: The applied object will create a rotatable security device? Static Mode = No, Physics = On, Explodable = Yes, IsImobile = Yes
-- DESCRIPTION: Change the [PROMPT_TEXT$="Secuity alert was started"], the detection [SCAN_RANGE=1000], [SCAN_RADIUS=136(1,360)], [SCAN_SPEED=3(1,10)]. Detection [@ALARM=2(1=Off, 2=On)] and [ALARM_RANGE=500(1,3000)] [SENTRY_DAMAGE=60] inflicted on player, [MUZZLE_FLASH_NAME$="sentry muzzleflash"], [MUZZLE_Y=46(1,100)],[MUZZLE_Z=47(1,100)], Change its [@Allegiance=0(0=Enemy,1=Ally,2=Neutral)] also [@VISIBILITY=1(1=Visible, 2=Invisible)] <Sound0> - Sentry scan Loop  <Sound1> - Sentry Firing

local U = require "scriptbank\\utillib"
local Q = require "scriptbank\\quatlib"
local rad = math.rad	
local deg = math.deg
local atan = math.atan2
local lower = string.lower
			
g_sentry = {}
local prompt_text = {}
local scan_range = {}
local scan_radius = {}	
local scan_speed = {}
local alarm = {}
local alarm_range = {}
local sentry_damage = {}
local muzzle_flash_name = {}
local muzzle_flash = {}
local muzzle_y = {}
local muzzle_z = {}
local allegiance = {}	
local visibility = {}	
local current_angle = {}
local target_angle = {}
local start_angle = {}
local sweep = {}
local status = {}
local alert = {}
local sentrystart = {}
local sentrystop = {}
local target_distance = {}
local engaging_enemy = {}

function sentry_properties(e, prompt_text, scan_range, scan_radius, scan_speed, alarm, alarm_range, sentry_damage, muzzle_flash_name, muzzle_y, muzzle_z, allegiance, visibility)
	g_sentry[e] = g_Entity[e]
	g_sentry[e]['prompt_text'] = prompt_text	
	g_sentry[e]['scan_range'] = scan_range	
	g_sentry[e]['scan_radius'] = scan_radius
	g_sentry[e]['scan_speed'] = scan_speed
	g_sentry[e]['alarm'] = alarm
	g_sentry[e]['alarm_range'] = alarm_range
	g_sentry[e]['sentry_damage'] = sentry_damage
	g_sentry[e]['muzzle_flash_name'] = lower(muzzle_flash_name)
	g_sentry[e]['muzzle_flash'] = 0
	g_sentry[e]['muzzle_y'] = muzzle_y
	g_sentry[e]['muzzle_z'] = muzzle_z
	g_sentry[e]['allegiance'] = allegiance
	g_sentry[e]['visibility'] = visibility
end -- End properties

function sentry_init(e)
	g_sentry[e] = g_Entity[e]
	g_sentry[e]['prompt_text'] = "Secuity alert was started"
	g_sentry[e]['scan_range'] = 1000	
	g_sentry[e]['scan_radius'] = 136
	g_sentry[e]['scan_speed'] = 3
	g_sentry[e]['alarm'] = 2
	g_sentry[e]['alarm_range'] = 500	
	g_sentry[e]['sentry_damage'] = 60
	g_sentry[e]['muzzle_flash_name'] = "sentry muzzleflash"
	g_sentry[e]['muzzle_flash'] = 0
	g_sentry[e]['muzzle_y'] = 46
	g_sentry[e]['muzzle_z'] = 47
	g_sentry[e]['allegiance'] = 0
	g_sentry[e]['visibility'] = 1
	current_angle[e] = -1
	target_angle[e] = 0
	start_angle[e] = g_Entity[e]['angley']
	sweep[e] = 0
	status[e] = 'init'
	alert[e] = 0
	engaging_enemy[e] = 0
	sentrystart = 0
	sentrystop = 0
	target_distance = 0
	if g_Entity[e]['health'] <= 100 then SetEntityHealth(e,g_Entity[e]['health']+100) end
end

function sentry_main(e)
	g_sentry[e] = g_Entity[e]	
	
	if status[e] == 'init' then
		if g_sentry[e]['alarm'] == nil then g_sentry[e]['alarm'] = 1 end		
		if g_sentry[e]['visibility'] == nil then g_sentry[e]['visibility'] = 1 end
		if g_sentry[e]['visibility'] == 1 then Show(e) end
		if g_sentry[e]['visibility'] == 2 then Hide(e) end
		if g_Entity[e]['health'] <= 100 then SetEntityHealth(e,g_Entity[e]['health']+100) end
		current_angle[e] = start_angle[e]		
		if g_sentry[e]['muzzle_flash'] == 0 then
			for a = 1, g_EntityElementMax do			
				if a ~= nil and g_Entity[a] ~= nil then										
					if lower(GetEntityName(a)) == g_sentry[e]['muzzle_flash_name'] then
						g_sentry[e]['muzzle_flash'] = a
						Hide(a)
						break
					end					
				end
			end
		end		
		if g_sentry[e]['muzzle_flash'] > 0 then
			Ent = g_Entity[e]
			local x, y, z, xa, ya, za = GetObjectPosAng( Ent.obj )
			x=x+math.sin(math.rad(ya))*g_sentry[e]['muzzle_z']
			y=y-math.sin(math.rad(xa))*g_sentry[e]['muzzle_z']
			z=z+math.cos(math.rad(ya))*g_sentry[e]['muzzle_z']			
			ResetPosition(g_sentry[e]['muzzle_flash'], x,y+g_sentry[e]['muzzle_y'],z)		
			ResetRotation(g_sentry[e]['muzzle_flash'], xa,ya-180,za)
		end
		Hide(g_sentry[e]['muzzle_flash'])
		status[e] = 'scanning'			
	end
	--if player in range to hack	
	if GetPlayerDistance(e) < 80 then
		Prompt("H to hack device")				
		local key = GetKeyPressed(e, g_InKey)
		if key == "h" then
			g_sentry[e]['allegiance'] = g_sentry[e]['allegiance'] + 1
			if g_sentry[e]['allegiance'] > 2 then 
				g_sentry[e]['allegiance'] = 0				
			end
		end
		if g_sentry[e]['allegiance'] == 0 then TextCenterOnX(50,90,3,"Gun Allegiance: Enemy") end
		if g_sentry[e]['allegiance'] == 1 then TextCenterOnX(50,90,3,"Gun Allegiance: Ally") end	
		if g_sentry[e]['allegiance'] == 2 then TextCenterOnX(50,90,3,"Gun Allegiance: Neutral") end			
	end	
	
	--if Gun allegiance is ally
	if g_sentry[e]['allegiance'] == 1 then 
		if engaging_enemy[e] == 0 and status[e] == 'scanning' then
			if current_angle[e] < g_sentry[e]['scan_radius'] + start_angle[e] and sweep[e] == 0 then
				current_angle[e] = current_angle[e] + (g_sentry[e]['scan_speed']/10)
				CollisionOff(e)
				SetRotation(e,0,current_angle[e],0)
				CollisionOn(e)
				LoopSound(e,0)
				if current_angle[e] >= g_sentry[e]['scan_radius'] + start_angle[e] then sweep[e] = 1 end
				if g_sentry[e]['muzzle_flash'] > 0 then
					Ent = g_Entity[e]
					local x, y, z, xa, ya, za = GetObjectPosAng( Ent.obj )
					x=x+math.sin(math.rad(ya))*g_sentry[e]['muzzle_z']
					y=y-math.sin(math.rad(xa))*g_sentry[e]['muzzle_z']
					z=z+math.cos(math.rad(ya))*g_sentry[e]['muzzle_z']			
					ResetPosition(g_sentry[e]['muzzle_flash'], x,y+g_sentry[e]['muzzle_y'],z)
					ResetRotation(g_sentry[e]['muzzle_flash'], xa,ya-180,za)
				end		
			end
			if sweep[e] == 1 then
				current_angle[e] = current_angle[e] - (g_sentry[e]['scan_speed']/10)
				CollisionOff(e)
				SetRotation(e,0,current_angle[e],0)
				CollisionOn(e)
				SetSoundVolume(120)
				LoopSound(e,0)
				if current_angle[e] <= start_angle[e] then sweep[e] = 0 end
				if g_sentry[e]['muzzle_flash'] > 0 then
					Ent = g_Entity[e]
					local x, y, z, xa, ya, za = GetObjectPosAng( Ent.obj )
					x=x+math.sin(math.rad(ya))*g_sentry[e]['muzzle_z']
					y=y-math.sin(math.rad(xa))*g_sentry[e]['muzzle_z']
					z=z+math.cos(math.rad(ya))*g_sentry[e]['muzzle_z']			
					ResetPosition(g_sentry[e]['muzzle_flash'], x,y+g_sentry[e]['muzzle_y'],z)		
					ResetRotation(g_sentry[e]['muzzle_flash'], xa,ya-180,za)
				end			
			end			
		end
		--if npc is in range
		for a = 1, g_EntityElementMax do						
			if a ~= nil and g_Entity[a] ~= nil and math.ceil(GetFlatDistance(e,a)) <= g_sentry[e]['scan_range'] and g_Entity[a]['health'] > 1 then				
				local allegiance = GetEntityAllegiance(a) -- get the allegiance value for this entity (0-enemy, 1-ally, 2-neutral)					
				if allegiance == 0 then  --Gun is Ally - Enemies are targets					
					----PromptLocal(a, "Health: "..g_Entity[a]['health']) --For testing only---
					target_distance = math.ceil(GetFlatDistance(e,a))
					local enthealth = g_Entity[a]['health']
					status[e] = 'alarm'					
					if EntityLooking(e,a,g_sentry[e]['scan_range'],g_sentry[e]['scan_radius']/8) == 1 and math.ceil(GetFlatDistance(e,a)) <= g_sentry[e]['scan_range'] and g_Entity[a]['health'] > 1 then
						engaging_enemy[e] = 1
						x = g_Entity[a]['x'] - g_Entity[e]['x']
						z = g_Entity[a]['z'] - g_Entity[e]['z']
						target_angle[e] = math.atan2(x,z)
						target_angle[e] = target_angle[e] * (180.0 / math.pi)
						if target_angle[e] < 0 then
							target_angle[e] = g_sentry[e]['scan_radius'] + target_angle[e]
						elseif target_angle[e] > g_sentry[e]['scan_radius'] then
							target_angle[e] = target_angle[e] - g_sentry[e]['scan_radius']
						end		
						if current_angle[e] < target_angle[e] then
							current_angle[e] = current_angle[e] + (g_sentry[e]['scan_speed']/10)
						elseif current_angle[e]> target_angle[e] then
							current_angle[e] = current_angle[e] - (g_sentry[e]['scan_speed']/10)
						end						
						if current_angle[e] > g_sentry[e]['scan_radius'] + start_angle[e] then
							current_angle[e] = start_angle[e]
						end
						if current_angle[e] > 360 then current_angle[e] = 0 end
						if current_angle[e] < 0 then current_angle[e] = 0 end
						CollisionOff(e)						
						SetRotation(e,0,current_angle[e],0)					
						CollisionOn(e)						
						if g_sentry[e]['allegiance'] == 1 and g_Entity[a]['health'] > 1 then
							RotateToPlayer(a)
							current_angle[e] = g_Entity[e]['angley']							
							if g_Entity[a]['health'] > 5 then								
								LoopSound(e,1)								
							else
								StopSound(e,1)								
							end	
							if g_sentry[e]['muzzle_flash'] > 0 then
								Ent = g_Entity[e]
								local x, y, z, xa, ya, za = GetObjectPosAng( Ent.obj )
								x=x+math.sin(math.rad(ya))*g_sentry[e]['muzzle_z']
								y=y-math.sin(math.rad(xa))*g_sentry[e]['muzzle_z']
								z=z+math.cos(math.rad(ya))*g_sentry[e]['muzzle_z']			
								ResetPosition(g_sentry[e]['muzzle_flash'], x,y+g_sentry[e]['muzzle_y'],z)
								ResetRotation(g_sentry[e]['muzzle_flash'], xa,ya-180,za)								
								Show(g_sentry[e]['muzzle_flash'])								
							end	
							RotateToPlayer(a)
							current_angle[e] = g_Entity[e]['angley']
							enthealth = enthealth -5
							SetEntityHealth(a,enthealth)
							break
						end
					end
					if EntityLooking(e,a,g_sentry[e]['scan_range'],g_sentry[e]['scan_radius']/8) == 0 and status[e] == 'alarm' then			
						status[e] = 'scanning'
						target_angle[e] = -1						
					end					
				end	
			else				
				status[e] = 'scanning'
				target_angle[e] = -1								
				engaging_enemy[e] = 0
				Hide(g_sentry[e]['muzzle_flash'])
			end 
		end
	end	
		
	--if Gun allegiance is enemy
	if g_sentry[e]['allegiance'] == 0 then	
		if GetPlayerDistance(e) > g_sentry[e]['scan_range'] and status[e] == 'scanning' then
			if current_angle[e] < g_sentry[e]['scan_radius'] + start_angle[e] and sweep[e] == 0 then
				current_angle[e] = current_angle[e] + (g_sentry[e]['scan_speed']/10)
				CollisionOff(e)
				SetRotation(e,0,current_angle[e],0)
				CollisionOn(e)
				LoopSound(e,0)
				if current_angle[e] >= g_sentry[e]['scan_radius'] + start_angle[e] then sweep[e] = 1 end
				if g_sentry[e]['muzzle_flash'] > 0 then
					Ent = g_Entity[e]
					local x, y, z, xa, ya, za = GetObjectPosAng( Ent.obj )
					x=x+math.sin(math.rad(ya))*g_sentry[e]['muzzle_z']
					y=y-math.sin(math.rad(xa))*g_sentry[e]['muzzle_z']
					z=z+math.cos(math.rad(ya))*g_sentry[e]['muzzle_z']			
					ResetPosition(g_sentry[e]['muzzle_flash'], x,y+g_sentry[e]['muzzle_y'],z)
					ResetRotation(g_sentry[e]['muzzle_flash'], xa,ya-180,za)
				end		
			end
			if sweep[e] == 1 then
				current_angle[e] = current_angle[e] - (g_sentry[e]['scan_speed']/10)
				CollisionOff(e)
				SetRotation(e,0,current_angle[e],0)
				CollisionOn(e)
				SetSoundVolume(120)
				LoopSound(e,0)
				if current_angle[e] <= start_angle[e] then sweep[e] = 0 end
				if g_sentry[e]['muzzle_flash'] > 0 then
					Ent = g_Entity[e]
					local x, y, z, xa, ya, za = GetObjectPosAng( Ent.obj )
					x=x+math.sin(math.rad(ya))*g_sentry[e]['muzzle_z']
					y=y-math.sin(math.rad(xa))*g_sentry[e]['muzzle_z']
					z=z+math.cos(math.rad(ya))*g_sentry[e]['muzzle_z']			
					ResetPosition(g_sentry[e]['muzzle_flash'], x,y+g_sentry[e]['muzzle_y'],z)
					ResetRotation(g_sentry[e]['muzzle_flash'], xa,ya-180,za)
				end			
			end			
		end
		--if player in range
		GetEntityPlayerVisibility(e)
		if GetPlayerDistance(e) <= g_sentry[e]['scan_range'] and g_Entity[e]['health'] > 1 then
			if current_angle[e] < g_sentry[e]['scan_radius'] + start_angle[e] and sweep[e] == 0 then
				current_angle[e] = current_angle[e] + (g_sentry[e]['scan_speed']/10)
				CollisionOff(e)
				SetRotation(e,0,current_angle[e],0)
				CollisionOn(e)
				LoopSound(e,0)
				if current_angle[e] >= g_sentry[e]['scan_radius'] + start_angle[e] then sweep[e] = 1 end
				if g_sentry[e]['muzzle_flash'] > 0 then
					Ent = g_Entity[e]
					local x, y, z, xa, ya, za = GetObjectPosAng( Ent.obj )
					x=x+math.sin(math.rad(ya))*g_sentry[e]['muzzle_z']
					y=y-math.sin(math.rad(xa))*g_sentry[e]['muzzle_z']
					z=z+math.cos(math.rad(ya))*g_sentry[e]['muzzle_z']			
					ResetPosition(g_sentry[e]['muzzle_flash'], x,y+g_sentry[e]['muzzle_y'],z)
					ResetRotation(g_sentry[e]['muzzle_flash'], xa,ya-180,za)
				end				
			end
			if sweep[e] == 1 then
				current_angle[e] = current_angle[e] - (g_sentry[e]['scan_speed']/10)
				CollisionOff(e)
				SetRotation(e,0,current_angle[e],0)
				CollisionOn(e)
				SetSoundVolume(120)
				LoopSound(e,0)
				if current_angle[e] <= start_angle[e] then sweep[e] = 0 end
				if g_sentry[e]['muzzle_flash'] > 0 then
					Ent = g_Entity[e]
					local x, y, z, xa, ya, za = GetObjectPosAng( Ent.obj )
					x=x+math.sin(math.rad(ya))*g_sentry[e]['muzzle_z']
					y=y-math.sin(math.rad(xa))*g_sentry[e]['muzzle_z']
					z=z+math.cos(math.rad(ya))*g_sentry[e]['muzzle_z']			
					ResetPosition(g_sentry[e]['muzzle_flash'], x,y+g_sentry[e]['muzzle_y'],z)
					ResetRotation(g_sentry[e]['muzzle_flash'], xa,ya-180,za)
				end		
			end
			if EntityLookingAtPlayer(e,g_sentry[e]['scan_range'],g_sentry[e]['scan_radius']/8) == 1 and g_Entity[e]['plrvisible'] == 1 then					
				x = g_PlayerPosX - g_Entity[e]['x']
				z = g_PlayerPosZ - g_Entity[e]['z']
				target_angle[e] = math.atan2(x,z)
				target_angle[e] = target_angle[e] * (180.0 / math.pi)
				if target_angle[e] < 0 then
					target_angle[e] = g_sentry[e]['scan_radius'] + target_angle[e]
				elseif target_angle[e] > g_sentry[e]['scan_radius'] then
					target_angle[e] = target_angle[e] - g_sentry[e]['scan_radius']
				end		
				if current_angle[e] < target_angle[e] then
					current_angle[e] = current_angle[e] + (g_sentry[e]['scan_speed']/10)
				elseif current_angle[e]> target_angle[e] then
					current_angle[e] = current_angle[e] - (g_sentry[e]['scan_speed']/10)
				end
				--keep current_angle within 0 to selected radius up to 360
				if current_angle[e] > g_sentry[e]['scan_radius'] + start_angle[e] then
					current_angle[e] = start_angle[e]
				end
				if current_angle[e] > 360 then current_angle[e] = 0 end
				if current_angle[e] < 0 then current_angle[e] = 0 end
				CollisionOff(e)
				SetRotation(e,0,current_angle[e],0)
				CollisionOn(e)
				if g_PlayerHealth > 1 then
					LoopSound(e,1)
				else
					StopSound(e,1)
				end
				if g_sentry[e]['alarm'] == 2 or g_sentry[e]['alarm'] == 1 and g_sentry[e]['allegiance'] == 0 then 
					RotateToPlayer(e)
					current_angle[e] = g_Entity[e]['angley']
					status[e] = 'alarm'	
					if g_sentry[e]['muzzle_flash'] > 0 then
						Ent = g_Entity[e]
						local x, y, z, xa, ya, za = GetObjectPosAng( Ent.obj )
						x=x+math.sin(math.rad(ya))*g_sentry[e]['muzzle_z']
						y=y-math.sin(math.rad(xa))*g_sentry[e]['muzzle_z']
						z=z+math.cos(math.rad(ya))*g_sentry[e]['muzzle_z']			
						ResetPosition(g_sentry[e]['muzzle_flash'], x,y+g_sentry[e]['muzzle_y'],z)
						ResetRotation(g_sentry[e]['muzzle_flash'], xa,ya-180,za)
						Show(g_sentry[e]['muzzle_flash'])
					end					
					--Alert Soldiers here
					MakeAISound(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,g_sentry[e]['alarm_range'],1,e) -- make a sound with a specified category of game architype (0-neutral, 1-enemy, 2-player)
					Prompt(g_sentry[e]['prompt_text'])								
					CollisionOn(e)				
				end
				if g_sentry[e]['sentry_damage'] > 0 and g_PlayerHealth > 1 and g_sentry[e]['allegiance'] == 0 then	
					RotateToPlayer(e)
					current_angle[e] = g_Entity[e]['angley']
					HurtPlayer(e,g_sentry[e]['sentry_damage'])				
				end			
			end
			if EntityLookingAtPlayer(e,g_sentry[e]['scan_range'],g_sentry[e]['scan_radius']/8) == 0 and status[e] == 'alarm' then			
				status[e] = 'scanning'
				target_angle[e] = -1
				StopSound(e,1)
				Hide(g_sentry[e]['muzzle_flash'])
			end
		else	
			StopSound(e,1)		
			status[e] = 'scanning'
			target_angle[e] = -1
			Hide(g_sentry[e]['muzzle_flash'])
		end		
		if g_Entity[e]['health'] <= 1 then
			status[e] = 'destroyed'
			SetEntityHealth(e,0)
			StopSound(e,0)
			StopSound(e,1)
			Hide(e)		
			Destroy(e)
			Hide(g_sentry[e]['muzzle_flash'])
			Destroy(g_sentry[e]['muzzle_flash'])			
		end
	end		
end

function sentry_exit(e)
end

function EntityLookingAtPlayer(e,dis,v)
	if g_Entity[e] ~= nil then
		if dis == nil then
			dis = 3000
		end
		if v == nil then
			v = 0.5
		end
		local destx = g_PlayerPosX - g_Entity[e]['x']
		local destz = g_PlayerPosZ - g_Entity[e]['z']
		if math.sqrt((destx*destx)+(destz*destz)) <= dis then
			local angle = math.atan2(destx,destz)
			angle = angle * (180.0 / math.pi)
			if angle <= 0 then
				angle = 360 + angle
			elseif angle > 360 then
				angle = angle - 360
			end
			while g_Entity[e]['angley'] < 0 or g_Entity[e]['angley'] > 360 do
				if g_Entity[e]['angley'] <= 0 then
					g_Entity[e]['angley'] = 360 + g_Entity[e]['angley']
				elseif g_Entity[e]['angley'] > 360 then
					g_Entity[e]['angley'] = g_Entity[e]['angley'] - 360
				end
			end
			local L = angle - v
			local R = angle + v
			if L <= 0 then
				L = 360 + L 
			elseif L > 360 then
				L = L - 360
			end
			if R <= 0 then
				R = 360 + R
			elseif R > 360 then
				R = R - 360
			end
			
			if (L < R and math.abs(g_Entity[e]['angley']) > L and math.abs(g_Entity[e]['angley']) < R) then
				return 1
			elseif (L > R and (math.abs(g_Entity[e]['angley']) > L or math.abs(g_Entity[e]['angley']) < R)) then
				return 1
			else
				return 0
			end
		else
			return 0
		end
	end
end

function GetKeyPressed(e, key, ignorecase)
    key = key or ""
    ignorecase = ignorecase or false 
    lastpressed = lastpressed or {}
    e = e or 0
    lastpressed[e] = lastpressed[e] or {}
    local inkey = g_InKey
    if ignorecase then
        key = string.lower(key)
        inkey = string.lower(g_InKey)
    end
    local waspressed
    if inkey == key and lastpressed[e] ~= g_InKey then
        waspressed = g_InKey 
    else
        waspressed = "false"
    end
    lastpressed[e] = g_InKey
    return waspressed
end

function GetFlatDistance(e,v)
	if g_Entity[e] ~= nil and g_Entity[v] ~= nil then
	  local distDX = g_Entity[e]['x'] - g_Entity[v]['x']
	  local distDZ = g_Entity[e]['z'] - g_Entity[v]['z']
	  return math.sqrt((distDX*distDX)+(distDZ*distDZ));
	end
end

function EntityLooking(e,t,dis,v)
	if g_Entity[e] ~= nil and g_Entity[t] ~= nil then
		if dis == nil then
			dis = 3000
		end
		if v == nil then
			v = 0.5
		end
		local destx = g_Entity[t]['x'] - g_Entity[e]['x']
		local destz = g_Entity[t]['z'] - g_Entity[e]['z']
		if math.sqrt((destx*destx)+(destz*destz)) <= dis then
			local angle = math.atan2(destx,destz)
			angle = angle * (180.0 / math.pi)	 
			if angle <= 0 then
				angle = 360 + angle
			elseif angle > 360 then
				angle = angle - 360
			end
		while g_Entity[e]['angley'] < 0 or g_Entity[e]['angley'] > 360 do
			if g_Entity[e]['angley'] <= 0 then
				g_Entity[e]['angley'] = 360 + g_Entity[e]['angley']
			elseif g_Entity[e]['angley'] > 360 then
				g_Entity[e]['angley'] = g_Entity[e]['angley'] - 360
			end
		end
		local L = angle - v
		local R = angle + v
		if L <= 0 then
			L = 360 + L 
		elseif L > 360 then
			L = L - 360
		end
		if R <= 0 then
			R = 360 + R
		elseif R > 360 then
			R = R - 360
		end	 
		if (L < R and math.abs(g_Entity[e]['angley']) > L and math.abs(g_Entity[e]['angley']) < R) then
			return 1
		elseif (L > R and (math.abs(g_Entity[e]['angley']) > L or math.abs(g_Entity[e]['angley']) < R)) then
			return 1
		else
			return 0
		end
	else
		return 0
	end
	end
end

function RotateToPoint(e,x,z)
	if g_Entity[e] ~= nil and x > 0 and z > 0 then
		local destx = x - g_Entity[e]['x']
		local destz = z - g_Entity[e]['z']
		local angle = math.atan2(destx,destz)
		angle = angle * (180.0 / math.pi)
		if angle < 0 then
			angle = 360 + angle
		elseif angle > 360 then
			angle = angle - 360
		end
		SetRotation(e,0,angle,0)
		return angle
	end
end
