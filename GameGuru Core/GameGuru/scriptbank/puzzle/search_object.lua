-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Search Object v5 by Necrym59
-- DESCRIPTION: Searching this object will give the player the selected contents? 
-- DESCRIPTION: [PROMPT_TEXT$="E to Search"]
-- DESCRIPTION: [@CONTENT=1(1=Ammo, 2=Health, 3=Named Item, 4=Nothing,)]
-- DESCRIPTION: [NAMED_ITEM$=""]
-- DESCRIPTION: [QUANTITY=5(1,50)]
-- DESCRIPTION: [SEARCH_TIME=8(1,30)]
-- DESCRIPTION: [SEARCH_TEXT$="Searching..."]
-- DESCRIPTION: [RESULT_TEXT$="Found.."]
-- DESCRIPTION: [NOISE_RANGE=500(0-5000)]
-- DESCRIPTION: [@SEARCH_TRIGGER=1(1=Off, 2=On)]
-- DESCRIPTION: <Sound0> Searching sound
-- DESCRIPTION: <Sound1> Found item sound


	g_tasktool = {}					-- for compatability with Task Tool scripts if used with Named Item
	local searchobject 	= {}
	local prompt_text 	= {}
	local content		= {}		-- will be modified in future (for rpg spawning and inventory integration)
	local named_item 	= {}		-- will be modified in future to find loaded named object (for rpg spawning and inventory integration)
	local quantity		= {}
	local search_time 	= {}
	local search_text 	= {}
	local result_text 	= {}
	local search_trigger= {}
	local noise_range	= {}	
	
	local searchbar		= {}
	local stime 		= {}
	local status		= {}
	local wait			= {}
	local doonce		= {}
	local playonce		= {}

function search_object_properties(e, prompt_text, content, named_item, quantity, search_time, search_text, result_text, noise_range, search_trigger)
	searchobject[e] = g_Entity[e]
	searchobject[e].prompt_text = prompt_text
	searchobject[e].content = content
	searchobject[e].named_item = named_item
	searchobject[e].quantity = quantity
	searchobject[e].search_time = search_time
	searchobject[e].search_text = search_text
	searchobject[e].result_text = result_text
	searchobject[e].search_trigger = search_trigger
	searchobject[e].noise_range = noise_range
end

function search_object_init(e)
	searchobject[e] = g_Entity[e]
	searchobject[e].prompt_text = "E to Search"
	searchobject[e].content = 2
	searchobject[e].named_item = ""
	searchobject[e].quantity = 8
	searchobject[e].search_time = 8
	searchobject[e].search_text = ""
	searchobject[e].result_text = ""
	searchobject[e].noise_range = 500
	searchobject[e].search_trigger = 1	
	searchbar = CreateSprite(LoadImage("imagebank\\misc\\testimages\\search-bar.png"))
	SetSpriteSize(searchbar,5,-1)
	SetSpriteColor(searchbar,255,50,0,255)
	SetSpritePosition(searchbar,200,200)	
	status[e] = "init"
	wait[e] = math.huge
	doonce[e] = 0
	playonce[e] = 0
end 

function search_object_main(e)
	searchobject[e] = g_Entity[e]
	local PlayerDist = GetPlayerDistance(e)
	
	if status[e] == "init" then
		stime[e] = searchobject[e].search_time * 5
		status[e] = "sealed"
	end
	if PlayerDist < 80 then
		local LookingAt = GetPlrLookingAtEx(e,1)
		if LookingAt == 1 then
			if status[e] == "sealed" then  --Sealed		
				PromptLocalForVR(e,searchobject[e].prompt_text)			
				if g_KeyPressE == 1 then					
					if stime[e] > 0 then
						if playonce[e] == 0 then
							PlaySound(e,0)
							playonce[e] = 1
						end
						Prompt(searchobject[e].search_text)
						PasteSpritePosition(searchbar,50-(stime[e]/16),95)
						SetSpriteSize(searchbar,stime[e]/8,1)
						stime[e] = stime[e]-0.1
						if stime[e] < 0 then stime[e] = 0 end
					end
					if searchobject[e].noise_range > 0 then MakeAISound(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,searchobject[e].noise_range,1,e) end
					if stime[e] == 0 then					
						SetAnimationName(e,"open")
						PlayAnimation(e)						
						ActivateIfUsed(e)
						wait[e] = g_Time + 1000
						status[e] = "opened"
					end
				end
				if g_KeyPressE == 0 then StopSound(e,0) end	
			end
		end
		-----------------------------------------------------------------------
		if status[e] == "opened" then  --Opened
			if searchobject[e].content == 1 then	--Ammo
				PromptDuration(searchobject[e].result_text.. " " ..searchobject[e].quantity.. " Ammo",1000)					
				if doonce[e] == 0 then					
					for index = 1, 10, 1 do
						WeaponID = GetPlayerWeaponID()
						SetWeaponSlot (index, WeaponID, WeaponID)
						local amqty = GetWeaponAmmo ( index )
						SetWeaponAmmo(index,amqty + searchobject[e].quantity)
					end
					StopSound(e,0)
					PlaySound(e,1)
					doonce[e] = 1
				end
				if g_Time > wait[e] then status[e] = "searched" end
			end
			if searchobject[e].content == 2 then	--Health				
				PromptDuration(searchobject[e].result_text.. " " ..searchobject[e].quantity.. " Health",1000)
				if doonce[e] == 0 then
					StopSound(e,0)
					PlaySound(e,1)
					doonce[e] = 1
				end
				if g_PlayerHealth < g_gameloop_StartHealth then	SetPlayerHealth(g_PlayerHealth + searchobject[e].quantity) end				
				if g_Time > wait[e] then status[e] = "searched" end
			else
				StopSound(e,0)
			end				
			if searchobject[e].content == 3 then	--Named Item
				PromptDuration(searchobject[e].result_text.. " " ..searchobject[e].named_item,1000)
				g_tasktool = searchobject[e].named_item	
				if doonce[e] == 0 then
					StopSound(e,0)
					PlaySound(e,1)
					doonce[e] = 1
				end
				if g_Time > wait[e] then status[e] = "searched" end
			end
			if searchobject[e].content == 4 then
				PromptDuration("Nothing found",1000)
				StopSound(e,0)
				if g_Time > wait[e] then status[e] = "searched" end
			end				
		end
		if status[e] == "searched" then  --Finished		
			if searchobject[e].search_trigger == 2 then
				SetActivatedWithMP(e,201)
				PerformLogicConnections(e)
				status[e] = "finish"
			end
		end
	end
end