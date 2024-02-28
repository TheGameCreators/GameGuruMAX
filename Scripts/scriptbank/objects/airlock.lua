-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Airlock Switch v9 by Necrym59
-- DESCRIPTION: This object will be treated as a switch object for activating a set of airlock doors. Doors need to be locked.
-- DESCRIPTION: [@AIRLOCK_TYPE=1(1=Air, 2=Water)]
-- DESCRIPTION: [AIR_LEVEL=100(0,100)]
-- DESCRIPTION: [WATER_LEVEL=300(0,1000)]
-- DESCRIPTION: [PROMPT_TEXT$="[E] to begin"]
-- DESCRIPTION: [USE_RANGE=80(1,100)]
-- DESCRIPTION: [DEPRESSURISE_TEXT$="Depressurisation in"]
-- DESCRIPTION: [PRESSURISE_TEXT$="Pressurisation in"]
-- DESCRIPTION: [DOOR1_NAME$="Inner Door Name"]
-- DESCRIPTION: [DOOR2_NAME$="Outer Door Name"]
-- DESCRIPTION: [#DELAY=8(1,100)]
-- DESCRIPTION: [NPC_DROWNING!=0] If on will drown all NPC's under water level.
-- DESCRIPTION: <Sound0> when activating
-- DESCRIPTION: <Sound1> when processing

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local lower = string.lower
local g_waterControlMasterE = 0
local airlock			= {}
local airlock_type		= {}
local air_level			= {}
local water_level		= {}
local prompt_text		= {}
local use_range			= {}
local depressurise_text	= {}
local pressurise_text	= {}
local door1_name 		= {}
local door2_name 		= {}
local delay 			= {}
local npc_drowning 		= {}

local door1_number 		= {}
local door2_number 		= {}
local state 			= {}

local status = {}
local mode = {}
local tEnt = {}
local selectobj = {}
local doonce = {}
local wait = {}
local delaysecs = {}
local doordelay = {}
local countdown = {}
local current_wlevel = {}
local base_wlevel = {}
local atmosphere = {}
local airlock_status = {}
local depressurised = {}
local innerdoor = {}
local outerdoor = {}

function airlock_properties(e, airlock_type, air_level, water_level, prompt_text, use_range, depressurise_text, pressurise_text, door1_name, door2_name, delay, npc_drowning)
	airlock[e].airlock_type = airlock_type
	airlock[e].air_level = air_level
	airlock[e].water_level = water_level
	airlock[e].prompt_text = prompt_text
	airlock[e].use_range = use_range
	airlock[e].depressurise_text = depressurise_text
	airlock[e].pressurise_text = pressurise_text
	airlock[e].door1_name = lower(door1_name)
	airlock[e].door2_name = lower(door2_name)
	airlock[e].delay = delay
	airlock[e].npc_drowning = npc_drowning or 0
end

function airlock_init(e)
	airlock[e] = {}
	airlock[e].airlock_type = airlock_type
	airlock[e].air_level = 100
	airlock[e].water_level = water_level
	airlock[e].prompt_text = ""
	airlock[e].use_range = 80
	airlock[e].depressurise_text = ""
	airlock[e].pressurise_text = ""
	airlock[e].door1_name = ""
	airlock[e].door2_name = ""
	airlock[e].delay = 8
	airlock[e].npc_drowning = 0	
	airlock[e].door1_number = 0
	airlock[e].door2_number = 0
	airlock[e].state = 1
	
	status[e] = "init"
	wait[e] = math.huge
	mode[e] = ""
	g_tEnt = 0
	tEnt[e] = 0
	selectobj[e] = 0
	doonce[e] = 0
	countdown[e] = 0
	depressurised[e] = 0
	airlock_status[e] = "Pressurised"
	innerdoor[e] = "open"
	outerdoor[e] = "closed"
	delaysecs[e] = (airlock[e].delay * 1000)
	doordelay[e] = 1000
	current_wlevel[e] = GetWaterHeight()
	base_wlevel[e] = GetWaterHeight()
	atmosphere[e] = 0
end

function airlock_main(e)

	if status[e] == "init" then		
		airlock[e].w_level = w_level
		if airlock[e].door1_number == 0 or airlock[e].door1_number == nil then
			for a = 1, g_EntityElementMax do
				if a ~= nil and g_Entity[a] ~= nil then
					if lower(GetEntityName(a)) == airlock[e].door1_name then
						airlock[e].door1_number = a
						break
					end
				end
			end
		end
		if airlock[e].door2_number == 0 or airlock[e].door2_number == nil then
			for n = 1, g_EntityElementMax do
				if n ~= nil and g_Entity[n] ~= nil then
					if lower(GetEntityName(n)) == airlock[e].door2_name then
						airlock[e].door2_number = n
						break
					end
				end
			end
		end		
		local alreadyhaveair = GetGamePlayerControlDrownTimestamp()-Timer()
		status[e] = "endinit"
	end

	if airlock[e].state == 1 then
		doonce[e] = 0
		local PlayerDist = GetPlayerDistance(e)
		
		if PlayerDist < airlock[e].use_range and g_PlayerHealth > 0 then
			--pinpoint select object--
			module_misclib.pinpoint(e,airlock[e].use_range,300)
			tEnt[e] = g_tEnt
			--end pinpoint select object--	
		end	

		if PlayerDist < airlock[e].use_range and tEnt[e] ~= 0 then
			if airlock_status[e] == "Pressurised" then 
				PromptLocal(e,airlock[e].prompt_text.. " depressurisation")
				if g_KeyPressE == 1 then
					SetActivated(e,1)
					PlaySound(e,0)
					SetAnimationName(e,"on")
					PlayAnimation(e)
					mode[e] = "Depressurise"
					airlock[e].state = 2
					wait[e] = g_Time + delaysecs[e]
				end
			end	
			if airlock_status[e] == "Depressurised" then
				PromptLocal(e,airlock[e].prompt_text.. " pressurisation")
					if g_KeyPressE == 1 then
					SetActivated(e,1)
					PlaySound(e,0)
					SetAnimationName(e,"on")
					PlayAnimation(e)
					mode[e] = "Pressurise"
					airlock[e].state = 2
					wait[e] = g_Time + delaysecs[e]
				end
			end	
		end
	end
	
	if airlock[e].state == 2 then -- Close Both Doors
	
		SetActivatedWithMP(airlock[e].door1_number,0)
		SetActivatedWithMP(airlock[e].door2_number,0)

		if mode[e] == "Depressurise" and airlock_status[e] == "Pressurised" then
			SetActivatedWithMP(airlock[e].door1_number,1)
			SetActivatedWithMP(airlock[e].door2_number,0)
			innerdoor[e] = "closed"
			doordelay[e] = 600
			airlock_status[e] = "DePressurised"
		end	

		if mode[e] == "Pressurise" and airlock_status[e] == "Depressurised" then
			SetActivatedWithMP(airlock[e].door1_number,0)
			SetActivatedWithMP(airlock[e].door2_number,1)
			outerdoor[e] = "closed"
			doordelay[e] = 600
			airlock_status[e] = "Pressurised"
		end

		if doordelay[e] > 0 then
			doordelay[e] = doordelay[e] -3
			if doordelay[e] <= 0 then LoopSound(e,1) end
		end
		if mode[e] == "Depressurise" and doordelay[e] <= 0 then
			LoopSound(e,1)
			ActivateIfUsed(e)
			PromptLocal(e,airlock[e].depressurise_text.. " : " ..countdown[e].. " seconds")
		end
		if mode[e] == "Pressurise" and doordelay[e] <= 0 then
			LoopSound(e,1)
			ActivateIfUsed(e)
			PromptLocal(e,airlock[e].pressurise_text.. " : " ..countdown[e].. " seconds")
		end
	end

	if airlock[e].state == 3 then -- Open Outer Door
		if outerdoor[e] == "closed" then
			SetActivatedWithMP(airlock[e].door2_number,1)
			StopSound(e,1)
			SetActivated(e,0)
			airlock[e].state = 1 --(reset)
			mode[e] = ""
			if g_Entity[e]['activated'] == 1 then SetActivated(e,0) end
			SetAnimationName(e,"off")
			PlayAnimation(e)
			outerdoor[e] = "open"
			airlock_status[e] = "Depressurised"
		end	
	end
	
	if airlock[e].state == 4 then -- Open Inner Door
		if innerdoor[e] == "closed" then		
			SetActivatedWithMP(airlock[e].door1_number,1)
			StopSound(e,1)
			SetActivated(e,0)
			airlock[e].state = 1 --(reset)
			mode[e] = ""
			if g_Entity[e]['activated'] == 1 then SetActivated(e,0) end
			SetAnimationName(e,"off")
			PlayAnimation(e)
			innerdoor[e] = "open"
			airlock_status[e] = "Pressurised"
		end	
	end

	if g_Time > wait[e] and mode[e] == "Depressurise" then
		if doonce[e] == 0 then
			airlock[e].state = 3
			PerformLogicConnections(e)
			doonce[e] = 1
		end
	end
	if g_Time > wait[e] and mode[e] == "Pressurise" then
		if doonce[e] == 0 then
			airlock[e].state = 4
			PerformLogicConnections(e)
			doonce[e] = 1
		end
	end

	countdown[e] = math.floor((wait[e] - g_Time) / 1000)
	if countdown[e] <= -1 then countdown[e] = 0 end

	--Air airlock
	if airlock[e].airlock_type == 1 then
		if mode[e] == "Depressurise" then -- Depressurise Air
			SetGamePlayerControlDrownTimestamp(Timer() + 8000)
			if countdown[e] == 0 then atmosphere[e] = 1 end
		end
		if mode[e] == "Pressurise" then -- Pressurise Air
			if countdown[e] == 0 then atmosphere[e] = 0 end
		end
	end

	if atmosphere[e] == 1 then
		if ( Timer() > GetGamePlayerControlDrownTimestamp() ) then
			if airlock[e].air_level < 100 then HurtPlayer(-1,100-airlock[e].air_level) end
			SetGamePlayerControlDrownTimestamp(Timer() + 8000)
		end
	end

	--Water airlock
	if airlock[e].airlock_type == 2 then
		g_waterControlMasterE = e
		if g_waterControlMasterE == e then
			if airlock[e].npc_drowning == 0 then
				for a = 1, g_EntityElementMax do
					if a ~= nil and g_Entity[a] ~= nil then
						local allegiance = GetEntityAllegiance(a) -- (0-enemy, 1-ally, 2-neutral)
						if allegiance == 0 or allegiance == 1 or allegiance == 2 then
							SetEntityUnderwaterMode(a,1)
						end
					end
				end
			end	

			if mode[e] == "Depressurise" and current_wlevel[e] <= base_wlevel[e]+airlock[e].water_level then -- Raise Water Height
				if current_wlevel[e] <= current_wlevel[e]+airlock[e].water_level then
					current_wlevel[e] = current_wlevel[e] + 0.2
					SetWaterHeight(current_wlevel[e])
					ShowWater()
				end
				SetGamePlayerControlInWaterState(1)
				if countdown[e] == 0 then SetGamePlayerControlInWaterState(2) end
			end
			if mode[e] == "Pressurise" and current_wlevel[e] >= base_wlevel[e] then -- Lower Water Height
				if current_wlevel[e] >= current_wlevel[e]-airlock[e].water_level then
					current_wlevel[e] = current_wlevel[e] - 0.2
					SetWaterHeight(current_wlevel[e])
					ShowWater()
				end
			end
		end	
	end
	SetWaterHeight(current_wlevel[e])
end
