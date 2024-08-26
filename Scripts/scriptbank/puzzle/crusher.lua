-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Crusher Script v7
-- DESCRIPTION: This script is attached to the crusher object. Physics ON, Gravity OFF, Weight & Friction = 0, IsImobile ON
-- DESCRIPTION: [TRIGGER_RANGE=500(50,2000)]
-- DESCRIPTION: [MOVE_SPEED=60(1,500)]
-- DESCRIPTION: [DAMAGE_RANGE=80(1,500)]
-- DESCRIPTION: [DAMAGE_AMOUNT=50(0,500)] (0=Kill)
-- DESCRIPTION: [@USE_VARIABLE_SWITCH=2(1=Yes,2=No)]
-- DESCRIPTION: [VARIABLE_SWITCH_USER_GLOBAL$="Variable_Switch1"]
-- DESCRIPTION: <Sound0> for Crusher Loop
-- DESCRIPTION: <Sound1> for Crush Sound
-- DESCRIPTION: <Sound2> for Crusher hit

local lower = string.lower
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
local tableName = {}
local entrange	= {}
local trappedEnt = {}
local check_timer = {}
	
function crusher_properties(e, trigger_range, move_speed, damage_range, damage_amount, use_variable_switch, variable_switch_user_global)
	crusher[e] = g_Entity[e]
	crusher[e].trigger_range = trigger_range					
	crusher[e].move_speed = move_speed							
	crusher[e].damage_range = damage_range						
	crusher[e].damage_amount = damage_amount					
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
	check_timer[e] = 0
	trappedEnt[e] = 0	
	status[e] = "init"
	played = 0
	tableName[e] = "crushlist" ..tostring(e)
	_G[tableName[e]] = {}	
end
 
function crusher_main(e)

	if status[e] == "init" then
		minheight[e] = (GetSurfaceHeight(g_Entity[e]['x'],g_Entity[e]['y'],g_Entity[e]['z']))
		maxheight[e] = starty[e]
		for n = 1, g_EntityElementMax do
			if n ~= nil and g_Entity[n] ~= nil then
				if GetEntityAllegiance(n) > -1 then
					table.insert(_G[tableName[e]],n)
				end
			end
		end		
		status[e] = "crushcheck"
	end
	
	if status[e] == "crushcheck" then
		if crusher[e].use_variable_switch == 1 then
			if crusher[e].variable_switch_user_global ~= "" then
				if _G["g_UserGlobal['"..crusher[e].variable_switch_user_global.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..crusher[e].variable_switch_user_global.."']"] end
				crusher[e].move_speed = _G["g_UserGlobal['"..crusher[e].variable_switch_user_global.."']"]
			end
		end
		if g_Time > check_timer[e] then
			for _,v in pairs (_G[tableName[e]]) do
				if g_Entity[v] ~= nil then
					entrange[e] = math.ceil(GetFlatDistance(e,v))
					if g_Entity[v]["health"] > 0 and entrange[e] < crusher[e].trigger_range then							
						trappedEnt[e] = v
						break
					else
						trappedEnt[e] = 0
					end
				end
			end
			check_timer[e] = g_Time + 10
		end
		if GetPlayerFlatDistance(e) < crusher[e].trigger_range or entrange[e] < crusher[e].trigger_range and trappedEnt[e] > 0 then
			SetActivated(e,1)
			status[e] = "crush"
		end	
	end	

	if g_Entity[e]['activated'] == 1 then
		LoopSound(e,0)
		if status[e] == "crush" then				
			if g_Entity[e]['y'] > minheight[e] then
				CollisionOff(e)
				MoveUp(e,-crusher[e].move_speed)
			end
			if g_Entity[e]['y'] > minheight[e] * 0.9 then
				if GetPlayerFlatDistance(e) < crusher[e].damage_range then
					if crusher[e].damage_amount == 0 then
						HurtPlayer(-1,5000)
						SetPlayerHealth(0)
					end						
					if crusher[e].damage_amount > 0 then
						HurtPlayer(-1,crusher[e].damage_amount)
					end
				end				
			end
			if g_Entity[e]['y'] > minheight[e]+20 then				
				if GetPlayerFlatDistance(e) < crusher[e].damage_range / 2 then					
					HurtPlayer(-1,crusher[e].damage_amount)				
					if played == 0 then
						PlaySound(e,1)
						played = 1
					end
				end
			end
			if trappedEnt[e] ~= 0 then
				if entrange[e] < crusher[e].damage_range and g_Entity[trappedEnt[e]]['health'] > 0 then
					if crusher[e].damage_amount == 0 then
						SetEntityHealth(trappedEnt[e],0)						
					end
					if crusher[e].damage_amount > 0 then
						SetEntityHealth(trappedEnt[e],g_Entity[trappedEnt[e]]['health']-crusher[e].damage_amount)
					end					
				end
			end							
			if g_Entity[e]['y'] <= minheight[e] then
				PlaySound(e,2)
				status[e] = "reset"					
			end			
		end	
	
		if status[e] == "reset" then
			played = 0
			if g_Entity[e]['y'] < maxheight[e] then
				CollisionOff(e)					
				MoveUp(e,crusher[e].move_speed)				
			end
			if g_Entity[e]['y'] >= maxheight[e] then
				status[e] = "crushcheck"					
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

function GetFlatDistance(e,v)
	if g_Entity[e] ~= nil and g_Entity[v] ~= nil then
		local distDX = g_Entity[e]['x'] - g_Entity[v]['x']
		local distDZ = g_Entity[e]['z'] - g_Entity[v]['z']
		return math.sqrt((distDX*distDX)+(distDZ*distDZ));
	end
end