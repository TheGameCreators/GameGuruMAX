-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- DESCRIPTION: Radar v3 
-- DESCRIPTION: [RADAR_SIZE=20(5,80)]
-- DESCRIPTION: [RADAR_RANGE=4000(1000,5000)]
-- DESCRIPTION: [RADAR_SCREEN_X=85(0,100)]
-- DESCRIPTION: [RADAR_SCREEN_Y=20(0,100)]
-- DESCRIPTION: [@RADAR_BLIPS=1(1=Standard, 2=Detailed)]
-- DESCRIPTION: [@RADAR_STYLE=1(1=Rotating, 2=Fixed)]
-- DESCRIPTION: [IMAGEFILE$="imagebank\\misc\\testimages\\radar\\radar1.png"]

local  U = require "scriptbank\\utillib"

local lower = string.lower
local atan  = math.atan2
local sin   = math.sin
local cos   = math.cos
local sqrt  = math.sqrt
local rad   = math.rad

local gEnt = g_Entity

local radarx     = 85
local radary     = 20
local radarSize  = 20
local radarRange = 4000
local radarBlips = 1
local radarStyle = 1
local radarImage = "imagebank\\misc\\testimages\\radar\\radar1.png"

local radarState  = 'init'
local radarSprite = 0

local radarImageRed    = 0
local radarImageGreen  = 0
local radarImageYellow = 0
local radarImageBlue   = 0
local radarImageWhite  = 0
local radarImagePurple = 0
local radarImageOrange = 0

local radarRngCorrect = {}

g_character_attack = g_character_attack or {}
g_zombie_attack    = g_zombie_attack    or {}

g_base_ratio = 16 / 9
g_aspect_ratio = ( GetDeviceWidth() / GetDeviceHeight() )

g_aspectXcorrect = 1
g_aspectYcorrect = 1

if g_aspect_ratio < g_base_ratio then
	g_aspectYcorrect = g_aspect_ratio / g_base_ratio
elseif
   g_aspect_ratio > g_base_ratio then
	g_aspectXcorrect = g_base_ratio / g_aspect_ratio 
end

function SizeSpriteCentred( spr, x, y, offx, offy )
	local xs, ys = x * g_aspectXcorrect, y * g_aspectYcorrect
	offx = offx or xs / 2
	offy = offy or ys / 2
	SetSpriteSize( spr, xs, ys )
	SetSpriteOffset( spr, offx, offy )
end

local radarMakeInvisible = 0
local radarShowing = false

local radSprites = {}

function radar_hideallsprites()
	radarMakeInvisible = 1
end

function radar_showallsprites()
	radarMakeInvisible = 0
end

local function makeSprite( typ )
	local sprite = nil
	if typ == 'red' then
		sprite = CreateSprite( radarImageRed )
	elseif
	   typ == 'blue' then
		sprite = CreateSprite( radarImageBlue )
	elseif
	   typ == 'green' then
		sprite = CreateSprite( radarImageGreen )
	elseif
	   typ == 'yellow' then
		sprite = CreateSprite( radarImageYellow )
	elseif
	   typ == 'white' then
		sprite = CreateSprite( radarImageWhite )
	elseif
	   typ == 'purple' then
		sprite = CreateSprite( radarImagePurple )	
	elseif
	   typ == 'orange' then
		sprite = CreateSprite( radarImageOrange )		
	end
	
	if sprite ~= nil then
		SizeSpriteCentred( sprite, 0.5, 0.5 * g_base_ratio )
		return { spr = sprite, typ = typ }
	end
end	

function radar_properties(e, radar_size, radar_range, radar_screen_X, radar_screen_Y, radar_blips, radar_style, imagefile)
	radarSize  = radar_size
	radarRange = radar_range
	radarx     = radar_screen_X
	radary     = radar_screen_Y
	radarBlips = radar_blips
	radarStyle = radar_style	
	radarImage = imagefile
end	

local function createSprites()
	if radarBlips == 2 then
		radarImageRed    = LoadImage( "imagebank\\misc\\testimages\\radar\\radar-enemy_red.png" )
		radarImageGreen  = LoadImage( "imagebank\\misc\\testimages\\radar\\radar-enemy_green.png" )
		radarImageYellow = LoadImage( "imagebank\\misc\\testimages\\radar\\radar-objective_yellow.png" )
		radarImageBlue   = LoadImage( "imagebank\\misc\\testimages\\radar\\radar-objective_blue.png" )
		radarImageWhite  = LoadImage( "imagebank\\misc\\testimages\\radar\\radar-objective_white.png" )
		radarImagePurple = LoadImage( "imagebank\\misc\\testimages\\radar\\radar-objective_purple.png" )
		radarImageOrange = LoadImage( "imagebank\\misc\\testimages\\radar\\radar-objective_orange.png" )
	else
		radarImageRed    = LoadImage( "imagebank\\misc\\testimages\\radar\\radar-red.png" )
		radarImageGreen  = LoadImage( "imagebank\\misc\\testimages\\radar\\radar-green.png" )
		radarImageYellow = LoadImage( "imagebank\\misc\\testimages\\radar\\radar-yellow.png" )
		radarImageBlue   = LoadImage( "imagebank\\misc\\testimages\\radar\\radar-blue.png" )
		radarImageWhite  = LoadImage( "imagebank\\misc\\testimages\\radar\\radar-white.png" )
		radarImagePurple = LoadImage( "imagebank\\misc\\testimages\\radar\\radar-purple.png" )
		radarImageOrange = LoadImage( "imagebank\\misc\\testimages\\radar\\radar-orange.png" )
	end	
	radarSprite = CreateSprite( LoadImage( radarImage ) )
	SetSpriteDepth( radarSprite, 100 )
	SizeSpriteCentred( radarSprite, radarSize, radarSize * g_base_ratio )
	radarRngCorrect = radarRange / 3000
end

function radar_init(e)	
end

local function isEnemy( e )
	local Ent = gEnt[ e ]
	if Ent.active > 0 and Ent.health > 0 then
		local Atk = g_character_attack[ e ] or g_zombie_attack[ e ]
		if Atk ~= nil then
			if Atk.target == 'player' then
				return 'red'
			else
				return 'green'
			end
		end
	end
end

local timer = 0

local function deleteTargets( entlist )
	for k, v in pairs( radSprites ) do
		if v ~= nil then
			local delSprite = true
			for _, j in pairs( entlist ) do
				if k == j then
					if isEnemy( k ) == radSprites[ k ].typ or
					   ( isObjective ~= nil and isObjective( j ) ~= nil ) then
						delSprite = false
					end
					break
				end
			end
			if delSprite then 
				DeleteSprite( v.spr )
				radSprites[ k ] = nil
			end
		end
	end
end

function radar_main( e )
	if radarState == 'init' then
		createSprites()
		radarState = 'done'
		return
	end
	
	if radarMakeInvisible == 0 then 
		if not radarShowing then
			SetSpritePosition( radarSprite, radarx, radary )
			radarShowing = true
		end
		if radarStyle == 1 then
			SetSpriteAngle( radarSprite, -g_PlayerAngY )
		end
	else
		if radarShowing then
			SetSpritePosition( radarSprite, 200, 200 )
			for _, v in pairs( radSprites ) do	
				SetSpritePosition( v.spr, 200, 200 )
			end
			radarShowing = false
		end
		return
	end
	
	if g_Time > timer then
		timer = g_Time + 100  -- do 10 times a second
	
		local entlist = U.ClosestEntities( radarRange )
		
		-- first delete any sprites we don't need anymore
		deleteTargets( entlist )

		-- now create sprites as needed
		for _, k in pairs( entlist ) do
			local typ = isEnemy( k )
			if typ ~= nil and
			   radSprites[ k ] == nil then
				radSprites[ k ] = makeSprite( typ )
			end
			if radSprites[ k ] == nil and
			   isObjective ~= nil then
				local typ = isObjective( k )
				if typ ~= nil then
					radSprites[ k ] = makeSprite( lower( typ ) ) 
				end
			end
		end
	end
	
	-- finally display the sprites
	for k, v in pairs( radSprites ) do
		local Ent = gEnt[ k ]
		if Ent ~= nil then
			local x = g_PlayerPosX - Ent.x
			local z = g_PlayerPosZ - Ent.z
			local angle = atan( z , x ) + -90 * 0.0174533 + rad( g_PlayerAngY )
			dist = sqrt( x * x + z * z  )
			if dist > 2576 * radarRngCorrect then
				dist = 2576 * radarRngCorrect
			end
			dist = dist / 200 / radarRngCorrect
			x = radarx +  sin( angle ) * ( dist / g_base_ratio ) * radarSize / 15 * g_aspectXcorrect
			z = radary +  cos( angle ) * ( dist ) * radarSize / 15 * g_aspectYcorrect
			SetSpritePosition( v.spr, x, z )
		else
			SetSpritePosition( v.spr, 200, 200 )
		end
	end
end
