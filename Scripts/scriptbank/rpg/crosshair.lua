-- Crosshair v7 by Necrym59
-- DESCRIPTION: Will allow for an adaptive crosshair/hitmarker in gameplay. 
-- DESCRIPTION: Apply to an object, set Always Active ON
-- DESCRIPTION: [VIEW_RANGE=1000(0,3000)]
-- DESCRIPTION: [CROSSHAIR_IMAGEFILE$="imagebank\\crosshairs\\crosshair.dds"]
-- DESCRIPTION: [CROSSHAIR_IMAGEFILE$="imagebank\\crosshairs\\hitmarker.dds"]
-- DESCRIPTION: [HIGHLIGHTER!=0] If on enables active highlighting crosshairs 

local U = require "scriptbank\\utillib"
local P = require "scriptbank\\physlib"

local crosshairs 			= {}
local view_range 			= {}
local crosshair_imagefile 	= {}
local crosshair_imagefile 	= {}
local crosshair 			= {}
local hitmarker 			= {}
local highlighter			= {}

local sp_crosshair	= {}
local sp_hitmarker	= {}
local sp_imgwidth1	= {}
local sp_imgheight1	= {}
local sp_imgwidth2	= {}
local sp_imgheight2	= {}
local hitfader		= {}
local fadetime		= {}

local selectobj 	= {}
local pEnt 			= {}
local cEnt			= {}
local cHit			= {}
local status 		= {}
local checktimer	= {}

function crosshair_properties(e, view_range, crosshair_imagefile, hitmarker_imagefile, highlighter)
	crosshairs[e].view_range = view_range
	crosshairs[e].crosshair = crosshair_imagefile
	crosshairs[e].hitmarker = hitmarker_imagefile
	crosshairs[e].highlighter = highlighter
end

function crosshair_init(e)
	crosshairs[e] = {}
	crosshairs[e].view_range = 1000
	crosshairs[e].crosshair = ""
	crosshairs[e].hitmarker = ""
	crosshairs[e].highlighter = 0	

	hitfader[e] = 0
	fadetime[e] = 0
	selectobj[e] = 0
	pEnt[e] = 0
	cEnt[e] = 0
	cHit[e] = 0
	checktimer[e] =	math.huge
	status[e] = "init"
end

function crosshair_main(e)

	if status[e] == "init" then
		sp_crosshair = CreateSprite(LoadImage(crosshairs[e].crosshair))
		sp_imgwidth1 = GetImageWidth(LoadImage(crosshairs[e].crosshair))
		sp_imgheight1 = GetImageHeight(LoadImage(crosshairs[e].crosshair))
		SetSpriteSize(sp_crosshair,-1,-1)
		SetSpriteDepth(sp_crosshair,100)
		SetSpritePosition(sp_crosshair,50,50)
		SetSpriteOffset(sp_crosshair,sp_imgwidth1/2.0, sp_imgheight1/2.0)
		SetSpriteColor(sp_crosshair,0,0,0,0)

		sp_hitmarker = CreateSprite(LoadImage(crosshairs[e].hitmarker))
		sp_imgwidth2 = GetImageWidth(LoadImage(crosshairs[e].hitmarker))
		sp_imgheight2 = GetImageHeight(LoadImage(crosshairs[e].hitmarker))
		SetSpriteSize(sp_hitmarker,-1,-1)
		SetSpriteDepth(sp_hitmarker,100)
		SetSpritePosition(sp_hitmarker,50,50)
		SetSpriteOffset(sp_hitmarker,sp_imgwidth2/2.0, sp_imgheight2/2.0)
		SetSpriteColor(sp_hitmarker,255,255,255,255)
		
		checktimer[e] = g_Time + 150
		status[e] = "mouseplay"
	end

	if status[e] == "mouseplay" then
	
		if crosshairs[e].highlighter == 0 then
			SetSpriteColor(sp_crosshair,255,255,255,100)
			cEnt[e] = 1
		end
		if crosshairs[e].highlighter == 1 then
			if g_Time > checktimer[e] then
				selectobj[e] = U.ObjectPlayerLookingAt(crosshairs[e].view_range)
				if selectobj[e] == 0 then SetSpriteColor(sp_crosshair,255,255,255,100) end
				pEnt[e] = P.ObjectToEntity(selectobj[e])				
				if pEnt[e] and g_Entity[pEnt[e]] and g_Entity[pEnt[e]]["health"] > 0 then
					if GetEntityAllegiance(pEnt[e]) == 0 then SetSpriteColor(sp_crosshair,255,0,0,255) end
					if GetEntityAllegiance(pEnt[e]) == 1 then SetSpriteColor(sp_crosshair,0,255,0,255) end
					if GetEntityAllegiance(pEnt[e]) == 2 then SetSpriteColor(sp_crosshair,0,255,255,255) end
					cEnt[e] = 1
				end	
				checktimer[e] = g_Time + 150
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
