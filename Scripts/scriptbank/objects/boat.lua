-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Boat v21 by Necrym59
-- DESCRIPTION: Creates a rideable boat object behavior: Set Physics=ON, Gravity=OFF, IsImobile=YES.
-- DESCRIPTION: [PROMPT_TEXT$="E to embark"]
-- DESCRIPTION: [USE_RANGE=80]
-- DESCRIPTION: [MIN_SPEED=2(0,10)]
-- DESCRIPTION: [MAX_SPEED=4(0,100)]
-- DESCRIPTION: [TURN_SPEED=2(1,10)]
-- DESCRIPTION: [DRAG=5]
-- DESCRIPTION: [#PLAYER_Z_POSITION=0(-100,100)]
-- DESCRIPTION: [#PLAYER_Y_POSITION=20(-100,100)]
-- DESCRIPTION: [BOAT_ROTATION=0(0,180)]
-- DESCRIPTION: [BOAT_DRAFT=30]
-- DESCRIPTION: [BOAT_POWER=5(0,50)]
-- DESCRIPTION: [BOAT_BRAKE=5(0,50)]
-- DESCRIPTION: [BOAT_BUOYANCY=1(0,5)]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\hand.png"]
-- DESCRIPTION: <Sound0> Entry/Exit 
-- DESCRIPTION: <Sound1> Moving Loop
-- DESCRIPTION: <Sound2> Run Aground

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local boat 				= {}
local prompt_text 		= {}
local use_range			= {}
local min_speed			= {}
local max_speed			= {}
local turn_speed		= {}
local drag				= {}
local player_z_position = {}
local player_y_position = {}
local boat_rotation		= {}
local boat_draft		= {}
local boat_power		= {}
local boat_brake		= {}
local boat_buoyancy		= {}
local prompt_display	= {}
local item_highlight 	= {}
local highlight_icon	= {}

local playerdistance	= {}
local boat_active = {}
local boat_release = {}
local boat_aground = {}
local onboatcheck  = {}
local floatangle = {}
local colobj = {}
local boatlength = {}
local heightTerrain = {}
local heightSurface = {}
local startheight = {}
local boatx = {}
local boaty = {}
local boatz = {}
local speed = {}
local wobble = {}
local status = {}
local tEnt = {}
local hl_icon = {}
local hl_imgwidth = {}
local hl_imgheight = {}

function boat_properties(e, prompt_text, use_range, min_speed, max_speed, turn_speed, drag, player_z_position, player_y_position, boat_rotation, boat_draft, boat_power, boat_brake, boat_buoyancy, prompt_display, item_highlight, highlight_icon_imagefile)
	boat[e].prompt_text = prompt_text
	boat[e].use_range = use_range
	boat[e].min_speed = min_speed
	boat[e].max_speed = max_speed
	boat[e].turn_speed = turn_speed
	boat[e].drag = drag
	boat[e].player_z_position = player_z_position
	boat[e].player_y_position = player_y_position
	boat[e].boat_rotation = boat_rotation
	boat[e].boat_draft = boat_draft
	boat[e].boat_power = boat_power
	boat[e].boat_brake = boat_brake
	boat[e].boat_buoyancy = boat_buoyancy or 1
	boat[e].prompt_display = prompt_display
	boat[e].item_highlight = item_highlight
	boat[e].highlight_icon = highlight_icon_imagefile		
end

function boat_init(e)
	boat[e] = {}
	boat[e].prompt_text = "E to embark"
	boat[e].use_range = 80
	boat[e].min_speed = 2
	boat[e].max_speed = 4
	boat[e].turn_speed = 2
	boat[e].drag = 5
	boat[e].player_z_position = 0
	boat[e].player_y_position = 10
	boat[e].boat_rotation = 0
	boat[e].boat_draft = 30
	boat[e].boat_power = 5
	boat[e].boat_brake = 5
	boat[e].boat_buoyancy = 1
	boat[e].prompt_display = 1
	boat[e].item_highlight = 0
	boat[e].highlight_icon = "imagebank\\icons\\hand.png"
	
	boat_active[e] = 0
	boat_release[e] = 0
	boat_aground[e] =0
	speed[e]=0
	boatx[e]=g_Entity[e].x
	boaty[e]=g_Entity[e].y
	boatz[e]=g_Entity[e].z
	boatlength[e] = 0
	onboatcheck[e]=0
	startheight[e] = 0
	floatangle[e]=0
	colobj[e]=0
	wobble[e] = GetGamePlayerControlWobbleHeight()
	heightTerrain[e] = 0
	heightSurface[e] = 0
	g_tEnt = 0
	tEnt[e] = 0
	status[e] = "init"
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0		
end

function boat_main(e)

	if status[e] == 'init' then
		if boat[e].item_highlight == 3 and boat[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(boat[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(boat[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(boat[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end	
		local xmin, ymin, zmin, xmax, ymax, zmax = GetObjectColBox(g_Entity[e]['obj'])
		local sx,sy,sz = GetObjectScales(g_Entity[e]['obj'])
		boatlength[e] = ((zmax - zmin) * sz)/2
		startheight[e] = g_Entity[e].y
		status[e] = 'boating'
	end	

	if status[e] == 'boating' then	
		if boat_release[e]==nil then boat_release[e]=0 end
		if boat_active[e]==nil then boat_active[e]=0 end
		boatangle=g_Entity[e].angley
		PlayerDX = g_Entity[e].x - g_PlayerPosX;
		PlayerDZ = g_Entity[e].z - g_PlayerPosZ;
		PlayerDist = math.sqrt(math.abs(PlayerDX*PlayerDX)+math.abs(PlayerDZ*PlayerDZ));
		playerdistance[e]=PlayerDist
		onboatcheck[e]=IntersectAll(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ+35,g_PlayerPosX,g_Entity[e].y+36,g_PlayerPosZ,0)
		if onboatcheck[e] == g_Entity[e].obj or playerdistance[e] < boat[e].use_range and boat_active[e]==0 then
			--pinpoint select object--
			module_misclib.pinpoint(e,boat[e].use_range,boat[e].item_highlight,hl_icon[e])
			tEnt[e] = g_tEnt
			--end pinpoint select object--		
			if boat_active[e]==0 and tEnt[e] == e then
				if boat[e].prompt_display == 1 then TextCenterOnX(50,54,1,boat[e].prompt_text) end
				if boat[e].prompt_display == 2 then Prompt(boat[e].prompt_text) end	
			end
			if tEnt[e] == e then
				if g_KeyPressE==1 and boat_release[e]==0 and boat_active[e]==0 then
					boat_active[e]=1
					boat_release[e]=1
					PlaySound(e,0)
					PromptDuration('Q to dismount',3000)				
				end
			end	
		end

		if boat_active[e]==0 then
			SetGamePlayerControlWobbleHeight(wobble[e])
			GravityOff(e)
			CollisionOff(e)			
			if GetWaterHeight() <= startheight[e] then
				local nfloatheight = boat[e].boat_buoyancy/5
				floatangle[e] = floatangle[e] + (GetAnimationSpeed(e)/100.0)				
				local fFinalY = (GetWaterHeight()-1) + nfloatheight + (math.cos(floatangle[e])*nfloatheight)
				local fSwayXZ = nfloatheight/2 + (math.cos(floatangle[e])*nfloatheight/2)
				SetPosition(e,g_Entity[e].x,fFinalY,g_Entity[e].z)
				ResetPosition(e,g_Entity[e].x,fFinalY,g_Entity[e].z)
				SetRotation(e,fSwayXZ/3,boatangle,fSwayXZ)				
			else
				local nfloatheight = boat[e].boat_buoyancy/5
				floatangle[e] = floatangle[e] + (GetAnimationSpeed(e)/100.0)				
				local fFinalY = (GetWaterHeight()-1) + nfloatheight + (math.cos(floatangle[e])*nfloatheight)
				local fSwayXZ = nfloatheight/2 + (math.cos(floatangle[e])*nfloatheight/2)
				SetPosition(e,g_Entity[e].x,fFinalY,g_Entity[e].z)
				ResetPosition(e,g_Entity[e].x,fFinalY,g_Entity[e].z)
				SetRotation(e,fSwayXZ/3,boatangle,fSwayXZ)
			end
			GravityOn(e)
			CollisionOn(e)
			StopSound(e,1)
		end
		
		if boat_active[e]==1 then	
			SetGamePlayerControlWobbleHeight(0)
			local nfloatheight = boat[e].boat_buoyancy/5
			floatangle[e] = floatangle[e] + (GetAnimationSpeed(e)/100.0)
			local fFinalY = GetWaterHeight() + nfloatheight + (math.cos(floatangle[e])*nfloatheight)
			local fSwayXZ = nfloatheight/2 + (math.cos(floatangle[e])*nfloatheight/2)
			boatx[e]=g_Entity[e].x
			boaty[e]=g_Entity[e].y
			boatz[e]=g_Entity[e].z
			SetFreezePosition(freezex,g_Entity[e].y+5,freezez)
			TriggerWaterRipple(g_Entity[e].x,g_Entity[e].y-5,g_Entity[e].z)
			-- Boat speed control
			if g_KeyPressW == 1 then  -- W key
				if speed[e] < boat[e].max_speed then
					speed[e] = speed[e]+(boat[e].boat_power/100)
					LoopSound(e,1)
				end
			end
			if g_KeyPressS == 1 then -- S key
				if speed[e] > boat[e].min_speed then
					speed[e] = speed[e]-(boat[e].boat_brake/100)
					LoopSound(e,1)
				end
				if boat_aground[e] == 1 then
					speed[e] = speed[e]-0.5
					boat_aground[e] = 0
					StopSound(e,1)
					PlaySound(e,2)
				end
			end			
			-- Boat steering control
			if g_KeyPressA == 1 then -- A key
				boatangle=boatangle-(boat[e].turn_speed/10)
			end
			if g_KeyPressD == 1 then -- D key
				boatangle=boatangle+(boat[e].turn_speed/10)
			end
			if g_KeyPressQ == 1 then -- Q key
				SetFreezePosition(freezex,(g_Entity[e].y + 40),freezez)
				TransportToFreezePosition()
				boat_release[e]=0
				boat_active[e]=0
			end
			if  boat_active[e]==1 then
				GravityOff(e)
				CollisionOff(e)
				--Draft Check-------------------------------------------------------------------------------
				heightTerrain[e] = GetTerrainHeight(g_Entity[e].x,g_Entity[e].z)
				if (g_Entity[e].y - boat[e].boat_draft) <= heightTerrain[e] then
					SetFreezePosition(freezex,(g_Entity[e].y + 40),freezez)
					TransportToFreezePosition()
					boat_aground[e] = 1
					speed[e] = -1
					boat_release[e]=0
					boat_active[e]=0
				end
				--Collision Check--------------------------------------------------------------------------
				local ex,ey,ez,eax,eay,eaz = GetEntityPosAng(e)
				local ox,oy,oz = U.Rotate3D(0,0,boatlength[e],0,math.rad(eay),0)
				colobj[e] = IntersectAllIncludeTerrain(ex,ey,ez,ex+ox,ey+oy,ez+oz,g_Entity[e].obj,0,0,10,0)				
				if colobj[e] > 0 or colobj[e] == -1 then
					SetFreezePosition(freezex,(g_Entity[e].y + 40),freezez)
					TransportToFreezePosition()
					boat_aground[e] = 1
					speed[e] = -1
					boat_release[e]=0
					boat_active[e]=0
				end
				GravityOn(e)
				CollisionOn(e)				
				-------------------------------------------------------------------------------------------				
			end
			boatangledeg=math.rad(boatangle+boat[e].boat_rotation)
			boatangleleftdeg=math.rad(boatangle)
			boatanglerightdeg=math.rad(boatangle)
			-- Boat collision
			boatx[e]=boatx[e]+(math.sin(boatangledeg)*speed[e])
			boaty[e]=g_Entity[e].y
			boatz[e]=boatz[e]+(math.cos(boatangledeg)*speed[e])
			-- Update location
			if boat_active[e]==1 then
				freezex=g_Entity[e].x+(math.sin(boatangledeg)*(boat[e].player_z_position-speed[e])*-1)
				freezez=g_Entity[e].z+(math.cos(boatangledeg)*(boat[e].player_z_position-speed[e])*-1)
				SetFreezePosition(freezex,g_Entity[e].y+boat[e].player_y_position,freezez)
			end
			GravityOff(e)
			CollisionOff(e)
			SetPosition(e,boatx[e],boaty[e],boatz[e])
			SetRotation(e,fSwayXZ/3,boatangle,fSwayXZ)			
			TransportToFreezePositionOnly()
			if speed[e] > 0 and g_KeyPressW == 0 then speed[e] = speed[e]-(boat[e].drag/10000) end
			if speed[e] < 0 and g_KeyPressS == 0 then speed[e] = speed[e]/1.1 end
		end
	end
end