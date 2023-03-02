-- Document v2 - thanks to Necrym59 and Lee
-- DESCRIPTION: Change the prompt text [PROMPTTEXT$="E to look at document"]. 
-- DESCRIPTION: Change the position [SCREEN_X=25] and [SCREEN_Y=10]
-- DESCRIPTION: The image [SPRITE_SIZE=50(1,100)] percentage.
-- DESCRIPTION: The document image [IMAGEFILE$="imagebank/documents/default_doc.png"]

g_document = {}	
local document = {}
local prompt_text ={}	
local screen_x = {} --where the top left of the sprite will be
local screen_y = {} --where the top left of the sprite will be
local sprite_size = {} --% of the screen the sprite will take up
local imagefile = {}
local image = {}
local showing = {}
local sprite = {}	
local pressed = {}
local status ={}
		
function document_properties(e, prompt_text, screen_x, screen_y, sprite_size, imagefile)
	g_document[e] = g_Entity[e]
	g_document[e]['prompt_text'] = prompt_text		
	g_document[e]['screen_x'] = screen_x				
	g_document[e]['screen_y'] = screen_y				
	g_document[e]['sprite_size'] = sprite_size			
	g_document[e]['imagefile'] = imagefile				
end 
	
function document_init_name(e)
	g_document[e] = g_Entity[e]	
	g_document[e]['prompt_text'] = ""
	g_document[e]['screen_x'] = 0	
	g_document[e]['screen_y'] = 0
	g_document[e]['sprite_size'] = 0
	g_document[e]['imagefile'] = "imagebank\\documents\\default_doc.png"	
	showing[e] = 0
	sprite[e] = 0
	pressed[e] = 0	
	status[e] ="init"
end

function document_main(e)
	g_document[e] = g_Entity[e]		
	if status[e] == "init" then	
		status[e] = "endinit"
	end	
	if PlayerLooking(e,80,120) == 1 then	
		if showing[e] == 0 then PromptLocal(e,g_document[e]['prompt_text']) end
		if showing[e] == 1 then	PromptLocal(e,"E to return document") end
		if g_KeyPressE == 1 and pressed[e] == 0 then
			pressed[e] = 1
			PlaySound(e,0)
			if showing[e] == 0 then	
				Hide(e)
				showing[e] = 1 
				image[e] = LoadImage(g_document[e]['imagefile'])
				sprite[e] = CreateSprite(image[e])
				local imgAspectRatio = GetImageHeight(image[e]) / GetImageWidth(image[e])
				local imgWidth = g_document[e]['sprite_size']
				local imgHeight = imgWidth * imgAspectRatio
				SetSpriteSize(sprite[e],imgWidth,imgHeight)
				SetSpritePosition(sprite[e],g_document[e]['screen_x'],g_document[e]['screen_y'])
				PerformLogicConnections(e)
			else				
				showing[e] = 0
				Show(e)
				SetSpritePosition(sprite[e],1000,1000)
			end
		end		
	else
		if showing[e] == 1 then
			Show(e)			
			SetSpritePosition(sprite[e],1000,1000)
			showing[e] = 0
		end
	end
	if GetScancode() == 0 then
		pressed[e] = 0
	end	
end

function PlayerLooking(e,dis,v)
	if g_Entity[e] ~= nil then
		if dis == nil then
			dis = 3000
		end
		if v == nil then
			v = 0.5
		end
		if GetPlayerDistance(e) <= dis then
			local destx = g_Entity[e]['x'] - g_PlayerPosX
			local destz = g_Entity[e]['z'] - g_PlayerPosZ
			local angle = math.atan2(destx,destz)
			angle = angle * (180.0 / math.pi)
			if angle <= 0 then
				angle = 360 + angle
			elseif angle > 360 then
				angle = angle - 360
			end
			while g_PlayerAngY < 0 or g_PlayerAngY > 360 do
				if g_PlayerAngY <= 0 then
					g_PlayerAngY = 360 + g_PlayerAngY
				elseif g_PlayerAngY > 360 then
					g_PlayerAngY = g_PlayerAngY - 360
				end
			end
			local L = angle - v
			local R = angle + v
			if L <= 0 then
				L = 360 + L 
			elseif L > 360 then
				L = L - 360
			end
			if R <= 0 then
				R = 360 + R
			elseif R > 360 then
				R = R - 360
			end
			if (L < R and math.abs(g_PlayerAngY) > L and math.abs(g_PlayerAngY) < R) then
				return 1
			elseif (L > R and (math.abs(g_PlayerAngY) > L or math.abs(g_PlayerAngY) < R)) then
				return 1
			else
				return 0
			end
		else
			return 0
		end
	end
end