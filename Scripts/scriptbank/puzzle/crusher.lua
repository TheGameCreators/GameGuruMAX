-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Crusher Script v6
-- DESCRIPTION: This script is attached to the crusher object. Physics ON, Gravity OFF, Weight & Friction = 0, IsImobile ON
-- DESCRIPTION: [TRIGGER_RANGE=500(50,2000)]
-- DESCRIPTION: [MOVE_SPEED=60(1,500)]
-- DESCRIPTION: [DAMAGE_RANGE=80(1,500)]
-- DESCRIPTION: [DAMAGE_AMOUNT=50(1,500)]
-- DESCRIPTION: [@USE_VARIABLE_SWITCH=2(1=Yes,2=No)]
-- DESCRIPTION: [VARIABLE_SWITCH_USER_GLOBAL$="Variable_Switch1"]
-- DESCRIPTION: <Sound0> for Crusher Loop
-- DESCRIPTION: <Sound1> for Crush Sound
-- DESCRIPTION: <Sound2> for Crusher hit

local crusher 						= {}
local trigger_range 				= {}
local move_speed 					= {}
local damage_range 					= {}
local use_variable_switch 			= {}
local variable_switch_user_global	= {}		

local maxheight = {}
local minheight = {}
local currentvalue = {}
local status = {}
local played = {} 
local startx = {}
local starty = {}
local startz = {}
	
function crusher_properties(e, trigger_range, move_speed, damage_range, damage_amount, use_variable_switch, variable_switch_user_global)
	crusher[e] = g_Entity[e]
	crusher[e].trigger_range = trigger_range					--when to start moving the crusher
	crusher[e].move_speed = move_speed							--crusher speed
	crusher[e].damage_range = damage_range						--range at which damage will be start to be applied
	crusher[e].damage_amount = damage_amount					--crusher damage applied
	crusher[e].use_variable_switch = use_variable_switch
	crusher[e].variable_switch_user_global = variable_switch_user_global
end
	
function crusher_init(e)
	crusher[e] = {}
	crusher[e].trigger_range = 500
	crusher[e].move_speed = 60	
	crusher[e].damage_range = 80
	crusher[e].damage_amount = 50
	crusher[e].use_variable_switch = 2
	crusher[e].variable_switch_user_global = ""
	
	minheight[e] = 0
	maxheight[e] = 0
	starty[e] = g_Entity[e]['y']
	currentvalue[e] = 0
	status[e] = "init"
	played = 0
end
 
function crusher_main(e)

	if status[e] == "init" then
		minheight[e] = (GetSurfaceHeight(g_Entity[e]['x'],g_Entity[e]['y'],g_Entity[e]['z']))
		maxheight[e] = starty[e]
		status[e] = "crush"
	end
	
	if crusher[e].use_variable_switch == 1 then
		if crusher[e].variable_switch_user_global ~= "" then
			if _G["g_UserGlobal['"..crusher[e].variable_switch_user_global.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..crusher[e].variable_switch_user_global.."']"] end
			crusher[e].move_speed = _G["g_UserGlobal['"..crusher[e].variable_switch_user_global.."']"]
		end
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
				PlaySound(e,2)
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