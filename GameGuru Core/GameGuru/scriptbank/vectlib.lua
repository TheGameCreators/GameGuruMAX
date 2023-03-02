-- Vector math module for GameGuru
-- Written by Chris Stapleton
local U = require "scriptbank\\utillib"
local Q = require "scriptbank\\quatlib"

local V = {_G = _G}

-- import section: modules
local sqrt = math.sqrt
local rad  = math.rad
local abs  = math.abs
local atan = math.atan2

-- include these for debugging
local Prompt  = Prompt
local PromptD = PromptDuration
local PromptE = PromptLocal

local gEnt = g_Entity

_ENV = V 

-- create a vector
function V.Create( x, y, z )
	return { x = x, y = y, z = z }
end

-- add two vectors
function V.Add( v1, v2 )
	return Create( v1.x + v2.x, v1.y + v2.y, v1.z + v2.z )
end

-- subtract vectors
function V.Sub( v1, v2 )
	return Create( v1.x - v2.x, v1.y - v2.y, v1.z - v2.z )
end

-- multiply a vector by a scalar value
function V.Mul( v, val )
	return Create( v.x * val, v.y * val, v.z * val )
end

-- multiply a vector by another vector (a GG 'special')
function V.Scale( v1, v2 )
	return Create( v1.x * v2.x, v1.y * v2.y, v1.z * v2.z )
end

-- divide a vector by a scalar value
function V.Div( v, val )
	return Create( v.x / val, v.y / val, v.z / val )
end

-- returns the squared magnitude of a vector
function V.SqMag( v )
	return( v.x * v.x + v.y * v.y + v.z * v.z )
end

-- returns the magnitude of a vector
function V.Mag( v )
	return sqrt( V.SqMag( v ) )
end

-- normalise a vector
function V.Norm( v )
	return Div( v, Mag( v ) )
end

-- distance between two vectors
function V.Dist( v1, v2 )
	return Mag( Sub( v2, v1 ) )
end

-- squared distance between two vectors
function V.SqDist( v1, v2 )
	return SqMag( Sub( v2, v1 ) )
end

-- return vector representing cross product of two vectors
function V.Cross( v1, v2)
   return { x = v1.y * v2.z - v2.y * v1.z,
            y = v1.z * v2.x - v2.z * v1.x,
            z = v1.x * v2.y - v2.x * v1.y }
end

-- return dot product of two vectors
function V.Dot( v1, v2 )
   return ( v1.x * v2.x ) + ( v1.y * v2.y ) + ( v1.z * v2.z)
end

-- check if vectors closer than some value
function V.Closer( v1, v2, dist )
	return SqDist( v1, v2 ) < dist * dist
end

-- rotate a vector by a specific Euler angle
function V.Rot( v, xA, yA, zA )  -- Euler angles in radians
	local x, y, z = U.Rotate3D( v.x, v.y, v.z, xA, yA, zA )
	return Create( x, y, z )
end

-- rotate a vector by a specific Euler angle and normalise
function V.NRot( v, xA, yA, zA )  -- Euler angles in radians
	return Norm( Rot( v, xA, yA, zA ) )
end

local fVec = Create( 0, 0, 1 )
local uVec = Create( 0, 1, 0 )

-- return forward facing normalised vector for given orientation
-- specified as Euler angles
function V.FromEuler( xA, yA, zA, up )
	if up then
		return Rot( uVec, xA, yA, zA )
	else
		return Rot( fVec, xA, yA, zA )
	end
end

-- return forward facing normalised vector for given orientation
-- specified as quaternion
function V.FromQuat( quat, up )
	local xA, yA, zA = Q.ToEular( quat )
	return FromEuler( xA, yA, zA, up )
end

-- return a quaternion representing the rotation between two vectors
function V.QuatBetween( v1, v2) 
	local d    = Dot( v1, v2 ) 
	local axis = Cross( v1, v2 )
	
    local qw = sqrt( V.SqMag( v1 ) * V.SqMag( v2 ) ) + d
	if qw > 0.00001 then
		return Q.Normalise( { w = qw, x = axis.x, y = axis.y, z = axis.z } )
	else
		-- vectors are 180 degrees apart so we don't know which way to rotate
		-- to avoid returning an unstable quaternion we simply choose to Yaw
		return Q.FromEuler( 0, rad( 180 ), 0 )
	end
end 

-- returns the quadrant that the vector is in, N,S,E,W
function V.QuadrantIn( v )
	local ax, az = abs( v.x ), abs( v.z )
	if az >= ax then 
		if v.z < 0 then return 'S' end
		return 'N'
	else
		if v.x < 0 then return 'W' end
		return 'E'
	end
end

-- returns 'pitch' Angle and realworld 'yaw' angle represented by a vector plus the 
-- quadrant that the Y angle is in
function V.GetPYAngles( v )
	-- now work out the angle from the y and z components of the result
	local ang = atan( v.y, abs( v.z ) )
	if abs( v.z ) > abs( v.x ) then
		return ang, atan( v.x, v.z ), QuadrantIn( v )
	else
		return atan( v.y, abs( v.x ) ), atan( v.x, v.z ), QuadrantIn( v ) 
	end 
end	

-- Solve the Catmull-Rom parametric equation for a given time(t) and vector 
-- quadruple ( p1, p2, p3, p4 )
function V.CR_Point( t, p1, p2, p3, p4 )
    local t2 = t  * t
    local t3 = t2 * t

	local v1 = V.Mul( p1, 0.5 * (     -t3 + 2 * t2 - t ) )
	local v2 = V.Mul( p2, 0.5 * (  3 * t3 - 5 * t2 + 2 ) )
	local v3 = V.Mul( p3, 0.5 * ( -3 * t3 + 4 * t2 + t ) )
	local v4 = V.Mul( p4, 0.5 * (      t3 -     t2     ) )
	
    return ( V.Add( v1, V.Add( v2, V.Add( v3, v4 ) ) ) ) 
end

return V
