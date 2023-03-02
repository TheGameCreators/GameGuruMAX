-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Carry Object V10 by Necrym59
-- DESCRIPTION: Set Object to Physics=ON, Gravity=ON
-- DESCRIPTION: [PICKUP_TEXT$="Hold E or LMB to carry, RMB to throw"]
-- DESCRIPTION: [PICKUP_RANGE=40(1,500)]
-- DESCRIPTION: [RELEASE_TEXT$="Release E or LMB to drop, RMB-throw"]
-- DESCRIPTION: [REARM_WEAPON!=0]
-- DESCRIPTION: <Sound0> when picking up object.

	local U = require "scriptbank\\utillib"
	local P = require "scriptbank\\physlib"

	g_carrying				= {}
	g_carryingweight		= {}

	local carry_object		= {}
	local pickup_text 		= {}
	local pickup_range 		= {}
	local release_text 		= {}
	local rearm_weapon 		= {}

	local carry_mode 		= {}
	local new_y 			= {}
	local prop_x 			= {}
	local prop_y 			= {}
	local prop_z 			= {}
	local thrown 			= {}
	local doonce 			= {}
	local status 			= {}
	local objmass 			= {}
	local objheight			= {}
	local carrydist			= {}
	local last_gun			= {}
	local colobj			= {}

function carry_object_properties(e, pickup_text, pickup_range, release_text, rearm_weapon)
	carry_object[e] = g_Entity[e]
	carry_object[e].pickup_text = pickup_text
	carry_object[e].pickup_range = pickup_range
	carry_object[e].release_text = release_text
	carry_object[e].rearm_weapon = rearm_weapon
end

function carry_object_init(e)
	carry_object[e] = g_Entity[e]
	carry_object[e].prompt_text = "Hold E or LMB to carry, RMB to throw"
	carry_object[e].pickup_range = pickup_range
	carry_object[e].release_text = "Release E or LMB to drop, RMB to throw"
	carry_object[e].rearm_weapon = 0
	carry_mode[e] = 0
	new_y[e] = 0
	prop_x[e] = 0
	prop_z[e] = 0
	doonce[e] = 0
	thrown[e] = 0
	carrydist[e] = 0
	g_carrying = 0
	g_carryingweight = 0
	status[e] = 'init'
	last_gun[e] = g_PlayerGunName
	colobj[e] = 0
end

function carry_object_main(e)
	carry_object[e] = g_Entity[e]

	if status[e] == 'init' then
		local xmin, ymin, zmin, xmax, ymax, zmax = GetObjectColBox(g_Entity[e]['obj'])
		local sx, sy, sz = GetObjectScales(g_Entity[e]['obj'])
		local w, h, l = (xmax - xmin) * sx, (ymax - ymin) * sy, (zmax - zmin) * sz
		local massmod = GetEntityWeight(e) / 100
		objmass[e] = (w*h*l)/50*massmod
		objheight[e] = 5
		if w > 40 and  w > l then carrydist[e] = w else carrydist[e] = 40 end
		if l > 40 and  l > w then carrydist[e] = l else carrydist[e] = 40 end
		if carry_object[e].pickup_range < carrydist[e] then carry_object[e].pickup_range = carrydist[e]+15 end
		status[e] = 'pickup'
	end

	if status[e] == 'pickup' then
		local PlayerDist = GetPlayerDistance(e)

		if PlayerDist < carry_object[e].pickup_range and g_PlayerHealth > 0 then
			local LookingAt = GetPlrLookingAtEx(e,1)
			if LookingAt == 1 then
				Prompt(carry_object[e].pickup_text)
				if g_KeyPressE == 1 and g_carrying == 0 then
					status[e] = 'carry'
					g_carrying = 1
					last_gun[e] = g_PlayerGunName
					if g_PlayerGunID > 0 then
						CurrentlyHeldWeaponID = GetPlayerWeaponID()
						SetPlayerWeapons(0)
					end
					PlaySound(e,0)
				end
				if g_MouseClick == 1 and g_carrying == 0 then
					g_KeyPressE = 1
					last_gun[e] = g_PlayerGunName
					if g_PlayerGunID > 0 then
						CurrentlyHeldWeaponID = GetPlayerWeaponID()
						SetPlayerWeapons(0)
					end
					PlaySound(e,0)
					status[e] = 'carry'
					g_carrying = 1
					PlaySound(e,0)
				end
				if g_MouseClick == 2 and g_carrying == 0 then
					g_KeyPressE = 1
					last_gun[e] = g_PlayerGunName
					if g_PlayerGunID > 0 then
						CurrentlyHeldWeaponID = GetPlayerWeaponID()
						SetPlayerWeapons(0)
					end
					PlaySound(e,0)
					status[e] = 'carry'
					g_carrying = 1
					PlaySound(e,0)
					thrown[e] = 1
				end
				if thrown[e] == 1 and g_KeyPressE == 0 then
					SetEntityZDepthMode(e,1)
					local force = objmass[e]/2
					local paX, paY, paZ = math.rad( g_PlayerAngX ), math.rad( g_PlayerAngY ),math.rad( g_PlayerAngZ )
					local vx, vy, vz = U.Rotate3D( 0, 0, 1, paX, paY, paZ)
					PushObject(g_Entity[e]['obj'],vx*force, vy*force, vz*force, math.random()/100, math.random()/100, math.random()/100 )
					thrown[e] = 0
				end
			end
		end
	end

	if status[e] == 'carry' then
		g_carryingweight = GetEntityWeight(e)
		if doonce[e] == 0 then
			new_y[e] = math.rad(g_PlayerAngY)
			prop_x[e] = g_PlayerPosX + (math.sin(new_y[e]) * 40)
			prop_y[e] = g_PlayerPosY - math.sin(math.rad(g_PlayerAngX))*40
			prop_z[e] = g_PlayerPosZ + (math.cos(new_y[e]) * 40)
			SetEntityZDepthMode(e,2)
			GravityOff(e)
			doonce[e] = 1
		end

		if g_MouseWheel < 0 then
			SetPlayerWeapons(0)
			objheight[e] = objheight[e] - 1
			if objheight[e] < -10 then objheight[e] = -10 end
		elseif g_MouseWheel > 0 then
			SetPlayerWeapons(0)
			objheight[e] = objheight[e] + 1
			if objheight[e] > 25 then objheight[e] = 25 end
		end

		local th = GetTerrainHeight(g_Entity[e]['x'],g_Entity[e]['z'])
		if g_Entity[e]['y'] < th then prop_y[e] = th+1 end

		CollisionOff(e)
		PositionObject(g_Entity[e]['obj'],prop_x[e],prop_y[e]+objheight[e],prop_z[e])
		RotateObject(g_Entity[e]['obj'],0,g_PlayerAngY,g_PlayerAngZ)
		CollisionOn(e)
		Prompt(carry_object[e].release_text)
		-- Cap look up/down angle so cannot wrap around
		if ( GetGamePlayerStateCamAngleX()<-15) then SetGamePlayerStateCamAngleX(-15) end
		if ( GetGamePlayerStateCamAngleX()>55 ) then SetGamePlayerStateCamAngleX(55) end

		new_y[e] = math.rad(g_PlayerAngY)
		prop_x[e] = g_PlayerPosX + (math.sin(new_y[e]) * carrydist[e])
		prop_y[e] = g_PlayerPosY - math.sin(math.rad(g_PlayerAngX))*carrydist[e]
		prop_z[e] = g_PlayerPosZ + (math.cos(new_y[e]) * carrydist[e])

		local px, py, pz = g_PlayerPosX, g_PlayerPosY+(carrydist[e]+15), g_PlayerPosZ
		local rayX, rayY, rayZ = 0,0,carrydist[e]+10
		local paX, paY, paZ = math.rad(g_PlayerAngX), math.rad(g_PlayerAngY), math.rad(g_PlayerAngZ)
		rayX, rayY, rayZ = U.Rotate3D(rayX, rayY, rayZ, paX, paY, paZ)
		colobj[e]=(IntersectAll(px,py,pz, px+rayX, py+rayY, pz+rayZ,e))
		if colobj[e] > 0 then
			ForcePlayer(g_PlayerAngY + 180,0.3)
			CollisionOn(e)
		end

		if g_KeyPressE == 0 and g_MouseClick == 0 and colobj[e] == 0 then
			SetEntityZDepthMode(e,1)
			CollisionOff(e)
			PositionObject(g_Entity[e]['obj'],prop_x[e],prop_y[e]-15,prop_z[e])
			CollisionOn(e)
			GravityOn(e)
			objheight[e] = 5
			doonce[e] = 0
			status[e] = 'pickup'
			g_carrying = 0
			g_carryingweight = 0
			if carry_object[e].rearm_weapon == 1 then
				ChangePlayerWeapon(last_gun[e])
				SetPlayerWeapons(1)
				ChangePlayerWeaponID(CurrentlyHeldWeaponID)
			else
				SetPlayerWeapons(1)
			end			
		end
	end
end

