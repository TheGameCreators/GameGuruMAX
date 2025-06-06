-- Jumpgate v5 - by Necrym59
-- DESCRIPTION: Allows for a jumpgate teleport to a local named or connected or IfUsed entity or to another level.
-- DESCRIPTION: [JUMPGATE_RANGE=100(0,500)]
-- DESCRIPTION: [@JUMPGATE_TYPE=1(1=Instant, 2=Delayed, 3=Delayed+Countdown, 4=In-Vehicle)]
-- DESCRIPTION: [@JUMPGATE_MODE=1(1=Single-use, 2=Re-useable)]
-- DESCRIPTION: [JUMPGATE_DELAY=5(1,5))]
-- DESCRIPTION: [@JUMPGATE_EFFECT=1(1=None, 2=Video)]
-- DESCRIPTION: [PLAYER_LEVEL=0(0,100))] player level to be able use this jumpgate teleport
-- DESCRIPTION: [@DESTINATION=1(1=Local, 2=Level, 3=Win Game)]
-- DESCRIPTION: [LOCAL_DESTINATION_NAME$=""]
-- DESCRIPTION: [@GoToLevelMode=1(1=Use Storyboard Logic,2=Go to Specific Level)] controls whether the next level in the Storyboard, or another level is loaded after entry to the zone.
-- DESCRIPTION: <Video Slot> for effect video
-- DESCRIPTION: <Sound2> for jumpgate teleport effect

local U = require "scriptbank\\utillib" 
local lower = string.lower
g_JumpGate			= {}
g_JumpGateTargetX 	= {}
g_JumpGateTargetY 	= {}
g_JumpGateTargetZ 	= {}

local jumpgate = {}	
local jumpgate_range = {}
local jumpgate_type = {}
local jumpgate_mode = {}
local jumpgate_delay = {}	
local jumpgate_effect = {}
local player_level = {}
local destination = {}
local local_destination_name = {}

local jumpgate_target = {}
local timer = {}
local jumpedl = {}
local jumpedt = {}

local tlevelrequired = {}
local tplayerlevel = {}
local played = {}
local doonce = {}
local status = {}
local targetno = {}
local targetX = {}
local targetY = {}
local targetZ = {}
	
function jumpgate_properties(e, jumpgate_range, jumpgate_type, jumpgate_mode, jumpgate_delay, jumpgate_effect, player_level, destination, local_destination_name)
	jumpgate[e].jumpgate_range = jumpgate_range
	jumpgate[e].jumpgate_type = jumpgate_type
	jumpgate[e].jumpgate_mode = jumpgate_mode
	jumpgate[e].jumpgate_delay = jumpgate_delay
	jumpgate[e].jumpgate_effect	= jumpgate_effect
	jumpgate[e].player_level= player_level	
	jumpgate[e].destination	= destination
	jumpgate[e].local_destination_name = lower(local_destination_name)
	jumpgate[e].jumpgate_target = GetEntityString(e,0)
end

function jumpgate_init(e)
	jumpgate[e] = {}
	jumpgate[e].jumpgate_range = 100
	jumpgate[e].jumpgate_type = 1
	jumpgate[e].jumpgate_delay = 0
	jumpgate[e].jumpgate_mode = 2
	jumpgate[e].jumpgate_effect = 1
	jumpgate[e].player_level = 0
	jumpgate[e].destination	= 1
	jumpgate[e].local_destination_name = ""	
	jumpgate[e].in_vehicle = 0	
	jumpgate[e].jumpgate_target = GetEntityString(e,0)	

	jumpgate[e].timer = 0	
	tplayerlevel[e] = 0
	tlevelrequired[e] = 0
	jumpedl[e] = 0
	jumpedt[e] = 0
	played[e] = 0
	doonce[e] = 0
	
	targetno[e] = 0
	targetX[e] = 0
	targetY[e] = 0
	targetZ[e] = 0
	g_JumpGateTargetX = 0
	g_JumpGateTargetY = 0
	g_JumpGateTargetZ = 0
	status[e] = "init"
end

function jumpgate_main(e)

	if status[e] == "init" then
		tlevelrequired[e] = jumpgate[e].player_level
		if jumpgate[e].local_destination_name ~= "" and targetno[e] == 0 then			
			for n = 1, g_EntityElementMax do
				if n ~= nil and g_Entity[n] ~= nil then
					if lower(GetEntityName(n)) == jumpgate[e].local_destination_name then
						SetEntityIfUsed(e,jumpgate[e].local_destination_name)
						targetno[e] = n
						targetX[e] = g_Entity[n]['x']
						targetY[e] = g_Entity[n]['y']
						targetZ[e] = g_Entity[n]['z']
						break
					end
				end
			end			
		end
		status[e] = "endinit"		
	end
	local PlayerDist = GetPlayerDistance(e)	

	if jumpgate[e].jumpgate_type == 1 then   -- Instant --
		if jumpgate[e].destination == 1 then			
			if PlayerDist < jumpgate[e].jumpgate_range then				
				if _G["g_UserGlobal['".."MyPlayerLevel".."']"] ~= nil then tplayerlevel[e] = _G["g_UserGlobal['".."MyPlayerLevel".."']"] end
				if tplayerlevel[e] < tlevelrequired[e] then Prompt("You need to be level "..tlevelrequired[e].." to use this jumpgate") end
				if tplayerlevel[e] >= tlevelrequired[e] then				
					if jumpgate[e].jumpgate_target ~= nil then
						if jumpgate[e].jumpgate_effect == 1 then
							PlaySound(e,2)							
							TransportToIfUsed(e)
							PerformLogicConnections(e)
							if jumpgate[e].jumpgate_mode == 1 then Destroy(e) end	
							jumpgate[e].timer = 0
						end
						if jumpgate[e].jumpgate_effect == 2 then
							if doonce[e] == 0 then								
								PlayVideo(e,1)
								doonce[e] = 1
							end								
							if doonce[e] == 1 then
								StopVideo(e,1)
								PlaySound(e,2)
								TransportToIfUsed(e)							
								PerformLogicConnections(e)
								if jumpgate[e].jumpgate_mode == 1 then Destroy(e) end
								jumpgate[e].timer = 0
								doonce[e] = 0								
							end
						end	
					end
				end
			end
		end
		if jumpgate[e].destination == 2 then   -- Level --
			if PlayerDist < jumpgate[e].jumpgate_range then				
				if _G["g_UserGlobal['".."MyPlayerLevel".."']"] ~= nil then tplayerlevel[e] = _G["g_UserGlobal['".."MyPlayerLevel".."']"] end
				if tplayerlevel[e] < tlevelrequired[e] then Prompt("You need to be level "..tlevelrequired[e].." to use this jumpgate") end
				if tplayerlevel[e] >= tlevelrequired[e] then
					if jumpgate[e].jumpgate_effect == 1 then
						PlaySound(e,2)
						PerformLogicConnections(e)
						JumpToLevelIfUsed(e)
					end
					if jumpgate[e].jumpgate_effect == 2 then
						if doonce[e] == 0 then								
							PlayVideo(e,1)
							doonce[e] = 1
						end								
						if doonce[e] == 1 then
							StopVideo(e,1)
							PlaySound(e,2)													
							PerformLogicConnections(e)
							JumpToLevelIfUsed(e)
						end
					end				
				end
			end
		end
		if jumpgate[e].destination == 3 then   -- Win Game --
			if PlayerDist < jumpgate[e].jumpgate_range then				
				if _G["g_UserGlobal['".."MyPlayerLevel".."']"] ~= nil then tplayerlevel[e] = _G["g_UserGlobal['".."MyPlayerLevel".."']"] end
				if tplayerlevel[e] < tlevelrequired[e] then Prompt("You need to be level "..tlevelrequired[e].." to use this jumpgate") end
				if tplayerlevel[e] >= tlevelrequired[e] then
					if jumpgate[e].jumpgate_effect == 1 then
						PlaySound(e,2)
						WinGame()
					end
					if jumpgate[e].jumpgate_effect == 2 then
						if doonce[e] == 0 then								
							PlayVideo(e,1)
							doonce[e] = 1
						end								
						if doonce[e] == 1 then
							StopVideo(e,1)
							PlaySound(e,2)													
							WinGame()
						end
					end				
				end
			end
		end
	end

	if jumpgate[e].jumpgate_type == 2 or 3 then   -- Delayed or Timed --
		if jumpgate[e].destination == 1 then						
			if PlayerDist < jumpgate[e].jumpgate_range then				
				if _G["g_UserGlobal['".."MyPlayerLevel".."']"] ~= nil then tplayerlevel[e] = _G["g_UserGlobal['".."MyPlayerLevel".."']"] end
				if tplayerlevel[e] < tlevelrequired[e] then Prompt("You need to be level "..tlevelrequired[e].." to use this jumpgate") end
				if tplayerlevel[e] >= tlevelrequired[e] then
					if jumpgate[e].jumpgate_target ~= nil then
						g_JumpGate = 1
						g_JumpGateTargetX = targetX[e]
						g_JumpGateTargetY = targetY[e]
						g_JumpGateTargetZ = targetZ[e]
					else
						g_JumpGate = 0
						g_JumpGateTargetX = 0
						g_JumpGateTargetY = 0
						g_JumpGateTargetZ = 0			
					end
				end
			end
		end

		if jumpgate[e].destination == 2 then
			if PlayerDist < jumpgate[e].jumpgate_range then				
				if _G["g_UserGlobal['".."MyPlayerLevel".."']"] ~= nil then tplayerlevel[e] = _G["g_UserGlobal['".."MyPlayerLevel".."']"] end
				if tplayerlevel[e] < tlevelrequired[e] then Prompt("You need to be level "..tlevelrequired[e].." to use this jumpgate") end
				if tplayerlevel[e] >= tlevelrequired[e] then
					jumpgate[e].timer = jumpgate[e].timer + GetElapsedTime()
					if jumpgate[e].jumpgate_type == 3 then
						Prompt("Level Jumpgate in "..math.floor(jumpgate[e].jumpgate_delay - jumpgate[e].timer).." seconds")
					end
					if jumpgate[e].timer >= jumpgate[e].jumpgate_delay then
						if played[e] == 0 then					
							PlaySound(e,2)
							played[e] = 1						
						end	
						PerformLogicConnections(e)						
						JumpToLevelIfUsed(e)
					end
				end
			end
		end
		
		if jumpgate[e].destination == 3 then
			if PlayerDist < jumpgate[e].jumpgate_range then				
				if _G["g_UserGlobal['".."MyPlayerLevel".."']"] ~= nil then tplayerlevel[e] = _G["g_UserGlobal['".."MyPlayerLevel".."']"] end
				if tplayerlevel[e] < tlevelrequired[e] then Prompt("You need to be level "..tlevelrequired[e].." to use this jumpgate") end
				if tplayerlevel[e] >= tlevelrequired[e] then
					jumpgate[e].timer = jumpgate[e].timer + GetElapsedTime()
					if jumpgate[e].jumpgate_target ~= nil then
						jumpgate[e].timer = jumpgate[e].timer + GetElapsedTime()
						if jumpgate[e].jumpgate_type == 3 then Prompt("Jumpgate activating in "..math.floor(jumpgate[e].jumpgate_delay - jumpgate[e].timer).." seconds")	end
						if jumpgate[e].timer >= jumpgate[e].jumpgate_delay then							
							if jumpgate[e].jumpgate_effect == 1 and jumpgate[e].timer >= jumpgate[e].jumpgate_delay then
								Prompt("")
								PlaySound(e,2)
								WinGame()
								if jumpgate[e].jumpgate_mode == 1 then Destroy(e) end
								jumpgate[e].timer = 0								
							end
							if jumpgate[e].jumpgate_effect == 2 and jumpgate[e].timer >= jumpgate[e].jumpgate_delay then
								if doonce[e] == 0 then								
									PlayVideo(e,1)
									doonce[e] = 1
								end								
								if doonce[e] == 1 then
									StopVideo(e,1)
									PlaySound(e,2)
									WinGame()
									if jumpgate[e].jumpgate_mode == 1 then Destroy(e) end
									jumpgate[e].timer = 0
								end	
							end
						end	
					end
				end
			end
		end
	end
	
	if jumpgate[e].jumpgate_type == 4 then  -- In-Vehicle --
		if jumpgate[e].destination == 1 then -- Local -- Handled in Space Vehicle--
			if PlayerDist < jumpgate[e].jumpgate_range then				
				if _G["g_UserGlobal['".."MyPlayerLevel".."']"] ~= nil then tplayerlevel[e] = _G["g_UserGlobal['".."MyPlayerLevel".."']"] end
				if tplayerlevel[e] < tlevelrequired[e] then Prompt("You need to be level "..tlevelrequired[e].." to use this jumpgate") end
				if tplayerlevel[e] >= tlevelrequired[e] then
					if jumpgate[e].jumpgate_target ~= nil then
						g_JumpGate = 1
						g_JumpGateTargetX = targetX[e]
						g_JumpGateTargetY = targetY[e]
						g_JumpGateTargetZ = targetZ[e]
					else
						g_JumpGate = 0
						g_JumpGateTargetX = 0
						g_JumpGateTargetY = 0
						g_JumpGateTargetZ = 0
					end
				end
			end
		end		
	end
end