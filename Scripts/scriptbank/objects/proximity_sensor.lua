-- Proximity Sensor v11 by Necrym59
-- DESCRIPTION: The attached object will be a proximity range sensor for detecting an NPC and/or Player to activate a logic linked or IfUsed entity and/or process a selected action
-- DESCRIPTION: Set Always Active = On
-- DESCRIPTION: [SENSOR_RANGE=180(1,2000)]
-- DESCRIPTION: [SENSED_TEXT$="Detected"]
-- DESCRIPTION: [!SENSE_PLAYER$=1]
-- DESCRIPTION: [!SENSE_NPC$=0]
-- DESCRIPTION: [@NPC_TYPE=1(1=Enemy, 2=Ally, 3=Neutral, 4=Object)]
-- DESCRIPTION: [@ACTION_TYPE=1(1=None, 2=Damage, 3=Self Destruct, 4=Object Repel)].
-- DESCRIPTION: [ACTION_AMOUNT=10(1,500)] Amount to apply
-- DESCRIPTION: [ACTIVATED!=1] if unchecked use a switch or zone to activate this object
-- DESCRIPTION: <Sound0> - Collision Sound

local U = require "scriptbank\\utillib"
local P = require "scriptbank\\physlib"
local proximity_sensor = {}
local sensor_range = {}
local sensed_text = {}
local sense_player = {}
local sense_npc = {}
local npc_type = {}
local action_type = {}
local action_amount = {}
local activated = {}

local sensecheck = {}
local entinrange = {}
local allegiance = {}
local doonce = {}
local played = {}
local status = {}

function proximity_sensor_properties(e,sensor_range, sensed_text, sense_player, sense_npc, npc_type, action_type, action_amount, activated)
	proximity_sensor[e].sensor_range = sensor_range
	proximity_sensor[e].sensed_text = sensed_text
	proximity_sensor[e].sense_player = sense_player
	proximity_sensor[e].sense_npc = sense_npc
	proximity_sensor[e].npc_type = npc_type
	proximity_sensor[e].action_type = action_type
	proximity_sensor[e].action_amount = action_amount
	proximity_sensor[e].activated = activated or 1
end

function proximity_sensor_init(e)
	proximity_sensor[e] = {}
	proximity_sensor[e].sensor_range = 190
	proximity_sensor[e].sensed_text = "Detected"
	proximity_sensor[e].sense_player = 1
	proximity_sensor[e].sense_npc = 0
	proximity_sensor[e].npc_type = 1
	proximity_sensor[e].action_type = 1
	proximity_sensor[e].action_amount = 0
	proximity_sensor[e].activated = 1
	doonce[e] = 0
	played[e] = 0
	entinrange[e] = 0
	allegiance[e] = 0
	status[e] = "init"
	sensecheck[e] = math.huge
end

function proximity_sensor_main(e)

	if status[e] == "init" then		
		sensecheck[e] = g_Time + 100
		if proximity_sensor[e].activated == 1 then SetActivated(e,1) end
		if proximity_sensor[e].activated == 0 then SetActivated(e,0) end
		status[e] = "sensing"
	end
	if g_Entity[e]['activated'] == 1 then
		if status[e] == "sensing" then
			if proximity_sensor[e].sense_player == 1 then
				if GetPlayerDistance(e) < proximity_sensor[e].sensor_range then
					Prompt(proximity_sensor[e].sensed_text)
					if doonce[e] == 0 then
						PerformLogicConnections(e)
						ActivateIfUsed(e)						
						doonce[e] = 1
					end
				else	
					doonce[e] = 0
				end
			end
						
			if g_Time > sensecheck[e] and proximity_sensor[e].sense_npc == 1 then				
				if proximity_sensor[e].sense_npc == 1 then
					for a = 1, g_EntityElementMax do
						if a ~= nil and g_Entity[a] ~= nil and g_Entity[a] ~= g_Entity[e] and math.ceil(GetFlatDistance(e,a)) <= proximity_sensor[e].sensor_range and g_Entity[a]['health'] > 1 then
							allegiance[e] = GetEntityAllegiance(a)
							entinrange[e] = a
							if allegiance[e] == 0 and proximity_sensor[e].npc_type == 1 then
								if doonce[e] == 0 then
									PerformLogicConnections(e)
									ActivateIfUsed(e)
									if proximity_sensor[e].action_type == 2 then
										SetEntityHealth(entinrange[e],g_Entity[entinrange[e]]['health']-proximity_sensor[e].action_amount)
									end
									if proximity_sensor[e].action_type == 3 then
										SetEntityHealth(e,0)
										Destroy(e)
									end
									doonce[e] = 1
								end								
							end
							if allegiance[e] == 1 and proximity_sensor[e].npc_type == 2 then
								if doonce[e] == 0 then
									PerformLogicConnections(e)
									ActivateIfUsed(e)
									if proximity_sensor[e].action_type == 2 then
										SetEntityHealth(entinrange[e],g_Entity[entinrange[e]]['health']-proximity_sensor[e].action_amount)
									end
									if proximity_sensor[e].action_type == 3 then
										SetEntityHealth(e,0)
										Destroy(e)
									end
									doonce[e] = 1
								end								
							end
							if allegiance[e] == 2 and proximity_sensor[e].npc_type == 3 then
								if doonce[e] == 0 then
									PerformLogicConnections(e)
									ActivateIfUsed(e)
									if proximity_sensor[e].action_type == 2 then
										SetEntityHealth(entinrange[e],g_Entity[entinrange[e]]['health']-proximity_sensor[e].action_amount)
									end
									if proximity_sensor[e].action_type == 3 then
										SetEntityHealth(e,0)
										Destroy(e)
									end
									doonce[e] = 1
								end								
							end	
							if allegiance[e] == -1 and proximity_sensor[e].npc_type == 4 then
								if doonce[e] == 0 then
									if played[e] == 0 then
										PlaySound(e,0)
										played[e] = 1
									end
									PerformLogicConnections(e)
									ActivateIfUsed(e)
									if proximity_sensor[e].action_type == 2 then
										SetEntityHealth(entinrange[e],g_Entity[entinrange[e]]['health']-proximity_sensor[e].action_amount)
									end
									if proximity_sensor[e].action_type == 3 then
										SetEntityHealth(e,0)
										Destroy(e)
									end
									if proximity_sensor[e].action_type == 4 then										
										if proximity_sensor[e].action_amount > 0 then
											ResetRotation(g_Entity[entinrange[e]],0,g_Entity[entinrange[e]]['angley']-180,0)
											local paX, paY, paZ = math.rad( g_Entity[entinrange[e]]['anglex'] ), math.rad( g_Entity[entinrange[e]]['angley'] ),math.rad( g_Entity[entinrange[e]]['anglez'] )
											local paYR = math.random(paY-5,paY+5)
											local vx, vy, vz = U.Rotate3D( 0, 0, 1, paX, paYR, paZ)
											PushObject(g_Entity[entinrange[e]]['obj'],vx*proximity_sensor[e].action_amount, vy*proximity_sensor[e].action_amount, vz*proximity_sensor[e].action_amount, math.random()/100, math.random()/100, math.random()/100 )											
										end
									end											
									doonce[e] = 1
								end									
							end
						end
					end
				end
				sensecheck[e] = g_Time + 100
				doonce[e] = 0
				played[e] = 0
				status[e] = "sensing"
			end
		end
	end	
end

function GetFlatDistance(e,v)
	if g_Entity[e] ~= nil and g_Entity[v] ~= nil then
		local distDX = g_Entity[e]['x'] - g_Entity[v]['x']
		local distDZ = g_Entity[e]['z'] - g_Entity[v]['z']
		return math.sqrt((distDX*distDX)+(distDZ*distDZ));
	end
end