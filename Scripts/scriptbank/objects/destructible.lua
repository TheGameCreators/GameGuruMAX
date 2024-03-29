-- Destructible v1 by Lee
-- DESCRIPTION: Object has destruction fragment limbs that can be used to destroy an object and can block navmesh, also plays a <Sound0>. Set IsImmobile to YES.
-- DESCRIPTION: [SOLID_LIMB=2]
-- DESCRIPTION: [ANIMATION_NAME$="break"]
-- DESCRIPTION: [ANIMATION_SPEED=50]
-- DESCRIPTION: [COLLISION_OFF_TIME=250]
-- DESCRIPTION: [!DESTROY_AT_END=1]
-- DESCRIPTION: [DESTROY_ANIM_TRIM=40]
local NAVMESH = require "scriptbank\\navmeshlib"

g_destructible = {}

function destructible_properties(e, solidlimb, animationname, animationspeed, collisionofftime, destroyatend, destroyanimtrim)
	g_destructible[e] = {}
	g_destructible[e].solidlimb = solidlimb
	g_destructible[e].animationname = animationname
	g_destructible[e].animationspeed = animationspeed
	g_destructible[e].collisionofftime = collisionofftime
	g_destructible[e].destroyatend = destroyatend
	g_destructible[e].destroyanimtrim = destroyanimtrim
	g_destructible[e].status = "init"
	g_destructible[e].blocking = 1
end

function destructible_main(e)
	if g_destructible[e].status == "init" then
		g_destructible[e].status = "alive"
		StopAnimation(e)
		HideLimbsExcept(e,g_destructible[e].solidlimb)
		ShowLimb(e,g_destructible[e].solidlimb)
	end
	if g_destructible[e].status == "alive" then
		if g_Entity[e]['activated'] == 1 then
			g_destructible[e].status = "break"
		end
		if g_Entity[e]['health'] <= 0 then
			g_destructible[e].status = "break"
		end
	end
	if g_destructible[e].status == "break" then
		PlaySound(e,0)
		PerformLogicConnections(e)
		SetAnimationSpeed(e,g_destructible[e].animationspeed/25.0)
		SetAnimationName(e,g_destructible[e].animationname)
		PlayAnimation(e)
		SetAnimationFrame(e,0)
		ShowLimbsExcept(e,g_destructible[e].solidlimb)
		HideLimb(e,g_destructible[e].solidlimb)
		StartTimer(e)
		g_destructible[e].status = "breaking1"
	end
	if g_destructible[e].status == "breaking1" then
		if GetTimer(e) > g_destructible[e].collisionofftime then
			CollisionOff(e)
			g_destructible[e].status = "breaking2"
			g_destructible[e].blocking = 2
		end
	end
	if g_destructible[e].status == "breaking2" then
		if g_destructible[e].destroyatend == 1 then
			if GetObjectAnimationFinished(e,g_destructible[e].destroyanimtrim) == 1 then 
				Destroy(e)
			end
		end
	end
	-- navmesh blocker system
	g_destructible[e].blocking = NAVMESH.HandleBlocker(e,g_destructible[e].blocking,g_Entity[e]['x'],g_Entity[e]['y'],g_Entity[e]['z'])	
end

function destructible_exit(e)
end
