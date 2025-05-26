-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Light Mover v6 by Necrym59
-- DESCRIPTION: Allows a light to be attached to a moving object.
-- DESCRIPTION: Attach to a light and edit the settings.
-- DESCRIPTION: [OBJECT_NAME$=""] object name to attach to
-- DESCRIPTION: [ADJUST_X_OFFSET=0(-100,100)] ajdust x offset
-- DESCRIPTION: [ADJUST_Y_OFFSET=0(-100,100)] ajdust y offset
-- DESCRIPTION: [ADJUST_Z_OFFSET=0(-100,100)] ajdust z offset
-- DESCRIPTION: [@LIGHT_TYPE=1(1=Constant, 2=Flashing, 3=Rotating)] Rotating type requires spotlight
-- DESCRIPTION: [LIGHT_SPEED=0(0,100)]
-- DESCRIPTION: [LIGHT_ON!=1] set if the light is initially off or on.
-- DESCRIPTION: [!HIDE_OBJECT=0] hide the attached object

local rad = math.rad
local lower = string.lower

local lightmover = {}
local light_range = {}
local object_name = {}
local adjust_x_offset = {}
local adjust_y_offset = {}
local adjust_z_offset = {}
local light_type = {}
local light_speed = {}
local light_on = {}
local hide_object = {}
local object_number = {}

local status = {}
local attachTo = {}
local lightattached = {}
local lightNum = {}
local tpositionx = {}
local tpositiony = {}
local tpositionz = {}
local offsetx = {}
local offsety = {}
local offsetz = {}
local initialrange = {}
local flashspeed = {}
local flashswitch = {}
local lightangle = {}
local Lx = {}
local Ly = {}
local Lz = {}

function light_mover_properties(e, object_name, adjust_x_offset, adjust_y_offset, adjust_z_offset, light_type, light_speed, light_on, hide_object)
	lightmover[e].object_name = string.lower(object_name) -- name of entity light attached to
	lightmover[e].adjust_x_offset = adjust_x_offset
	lightmover[e].adjust_y_offset = adjust_y_offset
	lightmover[e].adjust_z_offset = adjust_z_offset
	lightmover[e].light_type = light_type
	lightmover[e].light_speed = light_speed
	lightmover[e].light_on = light_on or 1
	lightmover[e].hide_object = hide_object or 0
end

function light_mover_init(e)
	lightmover[e] = {}
	lightmover[e].object_name = ""
	lightmover[e].adjust_x_offset = 0
	lightmover[e].adjust_y_offset = 0
	lightmover[e].adjust_z_offset = 0
	lightmover[e].object_number = 0
	lightmover[e].light_type = 1
	lightmover[e].light_speed = 0
	lightmover[e].light_on = 1
	lightmover[e].hide_object = 0	
	lightNum[e] = GetEntityLightNumber(e)

	attachTo[e] = 0
	tpositionx[e] = 0
	tpositiony[e] = 0
	tpositionz[e] = 0
	offsetx[e] = 0
	offsety[e] = 0
	offsetz[e] = 0
	lightNum[e] = GetEntityLightNumber(e)
	lightattached[e] = 0
	flashspeed[e] = 0
	flashswitch[e] = 0
	lightangle[e]=0
	initialrange[e] = GetLightRange(lightNum[e])
	status[e] = "init"
end

function light_mover_main(e)
	if status[e] == "init" then
		tpositionx[e], tpositiony[e], tpositionz[e] = GetLightPosition(lightNum[e])
		if lightmover[e].object_name > "" and lightmover[e].object_number == 0 then
			for a = 1, g_EntityElementMax do
				if a ~= nil and g_Entity[a] ~= nil then
					if string.lower(GetEntityName(a)) == lightmover[e].object_name then
						lightmover[e].object_number = a
						attachTo[e] = lightmover[e].object_number
						lightattached[e] = 1
						if lightmover[e].hide_object == 1 then Hide(a) end
						break
					end
				end
			end
		end
		if lightattached[e] == 0 then
			Text(50,50,3,"No valid object to attach light to")
			return
		end
		if lightattached[e] == 1 then
			if lightmover[e].light_on == 1 then 
				SetLightRange(lightNum[e],initialrange[e])
				SetActivated(e,1)			
			end	
			local x,y,z,Ax,Ay,Az = GetEntityPosAng(attachTo[e])
			offsetx[e] = tpositionx[e] - x
			offsety[e] = tpositiony[e] - y
			offsetz[e] = tpositionz[e] - z			
		end
		lightNum[e] = GetEntityLightNumber(e)
		Lx,Ly,Lz = GetLightAngle(lightNum[e])
		status[e] = "initdone"
	end
	
	if attachTo[e] == -1 then return end
	
	if lightattached[e] == 1 then
		lightNum[e] = GetEntityLightNumber(e)		
		-- Position/Angle
		local x,y,z,Ax,Ay,Az = GetEntityPosAng(attachTo[e])
		SetLightPosition(lightNum[e], x+offsetx[e]+lightmover[e].adjust_x_offset, y+offsety[e]+lightmover[e].adjust_y_offset, z+offsetz[e]+lightmover[e].adjust_z_offset)
		
		--Set Flashing
		if lightmover[e].light_type == 2 and flashswitch[e] == 0 and g_Time > flashspeed[e] then					
			SetLightRange(lightNum[e],0)
			SetActivated(e,0)
			flashswitch[e] = 1
			flashspeed[e] = g_Time + lightmover[e].light_speed * 500
		end
		if lightmover[e].light_type == 2 and flashswitch[e] == 1 and g_Time > flashspeed[e] then
			SetLightRange(lightNum[e],initialrange[e])
			SetActivated(e,1)
			flashswitch[e] = 0
			flashspeed[e] = g_Time + lightmover[e].light_speed * 500
		end
		--Set Rotating
		if lightmover[e].light_type == 3 then
			lightangle[e]=lightangle[e]+lightmover[e].light_speed
			if lightangle[e] == 360 then lightangle[e] = 0 end
			SetLightEuler(lightNum[e], Lx+Ax,lightangle[e],Lz+Az)
		end
		if g_Entity[e]['activated'] == 1 then
			SetLightRange(lightNum[e],initialrange[e])
		end
		if g_Entity[e]['activated'] == 0 then
			SetLightRange(lightNum[e],0)
		end		
	
		if g_Entity[attachTo[e]]['health'] <= 0 then
			SetLightRange(lightNum[e],0)
			Destroy(lightNum[e])
		end
	end	
end