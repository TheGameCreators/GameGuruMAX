-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- WinSwitch v2: by Necrym59
-- DESCRIPTION: This object will be treated as a switch object to end the level. Edit the [PROMPT_TEXT$="to End Level"] Play the audio <Sound0> when the object is switched ON by the player. Select [@GoToLevelMode=1(1=Use Storyboard Logic,2=Go to Specific Level)] controls whether the next level in the Storyboard, or another level is loaded after the switch is turned on.

g_switch = {}

function winswitch_properties(e, prompt_text)
	g_switch[e]['prompt_text'] = prompt_text
	g_switch[e]['initialstate'] = 0
end 

function winswitch_init(e)	
	g_switch[e] = {}
	g_switch[e]['initialstate'] = 0
end

function winswitch_main(e)	
	local PlayerDist = GetPlayerDistance(e)
	if g_switch[e]['initialstate'] == 0 then SetActivatedWithMP(e,101) end
	
	if PlayerDist < 90 and g_PlayerHealth > 0 then
		local LookingAt = GetPlrLookingAtEx(e,1)
		if LookingAt == 1 then
			if g_Entity[e]['activated'] == 101 then
				if GetGamePlayerStateXBOX() == 1 then
					PromptLocalForVR(e,"Y Button " .. g_switch[e]['prompt_text'],3)
				else
					if GetHeadTracker() == 1 then
						PromptLocalForVR(e,"Trigger " ..g_switch[e]['prompt_text'],3)
					else
						PromptLocalForVR(e,"E " .. g_switch[e]['prompt_text'],3)
					end
				end
				if g_KeyPressE == 1 then
					SetActivatedWithMP(e,201)					
					SetAnimationName(e,"on")
					PlayAnimation(e) 
					PerformLogicConnections(e)
					PlaySound(e,0)
					JumpToLevelIfUsed(e)
				end			
			end
		end
	end
	if g_Entity[e]['activated'] == 101 then
		SetAnimationName(e,"off")
		PlayAnimation(e)
	end
end
