-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Stealthzone v4
-- DESCRIPTION: The player will be in Stealth mode while in this Zone
-- DESCRIPTION: Link to a trigger Zone.
-- DESCRIPTION: [PROMPT_TEXT$="In Stealth Zone"]
-- DESCRIPTION: Stealth Shileld [DISCOVERY_RANGE=80(50,2000)]
-- DESCRIPTION: Zone Height [ZONEHEIGHT=100(0,1000)]

	local stealthzone 		= {}
	local prompt_text 		= {}
	local discovery_range 	= {}
	local zoneheight		= {}
	local shieldmode 		= {}
	local stealthicon 		= {}
	
function stealthzone_properties(e, prompt_text, discovery_range, zoneheight)
	stealthzone[e] = g_Entity[e]
	stealthzone[e].prompt_text = prompt_text
	stealthzone[e].discovery_range = discovery_range
	stealthzone[e].zoneheight = zoneheight or 100
end
 
function stealthzone_init(e)
	stealthzone[e] = g_Entity[e]
	stealthzone[e].prompt_text = "In Stealth Zone"
	stealthzone[e].discovery_range = 30
	stealthzone[e].zoneheight = 100	
	shieldmode = 0		
	stealthicon = CreateSprite(LoadImage("imagebank\\misc\\testimages\\stealthzone_icon.png"))
	SetSpriteSize(stealthicon,-1,-1)
	SetSpriteDepth(stealthicon,1)
	SetSpriteColor(stealthicon,0,0,0,3)
	SetSpritePosition(stealthicon,200,200)	
end
 
function stealthzone_main(e)	
	stealthzone[e] = g_Entity[e]
	
	if g_Entity[e]['plrinzone'] == 1 and g_PlayerHealth > 0 and g_PlayerPosY < g_Entity[e]['y']+stealthzone[e].zoneheight then
		shieldmode = 1
		Prompt(stealthzone[e].prompt_text)				
		for a = 1, g_EntityElementMax do
			if a ~= nil and g_Entity[a] ~= nil then 
				local allegiance = GetEntityAllegiance(a) -- (0-enemy, 1-ally, 2-neutral)			
				if allegiance == 0 then 
					Ent = g_Entity[a]
					if shieldmode == 1 then
						SetEntityAllegiance(a,2) -- Become Neutral						
					end				
				end
				if allegiance == 2 then
					Ent = g_Entity[a]
					if shieldmode == 0 then
						SetEntityAllegiance(a,0) -- Become Enemy
					end
				end
				PlayerDist = GetPlayerDistance(a)
				if PlayerDist <= stealthzone[e].discovery_range and shieldmode == 1 then 
					SetEntityAllegiance(a,0)				
					shieldmode = 0				
				end
				if shieldmode == 1 then PasteSpritePosition(stealthicon,92,92) end				
			end
		end
	end
	if g_Entity[e]['plrinzone'] == 0 then
		shieldmode = 0
		for a = 1, g_EntityElementMax do
			if a ~= nil and g_Entity[a] ~= nil then 
				local allegiance = GetEntityAllegiance(a) -- (0-enemy, 1-ally, 2-neutral)			
				if allegiance == 2 then
					Ent = g_Entity[a]
					if shieldmode == 0 then
						SetEntityAllegiance(a,0) -- Become Enemy
					end
				end	
				SetSpritePosition(stealthicon,200,200)
			end
		end
	end	
end
 
function stealthzone_exit(e)	
end


