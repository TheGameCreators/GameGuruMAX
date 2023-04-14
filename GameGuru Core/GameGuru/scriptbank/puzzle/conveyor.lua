-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Conveyor Script v6
-- DESCRIPTION: Creates a conveyor object to propell the player in faced direction?
-- DESCRIPTION: Change the conveyor [MOVESPEED#=10(1,200)]
-- DESCRIPTION: Turn on/off [SHOW_CONVEYOR!=0] to hide/unhide conveyor. 
-- DESCRIPTION: <Sound0> - for conveyor.

local U = require "scriptbank\\utillib"
local rad = math.rad
local abs = math.abs

g_conveyor = {}
local conveyor = {}
local movespeed = {}
local show_conveyor = {}
local conveyor_active = {}
local state = {}
local onconveyor = {}
local current_level		= {}
	
function conveyor_properties(e, movespeed, show_conveyor)
	g_conveyor[e] = g_Entity[e]
	g_conveyor[e]['movespeed'] = movespeed	
	g_conveyor[e]['show_conveyor'] = show_conveyor
end -- End properties

function conveyor_init(e)
	g_conveyor[e] = {}	
	g_conveyor[e]['movespeed'] = 1
	g_conveyor[e]['show_conveyor'] = 0
	conveyor_active[e] = 0
	onconveyor[e] = 0
	current_level[e] = GetEntityBaseAlpha(e)
	SetEntityTransparency(e,1)	
	state[e] = "init"	
end -- End init

function conveyor_main(e)
	g_conveyor[e] = g_Entity[e]	
	
	-- Conveyor Init
	if state[e] == 'init' then
		if g_conveyor[e]['show_conveyor'] == 0 then SetEntityBaseAlpha(e,0) end
		if g_conveyor[e]['show_conveyor'] == 1 then SetEntityBaseAlpha(e,current_level[e]) end	
		state[e] = "endinit"		
	end
	
	-- Conveyor Belt Start	
	onconveyor[e]=IntersectAll(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,g_PlayerPosX,-1,g_PlayerPosZ,0)
	-- Active conveyor
 	if onconveyor[e] ~= g_Entity[e]['obj'] then onconveyor[e] = 0 end
	if g_Entity[e]['obj'] == onconveyor[e] then		
		conveyor_active[e] = 1
		LoopSound(e,0)
		state[e] = "move-on"
	else		
		conveyor_active[e] = 0
		StopSound(e,0)
	end
	--Move on conveyor	
	if state[e] == "move-on" and conveyor_active[e] == 1 then
		if onconveyor[e] > 0 and conveyor_active[e] == 1 then		
			ForcePlayer(g_PlayerAngY,g_conveyor[e]['movespeed']/10)
		end			
		if onconveyor[e] == 0 then
			state[e] = "move-off"
		end
	end
	--Move off conveyor	
	if state[e] == "move-off" then		
		StopSound(e,0)		
		conveyor_active[e] = 0
	end
end
	
function conveyor_exit(e)
	SetEntityBaseAlpha(e,current_level[e])
	SetEntityTransparency(e,0)
end
