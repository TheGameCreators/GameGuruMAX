-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Get Weapon Zone v5 by Amen Moses and Necrym59
-- DESCRIPTION: Gets closest weapon within set range to this zone and gives to the player, then destroys the zone.
-- DESCRIPTION: [SearchRange=1000(1,5000)]
-- DESCRIPTION: [ZoneHeight=100(0,1000)]
-- DESCRIPTION: [SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this zone
-- DESCRIPTION: [WeaponAmmunition=100(1,50)]
-- DESCRIPTION: [@WeaponState=1(1=Hidden, 2=Shown)]
-- DESCRIPTION: <Sound0> - Zone Entry Sound

local U = require "scriptbank\\utillib"
local gwzone 			= {}
local SearchRange		= {}
local ZoneHeight		= {}
local SpawnAtStart		= {}
local WeaponAmmunition	= {}
local WeaponState		= {}

local status 		= {}
local gwzweapon		= {}
local doonce		= {}
local ammocheck		= {}
local wait			= {}
local played		= {}

function get_weapon_zone_properties(e, SearchRange, ZoneHeight, SpawnAtStart, WeaponAmmunition, WeaponState)
    gwzone[e] = g_Entity[e]
	gwzone[e].SearchRange = SearchRange	
	gwzone[e].ZoneHeight = ZoneHeight
	gwzone[e].SpawnAtStart = SpawnAtStart
	gwzone[e].WeaponAmmunition = WeaponAmmunition
	gwzone[e].WeaponState = WeaponState or 1
end

function get_weapon_zone_init(e)
    gwzone[e] = {}
	gwzone[e].SearchRange =	1000
	gwzone[e].ZoneHeight = 100
	gwzone[e].SpawnAtStart = 1
	gwzone[e].WeaponAmmunition = 50
	gwzone[e].WeaponState = 1
	
	status[e] = "init"
	gwzweapon[e] = 0
	doonce[e] = 0
	wait[e] = math.huge
	ammocheck[e] = 0
	played[e] = 0
end

function get_weapon_zone_main(e)
	
	if status[e] == "init" then
		if gwzone[e].SpawnAtStart == 1 then SetActivated(e,1) end
		if gwzone[e].SpawnAtStart == 0 then SetActivated(e,0) end		
		local obList = U.ClosestEntities(gwzone[e].SearchRange,10,g_Entity[e]['x'],g_Entity[e]['z'])
		for k, v in pairs(obList) do
			if GetEntityWeaponID(v) ~= 0 then
				gwzweapon[e] = v
				if gwzone[e].WeaponState == 1 then Hide(v) end
				break
			end
		end
		status[e] = "endinit"		
	end
	
	if g_Entity[e]['activated'] == 1 then
		if g_Entity[e]['plrinzone'] == 1 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y'] + gwzone[e].ZoneHeight then
			if doonce[e] == 0 then
				AddPlayerWeapon(gwzweapon[e])				
				ActivateIfUsed(gwzweapon[e])
				Hide(gwzweapon[e])
				for index = 1, 10, 1 do
					WeaponID = GetPlayerWeaponID()
					GetWeaponSlot(index)
					local poolindex = GetWeaponPoolAmmoIndex(index)
					local amqty = GetWeaponPoolAmmo(poolindex)
					SetWeaponPoolAmmo(poolindex,amqty + gwzone[e].WeaponAmmunition)
				end
				if gwzweapon[e] == 0 then PromptLocal(e,"No Weapon Found within range of this zone") end
				if gwzweapon[e] ~= 0 then wait[e] = g_Time + 500 end
				doonce[e] = 1
			end	
			if played[e] == 0 then 
				PlaySound(e,0)
				played[e] = 1
			end				
		end
		if g_Time > wait[e] and doonce[e] == 1 then
			Destroy(gwzweapon[e])
			Destroy(e)
		end
	end	
end
