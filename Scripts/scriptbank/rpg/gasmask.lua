-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Gasmask v14   by Necrym59
-- DESCRIPTION: The applied object will give the player a gas protection mask. Set Always active ON.
-- DESCRIPTION: [PICKUP_TEXT$="E to Pickup"] 
-- DESCRIPTION: [PICKUP_RANGE=80(1-200)]
-- DESCRIPTION: [USEAGE_TEXT$="G to wear, Q to remove"]
-- DESCRIPTION: [IMAGEFILE$="imagebank\\misc\\testimages\\gasmask.png"] for screen overlay
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline)]
-- DESCRIPTION: <Sound0> Put-on Remove Sound
-- DESCRIPTION: <Sound1> Breathing Sound

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

g_toxiczone = {}
g_gasmask_on = {}

local gasmask			= {}
local pickup_text		= {}
local useage_text		= {}
local screen_image		= {}
local prompt_display 	= {}
local item_highlight 	= {}

local status			= {}
local gasmasksp			= {}
local gmswitch			= {}
local have_gasmask		= {}
local currenthealth		= {}
local played			= {}
local selectobj 		= {}
local tEnt 				= {}
	
function gasmask_properties(e, pickup_text, pickup_range, useage_text, screen_image, prompt_display, item_highlight)
	gasmask[e].pickup_text = pickup_text
	gasmask[e].pickup_range = pickup_range	
	gasmask[e].useage_text = useage_text	
	gasmask[e].screen_image = imagefile or screen_image
	gasmask[e].prompt_display = prompt_display
	gasmask[e].item_highlight = item_highlight	
end 
	
function gasmask_init(e)
	gasmask[e] = {}
	gasmask[e].pickup_text = "E to Pickup"
	gasmask[e].pickup_range = 80
	gasmask[e].useage_text = "G to wear, Q to remove"
	gasmask[e].screen_image = "imagebank\\misc\\testimages\\gasmask.png"
	gasmask[e].prompt_display = 1
	gasmask[e].item_highlight = 0	
	
	status[e] = "init"
	currenthealth[e] = 0	
	have_gasmask = 0
	g_gasmask_on = 0
	played[e] = 0
	gmswitch[e] = 0
	selectobj[e] = 0
	tEnt[e] = 0
	g_tEnt = 0
end
 
function gasmask_main(e)

	if status[e] == "init" then	
		gasmasksp = CreateSprite(LoadImage(gasmask[e].screen_image))
		SetSpriteSize(gasmasksp,100,100)
		SetSpritePosition(gasmasksp,200,200)
		currenthealth[e] = g_PlayerHealth
		status = "endinit"
	end
	local PlayerDist = GetPlayerDistance(e)
		
	if have_gasmask == 0 then
		if PlayerDist < gasmask[e].pickup_range and g_PlayerHealth > 0 then
			--pinpoint select object--
			module_misclib.pinpoint(e,gasmask[e].pickup_range,gasmask[e].item_highlight)
			tEnt[e] = g_tEnt
			--end pinpoint select object--	
			if PlayerDist < gasmask[e].pickup_range and tEnt[e] ~= 0 then
				if gasmask[e].prompt_display == 1 then PromptLocal(e,gasmask[e].pickup_text) end
				if gasmask[e].prompt_display == 2 then Prompt(gasmask[e].pickup_text) end		
				if g_KeyPressE == 1 then
					if GetEntityCollectable(tEnt[e]) == 0 then
						if gasmask[e].prompt_display == 1 then PromptDuration(gasmask[e].useage_text,3000) end
						if gasmask[e].prompt_display == 2 then PromptDuration(gasmask[e].useage_text,3000) end
						have_gasmask = 1					
						PlaySound(e,0)					
						CollisionOff(e)
						PerformLogicConnections(e)
						ActivateIfUsed(e)
						Hide(e)
					end
					if GetEntityCollectable(tEnt[e]) == 1 then
						if gasmask[e].prompt_display == 1 then PromptDuration(gasmask[e].useage_text,3000) end
						if gasmask[e].prompt_display == 2 then PromptDuration(gasmask[e].useage_text,3000) end
						have_gasmask = 1					
						PlaySound(e,0)					
						CollisionOff(e)
						PerformLogicConnections(e)
						ActivateIfUsed(e)
						SetEntityCollected(tEnt[e],1)
					end
				end	
			end
		end
	end
	if GetEntityCollected(tEnt[e]) == 0 then have_gasmask = 0 end
	if GetEntityCollected(tEnt[e]) == 1 then have_gasmask = 1 end
	
	if have_gasmask == 1 then	
		if gmswitch[e] == 0 then 
			if GetInKey() == "g" or GetInKey() == "G" and gmswitch[e] == 0 then
				if played[e] == 0 then
					if gasmask[e].prompt_display == 1 then PromptDuration(gasmask[e].useage_text,3000) end
					if gasmask[e].prompt_display == 2 then PromptDuration(gasmask[e].useage_text,3000) end
					PlaySound(e,0)
					played[e] = 1
				end
				gmswitch[e] = 1
				g_gasmask_on = 1
			end	
		end
		if gmswitch[e] == 1 then 	
			if GetInKey() == "q" or GetInKey() == "Q" and gmswitch[e] == 1 then			
				if played[e] == 0 then
					PlaySound(e,0)
					played[e] = 1
				end
				gmswitch[e] = 0
				g_gasmask_on = 0
			end
		end
		if gmswitch[e] == 1 then
			PasteSpritePosition(gasmasksp,0,0)			
			LoopSound(e,1)
			if g_toxiczone == 'gas' then SetPlayerHealth(currenthealth[e]) end						
			played[e] = 0
		end
		if gmswitch[e] == 0 then			
			StopSound(e,1)			
			PasteSpritePosition(gasmasksp,1000,1000)
			played[e] = 0
			currenthealth[e] = g_PlayerHealth
		end
		if g_PlayerHealth <=0 then have_gasmask = 0 end		
	end
end
