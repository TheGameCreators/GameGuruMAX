-- Add_Movement v3 by Necrym59
-- DESCRIPTION: Will add the selected movement effect to the named object.
-- DESCRIPTION: Attach to an object. Set Always active ON
-- DESCRIPTION: [OBJECT_NAME$=""]
-- DESCRIPTION: [MOVE_X=0(0,1000)]
-- DESCRIPTION: [MOVE_Y=0(0,1000)]
-- DESCRIPTION: [MOVE_Z=0(0,1000)]
-- DESCRIPTION: [#MOVE_SPEED=0.50(0.0,2.0)]
-- DESCRIPTION: [RESET_DELAY=0(0,100)] seconds
-- DESCRIPTION: [ActiveAtStart!=1] if unchecked use a switch or zone trigger to activate.
-- DESCRIPTION: <Sound1> - Reset Sound
-- DESCRIPTION: <Sound2> - Moving Sound

local U = require "scriptbank\\utillib"
local rad = math.rad
local lower = string.lower

local add_movement 		= {}
local object_name		= {}
local move_x			= {}
local move_y			= {}
local move_z			= {}
local move_speed		= {}
local reset_delay		= {}
local ActivateAtStart	= {}

local object_no			= {}
local status			= {}
local objEnt			= {}
local objectxpos		= {}
local objectypos		= {}
local objectzpos		= {}
local objectxang		= {}
local objectyang		= {}
local objectzang		= {}
local startxang 		= {}
local startyang 		= {}
local startzang 		= {}
local reset				= {}
local reached			= {}
local movestate			= {}
local movedir			= {}
local moved				= {}

function add_movement_properties(e, object_name, move_x, move_y, move_z, move_speed, reset_delay, ActivateAtStart)
	add_movement[e].object_name = lower(object_name)
	add_movement[e].move_x = move_x
	add_movement[e].move_y = move_y
	add_movement[e].move_z = move_z
	add_movement[e].move_speed = move_speed
	add_movement[e].reset_delay = reset_delay	
	add_movement[e].ActivateAtStart = ActivateAtStart
	add_movement[e].object_no = 0
end

function add_movement_init(e)
	add_movement[e] = {}
	add_movement[e].object_name = ""	
	add_movement[e].move_x =0
	add_movement[e].move_y = 0
	add_movement[e].move_z = 0
	add_movement[e].move_speed = 0
	add_movement[e].reset_delay = 0	
	add_movement[e].ActivateAtStart = 1
	add_movement[e].object_no = 0
	
	status[e] = "init"
	reset[e] = math.huge
	reached[e] = 0
	movestate[e] = 0
	movedir[e] = 1	
	moved[e] = 0
end

function add_movement_main(e)
	if status[e] == "init" then
		if add_movement[e].object_no == 0 then
			for n = 1, g_EntityElementMax do
				if n ~= nil and g_Entity[n] ~= nil then
					if lower(GetEntityName(n)) == add_movement[e].object_name then
						add_movement[e].object_no = n
						break
					end
				end
			end
		end
		if add_movement[e].object_no ~= 0 then
			local x,y,z,Ax,Ay,Az = GetEntityPosAng(add_movement[e].object_no)
			objectxpos[e] = x
			objectypos[e] = y
			objectzpos[e] = z
			objectxang[e] = Ax
			objectyang[e] = Ay
			objectzang[e] = Az
			_,_,_,startxang[e],startyang[e],startzang[e] = GetEntityPosAng(add_movement[e].object_no)
		end
		if add_movement[e].object_no ~= 0 then
			if add_movement[e].ActivateAtStart == 1 then SetActivated(e,1) end
			if add_movement[e].ActivateAtStart == 0 then SetActivated(e,0) end
		end		
		reset[e] = g_Time + (add_movement[e].reset_delay*1000)
		status[e] = "endinit"
	end

	if g_Entity[e].activated == 1 then
		Show(add_movement[e].object_no)
		local x,y,z,Ax,Ay,Az = GetEntityPosAng(add_movement[e].object_no)
		local ox,oy,oz = U.Rotate3D(add_movement[e].move_x, add_movement[e].move_y, add_movement[e].move_z, rad(startxang[e]),rad(startyang[e]),rad(startzang[e]))	
		objectxpos[e] = x + ox * add_movement[e].move_speed/100 * movedir[e]
		objectypos[e] = y + oy * add_movement[e].move_speed/100 * movedir[e]
		objectzpos[e] = z + oz * add_movement[e].move_speed/100 * movedir[e]
		objectxang[e] = Ax
		objectyang[e] = Ay
		objectzang[e] = Az
		if g_Time > reset[e] and reached[e] == 0 then
			GravityOff(add_movement[e].object_no)
			CollisionOff(add_movement[e].object_no)
			SetPosition(add_movement[e].object_no,objectxpos[e],objectypos[e],objectzpos[e])		
			StopSound(e,0)
			LoopSound(e,1)
			moved[e] = moved[e] + 1
			if add_movement[e].move_x > 0 then
				if moved[e] == add_movement[e].move_x then
					reached[e] = 1
					StopSound(e,1)
					PlaySound(e,0)				
					movedir[e] = movedir[e]* -1
					reset[e] = g_Time + (add_movement[e].reset_delay*1000)
				end
			end
			if add_movement[e].move_y > 0 then			
				if moved[e] == add_movement[e].move_y then
					reached[e] = 1
					StopSound(e,1)
					PlaySound(e,0)				
					movedir[e] = movedir[e]* -1
					reset[e] = g_Time + (add_movement[e].reset_delay*1000)
				end
			end
			if add_movement[e].move_z > 0 then				
				if moved[e] == add_movement[e].move_z then
					reached[e] = 1
					StopSound(e,1)
					PlaySound(e,0)
					movedir[e] = movedir[e]* -1
					reset[e] = g_Time + (add_movement[e].reset_delay*1000)
				end
			end	
		end
		if g_Time > reset[e] and reached[e] == 1 then
			GravityOff(add_movement[e].object_no)
			CollisionOff(add_movement[e].object_no)
			SetPosition(add_movement[e].object_no,objectxpos[e],objectypos[e],objectzpos[e])
			StopSound(e,0)			
			LoopSound(e,1)
			moved[e] = moved[e] - 1
			if moved[e] == 0 then
				reached[e] = 0
				movedir[e] = 1
				reset[e] = g_Time + (add_movement[e].reset_delay*1000)
			end
		end
	end
end