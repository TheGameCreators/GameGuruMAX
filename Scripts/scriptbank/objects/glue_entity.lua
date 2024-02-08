-- Glue Entity v2 by Amen Moses
-- DESCRIPTION: Attached entity can be logic linked (glued) to a moving entity to allow them all move together. 
-- DESCRIPTION: [BehaviorName$=""] of the moving entity.

local U = require "scriptbank\\utillib"
local Q = require "scriptbank\\quatlib"
local deg = math.deg
local rad = math.rad

local glueEnts 		= {}
local origFuncs 	= {}
local newFuncs  	= {}
local x, y, z, xa, ya, za
local nquat

function glue_entity_properties(e, BehaviorName)
	script = BehaviorName or ""	
	local ge = glueEnts[ e ]
	if ge == nil then return end	
	ge.script = script .. "_main"
end

function glue_entity_init_name( e, name )
	glueEnts[ e ] = { state = 'init', obj = g_Entity[ e ].obj }
end

local function getOffs( bobj, gobj ) -- creates list entry by calculating offsets between entities
	local x1, y1, z1, ax1, ay1, az1 = GetObjectPosAng( bobj )
	local x2, y2, z2, ax2, ay2, az2 = GetObjectPosAng( gobj )
	
	local q1 = Q.FromEuler( rad( ax1 ), rad( ay1 ), rad( az1 ) )
	local q2 = Q.FromEuler( rad( ax2 ), rad( ay2 ), rad( az2 ) )

	local xo, yo, zo = x2 - x1, y2 - y1, z2 - z1
	-- now need to rotate the offets to what they would be if the 
	-- base object was not rotated
	-- ** only works if origin is centred?? **
	q1 = Q.Conjugate( q1 )
	xo, yo, zo = U.Rotate3D( xo, yo, zo, Q.ToEuler( q1 ) )
	-- return rotated offsets and angular difference
	return { xo = xo, yo = yo, zo = zo, angQ = Q.Mul( q1 , q2 ) }
end

local function repositionGE( e, obj, offs )
	local xo, yo, zo = U.Rotate3D( offs.xo, offs.yo, offs.zo, xa, ya, za )	
	local nxa, nya, nza = Q.ToEuler( Q.Mul( nquat, offs.angQ ) )
	CollisionOff( e )
	PositionObject( obj, x + xo, y + yo, z + zo )
	RotateObject( obj, deg( nxa ), deg( nya ), deg( nza ) )
	CollisionOn( e )
end

local function updateAll( ge )
	x, y, z, xa, ya, za = GetObjectPosAng( ge.gluedToOb )
	xa, ya, za = rad( xa ), rad( ya ), rad( za )
	nquat = Q.FromEuler( xa, ya, za )
	repositionGE( ge.ent, ge.obj, ge.offs )
	for _, v in pairs( ge.gluedEnts ) do
		repositionGE( v.ent, v.obj, v.offs )
	end
end

local function processGlued( e )
	for _, v in pairs( glueEnts ) do
		if v.gluedToE == e then
			updateAll( v )
			return
		end
	end
end

local function replaceFunc( ge )
	if origFuncs[ ge.script ] ~= nil then
		return
	else
		-- point to original script
		origFuncs[ #origFuncs + 1 ] = _G[ ge.script ]
		-- now create replacement function
		local origFunc
		if #origFuncs == 1 then
			newFuncs[ ge.script ] = 
				function( e ) origFuncs[ 1 ]( e ); processGlued( e ); end
		elseif
		   #origFuncs == 2 then
			newFuncs[ ge.script ] = 
				function( e ) origFuncs[ 2 ]( e ); processGlued( e ); end
		elseif
		   #origFuncs == 3 then
			newFuncs[ ge.script ] = 
				function( e ) origFuncs[ 3 ]( e ); processGlued( e ); end
		elseif
		   #origFuncs == 4 then
			newFuncs[ ge.script ] = 
				function( e ) origFuncs[ 4 ]( e ); processGlued( e ); end
		elseif
		   #origFuncs == 5 then
			newFuncs[ ge.script ] = 
				function( e ) origFuncs[ 5 ]( e ); processGlued( e ); end
		elseif
		   #origFuncs == 6 then
			newFuncs[ ge.script ] = 
				function( e ) origFuncs[ 6 ]( e ); processGlued( e ); end
		elseif
		   #origFuncs == 7 then
			newFuncs[ ge.script ] = 
				function( e ) origFuncs[ 7 ]( e ); processGlued( e ); end
		elseif
		   #origFuncs == 8 then
			newFuncs[ ge.script ] = 
				function( e ) origFuncs[ 8 ]( e ); processGlued( e ); end
		elseif
		   #origFuncs == 9 then
			newFuncs[ ge.script ] = 
				function( e ) origFuncs[ 9 ]( e ); processGlued( e ); end
		elseif
		   #origFuncs == 10 then
			newFuncs[ ge.script ] = 
				function( e ) origFuncs[ 10 ]( e ); processGlued( e ); end
		else
			ge.state = 'too many'
			return
		end
		_G[ ge.script ] = newFuncs[ ge.script ]
	end
end

local function getEntityLinks( e )

	local list = {}
	for i = 0, 9 do
		local elink = GetEntityRelationshipID( e, i )
		if elink > 0 then
			list[ #list + 1 ] = elink
		end
	end
	return list
end

function glue_entity_main( e )
	
	local ge = glueEnts[ e ]

	if ge == nil or ge.state == 'ready' then return end
		
	if ge.state == 'init' then
		if ge.script ~= "_main" then
			if _G[ ge.script ] ~= nil then
				replaceFunc( ge )
			else
				ge.state = "nofunc"
			end
		end
		if ge.state ~= 'init' then return end
		
		local links = getEntityLinks( e )

		if #links == 0 then
			ge.state = 'unlinked'
			return
		end		
		-- assume the first link found is the moving entity
		ge.gluedToE  = links[ 1 ]
		ge.ent       = e
		ge.gluedToOb = g_Entity[ ge.gluedToE ].obj
		ge.offs      = getOffs( ge.gluedToOb, ge.obj )
		ge.state     = 'ready'
		-- get the rest
		ge.gluedEnts = {}
		
		for _, l in ipairs( links ) do
			if l ~= ge.gluedToE then
				local obj = g_Entity[ l ].obj
				ge.gluedEnts[ #ge.gluedEnts + 1 ] = 
				    { ent  = l,
					  obj  = obj,
 					  offs = getOffs( ge.gluedToOb, obj )
                    }
            end
		end				
	elseif
	   ge.state == 'too many' then
		PromptLocal( e, 'Too many different glued to entities' )
	elseif
	   ge.state == 'nofunc' then
		PromptLocal( e, 'Function ' .. ( ge.script or 'nil' ) .. ' does not exist' )		
	elseif
	   ge.state == 'unlinked' then
		PromptLocal( e, 'Not linked to anything' )
	end
end
