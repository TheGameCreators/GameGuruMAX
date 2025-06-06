-- DESCRIPTION: A global script that controls the in-game HUD. Do not assign to an object.
-- Hud0 - Version 3 
cursorControl = require "scriptbank\\huds\\cursorcontrol"
local U = require "scriptbank\\utillib"

g_liveHudScreen = 0
g_CraftingClass = ""

g_sprCursorPtrX = 50
g_sprCursorPtrY = 33
g_sprCursorPtrClick = 0

hud0 = {}
hud0_pointerID = -1
hud0_pointerSpriteID = nil
hud0_pointerSpriteImageID = 0
hud0_gridSpriteID = nil
hud0_gridSelected = 0
hud0_gridSelectedIndex = -1

hud0_pulledoutofslot = 0
hud0_pulledoutofslotfromX = 0
hud0_pulledoutofslotfromY = 0

hud0_freezeforpointer = 0

hud0_updatePlayerLimits = 0

hud0_playercontainer_screenID = 0
hud0_playercontainer_collectionindex = {}
hud0_playercontainer_img = {}
hud0_playercontainer_e = {}

hud0_lastgoodplayerinventory0qty = {}
hud0_lastgoodplayerinventory1qty = {}

hud0_mapView_LevelImage = -1
hud0_mapView_WindowX = 0
hud0_mapView_WindowY = 0
hud0_mapView_WindowW = 0
hud0_mapView_WindowH = 0
hud0_mapView_ImageX = 0
hud0_mapView_ImageY = 0
hud0_mapView_ImageW = 0
hud0_mapView_ImageH = 0
hud0_mapView_ImageOW = 0
hud0_mapView_ImageOH = 0
hud0_mapView_ScrollX = 0
hud0_mapView_ScrollY = 0
hud0_mapView_Scale = 1.0

hud0_populateallcontainers = 1
hud0_populateallcontainersfillshop = ""
hud0_populateallcontainersfilled = {}

hud0_scrollbar_forscreen = 0
hud0_scrollbar_mode = 0
hud0_scrollbar_boxx = 0
hud0_scrollbar_boxy = 0
hud0_scrollbar_boxw = 0
hud0_scrollbar_boxh = 0
hud0_scrollbar_percentage = 0
hud0_itemindexmaxslotused = 0
hud0_itemindexscrolloffset = 0
hud0_scrollpanel_mainrow = 0
hud0_scrollpanel_maincolumn = 0

-- reserved 29000-29999
hud0_sounds_levelup = 29001
hud0_sounds_silence = 29999

hud0_quest_qty = 0
hud0_quest_status = {}

hud0_buttonPressed = 0

function hud0.init()
 -- initialise all globals
 InitScreen("HUD0")
 -- create resources for HUD
 hud0_gridSpriteID = CreateSprite ( LoadImage("imagebank\\HUD\\blank.png") )
 -- load sound resources
 LoadGlobalSound("audiobank\\misc\\levelup.wav",hud0_sounds_levelup)
 LoadGlobalSound("audiobank\\misc\\silence.wav",hud0_sounds_silence)
 hud0_gridSpriteID = CreateSprite ( LoadImage("imagebank\\HUD\\blank.png") )
 -- init in-game HUD base container for hotkeys panel if any
 hud0_playercontainer_screenID = 0
 if hud0_playercontainer_collectionindex[hud0_playercontainer_screenID] == nil then 
	hud0_playercontainer_collectionindex[hud0_playercontainer_screenID] = {}
	hud0_playercontainer_img[hud0_playercontainer_screenID] = {}
	hud0_playercontainer_e[hud0_playercontainer_screenID] = {}
 end
 if hud0_playercontainer_collectionindex[hud0_playercontainer_screenID][1] == nil then 
	hud0_playercontainer_collectionindex[hud0_playercontainer_screenID][1] = {}
	hud0_playercontainer_img[hud0_playercontainer_screenID][1] = {}
	hud0_playercontainer_e[hud0_playercontainer_screenID][1] = {}
 end
 -- default container is a shop container
 g_UserGlobalContainer = ""
 g_UserGlobalContainerLast = ""
 g_UserGlobalContainerRefresh = 0
 g_UserGlobalContainerFilled = ""
 -- default settings
 hud0_scrollbar_mode = -1
 hud0_scrollbar_boxw = 0
 -- init quest states
 hud0_quest_qty = GetCollectionQuestQuantity()
 for tquestindex = 1, hud0_quest_qty, 1 do
	hud0_quest_status[tquestindex] = GetCollectionQuestAttribute(tquestindex,"status")
 end
 -- signal we need to load our map image in
 hud0_mapView_LevelImage = -1
 -- trigger a refresh of player stats to ensure correct maximums
 hud0_updatePlayerLimits = 1
end

function hud0.refreshHUD()
	-- populate local HUD containers with collection list
	local interestingtodebug = ""
	local tgridqty = GetScreenElementsType("user defined global panel")
	for gridi = 1, tgridqty, 1 do
		local thegridelementID = GetScreenElementTypeID("user defined global panel",gridi)
		if thegridelementID > 0 then	
			-- init this container
			if hud0_playercontainer_collectionindex[hud0_playercontainer_screenID] == nil then 
				hud0_playercontainer_collectionindex[hud0_playercontainer_screenID] = {}
				hud0_playercontainer_img[hud0_playercontainer_screenID] = {}
				hud0_playercontainer_e[hud0_playercontainer_screenID] = {}
			end
			if hud0_playercontainer_collectionindex[hud0_playercontainer_screenID][gridi] == nil then 
				hud0_playercontainer_collectionindex[hud0_playercontainer_screenID][gridi] = {}
				hud0_playercontainer_img[hud0_playercontainer_screenID][gridi] = {}
				hud0_playercontainer_e[hud0_playercontainer_screenID][gridi] = {}
			end
			-- provide by panel settings (row, column)
			totalinrow, totalincolumn = GetScreenElementDetails(thegridelementID)
			if totalinrow < 1 then totalinrow = 1 end
			if totalincolumn < 1 then totalincolumn = 1 end
			-- fill container with collection/inventory
			local whattoshowinpanel = 0
			local panelname = GetScreenElementName(thegridelementID)
			if panelname == "collection" then
				-- show ALL collection
				local itemindex = 0
				local tcollectionmax = GetCollectionItemQuantity()
				for tcollectionindex = 1, tcollectionmax, 1 do
					local titemimg = GetCollectionItemAttribute(tcollectionindex,"image")
					if string.len(titemimg) > 0 then
						hud0_playercontainer_collectionindex[hud0_playercontainer_screenID][gridi][itemindex] = tcollectionindex
						hud0_playercontainer_img[hud0_playercontainer_screenID][gridi][itemindex] = LoadImage(titemimg)
						hud0_playercontainer_e[hud0_playercontainer_screenID][gridi][itemindex] = 0
						itemindex=itemindex+1
					end
				end
			else
				-- refresh containers
				local playercontainer = 0
				local inventorycontainer = panelname
				local thisitemindexoffset = 0
				local itemcount = totalinrow*totalincolumn
				if panelname == "inventory:player" then 
					playercontainer = 1 
					thisitemindexoffset = hud0_itemindexscrolloffset
					if hud0_itemindexmaxslotused > itemcount then itemcount = hud0_itemindexmaxslotused end
				end
				if panelname == "inventory:hotkeys" then playercontainer = 2 end
				if panelname == "inventory:container" then playercontainer = 3 inventorycontainer = "inventory:"..g_UserGlobalContainer end
				if playercontainer > 0 then
					if playercontainer == 3 and g_UserGlobalContainer ~= g_UserGlobalContainerLast then
						-- if this did not exist at start of level, create it now
						local tcontainerfullname = "inventory:"
						tcontainerfullname = tcontainerfullname .. g_UserGlobalContainer
						MakeInventoryContainer(tcontainerfullname)
						-- must clear container if previously used for something else
						g_UserGlobalContainerLast = g_UserGlobalContainer
						for itemindex = 0, itemcount, 1 do
							hud0_playercontainer_collectionindex[hud0_playercontainer_screenID][gridi][itemindex] = nil
							hud0_playercontainer_img[hud0_playercontainer_screenID][gridi][itemindex] = nil								
							hud0_playercontainer_e[hud0_playercontainer_screenID][gridi][itemindex] = 0								
						end
					end
					local tinventoryqty = GetInventoryQuantity(inventorycontainer)
					for tinventoryindex = 1, tinventoryqty, 1 do
						local itemindex = -1
						local tcollectionindex = GetInventoryItem(inventorycontainer,tinventoryindex)
						local tcollectione = GetInventoryItemID(inventorycontainer,tinventoryindex)
						local tslotindex = GetInventoryItemSlot(inventorycontainer,tinventoryindex)
						local titemimg = GetCollectionItemAttribute(tcollectionindex,"image")
						if string.len(titemimg) > 0 then
							for findifexist = 0, itemcount, 1 do
								if findifexist == tslotindex and hud0_playercontainer_collectionindex[hud0_playercontainer_screenID][gridi][findifexist] == tcollectionindex and hud0_playercontainer_e[hud0_playercontainer_screenID][gridi][findifexist] == tcollectione then
									-- already know about this one, and in right slot, and correct E
									itemindex = findifexist
									break
								end
							end
							if itemindex == -1 then
								itemindex = tslotindex
								--if hud0_playercontainer_collectionindex[hud0_playercontainer_screenID][gridi][itemindex] == nil then could replace some old item that was the same type but new E
								-- add this new arrival
								hud0_playercontainer_collectionindex[hud0_playercontainer_screenID][gridi][itemindex] = tcollectionindex
								hud0_playercontainer_img[hud0_playercontainer_screenID][gridi][itemindex] = LoadImage(titemimg)
								local tentityindex = GetInventoryItemID(inventorycontainer,tinventoryindex)
								hud0_playercontainer_e[hud0_playercontainer_screenID][gridi][itemindex] = tentityindex
								--end
							end
						end
					end
					-- remove any items if no longer in inventory
					for itemindex = 0, itemcount, 1 do
						local thiscontainercollectionindex = hud0_playercontainer_collectionindex[hud0_playercontainer_screenID][gridi][itemindex]
						local thiscontainercollectione = hud0_playercontainer_e[hud0_playercontainer_screenID][gridi][itemindex]
						if thiscontainercollectionindex ~= nil then
							local foundininventoryandrightslot = 0
							for tinventoryindex = 1, tinventoryqty, 1 do
								local tcollectionindex = GetInventoryItem(inventorycontainer,tinventoryindex)
								if tcollectionindex == thiscontainercollectionindex then
									local tslotindex = GetInventoryItemSlot(inventorycontainer,tinventoryindex)
									if tslotindex == itemindex then
										local tcollectione = GetInventoryItemID(inventorycontainer,tinventoryindex)
										if tcollectione == thiscontainercollectione then
											foundininventoryandrightslot = 1
											break
										end
									end
								end
							end
							if foundininventoryandrightslot == 0 then
								hud0_playercontainer_collectionindex[hud0_playercontainer_screenID][gridi][itemindex] = nil
								hud0_playercontainer_img[hud0_playercontainer_screenID][gridi][itemindex] = nil								
								hud0_playercontainer_e[hud0_playercontainer_screenID][gridi][itemindex] = 0								
							end
						end
					end
				end
			end
		end
	end
	if interestingtodebug > "" then PromptDuration("refreshHUD:"..interestingtodebug,4000) end
end

function hud0.main()

 -- trigger creation of all chests and containers each time enter new level (so can be populated)
 if g_gameloop_AnotherInit == 1 then
	hud0_populateallcontainers = 1
	g_gameloop_AnotherInit = 0
 end

 -- load in correct map image for level
 if hud0_mapView_LevelImage == -1 then
	hud0_mapView_LevelImage = 0
	local tLevelMapImageFile = g_LevelFilename
	if tLevelMapImageFile ~= nil then
		tLevelMapImageFile = string.sub(tLevelMapImageFile,1,string.len(tLevelMapImageFile)-4)
		tLevelMapImageFile = tLevelMapImageFile .. ".png"
		hud0_mapView_LevelImage = LoadImage("mapbank\\"..tLevelMapImageFile)
	end
 end

 -- This section controls which HUD screens are shown to the player whilst in-game (not paused or in a menu screen)
 if IsPlayerInGame() then

  if hud0_populateallcontainers == 0 then  
    if g_UserGlobalContainer ~= "" then
		if g_UserGlobalContainer ~= g_UserGlobalContainerFilled then
			g_UserGlobalContainerFilled = g_UserGlobalContainer
			hud0_populateallcontainersfillshop = "inventory:"
			hud0_populateallcontainersfillshop = hud0_populateallcontainersfillshop .. g_UserGlobalContainerFilled
			hud0_populateallcontainers = 2
		end
	end
  end
  if hud0_populateallcontainers == 1 then 
    -- create all shop and chest containers
	local tcollectionmax = GetCollectionItemQuantity()
	for tcollectionindex = 1, tcollectionmax, 1 do
		local tcontainernameunparsed = GetCollectionItemAttribute(tcollectionindex,"container")
		if tcontainernameunparsed ~= nil then
			i, j = string.find(tcontainernameunparsed,"=")
			if i ~= nil then
				tcontainername = string.sub(tcontainernameunparsed,1,i-1)
			else
				tcontainername = tcontainernameunparsed
			end
			local tcontainerfullname = "inventory:"
			tcontainerfullname = tcontainerfullname .. tcontainername
			MakeInventoryContainer(tcontainerfullname)
		end
		hud0_populateallcontainersfilled[tcollectionindex] = 0
	end
	hud0_populateallcontainers = 0
 end
 if hud0_populateallcontainers == 2 and hud0_populateallcontainersfillshop ~= "" then 
	-- fill shop items from collection for this level
	local tcollectionmax = GetCollectionItemQuantity()
	for tcollectionindex = 1, tcollectionmax, 1 do
		if hud0_populateallcontainersfilled[tcollectionindex] == 0 then
			local applyqty = -1
			local tcontainerfullname = "inventory:shop"
			local tcontainernameunparsed = GetCollectionItemAttribute(tcollectionindex,"container")
			if tcontainernameunparsed ~= nil then
				i, j = string.find(tcontainernameunparsed,"=")
				if i ~= nil then
					tcontainername = string.sub(tcontainernameunparsed,1,i-1)
					local qty = string.sub(tcontainernameunparsed,i+1,-1)
					applyqty = tonumber(tqty)
				else
					tcontainername = tcontainernameunparsed
				end
				tcontainerfullname = "inventory:"
				tcontainerfullname = tcontainerfullname .. tcontainername
			end
			if tcontainerfullname == hud0_populateallcontainersfillshop	then
				local tname = GetCollectionItemAttribute(tcollectionindex,"title")
				local anyee = 0
				for ee = 1, g_EntityElementMax, 1 do
					if e ~= ee then
						if g_Entity[ee] ~= nil then
							if GetEntityName(ee) == tname then
								anyee = ee
								break
							end
						end
					end
				end
				if anyee > 0 then
					-- item object is in 3D world, create a clone for the shop
					local newe = SpawnNewEntity(anyee)
					SetEntityCollected(newe,3,-1,tcontainerfullname)
					SetEntityActive(newe,0)
					if applyqty ~= -1 then
						SetEntityQuantity(newe,applyqty)
					end
				end
				hud0_populateallcontainersfilled[tcollectionindex] = 1
			end
		end
	end
	hud0_populateallcontainers = 0
  end
 
  -- Some screens can be toggled on/off with a key press, if the required key press is detected then that screen will be set to the current screen and appear in-game
  CheckScreenToggles()
  
  -- needed to separate HUD panels
  if GetCurrentScreen() > 0 then
	hud0_playercontainer_screenID = GetCurrentScreen()
  else
	hud0_playercontainer_screenID = 0
  end
  
  -- detect any inventory changes
  local tinventory0qty = GetInventoryQuantity("inventory:player")
  local tinventory1qty = GetInventoryQuantity("inventory:hotkeys")
  if tinventory0qty ~= hud0_lastgoodplayerinventory0qty[hud0_playercontainer_screenID] or tinventory1qty ~= hud0_lastgoodplayerinventory1qty[hud0_playercontainer_screenID] or g_UserGlobalContainerRefresh == 1 then
	hud0_lastgoodplayerinventory0qty[hud0_playercontainer_screenID] = tinventory0qty
	hud0_lastgoodplayerinventory1qty[hud0_playercontainer_screenID] = tinventory1qty
	hud0.refreshHUD()
	hud0_gridSelected = 0
	hud0_gridSelectedIndex = -1
	g_UserGlobalContainerRefresh = 0
  end

  -- flag mouse pointer drawing below
  local drawMousePointer = 0
  
  -- Allow live firing when in user in-game HUD
  if g_liveHudScreen == 1 then
	EnableGunFireInHUD()
  else
	DisableGunFireInHUD()
  end
 
  -- If there is an active screen (resulting from CheckScreenToggles()) then display that screen, otherwise dispay the default HUD screen
  local currentScreen = GetCurrentScreen()
  if currentScreen > -1 then
  
	-- switch to mouse mode  
	if hud0_freezeforpointer == 0 and g_liveHudScreen == 0 then 
		SetCameraOverride(3)
		FreezePlayer()
		ActivateMouse()
		hud0_freezeforpointer = 1
		hud0.refreshHUD()
	end
	
	-- display HUD screen (and any button press handling later)
	local buttonElementID = DisplayCurrentScreen()
	
	-- use mouse pointer
	g_sprCursorPtrX,g_sprCursorPtrY,g_sprCursorPtrClick = cursorControl.getinput(g_sprCursorPtrX,g_sprCursorPtrY,g_sprCursorPtrClick)
	
	-- detect ineraction with scroll bars (controls panel grid offsets)
	if hud0_scrollbar_forscreen ~= hud0_playercontainer_screenID then
		hud0_scrollbar_forscreen = hud0_playercontainer_screenID
		hud0_scrollbar_mode = -1
		hud0_scrollbar_boxw = 0
	end	
	if hud0_pulledoutofslot == 0 then
		local timgqty = GetScreenElementsType("user defined global image")
		for tindex = 1, timgqty, 1 do
			local elementID = GetScreenElementTypeID("user defined global image",tindex)
			if elementID > 0 then
				if hud0_scrollbar_boxw == 0 then
					if GetScreenElementName(elementID) == "scrollbar:box" then
						hud0_scrollbar_boxx,hud0_scrollbar_boxy,hud0_scrollbar_boxw,hud0_scrollbar_boxh = GetScreenElementArea(elementID)    
					end
				end
				if GetScreenElementName(elementID) == "scrollbar:handle" then
					tareax,tareay,tareaw,tareah = GetScreenElementArea(elementID)    
					if hud0_scrollbar_mode == -1 then
						SetScreenElementPosition(elementID,hud0_scrollbar_boxx,hud0_scrollbar_boxy)
						hud0_scrollbar_percentage = 0
						hud0_scrollbar_mode = 0
					end
					if hud0_scrollbar_mode == 0 then		
						if hud0_itemindexmaxslotused - (hud0_scrollpanel_mainrow*hud0_scrollpanel_maincolumn) >= 0 then
							SetScreenElementVisibility(elementID,1)
						else
							SetScreenElementVisibility(elementID,0)
						end
						if g_sprCursorPtrClick == 1 then
							if g_sprCursorPtrX >= tareax and g_sprCursorPtrX <= tareax+tareaw then
								if g_sprCursorPtrY >= tareay and g_sprCursorPtrY <= tareay+tareah then
									hud0_scrollbar_mode = 1
								end
							end
						end
					else
						local toffsety = g_sprCursorPtrY-hud0_scrollbar_boxy
						if toffsety < 0 then toffsety = 0 end
						if toffsety > hud0_scrollbar_boxh-tareah then toffsety = hud0_scrollbar_boxh-tareah end
						SetScreenElementPosition(elementID,hud0_scrollbar_boxx,hud0_scrollbar_boxy+toffsety)
						hud0_scrollbar_percentage = toffsety / (hud0_scrollbar_boxh-tareah)
						local gridrowstoscroll = (hud0_scrollpanel_mainrow+(hud0_itemindexmaxslotused - (hud0_scrollpanel_mainrow*hud0_scrollpanel_maincolumn)))*hud0_scrollbar_percentage
						local gridrowslimit = math.floor(gridrowstoscroll/hud0_scrollpanel_mainrow)
						if gridrowslimit < 0 then gridrowslimit = 0 end
						hud0_itemindexscrolloffset = gridrowslimit * hud0_scrollpanel_mainrow
						if g_sprCursorPtrClick == 0 then
							hud0_scrollbar_mode = 0
						end
					end
				end
			end
		end
	end
					
	-- detect when select and let go of an element for each grid
	local tgridqty = GetScreenElementsType("user defined global panel")
	for gridi = 1, tgridqty, 1 do
		local thegrid = GetScreenElementTypeID("user defined global panel",gridi)
		if thegrid > 0 then
		
			local tgridx = 0
			local tgridy = 0
			local tgridwidth = 0
			local tgridheight = 0
			tgridx,tgridy,tgridwidth,tgridheight = GetScreenElementArea(thegrid)    

			-- provide by panel settings (row, column)Storyboard.widget_textoffset[nodeID][widgetSlot]
			totalinrow, totalincolumn = GetScreenElementDetails(thegrid)
			if totalinrow < 1 then totalinrow = 1 end
			if totalincolumn < 1 then totalincolumn = 1 end
			local gridtilewidth = tgridwidth/totalinrow
			local gridtileheight = tgridheight/totalincolumn
			
			-- get main inventory for scroll activity
			local thisitemindexoffset = 0
			if GetScreenElementName(thegrid) == "inventory:player" then
				hud0_scrollpanel_mainrow = totalinrow
				hud0_scrollpanel_maincolumn = totalincolumn
				thisitemindexoffset = hud0_itemindexscrolloffset
				hud0_itemindexmaxslotused = 0
				local tinventoryqty = GetInventoryQuantity("inventory:player")
				for tinventoryindex = 1, tinventoryqty, 1 do
					local tcollectionslot = GetInventoryItemSlot("inventory:player",tinventoryindex)
					if tcollectionslot > hud0_itemindexmaxslotused then
						hud0_itemindexmaxslotused = tcollectionslot
					end
				end
			end
		
			-- if in panel, control contents
			if g_sprCursorPtrX >= tgridx and g_sprCursorPtrX <= tgridx+tgridwidth then
				if g_sprCursorPtrY >= tgridy and g_sprCursorPtrY <= tgridy+tgridheight and hud0_scrollbar_mode == 0 then
				
					-- where in grid
					local itemindex = 0
					local gridslotx = g_sprCursorPtrX - tgridx
					local gridsloty = g_sprCursorPtrY - tgridy
					gridslotx=math.floor(gridslotx/gridtilewidth)
					gridsloty=math.floor(gridsloty/gridtileheight)
					itemindex=thisitemindexoffset+(gridslotx+(gridsloty*totalinrow))

					-- handle click/drag/release
					if hud0_pulledoutofslot == 0 and gridi > 0 then
						-- click into this grid for first time
						if g_sprCursorPtrClick == 1 then
							hud0_gridSelectedIndex = itemindex
							hud0_gridSelected = gridi
							local panelname = GetScreenElementName(thegrid)
							if panelname == "inventory:container" then
								if string.sub(g_UserGlobalContainer,1,5) == "chest" then
									-- chests are free for all
									hud0_pulledoutofslot = 1
								else	
									-- owned by other
								end
							else
								-- player inventories are okay
								hud0_pulledoutofslot = 1
							end
						end
					else
						if g_sprCursorPtrClick == 1 then
							if hud0_pulledoutofslot == 1 then
								hud0_pulledoutofslotfromX = g_sprCursorPtrX
								hud0_pulledoutofslotfromY = g_sprCursorPtrY
								hud0_pulledoutofslotfromW = gridtilewidth
								hud0_pulledoutofslotfromH = gridtileheight
								hud0_pulledoutofslot = 2
							end
							local whenoutofslot = 1
							if hud0_pulledoutofslot == 2 and (math.abs(g_sprCursorPtrX-hud0_pulledoutofslotfromX)>whenoutofslot or math.abs(g_sprCursorPtrY-hud0_pulledoutofslotfromY)>whenoutofslot) then
								-- can start dragging once pulled out of slot sufficiently
								hud0_pulledoutofslot = 3
							end
						else
							-- release dragged item into this grid if different in any way
							if hud0_pulledoutofslot == 3 then
								if hud0_gridSelected ~= gridi or hud0_gridSelectedIndex ~= itemindex then
									local placedatitemindex = -1
									if hud0_playercontainer_collectionindex[hud0_playercontainer_screenID][gridi][itemindex] == nil then
										placedatitemindex = itemindex
									else
										local itemcount = totalincolumn*totalinrow
										local newlocationitemindex = itemindex
										for gothroughall = 1, itemcount-1, 1 do
											newlocationitemindex = newlocationitemindex + 1
											if newlocationitemindex >= itemcount then newlocationitemindex = 0 end
											if hud0_playercontainer_collectionindex[hud0_playercontainer_screenID][gridi][newlocationitemindex] == nil then
												placedatitemindex = newlocationitemindex
												break
											end
										end
									end
									if placedatitemindex ~= -1 then
									
										-- from and to
										local thegridFrom = GetScreenElementTypeID("user defined global panel",hud0_gridSelected)
										local thegridTo = GetScreenElementTypeID("user defined global panel",gridi)
										local panelnameFrom = GetScreenElementName(thegridFrom)
										local panelnameTo = GetScreenElementName(thegridTo)
										
										-- if entity removed from collection, need entity index
										local cancelmove = 0
										local entityindex = 0
										
										-- cancel if dragging into a container not owned by player
										if panelnameTo == "inventory:container" then
											if string.sub(g_UserGlobalContainer,1,5) ~= "chest" then
												-- owned by other - cannot move item here
												cancelmove = 1
											end
										end		
										
										-- cancel if moving item into recipe that is not a recipe
										if panelnameTo == "inventory:craft" then
											local findcollectionindex = hud0_playercontainer_collectionindex[hud0_playercontainer_screenID][hud0_gridSelected][hud0_gridSelectedIndex]
											if findcollectionindex ~= -1 then												
												local tstyleclass = GetCollectionItemAttribute(findcollectionindex,"style")
												local tscindex = 6
												local tstyle = string.sub(tstyleclass, 1, tscindex)
												local tclass = string.sub(tstyleclass, tscindex+2, -1)
												if tstyle ~= "recipe" then -- fail on non recipe
													cancelmove = 1
												end
												if tstyle == "recipe" and g_CraftingClass ~= "" then
													if tclass ~= "" and tclass ~= g_CraftingClass then -- fail on non matching class if any
														cancelmove = 1
													end  
												end
											end											
										end															
										
										-- cancel if dragging into hotkeys and slot is blocked (Slot Not Used)
										if panelnameTo == "inventory:hotkeys" then
											local slotattempt = 1+placedatitemindex
											local slothaspreferenceforgunid = GetWeaponSlotPref(slotattempt)
											if slothaspreferenceforgunid > 0 then
												local tcollectionentity = hud0_playercontainer_e[hud0_playercontainer_screenID][hud0_gridSelected][hud0_gridSelectedIndex]
												if tcollectionentity > 0 then
													local weaponidofdragged = GetEntityWeaponID(tcollectionentity)
													if weaponidofdragged > 0 then
														if slothaspreferenceforgunid ~= weaponidofdragged then
															cancelmove = 1
														end
													end
												end
											end
										end	
										
										-- shuffled inside hotkey, handle weapons
										if cancelmove == 0 and (panelnameFrom == "inventory:hotkeys" or panelnameTo == "inventory:hotkeys") then
											local panelname = panelnameFrom
											local finditeme = hud0_playercontainer_e[hud0_playercontainer_screenID][hud0_gridSelected][hud0_gridSelectedIndex]
											if finditeme ~= -1 then
												local tinventoryqty = GetInventoryQuantity(panelname)
												for tinventoryindex = 1, tinventoryqty, 1 do
													local tcollectionentity = GetInventoryItemID(panelname,tinventoryindex)
													if tcollectionentity == finditeme then 
														if tcollectionentity == 0 then
															-- this item is FIXED and cannot be moved (i.e. start weapon)
															cancelmove = 1
														else
															if panelnameFrom == "inventory:hotkeys" then
																-- remove from hot key location
																if hud0_gridSelectedIndex >= 0 and hud0_gridSelectedIndex <= 9 then
																	RemovePlayerWeapon(1+hud0_gridSelectedIndex)
																	entityindex = tcollectionentity
																	if entityindex > 0 then
																		SetEntityCollected(entityindex,0,0)
																	end
																end
															else
																-- removing to place in hot key location
																if panelnameTo == "inventory:hotkeys" then
																	entityindex = tcollectionentity
																	if entityindex > 0 then
																		SetEntityCollected(entityindex,0,0)
																	end
																end
															end
														end
														break
													end
												end
											end
										end
										if cancelmove == 0 then
											if entityindex == 0 then
												-- moved item to new inventory container
												local collectionindex = hud0_playercontainer_collectionindex[hud0_playercontainer_screenID][hud0_gridSelected][hud0_gridSelectedIndex]
												local entityindex = hud0_playercontainer_e[hud0_playercontainer_screenID][hud0_gridSelected][hud0_gridSelectedIndex]
												local inventorycontainerFrom = panelnameFrom
												local inventorycontainerTo = panelnameTo
												if inventorycontainerFrom == "inventory:container" then inventorycontainerFrom = "inventory:"..g_UserGlobalContainer end
												if inventorycontainerTo == "inventory:container" then inventorycontainerTo = "inventory:"..g_UserGlobalContainer end
												MoveInventoryItem(inventorycontainerFrom,inventorycontainerTo,-1,entityindex,placedatitemindex)
												g_UserGlobalContainerRefresh = 1
											else
												-- add to new location as we removed it above
												local suggestedslotvalid = -1
												if panelnameTo == "inventory:hotkeys" then
													suggestedslotvalid = SetEntityCollected(entityindex,2,placedatitemindex)
													if GetEntityCollectable(entityindex) == 2 then
														-- resources can never be weapons
													else
														AddPlayerWeaponSuggestSlot(entityindex,1+placedatitemindex)
													end
												end
												if suggestedslotvalid == -1 then
													SetEntityCollected(entityindex,1,placedatitemindex)
													g_UserGlobalContainerRefresh = 1
												end
											end
											-- finish movement																		
											hud0_playercontainer_collectionindex[hud0_playercontainer_screenID][gridi][placedatitemindex] = hud0_playercontainer_collectionindex[hud0_playercontainer_screenID][hud0_gridSelected][hud0_gridSelectedIndex]
											hud0_playercontainer_img[hud0_playercontainer_screenID][gridi][placedatitemindex] = hud0_playercontainer_img[hud0_playercontainer_screenID][hud0_gridSelected][hud0_gridSelectedIndex]
											hud0_playercontainer_e[hud0_playercontainer_screenID][gridi][placedatitemindex] = hud0_playercontainer_e[hud0_playercontainer_screenID][hud0_gridSelected][hud0_gridSelectedIndex]
											hud0_playercontainer_collectionindex[hud0_playercontainer_screenID][hud0_gridSelected][hud0_gridSelectedIndex] = nil
											hud0_playercontainer_img[hud0_playercontainer_screenID][hud0_gridSelected][hud0_gridSelectedIndex] = nil
											hud0_playercontainer_e[hud0_playercontainer_screenID][hud0_gridSelected][hud0_gridSelectedIndex] = 0
											hud0_gridSelected = gridi
											hud0_gridSelectedIndex = placedatitemindex
										end
									end
								end
								hud0_pulledoutofslot = 0
							end
						end
					end			  	
				end
			end
			
			-- paste all items into the panel from collection list
			if hud0_gridSpriteID ~= nil then
				for yy = 0, totalincolumn-1, 1 do
					for xx = 0, totalinrow-1, 1 do
						local itemindex = thisitemindexoffset+(xx+(yy*totalinrow))
						if hud0_playercontainer_collectionindex[hud0_playercontainer_screenID] ~= nil then
							if hud0_playercontainer_collectionindex[hud0_playercontainer_screenID][gridi] ~= nil then
								if hud0_playercontainer_collectionindex[hud0_playercontainer_screenID][gridi][itemindex] ~= nil then
									-- except if dragging it
									if gridi ~= hud0_gridSelected or itemindex ~= hud0_gridSelectedIndex or g_sprCursorPtrClick == 0 or hud0_pulledoutofslot ~= 3 then
										local scrx = tgridx+(xx*gridtilewidth)
										local scry = tgridy+(yy*gridtileheight)
										local scrimg = hud0_playercontainer_img[hud0_playercontainer_screenID][gridi][itemindex]
										SetSpritePosition(hud0_gridSpriteID,scrx,scry)
										SetSpriteImage(hud0_gridSpriteID,scrimg)
										SetSpriteSize(hud0_gridSpriteID,gridtilewidth,gridtileheight)
										if hud0_gridSelectedIndex == itemindex and hud0_gridSelected == gridi then
											SetSpriteColor(hud0_gridSpriteID,255,255,255,255)
										else
											SetSpriteColor(hud0_gridSpriteID,255,255,255,192)
										end
										SetSpritePriority(hud0_gridSpriteID,-1)
										PasteSprite(hud0_gridSpriteID)
									end
								end
							end
						end
					end
				end
			end
		
			-- draw the dragged item in transit
			if hud0_gridSelected > 0 and hud0_pulledoutofslot == 3 then
				local scrx = g_sprCursorPtrX
				local scry = g_sprCursorPtrY
				local scrimg = hud0_playercontainer_img[hud0_playercontainer_screenID][hud0_gridSelected][hud0_gridSelectedIndex]
				if scrimg ~= nil then
					SetSpritePosition(hud0_gridSpriteID,scrx,scry)
					SetSpriteImage(hud0_gridSpriteID,scrimg)
					SetSpriteSize(hud0_gridSpriteID,hud0_pulledoutofslotfromW,hud0_pulledoutofslotfromH)---1)--gridtilewidth,-1)
					SetSpriteColor(hud0_gridSpriteID,255,255,255,255)
					SetSpritePriority(hud0_gridSpriteID,-1)
					PasteSprite(hud0_gridSpriteID)
				end
			end

		end
	end
	if g_sprCursorPtrClick == 0 then	 
		hud0_pulledoutofslot = 0
	end
	
	-- draw any user defined global overlays (hud0_gridSelectedIndex)
	local displayingselecteditem = 0
	if hud0_gridSelectedIndex >= 0 and hud0_gridSelected > 0 then
		if hud0_playercontainer_img[hud0_playercontainer_screenID] ~= nil then
			if hud0_playercontainer_img[hud0_playercontainer_screenID][hud0_gridSelected] ~= nil then
				if hud0_playercontainer_img[hud0_playercontainer_screenID][hud0_gridSelected][hud0_gridSelectedIndex] ~= nil then
					if hud0_playercontainer_img[hud0_playercontainer_screenID][hud0_gridSelected][hud0_gridSelectedIndex] ~= nil then
						local iqty = GetScreenElements("selected:*")
						for ti = 1, iqty, 1 do
							local elementID = GetScreenElementID("selected:*",ti)
							local elementName = GetScreenElementName(elementID)
							if elementName == "selected:image" then
								local elementImageID = elementID
								if elementImageID > 0 then
									telementx,telementy,telementwidth,telementheight = GetScreenElementArea(elementImageID)    
									SetSpritePosition(hud0_gridSpriteID,telementx,telementy)
									SetSpriteImage(hud0_gridSpriteID,hud0_playercontainer_img[hud0_playercontainer_screenID][hud0_gridSelected][hud0_gridSelectedIndex])
									SetSpriteColor(hud0_gridSpriteID,255,255,255,255)
									SetSpriteSize(hud0_gridSpriteID,telementwidth,telementheight)---1)
									SetSpritePriority(hud0_gridSpriteID,-1)
									PasteSprite(hud0_gridSpriteID)
									displayingselecteditem = 1
								end
							else
								local tcollectionattribqty = GetCollectionAttributeQuantity()
								for tattribindex = 1, tcollectionattribqty, 1 do
									local attributelabel = GetCollectionAttributeLabel(tattribindex)
									if elementName == "selected:"..attributelabel then
										local elementTextID = elementID
										if elementTextID > 0 then
											local tcollectionindex = hud0_playercontainer_collectionindex[hud0_playercontainer_screenID][hud0_gridSelected][hud0_gridSelectedIndex]
											local tattrubutedata = GetCollectionItemAttribute(tcollectionindex,attributelabel)
											local tentityindex = hud0_playercontainer_e[hud0_playercontainer_screenID][hud0_gridSelected][hud0_gridSelectedIndex]
											if tentityindex > 0 then
												if GetEntityCollectable(tentityindex) == 2 and attributelabel == "title" then
													local tQty = GetEntityQuantity(tentityindex)
													if tQty > 1 then
														tattrubutedata = tattrubutedata .. " (" .. tQty .. ")"
													end
												end
											end
											SetScreenElementText(elementTextID,tattrubutedata)
											SetScreenElementVisibility(elementTextID,1)
											displayingselecteditem = 1
										end
									end
								end
							end
						end
					end
				end			
			end			
		end
	end
	if displayingselecteditem == 0 then
		local iqty = GetScreenElements("selected:*")
		for ti = 1, iqty, 1 do
			local elementID = GetScreenElementID("selected:*",ti)
			local elementName = GetScreenElementName(elementID)
			if elementName ~= "selected:image" then
				local tcollectionattribqty = GetCollectionAttributeQuantity()
				for tattribindex = 1, tcollectionattribqty, 1 do
					local attributelabel = GetCollectionAttributeLabel(tattribindex)
					if elementName == "selected:"..attributelabel then
						local elementTextID = elementID
						if elementTextID > 0 then
							SetScreenElementText(elementTextID,"")
						end
					end
				end
			end			
		end			
	end
	
	-- handle any craft text	
	local iqty = GetScreenElements("craft:*")
	for ti = 1, iqty, 1 do
		local elementID = GetScreenElementID("craft:*",ti)
		local elementName = GetScreenElementName(elementID)
		local tcollectionattribqty = GetCollectionAttributeQuantity()
		for tattribindex = 1, tcollectionattribqty, 1 do
			local attributelabel = GetCollectionAttributeLabel(tattribindex)
			if elementName == "craft:"..attributelabel then
				local tcontainerfullname = "inventory:craft"
				MakeInventoryContainer(tcontainerfullname)
				local tinventoryindex = GetInventoryQuantity(tcontainerfullname)
				if tinventoryindex == 1 then
					local tcollectionindex = GetInventoryItem(tcontainerfullname,tinventoryindex)
					local tattrubutedata = GetCollectionItemAttribute(tcollectionindex,attributelabel)
					SetScreenElementText(elementID,tattrubutedata)
				else
					SetScreenElementText(elementID,"")
				end
			end
		end
	end
	
	-- useful to know current player level
	local currentplayerlevel = 1 if _G["g_UserGlobal['".."MyPlayerLevel".."']"] ~= nil then currentplayerlevel = _G["g_UserGlobal['".."MyPlayerLevel".."']"] end
	if currentplayerlevel < 1 then currentplayerlevel = 1 end
	
	-- handle custom quest reward text (add 'quest:show:reward' and hide it, then add 'MyRewardText' and specify your reward text)
	local customrrewardtext = nil
	local iqty = GetScreenElements("quest:*")
	for ti = 1, iqty, 1 do
		local elementID = GetScreenElementID("quest:*",ti)
		local elementName = GetScreenElementName(elementID)
		if elementID > 0 then
			if string.sub(elementName,1,11) == "quest:show:" then
				if elementName == "quest:show:reward" then
					customrrewardtext = _G["g_UserGlobal['".."MyRewardText".."']"]
					if customrrewardtext == nil then
						customrrewardtext = ""
					end
				end
			end
		end	
	end			
		
	-- handle any quest text
	local iqty = GetScreenElements("quest:*")
	for ti = 1, iqty, 1 do
		local elementID = GetScreenElementID("quest:*",ti)
		local elementName = GetScreenElementName(elementID)
		if elementID > 0 then
			for tlisti = 1, 5, 1 do
				if elementName == "quest:list:"..tlisti then
					local ttextvalue = ""
					local tactivelistcount = 0
					for tquestindex = 1, hud0_quest_qty, 1 do
						if hud0_quest_status[tquestindex] == "active" then
							tactivelistcount=tactivelistcount+1
							if tactivelistcount == tlisti then
								ttextvalue = GetCollectionQuestAttribute(tquestindex,"title")
								if ttextvalue == g_UserGlobalQuestTitleActive then
									ttextvalue = "["..g_UserGlobalQuestTitleActive.."]"
									SetScreenElementColor(elementID, 1, 1, 0, 1 )
								else
									SetScreenElementColor(elementID, 1, 1, 1, 1 )
								end
							end
						end
					end
					if ttextvalue == "" then
						SetScreenElementVisibility(elementID,0)
					else
						SetScreenElementVisibility(elementID,1)
					end
					SetScreenElementText(elementID,ttextvalue)
				end
			end
			if string.sub(elementName,1,11) == "quest:show:" then
				ttextvalue = ""
				if g_UserGlobalQuestTitleShowing ~= nil then
					if elementName == "quest:show:title" then
						ttextvalue = g_UserGlobalQuestTitleShowing
					else
						for tquestindex = 1, hud0_quest_qty, 1 do
							if GetCollectionQuestAttribute(tquestindex,"title") == g_UserGlobalQuestTitleShowing then
								local optionalrewardseperation = 0
								if elementName == "quest:show:completed" then
									if hud0_quest_status[tquestindex] == "complete" then
										ttextvalue = "Quest Completed"
									else
										if customrrewardtext == nil then
											local tlevelrequired = tonumber(GetCollectionQuestAttribute(tquestindex,"level"))
											if currentplayerlevel >= tlevelrequired then
												ttextvalue = "Quest worth "..GetCollectionQuestAttribute(tquestindex,"points").." XP points and "..GetCollectionQuestAttribute(tquestindex,"value").." money"
											else
												ttextvalue = "You need to be level "..tlevelrequired.." to start this quest"
											end
										end
									end
								else
									if elementName == "quest:show:reward" then
										-- custom reward prompt
										local tlevelrequired = tonumber(GetCollectionQuestAttribute(tquestindex,"level"))
										if currentplayerlevel >= tlevelrequired then
											ttextvalue = "Quest worth "..GetCollectionQuestAttribute(tquestindex,"points").." "..customrrewardtext
										else
											ttextvalue = "You need to be level "..tlevelrequired.." to start this quest"
										end
									else
										if elementName == "quest:show:desc1" then ttextvalue = GetCollectionQuestAttribute(tquestindex,"desc1") end
										if elementName == "quest:show:desc2" then ttextvalue = GetCollectionQuestAttribute(tquestindex,"desc2") end
										if elementName == "quest:show:desc3" then ttextvalue = GetCollectionQuestAttribute(tquestindex,"desc3") end
									end
								end
								break
							end
						end
					end
				end
				if ttextvalue == "none" then ttextvalue = "" end
				SetScreenElementText(elementID,ttextvalue)
			end
		end	
	end			
	
	-- handle any logic ascribed to global text elements
	local telementqty = GetScreenElementsType("user defined global text")
	for elementi = 1, telementqty, 1 do
		local elementid = GetScreenElementTypeID("user defined global text",elementi)
		if elementid > 0 then
			local elementName = GetScreenElementName(elementid)
			if elementName == "MyNewPointsText" then 
				local pointavailable = 0
				if _G["g_UserGlobal['".."MyNewPoints".."']"] ~= nil then pointavailable = _G["g_UserGlobal['".."MyNewPoints".."']"] end
				if pointavailable == 0 then
					SetScreenElementText(elementid,"")
				else
					if pointavailable == 1 then
						SetScreenElementText(elementid,"You can upgrade your stats, 1 point remains")
					else
						SetScreenElementText(elementid,"You can upgrade your stats, "..pointavailable.." points remain")
					end
				end
			end
		end
	end
	
	-- handle hiding and showing of buttons
	local iqty = GetScreenElements("*")
	for ti = 1, iqty, 1 do
		local elementID = GetScreenElementID("*",ti)
		local buttonElementName = GetScreenElementName(elementID)
		if string.len(buttonElementName) > 0 then
			local buttonOnScreen = 0
			if buttonElementName == "ACCEPT" then buttonOnScreen = 104 end
			if buttonElementName == "ABANDON" then buttonOnScreen = 105 end
			if buttonOnScreen > 0 then
				if buttonOnScreen == 104 or buttonOnScreen == 105 then
					SetScreenElementVisibility(elementID,0)
					if g_UserGlobalQuestTitleShowing ~= "" then
						if buttonOnScreen == 104 then
							if g_UserGlobalQuestTitleShowing ~= g_UserGlobalQuestTitleActive then
								-- allow accepting of quest
								for tquestindex = 1, hud0_quest_qty, 1 do
									if GetCollectionQuestAttribute(tquestindex,"title") == g_UserGlobalQuestTitleShowing then
										if hud0_quest_status[tquestindex] == "active" then
											-- switch to another active quest
											SetScreenElementVisibility(elementID,1)
											break
										end
										if hud0_quest_status[tquestindex] == "inactive" then
											-- allow inactive quest to be accepted - if reached required level
											local tlevelrequired = tonumber(GetCollectionQuestAttribute(tquestindex,"level"))
											if currentplayerlevel >= tlevelrequired then
												SetScreenElementVisibility(elementID,1)
											end
											break
										end
									end
								end
							end
						end
						if buttonOnScreen == 105 then
							if g_UserGlobalQuestTitleShowing == g_UserGlobalQuestTitleActive then
								-- allow canceling of quest
								SetScreenElementVisibility(elementID,1)
							end
						end
					end
				end
			end
		end
	end

	-- handle any button activity for INVENTORY = DROP, USE, BUY, SELL, etc
	if buttonElementID == -1 and hud0_buttonPressed == 1 then hud0_buttonPressed = 0 end
	if buttonElementID ~= -1 and hud0_buttonPressed == 0 then
		hud0_buttonPressed = 1
		local buttonElementName = GetScreenElementName(1+buttonElementID)
		if string.len(buttonElementName) > 0 then
			local actionOnObject = 0
			if buttonElementName == "DROP" then actionOnObject = 1 end
			if buttonElementName == "USE" then actionOnObject = 2 end
			if buttonElementName == "BUY" then actionOnObject = 3 end
			if buttonElementName == "SELL" then actionOnObject = 4 end
			if actionOnObject > 0 then
				-- DROP OR USE OBJECT FROM INVENTORY
				if hud0_gridSelected > 0 and hud0_gridSelectedIndex >= 0 then
					local thisdisplayingselecteditem = hud0_playercontainer_collectionindex[hud0_playercontainer_screenID][hud0_gridSelected][hud0_gridSelectedIndex]
					local thegridelementID = GetScreenElementTypeID("user defined global panel",hud0_gridSelected)
					if thegridelementID > 0 then	
						local thispanelname = GetScreenElementName(thegridelementID)
						local panelname = ""
						local inventorycontainer = thispanelname
						if actionOnObject == 1 or actionOnObject == 2 then
							if thispanelname == "inventory:player" then panelname = thispanelname end
							if thispanelname == "inventory:hotkeys" then panelname = thispanelname end
						end
						if actionOnObject == 3 or actionOnObject == 4 then
							if thispanelname == "inventory:player" then panelname = thispanelname end
							if thispanelname == "inventory:container" then panelname = thispanelname inventorycontainer = "inventory:"..g_UserGlobalContainer end
						end
						local selecteditemcontainerID = -1
						if thispanelname == panelname then selecteditemcontainerID = thisdisplayingselecteditem end
						if selecteditemcontainerID ~= -1 then
							local tinventoryqty = GetInventoryQuantity(inventorycontainer)
							for tinventoryindex = 1, tinventoryqty, 1 do
								local tcollectionindex = GetInventoryItem(inventorycontainer,tinventoryindex)
								local tcollectionslot = GetInventoryItemSlot(inventorycontainer,tinventoryindex)
								if tcollectionindex == selecteditemcontainerID and tcollectionslot == hud0_gridSelectedIndex then
									if actionOnObject == 1 then
										-- DROP
										local entityindex = GetInventoryItemID(inventorycontainer,tinventoryindex)
										if entityindex > 0 then
											local floorlevelfordrop = RDGetYFromMeshPosition(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ)
											local floorlevelhitvalue = IntersectAll(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,g_PlayerPosX,floorlevelfordrop-10.0,g_PlayerPosZ,g_Entity[entityindex].obj)
											if floorlevelhitvalue ~= 0 then 
												floorlevelfordrop = GetIntersectCollisionY()
											end
											local tcancelregulardrop = 0
											if GetEntityCollectable(entityindex) == 2 then
												local tqty = GetEntityQuantity(entityindex)
												if tqty > 1 then
													-- special resource drop
													local entityResourceParentID = GetEntityParentID(entityindex)
													local findunusedresourceee = 0
													for ee = 1, g_EntityElementMax, 1 do
														if entityindex ~= ee then
															if g_Entity[ee] ~= nil then
																if GetEntityParentID(ee) == entityResourceParentID then
																	if GetEntityCollected(ee) ~= 0 then
																		findunusedresourceee = ee
																		break
																	end
																end
															end
														end
													end
													if findunusedresourceee > 0 then
														SetEntityCollected(findunusedresourceee,0,0)
														ResetPosition(findunusedresourceee,g_PlayerPosX,floorlevelfordrop,g_PlayerPosZ)
														Show(findunusedresourceee)
														SetEntityQuantity(entityindex,tqty-1)
													end
													tcancelregulardrop = 1
												end
											end
											if tcancelregulardrop == 0 then
												if panelname == "inventory:hotkeys" then
													-- remove from hot key location
													if hud0_gridSelectedIndex >= 0 and hud0_gridSelectedIndex <= 9 then
														RemovePlayerWeapon(1+hud0_gridSelectedIndex)
													end
												end
												SetEntityCollected(entityindex,0,0)
												ResetPosition(entityindex,g_PlayerPosX,floorlevelfordrop,g_PlayerPosZ)
												Show(entityindex)
											end
										end
									end
									if actionOnObject == 2 then
										-- USE
										local entityindex = GetInventoryItemID(inventorycontainer,tinventoryindex)
										if entityindex > 0 then
											if GetEntityUsed(entityindex) == 0 then
												SetEntityUsed(entityindex,1)
											end
										end
									end
									if actionOnObject == 3 or actionOnObject == 4 then
										-- BUY or SELL
										local tinventorysource = ""
										local tinventorydest = ""
										local thisValue = 0
										if actionOnObject == 3 and panelname == "inventory:container" then 
											-- BUY
											tinventorysource = "inventory:"..g_UserGlobalContainer
											tinventorydest = "inventory:player"
											thisValue = tonumber(GetCollectionItemAttribute(tcollectionindex,"cost"))
										end
										if actionOnObject == 4 and panelname == "inventory:player" then 
											-- SELL
											tinventorysource = "inventory:player"
											tinventorydest = "inventory:"..g_UserGlobalContainer
											thisValue = tonumber(GetCollectionItemAttribute(tcollectionindex,"value"))
										end
										-- money check
										local myMoney = 0
										if _G["g_UserGlobal['".."MyMoney".."']"] ~= nil then myMoney = _G["g_UserGlobal['".."MyMoney".."']"] end
										if (actionOnObject==3 and thisValue <= myMoney) or actionOnObject == 4 then
											-- spend or gain money
											if actionOnObject == 3 then
												myMoney = myMoney - thisValue
											end
											if actionOnObject == 4 then
												myMoney = myMoney + thisValue
											end
											_G["g_UserGlobal['".."MyMoney".."']"] = myMoney
											-- find destination entity if any
											local entityindexto = 0
											local tinventorytoqty = GetInventoryQuantity(tinventorydest)
											for tinventorytoindex = 1, tinventorytoqty, 1 do
												local tcollectiontoindex = GetInventoryItem(tinventorydest,tinventorytoindex)
												if tcollectiontoindex == tcollectionindex then
													entityindexto = GetInventoryItemID(tinventorydest,tinventorytoindex)
													if GetEntityCollectable(entityindexto) ~= 2 then
														entityindexto = 0
													end
												end
											end
											-- deduct from source
											local entityindexfrom = GetInventoryItemID(tinventorysource,tinventoryindex)
											if entityindexfrom > 0 then
												if GetEntityCollectable(entityindexfrom) == 2 then
													local tqtyfrom = GetEntityQuantity(entityindexfrom)
													tqtyfrom = tqtyfrom - 1
													SetEntityQuantity(entityindexfrom,tqtyfrom)
													if tqtyfrom == 0 then
														MoveInventoryItem(tinventorysource,"",-1,entityindexfrom,-1)
														g_UserGlobalContainerRefresh = 1
													end
												else
													entityindexfrom = 0
												end
											end
											-- add to dest or move to dest
											if entityindexfrom > 0 then
												if entityindexto > 0 then
													local tqtyto = 1
													tqtyto = GetEntityQuantity(entityindexto)
													tqtyto = tqtyto + 1
													SetEntityQuantity(entityindexto,tqtyto)
												else
													-- need to create ONE new resource item in destination (as source still needs its item as not depleted)
													local newe = SpawnNewEntity(entityindexfrom)
													SetEntityCollected(newe,3,-1,tinventorydest)
													SetEntityQuantity(newe,1)
													SetEntityActive(newe,0)
													g_UserGlobalContainerRefresh = 1
												end
											else
												local entityindexfrom = GetInventoryItemID(tinventorysource,tinventoryindex)
												if entityindexfrom > 0 then
													MoveInventoryItem(tinventorysource,tinventorydest,-1,entityindexfrom,-1)
												else
													MoveInventoryItem(tinventorysource,tinventorydest,tcollectionindex,0,-1)
												end
												g_UserGlobalContainerRefresh = 1
											end
										end
									end
									break
								end
							end
						end
					end
				end
			end
			local actionOnScreen = 0
			if buttonElementName == "LEAVE" then actionOnScreen = 101 end
			if buttonElementName == "TAKE ALL" then actionOnScreen = 102 end
			if buttonElementName == "CRAFT" then actionOnScreen = 103 end
			if buttonElementName == "ACCEPT" then actionOnScreen = 104 end
			if buttonElementName == "ABANDON" then actionOnScreen = 105 end
			if actionOnScreen > 0 then
				if actionOnScreen == 101 then
					-- LEAVE HUD screen
					ScreenToggle("")
					g_CraftingClass = ""
				end
				if actionOnScreen == 102 then
					-- TAKE ALL Contents
					local cycleuntilnomoretransfers = 1
					while cycleuntilnomoretransfers == 1 do
						cycleuntilnomoretransfers = 0 
						local inventorycontainer = "inventory:"..g_UserGlobalContainer
						local tinventoryqty = GetInventoryQuantity(inventorycontainer)
						for tinventoryindex = 1, tinventoryqty, 1 do
							local tcollectionindex = GetInventoryItem(inventorycontainer,tinventoryindex)
							local tentityindex = GetInventoryItemID(inventorycontainer,tinventoryindex)
							MoveInventoryItem(inventorycontainer,"inventory:player",-1,tentityindex,-1)
							g_UserGlobalContainerRefresh = 1
							cycleuntilnomoretransfers = 1
						end
					end
				end
				
				if actionOnScreen == 103 then
					-- CRAFT recipe from inventory:craft
					local inventorycontainer = "inventory:craft"
					local tinventoryindex = GetInventoryQuantity(inventorycontainer)
					if tinventoryindex == 1 then
						-- determine if can craft 'nameofitemtomake'
						local tcollectionindex = GetInventoryItem(inventorycontainer,tinventoryindex)
						local nameofitemtomake = GetCollectionItemAttribute(tcollectionindex,"description")						
						local anyee = 0
						for ee = 1, g_EntityElementMax, 1 do
							if e ~= ee then
								if g_Entity[ee] ~= nil then
									if GetEntityName(ee) == nameofitemtomake then
										anyee = ee
										break
									end
								end
							end
						end
						if anyee > 0 then
							-- check then eat ingredients
							local listofitems = {}
							local listofitemscount = 0							
							local haveallingredients = 0
							for twopasses = 1, 2, 1 do
								haveallingredients = 1
								local ingredientsneeded = GetCollectionItemAttribute(tcollectionindex,"ingredients")
								while string.len(ingredientsneeded) > 0 do
									i, j = string.find(ingredientsneeded,",")
									if i ~= nil then
										nextingredient = string.sub(ingredientsneeded,1,i-1)
										ingredientsneeded = string.sub(ingredientsneeded,i+1,-1)
									else
										nextingredient = ingredientsneeded
										ingredientsneeded = ""
									end
									if string.len(nextingredient) > 0 then
										local inventorycontainer = "inventory:"..g_UserGlobalContainer
										local tinventoryqty = GetInventoryQuantity(inventorycontainer)
										local wehavethisone = 0
										for tinventoryindex = 1, tinventoryqty, 1 do
											local dowehavethisitem = GetInventoryItem(inventorycontainer,tinventoryindex)
											local nameofingredientitem = GetCollectionItemAttribute(dowehavethisitem,"title")
											local tstring1 = string.upper(nextingredient)
											local tstring2 = string.upper(nameofingredientitem)
											if tstring1 == tstring2 then
												wehavethisone = 1
												if twopasses == 2 then
													-- mark for consumption
													local entityindex = GetInventoryItemID(inventorycontainer,tinventoryindex)
													if entityindex > 0 then
														listofitemscount=listofitemscount+1
														listofitems[listofitemscount] = entityindex
													end
												end
											end
										end
										if wehavethisone == 0 then
											haveallingredients = 0
											break
										end
									end
								end
								if twopasses == 1 and haveallingredients == 0 then
									break
								end
							end
							if haveallingredients == 1 then
								-- consume used items
								local inventorycontainer = "inventory:"..g_UserGlobalContainer
								local cycleuntilnomoretransfers = 1
								while cycleuntilnomoretransfers == 1 do
									cycleuntilnomoretransfers = 0 
									local tinventoryqty = GetInventoryQuantity(inventorycontainer)
									for tinventoryindex = 1, tinventoryqty, 1 do
										local entityindex = GetInventoryItemID(inventorycontainer,tinventoryindex)
										for tlistindex = 1, listofitemscount, 1 do
											if listofitems[tlistindex] == entityindex then
												local tcollectionindex = GetInventoryItem(inventorycontainer,tinventoryindex)
												local tqty = 1
												if GetEntityCollectable(entityindex) == 2 then
													tqty = GetEntityQuantity(entityindex)
												end
												tqty = tqty - 1
												if tqty <= 0 then
													SetEntityCollected(entityindex,0,0)
													Destroy(entityindex)
													cycleuntilnomoretransfers = 1
													break
												else
													if GetEntityCollectable(entityindex) == 2 then
														SetEntityQuantity(entityindex,tqty)
													end
												end
											end
										end
									end
								end
								-- create new item
								local newe = SpawnNewEntity(anyee)
								SetEntityCollected(newe,1,-1)
							end
						end
					end
				end
				if actionOnScreen == 104 then
					-- ACCEPT on QUEST screen
					local findee = 0
					for ee = 1, g_EntityElementMax, 1 do
						if e ~= ee then
							if g_Entity[ee] ~= nil then
								if GetEntityName(ee) == g_UserGlobalQuestTitleShowingObject then
									findee = ee
									break
								end
							end
						end
					end
					if findee > 0 then
						-- update game quest status
						local tquestwasinactive = 0
						for tquestindex = 1, hud0_quest_qty, 1 do
							if GetCollectionQuestAttribute(tquestindex,"title") == g_UserGlobalQuestTitleShowing then
								g_UserGlobalQuestTitleShowingObject = GetCollectionQuestAttribute(tquestindex,"object")
								if hud0_quest_status[tquestindex] == "inactive" then
									tquestwasinactive = 1
								end
								hud0_quest_status[tquestindex] = "active"
								break
							end
						end
						-- set new current quest now if no quest 
						if g_UserGlobalQuestTitleActive == "" or tquestwasinactive == 0 then
							g_UserGlobalQuestTitleActive = g_UserGlobalQuestTitleShowing
							g_UserGlobalQuestTitleActiveObject = g_UserGlobalQuestTitleShowingObject
							g_UserGlobalQuestTitleActiveObject2 = g_UserGlobalQuestTitleShowingObject2
							g_UserGlobalQuestTitleActiveE = findee
						end
						ScreenToggle("")
					else
						-- Error, level did not specify an object for this quest!
						PromptGuruMeditation("ERROR: This QUEST has not been given an OBJECT!")
						ScreenToggle("")
					end
				end
				if actionOnScreen == 105 then
					-- ABANDON current QUEST
					if g_UserGlobalQuestTitleActive ~= nil then
						for tquestindex = 1, hud0_quest_qty, 1 do
							if GetCollectionQuestAttribute(tquestindex,"title") == g_UserGlobalQuestTitleActive then
								hud0_quest_status[tquestindex] = "inactive"
								break
							end
						end
					end
					-- no current quest
					g_UserGlobalQuestTitleActive = ""
					g_UserGlobalQuestTitleActiveObject = ""
					g_UserGlobalQuestTitleActiveObject2 = ""
					g_UserGlobalQuestTitleActiveE = 0
					g_UserGlobalQuestTitleShowing = ""
					g_UserGlobalQuestTitleShowingObject = ""
					g_UserGlobalQuestTitleShowingObject2 = ""
					-- close the HUD
					ScreenToggle("")
				end
			end
			-- if still no action, see if special button
			if actionOnScreen == 0 then
				for tlisti = 1, 5, 1 do
					if buttonElementName == "quest:list:"..tlisti then
						local tactivelistcount = 0
						for tquestindex = 1, hud0_quest_qty, 1 do
							if hud0_quest_status[tquestindex] == "active" then
								tactivelistcount=tactivelistcount+1
								if tactivelistcount == tlisti then
									g_UserGlobalQuestTitleShowing = GetCollectionQuestAttribute(tquestindex,"title")
									g_UserGlobalQuestTitleShowingObject = GetCollectionQuestAttribute(tquestindex,"object")								
								end
							end
						end
					end
				end
			end
		end
	end
	-- monitor active quest object and switch to secondary one if primary is 'handled'
	if g_UserGlobalQuestTitleActiveE > 0 then
		local findee = g_UserGlobalQuestTitleActiveE
		if g_Entity[findee]['active'] == 0 or GetEntityCollected(findee) ~= 0 then
			findee = 0
			for ee = 1, g_EntityElementMax, 1 do
				if e ~= ee then
					if g_Entity[ee] ~= nil then
						if GetEntityName(ee) == g_UserGlobalQuestTitleActiveObject2 then
							findee = ee
							break
						end
					end
				end
			end
			if findee > 0 then
				g_UserGlobalQuestTitleActiveE = findee
			end
		end
	end
	-- handle any button activity for GLOBAL VALUES = AWARD, etc
	if buttonElementID ~= -1 then
		local buttonElementName = GetScreenElementName(1+buttonElementID)
		if string.len(buttonElementName) > 0 then
			local functionOnObject = 0
			local functionName = string.sub(buttonElementName, 1, 6)
			local functionParameter = ""
			if functionName == "AWARD:" then 
				functionOnObject = 1 
				functionParameter = string.sub(buttonElementName, 7, -1)
			end
			if functionOnObject > 0 then
				if functionOnObject == 1 then
					-- AWARD = Spend a POINT to increase a global value
					local pointavailable = 0
					if _G["g_UserGlobal['".."MyNewPoints".."']"] ~= nil then pointavailable = _G["g_UserGlobal['".."MyNewPoints".."']"] end
					local valuetoamend = 0
					if _G["g_UserGlobal['"..functionParameter.."']"] ~= nil then valuetoamend = _G["g_UserGlobal['"..functionParameter.."']"] end
					local valuetoamendmax = 100
					if _G["g_UserGlobal['"..functionParameter.."Max".."']"] ~= nil then valuetoamendmax = _G["g_UserGlobal['"..functionParameter.."Max".."']"] end
					local amounttoamend = 1
					if pointavailable > 0 and valuetoamend < valuetoamendmax then
						_G["g_UserGlobal['"..functionParameter.."']"] = valuetoamend + amounttoamend
						_G["g_UserGlobal['".."MyNewPoints".."']"] = pointavailable - amounttoamend
						hud0_updatePlayerLimits = 1
					end
				end
			end
		end
	end
	
	-- draw mouse pointer below (only for regular HUDs, ie not in-game additional huds)
	if g_liveHudScreen == 0 then
		drawMousePointer = 1
	end
	
	-- VR controller can leave any HUD Screen 9 (VR MENU)
	if GetHeadTracker() == 1 then
		if GetGamePlayerStateMotionController() == 1 and GetGamePlayerStateMotionControllerType() == 2 then -- OPENXR
			if CombatControllerButtonB() == 1 then
				if g_VRHUDToggle == 0 then
					g_VRHUDToggle = 1
					ScreenToggle("")
				end
			else
				g_VRHUDToggle = 0
			end
		end
	end

  end -- was else but now we may want to have main in-game Hud as an additional display at some point (using 'alternative' for now as allows more customizing)
  if currentScreen <= -1 then
  
	-- leave mouse mode
	if hud0_freezeforpointer == 1 and g_liveHudScreen == 0 then 
		DeactivateMouse()
		UnFreezePlayer()
		SetCameraOverride(0)
		hud0_freezeforpointer = 0
		hud0.refreshHUD()
	end
  
	-- display main in-game HUD screen
	DisplayScreen("HUD0")

	-- VR controller can call up HUD Screen 9 (VR MENU)
	if GetHeadTracker() == 1 then
		if GetGamePlayerStateMotionController() == 1 and GetGamePlayerStateMotionControllerType() == 2 then -- OPENXR
			if CombatControllerButtonB() == 1 then
				if g_VRHUDToggle == 0 then
					ScreenToggleByKey("`") --ScreenToggle("HUD Screen 9")
					g_VRHUDToggle = 1
				end
			else
				g_VRHUDToggle = 0
			end
		end
	end

	-- display contents of any hot key panels in the main in-game HUD too (although need to get order from hotkey shortcut)
	local tgridqty = GetScreenElementsType("user defined global panel")
	for gridi = 1, tgridqty, 1 do
		local thegrid = GetScreenElementTypeID("user defined global panel",gridi)
		if thegrid > 0 then
			local panelname = GetScreenElementName(thegrid)
			if panelname == "inventory:hotkeys" then
				local tgridx = 0
				local tgridy = 0
				local tgridwidth = 0
				local tgridheight = 0
				tgridx,tgridy,tgridwidth,tgridheight = GetScreenElementArea(thegrid)    
				totalinrow, totalincolumn = GetScreenElementDetails(thegrid)
				if totalinrow < 1 then totalinrow = 1 end
				if totalincolumn < 1 then totalincolumn = 1 end
				local gridtilewidth = tgridwidth/totalinrow
				local gridtileheight = tgridheight/totalincolumn
				if hud0_gridSpriteID ~= nil then		
					for itemindex = 0, 8, 1 do
						local tcollectionindex = hud0_playercontainer_collectionindex[hud0_playercontainer_screenID][gridi][itemindex]
						local scrimg = hud0_playercontainer_img[hud0_playercontainer_screenID][gridi][itemindex]
						if tcollectionindex ~= nil and scrimg > 0 then
							local scrx = tgridx+((itemindex)*gridtilewidth)
							SetSpritePosition(hud0_gridSpriteID,scrx,tgridy)
							SetSpriteImage(hud0_gridSpriteID,scrimg)
							SetSpriteSize(hud0_gridSpriteID,gridtilewidth,gridtileheight)---1)
							SetSpriteColor(hud0_gridSpriteID,255,255,255,255)
							SetSpritePriority(hud0_gridSpriteID,-1)
							PasteSprite(hud0_gridSpriteID)
						end
					end
				end
			end
		end
	end

  end 

  -- display contents of any user defined images for map views 
  if GetIfUsingTABScreen() == 0 then
   local realmapsize = 1000000 --100000
   if g_LevelTerrainSize > 0 then realmapsize = g_LevelTerrainSize end
   local tqty = GetScreenElementsType("user defined global image")
   for ii = 1, tqty, 1 do
	local theelementID = GetScreenElementTypeID("user defined global image",ii)
	if theelementID > 0 then
		local imagename = GetScreenElementName(theelementID)
		local isMap = string.sub(imagename, 1, 4)
		if isMap == "map:" then
			scrx,scry,tscrwidth,tscrheight = GetScreenElementArea(theelementID)    
			local scrimg = GetScreenElementImage(theelementID)
			SetScreenElementVisibility(theelementID,0)
			local mapName = string.sub(imagename, 5, -1)
			local scritems = -1
			if mapName == "window" then
				hud0_mapView_WindowX = scrx
				hud0_mapView_WindowY = scry
				hud0_mapView_WindowW = tscrwidth
				hud0_mapView_WindowH = tscrheight
			end
			if mapName == "image" then
				scrx = hud0_mapView_ImageX
				scry = hud0_mapView_ImageY
				tscrwidth = hud0_mapView_WindowW
				if hud0_mapView_LevelImage > 0 then
					if GetImageWidth(hud0_mapView_LevelImage) > 0 then scrimg = hud0_mapView_LevelImage end
				end
				tscrheight = tscrwidth * (GetImageHeight(scrimg)/GetImageWidth(scrimg))
				hud0_mapView_ImageOW = tscrwidth
				hud0_mapView_ImageOH = tscrheight
				tscrwidth=tscrwidth*hud0_mapView_Scale
				tscrheight=tscrheight*hud0_mapView_Scale
			end
			if mapName == "player" then
				placex =  g_PlayerPosX
				placez =  g_PlayerPosZ
				scrx = (hud0_mapView_ImageX + (hud0_mapView_ImageW/2)) - (tscrwidth/2) + (placex/realmapsize)*(hud0_mapView_ImageW/2)
				scry = (hud0_mapView_ImageY + (hud0_mapView_ImageH/2)) - (tscrheight/2) - (placez/realmapsize)*(hud0_mapView_ImageH/2)
			end
			local itemstodraw = {}
			local itemstodrawscale = {}
			local itemstodrawscaleoffset = {}
			if mapName == "character" or mapName == "objective" or mapName == "winzone" then
				local entlist = U.ClosestEntities( realmapsize )
				scritems = 0
				for entlistindex = 1, #entlist, 1 do
					local ee = entlist[entlistindex]
					if ee > 0 then
						if g_Entity[ee].active > 0 then
							local tscaleit = 0
							if mapName == "character" then
								local thisallegiance = GetEntityAllegiance(ee)
								if thisallegiance >= 0 then
									scritems = scritems + 1
									itemstodraw[scritems] = ee
									itemstodrawscale[scritems] = 1.0
									itemstodrawscaleoffset[scritems] = 0
									tscaleit = 1
								end
							end
							if mapName == "objective" then
								local thisobjective = GetEntityObjective(ee)
								if thisobjective == 1 then
									if g_UserGlobalQuestTitleActiveE > 0 and g_UserGlobalQuestTitleActiveE == ee then
										scritems = scritems + 1
										itemstodraw[scritems] = ee
										itemstodrawscale[scritems] = 1.0
										itemstodrawscaleoffset[scritems] = 0
										tscaleit = 1
									end
								end
							end
							if mapName == "winzone" then
								local thisobjective = GetEntityObjective(ee)
								if thisobjective == 2 then
									scritems = scritems + 1
									itemstodraw[scritems] = ee
									itemstodrawscale[scritems] = 1.0
									itemstodrawscaleoffset[scritems] = 0
									tscaleit = 1
								end
							end
							if tscaleit == 1 then
								if g_UserGlobalQuestTitleActiveE > 0 and g_UserGlobalQuestTitleActiveE == ee then
									itemstodrawscale[scritems] = 2.0 + math.cos(Timer()/500.0)
									itemstodrawscaleoffset[scritems] = 1
								end
							end
						end
					end
				end
			end
			if hud0_gridSpriteID ~= nil then
				local drawthelot = scritems
				if scritems == -1 and drawthelot < 1 then drawthelot = 1 end
				for titem = 1, drawthelot, 1 do
					if scritems > 0 then
						placex = g_Entity[itemstodraw[titem]].x
						placez = g_Entity[itemstodraw[titem]].z
						scrx = (hud0_mapView_ImageX + (hud0_mapView_ImageW/2)) - (tscrwidth/2) + (placex/realmapsize)*(hud0_mapView_ImageW/2)
						scry = (hud0_mapView_ImageY + (hud0_mapView_ImageH/2)) - (tscrheight/2) - (placez/realmapsize)*(hud0_mapView_ImageH/2)
					end
					local tusescrwidth = tscrwidth
					local tusescrheight = tscrheight
					if scritems > 0 then
						tusescrwidth = tusescrwidth * itemstodrawscale[titem]
						tusescrheight = tusescrheight * itemstodrawscale[titem]
					end
					if scrimg > 0 then
						SetSpritePosition(hud0_gridSpriteID,scrx,scry)
						SetSpriteImage(hud0_gridSpriteID,scrimg)
						if itemstodrawscaleoffset[titem] == 1 then
							SetSpriteOffset(hud0_gridSpriteID,tusescrwidth/2,-1)
						else
							SetSpriteOffset(hud0_gridSpriteID,0,0)
						end
						SetSpriteSize(hud0_gridSpriteID,tusescrwidth,tusescrheight)
						SetSpriteColor(hud0_gridSpriteID,255,255,255,255)
						SetSpritePriority(hud0_gridSpriteID,-1)
						SetSpriteScissor(hud0_mapView_WindowX,hud0_mapView_WindowY,hud0_mapView_WindowW,hud0_mapView_WindowH)
						PasteSprite(hud0_gridSpriteID)
						SetSpriteScissor(0,0,0,0)
						SetSpriteOffset(hud0_gridSpriteID,0,0)
					end
				end
			end
		end
	end
   end
   -- map controls and scroller update
   local updateMapViewImage = 1
   if 1 then
	myuserglobal = "MyMapMiniZoom"
	if GetCurrentScreen() > -1 then
		if _G["g_UserGlobal['"..myuserglobal.."']"] ~= nil then hud0_mapView_Scale = _G["g_UserGlobal['"..myuserglobal.."']"]/100.0 end
		if g_MouseWheel > 0 then
			hud0_mapView_Scale = hud0_mapView_Scale + 0.2
			if hud0_mapView_Scale > 50 then hud0_mapView_Scale = 50 end
		end
		if g_MouseWheel < 0 then
			hud0_mapView_Scale = hud0_mapView_Scale - 0.2
			if hud0_mapView_Scale < 1 then hud0_mapView_Scale = 1 end
		end
		_G["g_UserGlobal['"..myuserglobal.."']"] = hud0_mapView_Scale * 100.0
	else
		if _G["g_UserGlobal['"..myuserglobal.."']"] ~= nil then hud0_mapView_Scale = _G["g_UserGlobal['"..myuserglobal.."']"]/100.0 end
	end
   end
   if updateMapViewImage == 1 then
	tscrwidth = hud0_mapView_ImageOW
	tscrheight = hud0_mapView_ImageOH
	tscrwidth=tscrwidth*hud0_mapView_Scale
	tscrheight=tscrheight*hud0_mapView_Scale
	hud0_mapView_ImageW = tscrwidth
	hud0_mapView_ImageH = tscrheight
	placex =  g_PlayerPosX
	placez =  g_PlayerPosZ
	scrollxlimit = (hud0_mapView_ImageW/2) - (hud0_mapView_WindowW/2)
	hud0_mapView_ScrollX = (placex/realmapsize)*(hud0_mapView_ImageW/2)
	if hud0_mapView_ScrollX < scrollxlimit*-1 then hud0_mapView_ScrollX = scrollxlimit*-1 end
	if hud0_mapView_ScrollX > scrollxlimit then hud0_mapView_ScrollX = scrollxlimit end
	scrollylimit = (hud0_mapView_ImageH/2) - (hud0_mapView_WindowH/2)	
	hud0_mapView_ScrollY = ((placez*-1)/realmapsize)*(hud0_mapView_ImageH/2)
	if hud0_mapView_ScrollY < scrollylimit*-1 then hud0_mapView_ScrollY = scrollylimit*-1 end
	if hud0_mapView_ScrollY > scrollylimit then hud0_mapView_ScrollY = scrollylimit end
	hud0_mapView_ImageX = hud0_mapView_WindowX - ((hud0_mapView_ImageW-hud0_mapView_WindowW)/2) - hud0_mapView_ScrollX
	hud0_mapView_ImageY = hud0_mapView_WindowY - ((hud0_mapView_ImageH-hud0_mapView_WindowH)/2) - hud0_mapView_ScrollY
   end
  end
	
  -- handle any adjustments to player limits and maximums	
  if hud0_updatePlayerLimits == 1 then
	-- these can be amended via custom scripts for more sophisticated mechanics
	local modifyglobal = ""
	-- strength increases max health
	local newhealthmax = 0
	if _G["g_UserGlobal['".."MyHealthInitial".."']"] ~= nil then
		local currentstrength = 0 if _G["g_UserGlobal['".."MyStrength".."']"] ~= nil then currentstrength = _G["g_UserGlobal['".."MyStrength".."']"] end
		local initialhealthmax = _G["g_UserGlobal['".."MyHealthInitial".."']"]
		newhealthmax = initialhealthmax + (currentstrength*100)
	else
		newhealthmax = g_PlayerStartStrength
	end
	SetGamePlayerControlStartStrength(newhealthmax)
	g_gameloop_StartHealth = newhealthmax
	-- intelligence increases max mana
	if _G["g_UserGlobal['".."MyManaInitial".."']"] ~= nil then 
		local currentintelligence = 0 if _G["g_UserGlobal['".."MyIntelligence".."']"] ~= nil then currentintelligence = _G["g_UserGlobal['".."MyIntelligence".."']"] end
		local initialmanamax = _G["g_UserGlobal['".."MyManaInitial".."']"]
		modifyglobal = "MyManaMax" _G["g_UserGlobal['"..modifyglobal.."']"] = initialmanamax + (currentintelligence*100)
	end
	-- dexterity increases stamina
	if _G["g_UserGlobal['".."MyStaminaInitial".."']"] ~= nil then
		local currentdexterity = 0 if _G["g_UserGlobal['".."MyDexterity".."']"] ~= nil then currentdexterity = _G["g_UserGlobal['".."MyDexterity".."']"] end
		local initialstaminamax = _G["g_UserGlobal['".."MyStaminaInitial".."']"]
		modifyglobal = "MyStaminaMax" _G["g_UserGlobal['"..modifyglobal.."']"] = initialstaminamax + (currentdexterity*100)
	end
  end

  -- draw mouse pointer last
  if drawMousePointer == 1 then
	if hud0_pointerID == -1 then hud0_pointerID = GetScreenElementID("pointer",1) end
	if hud0_pointerID > 0 then
		-- use custom HUD pointer image if available
		SetScreenElementPosition(hud0_pointerID,g_sprCursorPtrX,g_sprCursorPtrY)
		hud0_pointerSpriteImageID = GetScreenElementImage(hud0_pointerID)
	end
	if hud0_pointerSpriteImageID == 0 then hud0_pointerSpriteImageID = LoadImage("imagebank\\HUD\\cursor-medium.png") end
	if hud0_pointerSpriteID == nil then hud0_pointerSpriteID = CreateSprite ( hud0_pointerSpriteImageID ) end
	SetSpritePosition ( hud0_pointerSpriteID, g_sprCursorPtrX, g_sprCursorPtrY )
	SetSpritePriority(hud0_pointerSpriteID,-1)
	PasteSprite(hud0_pointerSpriteID)
  end
  
  -- handle slow absorbsion of magic
  local currentmana = 0 
  if _G["g_UserGlobal['".."MyMana".."']"] ~= nil then currentmana = _G["g_UserGlobal['".."MyMana".."']"] end
  local maxiumummana = 100 if _G["g_UserGlobal['".."MyManaMax".."']"] ~= nil then maxiumummana = _G["g_UserGlobal['".."MyManaMax".."']"] end
  if currentmana < maxiumummana then
	currentmana = currentmana + 0.01
  end
  _G["g_UserGlobal['".."MyMana".."']"] = currentmana
  
  -- handle awarding of XP points
  local maximumXP = 0 if _G["g_UserGlobal['".."MyXPMax".."']"] ~= nil then maximumXP = _G["g_UserGlobal['".."MyXPMax".."']"] end
  if maximumXP > 0 then
	local currentXP = 0
	if _G["g_UserGlobal['".."MyXP".."']"] ~= nil then currentXP = _G["g_UserGlobal['".."MyXP".."']"] end
	local entitykilled = GetNearestEntityDestroyed(0)
	if entitykilled > 0 then
		-- determine what has been destroyed
		local allegiance = GetEntityAllegiance(entitykilled) -- get the allegiance value for this object (-1-none, 0-enemy, 1-ally, 2-neutral)
		if allegiance == 0 then
			local scoredXP = 100
			currentXP = currentXP + scoredXP
		end
	end
	if currentXP >= maximumXP then
		-- levelling up!
		triggerElementPrompt = "MyLevelUpText"
		triggerElementPromptText = "YOU'VE LEVELLED UP!"
		triggerElementPromptTimer = Timer()
		if GetGlobalSoundExist(hud0_sounds_levelup) ==1 then PlayGlobalSound(hud0_sounds_levelup) end
		-- reset current XP
		currentXP = currentXP - maximumXP
		_G["g_UserGlobal['".."MyXP".."']"] = currentXP 
		-- increase new points available
		local pointavailable = 0 if _G["g_UserGlobal['".."MyNewPoints".."']"] ~= nil then pointavailable = _G["g_UserGlobal['".."MyNewPoints".."']"] end
		_G["g_UserGlobal['".."MyNewPoints".."']"] = pointavailable + 2
		-- increase player level
		local playerlevel = 1 if _G["g_UserGlobal['".."MyPlayerLevel".."']"] ~= nil then playerlevel = _G["g_UserGlobal['".."MyPlayerLevel".."']"] end
		playerlevel = playerlevel + 1
		_G["g_UserGlobal['".."MyPlayerLevel".."']"] = playerlevel
		-- new XP threshold for new player level
		maximumXP = playerlevel * 500
	end
	modifyglobal = "MyXP" _G["g_UserGlobal['"..modifyglobal.."']"] = currentXP
	modifyglobal = "MyXPMax" _G["g_UserGlobal['"..modifyglobal.."']"] = maximumXP
  end
  
  -- handle any general element text work
  if triggerElementPrompt ~= nil then
	local telementqty = GetScreenElementsType("user defined global text")
	local elementbeingused = -1
	for elementi = 1, telementqty, 1 do
		local elementid = GetScreenElementTypeID("user defined global text",elementi)
		if elementid > 0 then
			local elementName = GetScreenElementName(elementid)
			if elementName == triggerElementPrompt then 
				elementbeingused = elementid
			end
		end
	end
	if elementbeingused > -1 then
		if Timer() < triggerElementPromptTimer + 3000 then
			SetScreenElementText(elementbeingused,triggerElementPromptText)
		else
			SetScreenElementText(elementbeingused,"")
			triggerElementPrompt = nil
		end
	end
  end 
 
  -- remove sprite pasters
  if hud0_pointerSpriteID ~= nil then SetSpritePosition(hud0_pointerSpriteID,-99999,-99999) end
  if hud0_gridSpriteID ~= nil then SetSpritePosition(hud0_gridSpriteID,-99999,-99999) end
      
  -- Hot Key Select can be from keys 1,2,3,4,5,6,7,8,9,0 in any HUD screen
  local tselkeystate = 0
  if tselkeystate == 0 then 
	for ti = 2, 11, 1 do
		if GetKeyState(ti) == 1 then 
			tselkeystate = ti-1
			break
		end
	end
  end
  if tselkeystate >= 1 and tselkeystate <= 9 then
	itemindex = tselkeystate - 1
	local panelname = "inventory:hotkeys"
	local tinventoryqty = GetInventoryQuantity(panelname)
	for tinventoryindex = 1, tinventoryqty, 1 do
		local tslotindex = GetInventoryItemSlot(panelname,tinventoryindex)
		if tslotindex == itemindex then 
			local entityindex = GetInventoryItemID(panelname,tinventoryindex)
			if entityindex > 0 then
				if GetEntityUsed(entityindex) == 0 then
					SetEntityUsed(entityindex,1)
				end
			end
		end
	end
  end
  
 end
end

function hud0.quit()
end

return hud0