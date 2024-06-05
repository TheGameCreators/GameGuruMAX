-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Kill Count v2 by Necrym59
-- DESCRIPTION: Creates a counter for all enemies killed.
-- DESCRIPTION: [USER_GLOBAL_AFFECTED$=""] (eg: "MyKillCounter")

local killcount 	= {}
local user_global_affected	= {}

local pEntalive 	= {}
local pEnttotal		= {}
local status	 	= {}
local checktime		= {}
local currcount 	= {} 
local doonce		= {}

function killcount_properties(e, user_global_affected)
	killcount[e] = g_Entity[e]
	killcount[e].user_global_affected = user_global_affected
end 

function killcount_init(e)
	killcount[e] = g_Entity[e]
	killcount[e].user_global_affected = ""
	
	pEntalive[e] = 0
	pEnttotal[e] = 0
	checktime[e] = math.huge
	currcount[e] = 0
	doonce[e] = 0
	status[e] = 'check'
end

function killcount_main(e)

	if status[e] == 'check' then
		pEntalive[e] = 0
		for a = 1, g_EntityElementMax do
			if a ~= nil and g_Entity[a] ~= nil then 
				local allegiance = GetEntityAllegiance(a)				
				if allegiance == 0 then
					if g_Entity[a]['health'] > 1 then
						pEntalive[e] = pEntalive[e] + 1
					end
				end
			end				
		end
		if doonce[e] == 0 then
			pEnttotal[e] = pEntalive[e]
			doonce[e] = 1
		end
		currcount[e] = (pEnttotal[e] - pEntalive[e])
		checktime[e] = g_Time + 1000
		status[e] = 'run'
	end
	
	if status[e] == 'run' then
		if killcount[e].user_global_affected > "" then
			if _G["g_UserGlobal['"..killcount[e].user_global_affected.."']"] ~= nil then
				_G["g_UserGlobal['"..killcount[e].user_global_affected.."']"] = currcount[e]
			end
		end	
		if g_Time > checktime[e] then		
			if pEntalive[e] > 0 then status[e] = 'check' end
		end	
	end
end