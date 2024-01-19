-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Break Object v2 by Necrym59
-- DESCRIPTION: Allows to break an animated or non-animated object
-- DESCRIPTION: [@BREAK_ANIMATION$=-1(0=AnimSetList)]
-- DESCRIPTION: [FADE_DELAY=2(1,10)] Seconds
-- DESCRIPTION: [!CAN_EXPLODE=0]
-- DESCRIPTION: <Sound0> Breaking sound

	local breakobject 		= {}
	local break_animation	= {}
	local fade_delay		= {}
	local can_explode		= {}	
	
	local cleanuptime		= {}
	local fade_level		= {}
	local xpos				= {}
	local ypos				= {}	
	local zpos				= {}
	local xang				= {}
	local yang				= {}
	local zang				= {}
	local status			= {}
	
function break_object_properties(e, break_animation, fade_delay, can_explode)
	breakobject[e] = g_Entity[e]
	breakobject[e].break_animation = "=" .. tostring(break_animation)
	breakobject[e].fade_delay = fade_delay
	breakobject[e].can_explode = can_explode	
end

function break_object_init(e)
	breakobject[e] = {}
	breakobject[e].break_animation = ""
	breakobject[e].fade_delay = 2
	breakobject[e].can_explode = 0
	
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
		local x,y,z,Ax,Ay,Az = GetEntityPosAng(e)
		xpos[e] = x
		ypos[e] = y
		zpos[e] = z
		xang[e] = Ax
		yang[e] = Ay
		zang[e] = Az
		SetPosition(e,xpos[e],ypos[e],zpos[e])
		SetRotation(e,xang[e],yang[e],zang[e])		
		status[e] = "sealed"
	end

	if status[e] == "sealed" then  --Sealed
		if g_Entity[e]['health'] > 100 then
			SetPosition(e,xpos[e],ypos[e],zpos[e])
			SetRotation(e,xang[e],yang[e],zang[e])
		end	
		if g_Entity[e]['health'] <= 100 then
			if breakobject[e].can_explode == 1 then SetEntityHealth(e,0) end
			SetAnimationName(e,breakobject[e].break_animation)
			PlayAnimation(e)
			PlaySound(e,0)
			ActivateIfUsed(e)
			PerformLogicConnections(e)
			status[e] = "cleanup"
			GravityOn(e)			
			if cleanuptime[e] == 0 then
				cleanuptime[e] = g_Time + (breakobject[e].fade_delay * 1000)
			end
		end
	end				

	if status[e] == "cleanup" then --Cleanup
		Show(e)
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
			end
		end
	end
end
