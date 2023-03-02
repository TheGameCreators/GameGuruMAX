-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Security Camera v10 by Necrym59
-- DESCRIPTION: The applied object will create a rotatable security device? Static Mode = No, Physics = On, Explodable = Yes, IsImobile = Yes
-- DESCRIPTION: Change the [PROMPT_TEXT$="Secuity alert was started"], the detection [SCAN_RANGE=500], [SCAN_RADIUS=136(1,360)], [SCAN_SPEED=3(1,10)]. Detection [@ALARM=2(1=Off, 2=On)] and [ALARM_RANGE=500(1,3000)] Change its [@VISIBILITY=1(1=Visible, 2=Invisible)] <Sound0> - Device Loop  <Sound1> - Alarm Loop
			
	local U = require "scriptbank\\utillib"
	local Q = require "scriptbank\\quatlib"
	local rad = math.rad	
	local deg = math.deg
	local atan = math.atan2
			
			
	g_seccam = {}
	local prompt_text = {}
	local scan_range = {}
	local scan_radius = {}	
	local scan_speed = {}
	local alarm = {}
	local alarm_range = {}
	local visibility = {}	
	local current_angle = {}
	local target_angle = {}
	local start_angle = {}
	local sweep = {}
	local status = {}
	local alert = {}
	local svol = {}
	

function seccam_properties(e, prompt_text, scan_range, scan_radius, scan_speed, alarm, alarm_range, visibility)
	g_seccam[e] = g_Entity[e]
	g_seccam[e]['prompt_text'] = prompt_text	
	g_seccam[e]['scan_range'] = scan_range	
	g_seccam[e]['scan_radius'] = scan_radius
	g_seccam[e]['scan_speed'] = scan_speed
	g_seccam[e]['alarm'] = alarm
	g_seccam[e]['alarm_range'] = alarm_range	
	g_seccam[e]['visibility'] = visibility
end -- End properties

function seccam_init(e)
	g_seccam[e] = g_Entity[e]
	g_seccam[e]['prompt_text'] = "Secuity alert was started"
	g_seccam[e]['scan_range'] = 500	
	g_seccam[e]['scan_radius'] = 136
	g_seccam[e]['scan_speed'] = 3
	g_seccam[e]['alarm'] = 2
	g_seccam[e]['alarm_range'] = 500
	g_seccam[e]['visibility'] = 1
	current_angle[e] = -1
	target_angle[e] = 0
	start_angle[e] = g_Entity[e]['angley']
	sweep[e] = 0
	status[e] = 'init'
	alert[e] = 0
	svol[e] = 0
end

function seccam_main(e)
	g_seccam[e] = g_Entity[e]
	if status[e] == 'init' then
		if g_seccam[e]['alarm'] == nil then g_seccam[e]['alarm'] = 1 end		
		if g_seccam[e]['visibility'] == nil then g_seccam[e]['visibility'] = 1 end
		if g_seccam[e]['visibility'] == 1 then Show(e) end
		if g_seccam[e]['visibility'] == 2 then Hide(e) end		
		current_angle[e] = start_angle[e]
		svol[e] = 0
		status[e] = 'scanning'		
	end
	
	svol[e] = (3000-GetPlayerDistance(e))/30
	
	--if player not in range then scan area		
	if GetPlayerDistance(e) > g_seccam[e]['scan_range'] and status[e] == 'scanning' then		
		if current_angle[e] < g_seccam[e]['scan_radius'] + start_angle[e] and sweep[e] == 0 then
			current_angle[e] = current_angle[e] + (g_seccam[e]['scan_speed']/10)
			CollisionOff(e)
			SetRotation(e,0,current_angle[e],0)
			CollisionOn(e)			
			LoopSound(e,0)
			SetSoundVolume(svol[e])			
			if current_angle[e] >= g_seccam[e]['scan_radius'] + start_angle[e] then sweep[e] = 1 end
		end
		if sweep[e] == 1 then
			current_angle[e] = current_angle[e] - (g_seccam[e]['scan_speed']/10)
			CollisionOff(e)
			SetRotation(e,0,current_angle[e],0)
			CollisionOn(e)			
			LoopSound(e,0)
			SetSoundVolume(svol[e])
			if current_angle[e] <= start_angle[e] then sweep[e] = 0 end
		end			
	end
	
	--check target enters range
	if GetPlayerDistance(e) <= g_seccam[e]['scan_range'] and g_Entity[e]['health'] > 1 then
		if current_angle[e] < g_seccam[e]['scan_radius'] + start_angle[e] and sweep[e] == 0 then
			current_angle[e] = current_angle[e] + (g_seccam[e]['scan_speed']/10)
			CollisionOff(e)
			SetRotation(e,0,current_angle[e],0)
			CollisionOn(e)			
			LoopSound(e,0)
			SetSoundVolume(svol[e])
			if current_angle[e] >= g_seccam[e]['scan_radius'] + start_angle[e] then sweep[e] = 1 end
		end
		if sweep[e] == 1 then
			current_angle[e] = current_angle[e] - (g_seccam[e]['scan_speed']/10)
			CollisionOff(e)
			SetRotation(e,0,current_angle[e],0)
			CollisionOn(e)			
			LoopSound(e,0)
			SetSoundVolume(svol[e])
			if current_angle[e] <= start_angle[e] then sweep[e] = 0 end
		end			
		if EntityLookingAtPlayer(e,g_seccam[e]['scan_range'],g_seccam[e]['scan_radius']/8) == 1 then		 
			x = g_PlayerPosX - g_Entity[e]['x']
			z = g_PlayerPosZ - g_Entity[e]['z']
			target_angle[e] = math.atan2(x,z)
			target_angle[e] = target_angle[e] * (180.0 / math.pi)
			if target_angle[e] < 0 then
				target_angle[e] = g_seccam[e]['scan_radius'] + target_angle[e]
			elseif target_angle[e] > g_seccam[e]['scan_radius'] then
				target_angle[e] = target_angle[e] - g_seccam[e]['scan_radius']
			end		
			if current_angle[e] < target_angle[e] then
				current_angle[e] = current_angle[e] + (g_seccam[e]['scan_speed']/10)
			elseif current_angle[e]> target_angle[e] then
				current_angle[e] = current_angle[e] - (g_seccam[e]['scan_speed']/10)
			end
			--keep current_angle within 0 to selected radius up to 360
			if current_angle[e] > g_seccam[e]['scan_radius'] + start_angle[e] then
				current_angle[e] = start_angle[e]
			end
			if current_angle[e] > 360 then current_angle[e] = 0 end
			if current_angle[e] < 0 then current_angle[e] = 0 end
			CollisionOff(e)
			SetRotation(e,0,current_angle[e],0)
			CollisionOn(e)		
			if g_seccam[e]['alarm'] == 2 then
				RotateToPlayer(e)
				current_angle[e] = g_Entity[e]['angley']
				status[e] = 'alarm'				
				--Alert Soldiers here
				SetActivatedWithMP(e,201)				
				ActivateIfUsed(e)
				PerformLogicConnections(e)
				MakeAISound(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,g_seccam[e]['alarm_range'],1,e) -- make a sound with a specified category of game architype (0-neutral, 1-enemy, 2-player)
				Prompt(g_seccam[e]['prompt_text'])
				LoopSound(e,1)
				SetSoundVolume(120)
				CollisionOn(e);								
				--Can modify here for other damage			
			end	
		end
		if EntityLookingAtPlayer(e,g_seccam[e]['scan_range'],g_seccam[e]['scan_radius']/8) == 0 and status[e] == 'alarm' then			
			status[e] = 'scanning'
			target_angle[e] = -1			
			StopSound(e,1)
		end	
	else
		StopSound(e,1)
		StopSound(e,0)
		status[e] = 'scanning'
		target_angle[e] = -1
	end
	if g_Entity[e]['health'] <= 1 then		
		status[e] = 'destroyed'
		StopSound(e,0)
		StopSound(e,1)
		Hide(e)
		Destroy(e)
	end	
end

function seccam_exit(e)
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