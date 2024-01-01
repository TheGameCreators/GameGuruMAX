-- Trapdoor v5 - thanks to Necrym59 & Lee
-- DESCRIPTION: For use with an animated door as a trapdoor when a player stands on it,
-- DESCRIPTION: Apply floor texture to disguise if required.
-- DESCRIPTION: When the player is within [RANGE=60(30,100)] 
-- DESCRIPTION: will open the trapdoor after [OPEN_DELAY=0(0,20)]
-- DESCRIPTION: and close after [CLOSE_DELAY=3(1,20)]
-- DESCRIPTION: Plays <Sound0> when opening. 
-- DESCRIPTION: Plays <Sound1> when closing.
-- DESCRIPTION: Change the trapdoors [@VISIBILITY=1(1=Visible, 2=Invisible)]

g_trapdoor = {}	
local range = {}
local open_delay = {}
local close_delay = {}
local visibility = {}	
local collision_delay = {}
local trap_status = {}
local trapdoor_pressed = {}
local time1 = {}
local time2 = {}
local timepassed = {}
local ontrapdoor = {}
local status = {}

function trapdoor_properties(e, range, open_delay, close_delay, visibility)
	g_trapdoor[e] = g_Entity[e]
	g_trapdoor[e]['range'] = range
	g_trapdoor[e]['open_delay'] = open_delay
	g_trapdoor[e]['close_delay'] = close_delay
	g_trapdoor[e]['visibility'] = visibility
end 

function trapdoor_init(e)
	g_trapdoor[e] = g_Entity[e]
	g_trapdoor[e]['range'] = 30
	g_trapdoor[e]['open_delay'] = 0
	g_trapdoor[e]['close_delay'] = 0
	g_trapdoor[e]['visibility'] = 1
	g_trapdoor[e]['mode'] = 0
	collision_delay[e] = 1.5
	trap_status[e] = 0
	time1[e] = 0
	time2[e] = 0
	timepassed[e] = 0
	ontrapdoor[e] = 0
	status[e] = "init"
end
	 
function trapdoor_main(e)
	g_trapdoor[e] = g_Entity[e]
	if status[e] == "init" then		
		if g_trapdoor[e]['range'] == nil then g_trapdoor[e]['range'] = 50 end
		if g_trapdoor[e]['visibility'] == nil then g_conveyor[e]['visibility'] = 1 end
		if g_trapdoor[e]['visibility'] == 1 then Show(e) end
		if g_trapdoor[e]['visibility'] == 2 then Hide(e) end
		status[e] = "finish"
	end
	local PlayerDist = GetPlayerDistance(e)	
	SetActivated(e,1)
	
	ontrapdoor[e]=IntersectAll(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,g_Entity[e]['x'],-1,g_Entity[e]['z'],0)
	if ontrapdoor[e] ~= g_Entity[e]['obj'] then
		ontrapdoor[e] = 0
	else
		ontrapdoor[e] = 1
	end	
	
	if g_PlayerHealth > 0 then
		if time1[e] == 0 then time1[e] = g_Time	end
		time2[e] = g_Time
		timepassed[e] = (time2[e] - time1[e]) / 1000		
		if PlayerDist <= g_trapdoor[e]['range'] and ontrapdoor[e] == 1 then
			if trap_status[e] == 0 then
				if timepassed[e] >= g_trapdoor[e]['open_delay'] and g_Entity[e]['animating'] == 0 then
					g_trapdoor[e]['mode'] = 201
					time1[e] = 0
					trap_status[e] = 1		
				end
			end
		end
		if trap_status[e] == 1 then
			if timepassed[e] >= g_trapdoor[e]['close_delay'] and g_Entity[e]['animating'] == 0 then
				g_trapdoor[e]['mode'] = 202
				time1[e] = 0
				trap_status[e] = 0
			end
		end
	end
	--allow close in its own time, even if player falls far down (maybe an option?)
	--if (PlayerDist > g_trapdoor[e]['range']) then
	--	if trap_status[e] == 1 then
	--		g_trapdoor[e]['mode'] = 202
	--		time1[e] = 0
	--		trap_status[e] = 0			
	--	end
	--end	
	if g_trapdoor[e]['mode'] == 201 then
		-- open trapdoor trigger
		SetAnimationName(e,"open")
		SetAnimationSpeed(e,200)
		PlayAnimation(e)  
		g_trapdoor[e]['mode'] = 2
		PlaySound(e,0)
		StartTimer(e)
	end
	if g_trapdoor[e]['mode'] == 2 then
		-- trapdoor collision off after X second
		if GetTimer(e)>collision_delay[e] then
			CollisionOff(e)
			g_trapdoor[e]['mode'] = 1
		end
	end
	if g_trapdoor[e]['mode'] == 202 then
		-- close trapdoor trigger
		SetAnimationName(e,"close")
		SetAnimationSpeed(e,100)
		PlayAnimation(e)  
		g_trapdoor[e]['mode'] = 3
		StartTimer(e)
	end	
	if g_trapdoor[e]['mode'] == 3 then
		-- trapdoor collision On after X second
		if GetTimer(e)>collision_delay[e] then
			CollisionOn(e)
			PlaySound(e,1)
			g_trapdoor[e]['mode'] = 0
		end
	end
end
