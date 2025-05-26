-- Destructible v3 by Lee and Necrym59
-- DESCRIPTION: This object has destruction fragment limbs that can be used to destroy an object and can also block navmesh.
-- DESCRIPTION: Set IsImmobile to YES.
-- DESCRIPTION: [SOLID_LIMB=2]
-- DESCRIPTION: [@ANIMATION_NAME$=-1(0=AnimSetList)]
-- DESCRIPTION: [ANIMATION_SPEED=50]
-- DESCRIPTION: [COLLISION_OFF_TIME=250]
-- DESCRIPTION: [!DESTROY_AT_END=1]
-- DESCRIPTION: [DESTROY_ANIM_TRIM=40]
-- DESCRIPTION: <Sound0> plays when destroyed.

local NAVMESH = require "scriptbank\\navmeshlib"
local lower = string.lower
local destructible = {}
local solid_limb = {}
local animation_name = {}
local animation_speed = {}
local collision_off_time = {}
local destroy_at_end = {}
local destroy_anim_trim = {}

local status = {}
local blocking = {}

function destructible_properties(e, solid_limb, animation_name, animation_speed, collision_off_time, destroy_at_end, destroy_anim_trim)
	destructible[e].solid_limb = solid_limb
	destructible[e].animation_name = "=" .. tostring(animation_name)
	destructible[e].animation_speed = animation_speed
	destructible[e].collision_off_time = collision_off_time
	destructible[e].destroy_at_end = destroy_at_end
	destructible[e].destroy_anim_trim = destroy_anim_trim
end

function destructible_init(e)
	destructible[e] = {}
	destructible[e].solid_limb = 2
	destructible[e].animation_name = ""
	destructible[e].animation_speed = 50
	destructible[e].collision_off_time = 250
	destructible[e].destroy_at_end = 1
	destructible[e].destroy_anim_trim = 40
	destructible[e].status = "init"
	destructible[e].blocking = 1
end

function destructible_main(e)
	if destructible[e].status == "init" then		
		StopAnimation(e)
		HideLimbsExcept(e,destructible[e].solid_limb)
		ShowLimb(e,destructible[e].solid_limb)
		destructible[e].status = "alive"
	end
	if destructible[e].status == "alive" then
		if g_Entity[e]['activated'] == 1 then
			destructible[e].status = "destruct"
		end
		if g_Entity[e]['health'] <= 0 then
			destructible[e].status = "destruct"
		end
	end
	if destructible[e].status == "destruct" then
		PlaySound(e,0)
		PerformLogicConnections(e)
		SetAnimationName(e,destructible[e].animation_name)
		SetAnimationSpeed(e,destructible[e].animation_speed/25.0)
		PlayAnimation(e)
		SetAnimationFrame(e,0)
		ShowLimbsExcept(e,destructible[e].solid_limb)
		HideLimb(e,destructible[e].solid_limb)
		StartTimer(e)
		destructible[e].status = "destructed"
	end
	if destructible[e].status == "destructed" then
		if GetTimer(e) > destructible[e].collision_off_time then
			CollisionOff(e)
			destructible[e].status = "destroy"
			destructible[e].blocking = 2
		end
	end
	if destructible[e].status == "destroy" then
		if destructible[e].destroy_at_end == 1 then
			if GetObjectAnimationFinished(e,destructible[e].destroy_anim_trim) == 1 then 
				Destroy(e)
			end
		end
	end
	-- navmesh blocker system
	destructible[e].blocking = NAVMESH.HandleBlocker(e,destructible[e].blocking,g_Entity[e]['x'],g_Entity[e]['y'],g_Entity[e]['z'])	
end

function destructible_exit(e)
end
