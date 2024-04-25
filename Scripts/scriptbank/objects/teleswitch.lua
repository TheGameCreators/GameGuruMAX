-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- TeleSwitch v8: by Necrym59
-- DESCRIPTION: This object will be treated as a switch object to teleport to a linked object.
-- DESCRIPTION: It is better to use a small or a flat object to avoid getting stuck when you reappear.
-- DESCRIPTION: [PROMPT_TEXT$="to Teleport"]
-- DESCRIPTION: [USE_RANGE=90(1,100)]
-- DESCRIPTION: [PlayerLevel=0(0,100))] player level to be able use this switch
-- DESCRIPTION: [TELEPORT_EXIT_ANGLE=1(1,360))] Player exit angle upon teleport
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline)] Use emmisive color for shape option
-- DESCRIPTION: Play <Sound0> when the object is switched ON.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local teleswitch 			= {}
local prompt_text 			= {}
local use_range 			= {}
local playerlevel 			= {}
local teleport_exit_angle 	= {}
local item_highlight		= {}

local tlevelrequired 	= {}
local tplayerlevel 		= {}
local dest_angle 		= {}
local status 			= {}
local doonce			= {}
local tEnt 				= {}
local selectobj 		= {}

function teleswitch_properties(e, prompt_text, use_range, playerlevel, teleport_exit_angle, item_highlight)
	teleswitch[e].prompt_text = prompt_text
	teleswitch[e].use_range = use_range
	teleswitch[e].playerlevel = playerlevel
	teleswitch[e].teleport_exit_angle = teleport_exit_angle	or 1
	teleswitch[e].item_highlight = item_highlight or 0	
end

function teleswitch_init(e)
	teleswitch[e] = {}
	teleswitch[e].prompt_text = ""
	teleswitch[e].use_range = 90
	teleswitch[e].playerlevel = 0
	teleswitch[e].teleport_exit_angle = 1	
	teleswitch[e].item_highlight = 0	
	teleswitch[e].teleport_target = GetEntityString(e,0)
	
	tplayerlevel[e] = 0
	tlevelrequired[e] = 0
	dest_angle[e] = 0
	doonce[e] = 0
	g_tEnt = 0
	status = "init"
end

function teleswitch_main(e)

	if status == "init" then
		tplayerlevel[e] = 0
		tlevelrequired[e] = teleswitch[e].playerlevel
		dest_angle[e] = teleswitch[e].teleport_exit_angle
		status = "endinit"
	end
	local PlayerDist = GetPlayerDistance(e)
	
	if PlayerDist < teleswitch[e].use_range then
		--pinpoint select object--
		module_misclib.pinpoint(e,teleswitch[e].use_range,teleswitch[e].item_highlight)
		tEnt[e] = g_tEnt
		--end pinpoint select object--	
	end	
	
	if PlayerDist < teleswitch[e].use_range and tEnt[e] ~= 0 then
		if _G["g_UserGlobal['".."MyPlayerLevel".."']"] ~= nil then tplayerlevel[e] = _G["g_UserGlobal['".."MyPlayerLevel".."']"] end
		if tplayerlevel[e] < tlevelrequired[e] then PromptLocal(e,"You need to be level "..tlevelrequired[e].." to use this switch") end
		if tplayerlevel[e] >= tlevelrequired[e] then

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
				if doonce[e] == 0 then
					SetActivatedWithMP(e,101)
					SetAnimationName(e,"on")
					PlayAnimation(e)
					PlaySound(e,0)
					TransportToIfUsed(e)
					SetGamePlayerControlFinalCameraAngley(dest_angle[e])
					PerformLogicConnections(e)
					status = "init"
					doonce[e] = 1
				end	
			end
		end
		doonce[e] = 0
	end
	if g_Entity[e]['activated'] == 201 then
		SetAnimationName(e,"off")
		PlayAnimation(e)
	end
end
