-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Healthbar v5 - by Necrym,59
-- DESCRIPTION: A global behavior that display a viewed enemys health in a bar or text, set Always Active.
-- DESCRIPTION: Place the Health Bar object on your map.
-- DESCRIPTION: Attach this behavior to the Health Bar object.
-- DESCRIPTION: [DISPLAY_RANGE=200(100,1000)]
-- DESCRIPTION: [@DISPLAY_MODE=1(1=Bar, 2=Text, 3=Text+Bar)]
-- DESCRIPTION: [Y_ADJUSTMENT=0(-50,50)]

local P = require "scriptbank\\physlib"
local healthbar = {}
local display_range = {}
local display_mode = {}
local y_adjustment = {}
local allegiance = {}

local entheight	= {}

function healthbar_properties(e, display_range, display_mode, y_adjustment)
	healthbar[e].display_range = display_range or 500
	healthbar[e].display_mode = display_mode or 1
	healthbar[e].y_adjustment = y_adjustment	
end

function healthbar_init(e)
	healthbar[e] = {}
	healthbar[e].display_range = 500
	healthbar[e].display_mode = 1
	healthbar[e].y_adjustment = 0	
	healthbar[e].allegiance = 0
	Hide(e)
	GravityOff(e)
	entheight[e] = 0
end

function healthbar_main(e)
	 
	if healthbar[e].allegiance == 0 then 
		for a = 1, g_EntityElementMax do 
			if a ~= nil and g_Entity[a] ~= nil then 
				local allegiance = GetEntityAllegiance(a) -- get the allegiance value for this entity (0-enemy, 1-ally, 2-neutral)
				if allegiance == 0 then
					Ent = g_Entity[a]
					local dims = P.GetObjectDimensions(Ent.obj)
					entheight[e] = (dims.h + healthbar[e].y_adjustment)
					if PlayerLooking(a,healthbar[e].display_range,5) == 1 then
						PlayerDist = GetPlayerDistance(a)
						if PlayerDist < healthbar[e].display_range then 
							if healthbar[e].display_mode == 1 and g_Entity[a]['health'] > 0 then
								Show(e)
							end
							if healthbar[e].display_mode == 2 and g_Entity[a]['health'] > 0 then
								ScaleObject(5999,20.0,50.0,20.0)
								Prompt3D("Health: "..g_Entity[a]['health'],10)
								PositionPrompt3D(g_Entity[e]['x'],g_Entity[e]['y']-5,g_Entity[e]['z'],g_PlayerAngY)
							end
							if healthbar[e].display_mode == 3 and g_Entity[a]['health'] > 0 then
								Show(e)
								ScaleObject(5999,20.0,50.0,20.0) 
								Prompt3D("Health: "..g_Entity[a]['health'],10)
								PositionPrompt3D(g_Entity[e]['x'],g_Entity[e]['y']-5,g_Entity[e]['z'],g_PlayerAngY)
							end							
						end
						if PlayerDist > healthbar[e].display_range -20 or g_Entity[a]['health'] < 1 then
							if healthbar[e].display_mode == 1 then
								Hide(e)
							end
							if healthbar[e].display_mode == 2 then
								Prompt3D("",10)
								PositionPrompt3D(g_Entity[e]['x'],g_Entity[e]['y'],g_Entity[e]['z'],g_PlayerAngY)
							end
							if healthbar[e].display_mode == 3 then
								Hide(e)
								Prompt3D("",10)
								PositionPrompt3D(g_Entity[e]['x'],g_Entity[e]['y']-5,g_Entity[e]['z'],g_PlayerAngY)
							end
						end
						if g_Entity[a]['health'] < 9000 then														
							ScaleObject(g_Entity[e]['obj'],g_Entity[a]['health']/5,100,1)
							SetPosition(e,g_Entity[a]['x'], g_Entity[a]['y']+entheight[e], g_Entity[a]['z'])							
							ResetPosition(e,g_Entity[a]['x'], g_Entity[a]['y']+entheight[e], g_Entity[a]['z'])																					
							if g_Entity[a]['health'] > 100 then SetRotation(e,0,g_PlayerAngY+180,g_PlayerAngZ) end
							if g_Entity[a]['health'] < 100 then SetRotation(e,0,g_PlayerAngY,g_PlayerAngZ) end
						end						
					end
				end				
			end 
		end	
	end
end

function PlayerLooking(e,dis,v)
	if g_Entity[e] ~= nil then
		if dis == nil then
			dis = 3000
		end
		if v == nil then
			v = 0.5
		end
		if GetPlayerDistance(e) <= dis then
			local destx = g_Entity[e]['x'] - g_PlayerPosX
			local destz = g_Entity[e]['z'] - g_PlayerPosZ
			local angle = math.atan2(destx,destz)
			angle = angle * (180.0 / math.pi)
			if angle <= 0 then
				angle = 360 + angle
			elseif angle > 360 then
				angle = angle - 360
			end
			while g_PlayerAngY < 0 or g_PlayerAngY > 360 do
				if g_PlayerAngY <= 0 then
					g_PlayerAngY = 360 + g_PlayerAngY
				elseif g_PlayerAngY > 360 then
					g_PlayerAngY = g_PlayerAngY - 360
				end
			end
			local L = angle - v
			local R = angle + v
			if L <= 0 then
				L = 360 + L 
			elseif L > 360 then
				L = L - 360
			end
			if R <= 0 then
				R = 360 + R
			elseif R > 360 then
				R = R - 360
			end
			if (L < R and math.abs(g_PlayerAngY) > L and math.abs(g_PlayerAngY) < R) then
				return 1
			elseif (L > R and (math.abs(g_PlayerAngY) > L or math.abs(g_PlayerAngY) < R)) then
				return 1
			else
				return 0
			end
		else		
			return 0		
		end
	end
end
