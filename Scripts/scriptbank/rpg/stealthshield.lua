-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Stealth Shield v12
-- DESCRIPTION: The Stealth shield object will give the player Stealth capability while activated.
-- DESCRIPTION: [@STYLE=1(1=Pickup, 2=Collected)]
-- DESCRIPTION: [PICKUP_RANGE=80(0,100)]
-- DESCRIPTION: [PROMPT_TEXT$="E to Pickup"]
-- DESCRIPTION: [USE_TEXT$="+ Shield On, - Shield Off"]
-- DESCRIPTION: [ON_TEXT$="Stealth Shield ON"]
-- DESCRIPTION: [OFF_TEXT$="Stealth Shield OFF"]
-- DESCRIPTION: [@MODE=1(1=Timed, 2=Random)]
-- DESCRIPTION: [DURATION=30(1,60)] seconds
-- DESCRIPTION: [DISCOVERY_RANGE=50(50,2000)]
-- DESCRIPTION: [SHIELD_RADIUS=800(50,2000)]
-- DESCRIPTION: [NO_OF_USES=1(1,3)]
-- DESCRIPTION: [ICON_IMAGEFILE$="imagebank\\misc\\testimages\\stealthshield_icon.png"]
-- DESCRIPTION: [ICON_POSITION_X=50(0,100)]
-- DESCRIPTION: [ICON_POSITION_Y=90(0,100)]

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local stealthshield 	= {}
local style 			= {}
local pickup_range		= {}
local prompt_text		= {}
local use_text			= {}
local on_text			= {}
local off_text 			= {}
local mode 				= {}
local duration			= {}
local discovery_range 	= {}
local shield_radius		= {}
local no_of_uses		= {}
local icon_size 		= {}
local icon_position_x 	= {}
local icon_position_y 	= {}

local sp_imgwidth		= {}
local sp_imgheight		= {}
local sallegiance		= {}
local shieldactive 		= {}
local status 			= {}
local shieldtime		= {}
local useage_text 		= {}
local use_count			= {}
local stealthicon 		= {}
local tEnt = {}
local selectobj = {}

function stealthshield_properties(e, style, pickup_range, prompt_text, use_text, on_text, off_text, mode, duration, discovery_range, shield_radius, no_of_uses, icon_imagefile, icon_position_x, icon_position_y)
	stealthshield[e].style = style
	stealthshield[e].pickup_range = pickup_range
	stealthshield[e].prompt_text = prompt_text
	stealthshield[e].use_text = use_text	
	stealthshield[e].on_text = on_text
	stealthshield[e].off_text = off_text
	stealthshield[e].mode = mode
	stealthshield[e].duration = duration
	stealthshield[e].discovery_range = discovery_range
	stealthshield[e].shield_radius = shield_radius
	stealthshield[e].no_of_uses = no_of_uses or 1
	stealthshield[e].icon_image = iconimage_file
	stealthshield[e].icon_position_x = icon_position_x
	stealthshield[e].icon_position_y = icon_position_y
end

function stealthshield_init(e)
	stealthshield[e] = {}
	stealthshield[e].style = 1
	stealthshield[e].pickup_range = 80
	stealthshield[e].prompt_text = "E to Pickup"
	stealthshield[e].use_text = "+ Shield On, - Shield Off"
	stealthshield[e].on_text = "Stealth Shield ON"
	stealthshield[e].off_text = "Stealth Shield OFF"
	stealthshield[e].mode = 1
	stealthshield[e].duration = 1
	stealthshield[e].discovery_range = 50
	stealthshield[e].shield_radius = 800
	stealthshield[e].no_of_uses = 1	
	stealthshield[e].icon_image = "imagebank\\misc\\testimages\\stealthshield_icon.png"
	stealthshield[e].icon_position_x = 90
	stealthshield[e].icon_position_y = 80
	status[e] = "init"
	shieldtime[e] = 0
	shieldactive[e] = 0	
	use_count[e] = 0
	tEnt[e] = 0
	g_tEnt = 0
	selectobj[e] = 0
	stealthicon = CreateSprite(LoadImage(stealthshield[e].icon_image))
	sp_imgwidth = GetImageWidth(LoadImage(stealthshield[e].icon_image))
	sp_imgheight = GetImageHeight(LoadImage(stealthshield[e].icon_image))
	SetSpriteSize(stealthicon,-1,-1)
	SetSpriteDepth(stealthicon,1)
	SetSpriteColor(stealthicon,255,255,255,3)
	SetSpriteOffset(stealthicon,sp_imgwidth/2.0, sp_imgheight/2.0)
	SetSpritePosition(stealthicon,200,200)
end

function stealthshield_main(e)

	if status[e] == "init" then
		if stealthshield[e].style == 1 then Show(e) end
		if stealthshield[e].style == 2 then
			CollisionOff(e)
			Hide(e)
		end
		if stealthshield[e].style == 1 then status[e] = "pickup" end
		if stealthshield[e].style == 2 then status[e] = "collected" end
	end

	if status[e] == "pickup" then
		local PlayerDist = GetPlayerDistance(e)
		if PlayerDist < stealthshield[e].pickup_range then
			--pinpoint select object--
			module_misclib.pinpoint(e,stealthshield[e].pickup_range,300)
			tEnt[e] = g_tEnt
			--end pinpoint select object--
			if PlayerDist < stealthshield[e].pickup_range and tEnt[e] ~= 0 and GetEntityVisibility(e) == 1 then
				PromptLocal(e,stealthshield[e].prompt_text)
				if g_KeyPressE == 1 then				
					PromptDuration(stealthshield[e].use_text,1000)
					PlaySound(e,0)
					Hide(e)
					CollisionOff(e)
					status[e] = "collected"
				end
			end	
		end
	end	

	if status[e] == "collected" then		
		if shieldactive[e] == 0 and use_count[e] < stealthshield[e].no_of_uses then
			if GetInKey() == "=" or GetInKey() == "+" then -- Key to turn On				
				PromptDuration(stealthshield[e].on_text,1000)			
				if stealthshield[e].mode == 1 then shieldtime[e] = g_Time + (stealthshield[e].duration * 1000) end
				if stealthshield[e].mode == 2 then shieldtime[e] = (g_Time + (stealthshield[e].duration * math.random(1000,3000))) end				
				shieldactive[e] = 1
				SetSpriteColor(stealthicon,255,255,255,3)
				use_count[e] = use_count[e] + 1
			end
		end
		if shieldactive[e] == 1 then
			if GetInKey() == "_" or GetInKey() == "-" then -- Key to turn Off
				PromptDuration(stealthshield[e].off_text,1000)
				shieldactive[e] = 0
				shieldtime[e] = 0
				if use_count[e] == stealthshield[e].no_of_uses then Destroy(e) end
			end
		end
		
		for a = 1, g_EntityElementMax do
			if shieldtime[e] > g_Time then shieldactive[e] = 1 end
			if a ~= nil and g_Entity[a] ~= nil then			
				sallegiance[e] = GetEntityAllegiance(a) -- get the allegiance (0-enemy, 1-ally, 2-neutral)
				if sallegiance[e] == 0 then
					Ent = g_Entity[a]
					if shieldactive[e] == 1 and GetPlayerDistance(a) < stealthshield[e].shield_radius then
						SetEntityAllegiance(a,2) -- Become Neutral
					end
					if shieldactive[e] == 1 and GetPlayerDistance(a) > stealthshield[e].shield_radius then
						SetEntityAllegiance(a,0) -- Stay Enemy
					end					
				end
				if sallegiance[e] == 2 then
					Ent = g_Entity[a]
					if shieldactive[e] == 0 and GetPlayerDistance(a) < stealthshield[e].shield_radius then
						SetEntityAllegiance(a,0) -- Become Enemy
					end					
				end			
				
				local PlayerDist = GetPlayerDistance(a)
				if PlayerDist <= stealthshield[e].discovery_range and shieldactive[e] == 1 then
					SetEntityAllegiance(a,0)
					shieldtime[e] = 0
					shieldactive[e] = 0
				end				
				if g_Time >= shieldtime[e]-5 and shieldactive[e] == 1 then
					SetEntityAllegiance(a,0)
					shieldtime[e] = 0
					shieldactive[e] = 0
					PromptDuration(stealthshield[e].off_text,1000)
				end
				if shieldactive[e] == 1 then	
					if g_Time >= shieldtime[e]-3000 then SetSpriteColor(stealthicon,255,255,255,3) end
					if g_Time >= shieldtime[e]-2000 then SetSpriteColor(stealthicon,255,255,255,2) end
					if g_Time >= shieldtime[e]-1000 then SetSpriteColor(stealthicon,255,255,255,1) end
					PasteSpritePosition(stealthicon,stealthshield[e].icon_position_x,stealthshield[e].icon_position_y)			
				end
			end		 
		end
		if shieldactive[e] == 1 then			
			local sht = math.floor(shieldtime[e]-g_Time)
			sht = math.floor(sht/1000)
			TextCenterOnX(stealthshield[e].icon_position_x,stealthshield[e].icon_position_y,2,"Shield Time: " ..sht.. " seconds")
			TextCenterOnX(stealthshield[e].icon_position_x,stealthshield[e].icon_position_y+1,2,"Uses Left: " ..stealthshield[e].no_of_uses - use_count[e])
		end		
	end
end