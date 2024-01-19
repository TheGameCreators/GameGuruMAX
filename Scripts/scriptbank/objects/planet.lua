-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Planet v5 by Necrym59
-- DESCRIPTION: Allows a planet object made using 3 identical sized spherical objects, a planet sphere, cloud sphere, atmosphere sphere, plus an optional circular rings object.
-- DESCRIPTION: and can trigger Logic Linked or IfUsed game elements or change lavel.
-- DESCRIPTION: Attach this behavior to the planet sphere object.
-- DESCRIPTION: [#PLANET_ADJUST_X=0.0(-20000.0,20000.0)]
-- DESCRIPTION: [#PLANET_ADJUST_Y=0.0(-20000.0,20000.0)]
-- DESCRIPTION: [#PLANET_ADJUST_Z=0.0(-20000.0,20000.0)]
-- DESCRIPTION: [CLOUD_SPHERE$=""]
-- DESCRIPTION: [ATMOS_SPHERE$=""]
-- DESCRIPTION: [RINGS_CIRCLE$=""]
-- DESCRIPTION: [CLOUD_SCALE=10]
-- DESCRIPTION: [ATMOS_SCALE=15]
-- DESCRIPTION: [RINGS_SCALE=25]
-- DESCRIPTION: [PLANET_ROTATION=1(0,100)]
-- DESCRIPTION: [CLOUD_ROTATION=5(0,100)]
-- DESCRIPTION: [RINGS_ROTATION=1(0,100)]
-- DESCRIPTION: [SENSE_RANGE=1000]
-- DESCRIPTION: [SENSE_TEXT$="Detected in Range"]
-- DESCRIPTION: [@SENSE_TRIGGER=1(1=Linked, 2=Video+ChangeLevel)]
-- DESCRIPTION: [@VIDEO_SKIP=1(1=Yes, 2=No)]
-- DESCRIPTION: [@GoToLevelMode=1(1=Use Storyboard Logic,2=Go to Specific Level)] controls whether to load the next level in the Storyboard, or a specific level.
-- DESCRIPTION: [ResetStates!=0] when entering the new level
-- DESCRIPTION: <Video Slot> for optional video

local lower = string.lower

local planet 			= {}
local planet_adjust_x	= {}
local planet_adjust_y	= {}
local planet_adjust_z	= {}
local cloud_sphere  	= {}
local atmos_sphere  	= {}
local cloud_scale  		= {}
local atmos_scale  		= {}
local planet_rotation	= {}
local cloud_rotation	= {}
local rings_rotation	= {}
local cloud_number  	= {}
local atmos_number  	= {}
local rings_number  	= {}
local sense_range		= {}
local sense_text		= {}
local sense_trigger		= {}
local video_skip		= {}
local resetstates		= {}

local planetposx	= {}
local planetposy	= {}
local planetposz	= {}
local planetangx	= {}
local planetangy	= {}
local planetangz	= {}
local planetscalex	= {}
local planetscaley	= {}
local planetscalez	= {}
local adjustrange	= {}
local status		= {}
local doonce		= {}
local endvid 		= {}

function planet_properties(e, planet_adjust_x, planet_adjust_y, planet_adjust_z, cloud_sphere, atmos_sphere, rings_circle, cloud_scale, atmos_scale, rings_scale, planet_rotation, cloud_rotation, rings_rotation, sense_range, sense_text, sense_trigger, video_skip, resetstates)
	planet[e] = g_Entity[e]
	planet[e].planet_adjust_x = planet_adjust_x
	planet[e].planet_adjust_y = planet_adjust_y
	planet[e].planet_adjust_z = planet_adjust_z
	planet[e].cloud_sphere = lower(cloud_sphere) or ""
	planet[e].atmos_sphere = lower(atmos_sphere) or ""
	planet[e].rings_circle = lower(rings_circle) or ""
	planet[e].cloud_scale = cloud_scale
	planet[e].atmos_scale = atmos_scale
	planet[e].rings_scale = rings_scale
	planet[e].planet_rotation = planet_rotation
	planet[e].cloud_rotation = cloud_rotation
	planet[e].rings_rotation = rings_rotation
	planet[e].cloud_number = 0
	planet[e].atmos_number = 0
	planet[e].rings_number = 0
	planet[e].sense_range = sense_range
	planet[e].sense_text = sense_text
	planet[e].sense_trigger = sense_trigger	
	planet[e].video_skip = video_skip	
	planet[e].resetstates = resetstates
end

function planet_init(e)
	planet[e] = {}
	planet[e].planet_adjust_x = 0
	planet[e].planet_adjust_y = 0
	planet[e].planet_adjust_z = 0	
	planet[e].cloud_sphere = ""
	planet[e].atmos_sphere = ""
	planet[e].rings_circle = ""	
	planet[e].cloud_scale = 10
	planet[e].atmos_scale = 15
	planet[e].rings_scale = 25	
	planet[e].planet_rotation = 0
	planet[e].cloud_rotation = 5
	planet[e].rings_rotation = 1
	planet[e].cloud_number = 0
	planet[e].atmos_number = 0
	planet[e].rings_number = 0
	planet[e].sense_range = 1000
	planet[e].sense_text = "Detected in Range"
	planet[e].sense_trigger = 1
	planet[e].video_skip = 2
	planet[e].resetstates = 0
	
	planetposx[e] = 0
	planetposy[e] = 0
	planetposz[e] = 0
	planetangx[e] = 0
	planetangy[e] = 0
	planetangz[e] = 0
	planetscalex[e] = 0
	planetscaley[e] = 0
	planetscalez[e] = 0
	adjustrange[e] = 0
	doonce[e] = 0
	endvid[e] = 0
	status[e] = "planet_build"
end

function planet_main(e)
	
	if status[e] == "planet_build" then
		GravityOff(e)
		planetposx[e],planetposy[e],planetposz[e],planetangx[e],planetangy[e],planetangz[e] = GetEntityPosAng(e)
		planetposx[e] = planetposx[e] + planet[e].planet_adjust_x
		planetposy[e] = planetposy[e] + planet[e].planet_adjust_y
		planetposz[e] = planetposz[e] + planet[e].planet_adjust_z
		ResetPosition(e,planetposx[e],planetposy[e],planetposz[e])
		planetscalex[e],planetscaley[e],planetscalez[e] = GetEntityScales(e)
		adjustrange[e] = planet[e].sense_range * GetEntityScales(e)
		if planet[e].cloud_number == 0 and planet[e].cloud_sphere > "" then
			for n = 1, g_EntityElementMax do
				if n ~= nil and g_Entity[n] ~= nil then
					if lower(GetEntityName(n)) == planet[e].cloud_sphere then 
						planet[e].cloud_number = n
						GravityOff(n)
						ResetPosition(n,planetposx[e],planetposy[e],planetposz[e])
						ScaleObject(g_Entity[planet[e].cloud_number]['obj'],planetscalex[e]*100+planet[e].cloud_scale,planetscaley[e]*100+planet[e].cloud_scale,planetscalez[e]*100+planet[e].cloud_scale)
						break
					end
				end
			end
		end
		if planet[e].atmos_number == 0 and planet[e].atmos_sphere > "" then
			for m = 1, g_EntityElementMax do
				if m ~= nil and g_Entity[m] ~= nil then
					if lower(GetEntityName(m)) == planet[e].atmos_sphere then 
						planet[e].atmos_number = m
						GravityOff(m)
						ResetPosition(m,planetposx[e],planetposy[e],planetposz[e])	
						ScaleObject(g_Entity[planet[e].atmos_number]['obj'],planetscalex[e]*100+planet[e].atmos_scale,planetscaley[e]*100+planet[e].atmos_scale,planetscalez[e]*100+planet[e].atmos_scale)
						break
					end
				end
			end
		end
		if planet[e].rings_number == 0 and planet[e].rings_circle > "" then
			for o = 1, g_EntityElementMax do
				if o ~= nil and g_Entity[o] ~= nil then
					if lower(GetEntityName(o)) == planet[e].rings_circle then 
						planet[e].rings_number = o
						GravityOff(o)
						ResetPosition(o,planetposx[e],planetposy[e],planetposz[e])
						ScaleObject(g_Entity[planet[e].rings_number]['obj'],planetscalex[e]*100+planet[e].rings_scale*100,planetscaley[e]*100+planet[e].rings_scale*100,planetscalez[e]*100+planet[e].rings_scale*100)
						break
					end
				end
			end
		end
		status[e] = "planet_process"
	end
	if status[e] == "planet_process" then
		RotateY(e,GetAnimationSpeed(e)*planet[e].planet_rotation/5)
		if planet[e].cloud_number ~= 0 then RotateY(planet[e].cloud_number,GetAnimationSpeed(e)*planet[e].cloud_rotation/5) end
		if planet[e].rings_number ~= 0 then RotateY(planet[e].rings_number,GetAnimationSpeed(e)*planet[e].rings_rotation/5) end

		if GetPlayerDistance(e) < adjustrange[e] then
			if doonce[e] == 0 then
				if planet[e].sense_trigger == 1 then
					PromptDuration(planet[e].sense_text,5000)
					PerformLogicConnections(e)
					ActivateIfUsed(e)
					doonce[e] = 1
				end
				if planet[e].sense_trigger == 2 then
					if endvid[e] == 0 then
						if planet[e].video_skip == 1 then PromptVideo(e,1) end
						if planet[e].video_skip == 2 then PromptVideoNoSkip(e,1) end
						endvid[e] = 1
					end
					if endvid[e] == 1 then
						JumpToLevelIfUsedEx(e, planet[e].resetstates)					
					end
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