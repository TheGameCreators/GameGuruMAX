-- Document v11 - thanks to Necrym59 and Lee
-- DESCRIPTION: Change the [PICKUP_TEXT$="E to look at document"].
-- DESCRIPTION: View position [SCREEN_X=25] and [SCREEN_Y=10]
-- DESCRIPTION: Set the [SPRITE_SIZE=15(1,100)] percentage.
-- DESCRIPTION: The document [IMAGEFILE$="imagebank\\documents\\default_doc.png"]
-- DESCRIPTION: Set the [PICKUP_RANGE=80(1,300)] 
-- DESCRIPTION: Change the [RETURN_TEXT$="Q to return document"].
-- DESCRIPTION: Set [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: Set to [LOCK_SCREEN!=0] when viewing.
-- DESCRIPTION: <Sound0> when picking/returning up document.

local U = require "scriptbank\\utillib"

local document 			= {}
local pickup_text 		= {}
local pickup_range 		= {}
local return_text 		= {}	
local screen_x 			= {}
local screen_y 			= {}
local sprite_size 		= {}
local imagefile 		= {}
local prompt_display 	= {}
local lock_screen 		= {}

local image 			= {}
local imgAspectRatio 	= {}
local imgWidth 			= {}
local imgHeight 		= {}
local showing 			= {}
local pressed 			= {}
local played			= {}
local doonce			= {}
local sprite 			= {}
local tEnt 				= {}
local selectobj			= {}
local status 			= {}
		
function document_properties(e, pickup_text, screen_x, screen_y, sprite_size, imagefile, pickup_range, return_text, prompt_display, lock_screen)
	document[e].pickup_text = pickup_text
	document[e].screen_x = screen_x					
	document[e].screen_y = screen_y					
	document[e].sprite_size = sprite_size			
	document[e].imagefile = imagefile
	if pickup_range == nil then pickup_range = 80 end
	document[e].pickup_range = pickup_range
	if return_text == nil then return_text = "" end
	document[e].return_text = return_text	
	if prompt_display == nil then prompt_display = 1 end
	document[e].prompt_display = prompt_display
	if lock_screen == nil then lock_screen = 0 end
	document[e].lock_screen = lock_screen
end 
	
function document_init_name(e)
	document[e] = {}	
	document[e].pickup_text = "E to look at document"
	document[e].pickup_range = 80
	document[e].return_text = "Q to return document"
	document[e].screen_x = 0	
	document[e].screen_y = 0
	document[e].sprite_size = 1
	document[e].imagefile = "imagebank\\documents\\default_doc.png"
	document[e].prompt_display = 1
	document[e].lock_screen = 0
	showing[e] = 0
	pressed[e] = 0
	sprite[e] = 0
	imgAspectRatio[e] = 0	
	imgWidth[e] = 0	
	imgHeight[e] = 0
	tEnt[e]	= 0
	selectobj[e] = 0	
	played[e] = 0
	doonce[e] = 0
	status[e] ="init"
end

function document_main(e)
	
	local PlayerDist = GetPlayerDistance(e)
	
	if status[e] == "init" then	
		SetCameraOverride(0)
		status[e] = "endinit"
	end	
	
	local LookingAt = GetPlrLookingAtEx(e,1)
	if PlayerDist < document[e].pickup_range and GetEntityVisibility(e) == 1 and LookingAt == 1 then
		-- pinpoint select object--		
		local px, py, pz = GetCameraPositionX(0), GetCameraPositionY(0), GetCameraPositionZ(0)
		local rayX, rayY, rayZ = 0,0,document[e].pickup_range
		local paX, paY, paZ = math.rad(GetCameraAngleX(0)), math.rad(GetCameraAngleY(0)), math.rad(GetCameraAngleZ(0))
		rayX, rayY, rayZ = U.Rotate3D(rayX, rayY, rayZ, paX, paY, paZ)
		selectobj[e]=IntersectAll(px,py,pz, px+rayX, py+rayY, pz+rayZ,e)
		if selectobj[e] ~= 0 or selectobj[e] ~= nil then
			if g_Entity[e].obj == selectobj[e] then
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
	
	if PlayerDist < document[e].pickup_range and tEnt[e] ~= 0 and GetEntityVisibility(e) == 1 then
		if showing[e] == 0 and tEnt[e] ~= 0 then
			if document[e].prompt_display ==1 then	PromptLocal(e,document[e].pickup_text) end
			if document[e].prompt_display ==2 then	Prompt(document[e].pickup_text) end
		end				
		if g_KeyPressE == 1 then
			if showing[e] == 0 and pressed[e] == 0 then
				pressed[e] = 1
				if played[e] == 0 then
					PlaySound(e,0)
					played[e] = 1
				end
				Hide(e)
				showing[e] = 1
				if showing[e] == 1 then
					if document[e].prompt_display ==1 then PromptLocal(e,document[e].return_text) end
					if document[e].prompt_display ==2 then PromptDuration(document[e].return_text,2000) end
				end
				if doonce[e] == 0 then
					image[e] = LoadImage(document[e].imagefile)
					sprite[e] = CreateSprite(image[e])
					imgAspectRatio[e] = GetImageHeight(image[e]) / GetImageWidth(image[e])
					imgWidth[e] = document[e].sprite_size
					imgHeight[e] = imgWidth[e] * imgAspectRatio[e]
					SetSpriteSize(sprite[e],imgWidth[e],imgHeight[e])
					doonce[e] = 1
				end	
				SetSpritePosition(sprite[e],document[e].screen_x,document[e].screen_y)
				PasteSpritePosition(sprite[e],document[e].screen_x,document[e].screen_y)
				if document[e].lock_screen == 1 then
					SetCameraOverride(3)
					FreezePlayer()
				end
				PerformLogicConnections(e)
			end
		end
	end
	local legacyhide = 0
	if document[e].return_text == "" and PlayerDist > document[e].pickup_range then legacyhide = 1 end
	if g_KeyPressQ == 1 or legacyhide == 1 then
		pressed[e] = 0
		if played[e] == 1 then
			PlaySound(e,0)
			played[e] = 0
		end
		if document[e].lock_screen == 1 then
			SetCameraOverride(0)
			UnFreezePlayer()
		end
		Show(e)
		SetSpritePosition(sprite[e],500,500)
		PasteSpritePosition(sprite[e],500,500)		
		showing[e] = 0
		tEnt[e] = 0
	end
end
