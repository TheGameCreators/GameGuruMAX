-- One way Door v6 by Necrym59 and Lee
-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- DESCRIPTION: Rotates a non-animating door when player interacts with it. When door is opened, will close after a few seconds delay.
-- DESCRIPTION: Can be locked permanently after closing by activation via a switch or zone.
-- DESCRIPTION: [USE_RANGE=80(1,150)]
-- DESCRIPTION: [USE_PROMPT$="Press E to open door"]
-- DESCRIPTION: [CLOSE_DELAY=2(0,30)]
-- DESCRIPTION: [CLOSE_SPEED=2(1,10)]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\hand.png"]
-- DESCRIPTION: <Sound0> When door is opened.
-- DESCRIPTION: <Sound1> When door is closing.

local module_misclib = require "scriptbank\\module_misclib"
local Q = require "scriptbank\\quatlib"
local U = require "scriptbank\\utillib"
local NAVMESH = require "scriptbank\\navmeshlib"
g_tEnt = {}

local deg = math.deg
local rad = math.rad

local doors			= {}
local names			= {}
local keyPressed	= false
local oneway_door	= {}
local close_delay	= {}
local close_speed	= {}
local tEnt			= {}
local selectobj		= {}
local status		= {}
local hl_icon		= {}
local hl_imgwidth	= {}
local hl_imgheight	= {}
local timeLastFrame = nil
local timeDiff      = 1
local controlEnt    = nil
local beenopened	= {}

function oneway_door_properties(e, use_range, use_prompt, close_delay, close_speed, prompt_display, item_highlight, highlight_icon_imagefile)
	oneway_door[e].use_range = use_range
	oneway_door[e].use_prompt = use_prompt
	oneway_door[e].close_delay = close_delay
	oneway_door[e].close_speed = close_speed	
	oneway_door[e].prompt_display = prompt_display
	oneway_door[e].item_highlight = item_highlight or 0
	oneway_door[e].highlight_icon = highlight_icon_imagefile	
end

function oneway_door_init_name(e,name)
	oneway_door[e] = {}
	oneway_door[e].use_range = 80
	oneway_door[e].use_prompt = "Press E to open door"
	oneway_door[e].close_delay = 3
	oneway_door[e].close_speed = 1	
	oneway_door[e].prompt_display = 1
	oneway_door[e].item_highlight = 0
	oneway_door[e].highlight_icon = "imagebank\\icons\\hand.png"
	names[e] = name	
	
	beenopened[e] = 0
	status[e] = "init"
	tEnt[e] = 0
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0	
	SetEntityAlwaysActive(e,1)
end

function oneway_door_main(e)

	if status[e] == "init" then
		if oneway_door[e].item_highlight == 3 and oneway_door[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(oneway_door[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(oneway_door[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(oneway_door[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end
		status[e] = "endinit"
	end	

	local door = doors[e]	
	if door == nil then
		-- initialise
		local Ent = g_Entity[e]
		local _, _, _, ax, ay, az = GetObjectPosAng(Ent.obj)
		doors[e] = {obj = Ent.obj, timer = math.huge, state = 'Closed', angle = 0, quat = Q.FromEuler(rad(ax),rad(ay),rad(az)), blocking = 1}
		doors[e].originalx   = -1
		doors[e].originaly   = -1
		doors[e].originalz   = -1
		return
	end
	
	if doors[e].originalx == -1 then
	 doors[e].originalx = g_Entity[e]['x']
	 doors[e].originaly = g_Entity[e]['y']
	 doors[e].originalz = g_Entity[e]['z']
	 return
	end

	if controlEnt == nil then controlEnt = e end	
	local timeThisFrame = g_Time	
	if controlEnt == e then
		if timeLastFrame == nil then 
			timeLastFrame = timeThisFrame
			timeDiff = 1
		else
			timeDiff = (timeThisFrame - timeLastFrame)/10
			timeLastFrame = timeThisFrame
		end
	end
	
	if U.PlayerLookingNear(e,oneway_door[e].use_range,oneway_door[e].use_range + 40) then
		--pinpoint select object--
		module_misclib.pinpoint(e,oneway_door[e].use_range,oneway_door[e].item_highlight,hl_icon[e])
		tEnt[e] = g_tEnt
		--end pinpoint select object--
		if tEnt[e] == e then
			if door.state == 'Locked' then
				if oneway_door[e].prompt_display == 1 then TextCenterOnX(50,55,1,"Door is Locked") end
				if oneway_door[e].prompt_display == 2 then Prompt("Door is Locked") end				
			end
			if door.state == 'Closed' then
				if oneway_door[e].prompt_display == 1 then TextCenterOnX(50,55,1,oneway_door[e].use_prompt) end
				if oneway_door[e].prompt_display == 2 then Prompt(oneway_door[e].use_prompt) end	
				if g_KeyPressE == 1 then
					if not keyPressed then
						SetAnimationFrames(1,30)
						PlayAnimation(e)
						PlaySound(e,0)
						PerformLogicConnections(e)
						door.state = 'Knob'
						door.timer = timeThisFrame + 500
						keyPressed = true				
						beenopened[e] = 1
					end
				else
					keyPressed = false
				end
			elseif
				door.state == 'Open' then
				if oneway_door[e].prompt_display == 1 then TextCenterOnX(50,55,1,"Press E to close door") end
				if oneway_door[e].prompt_display == 2 then Prompt("Press E to close door") end				
				if g_KeyPressE == 1 then
					if not keyPressed then
						door.state = 'Closing' 
						keyPressed = true										
					end
				else				
					keyPressed = false
				end
			end
		end	
	end
	
	if oneway_door[e].close_delay > 0 then
		if door.state == 'Open' and GetTimer(e) > oneway_door[e].close_delay*1000 then	
			g_KeyPressE = 1
			door.state = 'Closing'
			keyPressed = true			
		end
	end
	if door.state == 'Knob' and timeThisFrame > door.timer then
		door.state = 'Opening'
		door.timer = math.huge
	end
	
	if door.state == 'Opening' then		
		if door.angle < 90 then
			door.angle = door.angle + timeDiff
			local rotAng = door.angle
			if names[e] == 'Right' then rotAng = -rotAng end
			local rotq = Q.FromEuler(0,rad(rotAng),0)
			local ax, ay, az = Q.ToEuler(Q.Mul(door.quat,rotq))
			CollisionOff(e)
			ResetRotation(e,deg(ax),deg(ay),deg(az))
			CollisionOn(e)
			StartTimer(e)
		else
			door.state = 'Open' 
			door.blocking = 2			
		end		
	elseif
		door.state == 'Closing' then
		if door.angle > 0 then
			door.angle = door.angle - timeDiff * oneway_door[e].close_speed
			local rotAng = door.angle
			if names[e]  == 'Right' then rotAng = -rotAng end
			local rotq = Q.FromEuler(0,rad(rotAng),0)
			local ax, ay, az = Q.ToEuler(Q.Mul(door.quat,rotq))
			CollisionOff(e)
			ResetRotation(e,deg(ax),deg(ay),deg(az))
			CollisionOn(e)
		else
			PlaySound(e,1)
			door.state = 'Closed'
			door.blocking = 1
		end
	end

	if g_Entity[e].activated == 1 and door.state == 'Closed' and beenopened[e] == 1 then
		door.state = 'Locked'
	end	

	-- navmesh blocker system
	door.blocking = NAVMESH.HandleBlocker(e,door.blocking,door.originalx,door.originaly,door.originalz)
	
	-- restore logic
	if g_EntityExtra[e]['restoremenow'] ~= nil then
		if g_EntityExtra[e]['restoremenow'] == 1 then
			g_EntityExtra[e]['restoremenow'] = 0
			if door.state == 'Closed' then
				door.blocking = 1
			end
		end
	end	
end
