-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Global Transfer v7 by Necrym59
-- DESCRIPTION: Will transfer a value from a user global to player health or another user global.
-- DESCRIPTION: Attach to an entity. Set to AlwaysActive
-- DESCRIPTION: [@ACTIVATION=1(1=Automatic, 2=Key Press)]
-- DESCRIPTION: [ACTIVATION_KEY$="Z"] Bound key to activate
-- DESCRIPTION: [@@TRANSFER_FROM$=""(0=globallist)] user global to transfer amount from eg:(MyStamina)
-- DESCRIPTION: [TRANSFER_AMOUNT=10(0,100)] Amount to transfer
-- DESCRIPTION: [@TRANSFER_TO=1(1=Player Health, 2=User Global)]
-- DESCRIPTION: [@@USER_GLOBAL$=""(0=globallist)] user global to transfer amount to:(MyMoney)
-- DESCRIPTION: [ICON_IMAGEFILE$=""]Icon image
-- DESCRIPTION: [SCREEN_POSITION_X=2(0,100)]
-- DESCRIPTION: [SCREEN_POSITION_Y=10(0,100)]
-- DESCRIPTION: [SCREEN_SCALE=10(1,100)]
-- DESCRIPTION: <Sound0> - Activation Sound

local lower = string.lower
local global_transfer 			= {}
local activation				= {}
local activation_key			= {}
local transfer_from				= {}
local transfer_amount			= {}
local transfer_to				= {}
local user_global				= {}
local icon_imagefile 			= {}
local screen_position_x 		= {}
local screen_position_y 		= {}
local screen_scale 				= {}

local status			= {}
local sp_icon			= {}
local sp_imgwidth		= {}
local sp_imgheight		= {}
local sp_aspect			= {}

local tobeused			= {}
local currentvalue		= {}
local doonce			= {}
local icontimer			= {}
local keypressed		= {}
local keypause	 		= {}
local autocheck			= {}
local processed			= {}
local tmphealth			= {}
local played			= {}

function global_transfer_properties(e, activation, activation_key, transfer_from, transfer_amount, transfer_to, user_global, icon_imagefile, screen_position_x, screen_position_y, screen_scale)
	global_transfer[e].activation = activation or 1
	global_transfer[e].activation_key = activation_key
	global_transfer[e].transfer_from = transfer_from
	global_transfer[e].transfer_amount = transfer_amount
	global_transfer[e].transfer_to = transfer_to or 2
	global_transfer[e].user_global = user_global
	global_transfer[e].icon_imagefile = icon_imagefile
	global_transfer[e].screen_position_x = screen_position_x
	global_transfer[e].screen_position_y = screen_position_y
	global_transfer[e].screen_scale = screen_scale
end

function global_transfer_init(e)
	global_transfer[e] = {}
	global_transfer[e].activation = 1
	global_transfer[e].activation_key = "Z"
	global_transfer[e].transfer_from = ""
	global_transfer[e].transfer_amount = 0
	global_transfer[e].transfer_to = 2
	global_transfer[e].user_global = ""
	global_transfer[e].icon_imagefile = ""
	global_transfer[e].screen_position_x = screen_position_x
	global_transfer[e].screen_position_y = screen_position_y
	global_transfer[e].screen_scale = screen_scale

	status[e] = "init"
	currentvalue[e] = 0
	keypause[e] = math.huge
	keypressed[e] = 0
	doonce[e] = 0
	autocheck[e] = 0
	processed[e] = 0
	tmphealth[e] = 0
	played[e] = 0
end

function global_transfer_main(e)

	if status[e] == "init" then
		--Find Icon --
		if global_transfer[e].icon_imagefile ~= "" then
			sp_icon[e] = CreateSprite(LoadImage(global_transfer[e].icon_imagefile))
			sp_imgwidth[e] = GetImageWidth(LoadImage(global_transfer[e].icon_imagefile))
			sp_imgheight[e] = GetImageHeight(LoadImage(global_transfer[e].icon_imagefile))
			sp_aspect[e] = sp_imgheight[e] / sp_imgwidth[e]
			SetSpriteSize(sp_icon[e],global_transfer[e].screen_scale/sp_aspect[e],global_transfer[e].screen_scale)
			SetSpriteDepth(sp_icon[e],100)
			SetSpritePosition(sp_icon[e],500,500)
			autocheck[e] = g_Time + 5000
		end
		status[e] = "global_transfer"
	end

	if status[e] == "global_transfer" then
		if global_transfer[e].icon_imagefile ~= "" then
			if global_transfer[e].transfer_from > "" then
				if _G["g_UserGlobal['".. global_transfer[e].transfer_from.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['".. global_transfer[e].transfer_from.."']"] end
				if currentvalue[e] < global_transfer[e].transfer_amount then
					SetSpriteColor(sp_icon[e],255,255,255,50)
					PasteSpritePosition(sp_icon[e],global_transfer[e].screen_position_x,global_transfer[e].screen_position_y)
				end
				if currentvalue[e] >= global_transfer[e].transfer_amount then
					SetSpriteColor(sp_icon[e],255,255,255,255)
					PasteSpritePosition(sp_icon[e],global_transfer[e].screen_position_x,global_transfer[e].screen_position_y)
				end
			end
		else
			if global_transfer[e].transfer_from > "" then
				if _G["g_UserGlobal['".. global_transfer[e].transfer_from.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['".. global_transfer[e].transfer_from.."']"] end
			end
		end
		if global_transfer[e].activation == 1 and g_Time > autocheck[e] then -- Automatic
			if currentvalue[e] >= global_transfer[e].transfer_amount then
				if doonce[e] == 0 then
					if global_transfer[e].transfer_to == 1 then
						if g_PlayerHealth+global_transfer[e].transfer_amount < g_PlayerStartStrength then
							SetPlayerHealth(g_PlayerHealth+global_transfer[e].transfer_amount)
							SetPlayerHealthCore(g_PlayerHealth)
							_G["g_UserGlobal['".. global_transfer[e].transfer_from.."']"] = _G["g_UserGlobal['".. global_transfer[e].transfer_from.."']"] - global_transfer[e].transfer_amount
							doonce[e] = 1
							autocheck[e] = g_Time + 5000
						end
						if g_PlayerHealth+global_transfer[e].transfer_amount >= g_PlayerStartStrength then
							tmphealth[e] = (g_PlayerStartStrength - g_PlayerHealth)
							SetPlayerHealth(g_PlayerStartStrength)
							SetPlayerHealthCore(g_PlayerHealth)
							_G["g_UserGlobal['".. global_transfer[e].transfer_from.."']"] = _G["g_UserGlobal['".. global_transfer[e].transfer_from.."']"] - tmphealth[e]
							doonce[e] = 1
							autocheck[e] = g_Time + 5000
						end
					end
					if global_transfer[e].transfer_to == 2 then
						if global_transfer[e].user_global > "" then
							if _G["g_UserGlobal['".. global_transfer[e].user_global.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['".. global_transfer[e].user_global.."']"] end
							_G["g_UserGlobal['".. global_transfer[e].user_global.."']"] = currentvalue[e] + global_transfer[e].transfer_amount
						end
						doonce[e] = 1
						autocheck[e] = g_Time + 5000
						_G["g_UserGlobal['".. global_transfer[e].transfer_from.."']"] = _G["g_UserGlobal['".. global_transfer[e].transfer_from.."']"] - global_transfer[e].transfer_amount
					end
				end
			end
		end

		if global_transfer[e].activation == 2 and keypressed[e] == 0 then -- Keypress
			if g_InKey == string.lower(global_transfer[e].activation_key) or g_InKey == tostring(global_transfer[e].activation_key) and keypressed[e] == 0 then
				keypause[e] = g_Time + 1000
				keypressed[e] = 1
				if played[e] == 0 then
					PlaySound(e,0)
					played[e] = 1
				end
				if currentvalue[e] >= global_transfer[e].transfer_amount then
					if doonce[e] == 0 then
						if global_transfer[e].transfer_to == 1 then
							if g_PlayerHealth+global_transfer[e].transfer_amount < g_PlayerStartStrength then
								SetPlayerHealth(g_PlayerHealth+global_transfer[e].transfer_amount)
								SetPlayerHealthCore(g_PlayerHealth)
								_G["g_UserGlobal['".. global_transfer[e].transfer_from.."']"] = _G["g_UserGlobal['".. global_transfer[e].transfer_from.."']"] - global_transfer[e].transfer_amount
								doonce[e] = 1
							end
							if g_PlayerHealth+global_transfer[e].transfer_amount >= g_PlayerStartStrength then
								tmphealth[e] = (g_PlayerStartStrength - g_PlayerHealth)
								SetPlayerHealth(g_PlayerStartStrength)
								SetPlayerHealthCore(g_PlayerHealth)
								_G["g_UserGlobal['".. global_transfer[e].transfer_from.."']"] = _G["g_UserGlobal['".. global_transfer[e].transfer_from.."']"] - tmphealth[e]
								doonce[e] = 1
							end
						end
						if global_transfer[e].transfer_to == 2 then
							if global_transfer[e].user_global > "" then
								if _G["g_UserGlobal['".. global_transfer[e].user_global.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['".. global_transfer[e].user_global.."']"] end
								_G["g_UserGlobal['".. global_transfer[e].user_global.."']"] = currentvalue[e] + global_transfer[e].transfer_amount
							end
							doonce[e] = 1
							_G["g_UserGlobal['".. global_transfer[e].transfer_from.."']"] = _G["g_UserGlobal['".. global_transfer[e].transfer_from.."']"] - global_transfer[e].transfer_amount
						end
					end
				end
			end
		end

		if g_Time > keypause[e] then
			doonce[e] = 0
			played[e] = 0
			tmphealth[e] = 0
			keypressed[e] = 0
		end
		if g_Time > autocheck[e] then
			doonce[e] = 0
			tmphealth[e] = 0
		end
	end
end