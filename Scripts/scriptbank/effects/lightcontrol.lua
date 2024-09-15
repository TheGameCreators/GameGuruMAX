-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Light Control v23 by Necrym59, with thanks to synchromesh
-- DESCRIPTION: Ramps the strength distance of a light up or down when activated by a zone, switch or by range.
-- DESCRIPTION: Attach to a light.
-- DESCRIPTION: [@RANGE_SENSING=1(1=Yes, 2=No, 3=None-Light On)]
-- DESCRIPTION: [SENSOR_RANGE=100(1,1000)]
-- DESCRIPTION: [MAX_LIGHT_DISTANCE=150(1,1000)]
-- DESCRIPTION: [#SPEED=1.20(0.0,500.0)]
-- DESCRIPTION: [@MODE=1(1=Turn Off/On, 2=Minimum , 3=Keep On, 4=Change Color, 5=Overpower, 6=Use Day/Night Cycle)]
-- DESCRIPTION: [LIGHT_R=255(0,255)]
-- DESCRIPTION: [LIGHT_G=255(0,255)]
-- DESCRIPTION: [LIGHT_B=255(0,255)]
-- DESCRIPTION: [@FLICKER=1(1=None, 2=Startup, 3=Random)]
-- DESCRIPTION: [LIGHT_OBJECT_NAME$="Light Object"]
-- DESCRIPTION: <Sound0> when turning on
-- DESCRIPTION: <Sound1> when overpowered

local lower = string.lower

g_sunrollposition = {}

local lightcontrol 			= {}
local range_sensing 		= {}
local sensor_range 			= {}
local max_light_distance 	= {}
local speed 				= {}
local mode 					= {}
local light_r 				= {}
local light_g 				= {}
local light_b 				= {}
local flicker 				= {}
local light_object_name 	= {}
local light_object_no 		= {}

local current_level 		= {}
local current_light_r 		= {}
local current_light_g 		= {}
local current_light_b 		= {}
local minrange 				= {}
local lightNum = GetEntityLightNumber( e )
local status 				= {}
local played 				= {}
local wait 					= {}
local rwait 				= {}
local failtime				= {}
local doonce 				= {}

function lightcontrol_properties(e, range_sensing, sensor_range, max_light_distance, speed, mode, light_r, light_g, light_b, flicker, light_object_name, light_object_no)
	lightcontrol[e].range_sensing = range_sensing
	lightcontrol[e].sensor_range = sensor_range
	lightcontrol[e].max_light_distance = max_light_distance
	lightcontrol[e].speed = speed/10
	lightcontrol[e].mode = mode
	lightcontrol[e].light_r = light_r
	lightcontrol[e].light_g = light_g
	lightcontrol[e].light_b = light_b
	lightcontrol[e].flicker = flicker
	lightcontrol[e].light_object_name = lower(light_object_name)
	lightcontrol[e].light_object_no = 0
end

function lightcontrol_init(e)
	lightcontrol[e] = {}
	lightcontrol[e].range_sensing			= 1
	lightcontrol[e].sensor_range			= 100
	lightcontrol[e].max_light_distance		= 150
	lightcontrol[e].speed					= 1
	lightcontrol[e].mode 					= 1
	lightcontrol[e].light_r					= 255
	lightcontrol[e].light_g					= 255
	lightcontrol[e].light_b					= 255
	lightcontrol[e].flicker 				= 1
	lightcontrol[e].light_object_name 		= ""
	lightcontrol[e].light_object_no 		= 0

	status[e] = "init"
	lightNum = GetEntityLightNumber( e )
	current_level[e] = 0
	current_light_r , current_light_g, current_light_b = GetLightRGB( lightNum )
	wait[e] = math.huge
	failtime[e] = math.huge
	minrange[e] = 0
	played[e] = 0
	doonce[e] = 0
	rwait[e] = math.huge
	g_sunrollposition = 0
end

function lightcontrol_main(e)
	
	if status[e] == "init" then
		rwait[e] = g_Time + math.random(10,100)
		lightNum = GetEntityLightNumber(e)
		minrange[e] = GetLightRange(lightNum)
		SetLightRange(lightNum,minrange[e])
		current_level[e] = minrange[e]
		if lightcontrol[e].light_object_no == 0 and lightcontrol[e].light_object_name > "" then
			for a = 1, g_EntityElementMax do
				if a ~= nil and g_Entity[a] ~= nil then
					if lower(GetEntityName(a)) == lightcontrol[e].light_object_name then
						lightcontrol[e].light_object_no = GetEntityLightNumber(a)
						SetEntityEmissiveStrength(a,0)
						SetActivated(a,0)
						break
					end
				end
			end
		end
		status[e] = "endinit"
	end

	if lightcontrol[e].range_sensing == 1 then
		if GetPlayerDistance(e) < lightcontrol[e].sensor_range then
			g_Entity[e]['activated'] = 1
		end
		if GetPlayerDistance(e) > lightcontrol[e].sensor_range then
			g_Entity[e]['activated'] = 0
			wait[e] = g_Time + 100
		end
	end
	if lightcontrol[e].range_sensing == 3 then
		g_Entity[e]['activated'] = 1
	end
	
	if g_Entity[e]['activated'] == 1 then
		lightNum = GetEntityLightNumber( e )
		if current_level[e] < lightcontrol[e].max_light_distance then
			SetLightRange(lightNum,current_level[e])
			if lightcontrol[e].light_object_no > 0 then SetEntityEmissiveStrength(lightcontrol[e].light_object_no,current_level[e]) end
			current_level[e] = current_level[e] + lightcontrol[e].speed
			if current_level[e] > (lightcontrol[e].max_light_distance-1) then
				current_level[e] = lightcontrol[e].max_light_distance
				failtime[e] = g_Time + 500
			end
		end
		if lightcontrol[e].mode == 4 and current_level[e] == lightcontrol[e].max_light_distance then
			SetLightRGB(lightNum,lightcontrol[e].light_r,lightcontrol[e].light_g,lightcontrol[e].light_b)
		end		
	else
		lightNum = GetEntityLightNumber( e )
		if lightcontrol[e].mode == 4 then
			SetLightRGB(lightNum,current_light_r,current_light_g,current_light_b)
		end
	end	
	if lightcontrol[e].mode == 5 then
		if g_Time > failtime[e] then
			if current_level[e] < 2000 then
				SetLightRange(lightNum,current_level[e])
				if lightcontrol[e].light_object_no > 0 then SetEntityEmissiveStrength(lightcontrol[e].light_object_no,current_level[e]) end
				current_level[e] = current_level[e] + lightcontrol[e].speed*2				
				if current_level[e] >= 2000 then
					current_level[e] = 0
					SetLightRange(lightNum,current_level[e])
					if lightcontrol[e].light_object_no > 0 then SetEntityEmissiveStrength(lightcontrol[e].light_object_no,0) end
					PerformLogicConnections(e)
					PlaySound(e,1)
					Destroy(e)					
				end
			end
		end
	end
	
	if lightcontrol[e].mode == 6 then
		if g_sunrollposition > -90 and g_sunrollposition < 84 then  --Day
			SetActivated(e,0)
			current_level[e] = minrange[e]
		end	
		if g_sunrollposition > 85 then  --Night
			SetActivated(e,1)
		end
	end	
	
	if g_Entity[e]['activated'] == 0 then
		lightNum = GetEntityLightNumber( e )
		if lightcontrol[e].mode == 1 then
			if current_level[e] > minrange[e] then
				SetLightRange(lightNum,current_level[e])
				if lightcontrol[e].light_object_no > 0 then SetEntityEmissiveStrength(lightcontrol[e].light_object_no,current_level[e]) end
				current_level[e] = current_level[e] - lightcontrol[e].speed
				if current_level[e] <= minrange[e] then
					current_level[e] = 0
					SetLightRange(lightNum,current_level[e])
					if lightcontrol[e].light_object_no > 0 then SetEntityEmissiveStrength(lightcontrol[e].light_object_no,current_level[e]) end
					doonce[e] = 0
				end
			end
			wait[e] = g_Time + 100
		end
		if lightcontrol[e].mode == 2 then
			if current_level[e] > minrange[e] then
				SetLightRange(lightNum,current_level[e])
				current_level[e] = current_level[e] - lightcontrol[e].speed
				if lightcontrol[e].light_object_no > 0 then SetEntityEmissiveStrength(lightcontrol[e].light_object_no,current_level[e]) end
				if current_level[e] <= minrange[e] then
					current_level[e] = minrange[e]
					doonce[e] = 0
				end
			end
		end
		if lightcontrol[e].mode == 3 then
			--nothing keep light on
		end		
	end

	if lightcontrol[e].flicker == 2 and g_Entity[e]['activated'] == 1 then
		if g_Time > wait[e] then
			if doonce[e] < 80 then
				current_level[e] = math.random(10,lightcontrol[e].max_light_distance)
				if lightcontrol[e].light_object_no > 0 then SetEntityEmissiveStrength(lightcontrol[e].light_object_no,current_level[e]) end
				doonce[e] = doonce[e] + 1
			end
		end
	end
	if lightcontrol[e].flicker == 3 and g_Entity[e]['activated'] == 1 then
		if g_Time > rwait[e] then
			current_level[e] = math.random(50,lightcontrol[e].max_light_distance)
			if lightcontrol[e].light_object_no > 0 then SetEntityEmissiveStrength(lightcontrol[e].light_object_no,current_level[e]) end
		end	
		rwait[e] = g_Time + math.random(5,50)
	end

	--Sound--
	if current_level[e] > minrange[e]+2 and current_level[e] < minrange[e] + 15 and played[e] == 0 then
		PlaySound(e,0)
		played[e] = 1
	end
	if current_level[e] == lightcontrol[e].max_light_distance then StopSound(e,0) end
	if current_level[e] <= minrange[e] then played[e] = 0 end
end

