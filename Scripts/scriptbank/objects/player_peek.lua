-- Player_Peek v6 - Thanks to Amen Moses modified by Necrym59
-- DESCRIPTION: Will add player peeking functionality
-- DESCRIPTION: Attach to an object and set AwaysActive=ON
-- DESCRIPTION: [PROMPT$="A=Peek Left, D=Peek Right, W=PeekOver -if crouched"]
-- DESCRIPTION: [PEEK_SIDE_AMOUNT=30(0,45)]
-- DESCRIPTION: [PEEK_UP_AMOUNT=10]
-- DESCRIPTION: [#PEEK_SPEED=10.0(0.1,20.0)]
-- DESCRIPTION: [@PEEK_STYLE=1(1=Toggle with key, 2=Hold MMB)]
-- DESCRIPTION: [PEEK_TOGGLE_KEY$="Q"]
-- DESCRIPTION: [AUTO_CROUCH!=0]
-- DESCRIPTION: [HIDE_WEAPON!=0]

local U = require "scriptbank\\utillib"
local Q = require "scriptbank\\quatlib"

local rad = math.rad
local deg = math.deg
local player_peek		= {}
local prompt			= {}
local peek_side_amount 	= {}
local peek_up_amount   	= {}
local peek_speed		= {}
local peek_style		= {}
local peek_toggle_key  	= {}
local auto_crouch		= {}
local hide_weapon		= {}

local status		= {}
local keypressed	= {}
local keypause 		= {}
local usemouse		= {}
local lastgun     	= {}

local peekOn   		= 0
local peekCurrent 	= 0
local peekSide		= 0
local peekUp		= 0
local peekInc		= 0
local peekCrouch	= 0
local peekWeapon	= 0
local peekPrompt	= ""
local peekedLeft	= 0
local peekedRight	= 0
local peekedUp		= 0
local gpclm 		= nil
local ox,oy,oz 		= 0, 0, 0
local cx,cy,cz 		= 0, 0, 0
local cquat 		= nil
local rquat 		= nil

function player_peek_properties(e, prompt, peek_side_amount, peek_up_amount, peek_speed, peek_style, peek_toggle_key, auto_crouch, hide_weapon)
	player_peek[e].prompt = prompt
	player_peek[e].peek_side_amount = peek_side_amount
	player_peek[e].peek_up_amount = peek_up_amount
	player_peek[e].peek_speed = peek_speed
	player_peek[e].peek_style = peek_style
	player_peek[e].peek_toggle_key = peek_toggle_key
	player_peek[e].auto_crouch = auto_crouch or 0
	player_peek[e].hide_weapon = hide_weapon or 0
end

function player_peek_init(e)
	player_peek[e] = {}
	player_peek[e].prompt =	"A=Peek Left, D=Peek Right, W=PeekOver -if crouched"
	player_peek[e].peek_side_amount = 30
	player_peek[e].peek_up_amount   = 20
	player_peek[e].peek_speed = 10.0
	player_peek[e].peek_style = 1
	player_peek[e].peek_toggle_key = "Q"
	player_peek[e].auto_crouch = 0
	player_peek[e].hide_weapon = 0

	status[e] = "init"
	keypressed[e] = 0
	keypause[e] = 0
	usemouse[e] = 0	
	SetEntityAlwaysActive(e,1)
end

function player_peek_main(e)

	if 	status[e] == "init" then
		if player_peek[e].peek_side_amount > 45 then player_peek[e].peek_side_amount = 45 end
		peekSide = player_peek[e].peek_side_amount/2
		peekUp = player_peek[e].peek_up_amount
		peekCrouch = player_peek[e].auto_crouch
		peekPrompt = player_peek[e].prompt
		peekInc = player_peek[e].peek_speed
		peekWeapon = player_peek[e].hide_weapon
		status[e] = "endinit"
	end

	if player_peek[e].peek_style == 1 then
		if g_Time > keypause[e] and keypressed[e] == 0 then
			if GetInKey() == string.upper(player_peek[e].peek_toggle_key) or GetInKey() == string.lower(player_peek[e].peek_toggle_key) and keypressed[e] == 0 then
				PromptDuration(peekPrompt,1000)
				keypressed[e] = 1
				keypause[e] = g_Time + 1000
				peekOn = 1
				lastgun = g_PlayerGunName
				if peekOn == 1 then
					SetCameraOverride(3)
					cx = rad( GetGamePlayerControlFinalCameraAnglex() + GetGamePlayerStateShakeX() + GetGamePlayerControlFlinchCurrentx() )
					cy = rad( GetGamePlayerControlFinalCameraAngley() + GetGamePlayerStateShakeY() + GetGamePlayerControlFlinchCurrenty() )
					cz = rad( GetGamePlayerControlFinalCameraAnglez() + GetGamePlayerStateShakeZ() + GetGamePlayerControlFlinchCurrentz() )
					cquat = Q.FromEuler( cx, cy, cz )
				end				
			end
		end
		if g_Time > keypause[e] and keypressed[e] == 1 then
			if GetInKey() == string.upper(player_peek[e].peek_toggle_key) or GetInKey() == string.lower(player_peek[e].peek_toggle_key) and keypressed[e] == 1 then
				keypressed[e] = 0
				keypause[e] = g_Time + 1000
				peekOn = 0
				if player_peek[e].hide_weapon == 1 then
					SetPlayerWeapons(1)
					ChangePlayerWeapon(lastgun)
				end	
			end
		end
	end
	if player_peek[e].peek_style == 2 then
		if g_MouseClick == 4 and keypressed[e] == 0 then
			PromptDuration(peekPrompt,1000)
			keypressed[e] = 1
			keypause[e] = g_Time + 1000
			peekOn = 1
			lastgun = g_PlayerGunName
			if peekOn == 1 then
				SetCameraOverride(3)
				cx = rad( GetGamePlayerControlFinalCameraAnglex() + GetGamePlayerStateShakeX() + GetGamePlayerControlFlinchCurrentx() )
				cy = rad( GetGamePlayerControlFinalCameraAngley() + GetGamePlayerStateShakeY() + GetGamePlayerControlFlinchCurrenty() )
				cz = rad( GetGamePlayerControlFinalCameraAnglez() + GetGamePlayerStateShakeZ() + GetGamePlayerControlFlinchCurrentz() )
				cquat = Q.FromEuler( cx, cy, cz )
			end
		end
		if g_MouseClick ~= 4 and keypressed[e] == 1 and peekCurrent == 0 then
			keypressed[e] = 0
			keypause[e] = g_Time + 1000
			peekOn = 0
			if player_peek[e].hide_weapon == 1 then
				SetPlayerWeapons(1)
				ChangePlayerWeapon(lastgun)
			end
		end
	end

	if keypressed[e] == 1 then
		if gpclm == nil then
			if gameplayercontrol ~= nil then
				gpclm = gameplayercontrol.lookmove
				gameplayercontrol.lookmove = lean_player
			else
				Prompt( "No gameplayercontrol" )
			end
		end
	end
end

function incrementPeek(maximum)
	if peekCurrent >= maximum then
		return false
	else
		local inc = peekInc * GetTimeElapsed()
		peekCurrent = peekCurrent + inc
		return true
	end
end

function decrementPeek(maximum)
	if peekCurrent >= maximum then
		return false
	else
		local inc = peekInc * GetTimeElapsed()
		peekCurrent = peekCurrent - inc
		return true
	end
end

function lean_player()
	gpclm()

	if peekOn ~= 0 then
		if peekCrouch == 1 then
			g_PlrKeyC = 1
			SetGamePlayerStatePlayerDucking(2)
		end
		if peekWeapon == 1 then
			SetPlayerWeapons(0)
		end
		local ax, ay, az = rad(g_PlayerAngX), rad(g_PlayerAngY), rad(g_PlayerAngZ)
		if g_KeyPressA == 1 then
			peekedLeft = 1
			ox,oy,oz = U.Rotate3D( -peekCurrent, 0, 0, ax, ay, az )
			local rot = incrementPeek(peekSide)
			if rot then
				rquat = Q.Mul( cquat, Q.FromEuler( 0, 0, rad(peekCurrent)))
			end
		elseif g_KeyPressD == 1 then
			peekedRight = 1
			ox, oy, oz = U.Rotate3D( peekCurrent, 0, 0, ax, ay, az )
			local rot = incrementPeek(peekSide)
			if rot then
				rquat = Q.Mul( cquat, Q.FromEuler( 0, 0, rad( -peekCurrent)))
			end
		elseif g_KeyPressW == 1 then
			peekedUp = 1
			if g_PlrKeyC == 1 then
				rquat = nil
				ox, oy, oz = U.Rotate3D(0, peekCurrent, 0, ax, ay, az)
				incrementPeek(peekUp)
			end
		else
			if peekedLeft == 1 and g_KeyPressA == 0 then
				ox,oy,oz = U.Rotate3D( -peekCurrent, 0, 0, ax, ay, az )
				if peekCurrent > 0 then
					local rot = decrementPeek(peekSide)
					if rot then
						rquat = Q.Mul( cquat, Q.FromEuler( 0, 0, rad(peekCurrent)))
					end
					peekCurrent = peekCurrent - (peekInc * GetTimeElapsed())
					if peekCurrent <= 0 then
						peekedLeft = 0
						peekCurrent = 0
						ox,oy,oz = 0, 0, 0
						rquat = nil
					end
				end
			end
			if peekedRight == 1 and g_KeyPressD == 0 then
				ox,oy,oz = U.Rotate3D( peekCurrent, 0, 0, ax, ay, az )
				if peekCurrent > 0 then
					local rot = decrementPeek(peekUp)
					if rot then
						rquat = Q.Mul( cquat, Q.FromEuler( 0, 0, rad( -peekCurrent)))
					end
					peekCurrent = peekCurrent - (peekInc * GetTimeElapsed())
					if peekCurrent <= 0 then
						peekedRight = 0
						peekCurrent = 0
						ox,oy,oz = 0, 0, 0
						rquat = nil
					end
				end
			end
			if peekedUp == 1 and g_KeyPressW == 0 and g_PlrKeyC == 1 then
				if peekCurrent > 0 then
					rquat = nil
					ox, oy, oz = U.Rotate3D(0, peekCurrent, 0, ax, ay, az)
					decrementPeek(peekUp)
					peekCurrent = peekCurrent - (peekInc * GetTimeElapsed())
					if peekCurrent <= 0 then
						peekedUp = 0
						peekCurrent = 0
						ox,oy,oz = 0, 0, 0
						rquat = nil
					end
				end
			end
		end
		PositionCamera (0,GetGamePlayerStateFinalCamX()+ox, GetGamePlayerStateFinalCamY()+oy, GetGamePlayerStateFinalCamZ()+oz)
		if rquat ~= nil then
			local ncx, ncy, ncz = Q.ToEuler(rquat)
			SetCameraAngle(0,deg(ncx),deg(ncy),deg(ncz))
		else
			SetCameraAngle(0,deg(cx),deg(cy),deg(cz))
		end
	else
		SetCameraOverride(0)
		peekCurrent = 0	
		rquat = nil
	end
end