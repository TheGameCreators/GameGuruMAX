-- Hurt NPC v2 by Necrym59
-- DESCRIPTION: Will hurt any NPC when within proximity range of a named entity
-- DESCRIPTION: [ENTITY_NAME$=""]
-- DESCRIPTION: [HURT_RANGE=500(0,3000)]
-- DESCRIPTION: [HURT_AMOUNT=10(0,100)]
-- DESCRIPTION: [HURT_TIME=3(1,10)] seconds
-- DESCRIPTION: [HurtOnceOnly!=0].

local U = require "scriptbank\\utillib"
local lower = string.lower

local hurtnpc		= {}
local entity_name 	= {}
local hurt_range 	= {}
local hurt_amount 	= {}
local hurt_time 	= {}

local htimer 		= {}
local hEnt			= {}
local hurtonce		= {}
local status		= {}

function hurt_npc_properties(e, entity_name, hurt_range, hurt_amount, hurt_time, HurtOnceOnly)
	hurtnpc[e].entity_name = lower(entity_name)
	hurtnpc[e].hurt_range = hurt_range
	hurtnpc[e].hurt_amount = hurt_amount
	hurtnpc[e].hurt_time = hurt_time
	hurtnpc[e].HurtOnceOnly = HurtOnceOnly
end

function hurt_npc_init(e)
	hurtnpc[e] = {}
	hurtnpc[e].entity_name = ""
	hurtnpc[e].hurt_range = 500
	hurtnpc[e].hurt_amount = 10
	hurtnpc[e].hurt_time = 3
	hurtnpc[e].HurtOnceOnly = 0

	htimer[e] = 0
	hEnt[e]	= 0
	hurtonce[e] = 0
	status[e] = "init"
end


function hurt_npc_main(e)
	if status[e] == "init" then
		for n = 1, g_EntityElementMax do
			if n ~= nil and g_Entity[n] ~= nil then
				if lower(GetEntityName(n)) == hurtnpc[e].entity_name then					
					hEnt[e] = n
				end
			end
		end
		status[e] = "endinit"
	end
	if hurtnpc[e].HurtOnceOnly == 0 then
		if g_Time > htimer[e] then
			htimer[e] = g_Time + (hurtnpc[e].hurt_time*1000)
			for _, v in pairs( U.ClosestEntities(hurtnpc[e].hurt_range,math.huge,g_Entity[hEnt[e]]['x'],g_Entity[hEnt[e]]['z'])) do
				if GetEntityAllegiance(v) ~= -1 then
					if g_Entity[v]['health'] > 0 then
						SetEntityHealth(v,g_Entity[v]['health']-hurtnpc[e].hurt_amount)
					end
				end
			end
		end
	end	
	if hurtnpc[e].HurtOnceOnly == 1 then
		if hurtonce[e] == 0 then
			for _, v in pairs( U.ClosestEntities(hurtnpc[e].hurt_range,math.huge,g_Entity[hEnt[e]]['x'],g_Entity[hEnt[e]]['z'])) do
				if GetEntityAllegiance(v) ~= -1 then
					if g_Entity[v]['health'] > 0 then
						SetEntityHealth(v,g_Entity[v]['health']-hurtnpc[e].hurt_amount)
						hurtonce[e] = 1
						SwitchScript(e,"no_behavior_selected.lua")
					end
				end
			end			
		end		
	end
end