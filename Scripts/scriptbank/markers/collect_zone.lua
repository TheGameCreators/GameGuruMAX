-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Collect Zone v8 by Necrym59
-- DESCRIPTION: If an object entity enters the zone, displays a message and will remove/destroy the entity.
-- DESCRIPTION: [@ENTITY_TYPE=3(1=Active Object Only, 2=Character Only, 3=Objects Only)]
-- DESCRIPTION: [NOTIFICATION$="Entity collected"]
-- DESCRIPTION: [ZONEHEIGHT=50] controls how far above the zone the entity can be before the zone is not triggered
-- DESCRIPTION: [@COUNT_STYLE=1(1=Add, 2=Deduct)]
-- DESCRIPTION: [@COUNT_TYPE=1(1=Fixed Value, 2=By Weight)]
-- DESCRIPTION: [FIXED_VALUE=0(0,100)] to increment/deduct the User Global by.
-- DESCRIPTION: [@@USER_GLOBAL_AFFECTED$=""(0=globallist)] User Global that will be affected (eg; MyPoints)
-- DESCRIPTION: [!ACTIVATE_LOGIC=0]
-- DESCRIPTION: Plays <Sound0> when triggered.

local collect_zone 			= {}
local entity_type 			= {}	
local notification 			= {}
local zoneheight			= {}
local count_style			= {}
local count_type			= {}
local fixed_value			= {}
local user_global_affected 	= {}
local activate_logic		= {}

local played 		= {}
local doonce		= {}
local objweight		= {}
local checkEnt		= {}
local currentvalue	= {}
local EntityID		= {}
local status		= {}

	
function collect_zone_properties(e, entity_type, notification, zoneheight, count_style, count_type, fixed_value, user_global_affected, activate_logic)
	collect_zone[e].notification = notification	
	collect_zone[e].entity_type = entity_type or 3
	collect_zone[e].zoneheight = zoneheight or 100
	collect_zone[e].count_style = count_style or 1
	collect_zone[e].count_type = count_type or 1
	collect_zone[e].fixed_value = fixed_value or 1
	collect_zone[e].user_global_affected = user_global_affected	
	collect_zone[e].activate_logic = activate_logic	
end 

function collect_zone_init(e)
	collect_zone[e] = {}
	collect_zone[e].entity_type = 3
	collect_zone[e].notification = "Entity collected"	
	collect_zone[e].zoneheight = 100
	collect_zone[e].count_style = 1
	collect_zone[e].count_type = 1	
	collect_zone[e].fixed_value = 1
	collect_zone[e].user_global_affected = ""
	collect_zone[e].activate_logic = 0	
	
	EntityID[e] = 0
	status[e] = "init"
	doonce[e] = 0
	checkEnt[e] = 0
	objweight[e] = 0
	currentvalue[e] = 0	
end

function collect_zone_main(e)

	if status[e] == "init" then
		status[e] = "endinit"
	end
	
	GetEntityInZoneWithFilter(e,collect_zone[e].entity_type)
	EntityID[e] = g_Entity[e]['entityinzone']
	objweight[e] = GetEntityWeight(EntityID[e])
	if EntityID[e] ~= 0 and g_Entity[EntityID[e]]['y'] > g_Entity[e]['y']-1 and g_Entity[EntityID[e]]['y'] < g_Entity[e]['y'] + collect_zone[e].zoneheight then
		if doonce[e] == 0 then
			Prompt(collect_zone[e].notification)
			PlaySound(e,0)
			doonce[e] = 1
		end
		if collect_zone[e].user_global_affected > "" then
			if collect_zone[e].count_type == 1 then
				if _G["g_UserGlobal['"..collect_zone[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..collect_zone[e].user_global_affected.."']"] end
				if collect_zone[e].count_style == 1 then --add				
					_G["g_UserGlobal['"..collect_zone[e].user_global_affected.."']"] = currentvalue[e] + collect_zone[e].fixed_value/2
				end
				if collect_zone[e].count_style == 2 then --deduct				
					_G["g_UserGlobal['"..collect_zone[e].user_global_affected.."']"] = currentvalue[e] - collect_zone[e].fixed_value/2
				end
				Destroy(EntityID[e])
				if collect_zone[e].activate_logic == 1 then
					ActivateIfUsed(e)
					PerformLogicConnections(e)
				end
				doonce[e] = 0
			end	
			if collect_zone[e].count_type == 2 then
				if _G["g_UserGlobal['"..collect_zone[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..collect_zone[e].user_global_affected.."']"] end
				if collect_zone[e].count_style == 1 then --add				
					_G["g_UserGlobal['"..collect_zone[e].user_global_affected.."']"] = currentvalue[e] + objweight[e]/2
				end
				if collect_zone[e].count_style == 2 then --deduct				
					_G["g_UserGlobal['"..collect_zone[e].user_global_affected.."']"] = currentvalue[e] - objweight[e]/2
				end	
				Destroy(EntityID[e])
				if collect_zone[e].activate_logic == 1 then
					ActivateIfUsed(e)
					PerformLogicConnections(e)
				end				
				doonce[e] = 0
			end
		else
			Destroy(EntityID[e])
			if collect_zone[e].activate_logic == 1 then
				ActivateIfUsed(e)
				PerformLogicConnections(e)
			end
			doonce[e] = 0
		end		
	end
end

function collect_zone_exit(e)
end
