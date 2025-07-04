-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Ladder v26
-- DESCRIPTION: This script is to be attached to the ladder object? Set ladder object to Physics on, Collision=Box. IsImobile=ON
-- DESCRIPTION: [DISPLAY_PROMPTS!=1] to display on/off the prompts, change the 
-- DESCRIPTION: [PROMPT_TEXT$="W-up S-down, Spacebar-jump off"]
-- DESCRIPTION: Use 'E' to use as the [USE_MOUNT_KEY!=0] and the 
-- DESCRIPTION: [MOUNT_PROMPT$="E to use"]
-- DESCRIPTION: Set the [#CLIMBING_SPEED=0.7(0.1,3.0)]
-- DESCRIPTION: and ladder [GRIP_ADJUSTMENT=1(1,3)]
-- DESCRIPTION: Select the [@CLIMB_STYLE=2(1=With Weapon, 2=Hide Weapon)]
-- DESCRIPTION: Set the [#EXIT_FORCE=0.5(0.1,3.0)]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\hand.png"]
-- DESCRIPTION: <Sound0> - Footstep Climbing Loop
-- DESCRIPTION: <Sound1> - Exit Climb Sound

local module_misclib = require "scriptbank\\module_misclib"
local P = require "scriptbank\\physlib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local rad = math.rad
local abs = math.abs
local ladders 			= {}
local display_prompts 	= {}
local prompt_text		= {}
local use_mount_key		= {}
local mount_prompt		= {}
local climbing_speed	= {}
local grip_adjustment	= {}
local climb_style		= {}
local exit_force		= {}
local prompt_display	= {}
local item_highlight	= {}
local highlight_icon	= {}

local climbkeyscan = 17
local playerheight = 38
local playerwidth  = 30
local exittop = 0
local exitbottom = 0
local tEnt = 0
local hl_icon = 0
local hl_imgwidth = 0
local hl_imgheight = 0
local last_gun     = g_PlayerGunName

function ladder_properties(e, display_prompts ,prompt_text, use_mount_key, mount_prompt, climbing_speed, grip_adjustment, climb_style, exit_force, prompt_display, item_highlight, highlight_icon_imagefile)
	ladders[e].display_prompts = display_prompts
	ladders[e].prompt_text = prompt_text
	ladders[e].use_mount_key = use_mount_key or 0
	ladders[e].mount_prompt = mount_prompt
	ladders[e].climbing_speed = climbing_speed  or 0
	ladders[e].grip_adjustment = grip_adjustment or 1
	ladders[e].climb_style = climb_style or 2
	ladders[e].exit_force = exit_force or 0.5
	ladders[e].prompt_display = prompt_display
	ladders[e].item_highlight = item_highlight
	ladders[e].highlight_icon = highlight_icon_imagefile
end

function ladder_init(e)
	ladders[e] = { state = 'init' }
	ladders[e].display_prompts = 1
	ladders[e].prompt_text = ""
	ladders[e].use_mount_key = 0
	ladders[e].mount_prompt = ""
	ladders[e].climbing_speed = 0
	ladders[e].grip_adjustment = 1
	ladders[e].climb_style = 2
	ladders[e].exit_force = 0.5
	ladders[e].prompt_display = 1
	ladders[e].item_highlight = 0
	ladders[e].highlight_icon = "imagebank\\icons\\hand.png"	
	ladders[e].playerwidth = playerwidth
end

function ladder_main(e)
	local ladder = ladders[e]

	-- Ladder init ----------------------------------------------------------------
	if ladder.state == 'init' then
		if ladder.item_highlight == 3 and ladder.highlight_icon ~= "" then
			hl_icon = CreateSprite(LoadImage(ladder.highlight_icon))
			hl_imgwidth = GetImageWidth(LoadImage(ladder.highlight_icon))
			hl_imgheight = GetImageHeight(LoadImage(ladder.highlight_icon))
			SetSpriteSize(hl_icon,-1,-1)
			SetSpriteDepth(hl_icon,100)
			SetSpriteOffset(hl_icon,hl_imgwidth/2.0, hl_imgheight/2.0)
			SetSpritePosition(hl_icon,500,500)
		end	
		local Ent = g_Entity[e]
		local dims = P.GetObjectDimensions(Ent.obj)
		ladder.bottom = Ent.y
		ladder.top = ladder.bottom + dims.h
		local x, y, z, xa, ya, za = GetObjectPosAng(Ent.obj)
		local xo, _, zo = U.Rotate3D(0,0,-playerwidth,rad(xa),rad(ya),rad(za))
		ladder.x = x + xo
		ladder.y = y
		ladder.z = z + zo
		ladder.playerwidth = ladder.playerwidth + ladder.grip_adjustment
		ladder.state = 'idle'
	-- Ladder idle ----------------------------------------------------------------
	elseif
		ladder.state == 'idle' then		
		local PlayerDist = GetPlayerDistance(e)

		if U.PlayerCloserThanPos(ladder.x, g_PlayerPosY, ladder.z, ladder.playerwidth) or PlayerDist < ladder.playerwidth then
				--pinpoint select object--
				module_misclib.pinpoint(e,ladder.playerwidth,ladder.item_highlight,0)
				tEnt = g_tEnt
				--end pinpoint select object--

			if g_PlayerPosY < ladder.top or U.PlayerLookingAt(e,100) == true or g_PlayerPosY >= ladder.top or tEnt == e then
				if ladder.display_prompts == 0 then Prompt("") end
				if ladders[e].item_highlight == 3 then PasteSpritePosition(hl_icon,50,50) end
				if ladder.use_mount_key == 0 then
					--If at bottom not using mount key
					if ladder.display_prompts == 1 and g_PlayerPosY < ladder.top then
						if ladder.prompt_display == 1 then TextCenterOnX(50,55,1,ladder.prompt_text) end
						if ladder.prompt_display == 2 then Prompt(ladder.prompt_text) end
						climbkeyscan = 17
					end
					--If at top not using mount key
					if ladder.display_prompts == 1 and g_PlayerPosY >= ladder.top then
						if ladder.prompt_display == 1 then TextCenterOnX(50,55,1,ladder.prompt_text) end
						if ladder.prompt_display == 2 then Prompt(ladder.prompt_text) end
						climbkeyscan = 17
					end
				end	
				if ladder.use_mount_key == 1 then
					--If at bottom using mount key
					if ladder.display_prompts == 1 or ladder.display_prompts == 0 and g_PlayerPosY < ladder.top then
						if ladder.prompt_display == 1 then TextCenterOnX(50,55,1,ladder.mount_prompt) end
						if ladder.prompt_display == 2 then Prompt(ladder.mount_prompt) end
						climbkeyscan = 18
					end
					--If at top using mount key
					if ladder.display_prompts == 1 or ladder.display_prompts == 0 and g_PlayerPosY >= ladder.top then
						if ladder.prompt_display == 1 then TextCenterOnX(50,55,1,ladder.prompt_text) end
						if ladder.prompt_display == 2 then Prompt(ladder.prompt_text) end
						climbkeyscan = 17
					end					
				end	

				local playeraty = g_PlayerPosY - playerheight
				if GetGamePlayerControlInWaterState()== 0 then
					if g_Scancode == climbkeyscan and abs(playeraty-ladder.bottom) < playerheight then 	-- 'W' or alternate 'E' up
						ladder.state = 'active'
						SetEntityEmissiveStrength(tEnt,0)
						last_gun     = g_PlayerGunName
						ladder.ypos  = ladder.bottom
					elseif
						g_Scancode == 31 and abs(playeraty-ladder.top) < playerheight then				-- 'S' down
						ladder.state = 'active'
						last_gun     = g_PlayerGunName
						ladder.ypos  = ladder.top
					end
				end
				if GetGamePlayerControlInWaterState()>= 1 then
					if g_Scancode == climbkeyscan then 	-- 'W' or alternate 'E' up
						ladder.state = 'active'
						SetEntityEmissiveStrength(tEnt,0)
						last_gun     = g_PlayerGunName
						ladder.ypos  = g_PlayerPosY
					end
				end
			end
		end

	-- Ladder active -------------------------------------------------------------
	elseif
		ladder.state == 'active' then

		if g_PlayerHealth <= 0 then
			ladder.state = 'idle'
		end

		if ladder.display_prompts == 1 then
			if ladder.prompt_display == 1 then TextCenterOnX(50,55,1,ladder.prompt_text) end
			if ladder.prompt_display == 2 then Prompt(ladder.prompt_text) end
		end

		SetFreezePosition(ladder.x, ladder.ypos + playerheight, ladder.z)
		TransportToFreezePositionOnly()

		-- Allow ladders to work whilst swimming
		LimitSwimmingVerticalMovement(0)

		local keyScan = g_Scancode

		if keyScan == 57 then 	-- 'SPACEBAR' to jump off
			ladder.state = 'idle'
			StopSound(e,0)
			PlaySound(e,1)
			ForcePlayer(g_PlayerAngY + 180, ladders[e].exit_force)
			if ladder.climb_style == 2 then
				ChangePlayerWeapon(last_gun)
				SetPlayerWeapons(1)				
			end
			return

		elseif
		   keyScan == 17 then	-- 'W' up
			LoopSound(e,0)
			if ladder.climb_style == 2 then
				SetPlayerWeapons(0)
			end
			if ladder.ypos > ladder.top then
				g_PlayerPosY = ladder.top + playerheight
				ladder.z = ladder.z + 40
				local ox,oy,oz = U.Rotate3D( 0, 0, 8, math.rad( g_PlayerAngX ),
				                                      math.rad( g_PlayerAngY ),
													  math.rad( g_PlayerAngZ ) )
				local forwardposx, forwardposy, forwardposz = g_PlayerPosX + ox, g_PlayerPosY + oy, g_PlayerPosZ + oz + 2
				-- 'forwardposx' ( / y / z ) are the values for "forwards" based on the direction the player is looking
				SetFreezePosition(forwardposx, forwardposy, forwardposz)
				TransportToFreezePositionOnly(forwardposx, forwardposy, forwardposz)
				ForcePlayer(g_PlayerAngY, ladders[e].exit_force)
				ladder.z = ladder.z - 40
				ladder.state = 'top'
			else
				ladder.ypos = ladder.ypos + ladder.climbing_speed
			end

		elseif
		   keyScan == 31 then	-- 'S' down
			LoopSound(e,0)
			if ladder.climb_style == 2 then
				SetPlayerWeapons(0)
			end
			if ladder.ypos <= ladder.bottom+1 then
				if ladder.climb_style == 2 then
					ChangePlayerWeapon(last_gun)
					SetPlayerWeapons(1)
				end
				ladder.state = 'idle'
				StopSound(e,0)
			else
				ladder.ypos = ladder.ypos - ladder.climbing_speed
			end

		elseif
		   keyScan == 0 then
			StopSound(e,0)
		end

	-- Ladder Top and Bottom ------------------------------------------------------
	elseif
		ladder.state == 'top' then
		StopSound(e,0)
		PlaySound(e,1)
		if ladder.climb_style == 2 then
			ChangePlayerWeapon(last_gun)
			SetPlayerWeapons(1)
		end		
		ladder.state = 'wait'
		ladder.timer = g_Time + 1500

	elseif
		ladder.state == 'wait' and
		g_Time > ladder.timer then
		ladder.timer = math.huge
		ladder.state = 'idle'
	end
end