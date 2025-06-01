-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Global Indicator v6 by Necrym59
-- DESCRIPTION: Will display and update a screen image indicator based on a global value.
-- DESCRIPTION: Attach to an entity. Set to AlwaysActive
-- DESCRIPTION: [@GLOBAL_TO_MONITOR=1(1=Health, 2=User Global)]
-- DESCRIPTION: [@@USER_GLOBAL$=""(0=globallist)] eg: MyGlobal
-- DESCRIPTION: [SCREEN_POSITION_X=2(0,100)]
-- DESCRIPTION: [SCREEN_POSITION_Y=10(0,100)]
-- DESCRIPTION: [SCREEN_SCALE=50(1,100)]
-- DESCRIPTION: [FIRST_IMAGEFILE$="imagebank\\indicator\\indicator_1.png"]
-- DESCRIPTION: [FIRST_INDICATOR_VALUE=0]
-- DESCRIPTION: [SECOND_IMAGEFILE$="imagebank\\indicator\\indicator_2.png"]
-- DESCRIPTION: [SECOND_INDICATOR_VALUE=25]
-- DESCRIPTION: [THIRD_IMAGEFILE$="imagebank\\indicator\\indicator_3.png"]
-- DESCRIPTION: [THIRD_INDICATOR_VALUE=50]
-- DESCRIPTION: [FOURTH_IMAGEFILE$="imagebank\\indicator\\indicator_4.png"]
-- DESCRIPTION: [FOURTH_INDICATOR_VALUE=75]
-- DESCRIPTION: [FIFTH_IMAGEFILE$="imagebank\\indicator\\indicator_5.png"]
-- DESCRIPTION: [FIFTH_INDICATOR_VALUE=100]

local glindicator 				= {}
local global_to_monitor			= {}
local user_global 				= {}
local screen_position_x 		= {}
local screen_position_y 		= {}
local screen_scale 				= {}
local first_imagefile 			= {}
local first_indicator_value 	= {}
local second_imagefile 			= {}
local second_indicator_value	= {}
local third_imagefile 			= {}
local third_indicator_value 	= {}
local fourth_imagefile 			= {}
local fourth_indicator_value	= {}
local fifth_imagefile 			= {}
local fifth_indicator_value		= {}

local status		= {}
local currentvalue	= {}
local played		= {}
local sp_icon1		= {}
local sp_icon2		= {}
local sp_icon3		= {}
local sp_icon4		= {}
local sp_icon5		= {}
local sp_imgwidth	= {}
local sp_imgheight	= {}
local sp_aspect		= {} 

function global_indicator_properties(e, global_to_monitor, user_global, screen_position_x, screen_position_y, screen_scale, first_imagefile, first_indicator_value, second_imagefile, second_indicator_value, third_imagefile, third_indicator_value, fourth_imagefile, fourth_indicator_value, fifth_imagefile, fifth_indicator_value)
	glindicator[e].global_to_monitor = global_to_monitor or 1
	glindicator[e].user_global = user_global
	glindicator[e].screen_position_x = screen_position_x
	glindicator[e].screen_position_y = screen_position_y
	glindicator[e].screen_scale = screen_scale
	glindicator[e].first_imagefile = first_imagefile
	glindicator[e].first_indicator_value = first_indicator_value
	glindicator[e].second_imagefile = second_imagefile
	glindicator[e].second_indicator_value = second_indicator_value
	glindicator[e].third_imagefile = third_imagefile
	glindicator[e].third_indicator_value = third_indicator_value
	glindicator[e].fourth_imagefile = fourth_imagefile
	glindicator[e].fourth_indicator_value = fourth_indicator_value
	glindicator[e].fifth_imagefile = fifth_imagefile
	glindicator[e].fifth_indicator_value = fifth_indicator_value		
end

function global_indicator_init(e)
	glindicator[e] = {}
	glindicator[e].global_to_monitor = 1
	glindicator[e].user_global = ""
	glindicator[e].screen_position_x = 0
	glindicator[e].screen_position_y = 0
	glindicator[e].screen_scale = 50
	glindicator[e].first_imagefile = ""
	glindicator[e].first_indicator_value = 0
	glindicator[e].second_imagefile = ""
	glindicator[e].second_indicator_value = 25
	glindicator[e].third_imagefile = ""
	glindicator[e].third_indicator_value = 50
	glindicator[e].fourth_imagefile = ""
	glindicator[e].fourth_indicator_value = 75
	glindicator[e].fifth_imagefile = ""
	glindicator[e].fifth_indicator_value = 100
	
	status[e] = "init"
	currentvalue[e] = 0
	played[e] = 0
end

function global_indicator_main(e)

	if status[e] == "init" then
		sp_icon1[e] = CreateSprite(LoadImage(glindicator[e].first_imagefile))
		sp_imgwidth[e] = GetImageWidth(LoadImage(glindicator[e].first_imagefile))
		sp_imgheight[e] = GetImageHeight(LoadImage(glindicator[e].first_imagefile))
		sp_aspect[e] = sp_imgheight[e] / sp_imgwidth[e]
		SetSpriteSize(sp_icon1[e],glindicator[e].screen_scale/sp_aspect[e],glindicator[e].screen_scale)
		SetSpriteDepth(sp_icon1[e],100)
		SetSpritePosition(sp_icon1[e],500,500)

		sp_icon2[e] = CreateSprite(LoadImage(glindicator[e].second_imagefile))
		SetSpriteSize(sp_icon2[e],glindicator[e].screen_scale/sp_aspect[e],glindicator[e].screen_scale)
		SetSpriteDepth(sp_icon2[e],100)
		SetSpritePosition(sp_icon2[e],500,500)

		sp_icon3[e] = CreateSprite(LoadImage(glindicator[e].third_imagefile))
		SetSpriteSize(sp_icon3[e],glindicator[e].screen_scale/sp_aspect[e],glindicator[e].screen_scale)
		SetSpriteDepth(sp_icon3[e],100)
		SetSpritePosition(sp_icon3[e],500,500)

		sp_icon4[e] = CreateSprite(LoadImage(glindicator[e].fourth_imagefile))
		SetSpriteSize(sp_icon4[e],glindicator[e].screen_scale/sp_aspect[e],glindicator[e].screen_scale)
		SetSpriteDepth(sp_icon4[e],100)
		SetSpritePosition(sp_icon4[e],500,500)

		sp_icon5[e] = CreateSprite(LoadImage(glindicator[e].fifth_imagefile))
		SetSpriteSize(sp_icon5[e],glindicator[e].screen_scale/sp_aspect[e],glindicator[e].screen_scale)
		SetSpriteDepth(sp_icon5[e],100)
		SetSpritePosition(sp_icon5[e],500,500)

		status[e] = "endinit"
	end
	
	if glindicator[e].global_to_monitor == 1 then
		if g_PlayerHealth <= glindicator[e].first_indicator_value then
			if glindicator[e].first_imagefile > "" then PasteSpritePosition(sp_icon1[e],glindicator[e].screen_position_x,glindicator[e].screen_position_y) end
		end
		if g_PlayerHealth > glindicator[e].first_indicator_value and g_PlayerHealth <= glindicator[e].second_indicator_value then
			if glindicator[e].second_imagefile > "" then PasteSpritePosition(sp_icon2[e],glindicator[e].screen_position_x,glindicator[e].screen_position_y) end
		end
		if g_PlayerHealth > glindicator[e].second_indicator_value and g_PlayerHealth <= glindicator[e].third_indicator_value then
			if glindicator[e].third_imagefile > "" then PasteSpritePosition(sp_icon3[e],glindicator[e].screen_position_x,glindicator[e].screen_position_y) end
		end
		if g_PlayerHealth > glindicator[e].third_indicator_value and g_PlayerHealth <= glindicator[e].fourth_indicator_value then
			if glindicator[e].fourth_imagefile > "" then PasteSpritePosition(sp_icon4[e],glindicator[e].screen_position_x,glindicator[e].screen_position_y) end
		end
		if g_PlayerHealth > glindicator[e].fourth_indicator_value and g_PlayerHealth <= glindicator[e].fifth_indicator_value then
			if glindicator[e].fifth_imagefile > "" then  PasteSpritePosition(sp_icon5[e],glindicator[e].screen_position_x,glindicator[e].screen_position_y) end
		end
	end

	if glindicator[e].global_to_monitor == 2 then
		if glindicator[e].user_global > "" then
			if _G["g_UserGlobal['"..glindicator[e].user_global.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..glindicator[e].user_global.."']"] end
			if currentvalue[e] <= glindicator[e].first_indicator_value then
				PasteSpritePosition(sp_icon1[e],glindicator[e].screen_position_x,glindicator[e].screen_position_y)
			end
			if currentvalue[e] > glindicator[e].first_indicator_value and currentvalue[e] <= glindicator[e].second_indicator_value then
				PasteSpritePosition(sp_icon2[e],glindicator[e].screen_position_x,glindicator[e].screen_position_y)
			end
			if currentvalue[e] > glindicator[e].second_indicator_value and currentvalue[e] <= glindicator[e].third_indicator_value then
				PasteSpritePosition(sp_icon3[e],glindicator[e].screen_position_x,glindicator[e].screen_position_y)
			end
			if currentvalue[e] > glindicator[e].third_indicator_value and currentvalue[e] <= glindicator[e].fourth_indicator_value then
				PasteSpritePosition(sp_icon4[e],glindicator[e].screen_position_x,glindicator[e].screen_position_y)
			end
			if currentvalue[e] > glindicator[e].fourth_indicator_value and currentvalue[e] <= glindicator[e].fifth_indicator_value then
				PasteSpritePosition(sp_icon5[e],glindicator[e].screen_position_x,glindicator[e].screen_position_y)
			end
		end	
	end
end