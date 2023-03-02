-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Stealthsheild v6
-- DESCRIPTION: The Stealth shield object will give the player Stealth capability while activated.
-- DESCRIPTION: Shield [@STYLE=1(1=Pickup, 2=Collected)]
-- DESCRIPTION: Useage [@KEY_SET=1(1=INS-On DEL-off, 2=PAGEUP-On PAGEDN-Off)]
-- DESCRIPTION: Change the Stealth [ON_TEXT$="Stealth Shield ON"]
-- DESCRIPTION: Change the Stealth [OFF_TEXT$="Stealth Shield OFF"]
-- DESCRIPTION: Stealth [@MODE=1(1=Timed, 2=Random)]
-- DESCRIPTION: Stealth [DURATION=1(1,30)] seconds
-- DESCRIPTION: Stealth [DISCOVERY_RANGE=50(50,2000)]


	local stealthshield = {}
	local key_set = {}
	local on_text= {}
	local off_text = {}
	local style = {}
	local mode = {}
	local duration = {}
	local discovery_range = {}
	local shieldmode = {}
	local scode1 = {}
	local scode2 = {}
	local status = {}
	local shieldtime = {}
	local useage_text = {}
	local stealthicon = {}


function stealthshield_properties(e, style, key_set, on_text, off_text, mode, duration, discovery_range)
	stealthshield[e] = g_Entity[e]
	stealthshield[e].style = style
	stealthshield[e].key_set = key_set
	stealthshield[e].on_text = on_text
	stealthshield[e].off_text = off_text
	stealthshield[e].mode = mode
	stealthshield[e].duration = duration
	stealthshield[e].discovery_range = discovery_range
end

function stealthshield_init(e)
	stealthshield[e] = g_Entity[e]
	stealthshield[e].style = 1
	stealthshield[e].key_set = 1
	stealthshield[e].on_text = "Stealth Shield ON"
	stealthshield[e].off_text = "Stealth Shield OFF"
	stealthshield[e].mode = 1
	stealthshield[e].duration = 1
	stealthshield[e].discovery_range = 50
	shieldmode = 0
	status = "init"
	shieldtime[e] = 0
	useage_text[e] = ""
	stealthicon = CreateSprite(LoadImage("imagebank\\misc\\testimages\\stealthshield_icon.png"))
	SetSpriteSize(stealthicon,-1,-1)
	SetSpriteDepth(stealthicon,1)
	SetSpriteColor(stealthicon,0,0,0,3)
	SetSpritePosition(stealthicon,200,200)
end

function stealthshield_main(e)
	stealthshield[e] = g_Entity[e]
	if status == "init" then
		if stealthshield[e].style == 1 then Show(e) end
		if stealthshield[e].style == 2 then
			CollisionOff(e)
			Hide(e)
		end
		if stealthshield[e].key_set == 1 then
			scode1 = 210
			scode2 = 211
			useage_text[e] = "INS-On DEL-off"
		end
		if stealthshield[e].key_set == 2 then
			scode1 = 201
			scode2 = 209
			useage_text[e] = "PAGEUP-On PAGEDN-Off"
		end
	end
	if stealthshield[e].style == 1 then
		PlayerDist = GetPlayerDistance(e)
		if PlayerDist < 80 and stealthshield[e].style == 1 then
			Prompt("E to pickup Stealth Shield")
			if g_KeyPressE == 1 then
				stealthshield[e].style = 2
				PromptDuration(useage_text[e],3000)
				PlaySound(e,0)
				Hide(e)
				ActivateIfUsed(e)
			end
		end
	end

	if stealthshield[e].style == 2 then
		if GetScancode() == scode1 and shieldmode == 0 then -- Key turn On
			PromptDuration(stealthshield[e].on_text,3000)
			shieldmode = 1
			if stealthshield[e].mode == 1 then shieldtime[e] = g_Time + (stealthshield[e].duration * 1000) end
			if stealthshield[e].mode == 2 then shieldtime[e] = (g_Time + (stealthshield[e].duration * math.random(1000,3000))) end
			StartTimer(e)
		end
		if GetScancode() == scode2 and shieldmode == 1 then -- Key to turn Off
			PromptDuration(stealthshield[e].off_text,3000)
			shieldmode = 0
		end
	end

	for a = 1, g_EntityElementMax do
		if a ~= nil and g_Entity[a] ~= nil then
			local allegiance = GetEntityAllegiance(a) -- get the allegiance (0-enemy, 1-ally, 2-neutral)
			if allegiance == 0 then
				Ent = g_Entity[a]
				if shieldmode == 1 then
					SetEntityAllegiance(a,2) -- Become Neutral
				end
			end
			if allegiance == 2 then
				Ent = g_Entity[a]
				if shieldmode == 0 then
					SetEntityAllegiance(a,0) -- Become Enemy
				end
			end
			if g_Time >= shieldtime[e] and shieldmode == 1 then
				SetEntityAllegiance(a,0)
				shieldmode = 0
				g_Time = 0
				PromptDuration(stealthshield[e].off_text,3000)
			end
			PlayerDist = GetPlayerDistance(a)
			if PlayerDist <= stealthshield[e].discovery_range and shieldmode == 1 then
				SetEntityAllegiance(a,0)
				shieldmode = 0
				g_Time = 0
			end
			if shieldmode == 1 then
				if g_Time >= shieldtime[e]-3000 then SetSpriteColor(stealthicon,0,0,0,3) end
				if g_Time >= shieldtime[e]-2000 then SetSpriteColor(stealthicon,0,0,0,2) end
				if g_Time >= shieldtime[e]-1000 then SetSpriteColor(stealthicon,0,0,0,1) end
				PasteSpritePosition(stealthicon,92,92)
			else
				SetSpriteColor(stealthicon,0,0,0,3)
				SetSpritePosition(stealthicon,200,200)
			end
		end
	end
end