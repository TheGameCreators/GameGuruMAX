-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Binoculars v11 by Necrym59
-- DESCRIPTION: The Binocular object will give the player Binoculars? Always active ON.
-- DESCRIPTION: Set the [PICKUP_TEXT$="E to Pickup"] and [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: Set the [USEAGE_TEXT$="Hold B to use"]
-- DESCRIPTION: Set the [#MIN_ZOOM=-10(-20,1)], [MAX_ZOOM=20(1,30)], [ZOOM_SPEED=1(1,5)]
-- DESCRIPTION: Set the binocular [IMAGEFILE$="imagebank\\misc\\testimages\\binocs.png"]


g_binoculars = {}
local binoculars = {}
local pickup_text = {}
local pickup_range = {}
local useage_text = {}
local start_wheel = {}
local mod = {}
local zoom_speed = {}
local min_zoom = {}
local max_zoom = {}
local screen_image = {}
local last_gun = {}
local gunstatus = {}
local status = {}
	
function binoculars_properties(e, pickup_text, pickup_range, useage_text, min_zoom, max_zoom, zoom_speed, screen_image)
	g_binoculars[e] = g_Entity[e]
	g_binoculars[e]['pickup_text'] = pickup_text
	g_binoculars[e]['pickup_range'] = pickup_range
	g_binoculars[e]['useage_text'] = useage_text
	g_binoculars[e]['min_zoom'] = min_zoom	
	g_binoculars[e]['max_zoom'] = max_zoom
	g_binoculars[e]['zoom_speed'] = zoom_speed
	g_binoculars[e]['screen_image'] = imagefile or screen_image
end 
	
	
function binoculars_init(e)
	g_binoculars[e] = g_Entity[e]
	have_binoculars = 0
	g_binoculars[e]['pickup_text'] = "E to Pickup"
	g_binoculars[e]['pickup_range'] = 80
	g_binoculars[e]['useage_text'] = "Hold B to use"
	g_binoculars[e]['min_zoom'] = -20
	g_binoculars[e]['max_zoom'] = 60
	g_binoculars[e]['zoom_speed'] = 1
	g_binoculars[e]['screen_image'] ="imagebank\\misc\\testimages\\binocs.png"
	start_wheel = 0 
	mod = g_PlayerFOV
	fov = g_PlayerFOV
	last_gun = g_PlayerGunName
	gunstatus = 0
	status[e] = "init"
end
 
function binoculars_main(e)
	g_binoculars[e] = g_Entity[e]
	local PlayerDist = GetPlayerDistance(e)
	if status[e] == "init" then
		binoculars = CreateSprite(LoadImage(g_binoculars[e]['screen_image']))
		SetSpriteSize(binoculars,100,100)
		SetSpritePosition(binoculars,200,200)
		fov = g_PlayerFOV
		status[e] = "endinit"
	end
	if fov == nil then fov = g_PlayerFOV end 	
	
	if have_binoculars == 0 then
		local LookingAt = GetPlrLookingAtEx(e,1)
		if LookingAt == 1 and PlayerDist < g_binoculars[e]['pickup_range'] and have_binoculars == 0 then
			PromptLocalForVR(e,g_binoculars[e]['pickup_text'])
			if g_KeyPressE == 1 then
				have_binoculars = 1					
				PromptDuration(g_binoculars[e]['useage_text'],2000)
				PlaySound(e,0)
				Hide(e)
				CollisionOff(e)
				ActivateIfUsed(e)
			end
		end
	end
		
	if have_binoculars == 1 then		
		
		if g_Scancode == 48 then --Hold B Key to use
			
			if g_PlayerGunID > 0 then
				SetPlayerWeapons(0)
				gunstatus = 1
			end							
			PasteSpritePosition(binoculars,0,0)
			if g_MouseWheel < 0 then 
				mod = mod - g_binoculars[e]['zoom_speed']		
			elseif g_MouseWheel > 0 then 
				mod = mod + g_binoculars[e]['zoom_speed']	
			end	
			if mod < g_binoculars[e]['min_zoom'] then 
				mod = g_binoculars[e]['min_zoom']
			elseif mod > g_binoculars[e]['max_zoom'] then 
				mod = g_binoculars[e]['max_zoom']
			end
			SetPlayerFOV(fov-mod)
			Prompt("Magnification Factor: " ..math.ceil(mod))
		else
			if gunstatus == 1 then
				ChangePlayerWeapon(last_gun)
				SetPlayerWeapons(1)
				SetPlayerFOV(fov)
				gunstatus = 0
			end
			start_wheel = g_MouseWheel			
			if mod > 0 then SetPlayerFOV(fov) end
			if mod < 0 then	SetPlayerFOV(fov) end			
		end
		if g_Scancode == 0 then
			mod = 0
			SetPlayerFOV(fov)	
		end
	end
end