-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Toxic Zone v8 by Necrym59
-- DESCRIPTION: The player will be effected with healthloss while in this Zone
-- DESCRIPTION: Link to a trigger Zone.
-- DESCRIPTION: [PROMPT_TEXT$="In Toxic Zone use protection"]
-- DESCRIPTION: Toxic [@EFFECT=1(1=Gas, 2=Radiation)]
-- DESCRIPTION: Zone Height [ZONEHEIGHT=100(0,1000)]
-- DESCRIPTION: [@TOXIC_TO_NPC=1(1=Yes, 2=No)]
-- DESCRIPTION: <Sound0> - Zone Effect Sound
-- DESCRIPTION: <Sound1> - Pain Sound

g_toxiczone = {}
g_ppequipment = {}
local toxiczone 		= {}
local prompt_text 		= {}
local effect 			= {}
local zoneheight		= {}
local toxic_to_npc		= {}

function toxiczone_properties(e, prompt_text, effect, zoneheight, toxic_to_npc)
	toxiczone[e] = g_Entity[e]
	toxiczone[e].prompt_text = prompt_text
	toxiczone[e].effect = effect
	toxiczone[e].zoneheight = zoneheight or 100
	toxiczone[e].toxic_to_npc = toxic_to_npc
end
 
function toxiczone_init(e)
	toxiczone[e] = g_Entity[e]
	toxiczone[e].prompt_text = "In Toxic Zone use protection"
	toxiczone[e].effect = 1
	toxiczone[e].zoneheight = 100
	toxiczone[e].toxic_to_npc = 1
	g_ppequipment = 0
	StartTimer(e)
	EntityID = 0	
end
 
function toxiczone_main(e)	
	toxiczone[e] = g_Entity[e]
	
	if g_Entity[e]['plrinzone'] == 1 and g_PlayerHealth > 0 and g_PlayerPosY < g_Entity[e]['y']+toxiczone[e].zoneheight then
		PromptDuration(toxiczone[e].prompt_text,3000)
		if toxiczone[e].effect == 1 then	--Health Loss
			g_toxiczone = 'gas'
			LoopSound(e,0)
			if GetTimer(e) > 1000 then
				if g_ppequipment == 0 then PlaySound(e,1) end
				SetPlayerHealth(g_PlayerHealth - 1)				
				StartTimer(e)
			end			
		end		
		if toxiczone[e].effect == 2 then	--Radiation
			g_toxiczone = 'radiation'
			LoopSound(e,0)
			if GetTimer(e) > 1000 then
				if g_ppequipment == 0 then PlaySound(e,1) end
				SetPlayerHealth(g_PlayerHealth - 2)
				StartTimer(e)
			end			
		end	
	end	

	if g_Entity[e]['plrinzone'] == 0 then
		StopSound(e,0)
		StopSound(e,1)
		g_toxiczone = ""
	end
	
	GetEntityInZone(e)
	local EntityID = g_Entity[e]['entityinzone']	
	if EntityID > 0 and g_Entity[e]['entityinzone'] < g_Entity[e]['y'] + toxiczone[e].zoneheight then		
		if GetTimer(e) > 1000 then 
			if toxiczone[e].toxic_to_npc == 1 then SetEntityHealth(EntityID,g_Entity[EntityID]['health']-10) end			
			StartTimer(e)			
		end	
	end			
end
 
function toxiczone_exit(e)	
end

