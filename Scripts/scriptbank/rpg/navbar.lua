-- Navigation Bar v10 by Smallg and Necrym59
-- DESCRIPTION: Adds a Nagigation Bar to your game 
-- DESCRIPTION: [IMAGEFILE$="imagebank\\navbar\\navbar.png"]
-- DESCRIPTION: [VIEWRANGE=6000] of objectives shown on the radar
-- DESCRIPTION: [WIDTH=15(1,100)] of the navigation bar on screen 
-- DESCRIPTION: [HEIGHT=5(1,100)] of the navigation bar on screen
-- DESCRIPTION: [POSITION_X=50(0,100)] position of the navigation bar on screen
-- DESCRIPTION: [POSITION_Y=15(0,100)] position of the navigation bar on screen
-- DESCRIPTION: [#ICON_WIDTH=1.0(0.1,100.0)] of the icon on the navigation bar
-- DESCRIPTION: [#ICON_HEIGHT=3.0(0.1,100.0)] of the icon on the navigation bar
-- DESCRIPTION: [#ICON_POSITION_Y=-0.5(-5.0,5.0)] of the icon on the navigation bar
-- DESCRIPTION: [@COMPASS_MODE=1(1=On ,2=Off)] compass on/off
-- DESCRIPTION: [#COMPASS_Y=0.0(-2.0,2.0)] compass height adjustment
-- DESCRIPTION: [#COMPASS_VIEW=91(91,182)] compass view adjustment
-- DESCRIPTION: [COMPASS_R=255(0,255)] compass red color value
-- DESCRIPTION: [COMPASS_G=255(0,255)] compass green color value
-- DESCRIPTION: [COMPASS_B=255(0,255)] compass blue color value
-- DESCRIPTION: [COMPASS_FONT_SIZE=2(1,5)] compass font size
-- DESCRIPTION: [@OBJECTIVE_DATA=1(1=On,2=Off)] objective data on/off
-- DESCRIPTION: [COMPASS_SPACER$="."] compass spacer character

U = require "scriptbank\\utillib"

g_navigation_bar = {}
nbar = 0

local iconimage			= {}
local iconsprite		= {}
local navbarsprite		= {}
local navbar_entity		= {}
local isfixedsize		= {}
local ignorerange		= {}
local thisname			= {}
local icon_position_y	= {}
local compass_mode		= {}
local compass_y			= {}
local compass_view		= {}
local compass_r			= {}
local compass_g			= {}
local compass_b			= {}
local compass_font_size = {}
local objective_data	= {}
local compass_spacer 	= {}

local compass			= {}
local questentity 		= {}
local queststatus 		= {}
local status			= ""

function navbar_properties(e, navbarimage, viewrange, width, height, position_x, position_y, icon_width, icon_height, icon_position_y, compass_mode, compass_y, compass_view, compass_r, compass_g, compass_b, compass_font_size, objective_data, compass_spacer)
	nbar.navbarimage = navbarimage or imagefile
	nbar.viewrange = viewrange
	nbar.width = width
	nbar.height = height
	nbar.position_x = position_x
	nbar.position_y = position_y
	nbar.icon_width = icon_width
	nbar.icon_height = icon_height
	nbar.icon_position_y = icon_position_y
	nbar.compass_mode = compass_mode		
	nbar.compass_y = compass_y
	nbar.compass_view = compass_view
	nbar.compass_r = compass_r
	nbar.compass_g = compass_g
	nbar.compass_b = compass_b
	nbar.compass_font_size = compass_font_size
	nbar.objective_data = objective_data
	nbar.compass_spacer = compass_spacer
end 

function navbar_init(e)
	g_navigation_bar[e] = {}
	nbar = g_navigation_bar[e]
	nbar.navbarimage = ""
	nbar.viewrange = 6000
	nbar.width = 80
	nbar.height = 5
	nbar.position_x = 50
	nbar.position_y = 5
	nbar.icon_width = 5
	nbar.icon_height = 5
	nbar.icon_position_y = -0.5
	nbar.compass_mode = 1		
	nbar.compass_y = 0.5
	nbar.compass_view = 91
	nbar.compass_r = 255
	nbar.compass_g = 255
	nbar.compass_b = 255
	nbar.compass_font_size = 2
	nbar.objective_data = 1
	nbar.compass_spacer = "."	
	queststatus[e] = 0
	questentity[e] = 0
	status = "init"
end 

function navbar_main(e)
	if status == "init" then		
		navbarsprite = CreateSprite(LoadImage(nbar.navbarimage))
		SetSpriteSize(navbarsprite,nbar.width,nbar.height)
		SetSpritePosition(navbarsprite,20000,20000)
		SetSpriteOffset(navbarsprite,nbar.width/2,nbar.height/2)
		if nbar.compass_mode == 1 then defn_Compass() end
		status = "endinit"
	end
	
	local function PointAtPlayer(i)
		local x = g_PlayerPosX - g_Entity[i]['x']
		local z = g_PlayerPosZ - g_Entity[i]['z']
		local angle = math.atan2( z , x )
		angle = angle + (-90.0*0.0174533)
		return angle
	end
	
	PasteSpritePosition(navbarsprite,nbar.position_x,nbar.position_y)
	if nbar.compass_mode == 1 then show_Compass() end
	
	if g_UserGlobalQuestTitleActiveE > 0 and queststatus[e] == 0 then
		AddToNavbar(g_UserGlobalQuestTitleActiveE, "imagebank\\navbar\\currentquest.png", fixedsize, ignorerange, "")
		questentity[e] = g_UserGlobalQuestTitleActiveE
		queststatus[e] = 1
	end
	if g_UserGlobalQuestTitleActiveE == 0 and queststatus[e] == 1 then
		RemoveFromNavbar(questentity[e])
		questentity[e] = 0
		queststatus[e] = 0
	end
	
	for a,b in pairs(navbar_entity) do		
		local dist = GetPlayerDistance(b)
		if dist < nbar.viewrange or ignorerange[b] == 1 then 	
			local angle = PointAtPlayer(b) + math.rad(g_PlayerAngY)
			local anglex = g_PlayerAngY - GetAngleFromPointToPoint(g_PlayerPosX,g_PlayerPosZ, g_Entity[b]['x'],g_Entity[b]['z'])	
			if anglex > 180 then 
				anglex = anglex - 360
			elseif anglex < -180 then 
				anglex = anglex + 360
			end
			local maxrange = nbar.viewrange
			if ignorerange[b] == 1 then 
				dist = 0.999 
				maxrange = 1
			end
			local distx = dist / maxrange * 0.01
			local iconx = (distx + anglex) * -1		
			iconx = iconx + nbar.position_x
			local icony = nbar.position_y-1.5
			if iconx - nbar.icon_width/2 > nbar.position_x - nbar.width/2 and iconx + nbar.icon_width/2 < nbar.position_x + nbar.width/2 then 
				if isfixedsize[b] == 0 then 
					local distperc = 1.05 - dist / maxrange
					local iw = nbar.icon_width * distperc
					local ih = nbar.icon_height * distperc
					SetSpriteSize(iconsprite[b], iw,ih)
					SetSpriteOffset(iconsprite[b], iw/2, ih/2)
				end
				PasteSpritePosition(iconsprite[b],iconx,icony+nbar.icon_position_y)
				if nbar.objective_data == 1 then 
					local infoposy = nbar.icon_position_y+2.5
					TextCenterOnX(iconx,icony+infoposy,2,thisname[b])
					if g_Entity[b]['y'] > g_PlayerPosY then TextCenterOnX(iconx,icony+infoposy+1,2,math.floor(dist).. " +") end
					if g_Entity[b]['y'] < g_PlayerPosY then TextCenterOnX(iconx,icony+infoposy+1,2,math.floor(dist).. " -") end
				end
			end
		end
	end
end 

function AddToNavbar(ee, imgname, fixsize, ignorerng, tname)
	if navbar_entity[ee] == nil then 
		local img = LoadImage(imgname)
		local spr = CreateSprite(img)
		iconimage[ee] = img
		iconsprite[ee] = spr
		SetSpriteSize(spr,nbar.icon_width, nbar.icon_height)
		SetSpriteOffset(spr, nbar.icon_width/2, nbar.icon_height/2)
		SetSpritePosition(spr,20000,20000)
		navbar_entity[ee] = ee
		isfixedsize[ee] = fixsize
		ignorerange[ee] = ignorerng
		thisname[ee] = tname
		return true
	else 
		return false 
	end 
end

function RemoveFromNavbar(ee)
	if navbar_entity[ee] ~= nil then 
		DeleteSprite(iconsprite[ee])
		DeleteImage(iconimage[ee])
		navbar_entity[ee] = nil
	end
end

function GetAngleFromPointToPoint(x1,z1, x2,z2)
	local destx = x2 - x1
	local destz = z2 - z1
	local angle = math.atan2(destx,destz)
	angle = angle * (180.0 / math.pi)
	if angle < 0 then
		angle = 360 + angle
	elseif angle > 360 then
		angle = angle - 360
	end
	return angle
end

function show_Compass()	
	local start=0
	local compass_str=""
	local bearing=0
	bearing  = math.floor(math.fmod(g_PlayerAngY, 360))	
	if bearing < 0 then bearing=bearing+360 end	
 	if bearing < 45 then
		start = 315 + bearing
	else
		start=bearing-45
	end
	local arraypos=start    
	for i=1,nbar.compass_view, 1 do
		if arraypos==360 or arraypos<=0 then
			arraypos=1
		else  
			arraypos=arraypos+1
		end
		compass_str=compass_str..compass[arraypos]
	end	
	TextCenterOnXColor(nbar.position_x,nbar.position_y+nbar.compass_y,nbar.compass_font_size,compass_str,nbar.compass_r,nbar.compass_g,nbar.compass_b)
end

function defn_Compass()  
	for i=1, 360, 1 do
		if i == 1 then compass[i] = "N" 
		elseif i == 46 then compass[i] = "NE"
		elseif i == 91 then compass[i] = "E"
		elseif i == 136 then compass[i] = "SE"
		elseif i == 181 then compass[i] = "S"
		elseif i == 226 then compass[i] = "SW"
		elseif i == 271 then compass[i] = "W"  
		elseif i == 316 then compass[i] = "NW"
		elseif math.fmod(i,10) == 1 then
			compass[i] = nbar.compass_spacer
		else
			compass[i] = " "
		end
	end    
end

		