-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Moon v3 by Necrym59
-- DESCRIPTION: Allows a moon object to rotate and orbit another named object
-- DESCRIPTION: Attach this behavior to the moon object.
-- DESCRIPTION: [#MOON_ADJUST_X=0.0(-20000.0,20000.0)]
-- DESCRIPTION: [#MOON_ADJUST_Y=0.0(-20000.0,20000.0)]
-- DESCRIPTION: [#MOON_ADJUST_Z=0.0(-20000.0,20000.0)]
-- DESCRIPTION: [MOON_ROTATION=1(0,100)]
-- DESCRIPTION: [ORBIT_PLANET_NAME$=""]
-- DESCRIPTION: [ORBIT_DISTANCE=300(0,1000)]
-- DESCRIPTION: [#ORBIT_SPEED=0.1(0.0,100.0)]
-- DESCRIPTION: [SENSE_RANGE=1000]
-- DESCRIPTION: [SENSE_TEXT$="Detected in Range"]
-- DESCRIPTION: [@SENSE_TRIGGER=1(1=Linked, 2=Video+ChangeLevel, 3=Video)]
-- DESCRIPTION: [@VIDEO_SKIP=1(1=Yes, 2=No)]
-- DESCRIPTION: [@GoToLevelMode=1(1=Use Storyboard Logic,2=Go to Specific Level)] controls whether to load the next level in the Storyboard, or a specific level.
-- DESCRIPTION: [ResetStates!=0] when entering the new level
-- DESCRIPTION: <Video Slot> for optional video

local lower = string.lower

local moon 				= {}
local moon_adjust_x		= {}
local moon_adjust_y		= {}
local moon_adjust_z		= {}
local moon_rotation		= {}
local orbit_planet_name	= {}
local orbit_distance	= {}
local orbit_speed		= {}
local sense_range		= {}
local sense_text		= {}
local sense_trigger		= {}
local video_skip		= {}
local resetstates		= {}
local orbit_planet_no	= {}

local moonposx		= {}
local moonposy		= {}
local moonposz		= {}
local moonangx		= {}
local moonangy		= {}
local moonangz		= {}
local moonscalex	= {}
local moonscaley	= {}
local moonscalez	= {}
local planetposx	= {}
local planetposy	= {}
local planetposz	= {}
local planetangx	= {}
local planetangy	= {}
local planetangz	= {}
local adjustrange	= {}
local orbit			= {}
local status		= {}
local doonce		= {}
local endvid 		= {}

function moon_properties(e, moon_adjust_x, moon_adjust_y, moon_adjust_z, moon_rotation, orbit_planet_name, orbit_distance, orbit_speed, sense_range, sense_text, sense_trigger, video_skip, resetstates)
	moon[e] = g_Entity[e]
	moon[e].moon_adjust_x = moon_adjust_x
	moon[e].moon_adjust_y = moon_adjust_y
	moon[e].moon_adjust_z = moon_adjust_z
	moon[e].moon_rotation = moon_rotation
	moon[e].orbit_planet_name = lower(orbit_planet_name) or ""
	moon[e].orbit_distance = orbit_distance
	moon[e].orbit_speed = orbit_speed	
	moon[e].sense_range = sense_range
	moon[e].sense_text = sense_text
	moon[e].sense_trigger = sense_trigger	
	moon[e].video_skip = video_skip	
	moon[e].resetstates = resetstates
	moon[e].orbit_planet_no = 0
end

function moon_init(e)
	moon[e] = {}
	moon[e].moon_adjust_x = 0
	moon[e].moon_adjust_y = 0
	moon[e].moon_adjust_z = 0	
	moon[e].moon_rotation = 0
	moon[e].orbit_planet_name = ""	
	moon[e].orbit_distance = 300
	moon[e].orbit_speed = 0.01
	moon[e].sense_range = 1000
	moon[e].sense_text = "Detected in Range"
	moon[e].sense_trigger = 1
	moon[e].video_skip = 2
	moon[e].resetstates = 0
	moon[e].orbit_planet_no = 0
	
	moonposx[e] = 0
	moonposy[e] = 0
	moonposz[e] = 0
	moonangx[e] = 0
	moonangy[e] = 0
	moonangz[e] = 0
	moonscalex[e] = 0
	moonscaley[e] = 0
	moonscalez[e] = 0
	planetposx[e] = 0
	planetposy[e] = 0
	planetposz[e] = 0
	planetangx[e] = 0
	planetangy[e] = 0 
	planetangz[e] = 0
	adjustrange[e] = 0
	orbit[e] = 0
	doonce[e] = 0
	endvid[e] = 0
	status[e] = "moon_build"
end

function moon_main(e)
	
	if status[e] == "moon_build" then
		GravityOff(e)
		moonposx[e],moonposy[e],moonposz[e],moonangx[e],moonangy[e],moonangz[e] = GetEntityPosAng(e)
		moonposx[e] = moonposx[e] + moon[e].moon_adjust_x
		moonposy[e] = moonposy[e] + moon[e].moon_adjust_y
		moonposz[e] = moonposz[e] + moon[e].moon_adjust_z
		ResetPosition(e,moonposx[e],moonposy[e],moonposz[e])
		moonscalex[e],moonscaley[e],moonscalez[e] = GetEntityScales(e)
		adjustrange[e] = moon[e].sense_range * GetEntityScales(e)
		if moon[e].orbit_planet_no == 0 and moon[e].orbit_planet_name > "" then
			for p = 1, g_EntityElementMax do
				if p ~= nil and g_Entity[p] ~= nil then
					if lower(GetEntityName(p)) == moon[e].orbit_planet_name then 
						moon[e].orbit_planet_no = p
						planetposx[e],planetposy[e],planetposz[e],planetangx[e],planetangy[e],planetangz[e] = GetEntityPosAng(p)
						moonposx[e] = planetposx[e]
						moonposy[e] = planetposy[e]
						moonposz[e] = planetposz[e]+moon[e].orbit_distance
						ResetPosition(e,moonposx[e],moonposy[e],moonposz[e])
						break
					end
				end
			end
		end
		status[e] = "moon_process"
	end
	if status[e] == "moon_process" then
		RotateY(e,GetAnimationSpeed(e)*moon[e].moon_rotation/5)
		if moon[e].orbit_planet_no ~= 0 then
			-- ORBITAL ROTATION --	
			if orbit[e] < 100 then 
				local new_x = moonposx[e] + math.sin(orbit[e]) * moon[e].orbit_distance
				local new_y = moonposy[e]
				local new_z = planetposz[e] + math.cos(orbit[e]) * moon[e].orbit_distance
				orbit[e] = orbit[e] + moon[e].orbit_speed/10000
				PositionObject(g_Entity[e]['obj'],new_x,new_y,new_z)
				if orbit[e] >= 100 then orbit[e] = 0 end
			end	
		end		

		if GetPlayerDistance(e) < adjustrange[e] then
			if doonce[e] == 0 then
				if moon[e].sense_trigger == 1 then
					PromptDuration(moon[e].sense_text,5000)
					PerformLogicConnections(e)
					ActivateIfUsed(e)
					doonce[e] = 1
				end
				if moon[e].sense_trigger == 2 then
					if endvid[e] == 0 then
						if moon[e].video_skip == 1 then PromptVideo(e,1) end
						if moon[e].video_skip == 2 then PromptVideoNoSkip(e,1) end
						endvid[e] = 1
					end
					if endvid[e] == 1 then
						JumpToLevelIfUsedEx(e, moon[e].resetstates)					
					end
					doonce[e] = 1
				end
				if moon[e].sense_trigger == 3 then
					if moon[e].video_skip == 1 then PromptVideo(e,1) end
					if moon[e].video_skip == 2 then PromptVideoNoSkip(e,1) end
					doonce[e] = 1
				end
			end	
		end
		if GetPlayerDistance(e) > adjustrange[e] then
			if doonce[e] == 1 then
				doonce[e] = 0
				endvid[e] = 0
			end
		end
	end
end