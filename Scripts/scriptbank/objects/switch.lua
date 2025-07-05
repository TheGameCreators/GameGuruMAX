-- Switch v19 by Necrym59 and Lee
-- DESCRIPTION: This object will be treated as a switch object for activating other objects or game elements.
-- DESCRIPTION: Play the audio <Sound0> when the object is switched ON by the player, and <Sound1> when the object is switched OFF.
-- DESCRIPTION: Use the [SwitchedOn!=1] state to decide if the switch is initially off or on, and customize the [OnText$="E To Turn Switch ON"] and [OffText$="E To Turn Switch OFF"].
-- DESCRIPTION: [UseRange=90(1,200)]
-- DESCRIPTION: [PlayerLevel=0(0,100))] player level to be able use this switch
-- DESCRIPTION: [@SwitchType=1(1=Multi-Use, 2=Single-Use)]
-- DESCRIPTION: [@NPC_TRIGGER=2(1=On, 2=Off)]
-- DESCRIPTION: [@ON_ANIMATION$=1(0=AnimSetList)] Select ON animation (Default=ON)
-- DESCRIPTION: [@OFF_ANIMATION$=2(0=AnimSetList)] Select OFF animation (Default=OFF)
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)] Use emmisive color for shape option
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\switch.png"]
-- DESCRIPTION: <Sound0> when switch activates
-- DESCRIPTION: <Sound1> when switch deactivates

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local switch 			= {}

local ontext 			= {}
local offtext 			= {}
local userange 			= {}
local playerlevel 		= {}
local switchtype 		= {}
local npc_trigger 		= {}
local on_animation 		= {}
local off_animation 	= {}
local prompt_display	= {}
local item_highlight	= {}
local highlight_icon	= {}

local status 			= {}
local tEnt 				= {}
local selectobj 		= {}
local switched 			= {}
local tlevelrequired 	= {}
local tplayerlevel 		= {}
local sensecheck 		= {}
local doonce			= {}
local hl_icon 			= {}
local hl_imgwidth		= {}
local hl_imgheight		= {}

-- when you wish the state to be preserved after a game save and reload, always use g_nameofscript, ie
g_switch = {}

function switch_properties(e, switchedon, ontext, offtext, userange, playerlevel, switchtype, npc_trigger, on_animation, off_animation, prompt_display, item_highlight, highlight_icon_imagefile )
	g_switch[e].initialstate = switchedon
	switch[e].ontext = ontext
	switch[e].offtext = offtext
	switch[e].userange = userange or 90
	switch[e].playerlevel = playerlevel or 0
	switch[e].switchtype = switchtype or 1
	switch[e].npc_trigger = npc_trigger or 2
	switch[e].on_animation = "=" .. tostring(on_animation)
	switch[e].off_animation	= "=" .. tostring(off_animation)
	switch[e].prompt_display = prompt_display or 1
	switch[e].item_highlight = item_highlight or 0
	switch[e].highlight_icon = highlight_icon_imagefile
end

function switch_init(e)
	g_switch[e] = {}
	g_switch[e].initialstate = 1
	switch[e] = {}
	switch[e].ontext = "To Turn Switch ON"
	switch[e].offtext = "To Turn Switch OFF"
	switch[e].userange = 90
	switch[e].playerlevel = 0
	switch[e].switchtype = 1
	switch[e].npc_trigger = 1
	switch[e].on_animation = ""
	switch[e].off_animation	= ""
	switch[e].prompt_display = 1
	switch[e].item_highlight = 0
	switch[e].highlight_icon = "imagebank\\icons\\switch.png"

	tEnt[e] = 0
	selectobj[e] = 0
	switched[e] = 0
	tlevelrequired[e] = 0
	tplayerlevel[e] = 0
	sensecheck[e] = math.huge
	doonce[e] = 0
	g_tEnt = 0
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0
	status[e] = "init"
end

function switch_main(e)
	if status[e] == "init" then
		if switch[e].item_highlight == 3 and switch[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(switch[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(switch[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(switch[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end
		tplayerlevel[e] = 0
		tlevelrequired[e] = switch[e].playerlevel
		sensecheck[e] = g_Time + 1000
		status[e] = "endinit"
	end

	if switch[e].ontext == nil then switch[e].ontext = "To Turn Switch ON" end
	if switch[e].offtext == nil then switch[e].offtext = "To Turn Switch OFF" end
	if g_switch[e].initialstate ~= nil then
		if g_switch[e].initialstate >=0 then
			if g_switch[e].initialstate == 0 then SetActivatedWithMP(e,101) end
			if g_switch[e].initialstate == 1 then SetActivatedWithMP(e,201) end
		end
	end
	local PlayerDist = GetPlayerDistance(e)
	if PlayerDist < switch[e].userange then
		--pinpoint select object--
		module_misclib.pinpoint(e,switch[e].userange,switch[e].item_highlight,hl_icon[e])
		tEnt[e] = g_tEnt
		--end pinpoint select object--
	end
	if PlayerDist < switch[e].userange and tEnt[e] == e then
		if _G["g_UserGlobal['".."MyPlayerLevel".."']"] ~= nil then tplayerlevel[e] = _G["g_UserGlobal['".."MyPlayerLevel".."']"] end
		if tplayerlevel[e] < tlevelrequired[e] then PromptLocal(e,"You need to be level "..tlevelrequired[e].." to use this switch") end
		if tplayerlevel[e] >= tlevelrequired[e] then

			if switch[e].switchtype == 1 and g_KeyPressE == 0 then switched[e] = 0 end
			if g_Entity[e].activated == 0 and switched[e] == 0 then
				if switch[e].ontext ~= "" then
					if GetGamePlayerStateXBOX() == 1 then
						PromptLocalForVR(e,"Y Button " .. switch[e].ontext,3)
					else
						if GetHeadTracker() == 1 then
							PromptLocalForVR(e,"Trigger " .. switch[e].ontext,3)
						else
							if switch[e].prompt_display == 1 then PromptLocal(e,switch[e].ontext) end
							if switch[e].prompt_display == 2 then Prompt(switch[e].ontext) end
						end
					end
				end
				if g_KeyPressE == 1 then
					SetActivatedWithMP(e,201)
					switched[e] = 1
					doonce[e] = 0
				end
			else
				if switch[e].switchtype == 1 and g_KeyPressE == 0 then switched[e] = 0 end
				if g_Entity[e].activated == 1 and switched[e] == 0 then
					if switch[e].offtext ~= "" then
						if GetGamePlayerStateXBOX() == 1 then
							PromptLocalForVR(e,"Y Button " .. switch[e].offtext,3)
						else
							if GetHeadTracker() == 1 then
								PromptLocalForVR(e,"Trigger " .. switch[e].offtext,3)
							else
								if switch[e].prompt_display == 1 then PromptLocal(e,switch[e].offtext) end
								if switch[e].prompt_display == 2 then Prompt(switch[e].offtext) end
							end
						end
					end
					if g_KeyPressE == 1 then
						SetActivatedWithMP(e,101)
						switched[e] = 1
						doonce[e] = 0
					end
				end
			end
		end
	end
	if switch[e].npc_trigger == 1 and doonce[e] == 0 then
		if g_Time > sensecheck[e] then
			for a = 1, g_EntityElementMax do
				if a ~= nil and g_Entity[a] ~= nil then
					local allegiance = GetEntityAllegiance(a)
					if allegiance ~= -1 then
						local entrange = math.ceil(GetFlatDistance(e,a))
						if entrange < switch[e].userange then
							if doonce[e] == 0 then
								if g_Entity[e].activated == 0 then
									SetActivatedWithMP(e,201)
									switched[e] = 1
								end
								if g_Entity[e].activated == 1 then
									SetActivatedWithMP(e,101)
									switched[e] = 1
								end
								doonce[e] = 1
							end
						end
					end
				end
			end
			sensecheck[e] = g_Time + 1000
		end
	end
	-- proximity independence
	if g_Entity[e].activated == 100 then
		SetActivated(e,0)
	end
	if g_Entity[e].activated == 101 then
		SetAnimationName(e,switch[e].off_animation)
		PlayAnimation(e)
		if g_switch[e].initialstate == -1 then
			PerformLogicConnections(e)
			PlaySound(e,1)
		end
		g_switch[e].initialstate = -1
		SetActivated(e,102)
	end
	if g_Entity[e].activated == 102 then
		if g_KeyPressE == 0 then SetActivated(e,0) end
	end
	if g_Entity[e].activated == 201 then
		SetAnimationName(e,switch[e].on_animation)
		PlayAnimation(e)
		if g_switch[e].initialstate == -1 then
			PerformLogicConnections(e)
			ActivateIfUsed(e)
			PlaySound(e,0)
		end
		g_switch[e].initialstate = -1
		SetActivated(e,202)
	end
	if g_Entity[e].activated == 202 then
		if g_KeyPressE == 0 then SetActivated(e,1) end
	end
	-- restore logic
	if g_EntityExtra[e]['restoremenow'] ~= nil then
     if g_EntityExtra[e]['restoremenow'] == 1 then
      g_EntityExtra[e]['restoremenow'] = 0
     end
	end
end

function GetFlatDistance(e,v)
	if g_Entity[e] ~= nil and g_Entity[v] ~= nil then
		local distDX = g_Entity[e]['x'] - g_Entity[v]['x']
		local distDZ = g_Entity[e]['z'] - g_Entity[v]['z']
		return math.sqrt((distDX*distDX)+(distDZ*distDZ));
	end
end

