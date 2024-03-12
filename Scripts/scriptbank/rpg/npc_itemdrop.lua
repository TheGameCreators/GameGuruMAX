-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- NPC Item Drop v11 by Necrym59
-- DESCRIPTION: Allows a named item to be carried and dropped by a named npc.
-- DESCRIPTION: Make the named item: Always Active=ON, Physics=ON, Gravity=ON
-- DESCRIPTION: [NPC_NAME$=""]
-- DESCRIPTION: [ITEM_NAME$=""]

	local lower = string.lower
	
	local npc_itemdrop 	= {}
	local npc_name		= {}
	local item_name		= {}
	local npc_no		= {}
	local item_no		= {}
	
	local pEntn			= {}
	local pEnta			= {}
	local status		= {}
	local wait 			= {}
	local drop_delay 	= {}	
	
function npc_itemdrop_properties(e, npc_name, item_name, npc_no, item_no)
	npc_itemdrop[e] = g_Entity[e]
	npc_itemdrop[e].npc_name = lower(npc_name)
	npc_itemdrop[e].item_name = lower(item_name)
	npc_itemdrop[e].npc_no = 0
	npc_itemdrop[e].item_no = 0
end

function npc_itemdrop_init(e)
	npc_itemdrop[e] = {}
	npc_itemdrop[e].npc_name = ""	
	npc_itemdrop[e].item_name = ""
	npc_itemdrop[e].npc_no = 0
	npc_itemdrop[e].item_no = 0
	npc_itemdrop[e].drop_delay = 0
	status[e] = "init"	
	pEntn[e] = 0
	pEnta[e] = 0
	wait[e] = math.huge
end

function npc_itemdrop_main(e)
	
	if status[e] == "init" then
		npc_itemdrop[e].npc_no = 0
		npc_itemdrop[e].item_no = 0
		wait[e] = g_Time + 10
		status[e] = "carrier"
	end
	
	if status[e] == "carrier" then
		if npc_itemdrop[e].npc_no == 0 then
			for n = 1, g_EntityElementMax do			
				if n ~= nil and g_Entity[n] ~= nil then										
					if lower(GetEntityName(n)) == npc_itemdrop[e].npc_name then
						npc_itemdrop[e].npc_no = n
						pEntn[e] = n
						status[e] = "carried_item"
						break
					end			
				end				
			end
		end
	end	
	if status[e] == "carried_item" then
		if npc_itemdrop[e].item_no == 0 or nil then
			for a = 1, g_EntityElementMax do			
				if a ~= nil and g_Entity[a] ~= nil then										
					if lower(GetEntityName(a)) == npc_itemdrop[e].item_name then
						npc_itemdrop[e].item_no = a
						pEnta[e] = a
						Hide(pEnta[e])
						GravityOff(pEnta[e])
						CollisionOff(pEnta[e])						
						break
					end					
				end
			end			
		end
		if g_Time > wait[e] then			
			ResetPosition(pEnta[e],g_Entity[pEntn[e]].x, g_Entity[pEntn[e]].y+500, g_Entity[pEntn[e]].z)
			wait[e] = g_Time + 1000
		end	
		if g_Entity[pEntn[e]].health <= 0 then
			ResetPosition(pEnta[e],g_Entity[pEntn[e]].x+10, g_Entity[pEntn[e]].y+20, g_Entity[pEntn[e]].z+10)
			drop_delay[e] = g_Time + 10
			status[e] = "drop_item"
		end	
	end
	if status[e] == "drop_item" then
		if g_Time > drop_delay[e] then
			GravityOn(pEnta[e])
			CollisionOn(pEnta[e])
			Show(pEnta[e])
			status[e] = "dropped_item"
		end		
	end
end