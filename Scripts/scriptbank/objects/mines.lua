-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Mines v7  by Necrym 59
-- DESCRIPTION: Allows pickup and deployment of an anti-personnel mine device. Physics=ON, Always Active=ON, IsImobile=ON, Explodable=ON
-- DESCRIPTION: [WEAPON_NAME$="Anti-Personel Mine"]
-- DESCRIPTION: [PICKUP_RANGE=90(1,100)]
-- DESCRIPTION: [TRIGGER_DISTANCE=80(1,300)]
-- DESCRIPTION: [ENEMY_HEAR_DISTANCE=1000(0,3000)]
-- DESCRIPTION: [PICKUP_TEXT$="Press E To pick up mine"]
-- DESCRIPTION: [PLACING_TEXT$="Press R To place mine"]
-- DESCRIPTION: [ARMING_TEXT$="Mine is primed/armed"]
-- DESCRIPTION: [ARMING_DELAY=3(1,5)] seconds
-- DESCRIPTION: [@HARM_PLAYER=1(1=No, 2=Yes)]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\pickup.png"]
-- DESCRIPTION: <Sound0> Pickup Sound
-- DESCRIPTION: <Sound1> Primed/armed sound.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

g_mines = {}
local rad = math.rad
local mine = {}
local weapon_name = {}
local pickup_range = {}
local trigger_distance = {}
local enemy_hear_distance = {}
local pickup_text = {}
local placing_text = {}
local arming_text = {}
local harm_player = {}
local arming_delay = {}
local prompt_display = {}
local item_highlight = {}
local highlight_icon = {}

local status = {}
local hl_icon = {}
local hl_imgwidth = {}
local hl_imgheight = {}
local pressed = {}
local mine_state = {}
local mine_armed = {}
local prime_time = {}
local tEnt = {}
local currentY = {}
local enemy_distance = {}
local heard_time = {}
local surfacey = {}
local finaly = {}

function mines_properties(e, weapon_name, pickup_range, trigger_distance, enemy_hear_distance, pickup_text, placing_text, arming_text, arming_delay, harm_player, prompt_display, item_highlight, highlight_icon_imagefile)
	mine[e] = g_Entity[e]
	mine[e].weapon_name = weapon_name
	mine[e].pickup_range = pickup_range
	mine[e].trigger_distance = trigger_distance
	mine[e].enemy_hear_distance = enemy_hear_distance
	mine[e].pickup_text = pickup_text
	mine[e].placing_text = placing_text
	mine[e].arming_text = arming_text
	mine[e].arming_delay = arming_delay
	mine[e].harm_player = harm_player
	mine[e].prompt_display = prompt_display
	mine[e].item_highlight = item_highlight
	mine[e].highlight_icon = highlight_icon_imagefile
end

function mines_init(e)
	mine[e] = g_Entity[e]
	mine[e].weapon_name = "Anti-Personel Mine"
	mine[e].pickup_range = 50
	mine[e].trigger_distance = 80
	mine[e].enemy_hear_distance = 0
	mine[e].pickup_text = "Press E To pick up mine"
	mine[e].placing_text = "Press R To place mine"
	mine[e].arming_text = "Mine is primed/armed"
	mine[e].arming_delay = 3
	mine[e].harm_player = 1
	mine[e].prompt_display = 1
	mine[e].item_highlight = 0
	mine[e].highlight_icon = "imagebank\\icons\\pickup.png"
	pressed[e] = 0
	mine_state[e] = "collect"
	mine_armed[e] = 0
	status[e] = "init"
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0
	tEnt[e] = 0	
	g_mines = 0
	currentY[e] = 0
	prime_time[e] = 0
	heard_time[e] = 0
	surfacey[e] = 0
	finaly[e] =	0
end

function mines_main(e)
	mine[e] = g_Entity[e]
	if status[e] == "init" then
		if mine[e].item_highlight == 3 and mine[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(mine[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(mine[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(mine[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end	
		mine_state[e] = "collect"
		status[e] = "init_end"
	end
	
	PlayerDist = GetPlayerDistance(e)
	
	if PlayerDist < mine[e].pickup_range then
		--pinpoint select object--
		module_misclib.pinpoint(e,mine[e].pickup_range,mine[e].item_highlight,hl_icon[e])
		tEnt[e] = g_tEnt
		--end pinpoint select object--
		if mine_state[e] == "collect" and g_mines == 0 then
			if PlayerDist < mine[e].pickup_range and tEnt[e] == e and GetEntityVisibility(e) == 1 then
				if mine[e].prompt_display == 1 then PromptLocal(e,mine[e].pickup_text) end
				if mine[e].prompt_display == 2 then Prompt(mine[e].pickup_text) end
				if GetInKey() == "e" or GetInKey() == "E" and pressed[e] == 0 then
					PromptDuration("Collected the " ..mine[e].weapon_name,3000)
					pressed[e] = 1
					PlaySound(e,0)
					mine_state[e] = "collected"
					currentY[e] = g_Entity[e]['y']
					g_mines = 1										
					CollisionOff(e)
				end
			end
		end
	end	

	if mine_state[e] == "collected" and g_mines == 1 then		
		Prompt(mine[e].placing_text)
		local ox,oy,oz = U.Rotate3D(0,currentY[e],mine[e].pickup_range, math.rad(g_PlayerAngX), math.rad(g_PlayerAngY), math.rad(g_PlayerAngZ))
		local forwardposx, forwardposy, forwardposz = g_PlayerPosX + ox, g_PlayerPosY + oy, g_PlayerPosZ + oz
		surfacey[e] = GetSurfaceHeight(forwardposx,forwardposy,forwardposz)
		finaly[e] = surfacey[e]		
		SetPosition(e,forwardposx, finaly[e]+3, forwardposz)
		SetRotation(e,g_Entity[e]['anglex'],g_PlayerAngY,g_Entity[e]['anglez'])
		if GetInKey() == "r" or GetInKey() == "R" and pressed[e] == 1 then
			CollisionOn(e)
			mine_armed[e] = 0
			mine_state[e] = "placed"
			g_mines = 0
			Show(e)
			pressed[e] = 2
		end
	end
	if mine_state[e] == "placed" then
		if mine[e].prompt_display == 1 then PromptDuration(mine[e].arming_text,3000) end
		if mine[e].prompt_display == 2 then PromptDuration(mine[e].arming_text,3000) end
		if prime_time[e] == 0 then
			prime_time[e] = g_Time + (mine[e].arming_delay*1000)			
			PlaySound(e,1)
		end
		if g_Time > prime_time[e] then			
			mine_armed[e] = 1			
			SetAnimationName(e,"on")
			PlayAnimation(e)
			mine_state[e] = "primed"
			prime_time[e] = 0
		end
	end
	if mine_state[e] == "primed" then
		if mine_armed[e] == 1 then
			for a = 1, g_EntityElementMax do
				if g_Entity[a] ~= nil and g_Entity[a]['health'] > 0 then
					local allegiance = GetEntityAllegiance(a) -- get the allegiance value for this entity (0-enemy, 1-ally, 2-neutral)	
					if allegiance == 0 or allegiance == 2 or allegiance == 3 then
						MineDX = g_Entity[a]['x'] - g_Entity[e]['x']
						MineDY = g_Entity[a]['y'] - g_Entity[e]['y']
						MineDZ = g_Entity[a]['z'] - g_Entity[e]['z']
						enemy_distance[e] = math.sqrt(math.abs(MineDX*MineDX)+math.abs(MineDY*MineDY)+math.abs(MineDZ*MineDZ))				
						if enemy_distance[e] < mine[e].trigger_distance and mine_armed[e] == 1 then
							SetEntityHealth(e,0)
							SetEntityHealth(a,0)
							heard_time[e] = g_Time + 5000
							mine_state[e] = "exploded"
							CollisionOff(e)
							Hide(e)
						end
					end	
				end
			end
		end
	end
	if mine_state[e] == "exploded" then
		if g_Time < heard_time[e] then MakeAISound(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,mine[e].enemy_hear_distance,1,e) end -- other nearby enemies hear explosion
		if g_Time > heard_time[e] then Destroy(e) end
	end		
end
