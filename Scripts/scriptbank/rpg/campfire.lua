-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Campfire v10 by Necrym59
-- DESCRIPTION: This object will stow and deploy like a camp fire?
-- DESCRIPTION: Apply to an object to be used as a campfire.
-- DESCRIPTION: [USE_RANGE=120]
-- DESCRIPTION: [PICKUP_TEXT$="Press PGUP to pack up the campfire"]
-- DESCRIPTION: [DEPLOY_TEXT$="Press PGDN to deploy the campfire"]
-- DESCRIPTION: [USE_TEXT$="Press E to start or Q to put-out campfire"]
-- DESCRIPTION: [RESOURCE_REQUIRED$=""] eg: "Wood"
-- DESCRIPTION: [HEAT_RANGE=200]
-- DESCRIPTION: [HEAT_DELAY=3] in seconds
-- DESCRIPTION: [PARTICLE_NAME$=""] eg: Particle1
-- DESCRIPTION: [LIGHT_NAME$=""] eg: Light1
-- DESCRIPTION: [GLOW_EFFECT!=1] emmisive glow effect
-- DESCRIPTION: [IS_DEPLOYED!=1] if campfire is deployed or carried at start
-- DESCRIPTION: [USER_GLOBAL_AFFECTED$=""] eg:(MyBodyTemp)
-- DESCRIPTION: [@AFFECT_TYPE=1(1=Add, 2=Deduct)]
-- DESCRIPTION: [AFFECT_AMOUNT=1(1,100)] per heat delay seconds
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline)] Use emmisive color for shape option
-- DESCRIPTION: <Sound0> for collection sound.
-- DESCRIPTION: <Sound1> for deloyment sound.
-- DESCRIPTION: <Sound2> for campfire sound.

local module_misclib = require "scriptbank\\module_misclib"
module_lightcontrol = require "scriptbank\\markers\\module_lightcontrol"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local lower = string.lower
local campfire				= {}
local use_range				= {}
local pickup_text			= {}
local deploy_text			= {}
local use_text				= {}
local resource_required		= {}
local heat_range			= {}
local heat_delay			= {}
local particle_name			= {}
local light_name			= {}
local glow_effect			= {}
local is_deployed			= {}
local user_global_affected	= {}
local affect_type			= {}
local affect_amount			= {}
local prompt_display		= {}
local item_highlight		= {}

local particle_number		= {}
local light_number			= {}
local status 				= {}
local currentvalue			= {}
local tEnt 					= {}
local selectobj 			= {}	
local campfire_deployed 	= {}
local campfire_state 		= {}
local campfire_onoff 		= {}
local height_difference		= {}
local hurttime				= {}
local keypressed			= {}
local tobeused				= {}
	 
function campfire_properties(e, use_range, pickup_text, deploy_text, use_text, resource_required, heat_range, heat_delay, particle_name, light_name, glow_effect, is_deployed, user_global_affected, affect_type, affect_amount, prompt_display, item_highlight)
	campfire[e].use_range = use_range
	campfire[e].pickup_text = pickup_text
	campfire[e].deploy_text = deploy_text
	campfire[e].use_text = use_text
	campfire[e].resource_required = string.lower(resource_required)
	campfire[e].heat_range = heat_range	
	campfire[e].heat_delay = heat_delay
	campfire[e].particle_name = string.lower(particle_name)
	campfire[e].light_name = string.lower(light_name)
	campfire[e].glow_effect = glow_effect
	campfire[e].is_deployed = is_deployed	
	campfire[e].user_global_affected = user_global_affected
	campfire[e].affect_type = affect_type
	campfire[e].affect_amount = affect_amount
	campfire[e].prompt_display = prompt_display
	campfire[e].item_highlight = item_highlight
end
 
 
function campfire_init(e)
	campfire[e] = {}
	campfire[e].use_range = 120
	campfire[e].pickup_text = "Press PGUP to pack up the campfire"
	campfire[e].deploy_text = "Press PGDN to deplay the campfire"
	campfire[e].use_text = "Press E to start or Q to put-out campfire"
	campfire[e].resource_required = ""	
	campfire[e].heat_range = 200
	campfire[e].heat_delay = 3
	campfire[e].particle_name = ""
	campfire[e].light_name = ""
	campfire[e].glow_effect = 1	
	campfire[e].is_deployed = 1
	campfire[e].user_global_affected = ""
	campfire[e].affect_type = 1
	campfire[e].affect_amount = 0
	campfire[e].prompt_display = 1
	campfire[e].item_highlight = 1
	
	campfire[e].particle_number = 0
	campfire[e].light_number = 0

	status[e] = "init"
	tEnt[e] = 0
	g_tEnt = 0
	campfire_deployed[e] = 0
	campfire_state[e] = "out"
	campfire_onoff[e] = "off"
	height_difference[e] = 0
	keypressed[e] = 0
	tobeused[e] = 0
	math.randomseed(os.time())	
end
 
function campfire_main(e)

	if status[e] == "init" then
		new_y = math.rad(g_PlayerAngY)
		fire_x = g_PlayerPosX + (math.sin(new_y) * 70)
		fire_z = g_PlayerPosZ + (math.cos(new_y) * 70)
		height_difference[e] = g_PlayerPosY-g_Entity[e].y
		if campfire[e].is_deployed == 0 then campfire_deployed[e] = 0 end
		if campfire[e].is_deployed == 1 then campfire_deployed[e] = 1 end
		if campfire[e].is_deployed == 1 then campfire_state[e] = "deployed" end
		hurttime[e] = g_Time + 500
		status[e] = "start"
	end
	
	--Find Named Particle --
	if campfire[e].particle_name > "" and campfire[e].particle_number == 0 then
		for p = 1, g_EntityElementMax do
			if p ~= nil and g_Entity[p] ~= nil then
				if string.lower(GetEntityName(p)) == campfire[e].particle_name then					
					campfire[e].particle_number = p
					Hide(p)
					break
				end
			end
		end
	end
	--Find Named Light --
	if campfire[e].light_name > "" and campfire[e].light_number == 0 then
		for b = 1, g_EntityElementMax do
			if b ~= nil and g_Entity[b] ~= nil then
				if string.lower(GetEntityName(b)) == campfire[e].light_name then
					campfire[e].light_number = GetEntityLightNumber(b)
					SetActivated(b,0)
					break
				end
			end
		end		
	end
	--Find Resource Item --
	if campfire[e].resource_required > "" and tobeused[e] == 0 then			
		for ee = 1, g_EntityElementMax do
			if ee ~= nil and g_Entity[ee] ~= nil then
				if string.lower(GetEntityName(ee)) == campfire[e].resource_required and GetEntityCollected(ee) == 1 and GetEntityUsed(ee) ~= -1 then
					tobeused[e] = ee						
					break
				end
			end			
		end
	end	
	
	local PlayerDist = GetPlayerDistance(e)
	
	if campfire_deployed[e] == 1 then
		if PlayerDist < campfire[e].use_range then
			--pinpoint select object--
			module_misclib.pinpoint(e,campfire[e].use_range,campfire[e].item_highlight)
			tEnt[e] = g_tEnt
			--end pinpoint select object--	
			
			if PlayerDist < campfire[e].use_range and tEnt[e] ~= 0 and GetEntityVisibility(e) == 1 then
				if campfire_state[e] == "deployed" then					
					SetLightRange(campfire[e].light_number,0)
					SetEntityEmissiveStrength(e,0)						
				end	
				if campfire_state[e] == "lit" then
					if campfire[e].prompt_display == 1 then PromptLocal(e,campfire[e].use_text) end
					if campfire[e].prompt_display == 2 then	Prompt(campfire[e].use_text)	end
				end
				if campfire_onoff[e] == "off" then
					if campfire[e].prompt_display == 1 and tobeused[e] == 0 then PromptLocal(e,campfire[e].pickup_text.. " or " ..campfire[e].resource_required.. " is required to use") end
					if campfire[e].prompt_display == 2 and tobeused[e] == 0 then PromptDuration(campfire[e].pickup_text.. " or " ..campfire[e].resource_required.. " is required to use",2000) end					
					if campfire[e].prompt_display == 1 and tobeused[e] > 0 then PromptLocal(e,campfire[e].use_text) end
					if campfire[e].prompt_display == 2 and tobeused[e] > 0 then Prompt(campfire[e].use_text) end					
				end
				height_difference[e] = g_PlayerPosY - g_Entity[e].y
				if g_Scancode == 201 and campfire_onoff[e] == "on" then --AttemptedPickup
					if campfire[e].prompt_display == 1 then PromptLocal(e,"Put-out campfire before pickup") end
					if campfire[e].prompt_display == 2 then Prompt("Put-out campfire before pickup") end
				end	
				
				if g_Scancode == 201 and campfire_onoff[e] == "off" then --Pickup
					StopSound(e,2)
					PlaySound(e,0)
					campfire_state[e] = "out"
					campfire_deployed[e] = 0
					PromptLocal(e,"")
					if campfire[e].prompt_display == 1 then PromptDuration(campfire[e].deploy_text,2000) end
					if campfire[e].prompt_display == 2 then PromptDuration(campfire[e].deploy_text,2000) end
					Hide(campfire[e].particle_number)
					SetLightRange(campfire[e].light_number,0)
					keypressed[e] = 0
				end
			end			
		end
	end
	
	if campfire_deployed[e] == 0 then
		new_y = math.rad(g_PlayerAngY)
		fire_x = g_PlayerPosX + (math.sin(new_y) * 70)
		fire_z = g_PlayerPosZ + (math.cos(new_y) * 70)		
		Hide(e)
		CollisionOff(e)
		SetEntityEmissiveStrength(e,0)
		ResetPosition(e, fire_x, g_PlayerPosY-height_difference[e], fire_z)
		Hide(campfire[e].particle_number)		
		SetLightRange(campfire[e].light_number,0)
		if g_Scancode == 209 then --Deploy
			PlaySound(e,1)
			Show(e)
			CollisionOn(e)
			campfire_deployed[e] = 1
			StartTimer(e)
			campfire_state[e] = "deployed"
		end
	end
	
	if campfire_state[e] == "deployed" then
		new_y = math.rad(g_PlayerAngY)
		fire_x = g_PlayerPosX + (math.sin(new_y) * 70)
		fire_z = g_PlayerPosZ + (math.cos(new_y) * 70)
		Show(e)
		CollisionOn(e)
		SetEntityEmissiveStrength(e,0)
		Hide(campfire[e].particle_number)		
		ResetPosition(campfire[e].particle_number, g_Entity[e]['x'], g_Entity[e]['y'], g_Entity[e]['z'])		
		SetLightPosition(campfire[e].light_number, g_Entity[e]['x'], g_Entity[e]['y']+15, g_Entity[e]['z'])
		SetLightRange(campfire[e].light_number,0)
		if PlayerDist < campfire[e].use_range and tEnt[e] ~= 0 and GetEntityVisibility(e) == 1 then			
			if g_KeyPressE == 1 and keypressed[e] == 0 then
				if tobeused[e] == 0 then
					keypressed[e] = 0
				end
				if tobeused[e] > 0 then
					keypressed[e] = 1
					SetEntityUsed(tobeused[e],1*-1)
					campfire_onoff[e] = "on"					
				end
			end
		end	
	end	
	if keypressed[e] == 1 then
		campfire_state[e] = "lit" 
		Show(campfire[e].particle_number)
		local lvariance = math.random(50,100)
		local lrange = GetLightRange(campfire[e].light_number)
		if lrange < lvariance then lrange = lrange + 1 end
		if lrange > lvariance then lrange = lrange - 1 end
		SetLightRange(campfire[e].light_number,lrange)
		LoopSound(e,2)
		if GetPlayerDistance(e) < campfire[e].heat_range then
			if GetTimer(e) > campfire[e].heat_delay*1000 then
				StartTimer(e)
				if campfire[e].affect_type == 1 then
					if campfire[e].user_global_affected > "" then 
						if _G["g_UserGlobal['"..campfire[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..campfire[e].user_global_affected.."']"] end
						_G["g_UserGlobal['"..campfire[e].user_global_affected.."']"] = currentvalue[e] + campfire[e].affect_amount
						if _G["g_UserGlobal['"..campfire[e].user_global_affected.."']"] >= 100 then _G["g_UserGlobal['"..campfire[e].user_global_affected.."']"] = 100 end
					end										
				end
				if campfire[e].affect_type == 2 then
					if campfire[e].user_global_affected > "" then 
						if _G["g_UserGlobal['"..campfire[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..campfire[e].user_global_affected.."']"] end
						_G["g_UserGlobal['"..campfire[e].user_global_affected.."']"] = currentvalue[e] - campfire[e].affect_amount
						if _G["g_UserGlobal['"..campfire[e].user_global_affected.."']"] <= 0 then _G["g_UserGlobal['"..campfire[e].user_global_affected.."']"] = 0 end	
					end					
				end				
			end
			if GetTimer(e) > campfire[e].heat_delay*100 then
				if campfire[e].glow_effect == 1 then
					SetEntityEmissiveStrength(e,math.random(1000,3000))
				end				
			end
			if g_Time > hurttime[e] then
				if U.PlayerCloserThanPos(g_Entity[e]['x'], g_PlayerPosY, g_Entity[e]['z'],20) then HurtPlayer(-1,5) end
				hurttime[e] = g_Time + 500
			end
		end
		if g_KeyPressQ == 1 and keypressed[e] == 1 then
			keypressed[e] = 0
			SetEntityCollected(tobeused[e],0)
			Destroy(tobeused[e])
			tobeused[e] = 0
			campfire_onoff[e] = "off"
			campfire_state[e] = "deployed"
		end		
	end
end