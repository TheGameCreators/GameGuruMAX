-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Gasmask v10   by Necrym59
-- DESCRIPTION: The applied object will give the player a gas protection mask. Set Always active ON.
-- DESCRIPTION: Set the [PICKUP_TEXT$="E to Pickup"]
-- DESCRIPTION: Set the [PICKUP_RANGE=80(1-200)]
-- DESCRIPTION: Set the [USEAGE_TEXT$="G to wear, Q to remove"]
-- DESCRIPTION: Hud screen [IMAGEFILE$="imagebank\\misc\\testimages\\gasmask.png"]
-- DESCRIPTION: <Sound0> Put-on Remove Sound
-- DESCRIPTION: <Sound1> Breathing Sound

g_gasmask = {}
g_toxiczone = {}
g_ppequipment = {}
local gasmask 		= {}
local pickup_text 	= {}
local useage_text 	= {}
local screen_image 	= {}
local status 		= {}

local gmswitch 		= {}
local have_gasmask 	= {}
local currenthealth = {}
local played		= {}
	
function gasmask_properties(e, pickup_text, pickup_range, useage_text, screen_image)
	g_gasmask[e] = g_Entity[e]
	g_gasmask[e]['pickup_text'] = pickup_text
	g_gasmask[e]['pickup_range'] = pickup_range	
	g_gasmask[e]['useage_text'] = useage_text	
	g_gasmask[e]['screen_image'] = imagefile or screen_image
end 
	
function gasmask_init(e)
	g_gasmask[e] = g_Entity[e]
	g_gasmask[e]['pickup_text'] = "E to Pickup"
	g_gasmask[e]['pickup_range'] = 80
	g_gasmask[e]['useage_text'] = "G to wear, Q to remove"
	g_gasmask[e]['screen_image'] = "imagebank\\misc\\testimages\\gasmask.png"	
	status[e] = "init"
	currenthealth[e] = 0	
	have_gasmask = 0
	g_ppequipment = 0
	played[e] = 0
	gmswitch[e] = 0
	g_ppe = 1	--Gasmask = 1, Radsuit = 2
end
 
function gasmask_main(e)
	g_gasmask[e] = g_Entity[e]
	if status[e] == "init" then	
		gasmask = CreateSprite(LoadImage(g_gasmask[e]['screen_image']))
		SetSpriteSize(gasmask,100,100)
		SetSpritePosition(gasmask,200,200)
		currenthealth[e] = g_PlayerHealth
		status = "endinit"
	end
	local PlayerDist = GetPlayerDistance(e)
		
	if have_gasmask == 0 then
		local LookingAt = GetPlrLookingAtEx(e,1)	
		if LookingAt == 1 and PlayerDist < g_gasmask[e]['pickup_range'] and g_PlayerHealth > 0 then
			PromptLocalForVR(e,g_gasmask[e]['pickup_text'])
			if g_KeyPressE == 1 then				
				have_gasmask = 1					
				PlaySound(e,0)
				Hide(e)
				CollisionOff(e)
				ActivateIfUsed(e)
				PromptDuration(g_gasmask[e]['useage_text'],3000)
			end
		end
	end
	
	if have_gasmask == 1 then	
		if gmswitch[e] == 0 then 
			if GetInKey() == "g" or GetInKey() == "G" and gmswitch[e] == 0 then
				if played[e] == 0 then
					PromptDuration(g_gasmask[e]['useage_text'],3000)
					PlaySound(e,0)
					played[e] = 1
				end
				gmswitch[e] = 1
			end	
		end
		if gmswitch[e] == 1 then 	
			if GetInKey() == "q" or GetInKey() == "Q" and gmswitch[e] == 1 then			
				if played[e] == 0 then
					PlaySound(e,0)
					played[e] = 1
				end
				gmswitch[e] = 0			
			end
		end
		if gmswitch[e] == 1 then
			PasteSpritePosition(gasmask,0,0)			
			LoopSound(e,1)
			if g_toxiczone == 'gas' then SetPlayerHealth(currenthealth[e]) end
			g_ppequipment = 1
			played[e] = 0
		end
		if gmswitch[e] == 0 then			
			StopSound(e,1)			
			PasteSpritePosition(gasmask,1000,1000)
			g_ppequipment = 0
			played[e] = 0
			currenthealth[e] = g_PlayerHealth
		end
		if g_PlayerHealth <=0 then have_gasmask = 0 end		
	end
end
