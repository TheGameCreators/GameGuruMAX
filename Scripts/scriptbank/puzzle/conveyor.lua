-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Conveyor Script v9
-- DESCRIPTION:Creates a conveyor object to propell the player in conveyors direction?
-- DESCRIPTION:[#MOVESPEED=5(1,200)] the conveyor move speed
-- DESCRIPTION:[SHOW_CONVEYOR!=0] to hide/unhide conveyor object. 
-- DESCRIPTION:[@USE_VARIABLE_SWITCH=2(1=Yes,2=No)]
-- DESCRIPTION:[@@VARIABLE_SWITCH_USER_GLOBAL$=""(0=globallist)]  User Global to monitor (eg; Variable_Switch1)
-- DESCRIPTION:<Sound0> - for conveyor.

local U = require "scriptbank\\utillib"
local rad = math.rad
local abs = math.abs

local conveyor						= {}
local movespeed						= {}
local show_conveyor 				= {}
local use_variable_switch 			= {}
local variable_switch_user_global	= {}

local conveyor_active 	= {}
local state				= {}
local onconveyor		= {}
local current_alpha		= {}
local currentvalue		= {}
local conveyorxpos 		= {}
local conveyorypos 		= {}
local conveyorzpos 		= {}
local conveyorxang 		= {}
local conveyoryang 		= {}
local conveyorzang 		= {}
	
function conveyor_properties(e, movespeed, show_conveyor, use_variable_switch, variable_switch_user_global)
	conveyor[e].movespeed = movespeed	
	conveyor[e].show_conveyor = show_conveyor
	conveyor[e].use_variable_switch = use_variable_switch
	conveyor[e].variable_switch_user_global	= variable_switch_user_global
end

function conveyor_init(e)
	conveyor[e] = {}	
	conveyor[e].movespeed = 1
	conveyor[e].show_conveyor = 0
	conveyor[e].use_variable_switch = 2
	conveyor[e].variable_switch_user_global	= ""	
	
	conveyor_active[e] = 0
	onconveyor[e] = 0
	currentvalue[e] = 0
	current_alpha[e] = GetEntityBaseAlpha(e)
	SetEntityTransparency(e,1)	
	state[e] = "init"	
end

function conveyor_main(e)
	
	if state[e] == 'init' then
		conveyorxpos[e],conveyorypos[e],conveyorzpos[e],conveyorxang[e],conveyoryang[e],conveyorzang[e] = GetEntityPosAng(e)
		if conveyor[e].show_conveyor == 0 then SetEntityBaseAlpha(e,0) end
		if conveyor[e].show_conveyor == 1 then SetEntityBaseAlpha(e,current_alpha[e]) end	
		state[e] = "endinit"		
	end
		
	if conveyor[e].use_variable_switch == 1 then
		if conveyor[e].variable_switch_user_global ~= "" then
			if _G["g_UserGlobal['"..conveyor[e].variable_switch_user_global.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..conveyor[e].variable_switch_user_global.."']"] end
			conveyor[e].movespeed = _G["g_UserGlobal['"..conveyor[e].variable_switch_user_global.."']"]
		end
	end	
	
	-- Conveyor Belt Start	
	onconveyor[e]=IntersectAll(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,g_PlayerPosX,-1,g_PlayerPosZ,0)
	-- Active conveyor
 	if onconveyor[e] ~= g_Entity[e].obj then onconveyor[e] = 0 end
	if g_Entity[e].obj == onconveyor[e] then		
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
			ForcePlayer(conveyoryang[e],conveyor[e].movespeed/100)
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
	SetEntityBaseAlpha(e,current_alpha[e])
	SetEntityTransparency(e,0)
end
