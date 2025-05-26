-- Freeze Player v16
-- DESCRIPTION: When a player enters zone will freeze the player, and stay in a frozen state for
-- DESCRIPTION: [FREEZETIME=3] in seconds. (0=infinite)
-- DESCRIPTION: [ZONEHEIGHT=100(1,500)] controls how far above the zone the player can be before the zone is not triggered.
-- DESCRIPTION: [SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this zone
-- DESCRIPTION: [MultiTrigger!=1] if unchecked will destroy this zone after use
-- DESCRIPTION: [@FreezeStyle=1(1=Total Freeze, 2=Partial Freeze)]
-- DESCRIPTION: [ViewAngleLimit=90(1,180)] Sets the lateral view range
-- DESCRIPTION: [@ZONE_TRIGGER=2(1=Yes, 2=No)] Sets if zone will trigger other logic linked or IfUsed entities.
-- DESCRIPTION: <Sound0> Sound to play when freezing
-- DESCRIPTION: <Sound1> Sound to play when unfreezing

local freeze 		= {}
local frozenmode	= {}
local frozentime	= {}
local freezex		= {}
local freezey		= {}
local freezez		= {}
local freezeangy	= {}
local doonce 		= {}
local played		= {}
local status		= {}

function FreezePlayer_properties(e, freezetime, zoneheight, SpawnAtStart, MultiTrigger, FreezeStyle, ViewAngleLimit, zone_trigger)
	freeze[e].freezetime = freezetime or 3
	freeze[e].zoneheight = zoneheight or 100
	freeze[e].SpawnAtStart = SpawnAtStart or 1
	freeze[e].MultiTrigger = MultiTrigger or 1
	freeze[e].FreezeStyle = FreezeStyle or 1
	freeze[e].ViewAngleLimit = ViewAngleLimit or 90	
	freeze[e].zone_trigger = zone_trigger or 2
end

function FreezePlayer_init(e)
	freeze[e] = {}
	freeze[e].freezetime = 3
	freeze[e].zoneheight = 100
	freeze[e].SpawnAtStart = 1
	freeze[e].MultiTrigger = 1
	freeze[e].FreezeStyle = 1
	freeze[e].ViewAngleLimit = 90
	freeze[e].zone_trigger = 2	
	frozenmode[e] = 0
	freezex[e] = 0
	freezey[e] = 0
	freezez[e] = 0
	freezeangy[e] = 0
	frozentime[e] = math.huge
	played[e] = 0
	doonce[e] = 0
	status[e] = "init"
end

function FreezePlayer_main(e)
	if status[e] == "init" then
		if freeze[e].SpawnAtStart == 1 then SetActivated(e,1) end
		if freeze[e].SpawnAtStart == 0 then SetActivated(e,0) end
		status[e] = "endinit"
	end
	if g_Entity[e]['activated'] == 1 then
		if g_Entity[e]['plrinzone'] == 1 and g_PlayerHealth > 0 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y'] + freeze[e].zoneheight then
			if freeze[e].FreezeStyle == 1 then
				if freeze[e].freezetime == 0 or freeze[e].freezetime == nil and frozenmode[e] == 0 then
					--DisableBoundHudKeys()
					SetCameraOverride(3)
					frozentime[e] = g_Time * 2
					if played[e] == 0 then 
						PlaySound(e,0)
						played[e] = 1
					end
					frozenmode[e] = 1
				end
				if freeze[e].freezetime ~= nil and frozenmode[e] == 0 then
					SetCameraOverride(3)
					frozentime[e] = g_Time + (freeze[e].freezetime * 1000)			
					PlaySound(e,0)
					frozenmode[e] = 1
				end
				if g_Time > frozentime[e] and frozenmode[e] == 1 then
					if played[e] == 0 then 
						PlaySound(e,1)
						played[e] = 1
					end	
					SetCameraOverride(0)
					--EnableBoundHudKeys()
					if freeze[e].MultiTrigger == 0 then Destroy(e) end
				end
			end
			if freeze[e].FreezeStyle == 2 then
				if freeze[e].freezetime == 0 or freeze[e].freezetime == nil and frozenmode[e] == 0 then
					--DisableBoundHudKeys()
					frozentime[e] = g_Time * 2
					freezex[e] = g_PlayerPosX
					freezey[e] = g_PlayerPosY
					freezez[e] = g_PlayerPosZ
					freezeangy[e] = g_PlayerAngY					
					frozenmode[e] = 1					
					if played[e] == 0 then 
						PlaySound(e,0)
						played[e] = 1
					end
					SetGamePlayerControlFinalCameraAngley(freezeangy[e])
				end
				if freeze[e].freezetime ~= nil and frozenmode[e] == 0 then
					--DisableBoundHudKeys()
					frozentime[e] = g_Time + (freeze[e].freezetime * 1000)
					freezex[e] = g_PlayerPosX
					freezey[e] = g_PlayerPosY
					freezez[e] = g_PlayerPosZ
					freezeangy[e] = g_PlayerAngY
					PlaySound(e,0)
					frozenmode[e] = 1
					SetGamePlayerControlFinalCameraAngley(freezeangy[e])
				end
				if g_Time < frozentime[e] and frozenmode[e] == 1 then
					SetFreezePosition(freezex[e],freezey[e],freezez[e])	
					TransportToFreezePositionOnly()
					-- Cap look left/right angle so cannot wrap around
					if GetGamePlayerControlFinalCameraAngley()>freeze[e].ViewAngleLimit+freezeangy[e] then SetGamePlayerControlFinalCameraAngley(freeze[e].ViewAngleLimit+freezeangy[e]) end
					if GetGamePlayerControlFinalCameraAngley()<-freeze[e].ViewAngleLimit+freezeangy[e] then SetGamePlayerControlFinalCameraAngley(-freeze[e].ViewAngleLimit+freezeangy[e]) end
				end
				if g_Time > frozentime[e] and frozenmode[e] == 1 then					
					if played[e] == 0 then 
						PlaySound(e,1)
						played[e] = 1
					end	
					--EnableBoundHudKeys()
					if freeze[e].MultiTrigger == 0 then Destroy(e) end
				end
			end
			if freeze[e].zone_trigger == 1 then
				if doonce[e] == 0 then										
					PerformLogicConnections(e)
					ActivateIfUsed(e)
					doonce[e] = 1
				end
			end
		end	
		if g_Entity[e]['plrinzone'] == 0 then
			frozenmode[e] = 0
			played[e] = 0
		end
	end
end