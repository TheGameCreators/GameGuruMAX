-- No_Kill v2 by Necrym59
-- DESCRIPTION: Global behavior will kill the player if they kill a NPC of the designated type. Attach to an object, set AlwaysActive=ON.
-- DESCRIPTION:[@NPC_TYPE=1(0=Enemy, 1=Ally, 2=Neutral, 3=Ally+Neutral)]

local nokill 		= {}
local npc_type	 	= {}
local nklist 		= {}
local status		= {}

function no_kill_properties(e, npc_type)
	nokill[e].npc_type = npc_type or 1
end 

function no_kill_init(e)
	nokill[e] = {}
	nokill[e].npc_type = 2
	nokill[e].checkdelay = 0.5
	nokill[e].timer = 0
	
	status[e] = "init"
end 

function no_kill_main(e)

	if status[e] == "init" then
		for n = 1, g_EntityElementMax do
			if n ~= nil and g_Entity[n] ~= nil then
				local allegiance = GetEntityAllegiance(n)
				if nokill[e].npc_type == 0 then 
					if allegiance == 0 then 
						table.insert(nklist,n)
					end
				end
				if nokill[e].npc_type == 1 then 
					if allegiance == 1 then 
						table.insert(nklist,n)
					end
				end
				if nokill[e].npc_type == 2 then 
					if allegiance == 2 then 
						table.insert(nklist,n)
					end
				end
				if nokill[e].npc_type == 3 then 
					if allegiance == 1 or allegiance == 2 then 
						table.insert(nklist,n)
					end
				end				
			end
		end
		status[e] = "endinit"
	end
	
	if g_PlayerGunFired > 0 then 
		for a,b in pairs (nklist) do
			if g_Entity[b].health < 1 then 
				HurtPlayer(e,g_PlayerHealth)
				table.remove(nklist,a)
				break 
			end 
		end 
	else 
		nokill[e].timer = nokill[e].timer + GetElapsedTime()
		if nokill[e].timer > nokill[e].checkdelay then 
			nokill[e].timer = 0
			for a,b in pairs (nklist) do 
				if g_Entity[b].health < 1 then 
					table.remove(nklist,a)
				end 
			end 
		end 	
	end
end 