-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- TeleSwitch v3: by Necrym59
-- DESCRIPTION: This object will be treated as a switch object to teleport to a linked object.
-- DESCRIPTION: It is better to use a small or a flat object to avoid getting stuck when you reappear.
-- DESCRIPTION: [PROMPT_TEXT$="to Teleport"]
-- DESCRIPTION: [USE_RANGE=90(1,100)]
-- DESCRIPTION: Play <Sound0> when the object is switched ON.

local teleswitch = {}
local prompt_text = {}
local use_range = {}

function teleswitch_properties(e, prompt_text, use_range)
	teleswitch[e] = g_Entity[e]
	teleswitch[e].prompt_text = prompt_text
	teleswitch[e].use_range = use_range
	teleswitch[e].initialstate = 0
end

function teleswitch_init(e)
	teleswitch[e] = g_Entity[e]
	teleswitch[e].prompt_text = ""
	teleswitch[e].use_range = 90
	teleswitch[e].initialstate = 0
	teleswitch[e].teleport_target = GetEntityString(e,0)
end

function teleswitch_main(e)
	teleswitch[e] = g_Entity[e]
	local PlayerDist = GetPlayerDistance(e)

	if PlayerDist < teleswitch[e].use_range and g_PlayerHealth > 0 then
		local LookingAt = GetPlrLookingAtEx(e,1)
		if LookingAt == 1 then			
			if GetGamePlayerStateXBOX() == 1 then
				PromptLocalForVR(e,"Y Button " .. teleswitch[e]['prompt_text'],3)
			else
				if GetHeadTracker() == 1 then
					PromptLocalForVR(e,"Trigger " ..teleswitch[e]['prompt_text'],3)
				else
					PromptLocalForVR(e,"E " .. teleswitch[e]['prompt_text'],3)
				end
			end
			if g_KeyPressE == 1 then
				SetActivatedWithMP(e,101)
				SetAnimationName(e,"on")
				PlayAnimation(e)
				PlaySound(e,0)
				TransportToIfUsed(e)
				PerformLogicConnections(e)				
			end
		end
	end
	if g_Entity[e]['activated'] == 201 then
		SetAnimationName(e,"off")
		PlayAnimation(e)
	end
end
