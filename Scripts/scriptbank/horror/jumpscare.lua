-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Jump Scare v2 (created by by Smallg) modified by Necrym59
-- DESCRIPTION: Attach to an object and link to a trigger zone or switch.
-- DESCRIPTION: [APPEAR_RANDOM=1]
-- DESCRIPTION: [FADE_DELAY=650]
-- DESCRIPTION: [MOVEX_RANDOM=50]
-- DESCRIPTION: [MOVEZ_RANDOM=50] 

local jumpscare = {}
local appear_random
local fade_delay = {}	
local movex_random = {}
local movez_random = {}
local appearances = {}	
local hidden = {}
local fade_delay_init = {}  
	
function jumpscare_properties(e, appear_random, fade_delay, movex_random, movez_random)
	jumpscare[e] = g_Entity[e]
	jumpscare[e].appear_random = appear_random				--how many times entity will appear 	
	jumpscare[e].fade_delay	= fade_delay					--how long between appearances (and how long it will appear for)
	jumpscare[e].movex_random = movex_random				--how far away from starting location entity can move in x axis, set to 0 for no movement
	jumpscare[e].movez_random = movez_random				--how far away from starting location entity can move in z axis, set to 0 for no movement
end
 
function jumpscare_init(e)
	jumpscare[e] = {}
	jumpscare[e].appear_random  = 1		
	jumpscare[e].fade_delay = 650
	jumpscare[e].movex_random = 50
	jumpscare[e].movez_random = 50	
	---------------
	appearances[e] = math.random(1,jumpscare[e].appear_random)	
	hidden[e] = 1
	CollisionOff(e)
	Hide(e)
end
 
function jumpscare_main(e)
	RotateToPlayer(e)
	CollisionOff(e)
	if g_Entity[e]['activated'] == 1 then
		if GetTimer(e) > jumpscare[e].fade_delay then
			if appearances[e] > 0 then
				if hidden[e] == 1 then
					PlaySound(e,1)
					Show(e)
					RotateToPlayer(e)
					hidden[e] = 0
					appearances[e] = appearances[e] - 1
				else
					Hide(e)
					SetPosition(e,g_Entity[e]['x'] + math.random(-jumpscare[e].movex_random,jumpscare[e].movex_random),g_Entity[e]['y'],g_Entity[e]['z']+math.random(-jumpscare[e].movez_random,jumpscare[e].movez_random))
					hidden[e] = 1
				end
			else
				if hidden[e] == 0 then
					Hide(e)
				end
			end
			jumpscare[e].fade_delay = GetTimer(e) + math.random(jumpscare[e].fade_delay * 0.75,jumpscare[e].fade_delay * 1.25)
		end
	end 
end 
 
function jumpscare_exit(e) 
end