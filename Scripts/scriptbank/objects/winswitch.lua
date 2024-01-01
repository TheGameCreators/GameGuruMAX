-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- WinSwitch v12: by Necrym59 and Lee
-- DESCRIPTION: This object will be treated as a switch object to end the level.
-- DESCRIPTION: Edit the [PROMPT_TEXT$="to End Level"] Play the audio <Sound0> when the object is switched ON by the player. 
-- DESCRIPTION: Select [@GoToLevelMode=1(1=Use Storyboard Logic,2=Go to Specific Level)] controls whether the next level in the Storyboard, or another level is loaded after the switch is turned on.
-- DESCRIPTION: Set the [USE_RANGE=90(1,200)] 
-- DESCRIPTION: [ResetStates!=0] when entering the new level

local U = require "scriptbank\\utillib"
local winswitch 	= {}
local use_range 	= {}
local prompt_text 	= {}
local resetstates	= {}
local doonce		= {}
local tEnt 			= {}
local selectobj 	= {}

function winswitch_properties(e, prompt_text, gotolevelmode, use_range, resetstates)
	winswitch[e].prompt_text = prompt_text or ""
	-- gotolevelmode not used any more
	if use_range == nil then use_range = 100 end
	winswitch[e].use_range = use_range
	if resetstates == nil then resetstates = 0 end
	winswitch[e].resetstates = resetstates
	winswitch[e].initialstate = 0
end 

function winswitch_init(e)	
	winswitch[e] = {}
	winswitch[e].use_range = 90
	winswitch[e].prompt_text = "to End Level"
	winswitch[e].resetstates = 0
	winswitch[e].initialstate = 0
	doonce[e] = 0
	tEnt[e]	= 0
	selectobj[e] = 0
end

function winswitch_main(e)	
	local PlayerDist = GetPlayerDistance(e)
	if winswitch[e].initialstate == 0 then SetActivatedWithMP(e,101) end
	
	if PlayerDist < winswitch[e].use_range and g_PlayerHealth > 0 then
		-- pinpoint select object--
		local px, py, pz = GetCameraPositionX(0), GetCameraPositionY(0), GetCameraPositionZ(0)
		local rayX, rayY, rayZ = 0,0,winswitch[e].use_range
		local paX, paY, paZ = math.rad(GetCameraAngleX(0)), math.rad(GetCameraAngleY(0)), math.rad(GetCameraAngleZ(0))
		rayX, rayY, rayZ = U.Rotate3D(rayX, rayY, rayZ, paX, paY, paZ)
		selectobj[e]=IntersectAll(px,py,pz, px+rayX, py+rayY, pz+rayZ,e)
		if selectobj[e] ~= 0 or selectobj[e] ~= nil then
			if g_Entity[e].obj == selectobj[e] then
				TextCenterOnXColor(50-0.01,50,3,"+",255,255,255) --highliting (with crosshair at present)
				tEnt[e] = e
			else 
				tEnt[e] = 0
			end
			if g_Entity[e]['obj'] ~= selectobj[e] then selectobj[e] = 0 end
		end
		if selectobj[e] == 0 or selectobj[e] == nil then
			tEnt[e] = 0
			TextCenterOnXColor(50-0.01,50,3,"+",155,155,155) --highliting (with crosshair at present)
		end
		--end pinpoint select object--
	end
	
	if PlayerDist < winswitch[e].use_range and tEnt[e] ~= 0 then
		if g_Entity[e].activated == 101 then
			if GetGamePlayerStateXBOX() == 1 then
				PromptLocalForVR(e,"Y Button " .. winswitch[e].prompt_text,3)
			else
				if GetHeadTracker() == 1 then
					PromptLocalForVR(e,"Trigger " ..winswitch[e].prompt_text,3)
				else
					PromptLocalForVR(e,"E " .. winswitch[e].prompt_text,3)
				end
			end
			if g_KeyPressE == 1 then
				SetActivatedWithMP(e,201)
				SetAnimationName(e,"on")
				PlayAnimation(e) 
				PerformLogicConnections(e)
				if doonce[e] == 0 then 
					PlaySound(e,0)
					doonce[e] = 10
				end
				JumpToLevelIfUsedEx(e,winswitch[e].resetstates)
			end			
		end
	end
end
