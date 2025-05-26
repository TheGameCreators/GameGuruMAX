-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Break Wall v1
-- DESCRIPTION: Player can break an animated wall object when within range.
-- DESCRIPTION: Attach to an object. Set Always Active ON
-- DESCRIPTION: [RANGE=100(1,500)] 
-- DESCRIPTION: [PROMPT$="Press E to Break Wall"] 
-- DESCRIPTION: [@BREAK_ANIMATION$=-1(0=AnimSetList)]
-- DESCRIPTION: [CLEANUP_DELAY=3(0,10)] Seconds
-- DESCRIPTION: <Sound0> Break sound

local break_wall 		= {}
local range 			= {}
local prompt 			= {}
local break_animation 	= {}
local cleanup_delay 	= {}

local wall_destroyed 	= {}
local cleanup			= {}
local fade_level		= {}

function break_wall_properties(e, range, prompt, break_animation, cleanup_delay)
    break_wall[e].range = range
    break_wall[e].prompt = prompt	
    break_wall[e].break_animation = "=" .. tostring(break_animation)
    break_wall[e].cleanup_delay = cleanup_delay
end

function break_wall_init(e)
    break_wall[e] = {}	
    break_wall[e].range = 100	
    break_wall[e].prompt = ""
    break_wall[e].break_animation = ""
    break_wall[e].cleanup_delay = 3
	
    wall_destroyed[e] = 0
	cleanup[e] = 0
	fade_level[e] = GetEntityBaseAlpha(e)
	SetEntityTransparency(e,1)
end

function break_wall_main(e)

    if wall_destroyed[e] == 0 and GetPlayerDistance(e) < break_wall[e].range then
		Prompt(break_wall[e].prompt)
        if g_KeyPressE == 1 then
            PlayAnimation(e,break_wall[e].break_animation)
			PlaySound(e,0) 
            wall_destroyed[e] = 1
			cleanup[e] = g_Time + (break_wall[e].cleanup_delay * 1000)
        end
    end

    if wall_destroyed[e] == 1 and GetObjectAnimationFinished(e) == 0 then
		if g_Time > cleanup[e] then
			if fade_level[e] > 0 then
				SetEntityBaseAlpha(e,fade_level[e])
				fade_level[e] = fade_level[e]-0.5
			end
			if fade_level[e] <= 0 then							
				Hide(e)        		-- Hide
				CollisionOff(e)		-- Disable collision
				Destroy(e)			-- Destroy entity
			end
		end	
    end
end