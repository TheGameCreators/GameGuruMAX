-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Hot_Key v3 by Necrym59
-- DESCRIPTION: Will display an icon image on screen for a specified resource and activation key
-- DESCRIPTION: Attach to an entity. Set to AlwaysActive
-- DESCRIPTION: [SCREEN_POSITION_X=2(0,100)]
-- DESCRIPTION: [SCREEN_POSITION_Y=10(0,100)]
-- DESCRIPTION: [SCREEN_SCALE=10(1,100)]
-- DESCRIPTION: [ICON1_IMAGEFILE$=""] Inactive icon image
-- DESCRIPTION: [ICON2_IMAGEFILE$=""] Active icon image
-- DESCRIPTION: [RESOURCE_NAME$=""] eg: "Potion"
-- DESCRIPTION: [RESOURCE_AMOUNT=10(0,100)] Amount to add
-- DESCRIPTION: [@APPLIED_TO=1(1=Health, 2=User Global)]
-- DESCRIPTION: [USER_GLOBAL_AFFECTED$=""] eg:(MyMana or MyStaminaMax)
-- DESCRIPTION: [ACTIVATION_KEY$="Z"] Bound key to activate
-- DESCRIPTION: <Sound0> - Activation Sound

local lower = string.lower
local hotkey 					= {}
local screen_position_x 		= {}
local screen_position_y 		= {}
local screen_scale 				= {}
local icon1_imagefile 			= {}
local icon2_imagefile 			= {}
local resource_name				= {}
local resource_amount			= {}
local applied_to				= {}
local user_global_affected		= {}
local activation_key			= {}

local status			= {}
local sp_hsicon1		= {}
local sp_imgwidth1		= {}
local sp_imgheight1		= {}
local sp_aspect1		= {}
local sp_hsicon2		= {}
local sp_imgwidth2		= {}
local sp_imgheight2		= {}
local sp_aspect2		= {}

local tobeused			= {}
local currentvalue		= {}
local doonce			= {}
local icontimer			= {}
local keypressed		= {}

function hot_key_properties(e, screen_position_x, screen_position_y, screen_scale, icon1_imagefile, icon2_imagefile, resource_name, resource_amount, applied_to, user_global_affected, activation_key)
	hotkey[e].screen_position_x = screen_position_x
	hotkey[e].screen_position_y = screen_position_y
	hotkey[e].screen_scale = screen_scale
	hotkey[e].icon1_imagefile = icon1_imagefile
	hotkey[e].icon2_imagefile = icon2_imagefile
	hotkey[e].resource_name = string.lower(resource_name)
	hotkey[e].resource_amount = resource_amount
	hotkey[e].applied_to = resource_amount
	hotkey[e].user_global_affected = user_global_affected
	hotkey[e].activation_key = activation_key
end

function hot_key_init(e)
	hotkey[e] = {}
	hotkey[e].screen_position_x = screen_position_x
	hotkey[e].screen_position_y = screen_position_y
	hotkey[e].screen_scale = screen_scale	
	hotkey[e].icon1_imagefile = ""
	hotkey[e].icon2_imagefile = ""
	hotkey[e].resource_name = ""
	hotkey[e].resource_amount = 0
	hotkey[e].applied_to = 1
	hotkey[e].user_global_affected = ""
	hotkey[e].activation_key = "Z"
	
	status[e] = "init"
	tobeused[e] = 0
	currentvalue[e] = 0
	icontimer[e] = 0
	keypressed[e] = 0
	doonce[e] = 0
end

function hot_key_main(e)
	
	if status[e] == "init" then
		--Find Icons --
		if hotkey[e].icon1_imagefile ~= "" then
			sp_hsicon1[e] = CreateSprite(LoadImage(hotkey[e].icon1_imagefile))
			sp_imgwidth1[e] = GetImageWidth(LoadImage(hotkey[e].icon1_imagefile))
			sp_imgheight1[e] = GetImageHeight(LoadImage(hotkey[e].icon1_imagefile))
			sp_aspect1[e] = sp_imgheight1[e] / sp_imgwidth1[e]
			SetSpriteSize(sp_hsicon1[e],hotkey[e].screen_scale/sp_aspect1[e],hotkey[e].screen_scale)			
			SetSpriteDepth(sp_hsicon1[e],100)
			SetSpritePosition(sp_hsicon1[e],500,500)
		end	
		if hotkey[e].icon2_imagefile ~= "" then			
			sp_hsicon2[e] = CreateSprite(LoadImage(hotkey[e].icon2_imagefile))
			sp_imgwidth2[e] = GetImageWidth(LoadImage(hotkey[e].icon2_imagefile))
			sp_imgheight2[e] = GetImageHeight(LoadImage(hotkey[e].icon2_imagefile))
			sp_aspect2[e] = sp_imgheight2[e] / sp_imgwidth2[e]
			SetSpriteSize(sp_hsicon2[e],hotkey[e].screen_scale/sp_aspect1[e],hotkey[e].screen_scale)	
			SetSpriteDepth(sp_hsicon2[e],100)
			SetSpritePosition(sp_hsicon2[e],500,500)
		end	
		status[e] = "hotkey"
	end
	
	if status[e] == "hotkey" then
		--Check for Resource --
		if hotkey[e].resource_name > "" and tobeused[e] == 0 then
			for ee = 1, g_EntityElementMax do
				if ee ~= nil and g_Entity[ee] ~= nil then
					if string.lower(GetEntityName(ee)) == hotkey[e].resource_name and GetEntityCollected(ee) == 1 and GetEntityUsed(ee) ~= -1 then
						tobeused[e] = ee						
						break
					end
				end			
			end
		end
		
		if hotkey[e].icon1_imagefile ~= "" then
			if tobeused[e] == 0 then
				SetSpriteColor(sp_hsicon1[e],255,255,255,80)
				PasteSpritePosition(sp_hsicon1[e],hotkey[e].screen_position_x,hotkey[e].screen_position_y)
			end			
			if tobeused[e] > 0 then
				SetSpriteColor(sp_hsicon1[e],255,255,255,255)
				PasteSpritePosition(sp_hsicon1[e],hotkey[e].screen_position_x,hotkey[e].screen_position_y)
			end
		end
		
		if tobeused[e] > 0 then
			if g_InKey == string.lower(hotkey[e].activation_key) or g_InKey == tostring(hotkey[e].activation_key) and keypressed[e] == 0 then
				icontimer[e] = g_Time + 1000
				keypressed[e] = 1
				PlaySound(e,0)
				if tobeused[e] > 0 then
					if doonce[e] == 0 then
						if hotkey[e].applied_to == 1 then
							SetPlayerHealth(g_PlayerHealth+hotkey[e].resource_amount)
							if g_PlayerHealth > g_PlayerStartStrength then g_PlayerHealth = g_PlayerStartStrength end
							SetPlayerHealthCore(g_PlayerHealth)
							doonce[e] = 1
						end
						if hotkey[e].applied_to == 2 then
							if  hotkey[e].user_global_affected > "" then 
								if _G["g_UserGlobal['".. hotkey[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['".. hotkey[e].user_global_affected.."']"] end
								_G["g_UserGlobal['".. hotkey[e].user_global_affected.."']"] = currentvalue[e] + hotkey[e].resource_amount
							end
							doonce[e] = 1
						end						
					end
					SetEntityUsed(tobeused[e],1*-1)
					SetEntityCollected(tobeused[e],0)
					Destroy(tobeused[e])					
					tobeused[e] = 0
				end
			end
		end	
		if g_Time < icontimer[e] then PasteSpritePosition(sp_hsicon2[e],hotkey[e].screen_position_x,hotkey[e].screen_position_y) end
		if g_Time > icontimer[e] then
			doonce[e] = 0
			keypressed[e] = 0
		end
	end
end