-- Dynamite v12 - By Necrym59
-- DESCRIPTION: Allows for pick and deployment of an exposive bomb device. Set Entity Explodable.
-- DESCRIPTION: [WEAPON_NAME$="Dynamite"]
-- DESCRIPTION: [PLACEMENT_TIME=1(1,3)]
-- DESCRIPTION: [EXPLOSION_DELAY=1(1,50)]
-- DESCRIPTION: [@BOMB_TYPE=1(1=Manual, 2=Timer, 3=Remote, 4=Switch)]
-- DESCRIPTION: [BOMB_PRIME_KEY$="Z"] Bound key to activate
-- DESCRIPTION: [PLAYER_SAFE_DISTANCE=220(1,1000)]
-- DESCRIPTION: [ENEMY_HEAR_DISTANCE=1000(0,3000)]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [PROMPT_TEXT$="E to collect"]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\pickup.png"]
-- DESCRIPTION: <Sound0> Pickup Sound
-- DESCRIPTION: <Sound1> the primed/armed sound

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local rad = math.rad
local dynamite = {}
local weapon_name = {}
local placement_time = {}
local explosion_delay = {}
local bomb_type = {}
local bomb_prime_key = {}
local player_safe_distance = {}
local enemy_hear_distance = {}
local pickup_range = {}
local prompt_text = {}
local prompt_display = {}
local item_highlight = {}
local highlight_icon = {}

local bombs = {}
local bomb_state = {}
local timer_running = {}
local pressed = {}
local prime_time = {}
local armed_time = {}
local dynamite_armed = {}
local status = {}
local detonation_time = {}

local currentY = {}
local colobj = {}
local terrainy = {}
local surfacey = {}
local finaly = {}
local tEnt = {}
local hl_icon = {}
local hl_imgwidth = {}
local hl_imgheight = {}

function dynamite_properties(e, weapon_name, placement_time, explosion_delay, bomb_type, bomb_prime_key, player_safe_distance, enemy_hear_distance, pickup_range, prompt_text, prompt_display, item_highlight, highlight_icon_imagefile)
	dynamite[e].weapon_name = weapon_name						--Name of weapon
	dynamite[e].placement_time = placement_time					--How long you need to hold key for before the bomb is placed (in seconds)
	dynamite[e].explosion_delay = explosion_delay				--How long before the bomb explodes once triggered (in seconds)
	dynamite[e].bomb_type = bomb_type							--The explosive bomb type
	dynamite[e].bomb_prime_key = bomb_prime_key					--Key to prime
	dynamite[e].player_safe_distance = player_safe_distance		--Minimum safe distance player needs to be before the explosive can be triggered
	dynamite[e].enemy_hear_distance = enemy_hear_distance		--Distance nearby enemy can hear the explosion
	dynamite[e].pickup_range = pickup_range						--Pickup range
	dynamite[e].prompt_text = prompt_text						--Pickup prompt text
	dynamite[e].prompt_display = prompt_display					--Prompt text
	dynamite[e].item_highlight = item_highlight					--Highlight style
	dynamite[e].highlight_icon = highlight_icon_imagefile		--Highlight Icon
end

function dynamite_init(e)
	dynamite[e] = {}
	dynamite[e].weapon_name = "Dynamite"
	dynamite[e].placement_time = 1
	dynamite[e].explosion_delay = 1
	dynamite[e].bomb_type = 0
	dynamite[e].bomb_prime_key = "Z"	
	dynamite[e].player_safe_distance = 500
	dynamite[e].enemy_hear_distance = 0
	dynamite[e].pickup_range = 80
	dynamite[e].prompt_text = "E to collect"
	dynamite[e].prompt_display = prompt_display
	dynamite[e].item_highlight = item_highlight
	dynamite[e].highlight_icon = "imagebank\\icons\\pickup.png"
	
	bombs[e] = 0
	pressed[e] = 0
	bomb_state[e] = "collect"
	prime_time[e] = 0
	armed_time[e] = 0
	dynamite_armed[e] = 0
	detonation_time[e] = 0
	status[e] = "init"
	colobj[e] = 0
	SetEntityBaseAlpha(e,100)
	SetEntityTransparency(e,1)
	terrainy[e] = 0
	surfacey[e] = 0
	finaly[e] = 0
	tEnt[e] = 0
	g_tEnt = 0	
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0
end

function dynamite_main(e)

	if status[e] == "init" then
		if dynamite[e].item_highlight == 3 and dynamite[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(dynamite[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(dynamite[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(dynamite[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end	
		if dynamite[e].bomb_type == nil then dynamite[e].bomb_type = 1 end
		bomb_state[e] = "collect"
		SetActivated(e,1)
		currentY[e] = g_Entity[e].y
		status[e] = "init_end"
	end
	local PlayerDist = GetPlayerDistance(e)

	if g_Entity[e].activated == 1 and dynamite_armed[e] == 0 then
		if bomb_state[e] == "collect" and bombs[e] == 0 then
			if PlayerDist < dynamite[e].pickup_range then
				--pinpoint select object--
				module_misclib.pinpoint(e,dynamite[e].pickup_range,dynamite[e].item_highlight,hl_icon[e])
				tEnt[e] = g_tEnt
				--end pinpoint select object--
				if PlayerDist < dynamite[e].pickup_range and tEnt[e] == e and GetEntityVisibility(e) == 1 then
					if dynamite[e].prompt_display == 1 then PromptLocal(e,dynamite[e].prompt_text) end
					if dynamite[e].prompt_display == 2 then Prompt(dynamite[e].prompt_text) end
					if g_KeyPressE == 1 and pressed[e] == 0 then
						pressed[e] = 1
						PlaySound(e,0)
						bombs[e] = bombs[e] + 1
						bomb_state[e] = "collected"
						currentY[e] = currentY[e]
						ActivateIfUsed(e)
						Hide(e)
						CollisionOff(e)
						GravityOff(e)
					end
				end
			end
		end

		if bomb_state[e] == "collected" then			
			local ox,oy,oz = U.Rotate3D(0,currentY[e],dynamite[e].pickup_range, math.rad(g_PlayerAngX), math.rad(g_PlayerAngY), math.rad(g_PlayerAngZ))
			local forwardposx, forwardposy, forwardposz = g_PlayerPosX + ox, g_PlayerPosY + oy, g_PlayerPosZ + oz
			ResetPosition(e,forwardposx, forwardposy, forwardposz)
			RotateObject(g_Entity[e].obj,0,g_Entity[e].angley,g_PlayerAngZ)			
			SetEntityBaseAlpha(e,30)
			Show(e)
			if dynamite[e].prompt_display == 1 then PromptLocal(e,"Press " ..dynamite[e].bomb_prime_key.. " to place and prime the " ..dynamite[e].weapon_name.. " or Q to Drop") end
			if dynamite[e].prompt_display == 2 then Prompt("Press " ..dynamite[e].bomb_prime_key.. " to place and prime the " ..dynamite[e].weapon_name.. " or Q to Drop") end
			if g_InKey == string.lower(dynamite[e].bomb_prime_key) or g_InKey == tostring(dynamite[e].bomb_prime_key) and pressed[e] == 0 then
				SetEntityBaseAlpha(e,100)
				PromptLocal(e,"")
				local ox,oy,oz = U.Rotate3D(0,currentY[e],dynamite[e].pickup_range, math.rad(g_PlayerAngX), math.rad(g_PlayerAngY), math.rad(g_PlayerAngZ))
				local forwardposx, forwardposy, forwardposz = g_PlayerPosX + ox, g_PlayerPosY + oy, g_PlayerPosZ + oz
				terrainy[e] = GetTerrainHeight(forwardposx,forwardposz)
				surfacey[e] = GetSurfaceHeight(forwardposx,forwardposy,forwardposz)
				if surfacey[e] > terrainy[e] then finaly[e] = surfacey[e] end
				if terrainy[e] > surfacey[e] then finaly[e] = terrainy[e] end
				SetPosition(e,forwardposx, finaly[e]+3, forwardposz)
				ResetPosition(e,forwardposx, finaly[e]+3, forwardposz)
				bombs[e] = bombs[e] - 1
				dynamite_armed[e] = 0
				bomb_state[e] = "placed"
				GravityOn(e)
				CollisionOn(e)				
				Show(e)
				pressed[e] = 1
			end
			if GetInKey() == "q" or GetInKey() == "Q" and pressed[e] == 0 then
				SetEntityBaseAlpha(e,100)
				if dynamite[e].prompt_display == 1 then PromptLocal(e,"") end
				if dynamite[e].prompt_display == 2 then Prompt("") end				
				local ox,oy,oz = U.Rotate3D(0,currentY[e],dynamite[e].pickup_range, math.rad(g_PlayerAngX), math.rad(g_PlayerAngY), math.rad(g_PlayerAngZ))
				local forwardposx, forwardposy, forwardposz = g_PlayerPosX + ox, g_PlayerPosY + oy, g_PlayerPosZ + oz
				terrainy[e] = GetTerrainHeight(forwardposx,forwardposz)
				surfacey[e] = GetSurfaceHeight(forwardposx,forwardposy,forwardposz)
				if surfacey[e] > terrainy[e] then finaly[e] = surfacey[e] end
				if terrainy[e] > surfacey[e] then finaly[e] = terrainy[e] end
				SetPosition(e,forwardposx, finaly[e]+3, forwardposz)
				ResetPosition(e,forwardposx, finaly[e]+3, forwardposz)
				bombs[e] = bombs[e] - 1
				dynamite_armed[e] = 0
				bomb_state[e] = "collect"
				GravityOn(e)
				CollisionOn(e)
				Show(e)
				pressed[e] = 0
			end
		end
		if bomb_state[e] == "placed" then
			if prime_time[e] == 0 then
				if dynamite[e].prompt_display == 1 then PromptLocal(e,dynamite[e].weapon_name .. " placed") end
				if dynamite[e].prompt_display == 2 then Prompt(dynamite[e].weapon_name .. " placed") end			
				prime_time[e] = GetTimer(e) + (dynamite[e].placement_time * 1000)
				armed_time[e] = GetTimer(e) + (dynamite[e].explosion_delay * 1000)
				dynamite_armed[e] = 0
			end
			if GetTimer(e) > prime_time[e] -500 then PromptDuration(dynamite[e].weapon_name .. " primed", 1000) end
			if GetTimer(e) > prime_time[e] then
				bomb_state[e] = "primed"
				PlaySound(e,1)
			end
		end
		if bomb_state[e] == "primed" then
			if dynamite_armed[e] == 0 then
				if dynamite[e].bomb_type == 1 then  --(Manual Detonation)
					armed_time[e] = GetTimer(e)+ (5000 * 10)
					dynamite_armed[e] = 1
					SetEntityHealth(e,0)
					MakeAISound(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,dynamite[e].enemy_hear_distance,1,e)
				end
				if dynamite[e].bomb_type == 2 then  --(Timed Detonation)
					detonation_time[e] = math.floor(((armed_time[e] - GetTimer(e))/10)/60)
					Prompt(dynamite[e].weapon_name.. " will detonate in: " ..detonation_time[e]+1)
					if GetTimer(e) >= armed_time[e] then
						timer_running[e] = 1
						dynamite_armed[e] = 1
						SetEntityHealth(e,0)
						MakeAISound(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,dynamite[e].enemy_hear_distance,1,e)
					end
				end
				if dynamite[e].bomb_type == 3 and GetPlayerDistance(e) > dynamite[e].player_safe_distance then  --(Remote Detonation)
					PromptDuration("Press X To detonate the " ..dynamite[e].weapon_name,2000)
					if GetInKey() == "x" or GetInKey() == "X" then
						dynamite_armed[e] = 1
						SetEntityHealth(e,0)
						MakeAISound(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,dynamite[e].enemy_hear_distance,1,e)
					end
				end
				if dynamite[e].bomb_type == 4 then  --(switched Detonation)
					if dynamite[e].prompt_display == 1 then PromptLocal(e,"Use a switch to detonate the " ..dynamite[e].weapon_name) end
					if dynamite[e].prompt_display == 2 then PromptDuration("Use a switch to detonate the " ..dynamite[e].weapon_name,2000) end				
					dynamite_armed[e] = 1
					SetActivated(e,0)
				end				
			end
		end
	end
	
	if g_Entity[e].activated == 1 and dynamite_armed[e] == 1 then
		SetEntityHealth(e,0)
		MakeAISound(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,dynamite[e].enemy_hear_distance,1,e)
	end

	if g_KeyPressE == 0 then
		timer_running[e] = 0
		pressed[e] = 0
	end
end