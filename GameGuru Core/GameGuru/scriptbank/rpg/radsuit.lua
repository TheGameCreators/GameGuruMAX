-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Rad suit v9   by Necrym59
-- DESCRIPTION: The applied object will give the player radiation protection. Set Always active ON.
-- DESCRIPTION: Set the [PICKUP_TEXT$="E to Pickup"]
-- DESCRIPTION: Set the [PICKUP_RANGE=80(1-200)]
-- DESCRIPTION: Set the [USEAGE_TEXT$="K to wear, Q to remove"]
-- DESCRIPTION: Hud screen [IMAGEFILE$="imagebank\\misc\\testimages\\radsuit.png"]
-- DESCRIPTION: <Sound0> Put-on Remove Sound
-- DESCRIPTION: <Sound1> Breathing Sound

g_radsuit = {}
g_toxiczone = {}
g_ppequipment = {}
local radsuit 		= {}
local pickup_text 	= {}
local useage_text 	= {}
local screen_image 	= {}
local status 		= {}

local rsswitch 		= {}
local have_radsuit 	= {}
local currenthealth = {}
local played		= {}
	
function radsuit_properties(e, pickup_text, pickup_range, useage_text, screen_image)
	g_radsuit[e] = g_Entity[e]
	g_radsuit[e]['pickup_text'] = pickup_text
	g_radsuit[e]['pickup_range'] = pickup_range		
	g_radsuit[e]['useage_text'] = useage_text	
	g_radsuit[e]['screen_image'] = imagefile or screen_image
end 
	
function radsuit_init(e)
	g_radsuit[e] = g_Entity[e]
	g_radsuit[e]['pickup_text'] = "E to Pickup"
	g_radsuit[e]['pickup_range'] = 80
	g_radsuit[e]['useage_text'] = "K to wear, Q to remove"
	g_radsuit[e]['screen_image'] ="imagebank\\misc\\testimages\\radsuit.png"
	status[e] = "init"
	currenthealth[e] = 0	
	have_radsuit = 0
	g_ppequipment = 0
	played[e] = 0
	rsswitch[e] = 0
end
 
function radsuit_main(e)
	g_radsuit[e] = g_Entity[e]
	if status[e] == "init" then
		radsuit = CreateSprite(LoadImage(g_radsuit[e]['screen_image']))
		SetSpriteSize(radsuit,100,100)
		SetSpritePosition(radsuit,200,200)		
		currenthealth[e] = g_PlayerHealth
		status = "endinit"
	end
	PlayerDist = GetPlayerDistance(e)
		
	if have_radsuit == 0 then
		if PlayerDist < g_radsuit[e]['pickup_range'] and g_PlayerHealth > 0 then
			PromptLocalForVR(e,g_radsuit[e]['pickup_text'])
			if g_KeyPressE == 1 then				
				have_radsuit = 1				
				PlaySound(e,0)
				Hide(e)
				CollisionOff(e)
				ActivateIfUsed(e)
				PromptDuration(g_radsuit[e]['useage_text'],3000)
			end
		end
	end
	
	if have_radsuit == 1 then	
		if rsswitch[e] == 0 then 
			if GetInKey() == "k" or GetInKey() == "K" and rsswitch[e] == 0 then
				if played[e] == 0 then
					PromptDuration(g_radsuit[e]['useage_text'],3000)
					PlaySound(e,0)
					played[e] = 1
				end
				rsswitch[e] = 1
			end	
		end
		if rsswitch[e] == 1 then 	
			if GetInKey() == "q" or GetInKey() == "Q" and rsswitch[e] == 1 then			
				if played[e] == 0 then
					PlaySound(e,0)
					played[e] = 1
				end
				rsswitch[e] = 0			
			end
		end
		if rsswitch[e] == 1 then
			PasteSpritePosition(radsuit,0,0)			
			LoopSound(e,1)
			if g_toxiczone == 'radiation' then SetPlayerHealth(currenthealth[e]) end
			g_ppequipment = 1
			played[e] = 0
		end
		if rsswitch[e] == 0 then			
			StopSound(e,1)			
			PasteSpritePosition(radsuit,1000,1000)
			g_ppequipment = 0
			played[e] = 0
			currenthealth[e] = g_PlayerHealth
		end
		if g_PlayerHealth <=0 then have_radsuit = 0 end		
	end
end
