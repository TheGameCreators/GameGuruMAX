-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Hostile Zone v6 by Necrym59 and Lee
-- DESCRIPTION: The player must be unarmed while in the Hostile Zone.
-- DESCRIPTION: Link to a trigger Zone
-- DESCRIPTION: [PROMPT_TEXT$="In hostile zone, carry no weapon"]
-- DESCRIPTION: [HOSTILITY_RANGE=100(0,1000)]
-- DESCRIPTION: [ZONEHEIGHT=100(0,1000)]

local hostilezone 		= {}
local prompt_text 		= {}
local hostility_range	= {}	
local zoneheight		= {}
	
local hostility 		= {}
--local entrange			= {}
local tableName			= {}	
local status 			= {}
local doonce			= {}
local promptonce		= {}
local exitzone			= {}
	
function hostile_zone_properties(e, prompt_text, hostility_range, zoneheight)
	hostilezone[e].prompt_text = prompt_text
	hostilezone[e].hostility_range = hostility_range
	hostilezone[e].zoneheight = zoneheight or 100
end
 
function hostile_zone_init(e)
	hostilezone[e] = {}
	hostilezone[e].prompt_text = "In Hostile Zone,keep weapon holstered"
	hostilezone[e].hostility_range = 100
	hostilezone[e].zoneheight = 100	
	hostility[e] = 2
	exitzone[e] = 0
	--entrange[e] = 0
	doonce[e] = 0
	promptonce[e] = 0
	tableName[e] = "holstilelist" ..tostring(e)
	_G[tableName[e]] = {}	
	status[e] = "init"
end
 
function hostile_zone_main(e)	

	if status[e] == "init" then	
		for n = 1, g_EntityElementMax do
			if n ~= nil and g_Entity[n] ~= nil then
				if GetEntityAllegiance(n) == 0 then
					local tdistfromzonecenter = math.ceil(GetDistanceTo(e,g_Entity[n]['x'],g_Entity[e]['y'],g_Entity[n]['z']))
					if g_Entity[n]["health"] > 0 and tdistfromzonecenter < hostilezone[e].hostility_range then
						table.insert(_G[tableName[e]],n)
					end
				end
			end
		end	
		status[e] = "endinit"
	end

	WeaponID = GetPlayerWeaponID()
	
	if g_Entity[e]['plrinzone'] == 1 and g_PlayerHealth > 0 and g_PlayerPosY < g_Entity[e]['y']+hostilezone[e].zoneheight then
		if promptonce[e] == 0 then
			PromptDuration(hostilezone[e].prompt_text, 3000)
			promptonce[e] = 1
		end	
		if WeaponID == 0 then
			hostility[e] = 0
			exitzone[e] = 0
			doonce[e] = 0
		end
		if WeaponID > 0 then
			hostility[e] = 1
			exitzone[e] = 1
		end
	end
	
	if g_Entity[e]['plrinzone'] == 0 and hostility[e] == 1 then
		if exitzone[e] == 1 then Destroy(e) end
		if exitzone[e] == 0 then Destroy(e) end
	end	
	if g_Entity[e]['plrinzone'] == 0 and hostility[e] == 0 then
		promptonce[e] = 0
	end	
	
	if hostility[e] == 0 then
		if doonce[e] == 0 then
			for _,v in pairs (_G[tableName[e]]) do
				if g_Entity[v] ~= nil then
					if g_Entity[v]["health"] > 0 then
						SetEntityAllegiance(v,2)
					end
				end
			end
		end	
		doonce[e] = 1
	end
	
	if hostility[e] == 1 then
		for _,v in pairs (_G[tableName[e]]) do
			if g_Entity[v] ~= nil then
				if g_Entity[v]["health"] > 0 then
					SetEntityAllegiance(v,0)
				end						
			end
		end
	end
	
	-- debug hostile mode
	-- PromptGuruMeditation("Hostile Mode = "..hostility[e])
	
end
 
function GetFlatDistanceToPlayer(v)
	if g_Entity[v] ~= nil then
		local distDX = g_PlayerPosX - g_Entity[v]['x']
		local distDZ = g_PlayerPosZ - g_Entity[v]['z']
		return math.sqrt((distDX*distDX)+(distDZ*distDZ));
	end
end