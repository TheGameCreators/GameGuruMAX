-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Zipline v3  
-- DESCRIPTION: Apply to two connected zip point anchor objects with same name. Set Static Mode=NO, Physics=ON Always Active=ON
-- DESCRIPTION: Change the [PROMPT_TEXT$="E to use"] and [USE_RANGE=100(1-300)], also the [@ZIP_TYPE=1(1=High to Low Travel, 2=Two Way Travel)] [ZIP_SPEED#=0.10(0.01,2.00)]
-- DESCRIPTION: [@ZIP_RELEASE=2(1=Manual Hold/Release, 2=Auto Release)] [ZIP_RELEASE_TEXT$="Hold E until ready to release"], Zipline anchor point [@VISIBILITY=1(1=Visible, 2=Invisible)] 
-- DESCRIPTION: <Sound0> loop for zipline travel

	g_zipline = {}
	local zipline_name = {}
	local other_point = {}
	local state = {}
	local total_distance = {}
	local zip_type = {}
	local zip_speed = {}	
	local zip_release = {}
	local zip_release_text = {}
	local visibility = {}
	local use_range = {}
	local prompt_text = {}
	local anchor1 = {}
	local anchor2 = {}
	local playerheight ={}
	local dropdown = {}
	
function zipline_properties(e, prompt_text, use_range, zip_type, zip_speed, zip_release, zip_release_text, visibility)
	g_zipline[e] = g_Entity[e]
	g_zipline[e]['prompt_text'] = prompt_text
	g_zipline[e]['use_range'] = use_range
	g_zipline[e]['zip_type'] = zip_type
	g_zipline[e]['zip_speed'] = zip_speed	
	g_zipline[e]['zip_release'] = zip_release
	g_zipline[e]['zip_release_text'] = zip_release_text
	g_zipline[e]['visibility'] = visibility
end 

function zipline_init_name(e,name)
	g_zipline[e] = g_Entity[e]
	g_zipline[e]['prompt_text'] = ""
	g_zipline[e]['use_range'] = 80
	g_zipline[e]['zip_type'] = 2
	g_zipline[e]['zip_speed'] = 0.2
	g_zipline[e]['zip_release'] = 2
	g_zipline[e]['zip_release_text'] = ""
	g_zipline[e]['visibility'] = 1
	zipline_name[e] = name
	other_point[e] = 0
	state[e] = "init"
	playerheight[e] = 35
	anchor1[e] = 0
	anchor2[e] = 0	
end 

function zipline_main(e)
	g_zipline[e] = g_Entity[e]
	PlayerDist = GetPlayerDistance(e)
	
	if state[e] == "init" then
		if g_zipline[e]['visibility'] == nil then g_zipline[e]['visibility'] = 1 end
		if g_zipline[e]['zip_release'] == nil then g_zipline[e]['zip_release'] = 1 end
		if g_zipline[e]['zip_type'] == nil then g_zipline[e]['zip_type'] = 1 end				
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
		if g_zipline[e]['visibility'] == 2 then Hide(e) end		
	else		
		if state[e] == "wait" then						
			if PlayerDist < g_zipline[e]['use_range'] and g_PlayerHealth > 0 then
				local LookingAt = GetPlrLookingAtEx(e,1)
				if LookingAt == 1 then
					if g_zipline[e]['zip_type'] == 2 or g_Entity[e]['y'] >= g_Entity[other_point[e]]['y'] then				
						--PromptLocal(e,g_zipline[e]['prompt_text'])
						if g_zipline[e]['zip_release'] == 1 then PromptLocal(e,g_zipline[e]['prompt_text'].. " " ..g_zipline[e]['zip_release_text']) end
						if g_zipline[e]['zip_release'] == 2 then PromptLocal(e,g_zipline[e]['prompt_text']) end
						if g_KeyPressE == 1 then							
							anchor1[e] = g_Entity[e]['y']
							anchor2[e] = g_Entity[other_point[e]]['y']						
							total_distance[0] = g_Entity[other_point[e]]['x'] - g_Entity[e]['x']
							total_distance[1] = g_Entity[other_point[e]]['y'] - g_Entity[e]['y']
							total_distance[2] = g_Entity[other_point[e]]['z'] - g_Entity[e]['z']						
							SetFreezeAngle(0,GetEntityAngleY(e),0)								
							SetFreezePosition(g_Entity[e]['x'],g_Entity[e]['y'],g_Entity[e]['z'])
							total_distance[3] = GetPlayerFlatDistance(other_point[e])
							TransportToFreezePosition()
							state[e] = "zip"
						end 
					end 
				end
			end
		elseif state[e] == "zip" then
			if g_zipline[e]['zip_release'] == 1 then Prompt(g_zipline[e]['zip_release_text']) end
			local perc = GetPlayerFlatDistance(other_point[e])/total_distance[3]
			dropdown[e] = total_distance[1] * (1 - perc)			
			SetFreezePosition(g_PlayerPosX+(total_distance[0]*(g_zipline[e]['zip_speed']/60)),(anchor1[e]-playerheight[e])+dropdown[e],g_PlayerPosZ+(total_distance[2]*(g_zipline[e]['zip_speed']/60)))
			TransportToFreezePositionOnly()				
			LoopSound(e,0)
			if GetPlayerFlatDistance(other_point[e]) <= g_zipline[e]['use_range']*1.3 then
				state[e] = "drop"
				state[other_point[e]] = "drop"
				StopSound(e,0)
			end 
			if g_zipline[e]['zip_release'] == 1 and g_KeyPressE == 0 then 
				state[e] = "drop"
				state[other_point[e]] = "drop"
				StopSound(e,0)
			end			
		elseif state[e] == "drop" then			
			state[e] = "wait"
			StopSound(e,0)
		end	
	end		
end 

function zipline_exit(e)
end 

function GetPlayerFlatDistance(e)
	tPlayerDX = (g_Entity[e]['x'] - g_PlayerPosX)
	tPlayerDZ = (g_Entity[e]['z'] - g_PlayerPosZ)
	return math.sqrt(math.abs(tPlayerDX*tPlayerDX)+math.abs(tPlayerDZ*tPlayerDZ))
end

function RotateToEntity(e,v)
	if g_Entity[e] ~= nil and g_Entity[e] ~= 0 and g_Entity[v] ~= nil and g_Entity[v] ~= 0 then
		local x = g_Entity[v]['x'] - g_Entity[e]['x']
		local z = g_Entity[v]['z'] - g_Entity[e]['z']
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