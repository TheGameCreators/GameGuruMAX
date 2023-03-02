-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Collapsing Floor v6
-- DESCRIPTION: Creates a collapsing Floor area oject when walked on. Physics=ON, Gravity=ON
-- DESCRIPTION: [PROMPT_TEXT$="Floor is collapsing"],
-- DESCRIPTION: [@COLLAPSE_TYPE=1(1=Collapsing, 2=Fading)]
-- DESCRIPTION: [ACTIVE_TIME=2000(1,3000)]
-- DESCRIPTION: [ACTIVE_RANGE=40(0,100)]
-- DESCRIPTION: [@DAMAGE_TYPE=1(1=Fixed Fall, 2=Natural Fall)]
-- DESCRIPTION: [DAMAGE_HEIGHT=100(1,200)]
-- DESCRIPTION: [COLLISION_SCALE#=0.95(0.75,1.00)]

local U = require "scriptbank\\utillib"

local collapse_floor = {}
local collapse_type = {}
local active_time = {}
local active_range = {}
local damage_type = {}
local damage_height = {}
local collision_scale = {}
local state = {}
local fade_delay = {}
local active_time_new = {}
local collapse_delay_new = {}
local shown = {}
local onfloor = {}
local startheight = {}
local collapsing = {}
local fallen = {}

function collapsing_floor_properties(e, prompt_text, collapse_type, active_time, active_range, damage_type, damage_height, collision_scale)
	collapse_floor[e] = g_Entity[e]
	collapse_floor[e].prompt_text = prompt_text						--text upon floor collapsing
	collapse_floor[e].collapse_type = collapse_type or 1 			--1 gravity collapse or 2 fade/dissapear
	collapse_floor[e].active_time = active_time 					--delay before floor collapses
	collapse_floor[e].active_range = active_range 					--when it begins to fade	
	collapse_floor[e].damage_type = damage_type or 1 				--1 to set fall damage or 0 natural fall damage
	collapse_floor[e].damage_height = damage_height 				--when to start trigger damage
	collapse_floor[e].collision_scale = collision_scale				--when to start trigger damage
end

function collapsing_floor_init(e)
	collapse_floor[e] = g_Entity[e]
	collapse_floor[e].prompt_text = ""
	collapse_floor[e].collapse_type = 1
	collapse_floor[e].active_time = 2000
	collapse_floor[e].active_range = 80 
	collapse_floor[e].damage_type = 1
	collapse_floor[e].damage_height = 100 
	collapse_floor[e].collision_scale = 0.95
	fade_delay[e] = collapse_floor[e].active_time / 50
	shown[e] = 1
	onfloor[e] = 0
	fallen[e] = 0
	state[e] = "wait"
	startheight[e] = g_Entity[e]['y']
	GravityOff(e)	
	collapsing[e] = 0
end

function collapsing_floor_main(e)	
	collapse_floor[e] = g_Entity[e]
	SetBodyScaling(g_Entity[e]['obj'],collapse_floor[e].collision_scale,collapse_floor[e].collision_scale,collapse_floor[e].collision_scale)
	
	if state[e] == "wait" then		
		onfloor[e]=IntersectAll(g_PlayerPosX,g_PlayerPosY+35,g_PlayerPosZ,g_Entity[e]['x'],36,g_Entity[e]['z'],0)	
		if onfloor[e] == g_Entity[e]['obj'] and GetPlayerDistance(e) <= collapse_floor[e].active_range then			
			state[e] = "active"			
			active_time_new[e] = GetTimer(e) + collapse_floor[e].active_time
			collapse_delay_new[e] = GetTimer(e) + fade_delay[e]			
		end		
	elseif state[e] == "active" then
		Prompt(collapse_floor[e].prompt_text)
		if collapse_floor[e].collapse_type == 1 then
			if GetTimer(e) > active_time_new[e] then				
				collapsing[e] = 1
				fallen[e] = 1
				GravityOn(e)
			end
		end
		if collapse_floor[e].collapse_type == 2 then
			if GetTimer(e) < active_time_new[e] then
				if GetTimer(e) > collapse_delay_new[e] then					
					collapse_delay_new[e] = GetTimer(e) + fade_delay[e]
					fade_delay[e] = fade_delay[e] * 0.75
					if shown[e] == 1 then
						Hide(e)
						shown[e] = 0
					elseif shown[e] == 0 then
						Show(e)
						shown[e] = 1
					end
				end
			else
				Hide(e)		
				shown[e] = 0
				CollisionOff(e)
				fallen[e] = 1
			end
		end
	end
	if collapse_floor[e].damage_type == 1 and fallen[e] == 1 then
		if collapse_floor[e].collapse_type == 1 then
			if g_PlayerPosY < (startheight[e] - collapse_floor[e].damage_height) then
				HurtPlayer(e,g_PlayerHealth)				
			end
		end
		if collapse_floor[e].collapse_type == 2 and fallen[e] == 1 then
			if g_PlayerPosY < g_Entity[e]['y'] - collapse_floor[e].damage_height and GetPlayerFlatDistance(e) < collapse_floor[e].active_range then
				HurtPlayer(e,g_PlayerHealth)				
			end
		end
	end
end

function GetPlayerFlatDistance(e)
	tPlayerDX = (g_Entity[e]['x'] - g_PlayerPosX)
	tPlayerDZ = (g_Entity[e]['z'] - g_PlayerPosZ)
	return math.sqrt(math.abs(tPlayerDX*tPlayerDX)+math.abs(tPlayerDZ*tPlayerDZ));
end