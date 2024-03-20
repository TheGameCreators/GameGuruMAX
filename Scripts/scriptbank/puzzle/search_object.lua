-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Search Object v11 by Necrym59
-- DESCRIPTION: Searching this object will give the player the selected contents? 
-- DESCRIPTION: [PROMPT_TEXT$="E to Search"]
-- DESCRIPTION: [@CONTENT=1(1=Ammo, 2=Health, 3=Named Item, 4=Nothing)]
-- DESCRIPTION: [NAMED_ITEM$=""] Entity Name
-- DESCRIPTION: [QUANTITY=1(1,50)]
-- DESCRIPTION: [SEARCH_TIME=8(1,30)]
-- DESCRIPTION: [SEARCH_TEXT$="Searching..."]
-- DESCRIPTION: [RESULT_TEXT$="Found.."]
-- DESCRIPTION: [NOISE_RANGE=500(1,5000)]
-- DESCRIPTION: [@SEARCH_TRIGGER=1(1=Off, 2=On)]
-- DESCRIPTION: [USE_RANGE=90(0,100)]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local, 2=Screen)]
-- DESCRIPTION: [IMAGEFILE$="imagebank\\misc\\testimages\\search-bar.png"]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline)]
-- DESCRIPTION: <Sound0> Searching sound
-- DESCRIPTION: <Sound1> Found item sound

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local lower = string.lower
local searchobject 		= {}
local prompt_text 		= {}
local content			= {}
local named_item 		= {}
local quantity			= {}
local search_time 		= {}
local search_text 		= {}
local result_text 		= {}
local noise_range		= {}
local search_trigger	= {}
local use_range			= {}
local prompt_display	= {}
local searchbar_image	= {}
local item_highlight 	= {}

local searchbar		= {}
local stime 		= {}
local item_entity	= {}
local status		= {}
local wait			= {}
local doonce		= {}
local playonce		= {}
local tEnt 			= {}
local selectobj 	= {}

function search_object_properties(e, prompt_text, content, named_item, quantity, search_time, search_text, result_text, noise_range, search_trigger, use_range, prompt_display, searchbar_image, item_highlight)
	searchobject[e].prompt_text = prompt_text
	searchobject[e].content = content
	searchobject[e].named_item = lower(named_item)
	searchobject[e].quantity = quantity
	searchobject[e].search_time = search_time
	searchobject[e].search_text = search_text
	searchobject[e].result_text = result_text
	searchobject[e].noise_range = noise_range
	searchobject[e].search_trigger = search_trigger
	searchobject[e].use_range = use_range
	searchobject[e].prompt_display = prompt_display
	searchobject[e].searchbar_image = searchbar_image or imagefile
	searchobject[e].item_highlight = item_highlight	
end

function search_object_init(e)
	searchobject[e] = {}
	searchobject[e].prompt_text = "E to Search"
	searchobject[e].content = 2
	searchobject[e].named_item = ""
	searchobject[e].quantity = 8
	searchobject[e].search_time = 8
	searchobject[e].search_text = ""
	searchobject[e].result_text = ""
	searchobject[e].noise_range = 500
	searchobject[e].search_trigger = 1
	searchobject[e].use_range = 90
	searchobject[e].prompt_display = 1
	searchobject[e].searchbar_image = "imagebank\\misc\\testimages\\search-bar.png"
	searchobject[e].item_highlight = 0	
	
	searchbar[e] = CreateSprite(LoadImage(searchobject[e].searchbar_image))
	SetSpriteSize(searchbar[e],5,-1)
	SetSpriteColor(searchbar[e],255,50,0,255)
	SetSpritePosition(searchbar[e],200,200)	
	status[e] = "init"
	wait[e] = math.huge
	doonce[e] = 0
	playonce[e] = 0
	tEnt[e] = 0
	g_tEnt = 0 
	selectobj[e] = 0
end 

function search_object_main(e)

	local PlayerDist = GetPlayerDistance(e)
	
	if status[e] == "init" then
		if searchobject[e].content == 3 and searchobject[e].named_item ~= "" then
			item_entity[e] = nil
			for a = 1, g_EntityElementMax do
				if a ~= nil and g_Entity[a] ~= nil then
					if lower(GetEntityName(a)) == searchobject[e].named_item then
						item_entity[e] = a						
						CollisionOff(a)
						Hide(a)
						break
					end
				end
			end
		end
		stime[e] = searchobject[e].search_time * 5
		status[e] = "sealed"
	end
	if PlayerDist < searchobject[e].use_range and status[e] == "sealed" then
		--pinpoint select object--
		module_misclib.pinpoint(e,searchobject[e].use_range,searchobject[e].item_highlight)
		tEnt[e] = g_tEnt
		--end pinpoint select object--	
		if PlayerDist < searchobject[e].use_range and tEnt[e] ~= 0 and GetEntityVisibility(e) == 1 then
			if status[e] == "sealed" then  --Sealed	
				if searchobject[e].prompt_display == 1 then PromptLocal(e,searchobject[e].prompt_text) end
				if searchobject[e].prompt_display == 2 then Prompt(searchobject[e].prompt_text) end			
				if g_KeyPressE == 1 then					
					if stime[e] > 0 then
						if playonce[e] == 0 then
							PlaySound(e,0)
							playonce[e] = 1
						end
						if searchobject[e].prompt_display == 1 then PromptLocal(e,searchobject[e].search_text) end
						if searchobject[e].prompt_display == 2 then Prompt(searchobject[e].search_text) end	
						PasteSpritePosition(searchbar[e],50-(stime[e]/16),95)
						SetSpriteSize(searchbar[e],stime[e]/8,1)
						stime[e] = stime[e]-0.1
						if stime[e] < 0 then stime[e] = 0 end
					end					
					if stime[e] == 0 then
						if searchobject[e].noise_range > 0 then MakeAISound(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,searchobject[e].noise_range,1,-1) end
						SetAnimationName(e,"open")
						PlayAnimation(e)						
						wait[e] = g_Time + 1000
						status[e] = "opened"
					end
				end
				if g_KeyPressE == 0 then StopSound(e,0) end	
			end
		end
		----------------------------------------------------------------------------------------------------------------------------------------------------
		if status[e] == "opened" then  --Opened
			if searchobject[e].content == 1 then	--Ammo
				if searchobject[e].prompt_display == 1 then PromptLocal(e,searchobject[e].result_text.. " " ..searchobject[e].quantity..  " Ammo") end
				if searchobject[e].prompt_display == 2 then PromptDuration(searchobject[e].result_text.. " " ..searchobject[e].quantity..  " Ammo",2000) end	
				if doonce[e] == 0 then					
					for index = 1, 10, 1 do
						WeaponID = GetPlayerWeaponID()
						GetWeaponSlot (index, WeaponID, WeaponID)
						local amqty = GetWeaponPoolAmmo(index)
						SetWeaponPoolAmmo(index,amqty + searchobject[e].quantity)		
					end
					StopSound(e,0)
					PlaySound(e,1)
					doonce[e] = 1
				end
				if g_Time > wait[e] then status[e] = "searched" end
			end
			
			if searchobject[e].content == 2 then	--Health
				if searchobject[e].prompt_display == 1 then PromptLocal(e,searchobject[e].result_text.. " " ..searchobject[e].quantity..  " Health") end
				if searchobject[e].prompt_display == 2 then PromptDuration(searchobject[e].result_text.. " " ..searchobject[e].quantity..  " Health",2000) end			
				if doonce[e] == 0 then
					StopSound(e,0)
					PlaySound(e,1)
					if g_PlayerHealth < g_gameloop_StartHealth then
						local healthAmount = g_PlayerHealth + searchobject[e].quantity
						if healthAmount > g_gameloop_StartHealth then
							healthAmount = g_gameloop_StartHealth
						end
						SetPlayerHealth(healthAmount)
					end
					doonce[e] = 1
				end				
				if g_Time > wait[e] then status[e] = "searched" end
			else
				StopSound(e,0)
			end	

			if searchobject[e].content == 3 then -- Named Item
				if doonce[e] == 0 then
					StopSound(e, 0)
					PlaySound(e, 1)
					if item_entity[e] ~= nil then						
						CollisionOn(item_entity[e])
						Show(item_entity[e])
						if searchobject[e].prompt_display == 1 then PromptLocal(e,searchobject[e].result_text.. " " ..searchobject[e].quantity.. " "..searchobject[e].named_item) end
						if searchobject[e].prompt_display == 2 then PromptDuration(searchobject[e].result_text.. " " ..searchobject[e].quantity.. " "..searchobject[e].named_item,2000) end
					end
					doonce[e] = 1
				end	
			end
			
			if searchobject[e].content == 4 then	--Nothing
				if searchobject[e].prompt_display == 1 then PromptLocal(e,"Nothing found") end
				if searchobject[e].prompt_display == 2 then PromptDuration("Nothing found",2000) end				
				StopSound(e,0)
				if g_Time > wait[e] then status[e] = "searched" end
			end				
		end
		
		if status[e] == "searched" then  --Finished		
			if searchobject[e].search_trigger == 2 then	
				PerformLogicConnections(e)
				ActivateIfUsed(e)				
				status[e] = "finish"
			end
		end
	end
end