-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Jump Scare v3 by Necrym59
-- DESCRIPTION: Will allow a jump scare entity to appear set amount of times for set a period then is destroyed.
-- DESCRIPTION: Attach to an object and link to a trigger zone or switch to activate.
-- DESCRIPTION: [APPEARANCES=1(1,50)]	how many times entity will appear. 
-- DESCRIPTION: [#INTERVAL=2(1,100)] how long between appearances and length of apperance in 10th seconds
-- DESCRIPTION: [RANDOM_X_MOVEMENT=50] random movement x from start location
-- DESCRIPTION: [RANDOM_Z_MOVEMENT=50] random movement z from start location

local jumpscare = {}
local appearances = {}
local interval = {}	
local random_x_movement = {}
local random_z_movement = {}

local appcount = {}
local doonce = {}		
local showtime = {}
local endtime = {}
local status = {}
	
function jumpscare_properties(e, appearances, interval, random_x_movement, random_z_movement)
	jumpscare[e].appearances = appearances	
	jumpscare[e].interval = interval
	jumpscare[e].random_x_movement = random_x_movement	
	jumpscare[e].random_z_movement = random_z_movement	
end
 
function jumpscare_init(e)
	jumpscare[e] = {}
	jumpscare[e].appearances = 1		
	jumpscare[e].interval = 2
	jumpscare[e].random_x_movement = 50
	jumpscare[e].random_z_movement = 50	
	---------------	
	doonce[e] = 0
	appcount[e] = 0
	showtime[e] = 0
	endtime[e] = 0
	status[e] = "init"
	Hide(e)
	CollisionOff(e)
end
 
function jumpscare_main(e)
	if status[e] == "init" then
		appcount[e] = jumpscare[e].appearances+1
		status = "showtime"
	end 

	if g_Entity[e]['activated'] == 1 then
	
		if status == "showtime" then
			if appcount[e] > 0 then
				if doonce[e] == 0 then
					PlaySound(e,1)
					Show(e)
					RotateToPlayer(e)
					appcount[e] = appcount[e] - 1
					showtime[e] = g_Time + math.random((jumpscare[e].interval*100)*0.75,(jumpscare[e].interval*100)* 1.25)
					doonce[e] = 1
				end	
				if g_Time > showtime[e] then
					status = "endtime"
				end	
			end
		end
		
		if status == "endtime" then
			if doonce[e] == 1 then
				Hide(e)
				SetPosition(e,g_Entity[e]['x'] + math.random(-jumpscare[e].random_x_movement,jumpscare[e].random_x_movement),g_Entity[e]['y'],g_Entity[e]['z']+math.random(-jumpscare[e].random_z_movement,jumpscare[e].random_z_movement))								
				endtime[e] = g_Time + math.random((jumpscare[e].interval*100)*0.75,(jumpscare[e].interval*100)* 1.25)
				doonce[e] = 0
			end
			if g_Time > endtime[e] then
				status = "showtime"
			end			
			if appcount[e] == 1 then
				Hide(e)
				Destroy(e)
			end
		end
	end	
end 
 
function jumpscare_exit(e) 
end