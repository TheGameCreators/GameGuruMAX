-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Rise and Fall v10 - by Necrym59
-- DESCRIPTION: The attached object will rise and float then be released or be thrown after specified delay.
-- DESCRIPTION: Set AlwaysActive ON then logic link to a switch or zone to activate. 
-- DESCRIPTION: [PROMPT_TEXT$="What is happening"]
-- DESCRIPTION: [RISE_HEIGHT=30(0,100)]
-- DESCRIPTION: [RISE_SPEED=5(0,50)]
-- DESCRIPTION: [@HOVER_MODE=1(1=Off, 2=On)]
-- DESCRIPTION: [HOVER_HEIGHT=5(1,50)]
-- DESCRIPTION: [ROTATE_SPEED=6(0,50)]
-- DESCRIPTION: [ROTATE_X!=0]
-- DESCRIPTION: [ROTATE_Y!=1]
-- DESCRIPTION: [ROTATE_Z!=0]
-- DESCRIPTION: [MAX_OFFSET_Z=0(0,100)]
-- DESCRIPTION: [@RELEASE_MODE=1(1=Gravity Fall, 2=Thrown Randomly, 3=Thrown at Player)]
-- DESCRIPTION: [RELEASE_FORCE=18(0,2000)]
-- DESCRIPTION: [RELEASE_DELAY=6(0,30)] seconds.
-- DESCRIPTION: [HIT_DAMAGE=0(0,100)]
-- DESCRIPTION:  <Sound0> for rising object
-- DESCRIPTION:  <Sound1> for falling object

local V = require "scriptbank\\vectlib"

g_hover_heightangle 	= {}
local risefall			= {}
local prompt_text		= {}
local rise_height		= {}
local rise_speed 		= {}
local hover_mode		= {}
local rotate_speed 		= {}
local rotate_x 			= {}
local rotate_y 			= {}
local rotate_z 			= {}
local max_offset_z		= {}
local release_mode		= {}
local release_force		= {}
local release_delay		= {}

local status			= {}
local rf_time			= {}
local offsetmove		= {}
local startheight		= {}
local startx			= {}
local startz			= {}
local endheight			= {}
local endoffset			= {}
local hitonce			= {}
local hittime			= {}
local fFinalY			= {}
local nhoverheight		= {}
local played 			= {}

function rise_and_fall_properties(e, prompt_text, rise_height, rise_speed, hover_mode, hover_height, rotate_speed, rotate_x, rotate_y, rotate_z, max_offset_z, release_mode, release_force, release_delay, hit_damage)
	risefall[e].prompt_text = prompt_text
	risefall[e].rise_height = rise_height
	risefall[e].rise_speed = rise_speed
	risefall[e].hover_mode = hover_mode
	risefall[e].hover_height = hover_height
	risefall[e].rotate_speed  = rotate_speed
	risefall[e].rotate_x = rotate_x or 0
	risefall[e].rotate_y = rotate_y or 0
	risefall[e].rotate_z = rotate_z or 0
	risefall[e].max_offset_z = max_offset_z
	risefall[e].release_mode = release_mode
	risefall[e].release_force = release_force
	risefall[e].release_delay = release_delay
	risefall[e].hit_damage = hit_damage
end

function rise_and_fall_init(e)
	risefall[e] = {}
	risefall[e].prompt_text = ""
	risefall[e].rise_height = 30
	risefall[e].rise_speed = 5
	risefall[e].hover_mode = 1
	risefall[e].hover_height = 5
	risefall[e].rotate_speed = 6
	risefall[e].rotate_x = 0
	risefall[e].rotate_y = 1
	risefall[e].rotate_z = 0
	risefall[e].max_offset_z = 0
	risefall[e].release_mode = 1
	risefall[e].release_force = 18
	risefall[e].release_delay = 6
	risefall[e].hit_damage = 1
	
	rf_time[e] = 0
	played[e] = 0
	startx[e] = 0
	startz[e] = 0
	hitonce[e] = 0
	hittime[e] = math.huge
	offsetmove[e] = 0
	endheight[e] = 0
	endoffset[e] = 0	
	fFinalY[e] = 0
	nhoverheight[e] = 0
	g_hover_heightangle[e] = 0
	status[e] = "init"
end

function rise_and_fall_main(e)

	if status[e] == "init" then
		startheight[e] = g_Entity[e]['y']
		endheight[e] = startheight[e] + risefall[e].rise_height
		if risefall[e].hover_height > endheight[e]/3 then risefall[e].hover_height = endheight[e]/3 end
		startx[e] = g_Entity[e]['x']
		startz[e] = g_Entity[e]['z']	
		status[e] = "rise"
		hittime[e] = g_Time + 1000
		StartTimer(e)
	end

	if g_Entity[e]['activated'] == 1 then	
		PlayerDist = GetPlayerDistance(e)
		if status[e] == "rise" then			
			if g_Entity[e]['y'] < endheight[e] then
				GravityOff(e)
				CollisionOff(e)				
				MoveUp(e,risefall[e].rise_speed)
				if endoffset[e] < risefall[e].max_offset_z then
					MoveBackward(e,risefall[e].rise_speed)
					endoffset[e] = endoffset[e] + 0.01
				end	
				CollisionOn(e)
				if played[e] == 0 then
					PlaySound(e,0)
					played[e] = 1
				end
			end
			if g_Entity[e]['y'] >= endheight[e] then
				if rf_time[e] == 0 then
					rf_time[e] = GetTimer(e) + (risefall[e].release_delay * 1000)
				end
				status[e] = "hover"
			end
		end
		if status[e] == "hover" then
			if GetTimer(e) <= rf_time[e] then
				CollisionOff(e)				
				if risefall[e].rotate_x == 1 then RotateX(e,risefall[e].rotate_speed) end
				if risefall[e].rotate_y == 1 then RotateY(e,risefall[e].rotate_speed) end
				if risefall[e].rotate_z == 1 then RotateZ(e,risefall[e].rotate_speed) end				
				if risefall[e].hover_mode == 1 then
					ResetPosition(e,g_Entity[e]['x'],endheight[e],g_Entity[e]['z'])
				end
				if risefall[e].hover_mode == 2 then
					nhoverheight[e] = risefall[e].hover_height
					g_hover_heightangle[e] = g_hover_heightangle[e] + (GetAnimationSpeed(e)/100.0)
					fFinalY[e] = endheight[e] - nhoverheight[e] + (math.cos(g_hover_heightangle[e])*nhoverheight[e])					
					ResetPosition(e,g_Entity[e]['x'],fFinalY[e],g_Entity[e]['z'])
				end
				local new_x = startx[e] + math.sin(offsetmove[e]) * risefall[e].max_offset_z
				local new_z = startz[e] - math.cos(offsetmove[e]) * risefall[e].max_offset_z
				local new_y = endheight[e]-nhoverheight[e] + (math.cos(g_hover_heightangle[e])*nhoverheight[e])	
				offsetmove[e] = offsetmove[e] + risefall[e].rotate_speed/1000
				PositionObject(g_Entity[e]['obj'],new_x,new_y,new_z)
				CollisionOn(e)
				if hitonce[e] == 0 then 
					if PlayerDist < 50 then HurtPlayer(-1,1) end
					hittime[e] = g_Time + 1000
					hitonce[e] = 1
				end
				if g_Time > hittime[e] then hitonce[e] = 0 end
			end	
			if GetTimer(e) >= rf_time[e] then
				if risefall[e].release_mode == 1 then  --Gravity Fall
					status[e] = "falling"
					GravityOn(e)
					StopSound(e,0)
					PlaySound(e,1)
					g_Entity[e]['activated'] = 0
				end
				if risefall[e].release_mode == 2 then  --Random Throw
					status[e] = "throwing"
					GravityOn(e)
					PushObject(g_Entity[e]['obj'], math.random(1,risefall[e].release_force), math.random(1,risefall[e].release_force), math.random(1,risefall[e].release_force/2), 8, 1, 0 )
					StopSound(e,0)
					PlaySound(e,1)
					g_Entity[e]['activated'] = 0
				end
				if risefall[e].release_mode == 3 then  --Throw at Player
					status[e] = "throwing"
					GravityOn(e)
					local plrpos = V.Create(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ)
					local objpos = V.Create(g_Entity[e]['x'],g_Entity[e]['y'],g_Entity[e]['z'])
					newvec = V.Mul(V.Norm(V.Sub(plrpos,objpos)),risefall[e].release_force)
					PushObject(g_Entity[e]['obj'],newvec.x, newvec.y, newvec.z)
					if risefall[e].release_force > 3 then HurtPlayer(-1,risefall[e].hit_damage) end
					if risefall[e].release_force > 100 then HurtPlayer(-1,risefall[e].hit_damage*2) end
					if risefall[e].release_force > 300 then HurtPlayer(-1,risefall[e].hit_damage*3) end
					if risefall[e].release_force > 600 then HurtPlayer(-1,risefall[e].hit_damage*6) end
					if risefall[e].release_force > 800 then HurtPlayer(-1,risefall[e].hit_damage*8) end					
					if risefall[e].release_force > 1000 then HurtPlayer(-1,risefall[e].hit_damage*10) end					
					StopSound(e,0)
					PlaySound(e,1)
					g_Entity[e]['activated'] = 0
				end
			end
		end
	end
end