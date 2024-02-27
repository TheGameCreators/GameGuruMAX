-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- TeleSwitch v6: by Necrym59
-- DESCRIPTION: This object will be treated as a switch object to teleport to a linked object.
-- DESCRIPTION: It is better to use a small or a flat object to avoid getting stuck when you reappear.
-- DESCRIPTION: [PROMPT_TEXT$="to Teleport"]
-- DESCRIPTION: [USE_RANGE=90(1,100)]
-- DESCRIPTION: [PlayerLevel=0(0,100))] player level to be able use this switch
-- DESCRIPTION: Play <Sound0> when the object is switched ON.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local teleswitch 		= {}
local prompt_text 		= {}
local use_range 		= {}
local playerlevel 		= {}

local tlevelrequired 	= {}
local tplayerlevel 		= {}
local status 			= {}
local tEnt 				= {}
local selectobj 		= {}

function teleswitch_properties(e, prompt_text, use_range, playerlevel)
	teleswitch[e] = g_Entity[e]
	teleswitch[e].prompt_text = prompt_text
	teleswitch[e].use_range = use_range
	teleswitch[e].playerlevel = playerlevel	
	teleswitch[e].initialstate = 0
end

function teleswitch_init(e)
	teleswitch[e] = g_Entity[e]
	teleswitch[e].prompt_text = ""
	teleswitch[e].use_range = 90
	teleswitch[e].playerlevel = 0
	teleswitch[e].initialstate = 0
	teleswitch[e].teleport_target = GetEntityString(e,0)
	
	tplayerlevel[e] = 0
	tlevelrequired[e] = 0
	g_tEnt = 0
	status = "init"
end

function teleswitch_main(e)
	teleswitch[e] = g_Entity[e]
	if status == "init" then
		tplayerlevel[e] = 0
		tlevelrequired[e] = teleswitch[e].playerlevel
		status = "endinit"
	end
	local PlayerDist = GetPlayerDistance(e)
	
	if PlayerDist < teleswitch[e].use_range then
		--pinpoint select object--
		module_misclib.pinpoint(e,teleswitch[e].use_range,200)
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
