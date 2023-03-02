-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Climber v24:
-- DESCRIPTION: This script is attached to the climber object? Static mode = Physics on, Collision = Box. Turn ON Object Uses physics and IsImobile.
-- DESCRIPTION: Turn the default [USE_PROMPT!=1] on/off, change the [PROMPT_TEXT$="W-up S-down, Spacebar-jump off"]. Use 'E' to use as [MOUNT_KEY!=0] and the [MOUNT_PROMPT$="E to use"] Change the [@CLIMBING_SPEED=1(1=Slow, 2=Medium, 3=Quick, 4=Fast)] and the climber [GRIP_ADJUSTMENT=1(1,3)] and [@CLIMB_STYLE=2(1=With Weapon, 2=Hide Weapon)] and [EXIT_FORCE#=0.5(0.1,3.0)]
-- DESCRIPTION: Set [@EDITOR_MARKER=1(1=Hide, 2=Show)]
-- DESCRIPTION: <Sound0> - Footstep Climbing Loop
-- DESCRIPTION: <Sound1> - Exit Climb Sound

	local U = require "scriptbank\\utillib"
	local P = require "scriptbank\\physlib"

	local rad = math.rad
	local abs = math.abs

	local climbers = {}	
	local playerheight = 38	
    local playerwidth  = 30
	local playermovex  = 0
	local last_gun     = g_PlayerGunName
	
function climber_properties(e, use_prompt ,prompt_text, mount_key, mount_prompt, climbing_speed, grip_adjustment, climb_style, exit_force, editor_marker)
	climbers[e].use_prompt      = use_prompt
	climbers[e].prompt_text     = prompt_text
	climbers[e].mount_key       = mount_key
	climbers[e].mount_prompt    = mount_prompt
	climbers[e].climbing_speed  = climbing_speed  or 1
	climbers[e].grip_adjustment = grip_adjustment or 1
	climbers[e].climb_style     = climb_style     or 2
	climbers[e].exit_force      = exit_force
	climbers[e].editor_marker   = editor_marker   or 1
end 

function climber_init(e)
	climbers[e] = { state = 'init' }

	climbers[e].use_prompt      = 1
	climbers[e].prompt_text     = ""
	climbers[e].mount_key       = 0
	climbers[e].mount_prompt    = "" 
	climbers[e].climbing_speed  = 0
	climbers[e].grip_adjustment = 1
	climbers[e].climb_style     = 2
	climbers[e].exit_force      = 0.5
	climbers[e].editor_marker   = 1
	climbers[e].playerwidth     = playerwidth
end -- End init function

function climber_main(e)
	local climber = climbers[e]	
	-- Climber init ----------------------------------------------------------------
	if climber.state == 'init' then
		if climbers[e].editor_marker == 1 then Hide(e) end
		if climbers[e].editor_marker == 2 then Show(e) end
		local Ent = g_Entity[e]
		local dims = P.GetObjectDimensions( Ent.obj )
		climber.bottom = Ent.y
		climber.width = dims.w
		climber.top = climber.bottom + dims.h
		local x, y, z, xa, ya, za = GetObjectPosAng( Ent.obj )
		local xo, _, zo = U.Rotate3D( 0, 0, -playerwidth, rad( xa ), rad( ya ), rad( za ) )
		climber.x = x + xo
		climber.y = y
		climber.z = z + zo
		climber.playerwidth = climber.playerwidth + climber.grip_adjustment
		climber.state = 'idle'
		
	-- Climber idle ----------------------------------------------------------------
	elseif		
		climber.state == 'idle' then		
		if U.PlayerCloserThanPos( climber.x, g_PlayerPosY, climber.z, climber.playerwidth ) then
   
			if climber.use_prompt == 0 then 
				Prompt("")
			elseif
			   climber.use_prompt == 1 then 
				Prompt( climber.prompt_text ) 
			end
			local climbkeyscan = 17
			if climber.mount_key == 1 then 
				Prompt( climber.mount_prompt )
				climbkeyscan = 18
			end
			
			local playeraty = g_PlayerPosY - playerheight 
			
			if GetGamePlayerControlInWaterState()== 0 then
				if g_Scancode == climbkeyscan and abs( playeraty - climber.bottom ) < playerheight then	-- 'W' or alternate 'E' up					
					climber.state = 'active'
					last_gun = g_PlayerGunName
					climber.ypos  = climber.bottom 			
				elseif
					g_Scancode == 31 and abs( playeraty - climber.top ) < playerheight then	 -- 'S' down					
					climber.state = 'active'
					last_gun = g_PlayerGunName
					climber.ypos  = climber.top
				end
			end
			if GetGamePlayerControlInWaterState()>= 1 then
				climber.state = 'active'
				last_gun = g_PlayerGunName
				climber.ypos  = g_PlayerPosY
			end
		end
		
	-- Climber active -------------------------------------------------------------	
	elseif
		climber.state == 'active' then
		
		if climber.use_prompt == 1 then Prompt( climber.prompt_text ) end				
		
		SetFreezePosition( climber.x, climber.ypos + playerheight, climber.z )		
		TransportToFreezePositionOnly()
		
		local keyScan = g_Scancode
		playermovex = 0
		if keyScan == 57 then 	-- 'SPACEBAR' to jump off
			climber.state = 'idle'
			StopSound( e, 0 )
			PlaySound( e, 1 )
			ForcePlayer( g_PlayerAngY + 180, climbers[e].exit_force )
			if climber.climb_style == 2 then
				ChangePlayerWeapon( last_gun )
				SetPlayerWeapons( 1 )
			end
			return
		
		elseif
		   keyScan == 17 then		-- 'W' up
			LoopSound( e, 0 )
			if climber.climb_style == 2 then				
				SetPlayerWeapons( 0 )
			end
			if climber.ypos > climber.top then
				g_PlayerPosY = climber.top + playerheight
				climber.z = climber.z + 40				
				local ox,oy,oz = U.Rotate3D( 0, 0, 8, math.rad( g_PlayerAngX ), 
				                                      math.rad( g_PlayerAngY ), 
													  math.rad( g_PlayerAngZ ) )
				local forwardposx, forwardposy, forwardposz = g_PlayerPosX + ox, g_PlayerPosY + oy, g_PlayerPosZ + oz + 2
				-- 'forwardposx' ( / y / z ) are the values for "forwards" based on the direction the player is looking
				SetFreezePosition( forwardposx, forwardposy, forwardposz )
				TransportToFreezePositionOnly( forwardposx, forwardposy, forwardposz )				
				ForcePlayer( g_PlayerAngY, climbers[e].exit_force )	
				climber.z = climber.z - 40
				climber.state = 'top'
			else				
				climber.ypos = climber.ypos + climber.climbing_speed
			end
		
		elseif 
		   keyScan == 31 then		-- 'S' down
			LoopSound( e, 0 )
			if climber.climb_style == 2 then				
				SetPlayerWeapons( 0 )
			end
			if climber.ypos <= climber.bottom then
				if climber.climb_style == 2 then
					ChangePlayerWeapon( last_gun) 
					SetPlayerWeapons( 1 )										
				end
				climber.state = 'idle'
				StopSound( e, 0 )
			else
				climber.ypos = climber.ypos - climber.climbing_speed
			end
		elseif
			keyScan == 30 then 		--'A' across			
			climber.z = climber.z + 0.5
		elseif
			keyScan == 32 then 		--'D' across
			climber.z = climber.z - 0.5
		elseif
		   keyScan == 0 then 
			StopSound( e, 0 )
		end	
		
		
		
	-- Climber Top and Bottom ------------------------------------------------------
	elseif 
		climber.state == 'top' then
		StopSound( e, 0 )
		PlaySound( e, 1 )		
		if climber.climb_style == 2 then
			ChangePlayerWeapon( last_gun )
			SetPlayerWeapons( 1 )
		end
		climber.state = 'wait'
		climber.timer = g_Time + 1500
		
	elseif
		climber.state == 'wait' and
		g_Time > climber.timer then 
		climber.timer = math.huge
		climber.state = 'idle'
	end
end
