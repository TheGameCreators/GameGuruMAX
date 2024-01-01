-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Elevator Script V9  by Necrym59
-- DESCRIPTION: Will create a multi level elevator.
-- DESCRIPTION: Attach to an object. Physics=ON, IsImmobile=On
-- DESCRIPTION: [USE_TEXT$="E-Up Q-Down"]
-- DESCRIPTION: [NUMBER_OF_LEVELS=2(1,50)]
-- DESCRIPTION: [HEIGHT_BETWEEN_LEVELS=300]
-- DESCRIPTION: [MOVEMENT_SPEED=5(1,100)]
-- DESCRIPTION: <Sound0> Elevator Music
-- DESCRIPTION: <Sound1> Elevator Starting
-- DESCRIPTION: <Sound2> Elevator Running
-- DESCRIPTION: <Sound3> Elevator Stopping

g_elevator_calledno = {}
g_elevator_calledlevel = {}

local elevator 				= {}
local use_text 				= {}
local number_of_levels		= {}
local height_between_levels	= {}
local movement_speed		= {}

local onelevator			= {}
local current_level			= {}
local current_height		= {}
local destination_heightUp	= {}
local destination_heightDn	= {}
local maximum_height		= {}
local minimum_height		= {}
local called_height			= {}
local levelselect			= {}
local switchcall			= {}
local status				= {}

function elevator_properties(e, use_text, number_of_levels, height_between_levels, movement_speed)
	elevator[e] = g_Entity[e]
	elevator[e].use_text = use_text
	elevator[e].number_of_levels = number_of_levels
	elevator[e].height_between_levels = height_between_levels
	elevator[e].movement_speed =  movement_speed
end

function elevator_init(e)
	elevator[e] = {}
	elevator[e].use_text ="E-Up Q-Down"
	elevator[e].number_of_levels = 2
	elevator[e].height_between_levels = 300
	elevator[e].movement_speed = 5

	onelevator[e] = 0
	current_level[e] = 0
	current_height[e] = g_Entity[e]['y']
	destination_heightUp[e] = 0
	destination_heightDn[e] = 0
	maximum_height[e] = 0
	minimum_height[e] = 0
	called_height[e] = 0
	levelselect[e] = 0
	g_elevator_calledno = nil
	g_elevator_calledlevel = nil
	switchcall[e] = 0	
	status[e] = "init"
end

function elevator_main(e)
	elevator[e] = g_Entity[e]

	if status[e] == "init" then
		minimum_height[e] = g_Entity[e]['y']
		maximum_height[e] = (elevator[e].height_between_levels * elevator[e].number_of_levels)+g_Entity[e]['y']
		current_level[e] = 0
		current_height[e] = minimum_height[e]
		status[e] = "check"
	end

	if status[e] == "check" then
		onelevator[e]=IntersectAll(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,g_PlayerPosX,-1,g_PlayerPosZ,0)
		if onelevator[e] ~= g_Entity[e]['obj'] then
			onelevator[e] = 0
		end
		if g_Entity[e]['obj'] == onelevator[e] then
			LoopSound(e,0)
			TextCenterOnX(50,92,3,"Current Level: " ..current_level[e])
			TextCenterOnX(50,95,3,"MMB Direct to Level: " ..levelselect[e])
			Prompt(elevator[e].use_text)
			if current_height[e] < maximum_height[e] then destination_heightUp[e] = current_height[e] + elevator[e].height_between_levels end
			if current_height[e] > minimum_height[e] then destination_heightDn[e] = current_height[e] - elevator[e].height_between_levels end
			if g_KeyPressE == 1 and current_level[e] < elevator[e].number_of_levels then
				PlaySound(e,1)
				switchcall[e] = 0
				levelselect[e] = current_level[e] + 1
				status[e] = "up"
			end
			if g_KeyPressQ == 1 and current_level[e] > 0 then
				PlaySound(e,1)
				switchcall[e] = 0
				levelselect[e] = current_level[e] - 1
				status[e] = "down"
			end
			--If called from level select-------------------------------------------------------------
			if g_MouseWheel < 0 then
				levelselect[e] = levelselect[e] - 1
				if levelselect[e] < 0 then levelselect[e] = 0 end
			elseif g_MouseWheel > 0 then
				levelselect[e] = levelselect[e] + 1
				if levelselect[e] > elevator[e].number_of_levels then levelselect[e] = elevator[e].number_of_levels end
			end
			if g_MouseClick == 4 and levelselect[e] ~= nil then
				called_height[e] = (elevator[e].height_between_levels * levelselect[e])+minimum_height[e]
				if current_height[e] < called_height[e] then
					destination_heightUp[e] = called_height[e]
					switchcall[e] = 0
					PlaySound(e,1)
					status[e] = "up"
				end
				if current_height[e] > called_height[e] then
					destination_heightDn[e] = called_height[e]
					switchcall[e] = 0
					PlaySound(e,1)
					status[e] = "down"
				end
				called_height[e] = minimum_height[e]
			end
		else
			status[e] = "check"
		end
		--If called from switch-------------------------------------------------------------
		if g_elevator_calledlevel ~= nil and g_elevator_calledno == e then
			called_height[e] = (elevator[e].height_between_levels * g_elevator_calledlevel)+minimum_height[e]
			if current_height[e] < called_height[e] then
				destination_heightUp[e] = called_height[e]
				switchcall[e] = 1
				PlaySound(e,1)
				status[e] = "up"
			end
			if current_height[e] > called_height[e] then
				destination_heightDn[e] = called_height[e]
				switchcall[e] = 1
				PlaySound(e,1)
				status[e] = "down"
			end
			called_height[e] = 0
			g_elevator_calledno = nil
			g_elevator_calledlevel = nil
		end
	end

	if status[e] == "up" then
		if current_height[e] < destination_heightUp[e] then
			LoopSound(e,2)
			GravityOff(e)
			CollisionOff(e)
			ResetPosition(e,g_Entity[e]['x'],current_height[e],g_Entity[e]['z'])
			if switchcall[e] == 0 then
				SetFreezePosition(g_PlayerPosX,(g_Entity[e]['y'] + 38), g_PlayerPosZ)
				TransportToFreezePositionOnly()
			end
			CollisionOn(e)
			current_height[e] = current_height[e] + elevator[e].movement_speed/10
		end
		if current_height[e] >= destination_heightUp[e] then
			current_height[e] = destination_heightUp[e]
			StopSound(e,2)
			PlaySound(e,3)
			CollisionOff(e)
			ResetPosition(e,g_Entity[e]['x'],destination_heightUp[e],g_Entity[e]['z'])
			if switchcall[e] == 0 then
				SetFreezePosition(g_PlayerPosX,(g_Entity[e]['y'] + 38), g_PlayerPosZ)
				TransportToFreezePositionOnly()
			end
			CollisionOn(e)
			current_level[e] = current_level[e] + 1
			if levelselect[e] > 0 then current_level[e] = levelselect[e] end
			if current_height[e] >= maximum_height[e] then current_level[e] = elevator[e].number_of_levels end
			status[e] = "check"
		end
		if g_elevator_calledlevel ~= nil then
			TextCenterOnX(50,95,3,"Elevator called to level: " ..g_elevator_calledlevel)
		else
			TextCenterOnX(50,95,3,"In transit to level: " ..levelselect[e])
		end
	end
	if status[e] == "down" then
		if current_height[e] > destination_heightDn[e] then
			LoopSound(e,2)
			CollisionOff(e)
			ResetPosition(e,g_Entity[e]['x'],current_height[e],g_Entity[e]['z'])
			if switchcall[e] == 0 then
				SetFreezePosition(g_PlayerPosX,(g_Entity[e]['y'] + 38), g_PlayerPosZ)
				TransportToFreezePositionOnly()
			end
			CollisionOn(e)
			current_height[e] = current_height[e] - elevator[e].movement_speed/10
		end
		if current_height[e] <= destination_heightDn[e] then
			current_height[e] = destination_heightDn[e]
			StopSound(e,2)
			PlaySound(e,3)
			CollisionOff(e)
			ResetPosition(e,g_Entity[e]['x'],current_height[e],g_Entity[e]['z'])
			if switchcall[e] == 0 then
				SetFreezePosition(g_PlayerPosX,(g_Entity[e]['y'] + 38), g_PlayerPosZ)
				TransportToFreezePositionOnly()
			end
			CollisionOn(e)
			current_level[e] = current_level[e] - 1
			if levelselect[e] > 0 then current_level[e] = levelselect[e] end
			if current_height[e] <= minimum_height[e] then current_level[e] = 0 end
			status[e] = "check"
		end
		if g_elevator_calledlevel ~= nil then
			TextCenterOnX(50,95,3,"Elevator called to level: " ..g_elevator_calledlevel)
		else
			TextCenterOnX(50,95,3,"In transit to level: " ..levelselect[e])
		end
	end	
end

