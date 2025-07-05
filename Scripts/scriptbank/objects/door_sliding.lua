-- Door Sliding v35 - Necrym59 and Lee
-- DESCRIPTION: Open and close a sliding door. 
-- DESCRIPTION: [MOVE_ANGLE=0(0,360)] 
-- DESCRIPTION: [MOVE_DISTANCE=90] 
-- DESCRIPTION: [MOVE_DELAY=90] milliseconds
-- DESCRIPTION: [MOVE_PERIOD=1500] milliseconds.
-- DESCRIPTION: [LockedText$="Door is locked. Find a way to open it"] 
-- DESCRIPTION: [!IsUnlocked=1]
-- DESCRIPTION: [UnLockedText$="E to use door"] 
-- DESCRIPTION: [@DOOR_TYPE=2(1=Auto, 2=Manual, 3=Switched)]
-- DESCRIPTION: [@DOOR_STYLE=1(1=None, 2=DelayedClose, 3=DelayedClose+Lock, 4=Open+Lock)]
-- DESCRIPTION: [DOOR_RANGE=100(0,500)]
-- DESCRIPTION: [CLOSE_DELAY#=5.0] seconds
-- DESCRIPTION: [@SEND_TRIGGER=4(1=On Open, 2=On Close, 3=On Open+Close, 4=None)] to trigger linked or IfUsed entities
-- DESCRIPTION: [@PROMPT_DISPLAY=2(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\hand.png"]
-- DESCRIPTION: <Sound0> When door opening
-- DESCRIPTION: <Sound1> When the door is closing
-- DESCRIPTION: <Sound2> When the door is locking

local Q = require "scriptbank\\quatlib"
local U = require "scriptbank\\utillib"
local V = require "scriptbank\\vectlib"
local NAVMESH = require "scriptbank\\navmeshlib"
local module_misclib = require "scriptbank\\module_misclib"
g_tEnt = {}

local rad = math.rad
local deg = math.deg
local modf = math.modf

local defaultDoorType     		= 'Manual'
local defaultDoorStyle	  		= 1
local defaultDoorRange    		= 100
local defaultAngle        		= 0
local defaultDistance     		= 90
local defaultDoorDelay	  		= 200
local defaultPeriod       		= 1500
local defaultCloseDelay   		= 5000
local defaultLockedText   		= "Door is locked. Find a way to open it"
local defaultIsUnlocked 		= 1
local defaultUnLockedText		= "E to use door"
local defaultsend_trigger	= 4
local prompt_display			= 1
local item_highlight			= 0
local highlight_icon 			= "imagebank\\icons\\hand.png"
local played					= {}
local tEnt 						= {}
local selectobj 				= {}
local status 					= {}
local hl_icon					= {}
local hl_imgwidth				= {}
local hl_imgheight				= {}

g_door_sliding = {}

local doorTypes = { 'Auto', 'Manual', 'Switched' }

function door_sliding_properties( e, move_angle, move_distance, move_delay, move_period, lockedtext, IsUnlocked, unlockedtext, door_type, door_style, door_range, closedelay, send_trigger, prompt_display, item_highlight, highlight_icon_imagefile)
	local door = g_door_sliding[ e ]
	if door == nil then return end
	if door_type ~= nil then
		door.door_type = doorTypes[ door_type ]    
	end
	door.door_style = door_style or defaultDoorStyle
	door.door_range = door_range or defaultDoorRange
	door.move_angle = move_angle or defaultAngle
	door.move_distance = move_distance or defaultDistance
	door.move_delay = move_delay or defaultDoorDelay
	door.move_period = move_period or defaultPeriod
	door.send_trigger = send_trigger or defaultsend_trigger
	door.prompt_display = prompt_display
	door.item_highlight = item_highlight
	door.highlight_icon = highlight_icon or highlight_icon_imagefile	
	if closedelay ~= nil then
		door.closedelay = closedelay * 1000
	end
	if IsUnlocked ~= nil then
		door.IsUnlocked = IsUnlocked == 1
	end
	if lockedtext ~= nil then
		door.lockedtext = lockedtext
	end
	if unlockedtext ~= nil then
		door.unlockedtext = unlockedtext
	end
	if door.door_type ~= 'Auto' and door.door_type ~= 'Manual' and door.door_type ~= 'Switched' then
		-- this can happen if older level using older script with deranged params
		door.door_type = 'Auto'
	end
end 

function door_sliding_init( e )
	g_door_sliding[ e ] = { mode         	= 'init',
	                        raiseoffset  	= 0,
							move_delay   	= defaultDoorDelay,
							move_angle   	= defaultAngle,
							move_distance   = defaultDistance,
							move_period     = defaultPeriod,
							lockedtext   	= defaultLockedText,							
							IsUnlocked		= defaultIsUnlocked,
							unlockedtext	= defaultUnLockedText,							
							door_type    	= defaultDoorType,
							door_style    	= defaultDoorStyle,							
							door_range   	= defaultDoorRange,
							closedelay		= defaultCloseDelay,
							send_trigger			= defaultsend_trigger,
							prompt_display	= 1,
							item_highlight	= 0,
							highlight_icon	= "imagebank\\icons\\hand.png",
							blocking        = 1,
							originalx		= -1,
							originaly		= -1,
							originalz		= -1
					      }
	played[e] = 0
	tEnt[e] = 0
	g_tEnt = 0	
	selectobj[e] = 0
	status[e] = "init"
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0		
	SetEntityAlwaysActive(e,1)
	------------------------------------------------------
	local door = g_door_sliding[ e ]
	door.obj = g_Entity[ e ].obj
	local x, y, z, xa, ya, za = GetObjectPosAng( door.obj )
	door.pos = V.Create( x, y, z )
	door.vec = V.FromEuler( xa, ya, za, true )
	door.perf  = door.move_distance / door.move_period
	local v = V.Add( door.pos, V.Mul( door.vec, door.raiseoffset ) )
end
	 
local function LookingAt( e )
	return ( g_PlayerHealth > 0 and
			 GetPlrLookingAt( e ) == 1 )
end

local function PositionDoor( e, door )
	local v = V.Add( door.pos, V.Mul( door.vec, door.raiseoffset ) )
	CollisionOff(e)
	PositionObject( door.obj, v.x, v.y, v.z )
	CollisionOn(e)
end

local lastTime  = 0
local timeDiff  = 1
local timeSlice = 1000 / 60

function door_sliding_main(e)	
	local door = g_door_sliding[ e ]
	if door == nil then return end
	if status[e] == "init" then
		if door.item_highlight == 3 and door.highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(door.highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(door.highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(door.highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end
		status[e] = "endinit"
	end		
	
	local timeNow = g_Time
	if timeNow > lastTime then
		timeDiff = ( timeNow - lastTime ) / timeSlice
		lastTime = timeNow
	end	
	
	local Ent = g_Entity[ e ]
	
	if door.originalx == -1 then
		door.originalx = g_Entity[e]['x']
		door.originaly = g_Entity[e]['y']
		door.originalz = g_Entity[e]['z']
		return
	end

	-- only having a key can unlock a door
	if g_Entity[e]['haskey'] == 1 then 
		door.IsUnlocked = true
	end
	
	-- if was spawned at start, and activated here, reset activation so initial activation was just to unlock the door
	if door.IsUnlocked == false and door.door_type == 'Manual' then
		if GetEntitySpawnAtStart(e) == 1 and Ent.activated == 1 then
			door.IsUnlocked = false
			Ent.activated = 0
			SetActivated(e,0)
		end		
	end
	if door.IsUnlocked == true and door.door_type == 'Manual' then
		if GetEntitySpawnAtStart(e) == 1 and Ent.activated == 1 then
			door.IsUnlocked = true
			Ent.activated = 0
			SetActivated(e,0)
		end		
	end
	if door.IsUnlocked == true and door.door_type == 'Auto' then
		if GetEntitySpawnAtStart(e) == 1 and Ent.activated == 1 then
			door.IsUnlocked = true
			Ent.activated = 0
			SetActivated(e,0)
		end		
	end
	if door.IsUnlocked == true and door.door_type == 'Switched' then  -- Switch only mode
		if GetEntitySpawnAtStart(e) == 1 and Ent.activated == 1 then
			door.IsUnlocked = true
			Ent.activated = 0
			SetActivated(e,1)
		end
	end

	if door.mode == 'init' then
		door.obj = g_Entity[ e ].obj		
		local x, y, z, xa, ya, za = GetObjectPosAng( door.obj )		
		door.quat  = Q.FromEuler( rad( xa ), rad( ya ), rad( za ) )
		local quat = Q.Mul( door.quat, Q.FromEuler( 0, 0, rad( door.move_angle ) ) )
		xa, ya, za = Q.ToEuler( quat )
		door.pos   = V.Create( x, y, z )
		door.perf  = door.move_distance / door.move_period
		door.vec   = V.FromEuler( xa, ya, za, true )
		-- always start in closed position (and allow activation process through state machine later on)
		door.mode = 'closed'
		door.blocking = 1
		-- if was not spawned at start, and activated here, reset activation so initial activation was just to show the door
		if GetEntitySpawnAtStart(e) ~= 1 then
			Ent.activated = 0
			SetActivated(e,0)
		end		
	elseif		
	   door.mode == 'closed' then
		if door.door_type == 'Switched' then
			if U.PlayerCloserThanPos(door.pos.x,door.pos.y,door.pos.z,door.door_range) and door.IsUnlocked == false then
				if door.prompt_display == 1 then TextCenterOnX(50,55,1,door.lockedtext) end
				if door.prompt_display == 2 then Prompt(door.lockedtext) end
			end	
			if U.PlayerCloserThanPos(door.pos.x,door.pos.y,door.pos.z,door.door_range) and door.IsUnlocked == true then
				if door.prompt_display == 1 then TextCenterOnX(50,55,1,door.lockedtext) end
				if door.prompt_display == 2 then Prompt(door.lockedtext) end
			end
			if g_Entity[e]['activated'] == 1 then
				if door.IsUnlocked == true then					
					SetEntityActivated(e,1)
					door.mode     = 'delay'
					door.nextMode = 'opening'
					door.time     =  g_Time + door.move_delay
					PlaySound(e,0 )
				else
					if door.prompt_display == 1 then PromptLocal(e,door.lockedtext) end
					if door.prompt_display == 2 then Prompt(door.lockedtext) end
				end	
				if door.send_trigger == 1 or door.send_trigger == 3 then
					PerformLogicConnections(e)
					ActivateIfUsed(e)
				end
			end	
		end	
		if Ent.activated == 0 then
			if U.PlayerCloserThanPos( door.pos.x, door.pos.y, door.pos.z, door.door_range ) then				
				if door.door_type == 'Auto' then
					if door.IsUnlocked == true then
						SetEntityActivated(e,1)
						door.mode     = 'delay'
						door.nextMode = 'opening'
						door.time     =  g_Time + door.move_delay
						PlaySound(e,0)
					else
						if door.prompt_display == 1 then PromptLocal(e,door.lockedtext) end
						if door.prompt_display == 2 then Prompt(door.lockedtext) end
					end	
					if door.send_trigger == 1 or door.send_trigger == 3 then
						PerformLogicConnections(e)
						ActivateIfUsed(e)
					end
				end				
				if door.door_type == 'Manual' and LookingAt(e) then
					--pinpoint select object--
					module_misclib.pinpoint(e,door.door_range,door.item_highlight,hl_icon[e])
					tEnt[e] = g_tEnt
					--end pinpoint select object--	
					if door.IsUnlocked == true and tEnt[e] == e then
						if door.prompt_display == 1 then TextCenterOnX(50,55,1,door.unlockedtext) end
						if door.prompt_display == 2 then Prompt(door.unlockedtext) end
						if g_KeyPressE == 1 then
							SetEntityActivated(e,1)
							door.mode     = 'delay'
							door.nextMode = 'opening'
							door.time     =  g_Time + door.move_delay
							PlaySound(e,0)
							if door.send_trigger == 1 or door.send_trigger == 3 then
								PerformLogicConnections(e)
								ActivateIfUsed(e)
							end
						end
					end	
					if door.IsUnlocked == false then
						if door.prompt_display == 1 then TextCenterOnX(50,55,1,door.lockedtext) end
						if door.prompt_display == 2 then Prompt(door.lockedtext) end
					end
				end
			end  
		end		
	elseif
	   door.mode == 'delay' then
		if g_Time >= door.time then		
			door.mode = door.nextMode
		end
		PositionDoor( e, door )		
	elseif
	   door.mode == 'opening' then	    
	   	door.raiseoffset = door.raiseoffset + door.perf * timeSlice * timeDiff
		if door.raiseoffset >= door.move_distance then
			door.raiseoffset = door.move_distance
			door.mode = 'open'
			door.blocking = 2
			if door.door_style == 2 or door.door_style == 3 then
				door.closeTime = g_Time + door.closedelay
			end
			if door.door_style == 4 then
				PositionDoor(e,door)
				SetEntityActivated(e,0)
				SetActivated(e,0)
				PlaySound(e,2)
				SwitchScript(e,"no_behavior_selected.lua")
			end
		end
		PositionDoor(e,door)
		SetEntityActivated(e,0)
		SetActivated(e,0)
	elseif  
	   door.mode == 'open' then	
		if door.door_type == 'Switched' then
			if g_Entity[e]['activated'] == 1 then
				SetEntityHasKey(e,1)
				door.IsUnlocked = true 
				if door.IsUnlocked == true then
					-- in auto mode, if unlocked or unlocked, activate to slide door from external send_trigger
					SetEntityActivated(e,1)
					door.mode     = 'delay'
					door.nextMode = 'closing'
					door.closeTime = g_Time + 1
					PlaySound(e,0 )
				end	
			end	
		end	   
		if door.door_type == 'Manual' and LookingAt(e) and U.PlayerCloserThanPos(door.pos.x,door.pos.y,door.pos.z,door.door_range) and door.door_style == 1 then
			if door.IsUnlocked == false then
				-- special case where door can be opened by other means, so break the lock
				door.IsUnlocked = true
				g_Entity[e]['haskey'] = 1
				SetEntityHasKey(e,1)				
			end			
			if door.IsUnlocked == true then
				if door.prompt_display == 1 then TextCenterOnX(50,52,1,door.unlockedtext) end
				if door.prompt_display == 2 then Prompt(door.unlockedtext) end			
				if g_KeyPressE == 1 then
					SetEntityActivated(e,1)
					door.mode     = 'delay'			
					door.nextMode = 'closing'
					door.time     =  g_Time + door.move_delay
					if door.door_style == 1 then
						door.closeTime = g_Time + 1
					end				
				end
			end
		end
		if door.door_type == 'Auto' then
			door.mode     = 'delay'			
			door.nextMode = 'closing'
			door.time     =  g_Time + door.move_delay
			if door.door_style == 1 then
				door.closeTime = g_Time + 1
			end			
			if door.door_style == 2 or door.door_style == 3 then
				door.closeTime = g_Time + door.closedelay
			end
		end		
		if door.door_style == 2 or door.door_style == 3 and g_Time >= door.closeTime then
			door.mode     = 'delay'			
			door.nextMode = 'closing'
			door.time     =  g_Time + door.move_delay
		end
		door.raiseoffset = door.move_distance 
		PositionDoor(e,door)		
	elseif
	   door.mode == 'closing' then		
		if g_Time >= door.closeTime then 
			if played[e] == 0 then
				PlaySound(e,1)
				played[e] = 1
			end	
			door.raiseoffset = door.raiseoffset - door.perf * timeSlice * timeDiff			
			if door.raiseoffset <= 0 then
				door.raiseoffset = 0
				door.mode = 'closed'
				door.blocking = 1
				if door.send_trigger == 2 or door.send_trigger == 3 then
					PerformLogicConnections(e)
					ActivateIfUsed(e)
				end
				if door.door_style == 3 then
					PlaySound(e,2)
					door.IsUnlocked = false					
				end
				played[e] = 0
			end
			PositionDoor(e,door)		
			SetEntityActivated(e,0)
			SetActivated(e,0)
		end
	end
	-- navmesh blocker system
	door.blocking = NAVMESH.HandleBlocker(e,door.blocking,door.originalx,door.originaly,door.originalz)	
	-- restore logic
	if g_EntityExtra[e]['restoremenow'] ~= nil then
		if g_EntityExtra[e]['restoremenow'] == 1 then
			g_EntityExtra[e]['restoremenow'] = 0
			if door.mode == 'closed' then
				door.blocking = 1
			end
		end
	end	
end