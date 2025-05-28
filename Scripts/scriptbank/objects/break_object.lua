-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Break Object v5 by Necrym59
-- DESCRIPTION: Allows to break an animated or non-animated object
-- DESCRIPTION: [@BREAK_ANIMATION$=-1(0=AnimSetList)]
-- DESCRIPTION: [FADE_DELAY=2(1,10)] Seconds
-- DESCRIPTION: [!BREAK_PARTICLE=0] Use a particle when breaking
-- DESCRIPTION: [PARTICLE_NAME$=""] Name of particle to show
-- DESCRIPTION: [!HIDE_OBJECT=0] ON = Hide object initially and show on break
-- DESCRIPTION: <Sound0> Breaking sound

local U = require "scriptbank\\utillib"
local lower = string.lower
local breakobject 			= {}
local break_animation		= {}
local fade_delay			= {}
local can_explode			= {}
local break_particle		= {}
local particle_name			= {}
local particle_no			= {}
local hide_object			= {}

local cleanuptime		= {}
local fade_level		= {}
local status			= {}
	
function break_object_properties(e, break_animation, fade_delay, break_particle, particle_name, hide_object)
	breakobject[e].break_animation = "=" .. tostring(break_animation)
	breakobject[e].fade_delay = fade_delay
	breakobject[e].break_particle = break_particle or 0
	breakobject[e].particle_name = lower(particle_name)
	breakobject[e].hide_object = hide_object or 0
end

function break_object_init(e)
	breakobject[e] = {}
	breakobject[e].break_animation = ""
	breakobject[e].fade_delay = 2
	breakobject[e].break_particle = 0
	breakobject[e].particle_name = ""
	breakobject[e].particle_no = 0
	breakobject[e].hide_object = 0
	
	status[e] = "init"
	cleanuptime[e] = 0
	StartTimer(e)
	fade_level[e] = GetEntityBaseAlpha(e)
	SetEntityTransparency(e,1)
	GravityOff(e)
	if g_Entity[e]['health'] < 100 then SetEntityHealth(e,g_Entity[e]['health']+100) end
end

function break_object_main(e)

	if status[e] == "init" then
		if breakobject[e].particle_no == 0 and breakobject[e].particle_name ~= "" then
			for p = 1, g_EntityElementMax do
				if p ~= nil and g_Entity[p] ~= nil then
					if lower(GetEntityName(p)) == lower(breakobject[e].particle_name) then
						breakobject[e].particle_no = p						
						Hide(p)
						break
					end
				end			
			end
		end
		if breakobject[e].hide_object == 1 then Hide(e) end
		status[e] = "sealed"
	end

	if status[e] == "sealed" then  --Sealed		
		if g_Entity[e]['health'] <= 100 then
			if breakobject[e].hide_object == 1 then Show(e) end
			if breakobject[e].particle_no ~= 0 then
				CollisionOff(breakobject[e].particle_no)
				ResetPosition(breakobject[e].particle_no,g_Entity[e]['x'],g_Entity[e]['y'],g_Entity[e]['z'])
				Show(breakobject[e].particle_no)
			end			
			SetAnimationName(e,breakobject[e].break_animation)
			PlayAnimation(e)
			PlaySound(e,0)
			ActivateIfUsed(e)
			PerformLogicConnections(e)
			for n = 1, g_EntityElementMax do
				if n ~= nil and g_Entity[n] ~= nil then
					local objid = g_Entity[n]['obj']
					PushObject(objid,0,0,0,0,0,0)
				end
			end
			status[e] = "cleanup"
			GravityOn(e)
			CollisionOff(e)
			if cleanuptime[e] == 0 then
				cleanuptime[e] = g_Time + (breakobject[e].fade_delay * 1000)
			end
		end
	end

	if status[e] == "cleanup" then --Cleanup
		if g_Time >= cleanuptime[e] then
			if fade_level[e] > 0 then
				SetEntityBaseAlpha(e,fade_level[e])
				fade_level[e] = fade_level[e]-1
			end							
			if fade_level[e] <= 0 then
				fade_level[e] = 0
				Hide(e)
				CollisionOff(e)
				Destroy(e)
				SetEntityBaseAlpha(e,100)
			end
			if breakobject[e].break_particle == 1 then
				if breakobject[e].particle_no > 0 then
					Hide(breakobject[e].particle_no)
					Destroy(breakobject[e].particle_no)
				end	
			end
		end		
	end
end
