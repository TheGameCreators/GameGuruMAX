-- Switch Combo v12
-- DESCRIPTION: A combo-value switch to add to 100 to Activate IfUsed and/or logic linked object.
-- DESCRIPTION: [UseRange=90(1,200)]
-- DESCRIPTION: [SwitchedOn!=0] state to decide if the switch is initially off or on, and customize the
-- DESCRIPTION: [OnText$="To Turn Switch ON"] and [OffText$="To Turn Switch OFF"].
-- DESCRIPTION: [PlayerLevel=0(0,100))] player level to be able use this switch
-- DESCRIPTION: [@SwitchType=1(1=Multi-Use, 2=Single-Use)]
-- DESCRIPTION: [SwitchValue=0(0,100)] for this switch value
-- DESCRIPTION: [DeferLinks!=0] to defer linked connection trigger
-- DESCRIPTION: [DeferLinksValue=50(1,99)] defer linked connection trigger value
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline)] Use emmisive color for shape option
-- DESCRIPTION: <Sound0> when the object is switched ON.
-- DESCRIPTION: <Sound1> when the object is switched OFF.
 
local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}
g_swcvalue = {}

local switch_combo		= {}
local userange 			= {}
local initialstate 		= {}
local ontext 			= {}
local offtext 			= {}
local playerlevel 		= {}
local switchtype 		= {}
local switchvalue 		= {}
local deferlinks 		= {}
local deferlinksvalue 	= {}
local item_highlight	= {}

local doonce 			= {}
local dooncePC			= {}
local status 			= {}
local tEnt 				= {}
local selectobj 		= {}
local switched 			= {}
local tlevelrequired 	= {}
local tplayerlevel 		= {}
local reachedvalue		= {}
local reachedcheck		= {}

function switch_combo_properties(e, userange, switchedon, ontext, offtext, playerlevel, switchtype, switchvalue, deferlinks, deferlinksvalue, item_highlight)
	switch_combo[e] = g_Entity[e]
	switch_combo[e].userange = userange or 90
	switch_combo[e].initialstate = switchedon
	switch_combo[e].ontext = ontext
	switch_combo[e].offtext = offtext
	switch_combo[e].playerlevel = playerlevel
	switch_combo[e].switchtype = switchtype
	switch_combo[e].switchvalue = switchvalue
	switch_combo[e].deferlinks = deferlinks
	switch_combo[e].deferlinksvalue = deferlinksvalue or 99
	switch_combo[e].item_highlight = item_highlight	or 0	
end 

function switch_combo_init(e)
	switch_combo[e] = {}
	switch_combo[e].initialstate = 1
	switch_combo[e].ontext = "To Turn Switch ON"
	switch_combo[e].offtext = "To Turn Switch OFF"
	switch_combo[e].userange = 90	
	switch_combo[e].playerlevel = 0
	switch_combo[e].switchtype = 1
	switch_combo[e].switchvalue = 0	
	switch_combo[e].deferlinks = 0
	switch_combo[e].deferlinksvalue = 99
	switch_combo[e].item_highlight = 0
	
	tEnt[e] = 0
	g_tEnt = 0
	selectobj[e] = 0
	switched[e] = 0
	tlevelrequired[e] = 0
	tplayerlevel[e] = 0
	status[e] = "init"
	doonce[e] = 0
	dooncePC[e] = 0
	reachedvalue[e] = 0	
	reachedcheck[e] = 0	
	g_swcvalue = 0
end

function switch_combo_main(e)
	if status[e] == "init" then
		tplayerlevel[e] = 0
		tlevelrequired[e] = switch_combo[e].playerlevel		
		if switch_combo[e].switchvalue > 100 then switch_combo[e].switchvalue = 100 end
		if switch_combo[e].deferlinksvalue > 99 then switch_combo[e].deferlinksvalue = 99 end
		if switch_combo[e].ontext == nil then switch_combo[e].ontext = "To Turn Switch ON" end
		if switch_combo[e].offtext == nil then switch_combo[e].offtext = "To Turn Switch OFF" end
		if switch_combo[e].initialstate ~= nil then
			if switch_combo[e].initialstate >=0 then
				if switch_combo[e].initialstate == 0 then SetActivatedWithMP(e,101) end
				if switch_combo[e].initialstate == 1 then SetActivatedWithMP(e,201) end
			end
		end	
		status[e] = "endinit"
	end
	
	
	local PlayerDist = GetPlayerDistance(e)
	if PlayerDist < switch_combo[e].userange then
		--pinpoint select object--
		module_misclib.pinpoint(e,switch_combo[e].userange,switch_combo[e].item_highlight)
		tEnt[e] = g_tEnt
		--end pinpoint select object--	
	end	
	if PlayerDist < switch_combo[e].userange and tEnt[e] ~= 0 then
		if _G["g_UserGlobal['".."MyPlayerLevel".."']"] ~= nil then tplayerlevel[e] = _G["g_UserGlobal['".."MyPlayerLevel".."']"] end
		if tplayerlevel[e] < tlevelrequired[e] then PromptLocal(e,"You need to be level "..tlevelrequired[e].." to use this switch") end
		if tplayerlevel[e] >= tlevelrequired[e] then
		
			if switch_combo[e].switchtype == 1 then switched[e] = 0 end
			if g_Entity[e].activated == 0 and switched[e] == 0 then
				if switch_combo[e].ontext ~= "" then
					if GetGamePlayerStateXBOX() == 1 then
						PromptLocalForVR(e,"Y Button " .. switch_combo[e].ontext,3)
					else
						if GetHeadTracker() == 1 then
							PromptLocalForVR(e,"Trigger " .. switch_combo[e].ontext,3)
						else
							PromptLocalForVR(e,"E " .. switch_combo[e].ontext,3)
						end
					end
				end
				if g_KeyPressE == 1 then
					SetActivatedWithMP(e,201)
					if reachedcheck[e] == 0 then
						g_swcvalue = g_swcvalue + switch_combo[e].switchvalue
						if g_swcvalue == 100 + switch_combo[e].switchvalue then g_swcvalue = 100 end
					end	
					switched[e] = 1
					doonce[e] = 0					
				end
			else
				if switch_combo[e].switchtype == 1 then switched[e] = 0 end
				if g_Entity[e].activated == 1 and switched[e] == 0 then
					if switch_combo[e].offtext ~= "" then
						if GetGamePlayerStateXBOX() == 1 then
							PromptLocalForVR(e,"Y Button " .. switch_combo[e].offtext,3)
						else
							if GetHeadTracker() == 1 then
								PromptLocalForVR(e,"Trigger " .. switch_combo[e].offtext,3)
							else
								PromptLocalForVR(e,"E " .. switch_combo[e].offtext,3)
							end
						end
					end
					if g_KeyPressE == 1 then						
						SetActivatedWithMP(e,101)
						if reachedcheck[e] == 0 then
							g_swcvalue = g_swcvalue - switch_combo[e].switchvalue
							if g_swcvalue == 100 - switch_combo[e].switchvalue then g_swcvalue = 100 end
						end	
						switched[e] = 1
						doonce[e] = 0
					end
				end
			end
		end		
	end

	-- proximity independence
	if g_Entity[e].activated == 100 then
		SetActivated(e,0)
	end
	if g_Entity[e].activated == 101 then
		SetAnimationName(e,"off")
		PlayAnimation(e)
		if switch_combo[e].initialstate == -1 then
			if switch_combo[e].deferlinks == 0 then PerformLogicConnections(e) end
			PlaySound(e,1) 
		end
		switch_combo[e].initialstate = -1
		SetActivated(e,102)
	end
	if g_Entity[e].activated == 102 then
		if g_KeyPressE == 0 then SetActivated(e,0) end
	end
	if g_Entity[e].activated == 201 then
		SetAnimationName(e,"on")
		PlayAnimation(e)
		if switch_combo[e].initialstate == -1 then 
			if switch_combo[e].deferlinks == 0 then PerformLogicConnections(e) end			
			PlaySound(e,0) 
		end		
		switch_combo[e].initialstate = -1
		SetActivated(e,202)
	end
	if g_Entity[e].activated == 202 then		
		if g_KeyPressE == 0 then SetActivated(e,1) end
	end

	if g_swcvalue >= switch_combo[e].deferlinksvalue and dooncePC[e] == 0 then
		if switch_combo[e].deferlinks == 1 then
			PerformLogicConnections(e)
			dooncePC[e] = 1
		end
	end

	if g_swcvalue >= 100 then
		if reachedcheck[e] == 0 then
			reachedvalue[e] = 100
			reachedcheck[e] = 1
			g_swcvalue = 0
		end	
	end
	if g_swcvalue < 0 then g_swcvalue = 0 end
	
	if reachedvalue[e] == 100 and doonce[e] == 0 then
		if doonce[e] == 0 then			
			if switch_combo[e].deferlinks == 1 and dooncePC[e] ~= 1 then
				PerformLogicConnections(e)				
			end
			ActivateIfUsed(e)
			doonce[e] = 1
		end
	end
end
