-- Npc In Cover v4 by Smallg and Necrym59
-- DESCRIPTION: Allows an NPC to pop in and out view from cover and fire at player when within range
-- DESCRIPTION: [@COVER_STYLE=1(1=Up/Down, 2=Left, 3=Right)]
-- DESCRIPTION: [ALERT_DISTANCE= 2000]
-- DESCRIPTION: [EXPOSURE_TIME=1500]
-- DESCRIPTION: [COVER_TIME=1200]
-- DESCRIPTION: [@STAND_ANIMATION=-1(0=AnimSetList)]
-- DESCRIPTION: [@DUCK_ANIMATION=-1(0=AnimSetList)]
-- DESCRIPTION: [@LEFT_ANIMATION=-1(0=AnimSetList)]
-- DESCRIPTION: [@RIGHT_ANIMATION=-1(0=AnimSetList)]
-- DESCRIPTION: [@SHOOT_ANIMATION=-1(0=AnimSetList)]

local lower = string.lower
local npcincover 		= {}
local cover_style		= {}
local alert_distance 	= {}
local exposure_time 	= {}
local cover_time 		= {}
local stand_animation	= {}
local duck_animation	= {}
local left_animation	= {}
local right_animation	= {}
local shoot_animation	= {}

local firerate			= {}
local firedelay 		= {}
local weaponid 			= {}
local reloadquantity	= {}
local ammo 				= {}
local standing			= {}
local status			= {}
local doonce			= {}

function npc_incover_properties(e, cover_style, alert_distance, exposure_time, cover_time, stand_animation, duck_animation, left_animation, right_animation, shoot_animation)
	npcincover[e].cover_style = cover_style
	npcincover[e].alert_distance = alert_distance
	npcincover[e].exposure_time = exposure_time
	npcincover[e].cover_time = cover_time
	npcincover[e].stand_animation = "=" .. tostring(stand_animation)
	npcincover[e].duck_animation = "=" .. tostring(duck_animation)
	npcincover[e].left_animation = "=" .. tostring(left_animation)	
	npcincover[e].right_animation = "=" .. tostring(right_animation)	
	npcincover[e].shoot_animation = "=" .. tostring(shoot_animation)
end

function npc_incover_init(e)
	npcincover[e] = {}
	npcincover[e].cover_style = 1
	npcincover[e].alert_distance = 2200
	npcincover[e].exposure_time = 1450
	npcincover[e].cover_time = 2000
	npcincover[e].stand_animation = ""
	npcincover[e].duck_animation = ""
	npcincover[e].left_animation = ""
	npcincover[e].right_animation = ""
	npcincover[e].shoot_animation = ""

	npcincover[e].firedelay = g_Time
	npcincover[e].weaponid = GetEntityHasWeapon(e)
	npcincover[e].reloadquantity = GetWeaponReloadQuantity(npcincover[e].weaponid,0)
	npcincover[e].ammo = npcincover[e].reloadquantity
	npcincover[e].standing = e
	npcincover[e].firerate = GetWeaponFireRate(npcincover[e].weaponid, 0)
	if npcincover[e].firerate >= 12 then 					-- adjust firerate to millisecond delay equiv to player
		npcincover[e].firerate = npcincover[e].firerate * 20  -- favor slower standard non-auto weapons
	else
		npcincover[e].firerate = npcincover[e].firerate * 10	-- favor faster standard automatic weapons
	end
	doonce[e] = 0
	status[e] = "init"
end

function npc_incover_main(e)

	if status[e] == "init" then
		if npcincover[e].cover_style == 1 then
			SetAnimationName(e,npcincover[e].duck_animation)
			PlayAnimation(e)
		end	
		if npcincover[e].cover_style == 2 then
			SetAnimationName(e,npcincover[e].left_animation)
			PlayAnimation(e)
		end
		if npcincover[e].cover_style == 3 then
			SetAnimationName(e,npcincover[e].right_animation)
			PlayAnimation(e)
		end		
		status[e] = "endinit"
	end

	if (GetPlayerDistance(e)) < npcincover[e].alert_distance then
		doonce[e] = 0
		if npcincover[e].standing == e then
			if (GetTimer(e)) < npcincover[e].exposure_time and npcincover[e].ammo > 0 then
				RotateToPlayer(e)
				if GetEntityCanFire(e) == 1 then
					if g_Time > npcincover[e].firedelay then						
						FireWeaponInstant(e)
						SetAnimationName(e,npcincover[e].shoot_animation)
						PlayAnimation(e)
						npcincover[e].firedelay = g_Time + npcincover[e].firerate
						npcincover[e].ammo = npcincover[e].ammo - 1
					end
				end
			else
				StartTimer(e)				
				npcincover[e].standing = 0
				if npcincover[e].cover_style == 1 then
					SetAnimationName(e,npcincover[e].duck_animation)
					PlayAnimation(e)
				end
				if npcincover[e].cover_style == 2 then
					SetAnimationName(e,npcincover[e].left_animation)
					PlayAnimation(e)
				end
				if npcincover[e].cover_style == 3 then
					SetAnimationName(e,npcincover[e].right_animation)
					PlayAnimation(e)
				end
			end
		else
			if (GetTimer(e)) < npcincover[e].cover_time then
			else
				StartTimer(e)
				if npcincover[e].standing == 0 then
					npcincover[e].standing = e
					npcincover[e].ammo = npcincover[e].reloadquantity
					if npcincover[e].cover_style == 1 then
						SetAnimationName(e,npcincover[e].stand_animation)
						PlayAnimation(e)
					end
					if npcincover[e].cover_style == 2 then
						SetAnimationName(e,npcincover[e].stand_animation)
						PlayAnimation(e)						
					end
					if npcincover[e].cover_style == 3 then
						SetAnimationName(e,npcincover[e].stand_animation)
						PlayAnimation(e)
					end					
					PlaySound(e,1)
				end
			end
		end
	else
		StartTimer(e)
		if npcincover[e].standing == e then 
			npcincover[e].standing = 0
		end
	end
	if (GetPlayerDistance(e)) > npcincover[e].alert_distance then
		if doonce[e] == 0 then
			if npcincover[e].cover_style == 1 then
				SetAnimationName(e,npcincover[e].duck_animation)
				PlayAnimation(e)
			end
			if npcincover[e].cover_style == 2 then

				SetAnimationName(e,npcincover[e].left_animation)
				PlayAnimation(e)
			end
			if npcincover[e].cover_style == 3 then
				SetAnimationName(e,npcincover[e].right_animation)
				PlayAnimation(e)
			end
			doonce[e] = 1
		end
	end
end


-- Exit Function
function npc_incover_exit(e)
	PlaySound(e,2)
	if standing == e then
		standing = 0
	end
end
