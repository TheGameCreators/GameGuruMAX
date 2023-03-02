-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- NPC Item Drop v7 by Necrym59
-- DESCRIPTION: Allows a named item to be carried and dropped by a named npc.
-- DESCRIPTION: Make the named item: Always Active=ON, Physics=ON, Gravity=ON
-- DESCRIPTION: [NPC_NAME$=""]
-- DESCRIPTION: [ITEM_NAME$=""]

	--local U = require "scriptbank\\utillib"
	local lower = string.lower
	
	local npc_itemdrop 	= {}
	local npc_name		= {}
	local item_name		= {}
	local npc_no		= {}
	local item_no		= {}
	
	local pEntn			= {}
	local pEnta			= {}
	local status		= {}
	
function npc_itemdrop_properties(e, npc_name, item_name, npc_no, item_no)
	npc_itemdrop[e] = g_Entity[e]
	npc_itemdrop[e]['npc_name'] = lower(npc_name)
	npc_itemdrop[e]['item_name'] = lower(item_name)
	npc_itemdrop[e]['npc_no'] = 0
	npc_itemdrop[e]['item_no'] = 0
end

function npc_itemdrop_init(e)
	npc_itemdrop[e] = g_Entity[e]
	npc_itemdrop[e]['npc_name'] = ""	
	npc_itemdrop[e]['item_name'] = ""
	npc_itemdrop[e]['npc_no'] = 0
	npc_itemdrop[e]['item_no'] = 0
	status[e] = "init"	
	pEntn = 0
	pEnta = 0
end

function npc_itemdrop_main(e)
	npc_itemdrop[e] = g_Entity[e]
	
	if status[e] == "init" then
		npc_itemdrop[e]['npc_no'] = 0
		npc_itemdrop[e]['item_no'] = 0
		status[e] = "carrier"
	end
	
	if status[e] == "carrier" then
		if npc_itemdrop[e]['npc_no'] == 0 then
			for n = 1, g_EntityElementMax do			
				if n ~= nil and g_Entity[n] ~= nil then										
					if lower(GetEntityName(n)) == npc_itemdrop[e]['npc_name'] then
						npc_itemdrop[e]['npc_no'] = n
						pEntn = n
						status[e] = "carried_item"
						break
					end			
				end				
			end
		end
	end	
	if status[e] == "carried_item" then
		if npc_itemdrop[e]['item_no'] == 0 or nil then
			for a = 1, g_EntityElementMax do			
				if a ~= nil and g_Entity[a] ~= nil then										
					if lower(GetEntityName(a)) == npc_itemdrop[e]['item_name'] then
						npc_itemdrop[e]['item_no'] = a
						pEnta = a
						Hide(pEnta)
						break
					end					
				end
			end			
		end
		GravityOff(pEnta)
		CollisionOff(pEnta)
		SetPosition(pEnta,g_Entity[pEntn]['x'], g_Entity[pEntn]['y']+80, g_Entity[pEntn]['z'])
		ResetPosition(pEnta,g_Entity[pEntn]['x'], g_Entity[pEntn]['y']+80, g_Entity[pEntn]['z'])
		if g_Entity[pEntn]['health'] <= 0 then			
			SetPosition(pEnta,g_Entity[pEntn]['x']+32, g_Entity[pEntn]['y']+3, g_Entity[pEntn]['z']+32)
			ResetPosition(pEnta,g_Entity[pEntn]['x']+32, g_Entity[pEntn]['y']+3, g_Entity[pEntn]['z']+32)
			GravityOn(pEnta)
			CollisionOn(pEnta)
			Show(pEnta)
			status[e] = "dropped_item"
		end	
	end
end