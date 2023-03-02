-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Crusher Script v3
-- DESCRIPTION: This script is attached to the crusher object. Physics ON, Gravity OFF, Weight & Friction = 0, IsImobile ON
-- DESCRIPTION: Change the [TRIGGER_RANGE=500(50,2000)], [MOVE_SPEED=60(1,500)], [DAMAGE_RANGE=80(1,500)], [DAMAGE_AMOUNT=50(1,500)], <Sound0> for Crusher Loop. <Sound1> for Crush Sound

local crusher = {}
local trigger_range = {}
local move_speed = {}
local damage_range = {}	

local maxheight = {}
local minheight = {}
local status = {}
local played = {} 
local startx = {}
local starty = {}
local startz = {}
	
function crusher_properties(e, trigger_range, move_speed, damage_range, damage_amount)
	crusher[e] = g_Entity[e]
	crusher[e].trigger_range = trigger_range		--when to start moving the crusher
	crusher[e].move_speed = move_speed				--crusher speed
	crusher[e].damage_range = damage_range			--range at which damage will be start to be applied
	crusher[e].damage_amount = damage_amount		--crusher damage applied
end
	
function crusher_init(e)
	crusher[e] = g_Entity[e]
	crusher[e].trigger_range = 500
	crusher[e].move_speed = 60	
	crusher[e].damage_range = 80
	crusher[e].damage_amount = 50
	minheight[e] = 0
	maxheight[e] = 0
	status[e] = "init"
	played = 0
end
 
function crusher_main(e)
	crusher[e] = g_Entity[e] 
	if status[e] == "init" then
		minheight[e] = (GetTerrainHeight(g_Entity[e]['x'],g_Entity[e]['z']))
		maxheight[e] = g_Entity[e]['y']
		status[e] = "crush"
	end
	
	if GetPlayerFlatDistance(e) < crusher[e].trigger_range then	
		LoopSound(e,0)
		if status[e] == "crush" then		
			if g_Entity[e]['y'] > minheight[e] then
				CollisionOff(e)
				MoveUp(e,-crusher[e].move_speed)
			end
			if g_Entity[e]['y'] > minheight[e] * 0.9 then
				if GetPlayerFlatDistance(e) < crusher[e].damage_range then					
					HurtPlayer(e,crusher[e].damage_amount)
				end				
			end
			if g_Entity[e]['y'] > minheight[e]+20 then				
				if GetPlayerFlatDistance(e) < crusher[e].damage_range / 2 then					
					HurtPlayer(e,crusher[e].damage_amount)				
					if played == 0 then
						PlaySound(e,1)
						played = 1
					end
				end
			end			
			if g_Entity[e]['y'] <= minheight[e] then
				status[e] = "reset"					
			end
			CollisionOn(e)			
		end	
	
		if status[e] == "reset" then
			played = 0
			if g_Entity[e]['y'] < maxheight[e] then
				CollisionOff(e)					
				MoveUp(e,crusher[e].move_speed)
			end
			if g_Entity[e]['y'] >= maxheight[e] then
				status[e] = "crush"					
			end
			CollisionOn(e)
		end
	else
		StopSound(e,0)
	end
	if g_PlayerHealth <= 0 then
		status[e] = "init"
	end
end
 
function GetPlayerFlatDistance(e)
	tPlayerDX = (g_Entity[e]['x'] - g_PlayerPosX)
	tPlayerDZ = (g_Entity[e]['z'] - g_PlayerPosZ)
	return math.sqrt(math.abs(tPlayerDX*tPlayerDX)+math.abs(tPlayerDZ*tPlayerDZ));
end