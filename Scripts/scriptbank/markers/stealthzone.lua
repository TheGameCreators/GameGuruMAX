-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Stealthzone v5
-- DESCRIPTION: The player will be in Stealth mode while in this Zone
-- DESCRIPTION: Attach to a trigger Zone.
-- DESCRIPTION: [PROMPT_TEXT$="In Stealth Zone"]
-- DESCRIPTION: [DISCOVERY_RANGE=80(50,2000)]
-- DESCRIPTION: [ICON_IMAGEFILE$="imagebank\\misc\\testimages\\stealthzone_icon.png"]
-- DESCRIPTION: [ICON_POSITION_X=90(0,100)]
-- DESCRIPTION: [ICON_POSITION_Y=80(0,100)]
-- DESCRIPTION: Zone Height [ZONEHEIGHT=100(0,1000)]

local stealthzone 		= {}
local prompt_text 		= {}
local discovery_range 	= {}
local icon_imagefile 	= {}
local icon_position_x 	= {}
local icon_position_y 	= {}
local zoneheight		= {}

local shieldmode 		= {}
local stealthicon 		= {}
local sallegiance 		= {}
local doonce			= {}

function stealthzone_properties(e, prompt_text, discovery_range, icon_imagefile, icon_position_x, icon_position_y, zoneheight)
	stealthzone[e].prompt_text = prompt_text
	stealthzone[e].discovery_range = discovery_range
	stealthzone[e].icon_imagefile = icon_imagefile
	stealthzone[e].icon_position_x = icon_position_x
	stealthzone[e].icon_position_y = icon_position_y
	stealthzone[e].zoneheight = zoneheight or 100
end

function stealthzone_init(e)
	stealthzone[e] = {}
	stealthzone[e].prompt_text = "In Stealth Zone"
	stealthzone[e].discovery_range = 30
	stealthzone[e].icon_imagefile = "imagebank\\misc\\testimages\\stealthzone_icon.png"
	stealthzone[e].icon_position_x = 90
	stealthzone[e].icon_position_y = 80	
	stealthzone[e].zoneheight = 100
	shieldmode[e] = 0
	doonce[e] = 0
	stealthicon = CreateSprite(LoadImage(stealthzone[e].icon_imagefile))
	SetSpriteSize(stealthicon,-1,-1)
	SetSpriteDepth(stealthicon,1)
	SetSpriteColor(stealthicon,255,255,255,3)
	SetSpritePosition(stealthicon,200,200)
end

function stealthzone_main(e)
	
	if g_Entity[e]['plrinzone'] == 1 and g_PlayerHealth > 0 and g_PlayerPosY < g_Entity[e]['y']+stealthzone[e].zoneheight then
		shieldmode[e] = 1
		if doonce[e] == 0 then 
			PromptDuration(stealthzone[e].prompt_text,1000)
			doonce[e] = 1
		end		
		for a = 1, g_EntityElementMax do		
			if a ~= nil and g_Entity[a] ~= nil then
				local PlayerDist = GetPlayerDistance(a)
				sallegiance[e] = GetEntityAllegiance(a) -- (0-enemy, 1-ally, 2-neutral)
				if sallegiance[e] == 0 and PlayerDist < 3000 then
					Ent = g_Entity[a]
					if shieldmode[e] == 1 then
						SetEntityAllegiance(a,2) -- Become Neutral
					end
				end				
				if PlayerDist <= stealthzone[e].discovery_range and shieldmode[e] == 1 then
					SetEntityAllegiance(a,0)
					shieldmode[e] = 0
				end
				if shieldmode[e] == 1 then PasteSpritePosition(stealthicon,stealthzone[e].icon_position_x,stealthzone[e].icon_position_y) end
			end
		end
	end
	if g_Entity[e]['plrinzone'] == 0 then
		shieldmode[e] = 0
		doonce[e] = 0
		for a = 1, g_EntityElementMax do
			if a ~= nil and g_Entity[a] ~= nil then
				sallegiance[e] = GetEntityAllegiance(a) -- (0-enemy, 1-ally, 2-neutral)
				if sallegiance[e] == 2 then
					Ent = g_Entity[a]
					if shieldmode[e] == 0 then
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


