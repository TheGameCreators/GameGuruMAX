-- Quest Poster v12
-- DESCRIPTION: When player is within [RANGE=100] distance, show [QUEST_PROMPT$="Press E to view this quest"] and when E is pressed, player will be shown the [QUEST_SCREEN$="HUD Screen 8"].
-- DESCRIPTION: [@QuestChoice=1(0=QuestList)]
-- DESCRIPTION: <Sound0> when viewing the quest.
-- DESCRIPTION: <Sound1> when quest completed.

local lower = string.lower
local U = require "scriptbank\\utillib"

local g_quest_poster 	= {}
local quest_objno		= {}
local quest_recno		= {}
local quest_quantity	= {}
local check_quantity	= {}
local check_timer		= {}
local play_once			= {}
local tEnt 				= {}
local selectobj 		= {}

function quest_poster_init(e)
	g_quest_poster[e] = {}
	quest_poster_properties(e,100,"Press E to view this quest","HUD Screen 8","")
	quest_objno[e] = 0
	quest_recno[e] = 0
	quest_quantity[e] = 0
	check_quantity[e] = 0
	check_timer[e] = 0
	play_once[e] = 0
end

function quest_poster_properties(e, range, questprompt, questscreen, questchoice)
	g_quest_poster[e]['range'] = range
	g_quest_poster[e]['questprompt'] = questprompt
	g_quest_poster[e]['questscreen'] = questscreen
	g_quest_poster[e]['questchoice'] = questchoice
	g_quest_poster[e]['questtitle'] = ""
	g_quest_poster[e]['questtype'] = ""
	g_quest_poster[e]['questobject'] = ""
	g_quest_poster[e]['questreceiver'] = ""
	g_quest_poster[e]['questlevel'] = ""
	g_quest_poster[e]['questpoints'] = ""
	g_quest_poster[e]['questvalue'] = ""
	g_quest_poster[e]['questactivate'] = ""
	g_quest_poster[e]['queststarted'] = 0
	g_quest_poster[e]['questquantity'] = ""
	g_quest_poster[e]['questendtext'] = "Quest Completed"
end

function quest_poster_main(e)
	if g_quest_poster[e]['questtitle'] == "" then
		local totalquests = GetCollectionQuestQuantity()
		if totalquests ~= nil then
			local i = tonumber(g_quest_poster[e]['questchoice'])-1
			if i ~= nil then
				if i > 0 and i <= totalquests then
					g_quest_poster[e]['questtitle'] = GetCollectionQuestAttribute(i,"title")
					g_quest_poster[e]['questtype'] = GetCollectionQuestAttribute(i,"type")
					g_quest_poster[e]['questobject'] = GetCollectionQuestAttribute(i,"object")
					g_quest_poster[e]['questreceiver'] = GetCollectionQuestAttribute(i,"receiver")
					g_quest_poster[e]['questlevel'] = GetCollectionQuestAttribute(i,"level")
					g_quest_poster[e]['questpoints'] = GetCollectionQuestAttribute(i,"points")
					g_quest_poster[e]['questvalue'] = GetCollectionQuestAttribute(i,"value")
					g_quest_poster[e]['questactivate'] = GetCollectionQuestAttribute(i,"activate")
					g_quest_poster[e]['questquantity'] = tonumber(GetCollectionQuestAttribute(i,"quantity"))
				end
			else
				PromptDuration(g_quest_poster[e]['questchoice'],5000)
			end
		end
	end
	if g_UserGlobalQuestTitleActive == nil then g_UserGlobalQuestTitleActive = "" end
	if g_UserGlobalQuestTitleActive ~= nil then
		if g_quest_poster[e]['queststarted'] == 0 then
			Show(e)
			for tquestindex = 1, hud0_quest_qty, 1 do
				if GetCollectionQuestAttribute(tquestindex,"title") == g_quest_poster[e]['questtitle'] then
					if hud0_quest_status[tquestindex] == "active" then
						g_quest_poster[e]['queststarted'] = 1
					end
					break
				end
			end
			local PlayerDist = GetPlayerDistance(e)
			if PlayerDist < g_quest_poster[e]['range'] then 
				--pinpoint select object--
				local px, py, pz = GetCameraPositionX(0), GetCameraPositionY(0), GetCameraPositionZ(0)
				local rayX, rayY, rayZ = 0,0,g_quest_poster[e]['range']
				local paX, paY, paZ = math.rad(GetCameraAngleX(0)), math.rad(GetCameraAngleY(0)), math.rad(GetCameraAngleZ(0))
				rayX, rayY, rayZ = U.Rotate3D(rayX, rayY, rayZ, paX, paY, paZ)
				selectobj[e]=IntersectAll(px,py,pz, px+rayX, py+rayY, pz+rayZ,e)
				if selectobj[e] ~= 0 or selectobj[e] ~= nil then
					if g_Entity[e]['obj'] == selectobj[e] then
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
		
			if PlayerDist < g_quest_poster[e]['range'] and tEnt[e] ~= 0 and GetEntityVisibility(e) == 1 then
				PromptDuration(g_quest_poster[e]['questprompt'] ,1000)
				if g_KeyPressE == 1 then
					-- set game to this quest
					g_UserGlobalQuestTitleShowing = g_quest_poster[e]['questtitle']
					g_UserGlobalQuestTitleShowingObject = g_quest_poster[e]['questobject']
					g_UserGlobalQuestTitleShowingObject2 = g_quest_poster[e]['questreceiver']
					ScreenToggle(g_quest_poster[e]['questscreen'])
					if play_once[e] == 0 then
						PlaySound(e,0)
						play_once[e] = 1
					end
				end
			end
		else
			Hide(e)
			for tquestindex = 1, hud0_quest_qty, 1 do
				if GetCollectionQuestAttribute(tquestindex,"title") == g_quest_poster[e]['questtitle'] then
					if hud0_quest_status[tquestindex] == "inactive" then
						g_quest_poster[e]['queststarted'] = 0
					end
					break
				end
			end
		end
		if g_quest_poster[e]['questtitle'] == g_UserGlobalQuestTitleActive then
			-- hide in game, we have accepted this one and doing it now
			if g_UserGlobalQuestTitleActiveE > 0 then
				local tquestcomplete = 0
				
				if g_quest_poster[e]['questtype'] == "collect" then
					-- COLLECT
					if quest_objno[e] == 0 then						
						for a = 1, g_EntityElementMax do
							if a ~= nil and g_Entity[a] ~= nil then
								if lower(GetEntityName(a)) == lower(g_quest_poster[e]['questobject']) then
									quest_objno[e] = a
									break
								end
							end
						end
					end
					----Complete Collect Single Item Quest ----------------------
					if GetEntityCollected(quest_objno[e]) == 1 and g_quest_poster[e]['questquantity'] == 1 then					
						tquestcomplete = 1							
					end
					---- Complete Collect Multi Items Quest ----------------------
					if g_quest_poster[e]['questquantity'] > 1 then
						if g_Time > check_timer[e] then
							check_timer[e] = check_timer[e] + 500  -- check once per 1/2second			
							quest_quantity[e] = 0
							for ee = 1, g_EntityElementMax, 1 do
								if GetEntityCollected(ee) == 1 then
									if lower(GetEntityName(ee)) == lower(g_quest_poster[e]['questobject']) then
										tqty = GetEntityQuantity(ee)
										if GetEntityCollectable(entityindex) == 2 then
											-- resources spent can be zero
											if tqty < 0 then tqty = 0 end
										else
											-- non resource objects can be minus one
											if tqty < 1 then tqty = 1 end
										end											
										quest_quantity[e] = quest_quantity[e] + tqty
									end
								end
							end
						end
						tcurrentcount = quest_quantity[e]
						if tcurrentcount >= g_quest_poster[e]['questquantity'] then
							tcurrentcount = g_quest_poster[e]['questquantity']
							tquestcomplete = 1
						end					
						if quest_quantity[e] > check_quantity[e] then PromptDuration(tcurrentcount.. " of " ..g_quest_poster[e]['questquantity'].. " " ..g_quest_poster[e]['questobject'].. "'s collected",2000) end
						check_quantity[e] = quest_quantity[e]
					end
				end
				
				if g_quest_poster[e]['questtype'] == "destroy" then
					-- DESTROY
					if quest_objno[e] == 0 then
						for a = 1, g_EntityElementMax do
							if a ~= nil and g_Entity[a] ~= nil then
								if lower(GetEntityName(a)) == lower(g_quest_poster[e]['questobject']) then
								quest_objno[e] = a
								break
								end
							end
						end
					end
					if g_Entity[quest_objno[e]]['health'] <= 0 then
						tquestcomplete = 1
					end
				end
				
				if g_quest_poster[e]['questtype'] == "deliver" then
					-- DELIVER
					if quest_objno[e] == 0 then
						for a = 1, g_EntityElementMax do
							if a ~= nil and g_Entity[a] ~= nil then
								if lower(GetEntityName(a)) == lower(g_quest_poster[e]['questobject']) then
								quest_objno[e] = a								
								break
								end
							end
						end
					end
					if quest_recno[e] == 0 then
						for b = 1, g_EntityElementMax do
							if b ~= nil and g_Entity[b] ~= nil then
								if lower(GetEntityName(b)) == lower(g_quest_poster[e]['questreceiver']) then
								quest_recno[e] = b
								break
								end
							end
						end
					end					
					if GetEntityCollected(quest_objno[e]) == 1 then
						Hide(quest_objno[e])
						GravityOff(quest_objno[e])
						CollisionOff(quest_objno[e])
						ResetPosition(quest_objno[e],g_PlayerPosX,g_PlayerPosY+100,g_PlayerPosZ)
						if GetDistance(quest_objno[e],quest_recno[e]) <= 200 then
							PromptDuration("Press Q to complete quest",1000)
							if g_KeyPressQ == 1 then
								local ox,oy,oz = U.Rotate3D(0,0,30,math.rad(g_PlayerAngX),math.rad(g_PlayerAngY),math.rad(g_PlayerAngZ))
								local forwardposx, forwardposy, forwardposz = g_PlayerPosX + ox, g_PlayerPosY + oy, g_PlayerPosZ + oz								
								GravityOn(quest_objno[e])
								ResetPosition(quest_objno[e],forwardposx, forwardposy, forwardposz)
								CollisionOn(quest_objno[e])
								Show(quest_objno[e])								
								tquestcomplete = 1
								SetEntityCollected(quest_objno[e],0)
							end
						end
					end
				end			

				if tquestcomplete == 1 then
					SetSoundVolume(100)
					PlayNon3DSound(e,1)
					PromptDuration(g_quest_poster[e]['questendtext'],2000)
					-- award value
					local myMoney = 0
					if _G["g_UserGlobal['".."MyMoney".."']"] ~= nil then myMoney = _G["g_UserGlobal['".."MyMoney".."']"] end
					_G["g_UserGlobal['".."MyMoney".."']"] = myMoney + tonumber(g_quest_poster[e]['questvalue'])
					-- award XP
					local myXP = 0
					if _G["g_UserGlobal['".."MyXP".."']"] ~= nil then myXP = _G["g_UserGlobal['".."MyXP".."']"] end
					_G["g_UserGlobal['".."MyXP".."']"] = myXP + tonumber(g_quest_poster[e]['questpoints'])
					-- activate another object
					for ee = 1, g_EntityElementMax do
						if ee ~= nil and g_Entity[ee] ~= nil then
							if GetEntityName(ee) ~= nil then
								if string.lower(GetEntityName(ee)) == string.lower(g_quest_poster[e]['questactivate']) then
									SetActivated(ee,1)
									PerformLogicConnections(ee)
									break
								end
							end
						end
					end
					-- to complete a quest means to destroy it
					PerformLogicConnections(e)
					--Destroy(e)
					-- and update quest status
					if g_UserGlobalQuestTitleActive ~= nil then
						for tquestindex = 1, hud0_quest_qty, 1 do
							if GetCollectionQuestAttribute(tquestindex,"title") == g_UserGlobalQuestTitleActive then
								hud0_quest_status[tquestindex] = "complete"
								break
							end
						end
					end
					-- and reset quest
					g_UserGlobalQuestTitleActive = ""
					g_UserGlobalQuestTitleActiveObject = ""
					g_UserGlobalQuestTitleActiveE = 0					
					quest_objno[e] = 0
					quest_recno[e] = 0
				end				
			end
		end		
	end
	if GetCurrentScreen() == -1 then play_once[e] = 0 end
end

function GetDistance(e,v)
	if g_Entity[e] ~= nil and g_Entity[e] ~= 0 and g_Entity[v] ~= nil and g_Entity[v] ~= 0 then
		local disx = g_Entity[e]['x'] - g_Entity[v]['x']
		local disz = g_Entity[e]['z'] - g_Entity[v]['z']
		local disy = g_Entity[e]['y'] - g_Entity[v]['y']
		return math.sqrt(disx^2 + disz^2 + disy^2)
	end
end

