-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Carry Object V32 by Necrym59 and Lee
-- DESCRIPTION: A gobal behaviour for object handling, place on one in map object only.
-- DESCRIPTION: Set Object to Physics=ON, AlwaysActive=ON
-- DESCRIPTION: Weight: Must be between 1-99. 0=No Pickup.
-- DESCRIPTION: [PICKUP_TEXT$="Press Q or Hold LMB to carry, RMB to carry/throw"]
-- DESCRIPTION: [PICKUP_RANGE=80(1,500)]
-- DESCRIPTION: [MAX_PICKUP_WEIGHT=99(1,99)]
-- DESCRIPTION: [MAX_PICKUP_SIZE=40(1,100)]
-- DESCRIPTION: [RELEASE_TEXT$="Use R or LMB to drop"]
-- DESCRIPTION: [THROW_TEXT$="Shift to add force - Release RMB to throw"]
-- DESCRIPTION: [REARM_WEAPON!=0]
-- DESCRIPTION: [DIAGNOSTICS!=0]
-- DESCRIPTION: <Sound0> when picking up object.

	local U = require "scriptbank\\utillib"
	local P = require "scriptbank\\physlib"

	g_carrying				= {}
	g_carryingweight		= {}

	local carry_object		= {}
	local pickup_text 		= {}
	local pickup_range 		= {}
	local pickup_weight		= {}
	local pickup_size		= {}
	local throw_text 		= {}
	local release_text 		= {}
	local rearm_weapon 		= {}

	local carry_mode 		= {}
	local new_y 			= {}
	local prop_x 			= {}
	local prop_y 			= {}
	local propang_y 		= {}
	local prop_z 			= {}
	local thrown 			= {}
	local doonce 			= {}
	local showonce 			= {}
	local status 			= {}
	local nearEnt			= {}
	local allegiance		= {}
	local objmass 			= {}
	local objheight			= {}
	local objweight			= {}
	local objforce			= {}
	local objwidth			= {}
	local objlength			= {}
	local carrydist			= {}
	local nocarry			= {}
	local last_gun			= {}
	local kpressed			= {}
	local colobj			= {}
	local tEnt 				= {}
	local selectobj 		= {}
	local cox				= {}
	local fgain				= {}
	local hurtonce			= {}

function carry_object_properties(e, pickup_text, pickup_range, max_pickup_weight, max_pickup_size, release_text, throw_text, rearm_weapon, diagnostics)
	carry_object[e] = g_Entity[e]
	carry_object[e].pickup_text = pickup_text
	carry_object[e].pickup_range = pickup_range
	carry_object[e].pickup_weight = max_pickup_weight or 99
	carry_object[e].pickup_size = max_pickup_size
	carry_object[e].release_text = release_text
	carry_object[e].throw_text = throw_text	
	carry_object[e].rearm_weapon = rearm_weapon
	carry_object[e].diagnostics = diagnostics
end

function carry_object_init(e)
	carry_object[e] = {}
	carry_object[e].prompt_text = "Hold Q or LMB to carry, RMB to carry and throw"
	carry_object[e].pickup_range = 50
	carry_object[e].pickup_weight = 99
	carry_object[e].pickup_size = 40
	carry_object[e].release_text = "Use R or LMB to drop"
	carry_object[e].throw_text = "Release RMB to throw"	
	carry_object[e].rearm_weapon = 0
	carry_object[e].diagnostics = 0
	carry_mode[e] = 0
	new_y[e] = 0
	prop_x[e] = 0
	prop_z[e] = 0
	propang_y[e] = 0
	doonce[e] = 0
	thrown[e] = 0
	kpressed[e] = 0
	carrydist[e] = 0
	nocarry[e] = 2
	nearEnt[e] = 0
	g_carrying = 0
	g_carryingweight = 0
	status[e] = 'init'
	tEnt[e] = 0
	cox[e] = 0
	fgain[e] = 0
	hurtonce[e] = 0
	allegiance[e] = 0
	last_gun[e] = g_PlayerGunName
	colobj[e] = 0
end

function carry_object_main(e)
	carry_object[e] = g_Entity[e]

	if status[e] == 'init' then
		if carry_object[e].pickup_weight > 99 then carry_object[e].pickup_weight = 99 end
		if carry_object[e].pickup_size > 100 then carry_object[e].pickup_size = 100 end
		status[e] = 'pickup'
	end

	if status[e] == 'pickup' then
		nearEnt[e] = U.ClosestEntToPos(g_PlayerPosX, g_PlayerPosZ,carry_object[e].pickup_range*3)
		if nearEnt[e] ~= 0 or nearEnt[e] ~= 1 and nearEnt[e] > 1 then
			nocarry[e] = 2
			-- pinpoint select object--
			local px, py, pz = GetCameraPositionX(0), GetCameraPositionY(0), GetCameraPositionZ(0)
			local rayX, rayY, rayZ = 0,0,carry_object[e].pickup_range
			local paX, paY, paZ = math.rad(GetCameraAngleX(0)), math.rad(GetCameraAngleY(0)), math.rad(GetCameraAngleZ(0))
			rayX, rayY, rayZ = U.Rotate3D(rayX, rayY, rayZ, paX, paY, paZ)
			selectobj[e]=IntersectAll(px,py,pz, px+rayX, py+rayY, pz+rayZ,e)
			if selectobj[e] ~= 0 then				
				tEnt[e] = P.ObjectToEntity(selectobj[e])
				allegiance[e] = GetEntityAllegiance(tEnt[e])
				if allegiance[e] == -1 then
					local xmin, ymin, zmin, xmax, ymax, zmax = GetObjectColBox(g_Entity[tEnt[e]]['obj'])
					local sx, sy, sz = GetObjectScales(g_Entity[tEnt[e]]['obj'])
					local w, h, l = (xmax - xmin) * sx, (ymax - ymin) * sy, (zmax - zmin) * sz
					local massmod = GetEntityWeight(tEnt[e])/100
					local weight = GetEntityWeight(tEnt[e])
					objmass[tEnt[e]] = (w*h*l)/50*massmod
					objheight[tEnt[e]] = 5
					objweight[tEnt[e]] = weight
					objforce[tEnt[e]] = math.min(weight,objmass[tEnt[e]])/1.5
					objwidth[tEnt[e]] = w
					objlength[tEnt[e]] = l
					local pd = GetPlayerDistance(tEnt[e])
					carrydist[tEnt[e]] = GetPlayerDistance(tEnt[e])
					nocarry[e] = 0
					if w > carrydist[tEnt[e]] then carrydist[tEnt[e]] = carrydist[tEnt[e]] + 15 end
					if l > carrydist[tEnt[e]] then carrydist[tEnt[e]] = carrydist[tEnt[e]] + 15 end
					if l > carry_object[e].pickup_size and w > carry_object[e].pickup_size then nocarry[e] = 1 end					
					if objweight[tEnt[e]] == 0 then nocarry[e] = 1 end
				end
				if allegiance[e] == -1 then
					if objweight[tEnt[e]] <= carry_object[e].pickup_weight and nocarry[e] == 0 or nocarry[e] == nil then
						TextCenterOnXColor(50-0.01,50,3,"+",255,255,255)
						TextCenterOnX(50,95,3,carry_object[e].pickup_text)
					end
					if objweight[tEnt[e]] > carry_object[e].pickup_weight or nocarry[e] == 1 then
						tEnt[e] = 0
					end
				end
				if allegiance[e] ~= -1 then					
					tEnt[e] = 0
				end	
				--end pinpoint select object--
			end
			if selectobj[e] == 0 then tEnt[e] = 0 end
			if nearEnt[e] ~= tEnt[e] then				
				selectobj[e] = 0
			end			
		else
			nearEnt[e] = 0
			selectobj[e] = 0
			tEnt[e] = 0
		end
		
		if tEnt[e] ~= 0 and GetEntityVisibility(tEnt[e]) == 1 then			
			if g_KeyPressQ == 1 and g_carrying == 0 then
				kpressed[e] = 1				
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
				g_KeyPressQ = 1
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
				g_KeyPressQ = 1
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
		end
	end
	
	if status[e] == 'carry' then
		g_carryingweight = GetEntityWeight(tEnt[e])
		if doonce[e] == 0 then
			new_y[tEnt[e]] = math.rad(g_PlayerAngY)
			prop_x[tEnt[e]] = g_PlayerPosX + (math.sin(new_y[tEnt[e]]) * carrydist[tEnt[e]])
			prop_y[tEnt[e]] = g_PlayerPosY - math.sin(math.rad(g_PlayerAngX))*carrydist[tEnt[e]]
			prop_z[tEnt[e]] = g_PlayerPosZ + (math.cos(new_y[tEnt[e]]) * carrydist[tEnt[e]])
			SetEntityZDepthMode(tEnt[e],2)
			doonce[e] = 1
		end
		
		if g_MouseWheel < 0 then
			SetPlayerWeapons(0)
			objheight[tEnt[e]] = objheight[tEnt[e]] - 1			
			if objheight[tEnt[e]] < -10 then objheight[tEnt[e]] = -10 end			
		elseif g_MouseWheel > 0 then
			SetPlayerWeapons(0)
			objheight[tEnt[e]] = objheight[tEnt[e]] + 1
			if objheight[tEnt[e]] > 25 then objheight[tEnt[e]] = 25 end
		end		
		GravityOff(tEnt[e])
		CollisionOff(tEnt[e])
		PositionObject(g_Entity[tEnt[e]]['obj'],prop_x[tEnt[e]],prop_y[tEnt[e]]+objheight[tEnt[e]],prop_z[tEnt[e]])
		RotateObject(g_Entity[tEnt[e]]['obj'],0,g_Entity[tEnt[e]]['angley'],g_PlayerAngZ)
		CollisionOn(tEnt[e])
		GravityOn(tEnt[e])		
		if g_MouseClick == 2 and g_carrying == 1 then
			TextCenterOnX(50,95,3,carry_object[e].throw_text)
			if g_KeyPressSHIFT == 1 then
				fgain[e] = fgain[e] + 0.02
				TextCenterOnX(50,97,3,"Increasing throw force by " ..math.ceil(fgain[e]))
			end
			if fgain[e] >= 20 then fgain[e] = 20 end
		else
			TextCenterOnX(50,95,3,carry_object[e].release_text)
		end
		
		if ( GetGamePlayerStateCamAngleX()<-35) then SetGamePlayerStateCamAngleX(-35) end
		if ( GetGamePlayerStateCamAngleX()>35) then SetGamePlayerStateCamAngleX(35) end	
		
		new_y[tEnt[e]] = math.rad(g_PlayerAngY)
		prop_x[tEnt[e]] = g_PlayerPosX + (math.sin(new_y[tEnt[e]]) * carrydist[tEnt[e]])
		prop_y[tEnt[e]] = g_PlayerPosY - math.sin(math.rad(g_PlayerAngX))* carrydist[tEnt[e]]
		prop_z[tEnt[e]] = g_PlayerPosZ + (math.cos(new_y[tEnt[e]]) * carrydist[tEnt[e]])
		local px, py, pz = prop_x[tEnt[e]], prop_y[tEnt[e]], prop_z[tEnt[e]]
		local rayX, rayY, rayZ = 5,0,10
		local paX, paY, paZ = math.rad(g_PlayerAngX), math.rad(g_PlayerAngY), math.rad(g_PlayerAngZ)
		rayX, rayY, rayZ = U.Rotate3D(rayX, rayY, rayZ, paX, paY, paZ)		
		colobj[tEnt[e]]=IntersectAll(px,py,pz, px+rayX, py, pz+rayZ,g_Entity[tEnt[e]]['obj']) --avoids pushing carryobj through wall!
		--colobj[tEnt[e]]=IntersectAll(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ, px+rayX, py, pz+rayZ,g_Entity[tEnt[e]]['obj'])
		if colobj[tEnt[e]] > 0 then
			ForcePlayer(g_PlayerAngY + 180,0.3)
			CollisionOn(tEnt[e])
		end
		
		if g_MouseClick == 2 and g_carrying == 1 then thrown[e] = 1 end
		if g_KeyPressR == 1 or g_MouseClick == 1 or g_MouseClick == 2 then kpressed[e] = 0 end
		if kpressed[e] == 0 and g_MouseClick == 0 and colobj[e] == 0 then
			SetEntityZDepthMode(tEnt[e],1)			
			CollisionOff(tEnt[e])
			local sh = GetSurfaceHeight(g_Entity[tEnt[e]]['x'],g_Entity[tEnt[e]]['y'],g_Entity[tEnt[e]]['z'])
			if prop_y[tEnt[e]] < sh then prop_y[tEnt[e]] = sh end
			PositionObject(g_Entity[tEnt[e]]['obj'],prop_x[tEnt[e]],prop_y[tEnt[e]],prop_z[tEnt[e]])
			CollisionOn(tEnt[e])
			objheight[tEnt[e]] = 5
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
		if thrown[e] == 1 and g_MouseClick == 0 and g_KeyPressQ == 0 and GetEntityVisibility(tEnt[e]) == 1 then
			SetEntityZDepthMode(tEnt[e],1)
			local paX, paY, paZ = math.rad( g_PlayerAngX ), math.rad( g_PlayerAngY ),math.rad( g_PlayerAngZ )
			local vx, vy, vz = U.Rotate3D( 0, 0, 1, paX, paY, paZ)
			objforce[tEnt[e]] = objforce[tEnt[e]] + (fgain[e]*10)
			PushObject(g_Entity[tEnt[e]]['obj'],vx*objforce[tEnt[e]], vy*objforce[tEnt[e]], vz*objforce[tEnt[e]], math.random()/100, math.random()/100, math.random()/100 )			
			thrown[e] = 2
			status[e] = 'thrown'
		end
	end
	if status[e] == 'thrown' then
		for _, v in pairs(U.ClosestEntities(90,math.huge,g_Entity[tEnt[e]]['x'],g_Entity[tEnt[e]]['z'])) do
			if GetEntityAllegiance(v) >= -1 then
				if g_Entity[v]['health'] > 0 then
					SetEntityHealth(v,g_Entity[v]['health']-(objforce[tEnt[e]]))
					fgain[e] = 0
					hurtonce[e] = 1
				end
				if g_Entity[v] == nil then
					fgain[e] = 0
					status[e] = 'pickup'
				end	
			end
		end
		if hurtonce[e] == 1 then
			hurtonce[e] = 0
			status[e] = 'pickup'
		end
	end
	if carry_object[e].diagnostics == 1 then
	
		Text(10,54,3,"Entity #: ")
		Text(20,54,3,tEnt[e])
		Text(10,56,3,"Max Weight: ")
		Text(20,56,3,carry_object[e].pickup_weight)
		Text(10,58,3,"Weight: ")
		Text(20,58,3,objweight[tEnt[e]])
		Text(10,60,3,"Max Size: ")
		Text(20,60,3,carry_object[e].pickup_size.. " Width/Length")
		Text(10,62,3,"Width: ")
		Text(20,62,3,objwidth[tEnt[e]])
		Text(10,64,3,"Length: ")
		Text(20,64,3,objlength[tEnt[e]])
		Text(10,66,3,"Mass: ")
		Text(20,66,3,objmass[tEnt[e]])
		Text(10,68,3,"Force: ")
		Text(20,68,3,objforce[tEnt[e]])
		Text(10,70,3,"Force Gain: ")
		Text(20,70,3,fgain[e])
		if nocarry[e] == 0 then	Text(10,74,3,"Can Carry :   Yes") end
		if nocarry[e] == 1 then	Text(10,74,3,"Can Carry :   No") end	
	end
end