-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Remover Zone v5 by Necrym59
-- DESCRIPTION: When player enters the zone, displays a notification and will remove/destroy
-- DESCRIPTION: named entity/entities, activates any logic links, then destroys this zone.
-- DESCRIPTION: [NOTIFICATION$="Entity/Entities removed"]
-- DESCRIPTION: [ENTITY_NAME$=""] Name of entity/entities.
-- DESCRIPTION: [@REMOVE_STYLE=1 (1=Instant, 2=Timed, 3=Fade)]
-- DESCRIPTION: [REMOVE_TIME=1(0,100)] Seconds
-- DESCRIPTION: [ZONEHEIGHT=100] controls how far above the zone the player can be before the zone is not triggered
-- DESCRIPTION: [SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this zone
-- DESCRIPTION: <Sound0> Plays when triggered.

local lower = string.lower
local remover_zone 	= {}
local notification 	= {}
local entity_name	= {}
local remove_style 	= {}
local remove_time 	= {}
local zoneheight	= {}
local spawnatstart	= {}

local tableName		= {}
local ents			= {}
local entity_no		= {}
local played 		= {}
local wait			= {}
local doonce		= {}
local dlonce		= {}
local status		= {}
local current_level = {}

function remover_zone_properties(e, notification, entity_name, remove_style, remove_time, zoneheight, spawnatstart)
	remover_zone[e].notification = notification
	remover_zone[e].entity_name = lower(entity_name)
	remover_zone[e].remove_style = remove_style
	remover_zone[e].remove_time = remove_time
	remover_zone[e].zoneheight = zoneheight or 100
	remover_zone[e].spawnatstart	= spawnatstart
end

function remover_zone_init(e)
	remover_zone[e] = {}
	remover_zone[e].notification = "Entity removed"
	remover_zone[e].entity_name = ""
	remover_zone[e].remove_style = 1
	remover_zone[e].remove_time = 0
	remover_zone[e].zoneheight = 100
	remover_zone[e].spawnatstart	= 1
	remover_zone[e].entity_no = 0

	played[e] = 0
	doonce[e] = 0
	dlonce[e] = 0
	ents[e] = 0
	status[e] = "init"
	current_level[e] = 100
	wait[e] = math.huge
	tableName[e] = "entlist" ..tostring(e)
	_G[tableName[e]] = {}
end

function remover_zone_main(e)

	if status[e] == "init" then
		if remover_zone[e].entity_name > "" and remover_zone[e].entity_no == 0 then
			for n = 1, g_EntityElementMax do
				if n ~= nil and g_Entity[n] ~= nil then
					if lower(GetEntityName(n)) == remover_zone[e].entity_name then
						remover_zone[e].entity_no = n
						ents[e] = ents[e] + 1
						table.insert(_G[tableName[e]],n)						
					end
				end
			end
		end
		if remover_zone[e].spawnatstart == 1 then SetActivated(e,1) end
		if remover_zone[e].spawnatstart == 0 then SetActivated(e,0) end
		status[e] = "endinit"
	end

	if g_Entity[e]['activated'] == 1 then
		if g_Entity[e]['plrinzone'] == 1 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+remover_zone[e].zoneheight then
			if remover_zone[e].remove_style == 1 then --Instant
				if ents[e] > 0 then
					for a,b in pairs (_G[tableName[e]]) do
						if g_Entity[b] ~= nil then
							Hide(b)
							CollisionOff(b)
							EffectStop(b)
							Destroy(b)
							_G[tableName[e]][a] = nil
						end
						ents[e] = 0
					end
				end
				if played[e] == 0 then
					PromptDuration(remover_zone[e].notification,1000)
					PlaySound(e,0)
					played[e] = 1
				end
				ActivateIfUsed(e)
				PerformLogicConnections(e)
				if dlonce[e] == 0 then
					wait[e] = g_Time + 1000
					dlonce[e] = 1
				end
			end
			if remover_zone[e].remove_style == 2 then -- Timed
				if dlonce[e] == 0 then
					for a,b in pairs (_G[tableName[e]]) do
						if g_Entity[b] ~= nil then
							ModulateSpeed(b,0)
						end
					end				
					wait[e] = g_Time + (remover_zone[e].remove_time * 1000)
					dlonce[e] = 1
				end
			end
			if remover_zone[e].remove_style == 3 then --Fade
				if dlonce[e] == 0 then
					for a,b in pairs (_G[tableName[e]]) do
						if g_Entity[b] ~= nil then
							ModulateSpeed(b,0)
							SetEntityTransparency(b,1)
						end
						if current_level[e] > 0 then
							EffectSetOpacity(b,current_level[e])
							SetEntityBaseAlpha(b,current_level[e])
							current_level[e] = current_level[e] - 0.1
							if current_level[e] <= 0 then								
								current_level[e] = 0
								EffectStop(b)
								Hide(b)
								wait[e] = g_Time + 100
								dlonce[e] = 1
							end
						end
					end
				end
			end
		end
	end
	if g_Time > wait[e] then
		if remover_zone[e].remove_style == 2 or remover_zone[e].remove_style == 3 then
			if ents[e] > 0 then
				for a,b in pairs (_G[tableName[e]]) do
					if g_Entity[b] ~= nil then
						Hide(b)
						CollisionOff(b)						
						Destroy(b)
						_G[tableName[e]][a] = nil						
						current_level[e] = 100
					end
					ents[e] = 0
				end
			end
			if played[e] == 0 then
				PromptDuration(remover_zone[e].notification,1000)
				PlaySound(e,0)
				played[e] = 1
			end
			ActivateIfUsed(e)
			PerformLogicConnections(e)
		end
		Destroy(e)
	end
end

function remover_zone_exit(e)
end
