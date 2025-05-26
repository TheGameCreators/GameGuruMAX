-- Zipline v5 by Necrym 59 with thanks to smallg
-- DESCRIPTION: Apply to two connected zip anchor objects with same name.
-- DESCRIPTION: Set both to Physics=ON, IsImobile=Yes, Always Active=ON.
-- DESCRIPTION: Place the unique named zipline object anywhere nearby and type its name into the behavior
-- DESCRIPTION: [PROMPT_TEXT$="E to use"]
-- DESCRIPTION: [USE_RANGE=100(1-300)]
-- DESCRIPTION: [@ZIP_TYPE=2(1=High to Low Travel, 2=Two Way Travel)]
-- DESCRIPTION: [ZIP_SPEED#=0.10(0.01,2.00)]
-- DESCRIPTION: [@ZIP_RELEASE=2(1=Manual Release, 2=Auto Release)]
-- DESCRIPTION: [ZIP_RELEASE_TEXT$="Q to release"]
-- DESCRIPTION: [ZIPLINE_OBJECT$=""] eg: zipline
-- DESCRIPTION: [@ANCHOR_STATE=1(1=Visible, 2=Invisible)]
-- DESCRIPTION: [#RELEASE_ADJUSTMENT=0.10(0.10,3.00)]
-- DESCRIPTION: <Sound0> loop for zipline travel

local U = require "scriptbank\\utillib"

local lower = string.lower
local zipline = {}
local zipline_name = {}
local prompt_text = {}
local use_range = {}
local zip_type = {}
local zip_speed = {}	
local zip_release = {}
local zip_release_text = {}
local zipline_object = {}
local zipline_objnum = {}	
local anchor_state = {}
local release_adjustment = {}

local other_point = {}
local state = {}
local total_distance = {}
local line_distance = {}
local line_angleY = {}
local line_angleX = {}
local anchor1 = {}
local anchor2 = {}
local playerheight ={}
local dropdown = {}
	
function zipline_properties(e, prompt_text, use_range, zip_type, zip_speed, zip_release, zip_release_text, zipline_object, anchor_state, release_adjustment)
	zipline[e].prompt_text = prompt_text
	zipline[e].use_range = use_range
	zipline[e].zip_type = zip_type
	zipline[e].zip_speed = zip_speed	
	zipline[e].zip_release = zip_release
	zipline[e].zip_release_text = zip_release_text
	zipline[e].zipline_object = lower(zipline_object)
	zipline[e].anchor_state = anchor_state
	zipline[e].release_adjustment = release_adjustment or 0
end 

function zipline_init_name(e,name)
	zipline[e] = {}
	zipline[e].prompt_text = ""
	zipline[e].use_range = 80
	zipline[e].zip_type = 2
	zipline[e].zip_speed = 0.2
	zipline[e].zip_release = 2
	zipline[e].zip_release_text = ""
	zipline[e].zipline_object = ""
	zipline[e].anchor_state = 1
	zipline[e].release_adjustment = 1.03	
	zipline_name[e] = name
	zipline_objnum[e] = 0
	other_point[e] = 0
	state[e] = "init"
	playerheight[e] = 35
	line_distance[e] = 0
	anchor1[e] = 0
	anchor2[e] = 0
end 

function zipline_main(e)
	PlayerDist = GetPlayerDistance(e)
	
	if state[e] == "init" then
		line_distance[e] = 0
		if zipline[e].anchor_state == nil then zipline[e].anchor_state = 1 end
		if zipline[e].zip_release == nil then zipline[e].zip_release = 1 end
		if zipline[e].zip_type == nil then zipline[e].zip_type = 1 end
		if zipline[e].zipline_object ~= "" then
			for n = 1, g_EntityElementMax do
				if n ~= nil and g_Entity[n] ~= nil then
					if lower(GetEntityName(n)) == zipline[e].zipline_object then
						zipline_objnum[e] = n
					end			
				end
			end
		end
		state[e] = "wait"
	end
	if other_point[e] == 0 then 
		for a,b in pairs (zipline_name) do
			if zipline_name[e] == zipline_name[a] and a ~= e then 
				other_point[e] = a
			end 
		end 
		CollisionOff(e)
		RotateToEntity(e,other_point[e])
		---Build zipline-------------------------------------------------------------
		if zipline[e].zipline_object ~= "" then			
			line_distance[e] = GetDistance(e,other_point[e])*1.03
			line_angleY[e] = RotateToEntity(other_point[e],e)-180
			line_angleX[e] = GetXYRotationToPoint(other_point[e],g_Entity[e].x,g_Entity[e].y,g_Entity[e].z)
			ResetPosition(zipline_objnum[e],g_Entity[e].x, g_Entity[e].y+6, g_Entity[e].z)
			ResetRotation(zipline_objnum[e],line_angleX[e],line_angleY[e],0)
			ScaleObject(g_Entity[zipline_objnum[e]]['obj'],100,100,line_distance[e])
		end	
		-----------------------------------------------------------------------------
		if zipline[e].anchor_state == 2 then Hide(e) end		
	else		
		if state[e] == "wait" then						
			if PlayerDist < zipline[e].use_range and g_PlayerHealth > 0 then
				local LookingAt = GetPlrLookingAtEx(e,1)
				if LookingAt == 1 then
					if zipline[e].zip_type == 2 or g_Entity[e].y >= g_Entity[other_point[e]].y then				
						if zipline[e].zip_release == 1 then PromptLocal(e,zipline[e].prompt_text.. " " ..zipline[e].zip_release_text) end
						if zipline[e].zip_release == 2 then PromptLocal(e,zipline[e].prompt_text) end
						if g_KeyPressE == 1 then							
							anchor1[e] = g_Entity[e].y
							anchor2[e] = g_Entity[other_point[e]].y						
							total_distance[0] = g_Entity[other_point[e]].x - g_Entity[e].x
							total_distance[1] = g_Entity[other_point[e]].y - g_Entity[e].y
							total_distance[2] = g_Entity[other_point[e]].z - g_Entity[e].z						
							SetFreezeAngle(0,GetEntityAngleY(e),0)								
							SetFreezePosition(g_Entity[e].x,g_Entity[e].y,g_Entity[e].z)
							total_distance[3] = GetPlayerFlatDistance(other_point[e])
							TransportToFreezePosition()
							state[e] = "zip"							
						end 
					end 
				end
			end			
		elseif state[e] == "zip" then
			if zipline[e].zip_release == 1 then Prompt(zipline[e].zip_release_text) end
			local perc = GetPlayerFlatDistance(other_point[e])/total_distance[3]
			dropdown[e] = total_distance[1] * (1 - perc)			
			SetFreezePosition(g_PlayerPosX+(total_distance[0]*(zipline[e].zip_speed/60)),(anchor1[e]-playerheight[e])+dropdown[e],g_PlayerPosZ+(total_distance[2]*(zipline[e].zip_speed/60)))
			TransportToFreezePositionOnly()				
			LoopSound(e,0)
			if zipline[e].zip_release == 1 and g_KeyPressQ == 1 then
				state[e] = "drop"
				state[other_point[e]] = "drop"
				StopSound(e,0)
			end
			if zipline[e].zip_release == 1 and g_KeyPressQ == 0 then
				if GetPlayerFlatDistance(other_point[e]) <= zipline[e].use_range*zipline[e].release_adjustment then
					state[e] = "drop"
					state[other_point[e]] = "drop"
					StopSound(e,0)
				end
			end
			if zipline[e].zip_release == 2 then 
				if GetPlayerFlatDistance(other_point[e]) <= zipline[e].use_range*zipline[e].release_adjustment then
					state[e] = "drop"
					state[other_point[e]] = "drop"
					StopSound(e,0)
				end
			end
		end
		if state[e] == "drop" then			
			state[e] = "wait"
			StopSound(e,0)
		end		
	end	
end 

function zipline_exit(e)
end 

function GetDistance(e,v)
	if g_Entity[e] ~= nil and g_Entity[e] ~= 0 and g_Entity[v] ~= nil and g_Entity[v] ~= 0 then
		local disx = g_Entity[e]['x'] - g_Entity[v]['x']
		local disz = g_Entity[e]['z'] - g_Entity[v]['z']
		local disy = g_Entity[e]['y'] - g_Entity[v]['y']
		return math.sqrt(disx^2 + disz^2 + disy^2)
	end
end

function GetPlayerFlatDistance(e)
	tPlayerDX = (g_Entity[e]['x'] - g_PlayerPosX)
	tPlayerDZ = (g_Entity[e]['z'] - g_PlayerPosZ)
	return math.sqrt(math.abs(tPlayerDX*tPlayerDX)+math.abs(tPlayerDZ*tPlayerDZ))
end

function RotateToEntity(e,v)
	if g_Entity[e] ~= nil and g_Entity[e] ~= 0 and g_Entity[v] ~= nil and g_Entity[v] ~= 0 then
		local x = g_Entity[v].x - g_Entity[e].x
		local z = g_Entity[v].z - g_Entity[e].z
		local angle = math.atan2(x,z)
		angle = angle * (180.0 / math.pi)
		if angle < 0 then
			angle = 360 + angle
		elseif angle > 360 then
			angle = angle - 360
		end
		SetRotation(e,0,angle,0)
		return angle
	end
end

function GetXYRotationToPoint(e,x,y,z)
	local x1,y1,z1 = x,y,z
	local x2,y2,z2 = GetEntityPosAng(e)
	local tay = math.atan2( x2 - x1, z2 - z1 ) * ( 180.0 / math.pi )
	local nx = math.abs(x2 - x1)
	local nz = math.abs(z2 - z1)
	local tax = 0
	if nx > nz then 
	else
		tax = math.atan2( x2 - x1, y2 - y1 ) * ( 180.0 / math.pi )
	end
	local dx, dz = x2 - x1, z2 - z1
	local dist = math.sqrt(dx*dx+dz*dz)
	tax = math.atan2(dist , y2 - y1 ) * ( 180.0 / math.pi )
	tax = tax - 90
	return tax,tay,0
end