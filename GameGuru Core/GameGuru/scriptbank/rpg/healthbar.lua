-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Healthbar v2 - by Necrym,59
-- DESCRIPTION: Will display viewed enemy health in a bar or text, set Always Active.
-- DESCRIPTION: [DISPLAY_RANGE=500(500,1000)]
-- DESCRIPTION: [@DISPLAY_MODE=1(1=Bar, 2=Text)]

local healthbar = {}
local display_range = {}
local display_mode = {}
local allegiance = {}
local health = {}

function healthbar_properties(e, display_range, display_mode, allegiance, health)
	healthbar[e] = g_Entity[e]
	healthbar[e].display_range = display_range or 500
	healthbar[e].display_mode = display_mode or 1
	healthbar[e].allegiance = 0
	healthbar[e].health = 0	
end

function healthbar_init(e)
	healthbar[e] = g_Entity[e]
	healthbar[e].display_range = 500
	healthbar[e].display_mode = 1
	healthbar[e].allegiance = 0
	healthbar[e].health = 0
	Hide(e)	
end

function healthbar_main(e)
	healthbar[e] = g_Entity[e]	
	GravityOff(e)	
	 
	if healthbar[e]['allegiance'] == 0 then 
		for a = 1, g_EntityElementMax do 
			if a ~= nil and g_Entity[a] ~= nil then 
				local allegiance = GetEntityAllegiance(a) -- get the allegiance value for this entity (0-enemy, 1-ally, 2-neutral)
				if allegiance == 0 then
					Ent = g_Entity[a]					
					if PlayerLooking(a,healthbar[e].display_range,5) == 1 then
						PlayerDist = GetPlayerDistance(a)
						if healthbar[e].display_mode == 1 then 
							if PlayerDist < (healthbar[e].display_range - 100) then Show(e) end
							if PlayerDist > (healthbar[e].display_range - 100) then Hide(e) end
						end
						if healthbar[e].display_mode == 2 then
							if PlayerDist < (healthbar[e].display_range - 100) then PromptLocal(a,"Health: "..g_Entity[a]['health']) end
							if PlayerDist > (healthbar[e].display_range - 100) then PromptLocal(a,"") end
						end
						if g_Entity[a]['health'] < 9000 then														
							ScaleObject(g_Entity[e]['obj'],g_Entity[a]['health']/5,100,1)
							SetPosition(e,g_Entity[a]['x'], g_Entity[a]['y']+80, g_Entity[a]['z'])
							ResetPosition(e,g_Entity[a]['x'], g_Entity[a]['y']+80, g_Entity[a]['z'])																					
							if g_Entity[a]['health'] > 80 then SetRotation(e,0,g_PlayerAngY+180,g_PlayerAngZ) end
							if g_Entity[a]['health'] < 80 then SetRotation(e,0,g_PlayerAngY,g_PlayerAngZ) end
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
