-- Scroller_Control v7 by Necrym59
-- DESCRIPTION: Changes the object texture when activated by another object.
-- DESCRIPTION: Default texture is [IMAGEFILE1$=""]
-- DESCRIPTION: Default [#EMISSIVE_STRENGTH=300.0]
-- DESCRIPTION: Texture [#SCROLL_AMOUNT_U=0.0]
-- DESCRIPTION: Texture [#SCROLL_AMOUNT_V=0.0]
-- DESCRIPTION: Control [@SCROLL_DIRECTION=1(1=Positive, 2=Negative)]
-- DESCRIPTION: Control [#STARTUP_TIME=0.0]
-- DESCRIPTION: Control [#RUNNING_TIME=0.0]
-- DESCRIPTION: Control [#STOPPING_TIME=0.0]
-- DESCRIPTION: [@TELEPORTATION=1(1=Off, 2=On)] to IfUsed destination
-- DESCRIPTION: [@SHUDDERS=1(1=Off, 2=At Start, 3=At End, 4=Start+End)]
-- DESCRIPTION: [@ALWAYS_ON=2(1=Yes, 2=No)]
-- DESCRIPTION: [TELEPORT_EXIT_ANGLE=1(1,360))] Player exit angle upon teleport
-- DESCRIPTION: <Sound0> for Startup
-- DESCRIPTION: <Sound1> for Running
-- DESCRIPTION: <Sound2> for Stopping

local scroller = {}
local imagefile1 = {}
local emissive_strength = {}
local scroll_amount_u = {}
local scroll_amount_v = {}
local scroll_direction	= {}
local startup_time = {}
local running_time = {}
local stopping_time = {}
local teleportation = {}
local shudders = {}
local texture_offset_u = {}
local texture_offset_v = {}
local always_on = {}
local teleport_exit_angle = {}

local imagefile1id = {}

local current_time = {}
local status = {}
local doonce = {}

function scroller_control_properties(e,imagefile1, emissive_strength, scroll_amount_u,scroll_amount_v, scroll_direction, startup_time, running_time, stopping_time, teleportation, shudders, always_on, teleport_exit_angle)
	scroller[e] = g_Entity[e]
	scroller[e].imagefile1 = imagefile1
	scroller[e].emissive_strength = emissive_strength
	scroller[e].scroll_amount_u = scroll_amount_u
	scroller[e].scroll_amount_v = scroll_amount_v
	scroller[e].scroll_direction = scroll_direction
	scroller[e].startup_time = startup_time
	scroller[e].running_time = running_time
	scroller[e].stopping_time = stopping_time
	scroller[e].teleportation = teleportation
	scroller[e].shudders = shudders
	scroller[e].texture_offset_u = 0
	scroller[e].texture_offset_v = 0
	scroller[e].always_on = always_on	
	scroller[e].teleport_exit_angle = teleport_exit_angle or 1
	scroller[e].imagefile1id = LoadImage(imagefile1)
	if string.len(imagefile1)>0 then
		SetEntityTexture(e,scroller[e].imagefile1id)
		SetEntityEmissiveStrength(e,scroller[e].emissive_strength)
	end
end

function scroller_control_init_name(e,name)
	scroller[e] = g_Entity[e]
	scroller[e].imagefile1 = ""
	scroller[e].emissive_strength = 300
	scroller[e].scroll_amount_u = 0
	scroller[e].scroll_amount_v = 0
	scroller[e].scroll_direction = 1
	scroller[e].startup_time = 0
	scroller[e].running_time = 0
	scroller[e].stopping_time = 0
	scroller[e].teleportation = 1
	scroller[e].shudders = 1
	scroller[e].texture_offset_u = 0
	scroller[e].texture_offset_v = 0
	scroller[e].always_on = 1
	scroller[e].teleport_exit_angle = 1	
	scroller[e].imagefile1id = 0
	current_time[e] = 0
	status[e] = "init"
	doonce[e] = 0
end

function scroller_control_main(e)
	scroller[e] = g_Entity[e]

	if status[e] == "init" then
		if scroller[e].always_on == 1 then SetActivated(e,1) end
		if scroller[e].always_on == 2 then SetActivated(e,0) end
		SetEntityTextureScale(e,1,1)
		SetEntityTextureOffset(e,0,0)
		status[e] = "startup"
	end

	if g_Entity[e]['activated'] == 1 then

		if status[e] == "startup" then
			if doonce[e] == 0 then
				PlaySound(e,0)
				LoopSound(e,1)
				doonce[e] = 1
				if scroller[e].shudders == 2 or scroller[e].shudders == 4 then
					GamePlayerControlAddShakeTrauma(25.0)
					GamePlayerControlAddShakePeriod(47.0)
					GamePlayerControlAddShakeFade  (0.14)
					GamePlayerControlSetShakeTrauma(0)
					GamePlayerControlSetShakePeriod(0)
				end
			end
			if current_time[e] < scroller[e].scroll_amount_u or current_time[e] < scroller[e].scroll_amount_v then
				current_time[e] = current_time[e] + scroller[e].startup_time /100
				SetEntityTextureScale(e,1,1)
				if scroller[e].scroll_direction == 1 then
					if scroller[e].scroll_amount_u > 0 then scroller[e].texture_offset_u = math.fmod(scroller[e].texture_offset_u + current_time[e],1) end
					if scroller[e].scroll_amount_v > 0 then scroller[e].texture_offset_v = math.fmod(scroller[e].texture_offset_v + current_time[e],1) end
				end
				if scroller[e].scroll_direction == 2 then
					if scroller[e].scroll_amount_u > 0 then scroller[e].texture_offset_u = math.fmod(scroller[e].texture_offset_u - current_time[e],1) end
					if scroller[e].scroll_amount_v > 0 then scroller[e].texture_offset_v = math.fmod(scroller[e].texture_offset_v - current_time[e],1) end
				end
				SetEntityTextureOffset(e,scroller[e].texture_offset_u,scroller[e].texture_offset_v)
			end
			if scroller[e].scroll_amount_u > 0 and current_time[e] >= scroller[e].scroll_amount_u then
				current_time[e] = 0
				doonce[e] = 0
				status[e] = "running"
			end
			if scroller[e].scroll_amount_v > 0 and current_time[e] >= scroller[e].scroll_amount_v then
				current_time[e] = 0
				doonce[e] = 0
				status[e] = "running"
			end
		end

		if status[e] == "running" then
			if doonce[e] == 0 then
				LoopSound(e,1)
				doonce[e] = 1
			end
			if current_time[e] < scroller[e].running_time then
				current_time[e] = current_time[e] + scroller[e].startup_time /100
				SetEntityTextureScale(e,1,1)
				if scroller[e].scroll_direction == 1 then
					scroller[e].texture_offset_u = math.fmod(scroller[e].texture_offset_u + scroller[e].scroll_amount_u,1)
					scroller[e].texture_offset_v = math.fmod(scroller[e].texture_offset_v + scroller[e].scroll_amount_v,1)
				end
				if scroller[e].scroll_direction == 2 then
					scroller[e].texture_offset_u = math.fmod(scroller[e].texture_offset_u - scroller[e].scroll_amount_u,1)
					scroller[e].texture_offset_v = math.fmod(scroller[e].texture_offset_v - scroller[e].scroll_amount_v,1)
				end
				SetEntityTextureOffset(e,scroller[e].texture_offset_u,scroller[e].texture_offset_v)
			end
			if current_time[e] >= scroller[e].running_time then
				if scroller[e].scroll_amount_u > 0 and current_time[e] >= scroller[e].scroll_amount_u then
					current_time[e] = scroller[e].scroll_amount_u
					doonce[e] = 0
					status[e] = "stopping"
				end
				if scroller[e].scroll_amount_v > 0 and current_time[e] >= scroller[e].scroll_amount_v then
					current_time[e] = scroller[e].scroll_amount_v
					doonce[e] = 0
					status[e] = "stopping"
				end
			end
		end

		if status[e] == "stopping" then
			if doonce[e] == 0 then
				StopSound(e,1)
				PlaySound(e,2)
				doonce[e] = 1
				if scroller[e].shudders == 3 or scroller[e].shudders == 4 then
					GamePlayerControlAddShakeTrauma(25.0)
					GamePlayerControlAddShakePeriod(47.0)
					GamePlayerControlAddShakeFade  (0.14)
					GamePlayerControlSetShakeTrauma(0)
					GamePlayerControlSetShakePeriod(0)
				end
			end
			if current_time[e] > 0 then
				current_time[e] = current_time[e] - scroller[e].stopping_time /100
				SetEntityTextureScale(e,1,1)
				if scroller[e].scroll_direction == 1 then
					if scroller[e].scroll_amount_u > 0 then scroller[e].texture_offset_u = math.fmod(scroller[e].texture_offset_u + current_time[e],1) end
					if scroller[e].scroll_amount_v > 0 then scroller[e].texture_offset_v = math.fmod(scroller[e].texture_offset_v + current_time[e],1) end
				end
				if scroller[e].scroll_direction == 2 then
					if scroller[e].scroll_amount_u > 0 then scroller[e].texture_offset_u = math.fmod(scroller[e].texture_offset_u - current_time[e],1) end
					if scroller[e].scroll_amount_v > 0 then scroller[e].texture_offset_v = math.fmod(scroller[e].texture_offset_v - current_time[e],1) end
				end
				SetEntityTextureOffset(e,scroller[e].texture_offset_u,scroller[e].texture_offset_v)
			end
			if current_time[e] <= 0 then
				doonce[e] = 0
				current_time[e] = 0
				StopSound(e,0)
				StopSound(e,1)
				StopSound(e,2)
				status[e] = "init"
				if scroller[e].teleportation == 2 then
					TransportToIfUsed(e)
					SetGamePlayerControlFinalCameraAngley(scroller[e].teleport_exit_angle)
				end
				SetActivated(e,0)
			end
		end
	end
end