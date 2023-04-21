-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Break Open v3 by Necrym59
-- DESCRIPTION: Breaking open this object will give the player the selected item.
-- DESCRIPTION: [PROMPT_TEXT$="Break open"]
-- DESCRIPTION: [@CONTENT=1(1=Ammo, 2=Health, 3=Named Item, 4=Nothing)]
-- DESCRIPTION: [NAMED_ITEM$=""]
-- DESCRIPTION: [QUANTITY=5(1,5)]
-- DESCRIPTION: [NOISE_RANGE=500(0-5000)]
-- DESCRIPTION: [@OPEN_TRIGGER=1(1=Off, 2=On)]
-- DESCRIPTION: [COLLECT_TEXT$="Found.."]
-- DESCRIPTION: <Sound0> Breaking sound
-- DESCRIPTION: <Sound1> Found item sound

	g_tasktool = {}					-- for compatability with Task Tool scripts if used with Named Item
	local break_open 	= {}
	local prompt_text 	= {}
	local content		= {}		-- will be modified in future (for rpg spawning and inventory integration)
	local named_item 	= {}		-- will be modified in future to find loaded named object (for rpg spawning and inventory integration)
	local quantity		= {}
	local open_trigger	= {}
	local collect_text 	= {}
	local noise_range	= {}
	local cleanuptime	= {}
	local fade_level	= {}
	local status		= {}
	local doonce		= {}
	local playonce		= {}
	local wait			= {}

function break_open_properties(e, prompt_text, content, named_item, quantity, noise_range, open_trigger, collect_text)
	break_open[e] = g_Entity[e]
	break_open[e].prompt_text = prompt_text
	break_open[e].content = content
	break_open[e].named_item = named_item
	break_open[e].quantity = quantity
	break_open[e].noise_range = noise_range
	break_open[e].open_trigger = open_trigger
	break_open[e].collect_text = collect_text	
end

function break_open_init(e)
	break_open[e] = g_Entity[e]
	break_open[e].prompt_text = "Break open"
	break_open[e].content = 2
	break_open[e].named_item = ""
	break_open[e].quantity = 2
	break_open[e].noise_range = 500
	break_open[e].open_trigger = 1
	break_open[e].collect_text = "Found.."
	status[e] = "init"
	cleanuptime[e] = 0
	StartTimer(e)
	fade_level[e] = GetEntityBaseAlpha(e)
	SetEntityBaseAlpha(e,100)
	SetEntityTransparency(e,1)
	doonce[e] = 0
	playonce[e] = 0	
	wait[e] = math.huge
end

function break_open_main(e)
	break_open[e] = g_Entity[e]
	if status[e] == "init" then				
		status[e] = "sealed"
	end
	local PlayerDist = GetPlayerDistance(e)
	
	if PlayerDist < 80 and g_PlayerHealth > 0 then
		local LookingAt = GetPlrLookingAtEx(e,1)
		if LookingAt == 1 then

			if status[e] == "sealed" then  --Sealed
				PromptLocalForVR(e,break_open[e].prompt_text)
				if g_Entity[e]['health'] <= 1 then
					CollisionOff(e)
					SetAnimationName(e,"break")
					PlayAnimation(e)
					PlaySound(e,0)
					if break_open[e].noise_range > 0 then MakeAISound(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,break_open[e].noise_range,1,e) end
					ActivateIfUsed(e)
					wait[e] = g_Time + 1000
					status[e] = "opened"
					if cleanuptime[e] == 0 then
						cleanuptime[e] = GetTimer(e) + 5000
					end					
				end
			end
		end
	end

	if status[e] == "opened" then  --Opened
		if break_open[e].content == 1 then
			if doonce[e] == 0 then
				PromptDuration(break_open[e].collect_text.. " " ..break_open[e].quantity.. " Ammo",1000)
				for index = 1, 10, 1 do
					WeaponID = GetPlayerWeaponID()
					SetWeaponSlot (index, WeaponID, WeaponID)
					local amqty = GetWeaponAmmo ( index )
					SetWeaponAmmo(index,amqty + break_open[e].quantity)
				end
				StopSound(e,0)
				PlaySound(e,1)
				doonce[e] = 1
			end
			if g_Time > wait[e] then status[e] = "cleanup" end
		end
		if break_open[e].content == 2 then
			PromptDuration(break_open[e].collect_text.. " " ..break_open[e].quantity.. " Health",1000)
			if doonce[e] == 0 then
				StopSound(e,0)
				PlaySound(e,1)
				doonce[e] = 1
			end
			if g_PlayerHealth < g_gameloop_StartHealth then	SetPlayerHealth(g_PlayerHealth + break_open[e].quantity) end	
			if g_Time > wait[e] then status[e] = "cleanup" end
		end
		if break_open[e].content == 3 then
			PromptDuration(break_open[e].collect_text.. " " ..break_open[e].named_item,1000)
			g_tasktool = break_open[e].named_item
			if doonce[e] == 0 then
				StopSound(e,0)
				PlaySound(e,1)
				doonce[e] = 1
			end
			if g_Time > wait[e] then status[e] = "cleanup" end
		end
		if break_open[e].content == 4 then
			PromptDuration("Nothing found",1000)
			StopSound(e,0)
			if g_Time > wait[e] then status[e] = "cleanup" end
		end		
	end
	
	if status[e] == "cleanup" then --Cleanup
		if break_open[e].open_trigger == 2 then
			SetActivatedWithMP(e,201)
			PerformLogicConnections(e)			
		end		
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
			end
		end
	end
end
