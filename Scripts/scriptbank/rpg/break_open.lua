-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Break Open v15 by Necrym59 and oosayeroo
-- DESCRIPTION: Breaking open this animated object will give the player the selected item or items.
-- DESCRIPTION: [PROMPT_TEXT$="Break open"]
-- DESCRIPTION: [PROMPT_RANGE=80(0,100)]
-- DESCRIPTION: [@CONTENT=1(1=Ammo, 2=Health, 3=Named Item, 4=Nothing, 5=Random)]
-- DESCRIPTION: [NAMED_ITEM$=""]
-- DESCRIPTION: [QUANTITY=5(1,5)]
-- DESCRIPTION: [NOISE_RANGE=500(0-5000)]
-- DESCRIPTION: [@BREAK_ANIMATION=0(0=AnimSetList)]
-- DESCRIPTION: [@OPEN_TRIGGER=1(1=Off, 2=On)]
-- DESCRIPTION: [COLLECT_TEXT$="Found.."]
-- DESCRIPTION: [FADE_DELAY=10(0,100)] seconds
-- DESCRIPTION: [RANDOM_ITEMS$=""] Entity Names separated by commas
-- DESCRIPTION: [RANDOM_MIN=1(1,50)] Minimum random quantity
-- DESCRIPTION: [RANDOM_MAX=50(1,50)] Maximum random quantity
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local, 2=Screen)]
-- DESCRIPTION: <Sound0> Breaking sound
-- DESCRIPTION: <Sound1> Found item sound

local lower = string.lower
local break_open 	= {}
local prompt_text 	= {}
local prompt_range	= {}
local content		= {}
local named_item 	= {}		
local quantity		= {}
local random_items  = {}
local random_min    = {}
local random_max    = {}
local prompt_display= {}

local open_trigger	= {}
local collect_text 	= {}
local fade_delay 	= {}	
local noise_range 	= {}
local cleanuptime	= {}
local fade_level	= {}
local fadetime		= {}
local status		= {}
local doonce		= {}
local playonce		= {}
local wait			= {}
local item_entity   = {}

function break_open_properties(e, prompt_text, prompt_range, content, named_item, quantity, noise_range, break_animation, open_trigger, collect_text, fade_delay, random_items, random_min, random_max, prompt_display)
	break_open[e] = g_Entity[e]
	break_open[e].prompt_text = prompt_text
	break_open[e].prompt_range = prompt_range
	break_open[e].content = content
	break_open[e].named_item = lower(named_item)
	break_open[e].quantity = quantity
	break_open[e].noise_range = noise_range
	break_open[e].break_animation = "=" .. tostring(break_animation)
	break_open[e].open_trigger = open_trigger
	break_open[e].collect_text = collect_text
	break_open[e].fade_delay = fade_delay
	break_open[e].random_items = {}  -- Initialize random_items as a table
    local items = string.gmatch(random_items, "[^,]+")
    for item in items do
        table.insert(break_open[e].random_items, lower(item))
    end
	break_open[e].random_min = random_min
	break_open[e].random_max = random_max
	break_open[e].prompt_display = prompt_display
end

function break_open_init(e)
	break_open[e] = g_Entity[e]
	break_open[e].prompt_text = "Break open"
	break_open[e].prompt_range = 80
	break_open[e].content = 2
	break_open[e].named_item = ""
	break_open[e].quantity = 2
	break_open[e].noise_range = 500
	break_open[e].break_animation = ""
	break_open[e].open_trigger = 1
	break_open[e].collect_text = "Found.."
	break_open[e].fade_delay = 10
	break_open[e].random_items = ""
	break_open[e].random_min = 1
	break_open[e].random_max = 50
	break_open[e].prompt_display = 1
	
	status[e] = "init"
	cleanuptime[e] = 0
	fadetime[e] = break_open[e].fade_delay * 1000
	StartTimer(e)
	fade_level[e] = GetEntityBaseAlpha(e)
	SetEntityBaseAlpha(e,100)
	SetEntityTransparency(e,1)
	doonce[e] = 0
	playonce[e] = 0	
	wait[e] = math.huge
	item_entity[e] = 0
end

function break_open_main(e)
	break_open[e] = g_Entity[e]
	if status[e] == "init" then
		if break_open[e].content == 3 and break_open[e].named_item ~= "" then
			-- Iterate through entities to find the Named item and hides it for relocation later
			item_entity[e] = nil		
			for a = 1, g_EntityElementMax do
				if a ~= nil and g_Entity[a] ~= nil then
					if lower(GetEntityName(a)) == break_open[e].named_item then
						item_entity[e] = a						
						CollisionOff(item_entity[e])
						Hide(item_entity[e])
						break
					end
				end
			end
		end	
		status[e] = "sealed"
	end

	local PlayerDist = GetPlayerDistance(e)
	
	if status[e] == "sealed" then  --Sealed
		if PlayerDist < break_open[e].prompt_range then
			if break_open[e].prompt_display == 1 then PromptLocal(e,break_open[e].prompt_text) end
			if break_open[e].prompt_display == 2 then Prompt(break_open[e].prompt_text) end
		end
		if g_Entity[e]['health'] < 1 then
			CollisionOff(e)
			SetAnimationName(e,break_open[e].break_animation)
			PlayAnimation(e)
			PlaySound(e,0)
			if break_open[e].noise_range > 0 then MakeAISound(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,break_open[e].noise_range,1,e) end
			wait[e] = g_Time + 1000
			status[e] = "opened"
			if cleanuptime[e] == 0 then
				cleanuptime[e] = GetTimer(e) + fadetime[e]
			end					
		end
	end

	if status[e] == "opened" then  --Opened
		
		if break_open[e].content == 1 then -- Ammo Item
			if break_open[e].prompt_display == 1 then PromptLocal(e,break_open[e].collect_text.. " " ..break_open[e].quantity.. " Ammo") end
			if break_open[e].prompt_display == 2 then PromptDuration(break_open[e].collect_text.. " " ..break_open[e].quantity.. " Ammo",2000) end
			if doonce[e] == 0 then
				for index = 1, 10, 1 do
					WeaponID = GetPlayerWeaponID()
					GetWeaponSlot (index, WeaponID, WeaponID)
					local amqty = GetWeaponPoolAmmo(index)
					SetWeaponPoolAmmo(index,amqty + break_open[e].quantity)					
				end
				StopSound(e,0)
				PlaySound(e,1)
				doonce[e] = 1
			end
			if g_Time > wait[e] then status[e] = "cleanup" end
		end
		
		if break_open[e].content == 2 then -- Health Item
			if break_open[e].prompt_display == 1 then PromptLocal(e,break_open[e].collect_text.. " " ..break_open[e].quantity.. " Health") end
			if break_open[e].prompt_display == 2 then PromptDuration(break_open[e].collect_text.. " " ..break_open[e].quantity.. " Health",2000) end
			if doonce[e] == 0 then
				StopSound(e,0)
				PlaySound(e,1)
				-- ensures max health is never exceeded
				if g_PlayerHealth < g_gameloop_StartHealth then
					local healthAmount = g_PlayerHealth + break_open[e].quantity
					if healthAmount > g_gameloop_StartHealth then
						healthAmount = g_gameloop_StartHealth
					end
					SetPlayerHealth(healthAmount)
				end
				doonce[e] = 1
			end				
			
			if g_Time > wait[e] then status[e] = "cleanup" end
		end
		
		if break_open[e].content == 3 then -- Named Item
			if doonce[e] == 0 then
				StopSound(e, 0)
				PlaySound(e, 1)
				-- Iterate through entities to find the item
				item_entity[e] = nil		
				for a = 1, g_EntityElementMax do
					if a ~= nil and g_Entity[a] ~= nil then
						if lower(GetEntityName(a)) == break_open[e].named_item then
							item_entity[e] = a
							break
						end
					end
				end
				doonce[e] = 1
			end					
			if item_entity[e] ~= nil then				
				ResetPosition(item_entity[e], g_Entity[e].x, g_Entity[e].y + 5, g_Entity[e].z)
				CollisionOn(item_entity[e])
				Show(item_entity[e])
				if break_open[e].prompt_display == 1 then PromptLocal(e,break_open[e].collect_text.. " x" ..break_open[e].quantity.. " "..break_open[e].named_item) end
				if break_open[e].prompt_display == 2 then PromptDuration(break_open[e].collect_text.. " x" ..break_open[e].quantity.. " "..break_open[e].named_item,2000) end
			end		
			if g_Time > wait[e] then
				status[e] = "cleanup"
			end
		end
		
		if break_open[e].content == 4 then -- Nothing Items
			if break_open[e].prompt_display == 1 then PromptLocal(e,"Nothing found") end
			if break_open[e].prompt_display == 2 then PromptDuration("Nothing found",1000) end		
			StopSound(e,0)
			if g_Time > wait[e] then status[e] = "cleanup" end
		end
		
		if break_open[e].content == 5 then  -- Random Items
			if doonce[e] == 0 then
				StopSound(e, 0)
				PlaySound(e, 1)
				item_entity[e] = nil
				chosen = break_open[e].random_items[math.random(1, #break_open[e].random_items)]
				randomAmount = math.random(break_open[e].random_min, break_open[e].random_max)
				-- Iterate through entities to find the Random item			
				if item_entity[e] == nil then
					for a = 1, g_EntityElementMax do
						if a ~= nil and g_Entity[a] ~= nil then
							if lower(GetEntityName(a)) == chosen then
								item_entity[e] = a
								break
							end
						end
					end	
				end	
				doonce[e] = 1			
			end
			
			if GetEntityCollectable(item_entity[e]) == 1 then
				if GetEntityCollected(item_entity[e]) == 0 then
					SetEntityQuantity(item_entity[e],1)
					SetEntityCollected(item_entity[e],1)
				end
				if GetEntityCollected(item_entity[e]) == 1 then
					SetEntityQuantity(item_entity[e],GetEntityQuantity(item_entity[e])+1)
					SetEntityCollected(item_entity[e],1)
				end
			end
			if GetEntityCollectable(item_entity[e]) == 2 then
				if GetEntityCollected(item_entity[e]) == 0 then
					SetEntityQuantity(item_entity[e],randomAmount)
					SetEntityCollected(item_entity[e],1)
				end	
			end
			CollisionOff(item_entity[e])
			Hide(item_entity[e])
			if GetEntityCollectable(item_entity[e]) == 1 then 
				if break_open[e].prompt_display == 1 then PromptLocal(e,break_open[e].collect_text.. " x 1 " ..chosen) end
				if break_open[e].prompt_display == 2 then PromptDuration(break_open[e].collect_text.. " x 1 " ..chosen,2000) end	
			end
			if GetEntityCollectable(item_entity[e]) == 2 then 
				if break_open[e].prompt_display == 1 then PromptLocal(e,break_open[e].collect_text.. " x " ..randomAmount.. " "..chosen) end
				if break_open[e].prompt_display == 2 then PromptDuration(break_open[e].collect_text.. " x " ..randomAmount.. " "..chosen,2000) end	
			end			
			if g_Time > wait[e] then
				status[e] = "cleanup"
			end
		end
		
		if break_open[e].open_trigger == 2 then
			PerformLogicConnections(e)
			ActivateIfUsed(e)
		end
	end

	if status[e] == "cleanup" then --Cleanup			
		if GetTimer(e) >= cleanuptime[e] then
			if fade_level[e] > 0 then
				SetEntityBaseAlpha(e,fade_level[e])
				fade_level[e] = fade_level[e]-1
			end
			if fade_level[e] <= 0 then
				fade_level[e] = 0
				CollisionOff(e)
				Destroy(e)
				status[e] = "end"
				SwitchScript(e,"no_behavior_selected.lua")
			end
		end
	end
end
