-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Drop Zone v3 by Necrym59
-- DESCRIPTION: The player receive the specified fall damage when dropping from within zone area and within its given height.
-- DESCRIPTION: [FALL_DAMAGE=1(0,100)] how much damage to receive
-- DESCRIPTION: [ZONEHEIGHT=200(1,1000)] controls how far above the zone the player can be before the zone is not triggered.
-- DESCRIPTION: [SPAWN_AT_START!=1] if unchecked use a switch or other trigger to spawn this zone
-- DESCRIPTION: <Sound0> plays a sound when entering the zone.

drop_zone 		= {}
fall_damage 	= {}
zoneheight 		= {}
spawn_at_start	= {}
played 			= {}
setonce 		= {}
setdist 		= {}
doonce 			= {}
dropdist 		= {}
status 			= {}

function drop_zone_properties(e, fall_damage, zoneheight, spawn_at_start)
	drop_zone[e].fall_damage = fall_damage
	drop_zone[e].zoneheight = zoneheight or 100
	drop_zone[e].spawn_at_start = 1
end

function drop_zone_init(e)
	drop_zone[e] = {}
	drop_zone[e].fall_damage = 1
	drop_zone[e].zoneheight = 100
	drop_zone[e].spawn_at_start = 1
	status[e] = "init"
	played[e] = 0
	doonce[e] = 0
	setonce[e] = 0
	setdist[e] = 0
	dropdist[e] = 0
end

function drop_zone_main(e)

	if status[e] == "init" then
		if drop_zone[e].spawn_at_start == 1 then SetActivated(e,1) end
		if drop_zone[e].spawn_at_start == 0 then SetActivated(e,0) end
		if drop_zone[e].zoneheight == nil then drop_zone[e].zoneheight = 100 end
		status[e] = "endinit"
	end
	if g_Entity[e]['activated'] == 1 then
		if g_Entity[e]['plrinzone']==1 and g_PlayerPosY > g_Entity[e]['y']+70 and setonce[e] == 0 then
			setdist[e] = g_PlayerPosY-35
			setonce[e] = 1
		end
		if g_Entity[e]['plrinzone']==1 and g_PlayerPosY+70 > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y'] + drop_zone[e].zoneheight then
			SetGamePlayerControlHurtFall(0)
			if played[e] == 0 then
				PlaySound(e,0)
				played[e] = 1
			end
			if setdist[e] > 0 then dropdist[e] = setdist[e] - (g_PlayerPosY-35) end
			if dropdist[e] > setdist[e] then
				if g_PlayerPosY <= g_Entity[e]['y']+35 then
					if doonce[e] == 0 then
						HurtPlayer(-1,drop_zone[e].fall_damage)
						setdist[e] = 0
						dropdist[e] = 0
						setonce[e] = 0
						doonce[e] = 1
					end
				end
			end
		else
			SetGamePlayerControlHurtFall(1)
		end

		if g_Entity[e]['plrinzone']==0 then
			setdist[e] = 0
			dropdist[e] = 0
			setonce[e] = 0
			doonce[e] = 0
		end
	end
end

function drop_zone_exit(e)
end