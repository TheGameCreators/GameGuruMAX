-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Healthbar v8 - by Necrym,59
-- DESCRIPTION: A global behavior that will display a viewed enemys health in a bar or text, set Always Active.
-- DESCRIPTION: [DISPLAY_RANGE=200(100,1000)]
-- DESCRIPTION: [@DISPLAY_MODE=1(1=Bar, 2=Text, 3=Text+Bar)]
-- DESCRIPTION: [Y_ADJUSTMENT=10(0,50)]
-- DESCRIPTION: [HEALTH_TEXT$="Health:"]
-- DESCRIPTION: [HEALTH_BAR_IMAGEFILE$="imagebank\\buttons\\slider-bar-full.png"]
-- DESCRIPTION: [HEALTH_COLOR_CHANGE=150(1,1000)]

local P = require "scriptbank\\physlib"
g_LegacyNPC = {}
local healthbar = {}
local display_range = {}
local display_mode = {}
local y_adjustment = {}
local health_text = {}
local health_bar = {}
local health_color_change = {}

local rotheight	= {}
local status = {}
local hbarsize = {}
local hreadout = {}
local hbarsprite = {}
local tableName = {}
local checktimer = {}
local entrange = {}
local enemies = {}

function healthbar_properties(e, display_range, display_mode, y_adjustment, health_text,health_bar, health_color_change)
	healthbar[e].display_range = display_range or 500
	healthbar[e].display_mode = display_mode or 1
	healthbar[e].y_adjustment = y_adjustment
	healthbar[e].health_text = health_text
	healthbar[e].health_bar = health_bar
	healthbar[e].health_color_change = health_color_change
end

function healthbar_init(e)
	healthbar[e] = {}
	healthbar[e].display_range = 500
	healthbar[e].display_mode = 1
	healthbar[e].y_adjustment = 0
	healthbar[e].health_text = "Health:"
	healthbar[e].health_bar = "imagebank\\buttons\\slider-bar-full.png"
	healthbar[e].health_color_change = 100

	status[e] = "init"
	rotheight[e] = 0
	hbarsize[e] = 0
	hreadout[e] = 0
	hbarsprite[e] = 0
	g_LegacyNPC = 0
	enemies[e] = 0
	checktimer[e] =	math.huge
	tableName[e] = "hbenemies" ..tostring(e)
	_G[tableName[e]] = {}
	entrange[e] = 0
	SetEntityAlwaysActive(e,1)
end

function healthbar_main(e)

	if status[e] == "init" then
		if healthbar[e].health_bar ~= "" then
			hbarsprite[e] = CreateSprite(LoadImage(healthbar[e].health_bar))
			SetSpriteSize(hbarsprite[e],-1,-1)
			SetSpritePosition(hbarsprite[e],200,200)
		end
		for n = 1, g_EntityElementMax do
			if n ~= nil and g_Entity[n] ~= nil then
				if GetEntityAllegiance(n) == 0 then
					table.insert(_G[tableName[e]],n)
					enemies[e] = enemies[e]+1
				end
			end
		end
		checktimer[e] = g_Time + 2
		status[e] = "active"
	end

	if status[e] == "active" then

		if g_Time > checktimer[e] then
			for _,a in pairs (_G[tableName[e]]) do
				if g_Entity[a] ~= nil then
					entrange[e] = math.ceil(GetFlatDistanceToPlayer(a))
					if g_Entity[a]["health"] > 0 and entrange[e] < healthbar[e].display_range then
						--Entity dimensions check--
						Ent = g_Entity[a]
						local dims = P.GetObjectDimensions(Ent.obj)
						rotheight[e] = (dims.h + healthbar[e].y_adjustment)
						--3dto2d check--
						ScreenPosX = -1
						ScreenPosX,ScreenPosY = Convert3DTo2D(g_Entity[a]['x'],g_Entity[a]['y']+rotheight[e],g_Entity[a]['z'])
						if ScreenPosX < 0 then
							ScreenPosX = 0
							ScreenPosY = 0
						else
							percentx,percenty = ScreenCoordsToPercent(ScreenPosX,ScreenPosY)
						end
						--Health and Healthbar check--
						if g_LegacyNPC == 0 then hreadout[e] = g_Entity[a]['health'] end
						if g_LegacyNPC == 1 then hreadout[e] = g_Entity[a]['health']-1000 end
						if g_Entity[a]['health'] < 9000 then
							hbarsize[e] = hreadout[e]/200
							SetSpriteSize(hbarsprite[e],hbarsize[e],3)
							if hreadout[e] > healthbar[e].health_color_change then SetSpriteColor(hbarsprite[e],0,255,0,255) end
							if hreadout[e] < healthbar[e].health_color_change then SetSpriteColor(hbarsprite[e],255,0,0,255) end
						end
						--Display Healthbar and Health--
						if healthbar[e].display_mode == 1 and hreadout[e] > 0 then
							PasteSpritePosition(hbarsprite[e],percentx-(hbarsize[e]/2),percenty)
						end
						if healthbar[e].display_mode == 2 and hreadout[e] > 0 then
							TextCenterOnX(percentx,percenty,1,healthbar[e].health_text.. " " ..hreadout[e])
						end
						if healthbar[e].display_mode == 3 and hreadout[e] > 0 then
							PasteSpritePosition(hbarsprite[e],percentx-(hbarsize[e]/2),percenty)
							TextCenterOnX(percentx,percenty,1,"")
							TextCenterOnX(percentx,percenty,1,healthbar[e].health_text.. " " ..hreadout[e])
						end
						if g_LegacyNPC == 1 and g_Entity[a]['health'] < 1000 then
							g_LegacyNPC = 0
						end
					end
				end
			end
			--Destroy Dead Entities check--
			for _,a in pairs (_G[tableName[e]]) do
				if g_Entity[a] ~= nil then
					if g_Entity[a]['health'] <= 0 then
						Destroy(a)
						table.remove(_G[tableName[e]], tableFind(_G[tableName[e]],a))
					end
				end
			end
			checktimer[e] = g_Time + 2
		end
	end
end

function tableFind(tbl, value)
    for key, val in pairs(tbl) do
        if val == value then
            return key
        end
    end
    return nil
end

function GetFlatDistanceToPlayer(v)
	if g_Entity[v] ~= nil then
		local distDX = g_PlayerPosX - g_Entity[v]['x']
		local distDZ = g_PlayerPosZ - g_Entity[v]['z']
		return math.sqrt((distDX*distDX)+(distDZ*distDZ));
	end
end