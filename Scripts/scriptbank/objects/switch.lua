-- Switch v12
-- DESCRIPTION: This object will be treated as a switch object for activating other objects or game elements.
-- DESCRIPTION: Play the audio <Sound0> when the object is switched ON by the player, and <Sound1> when the object is switched OFF. 
-- DESCRIPTION: Use the [SwitchedOn!=1] state to decide if the switch is initially off or on, and customize the [OnText$="To Turn Switch ON"] and [OffText$="To Turn Switch OFF"].
-- DESCRIPTION: [UseRange=90(1,200)]
-- DESCRIPTION: [PlayerLevel=0(0,100))] player level to be able use this switch
-- DESCRIPTION: [@SwitchType=1(1=Multi-Use, 2=Single-Use)]
-- DESCRIPTION: [@NPC_TRIGGER=2(1=On, 2=Off)]

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local switch 			= {}
local initialstate 		= {}
local ontext 			= {}
local offtext 			= {}
local userange 			= {}
local playerlevel 		= {}
local switchtype 		= {}
local npc_trigger 		= {}

local status 			= {}
local tEnt 				= {}
local selectobj 		= {}
local switched 			= {}
local tlevelrequired 	= {}
local tplayerlevel 		= {}
local sensecheck 		= {}
local doonce			= {}

function switch_properties(e, switchedon, ontext, offtext, userange, playerlevel, switchtype, npc_trigger)
	switch[e].initialstate = switchedon
	switch[e].ontext = ontext
	switch[e].offtext = offtext
	switch[e].userange = userange or 90
	switch[e].playerlevel = playerlevel or 0
	switch[e].switchtype = switchtype or 1
	switch[e].npc_trigger = npc_trigger or 2	
end 

function switch_init(e)
	switch[e] = {}
	switch[e].initialstate = 1
	switch[e].ontext = "To Turn Switch ON"
	switch[e].offtext = "To Turn Switch OFF"
	switch[e].userange = 90	
	switch[e].playerlevel = 0
	switch[e].switchtype = 1
	switch[e].npc_trigger = 1	
	tEnt[e] = 0
	selectobj[e] = 0
	switched[e] = 0
	tlevelrequired[e] = 0
	tplayerlevel[e] = 0
	sensecheck[e] = math.huge
	doonce[e] = 0
	g_tEnt = 0
	status[e] = "init"	
end

function switch_main(e)
	if status[e] == "init" then
		tplayerlevel[e] = 0
		tlevelrequired[e] = switch[e].playerlevel
		sensecheck[e] = g_Time + 1000
		status[e] = "endinit"
	end
	if switch[e].ontext == nil then switch[e].ontext = "To Turn Switch ON" end
	if switch[e].offtext == nil then switch[e].offtext = "To Turn Switch OFF" end
	if switch[e].initialstate ~= nil then
		if switch[e].initialstate >=0 then
			if switch[e].initialstate == 0 then SetActivatedWithMP(e,101) end
			if switch[e].initialstate == 1 then SetActivatedWithMP(e,201) end
		end
	end

	local PlayerDist = GetPlayerDistance(e)
	if PlayerDist < switch[e].userange then
		--pinpoint select object--
		module_misclib.pinpoint(e,switch[e].userange,200)
		tEnt[e] = g_tEnt
		--end pinpoint select object--
	end	
	
	if PlayerDist < switch[e].userange and tEnt[e] ~= 0 then
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
							PromptLocalForVR(e,"E " .. switch[e].ontext,3)
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
								PromptLocalForVR(e,"E " .. switch[e].offtext,3)
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
		SetAnimationName(e,"off")
		PlayAnimation(e)
		if switch[e].initialstate == -1 then 
			PerformLogicConnections(e)
			PlaySound(e,1) 
		end
		switch[e].initialstate = -1
		SetActivated(e,102)
	end
	if g_Entity[e].activated == 102 then
		if g_KeyPressE == 0 then SetActivated(e,0) end
	end
	if g_Entity[e].activated == 201 then
		SetAnimationName(e,"on")
		PlayAnimation(e)
		if switch[e].initialstate == -1 then 
			PerformLogicConnections(e)
			PlaySound(e,0) 
		end
		switch[e].initialstate = -1
		SetActivated(e,202)		
	end
	if g_Entity[e].activated == 202 then
		if g_KeyPressE == 0 then SetActivated(e,1) end
	end  
end

function GetFlatDistance(e,v)
	if g_Entity[e] ~= nil and g_Entity[v] ~= nil then
		local distDX = g_Entity[e]['x'] - g_Entity[v]['x']
		local distDZ = g_Entity[e]['z'] - g_Entity[v]['z']
		return math.sqrt((distDX*distDX)+(distDZ*distDZ));
	end
end

	