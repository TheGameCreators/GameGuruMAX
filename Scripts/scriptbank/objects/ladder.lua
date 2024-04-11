-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Ladder v24
-- DESCRIPTION: This script is to be attached to the ladder object? Set ladder objects 
-- DESCRIPTION: Static mode = Physics on, Collision = Box. Turn ON Object Uses physics, IsImobile.
-- DESCRIPTION: Turn the default [USE_PROMPT!=1] on/off, change the [PROMPT_TEXT$="W-up S-down, Spacebar-jump off"]. 
-- DESCRIPTION: Use 'E' to use as [MOUNT_KEY!=0] and the [MOUNT_PROMPT$="E to use"] 
-- DESCRIPTION: Change the [@CLIMBING_SPEED=1(1=Slow, 2=Medium, 3=Quick, 4=Fast)] and the 
-- DESCRIPTION: ladder [GRIP_ADJUSTMENT=1(1,3)] and [@CLIMB_STYLE=2(1=With Weapon, 2=Hide Weapon)]
-- DESCRIPTION: Exit force [EXIT_FORCE#=0.5(0.1,3.0)]
-- DESCRIPTION: <Sound0> - Footstep Climbing Loop
-- DESCRIPTION: <Sound1> - Exit Climb Sound

	local U = require "scriptbank\\utillib"
	local P = require "scriptbank\\physlib"

	local rad = math.rad
	local abs = math.abs

	local ladders = {}
	
	local playerheight = 38	
    local playerwidth  = 30
	local last_gun     = g_PlayerGunName
	
function ladder_properties(e, use_prompt ,prompt_text, mount_key, mount_prompt, climbing_speed, grip_adjustment, climb_style, exit_force)
	ladders[e].use_prompt      = use_prompt
	ladders[e].prompt_text     = prompt_text
	ladders[e].mount_key       = mount_key
	ladders[e].mount_prompt    = mount_prompt
	ladders[e].climbing_speed  = climbing_speed  or 0
	ladders[e].grip_adjustment = grip_adjustment or 1
	ladders[e].climb_style     = climb_style     or 2
	ladders[e].exit_force      = exit_force		 or 0.5
end 

function ladder_init(e)
	ladders[e] = { state = 'init' }

	ladders[e].use_prompt      = 1
	ladders[e].prompt_text     = ""
	ladders[e].mount_key       = 0
	ladders[e].mount_prompt    = "" 
	ladders[e].climbing_speed  = 0
	ladders[e].grip_adjustment = 1
	ladders[e].climb_style     = 2
	ladders[e].exit_force	   = 0.5
	ladders[e].playerwidth     = playerwidth
end -- End init function

function ladder_main(e)
	local ladder = ladders[e]	
	-- Ladder init ----------------------------------------------------------------
	if ladder.state == 'init' then
		local Ent = g_Entity[e]
		local dims = P.GetObjectDimensions( Ent.obj )
		ladder.bottom = Ent.y 
		ladder.top = ladder.bottom + dims.h
		local x, y, z, xa, ya, za = GetObjectPosAng( Ent.obj )
		local xo, _, zo = U.Rotate3D( 0, 0, -playerwidth, rad( xa ), rad( ya ), rad( za ) )
		ladder.x = x + xo
		ladder.y = y
		ladder.z = z + zo
		ladder.playerwidth = ladder.playerwidth + ladder.grip_adjustment
			
		ladder.state = 'idle'
		
	-- Ladder idle ----------------------------------------------------------------
	elseif		
		ladder.state == 'idle' then		
		if U.PlayerCloserThanPos( ladder.x, g_PlayerPosY, ladder.z, ladder.playerwidth ) then
   
			if ladder.use_prompt == 0 then 
				Prompt("")
			elseif
			   ladder.use_prompt == 1 then 
				Prompt( ladder.prompt_text ) 
			end
			local climbkeyscan = 17
			if ladder.mount_key == 1 then 
				Prompt( ladder.mount_prompt )
				climbkeyscan = 18
			end
			
			local playeraty = g_PlayerPosY - playerheight 
			if GetGamePlayerControlInWaterState()== 0 then
				if g_Scancode == climbkeyscan and abs(playeraty-ladder.bottom) < playerheight then 	-- 'W' or alternate 'E' up				
					ladder.state = 'active'
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
					last_gun     = g_PlayerGunName
					ladder.ypos  = g_PlayerPosY
				end
			end			
		end
				
	-- Ladder active -------------------------------------------------------------	
	elseif
		ladder.state == 'active' then
		
		if g_PlayerHealth <= 0 then
			ladder.state = 'idle'
		end
		
		if ladder.use_prompt == 1 then Prompt( ladder.prompt_text ) end				
		
		SetFreezePosition( ladder.x, ladder.ypos + playerheight, ladder.z ) 
		TransportToFreezePositionOnly()
		
		-- Allow ladders to work whilst swimming
		LimitSwimmingVerticalMovement(0)
		
		local keyScan = g_Scancode
		
		if keyScan == 57 then 	-- 'SPACEBAR' to jump off
			ladder.state = 'idle'
			StopSound( e, 0 )
			PlaySound( e, 1 )
			ForcePlayer( g_PlayerAngY + 180, ladders[e].exit_force )
			if ladder.climb_style == 2 then
				ChangePlayerWeapon( last_gun )
				SetPlayerWeapons( 1 )
			end
			return
		
		elseif
		   keyScan == 17 then		-- 'W' up
			LoopSound( e, 0 )
			if ladder.climb_style == 2 then				
				SetPlayerWeapons( 0 )
			end
			if ladder.ypos > ladder.top then
				g_PlayerPosY = ladder.top + playerheight
				ladder.z = ladder.z + 40
				local ox,oy,oz = U.Rotate3D( 0, 0, 8, math.rad( g_PlayerAngX ), 
				                                      math.rad( g_PlayerAngY ), 
													  math.rad( g_PlayerAngZ ) )
				local forwardposx, forwardposy, forwardposz = g_PlayerPosX + ox, g_PlayerPosY + oy, g_PlayerPosZ + oz + 2
				-- 'forwardposx' ( / y / z ) are the values for "forwards" based on the direction the player is looking
				SetFreezePosition( forwardposx, forwardposy, forwardposz )
				TransportToFreezePositionOnly( forwardposx, forwardposy, forwardposz )				
				ForcePlayer( g_PlayerAngY, ladders[e].exit_force )
				ladder.z = ladder.z - 40
				ladder.state = 'top'
			else				
				ladder.ypos = ladder.ypos + ladder.climbing_speed
			end
		
		elseif 
		   keyScan == 31 then		-- 'S' down
			LoopSound( e, 0 )
			if ladder.climb_style == 2 then				
				SetPlayerWeapons( 0 )
			end
			if ladder.ypos <= ladder.bottom then
				if ladder.climb_style == 2 then
					ChangePlayerWeapon( last_gun) 
					SetPlayerWeapons( 1 )										
				end
				ladder.state = 'idle'
				StopSound( e, 0 )
			else
				ladder.ypos = ladder.ypos - ladder.climbing_speed
			end
			
		elseif
		   keyScan == 0 then 
			StopSound( e, 0 )
		end
		
	-- Ladder Top and Bottom ------------------------------------------------------
	elseif 
		ladder.state == 'top' then
		StopSound( e, 0 )
		PlaySound( e, 1 )		
		if ladder.climb_style == 2 then
			ChangePlayerWeapon( last_gun )
			SetPlayerWeapons( 1 )
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
