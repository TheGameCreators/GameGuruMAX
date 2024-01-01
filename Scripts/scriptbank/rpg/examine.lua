-- Examine v6: by Necrym59
-- DESCRIPTION: [PICKUP_RANGE=100]
-- DESCRIPTION: [PICKUP_MESSAGE$="E to Examine object"]
-- DESCRIPTION: [EXAMINE_MESSAGE$="Hmmm..."]
-- DESCRIPTION: [EXAMINE_SPEED=50]

U = require "scriptbank\\utillib"
Q = require "scriptbank\\quatlib"
local rad = math.rad
local deg = math.deg
local abs = math.abs

local examine 				= {}
local pickup_range			= {}
local pickup_message		= {}
local examine_message		= {}
local examine_speed			= {}

local exminetime		= {}
local status 			= {}
local tEnt 				= {}
local selectobj 		= {}
local startposx 		= {}
local startposy 		= {}
local startposz 		= {}
local startangx 		= {}
local startangy 		= {}
local startangz 		= {}
local new_y 			= {}
local prop_x 			= {}
local prop_y 			= {}
local prop_z 			= {}
local prop_h 			= {}	
local last_gun			= {}

function examine_properties(e, pickup_range, pickup_message, examine_message, examine_speed)
	examine[e] = g_Entity[e]
	examine[e].pickup_range = pickup_range
	examine[e].pickup_message =  pickup_message
	examine[e].examine_message = examine_message
	examine[e].examine_speed = examine_speed
end 

function examine_init(e)
	examine[e] = {}
	examine[e].pickup_range = 100
	examine[e].pickup_message = "E to Examine object"
	examine[e].examine_message = "Hmmm..."
	examine[e].examine_speed = 50	
	status[e] = "init"
	exminetime[e] = 0
	last_gun[e] = g_PlayerGunName
end 

function examine_main(e)
	examine[e] = g_Entity[e]
	
	local PlayerDist = GetPlayerDistance(e)
	if status[e] == "init" then
		local ex,ey,ez,eax,eay,eaz = GetEntityPosAng(e)
		startposx[e] = ex
		startposy[e] = ey
		startposz[e] = ez
		startangx[e] = eax
		startangy[e] = eay
		startangz[e] = eaz
		last_gun[e] = g_PlayerGunName	
		status[e] = "pick up"
	end
	
	if status[e] == "pick up" then 	
		if PlayerDist < examine[e].pickup_range then			
			-- pinpoint select object--			
			local px, py, pz = GetCameraPositionX(0), GetCameraPositionY(0), GetCameraPositionZ(0)
			local rayX, rayY, rayZ = 0,0,examine[e].pickup_range
			local paX, paY, paZ = math.rad(GetCameraAngleX(0)), math.rad(GetCameraAngleY(0)), math.rad(GetCameraAngleZ(0))
			rayX, rayY, rayZ = U.Rotate3D(rayX, rayY, rayZ, paX, paY, paZ)
			selectobj[e]=IntersectAll(px,py,pz, px+rayX, py+rayY, pz+rayZ,e)
			if selectobj[e] ~= 0 or selectobj[e] ~= nil then
				if g_Entity[e]['obj'] == selectobj[e] then
					TextCenterOnXColor(50-0.01,50,3,"+",255,255,255) --highliting (with crosshair at present)
					tEnt[e] = e
				else 
					tEnt[e] = 0				
				end
			end
			if selectobj[e] == 0 or selectobj[e] == nil then
				tEnt[e] = 0
				TextCenterOnXColor(50-0.01,50,3,"+",155,155,155) --highliting (with crosshair at present)
			end
			--end pinpoint select object--
		end	
		if PlayerDist < examine[e].pickup_range and tEnt[e] ~= 0 and GetEntityVisibility(e) == 1 then
			PromptLocal(e, examine[e].pickup_message)				
			if g_KeyPressE == 1 then
				GravityOff(e)
				CollisionOff(e)
				local xmin, ymin, zmin, xmax, ymax, zmax = GetObjectColBox(g_Entity[tEnt[e]]['obj'])
				local sx, sy, sz = GetObjectScales(g_Entity[tEnt[e]]['obj'])
				local w, h, l = (xmax - xmin) * sx, (ymax - ymin) * sy, (zmax - zmin) * sz
				prop_h[e] = h
				new_y[tEnt[e]] = math.rad(g_PlayerAngY)
				prop_x[tEnt[e]] = g_PlayerPosX + (math.sin(new_y[tEnt[e]]) * 20)
				prop_y[tEnt[e]] = g_PlayerPosY - (math.sin(math.rad(GetCameraAngleX(0)))* 30)+30
				prop_z[tEnt[e]] = g_PlayerPosZ + (math.cos(new_y[tEnt[e]]) * 20)
				PositionObject(g_Entity[tEnt[e]]['obj'],prop_x[tEnt[e]],prop_y[tEnt[e]]-h/2,prop_z[tEnt[e]])		
				RotateObject(g_Entity[tEnt[e]]['obj'],0,g_Entity[tEnt[e]]['angley'],g_PlayerAngZ)
				SetCameraOverride(3)
				exminetime[e] = 0
				last_gun[e] = g_PlayerGunName
				if g_PlayerGunID > 0 then
					CurrentlyHeldWeaponID = GetPlayerWeaponID()
					SetPlayerWeapons(0)
				end
				status[e] = "examining"							
			end 
		end 
	end
	
	if status[e] == "examining" then
		ActivateMouse()
		PromptLocal(e,examine[e].examine_message)
		Prompt("WASD or MB1+Move, MMW=Up/Dn, Q to Exit")
		exminetime[e] = GetElapsedTime() * 100
		PositionObject(g_Entity[tEnt[e]]['obj'],prop_x[tEnt[e]],prop_y[tEnt[e]]-prop_h[e]/2,prop_z[tEnt[e]])
		RotateObject(g_Entity[tEnt[e]]['obj'],g_Entity[tEnt[e]]['anglex'],g_Entity[tEnt[e]]['angley'],g_Entity[tEnt[e]]['anglez'])	

		if g_KeyPressW == 1 then 
			RotateX(e, examine[e].examine_speed * exminetime[e])
		end
		if g_KeyPressS == 1 then 
			RotateX(e, -examine[e].examine_speed * exminetime[e])
		end 		
		if g_KeyPressA == 1 then 
			RotateY(e, -examine[e].examine_speed * exminetime[e])
		end
		if g_KeyPressD == 1 then 
			RotateY(e, examine[e].examine_speed * exminetime[e])
		end
		
		if g_MouseClick == 1 or g_MouseClick == 2 then
			omx, omy = (g_MouseX-50)/50, (g_MouseY-50)/50
			if abs( omx ) < 0.05 then omx = 0 end
			if abs( omy ) < 0.05 then omy = 0 end
			if omx < 0 then 
				RotateY(e,(omx*100)*exminetime[e])
			end
			if omx > 0 then 
				RotateY(e,(omx*100)*exminetime[e])
			end
			if omy < 0 then 
				RotateX(e,(omy*100)*exminetime[e])
			end
			if omy > 0 then 
				RotateX(e,(omy*100)*exminetime[e])
			end
		end		
		
		if g_MouseWheel < 0 then	
			prop_y[tEnt[e]] = prop_y[tEnt[e]] + 1
		elseif g_MouseWheel > 0 then
			prop_y[tEnt[e]] = prop_y[tEnt[e]] - 1
		end
		if g_KeyPressQ == 1 then 
			exminetime[e] = 0				
			SetCameraOverride(0)
			DeactivateMouse()
			ResetPosition(e, startposx[e], startposy[e], startposz[e])
			ResetRotation(e, startangx[e], startangy[e], startangz[e])
			CollisionOn(e)
			GravityOn(e)
			ChangePlayerWeapon(last_gun[e])
			SetPlayerWeapons(1)
			ChangePlayerWeaponID(CurrentlyHeldWeaponID)
			status[e] = "pick up"
		end 
	end
end 