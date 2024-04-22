-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Hologram v7 by Necrym59
-- DESCRIPTION: Attach an object. Link to switch to activate or set switched on.
-- DESCRIPTION: [PROMPT_TEXT$="Displaying Hologram"] for the displaying
-- DESCRIPTION: [@HOLOGRAM_MODE=1(1=Fixed, 2=Grow)] type of hologram
-- DESCRIPTION: [HOLOGRAM_ROTATION=0(0,100)] rotation speed of hologram
-- DESCRIPTION: [HOLOGRAM_NAME$="Holo-gram"] name of a hologram object.
-- DESCRIPTION: [HOLOBEAM_NAME$="Holo-beam"] name of a holobeam object if required
-- DESCRIPTION: [@SWITCHED=1(1=Off, 2=On)] Start switched on or off
-- DESCRIPTION: <Sound0> Hologram on sound

local hologram = {}
local prompt_text = {}
local hologram_mode = {}
local hologram_rotation = {}
local hologram_name = {}
local holobeam_name = {}
local switched = {}
local hologram_number = {}
local holobeam_number = {}

local status = {}
local growtime = {}
local emstrength = {}
local doonce = {}
	
function hologram_properties(e, prompt_text, hologram_mode, hologram_rotation, hologram_name, holobeam_name, switched)
	hologram[e].prompt_text = prompt_text	
	hologram[e].hologram_mode = hologram_mode	
	hologram[e].hologram_rotation = hologram_rotation
	hologram[e].hologram_name = hologram_name
	hologram[e].holobeam_name = string.lower(holobeam_name)
	hologram[e].switched = switched	
end

function hologram_init(e)
	hologram[e] = {}
	hologram[e].prompt_text = "Displaying Hologram"
	hologram[e].hologram_mode = 1	
	hologram[e].hologram_rotation = 0
	hologram[e].hologram_name = ""	
	hologram[e].holobeam_name = ""
	hologram[e].switched = 1
	hologram[e].hologram_number	= 0
	hologram[e].holobeam_number = 0 	
	
	growtime[e] = 1
	SetEntityTransparency(e,1)
	SetEntityEmissiveStrength(e,0)
	emstrength[e] = 0
	doonce[e] = 0
	status[e] = "init"
end

function hologram_main(e)
	
	if status[e] == "init" then
		if hologram[e].hologram_name > "" and hologram[e].hologram_number == 0 then
			for h = 1, g_EntityElementMax do			
				if h ~= nil and g_Entity[h] ~= nil then		
					if string.lower(GetEntityName(h)) == string.lower(hologram[e].hologram_name) then
						hologram[e].hologram_number = h
						Hide(h)
						break
					end					
				end
			end
		end			
		if hologram[e].holobeam_name > "" and hologram[e].holobeam_number == 0 then
			for a = 1, g_EntityElementMax do			
				if a ~= nil and g_Entity[a] ~= nil then		
					if string.lower(GetEntityName(a)) == string.lower(hologram[e].holobeam_name) then
						hologram[e].holobeam_number = a
						Hide(a)						
						break
					end					
				end
			end
		end		
		if hologram[e].switched == 1 then
			SetActivated(e,0)
			g_Entity[e]['activated'] = 0			
		end
		if hologram[e].switched == 2 then
			SetActivated(e,1)
			g_Entity[e]['activated'] = 1
		end
		status[e] = "end_init"
	end	
	
	if g_Entity[e]['activated'] == 1 then
		local PlayerDist = GetPlayerDistance(e)
		if doonce[e] == 0 then	
			PromptDuration(hologram[e].prompt_text,1000)			
			doonce[e] = 1
		end	
		if hologram[e].hologram_mode == 1 then		    
			Show(hologram[e].hologram_number)			
			CollisionOff(hologram[e].hologram_number)
			PlaySound(hologram[e].hologram_number,0)
		end
		if hologram[e].hologram_mode == 2 then				
			if growtime[e] < 100 then				
				Show(hologram[e].hologram_number)				
				Scale(hologram[e].hologram_number,growtime[e])
				SetEntityEmissiveStrength(hologram[e].holobeam_number,growtime[e])
				if hologram[e].holobeam_number > 0 then
					Show(hologram[e].holobeam_number)
					Scale(hologram[e].holobeam_number,growtime[e])
				end
				growtime[e] = growtime[e] + 0.5				
			end
			if hologram[e].hologram_rotation > 0  then
				RotateY(hologram[e].hologram_number,GetAnimationSpeed(hologram[e].hologram_number)*hologram[e].hologram_rotation)
			end
			CollisionOff(hologram[e].hologram_number)
			PlaySound(e,0)
		end
		if hologram[e].holobeam_number > 0 then -- Set Holobeam Object
			Show(hologram[e].holobeam_number)			
		end			
		SetEntityEmissiveStrength(hologram[e].hologram_number,math.random(38,42))
		SetEntityEmissiveStrength(hologram[e].holobeam_number,math.random(38))
	end
	if g_Entity[e]['activated'] == 0 then
		Hide(hologram[e].hologram_number)				
		SetEntityEmissiveStrength(hologram[e].hologram_number,0)
		if hologram[e].holobeam_number > 0 then
			Hide(hologram[e].holobeam_number)
			SetEntityEmissiveStrength(hologram[e].holobeam_number,0)
		end	
		doonce[e] = 0
		growtime[e] = 0
	end
end