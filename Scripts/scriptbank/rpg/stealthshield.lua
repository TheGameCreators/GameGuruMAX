-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Stealth Shield v16
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
-- DESCRIPTION: [@DISABLE_WEAPON=1(1=On, 2=Off)]
-- DESCRIPTION: [STEALTH_ICON_IMAGEFILE$="imagebank\\misc\\testimages\\stealthshield_icon.png"]
-- DESCRIPTION: [ICON_POSITION_X=50(0,100)]
-- DESCRIPTION: [ICON_POSITION_Y=90(0,100)]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\pickup.png"]
-- DESCRIPTION: <Sound0> - Activation Sound
-- DESCRIPTION: <Sound1> - Deactivation Sound

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
local disable_weapon 	= {}
local icon_image		= {}
local icon_size 		= {}
local icon_position_x 	= {}
local icon_position_y 	= {}
local prompt_display 	= {}
local item_highlight 	= {}
local highlight_icon	= {}

local shieldactive 		= {}
local shieldedEnt		= {}
local entrange			= {}
local status 			= {}
local hl_icon			= {}
local hl_imgwidth		= {}
local hl_imgheight		= {}
local shieldtime		= {}
local useage_text 		= {}
local use_count			= {}
local stealthicon		= {}
local sp_imgwidth		= {}
local sp_imgheight		= {}
local tableName			= {}
local tEnt 				= {}
local selectobj 		= {}
local last_gun			= {}
local doonce			= {}

function stealthshield_properties(e, style, pickup_range, prompt_text, use_text, on_text, off_text, mode, duration, discovery_range, shield_radius, no_of_uses, disable_weapon, stealth_icon_imagefile, icon_position_x, icon_position_y, prompt_display, item_highlight, highlight_icon_imagefile)
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
	stealthshield[e].disable_weapon = disable_weapon or 1	
	stealthshield[e].icon_image = stealth_icon_imagefile
	stealthshield[e].icon_position_x = icon_position_x
	stealthshield[e].icon_position_y = icon_position_y
	stealthshield[e].prompt_display = prompt_display
	stealthshield[e].item_highlight = item_highlight
	stealthshield[e].highlight_icon = highlight_icon_imagefile	
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
	stealthshield[e].disable_weapon = 1		
	stealthshield[e].icon_image = "imagebank\\misc\\testimages\\stealthshield_icon.png"
	stealthshield[e].icon_position_x = 90
	stealthshield[e].icon_position_y = 80
	stealthshield[e].prompt_display = 1
	stealthshield[e].item_highlight = 0
	stealthshield[e].highlight_icon = "imagebank\\icons\\pickup.png"

	status[e] = "init"
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0	
	shieldtime[e] = 0
	shieldactive[e] = 0
	shieldedEnt[e] = 0
	use_count[e] = 0
	tEnt[e] = 0
	g_tEnt = 0
	selectobj[e] = 0
	doonce[e] = 2
	last_gun[e] = g_PlayerGunName
	tableName[e] = "shieldlist" ..tostring(e)
	_G[tableName[e]] = {}
	stealthicon[e] = CreateSprite(LoadImage(stealthshield[e].icon_image))
	sp_imgwidth[e] = GetImageWidth(LoadImage(stealthshield[e].icon_image))
	sp_imgheight[e] = GetImageHeight(LoadImage(stealthshield[e].icon_image))
	SetSpriteSize(stealthicon[e],-1,-1)
	SetSpriteDepth(stealthicon[e],1)
	SetSpriteColor(stealthicon[e],255,255,255,3)
	SetSpriteOffset(stealthicon[e],sp_imgwidth[e]/2.0, sp_imgheight[e]/2.0)
	SetSpritePosition(stealthicon[e],200,200)
end

function stealthshield_main(e)

	if status[e] == "init" then
		if stealthshield[e].item_highlight == 3 and stealthshield[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(stealthshield[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(stealthshield[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(stealthshield[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end	
		if stealthshield[e].style == 1 then Show(e) end
		if stealthshield[e].style == 2 then
			CollisionOff(e)
			Hide(e)
		end
		if stealthshield[e].style == 1 then status[e] = "pickup" end
		if stealthshield[e].style == 2 then status[e] = "collected" end
		for n = 1, g_EntityElementMax do
			if n ~= nil and g_Entity[n] ~= nil then
				if GetEntityAllegiance(n) == 0 then
					table.insert(_G[tableName[e]],n)
				end
			end
		end
		status[e] = "pickup"
	end

	if status[e] == "pickup" then
		local PlayerDist = GetPlayerDistance(e)
		if PlayerDist < stealthshield[e].pickup_range then
			--pinpoint select object--
			module_misclib.pinpoint(e,stealthshield[e].pickup_range,stealthshield[e].item_highlight,hl_icon[e])
			tEnt[e] = g_tEnt
			--end pinpoint select object--
			if PlayerDist < stealthshield[e].pickup_range and tEnt[e] == e and GetEntityVisibility(e) == 1 then
				if stealthshield[e].prompt_display == 1 then PromptLocal(e,stealthshield[e].prompt_text) end
				if stealthshield[e].prompt_display == 2 then Prompt(stealthshield[e].prompt_text) end			
				if g_KeyPressE == 1 then
					PromptDuration(stealthshield[e].use_text,2000)
					PlaySound(e,0)
					Hide(e)
					CollisionOff(e)
					status[e] = "collected"
				end
			end
		end
	end

	if status[e] == "collected" then
		ResetPosition(e,g_PlayerPosX,g_PlayerPosY+500,g_PlayerPosZ)
		if shieldactive[e] == 0 and use_count[e] < stealthshield[e].no_of_uses then
			if GetInKey() == "=" or GetInKey() == "+" then -- Key to turn On
				PromptDuration(stealthshield[e].on_text,1000)
				PlayNon3DSound(e,0)
				if stealthshield[e].mode == 1 then shieldtime[e] = g_Time + (stealthshield[e].duration * 1000) end
				if stealthshield[e].mode == 2 then shieldtime[e] = (g_Time + (stealthshield[e].duration * math.random(1000,3000))) end
				shieldactive[e] = 1
				SetSpriteColor(stealthicon[e],255,255,255,100)
				use_count[e] = use_count[e] + 1
				if stealthshield[e].disable_weapon == 1 then 
					last_gun[e] = g_PlayerGunName
					SetPlayerWeapons(0)
				end	
			end
		end
		if shieldactive[e] == 1 then
			if GetInKey() == "_" or GetInKey() == "-" then -- Key to turn Off
				PromptDuration(stealthshield[e].off_text,1000)
				PlayNon3DSound(e,1)
				shieldactive[e] = 0
				shieldtime[e] = 0
				if use_count[e] == stealthshield[e].no_of_uses then Destroy(e) end
				if stealthshield[e].disable_weapon == 1 then 
					SetPlayerWeapons(1)
					ChangePlayerWeapon(last_gun[e])
				end
				doonce[e] = 0
			end
		end

		if shieldactive[e] == 0 then
			if doonce[e] == 0 then
				for _,v in pairs (_G[tableName[e]]) do
					if g_Entity[v] ~= nil then
						shieldedEnt[e] = v
						SetEntityAllegiance(v,0)
					end
				end
			end	
			doonce[e] = 1	
		end
		if shieldactive[e] == 1 then
			for _,v in pairs (_G[tableName[e]]) do
				if g_Entity[v] ~= nil then
					entrange[e] = math.ceil(GetFlatDistanceToPlayer(v))
					if g_Entity[v]["health"] > 0 and entrange[e] < stealthshield[e].shield_radius and entrange[e] > stealthshield[e].discovery_range then							
						shieldedEnt[e] = v
						SetEntityAllegiance(v,2)
					else
						SetEntityAllegiance(v,0)
						shieldedEnt[e] = 0
					end
				end
			end
		end		

		if g_Time >= shieldtime[e]-5 and shieldactive[e] == 1 then
			shieldactive[e] = 0
			shieldtime[e] = 0
			shieldactive[e] = 0
			doonce[e] = 0
			PromptDuration(stealthshield[e].off_text,1000)
			if stealthshield[e].disable_weapon == 1 then 
				SetPlayerWeapons(1)
				ChangePlayerWeapon(last_gun[e])
			end
		end
		if shieldactive[e] == 1 then
			if g_Time >= shieldtime[e]-3000 then SetSpriteColor(stealthicon[e],255,255,255,80) end
			if g_Time >= shieldtime[e]-2000 then SetSpriteColor(stealthicon[e],255,255,255,50) end
			if g_Time >= shieldtime[e]-1000 then SetSpriteColor(stealthicon[e],255,255,255,1) end
			PasteSpritePosition(stealthicon[e],stealthshield[e].icon_position_x,stealthshield[e].icon_position_y)
		end
		if shieldactive[e] == 1 then			
			local sht = math.floor(shieldtime[e]-g_Time)
			sht = math.floor(sht/1000)
			TextCenterOnX(stealthshield[e].icon_position_x,stealthshield[e].icon_position_y,2,"Shield Time: " ..sht.. " seconds")
			TextCenterOnX(stealthshield[e].icon_position_x,stealthshield[e].icon_position_y+1,2,"Uses Left: " ..stealthshield[e].no_of_uses - use_count[e])
		end
	end	
end

function GetFlatDistanceToPlayer(v)
	if g_Entity[v] ~= nil then
		local distDX = g_PlayerPosX - g_Entity[v]['x']
		local distDZ = g_PlayerPosZ - g_Entity[v]['z']
		return math.sqrt((distDX*distDX)+(distDZ*distDZ));
	end
end