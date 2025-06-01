-- Crosshair v12 by Necrym59
-- DESCRIPTION: Will allow for an adaptive crosshair/hitmarker in gameplay. 
-- DESCRIPTION: Apply to an object, set Always Active ON
-- DESCRIPTION: [VIEW_RANGE=1000(0,3000)]
-- DESCRIPTION: [CROSSHAIR_IMAGEFILE$="imagebank\\crosshairs\\crosshair.dds"]
-- DESCRIPTION: [HITMARKER_IMAGEFILE$="imagebank\\crosshairs\\hitmarker.dds"]
-- DESCRIPTION: [HIGHLIGHTER!=0] If on enables active highlighting crosshairs 
-- DESCRIPTION: [WEAPON_ACTIVE!=0] Only shows when using weapon
-- DESCRIPTION: [ZOOM_HIDE!=0] Hides crosshair when weapon zoomed

local U = require "scriptbank\\utillib"
local crosshairs 			= {}
local view_range 			= {}
local crosshair_imagefile 	= {}
local hitmarker_imagefile 	= {}
local crosshair 			= {}
local hitmarker 			= {}
local highlighter			= {}
local weapon_active			= {}
local zoom_hide				= {}

local sp_crosshair	= {}
local sp_hitmarker	= {}
local sp_imgwidth1	= {}
local sp_imgheight1	= {}
local sp_imgwidth2	= {}
local sp_imgheight2	= {}
local hitfader		= {}
local fadetime		= {}

local entrange		= {}
local noshoot		= {}
local pEnt 			= {}
local cEnt			= {}
local cHit			= {}
local status 		= {}
local tableName		= {}
local checktimer	= {}

function crosshair_properties(e, view_range, crosshair_imagefile, hitmarker_imagefile, highlighter, weapon_active, zoom_hide)
	crosshairs[e].view_range = view_range
	crosshairs[e].crosshair = crosshair_imagefile
	crosshairs[e].hitmarker = hitmarker_imagefile
	crosshairs[e].highlighter = highlighter or 0
	crosshairs[e].weapon_active = weapon_active or 0
	crosshairs[e].zoom_hide = zoom_hide or 0
end

function crosshair_init(e)
	crosshairs[e] = {}
	crosshairs[e].view_range = 1000
	crosshairs[e].crosshair = ""
	crosshairs[e].hitmarker = ""
	crosshairs[e].highlighter = 0
	crosshairs[e].weapon_active = 0	
	crosshairs[e].zoom_hide = 0	

	hitfader[e] = 0
	fadetime[e] = 0
	entrange[e] = 0
	pEnt[e] = 0
	cEnt[e] = 0
	cHit[e] = 0
	noshoot[e]= 0
	checktimer[e] =	math.huge
	tableName[e] = "croshairlist" ..tostring(e)
	_G[tableName[e]] = {}	
	status[e] = "init"
end

function crosshair_main(e)

	if status[e] == "init" then
		sp_crosshair = CreateSprite(LoadImage(crosshairs[e].crosshair))
		sp_imgwidth1 = GetImageWidth(LoadImage(crosshairs[e].crosshair))
		sp_imgheight1 = GetImageHeight(LoadImage(crosshairs[e].crosshair))
		SetSpriteSize(sp_crosshair,-1,-1)
		SetSpriteDepth(sp_crosshair,100)
		SetSpriteOffset(sp_crosshair,sp_imgwidth1/2.0, sp_imgheight1/2.0)
		SetSpriteColor(sp_crosshair,0,0,0,0)

		sp_hitmarker = CreateSprite(LoadImage(crosshairs[e].hitmarker))
		sp_imgwidth2 = GetImageWidth(LoadImage(crosshairs[e].hitmarker))
		sp_imgheight2 = GetImageHeight(LoadImage(crosshairs[e].hitmarker))
		SetSpriteSize(sp_hitmarker,-1,-1)
		SetSpriteDepth(sp_hitmarker,100)
		SetSpriteOffset(sp_hitmarker,sp_imgwidth2/2.0, sp_imgheight2/2.0)
		SetSpriteColor(sp_hitmarker,255,255,255,255)
		
		for n = 1, g_EntityElementMax do
			if n ~= nil and g_Entity[n] ~= nil then
				if GetEntityAllegiance(n) ~= -1 then
					table.insert(_G[tableName[e]],n)
				end
			end
		end	
		
		checktimer[e] = g_Time + 150
		status[e] = "mouseplay"
	end
	
	if status[e] == "mouseplay" then
		WeaponID = GetPlayerWeaponID()		
		noshoot[e] = GetWeaponReloadQuantity(WeaponID,0)
		if noshoot[e] > 0 then
			if crosshairs[e].weapon_active == 1 and WeaponID == 0 then
				PasteSpritePosition(sp_crosshair,500,500)
				PasteSpritePosition(sp_hitmarker,500,500)
			else
				if crosshairs[e].zoom_hide == 1 and g_MouseClick == 2 then
					PasteSpritePosition(sp_crosshair,500,500)
					SetSpriteColor(sp_crosshair,0,0,0,0)
					SetSpriteColor(sp_hitmarker,0,0,0,0)
				else
					PasteSpritePosition(sp_crosshair,50,50)
					PasteSpritePosition(sp_hitmarker,50,50)
				end
			end
			if crosshairs[e].highlighter == 0 then
				SetSpriteColor(sp_crosshair,255,255,255,100)
				cEnt[e] = 1
			end
			if crosshairs[e].highlighter == 1 then
				if g_Time > checktimer[e] then
					pEnt[e] = 0
					for _,v in pairs (_G[tableName[e]]) do
						if g_Entity[v] ~= nil then
							entrange[e] = math.ceil(GetFlatDistanceToPlayer(v))
							if g_Entity[v]["health"] > 0 and entrange[e] < crosshairs[e].view_range and U.PlayerLookingAt(v,crosshairs[e].view_range) then
								pEnt[e] = v
								if GetEntityAllegiance(pEnt[e]) == 0 then SetSpriteColor(sp_crosshair,255,0,0,255) end
								if GetEntityAllegiance(pEnt[e]) == 1 then SetSpriteColor(sp_crosshair,0,255,0,255) end
								if GetEntityAllegiance(pEnt[e]) == 2 then SetSpriteColor(sp_crosshair,0,255,255,255) end
								cEnt[e] = 1
								break
							end	
						end
					end
					if pEnt[e] == 0 then SetSpriteColor(sp_crosshair,255,255,255,100) end				
					checktimer[e] = g_Time + 100
				end
			end	
			fadetime[e] = GetTimeElapsed()
			hitfader[e] = math.max(0,hitfader[e] - (255/3) * fadetime[e])
			if GetGamePlayerStateFiringMode() == 1 and cEnt[e] == 1 then
				cHit[e] = 1
			else
				cHit[e] = 0
			end	
			if cHit[e] == 1 then
				hitfader[e] = 255			
				SetSpriteColor(sp_hitmarker,255,255,255,hitfader[e])			
			else
				SetSpriteColor(sp_hitmarker,0,0,0,0)
				cEnt[e] = 0
			end
		end
	end	
end

function GetFlatDistanceToPlayer(v)
	if g_Entity[v] ~= nil then
		local distDX = g_PlayerPosX - g_Entity[v]['x']
		local distDZ = g_PlayerPosZ - g_Entity[v]['z']
		return math.sqrt((distDX*distDX)+(distDZ*distDZ));
	end
end