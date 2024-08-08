-- Gravity Zone v5  Necrym59
-- DESCRIPTION: Will alter the gravity while a player is in the zone, and can trigger selected option upon reaching a trigger height.
-- DESCRIPTION: [#GRAVITY_LEVEL=-3(-20,20)]
-- DESCRIPTION: [#FALL_RATE=300.0(1.0,1000.0)]
-- DESCRIPTION: [ZONEHEIGHT=100(1,1000)] controls how far above the zone the player can be before the zone is not triggered.
-- DESCRIPTION: [SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this zone
-- DESCRIPTION: [@TRIGGER_TYPE=1(1=None, 2=On, 3=Kill Player, 4=Lose Game, 5=Win Game)]
-- DESCRIPTION: [TRIGGER_HEIGHT=500(1,1000)]
-- DESCRIPTION: [#GRAVITY_LIFT=0.0(0.0,100.0)]
-- DESCRIPTION: <Sound0> When entering zone
-- DESCRIPTION: <Sound1> When height reached

local gravity_zone 			= {}
local gravity_level			= {}
local fall_rate				= {}
local zoneheight			= {}
local SpawnAtStart			= {}
local trigger_height		= {}
local trigger_type			= {}
local gravity_lift			= {}

local startposy		= {}
local startzh		= {}
local played		= {}
local doonce		= {}
local status		= {}


function gravity_zone_properties(e, gravity_level, fall_rate, zoneheight, SpawnAtStart, trigger_type, trigger_height, gravity_lift)
	gravity_zone[e].gravity_level = gravity_level
	gravity_zone[e].fall_rate = fall_rate
	gravity_zone[e].zoneheight = zoneheight or 100
	gravity_zone[e].SpawnAtStart = SpawnAtStart	
	gravity_zone[e].trigger_type = trigger_type
	gravity_zone[e].trigger_height = trigger_height
	gravity_zone[e].gravity_lift = gravity_lift
end

function gravity_zone_init(e)
	gravity_zone[e] = {}
	gravity_zone[e].gravity_level = -3
	gravity_zone[e].fall_rate = 300
	gravity_zone[e].zoneheight = 100
	gravity_zone[e].SpawnAtStart = 1	
	gravity_zone[e].trigger_type = 1
	gravity_zone[e].trigger_height = 500	
	gravity_zone[e].gravity_lift = 0
	
	played[e] = 0
	doonce[e] = 0
	status[e] = "init"
end

function gravity_zone_main(e)

	if status[e] == "init" then
		startposy[e] = g_Entity[e]['y']
		startzh[e] = gravity_zone[e].zoneheight		
		if gravity_zone[e].SpawnAtStart == 1 then SetActivated(e,1) end
		if gravity_zone[e].SpawnAtStart == 0 then SetActivated(e,0) end	
		if gravity_zone[e].gravity_lift > 0 then gravity_zone[e].zoneheight = gravity_zone[e].trigger_height end
		status[e] = "endinit"
	end
	if g_Entity[e]['activated'] == 1 then
		if g_Entity[e]['plrinzone'] == 1 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y'] + gravity_zone[e].zoneheight then
			SetWorldGravity(0,gravity_zone[e].gravity_level,0,gravity_zone[e].fall_rate) -- Set to required gravity
			if gravity_zone[e].gravity_lift ~= 0 and g_Entity[e]['y'] < gravity_zone[e].trigger_height then
				GravityOff(e)
				CollisionOn(e)
				ResetPosition(e,g_Entity[e]['x'],g_Entity[e]['y']+gravity_zone[e].gravity_lift,g_Entity[e]['z'])
				SetFreezePosition(g_PlayerPosX,(g_Entity[e]['y']+38),g_PlayerPosZ)
				TransportToFreezePositionOnly()
			end	
			if played[e] == 0 then
				PlaySound(e,0)
				played[e] = 1
			end			
		end
		
		if g_Entity[e]['plrinzone'] == 1 and g_PlayerPosY >= gravity_zone[e].trigger_height then
			if gravity_zone[e].trigger_type == 2 then
				if doonce[e] == 0 then 
					PlaySound(e,1)
					PerformLogicConnections(e)
					ActivateIfUsed(e)
					doonce[e] = 1
				end
			end
			if gravity_zone[e].trigger_type == 3 then
				HurtPlayer(-1,1000)
			end
			if gravity_zone[e].trigger_type == 4 then
				LoseGame()
			end	
			if gravity_zone[e].trigger_type == 5 then
				WinGame()
			end			
		end
		
		if g_Entity[e]['plrinzone'] == 0 then
			SetWorldGravity(0,-20,0,0) -- Reset to normal gravity
			StopSound(e,0)
			played[e] = 0
			doonce[e] = 0
			ResetPosition(e,g_Entity[e]['x'],startposy[e],g_Entity[e]['z'])			
		end		
	end
end