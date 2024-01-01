-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Hologram v5
-- DESCRIPTION: Attach to the hologram object. Set Always active ON
-- DESCRIPTION: Change the [PROMPT_TEXT$="Displaying Hologram"], Display [@HOLOGRAM_MODE=1(1=Fixed, 2=Grow)],
-- DESCRIPTION: [HOLOGRAM_ROTATION=1(1,100)] [HOLOBEAM_NAME$="Holo-beam"] [@SWITCHED=1(1=Off, 2=On)]
-- DESCRIPTION: <Sound0> Hologram on sound
	

local hologram = {}
local prompt_text = {}
local hologram_mode = {}
local hologram_rotation = {}
local holobeam_name = {}
local holobeam_number = {}
local switched = {}

local status = {}
local growtime = {}
local emstrength = {}
local doonce = {}
	
function hologram_properties(e, prompt_text, hologram_mode, hologram_rotation, holobeam_name, holobeam_number, switched)
	hologram[e] = g_Entity[e]
	hologram[e].prompt_text = prompt_text	
	hologram[e].hologram_mode = hologram_mode	
	hologram[e].hologram_rotation = hologram_rotation
	hologram[e].holobeam_name = string.lower(holobeam_name)
	hologram[e].holobeam_number = 0
	hologram[e].switched = switched
end -- End properties

function hologram_init(e)
	hologram[e] = g_Entity[e]
	hologram[e].prompt_text = "Displaying Hologram"
	hologram[e].hologram_mode = 1	
	hologram[e].hologram_rotation = 0
	hologram[e].holobeam_name = "holo-beam"
	hologram[e].holobeam_number = 0 
	hologram[e].switched = 1	
	growtime[e] = 1
	SetEntityTransparency(e,1)
	SetEntityEmissiveStrength(e,0)
	emstrength[e] = 0
	doonce[e] = 0
	status[e] = "init"
end

function hologram_main(e)
	hologram[e] = g_Entity[e]	
	if status[e] == "init" then	
		Hide(e)		
		if hologram[e].holobeam_number == 0 or nil then
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
			Show(e)		
			if hologram[e].hologram_rotation > 0  then
				RotateY(e,GetAnimationSpeed(e)*hologram[e].hologram_rotation)
			end			
			CollisionOff(e)
			PlaySound(e,0)
		end
		if hologram[e].hologram_mode == 2 then				
			if growtime[e] < 100 then
				Hide(e)
				Scale(e,growtime[e])
				if hologram[e].holobeam_number > 0 then Scale(hologram[e].holobeam_number,growtime[e]) end
				--SetEntityEmissiveStrength(e,GetEntityEmissiveStrength(e)+growtime[e]/2)
				SetEntityEmissiveStrength(e,GetEntityEmissiveStrength(e)+growtime[e])				
				growtime[e] = growtime[e] + 0.5				
				Show(e)				
			end
			if hologram[e].hologram_rotation > 0  then
				RotateY(e,GetAnimationSpeed(e)*hologram[e].hologram_rotation)
			end
			CollisionOff(e)
			PlaySound(e,0)
		end
		if hologram[e].holobeam_number > 0 then --Set Holobeam Object
			Show(hologram[e].holobeam_number)			
		end	
		SetEntityEmissiveStrength(hologram[e].holobeam_number,math.random(38))
	end
	if g_Entity[e]['activated'] == 0 then
		Hide(e)
		SetEntityEmissiveStrength(e,0)
		Hide(hologram[e].holobeam_number)
		doonce[e] = 0
		growtime[e] = 10		
	end
end