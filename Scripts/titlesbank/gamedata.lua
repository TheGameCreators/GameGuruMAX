-- gamedata module version 0.01.8
--  added table saving
--   minor optimosation (0.01.09)

local gamedata = {}

g_gamedata_levelstatemode = 0

function gamedata.mode(savelevelstatemode)
	g_gamedata_levelstatemode = savelevelstatemode
end

function gamedata.save(slotnumber,uniquename)

	-- g_gamedata_levelstatemode : 0-game save (1+2), 1-level state, 2-player stats

	-- save slot file
	file = io.open("savegames\\gameslot" .. slotnumber .. ".dat", "w")
	io.output(file)
	
	-- header for game ID (untouched for compatablity )
	io.write(123 .. "\n")
	local tgnn = 0
	if g_iGameNameNumber ~= nil then tgnn = g_iGameNameNumber end
	io.write(tgnn .. "\n")
	io.write(uniquename .. "\n")
	io.write(g_LevelFilename .. "\n")
	io.write(0 .. "\n")

	-- player stats
	if g_gamedata_levelstatemode == 0 then
		save("iPlayerPosX",g_PlayerPosX)
		save("iPlayerPosY",g_PlayerPosY)
		save("iPlayerPosZ",g_PlayerPosZ)
		save("iPlayerAngX",g_PlayerAngX)
		save("iPlayerAngY",g_PlayerAngY)
		save("iPlayerAngZ",g_PlayerAngZ)
	end
	if g_gamedata_levelstatemode ~= 1 then
		save("iPlayerHealth",g_PlayerHealth)
		save("iPlayerLives",g_PlayerLives)
		save("strPlayerGunName",g_PlayerGunName)
	end
	
	-- game containers for game and level state (player globals only needs first two)
	local invtotal = GetInventoryTotal()
	if g_gamedata_levelstatemode == 2 then invtotal = 2 end
	save("g_UserContainerTotal",invtotal)
	for c = 0, invtotal-1, 1 do
		local inventorycontainer = GetInventoryName(c)
		local tinventoryqty = GetInventoryQuantity(inventorycontainer)
		g_UserContainerName[c] = inventorycontainer
		g_UserContainerCount[c] = tinventoryqty
		save("g_UserContainerName["..c.."]",inventorycontainer)
		save("g_UserContainerCount["..c.."]",tinventoryqty)
		for tinventoryindex = 1, tinventoryqty, 1 do
			local tslotindex = GetInventoryItemSlot(inventorycontainer,tinventoryindex)
			local tcollectionindex = GetInventoryItem(inventorycontainer,tinventoryindex)
			local tcollectionindexe = GetInventoryItemID(inventorycontainer,tinventoryindex)
			local fulloffset = (c*100000)+tinventoryindex
			g_UserContainerIndex[fulloffset] = tcollectionindex
			g_UserContainerSlot[fulloffset] = tslotindex
			local tQty = GetEntityQuantity(tcollectionindexe)
			g_UserContainerQty[fulloffset] = tQty
			g_UserContainerE[fulloffset] = tcollectionindexe
			save("g_UserContainerIndex[" .. fulloffset .. "]", g_UserContainerIndex[fulloffset])
			save("g_UserContainerSlot[" .. fulloffset .. "]", g_UserContainerSlot[fulloffset])
			save("g_UserContainerQty[" .. fulloffset .. "]", g_UserContainerQty[fulloffset])
			save("g_UserContainerE[" .. fulloffset .. "]", g_UserContainerE[fulloffset])
		end
	end

	-- all entity elements
	if g_gamedata_levelstatemode ~= 2 then
		save("g_EntityElementMax",g_EntityElementMax)
		for e = 1, g_EntityElementMax, 1 do
			if g_Entity[e] ~= nil then
				save("g_Entity[" .. e .. "]", {})
				save("g_Entity[" .. e .. "]['x']", g_Entity[e]['x'])
				save("g_Entity[" .. e .. "]['y']", g_Entity[e]['y'])
				save("g_Entity[" .. e .. "]['z']", g_Entity[e]['z'])
				save("g_Entity[" .. e .. "]['anglex']", GetEntityAngleX(e))
				save("g_Entity[" .. e .. "]['angley']", GetEntityAngleY(e))
				save("g_Entity[" .. e .. "]['anglez']", GetEntityAngleZ(e))
				save("g_Entity[" .. e .. "]['active']", GetEntityActive(e))
				save("g_Entity[" .. e .. "]['activated']",g_Entity[e]['activated'])
				save("g_Entity[" .. e .. "]['collected']", g_Entity[e]['collected'])
				save("g_Entity[" .. e .. "]['haskey']", g_Entity[e]['haskey'])
				save("g_Entity[" .. e .. "]['health']", g_Entity[e]['health'])
				save("g_Entity[" .. e .. "]['frame']", g_Entity[e]['frame'])
				save("g_EntityExtra[" .. e .. "]['visible']", g_EntityExtra[e]['visible'])
				save("g_EntityExtra[" .. e .. "]['spawnatstart']", GetEntitySpawnAtStart(e))
				save("g_EntityExtra[" .. e .. "]['clonedsincelevelstart']", GetEntityClonedSinceStartValue(e))
			end
		end
	end

	-- radarobjectives array
	if g_gamedata_levelstatemode ~= 2 then
		if radarObjectives ~= nil then
			save ("radarObjectives", radarObjectives)
		end
	end

	-- make sure weapon info at latest
	UpdateWeaponStats() 

	-- globals
	for n in pairs(_G) do
		if string.find(tostring(_G[n]), "function:") == nil then
			local dumpout = 1
			if n == "file" then dumpout = 0 end
			if n == "_G" then dumpout = 0 end
			if n == "io" then dumpout = 0 end
			if n == "math" then dumpout = 0 end
			if string.sub(n,1,2) == "g_" then -- Other globals
			
				if g_gamedata_levelstatemode == 2 then
				
					-- only dump player state globals as we move from level to level
					dumpout = 0
					if string.sub(n,1, 8) == "g_Weapon" then dumpout = 1 end
					if string.sub(n,1,14) == "g_UserGlobal['" then dumpout = 1 end
					if string.sub(n,1,12) == "g_Containers" then dumpout = 1 end

				else
			
					-- filter out Suff we don't want to save
					-- choped it into chunks to cut down number of string compars per iteration
					if n == "g_Entity" or n == "g_EntityExtra" then -- (0.01.09) these take up the most slots so get out early saves time
						dumpout = 0
					else
						if string.sub(n,1,8) == "g_Player" then -- no Player stuff as taken care of else where (saves 22)
							if n == "g_PlayerPosX" then dumpout = 0  -- restore target changed to iPlayerPosX
								elseif n == "g_PlayerPosY" then dumpout = 0  -- restore target changed to iPlayerPosY
								elseif n == "g_PlayerPosZ" then dumpout = 0  -- restore target changed to iPlayerPosZ
								elseif n == "g_PlayerAngX" then dumpout = 0  -- restore target changed to iPlayerAngX
								elseif n == "g_PlayerAngY" then dumpout = 0  -- restore target changed to iPlayerAngY
								elseif n == "g_PlayerAngZ" then dumpout = 0  -- restore target changed to iPlayerAngZ
								elseif n == "g_PlayerHealth" then dumpout = 0  -- restore target changed to iPlayerHealth
								elseif n == "g_PlayerLives" then dumpout = 0  -- restore target changed to iPlayerLives
								elseif n == "g_PlayerGunName" then dumpout = 0  -- restore target changed to strPlayerGunName
								elseif n == "g_PlayerGunZoomed" then dumpout = 0
								elseif n == "g_PlayerGunID" then dumpout = 0
								elseif n == "g_PlayerGunFired" then dumpout = 0	
								elseif n == "g_PlayerGunMode" then dumpout = 0
								elseif n == "g_PlayerGunClipCount" then dumpout = 0
								elseif n == "g_PlayerGunAmmoCount" then dumpout = 0
								elseif n == "g_PlayerGunCount" then dumpout = 0
								elseif n == "g_PlayerFlashlight" then dumpout = 0
								elseif n == "g_PlayerLastHitTime" then dumpout = 0						
								elseif n == "g_PlayerObjNo" then dumpout = 0
								elseif n == "g_PlayerDeadTime" then dumpout = 0
								elseif n == "g_PlayerThirdPerson" then dumpout = 0
								elseif n == "g_PlayerController" then dumpout = 0
								elseif n == "g_PlayerFOV" then dumpout = 0
							end
						end
		
						if string.sub(n,1,7) == "g_Mouse" then -- no mouse stuff (saves 3)
							if n == "g_MouseX" then dumpout = 0
								elseif n == "g_MouseY" then dumpout = 0
								elseif n == "g_MouseClick" then dumpout = 0
								elseif n == "g_MouseWheel" then dumpout = 0
							end
						end
		
						if string.sub(n,1,11) == "g_gameloop_" then -- no game control stuff (saves 3)
							if n == "g_gameloop_RegenRate" then dumpout = 0
								elseif n == "g_gameloop_RegenSpeed" then dumpout = 0
								elseif n == "g_gameloop_RegenDelay" then dumpout = 0
								elseif n == "g_gameloop_StartHealth" then dumpout = 0
							end
						end
		
						if string.sub(n,1,3) == "g_i" then -- (saves 18)
							if string.sub(n,1,5) == "g_img" then -- no standard images ?? (saves 10)
								if n == "g_imgCursor" then dumpout = 0
									elseif n == "g_imgBackdrop" then dumpout = 0
									elseif n == "g_imgHeading" then dumpout = 0
									elseif n == "g_imgAboutBackdrop" then dumpout = 0
									elseif n == "g_imgAboutCursor" then dumpout = 0
									elseif n == "g_imgProgressB" then dumpout = 0
									elseif n == "g_imgProgressF" then dumpout = 0
									elseif n == "g_imgAboutButton" then dumpout = 0 -- not in original
									elseif n == "g_imgAboutButtonH" then dumpout = 0 -- not in original
									elseif n == "g_imgButton" then dumpout = 0  -- not in original
									elseif n == "g_imgButtonS" then dumpout = 0 -- not in original
									elseif n == "g_imgButtonH" then dumpout = 0 -- not in original
								end
								elseif n == "g_iGameNameNumber" then dumpout = 0 -- no standard integers
								elseif n == "g_iGraphicChoice" then dumpout = 0
								elseif n == "g_iLoadingCountdown" then dumpout = 0
								elseif n == "g_iMusicChoice" then dumpout = 0
								elseif n == "g_iSoundChoice" then dumpout = 0
							end
						end
		
						if string.sub(n,1,5) == "g_spr" then -- no standard sprites ?? (saves 11)
							if n == "g_sprHeading" then dumpout = 0
								elseif n == "g_sprSlider" then dumpout = 0
								elseif n == "g_sprSliderM" then dumpout = 0
								elseif n == "g_sprProgressF" then dumpout = 0
								elseif n == "g_sprAboutBackdrop" then dumpout = 0
								elseif n == "g_sprCursor" then dumpout = 0
								elseif n == "g_sprSliderS" then dumpout = 0
								elseif n == "g_sprAboutCursor" then dumpout = 0
								elseif n == "g_sprProgressB" then dumpout = 0
								elseif n == "g_sprBackdrop" then dumpout = 0
								elseif n == "g_sprButton" then dumpout = 0 -- not in original
								elseif n == "g_sprAboutButton" then dumpout = 0 -- not in original
							end
						end
		
						if string.sub(n,1,10) == "g_KeyPress" then -- no keystates (saves 9)
							if n == "g_KeyPressC" then dumpout = 0
								elseif n == "g_KeyPressA" then dumpout = 0
								elseif n == "g_KeyPressR" then dumpout = 0
								elseif n == "g_KeyPressF" then dumpout = 0
								elseif n == "g_KeyPressS" then dumpout = 0
								elseif n == "g_KeyPressD" then dumpout = 0
								elseif n == "g_KeyPressW" then dumpout = 0
								elseif n == "g_KeyPressE" then dumpout = 0
								elseif n == "g_KeyPressSHIFT" then dumpout = 0
								elseif n == "g_KeyPressSPACE" then dumpout = 0
							end
						end
		
						if string.sub(n,1,5) == "g_str" then -- nostandard strings (saves 3)
							if n == "g_strStyleFolder" then dumpout = 0
								elseif n == "g_strBestResolution" then dumpout = 0
								elseif n == "g_strLevelFilenames" then dumpout = 0 -- not in original
								elseif n == "g_strSlotNames" then dumpout = 0 -- not in original
							end
						end
		
						if string.sub(n,1,5) == "g_pos" then -- no standard positioning info (saves 3)
							if n == "g_posButton" then dumpout = 0 -- not in original
								elseif n == "g_posAboutButton" then dumpout = 0 -- not in original
								elseif n == "g_posBackdropAngle" then dumpout = 0
								elseif n == "g_posAboutBackdropAngle" then dumpout = 0
							end
						end
						
						if string.sub(n,1,19) == "g_masterinterpreter" then -- no interpreter stuff
							dumpout = 0
						end

						-- no weapon data needed in level state file
						if g_gamedata_levelstatemode == 1 then --g_gamedata_levelstatemode ~= 1 then	
							if string.sub(n,1,8) == "g_Weapon" then dumpout = 0 end
						end						
	
						-- oddes and ends
						if n == "g_InKey" then dumpout = 0
							elseif n == "g_Time" then dumpout = 0
							elseif n == "g_TimeElapsed" then dumpout = 0
							elseif n == "g_GameStateChange" then dumpout = 0
							elseif n == "g_LevelFilename" then dumpout = 0
							elseif n == "g_DebugStringPeek" then dumpout = 0
							elseif n == "g_Scancode" then dumpout = 0
						end
						
						if n == "g_UserContainerTotal" then dumpout = 0 end
						if n == "g_EntityElementMax" then dumpout = 0 end
						
					end
				end
				if dumpout == 1 then
					save (n, _G[n])
				end
			end
		end
	end
	
	-- states in HUD0 that must be retained
	save("hud0_populateallcontainers",hud0_populateallcontainers)
	local tcollectionmax = GetCollectionItemQuantity()
	for c = 1, tcollectionmax, 1 do
		save("hud0_populateallcontainersfilled["..c.."]",hud0_populateallcontainersfilled[c])
	end
	save("hud0_quest_qty",hud0_quest_qty)
	for c = 1, hud0_quest_qty, 1 do
		save("hud0_quest_status["..c.."]",hud0_quest_status[c])
	end

	-- will mean load returning successful means whole file read
	io.write("successful=1\n") 
	io.close(file)

end

function gamedata.load(slotnumber)

	-- if cloned extra entities in the game save, need to creatr these in the table
	local oldEntityElementMax = g_EntityElementMax
	-- load game data
	successful = 0
	--local file = assert(io.open("savegames\\gameslot" .. slotnumber .. ".dat", "r"))
	local file = io.open("savegames\\gameslot" .. slotnumber .. ".dat", "r")
	if file ~= nil then
		-- header for game ID
		iMagicNumber = tonumber(file:read())
		iGameNameNumber = tonumber(file:read())
		uniquename = file:read()
		strLevelFilename = file:read()
		iReserved = tonumber(file:read())
		-- get game data
		strField = file:read()
		while strField ~= nil do -- get each data item  optimised (0.01.09)
		
			--LB: This slight modification avoids crash with 3 dimensional arrays
			local func = nil
			local err
			
			if string.find(strField,"{}$") == nil and string.find(strField, "=") then -- crashes  if {} ?  and need  = so skip

				if string.sub(strField,1,14) == "g_UserGlobal['" then
					i, j = string.find(strField,"=")
					if i ~= nil then
						local fireasglobal1 = '_G["'
						local fireasglobal2 = string.sub(strField,1,i-1)
						local fireasglobal3 = '"] '..string.sub(strField,i,-1)
						local fireasglobal = fireasglobal1..fireasglobal2..fireasglobal3
						func, err = load(fireasglobal)
					end
				else
					strsub = string.match(strField,"%w-%[(%d%d-)%]") -- new approach (0.01.09)
					if strsub then
						i = tonumber(strsub) 
						if (string.sub(strField,1,8) == "g_Entity" and g_Entity[i] == nil) or (string.sub(strField,1,16) == "radarObjectives[" and radarObjectives[i] == nil) then
							func = nil
						else
							func, err = load(strField)
						end
					else
						func, err = load(strField)
					end
				end
				
				-- skip bad lines as bad lines crash gg also check for bug reported in 2002 re incorrect type returned from load
				if func then
				  local ok, add = pcall(func)
				  if ok then	  
					if string.sub(strField,1,18) == "g_EntityElementMax" then
						for i = oldEntityElementMax+1, g_EntityElementMax, 1 do
							g_Entity[i] = {}
							g_Entity[i]['x'] = 0
							g_Entity[i]['y'] = 0
							g_Entity[i]['z'] = 0
							g_Entity[i]['anglex'] = 0
							g_Entity[i]['angley'] = 0
							g_Entity[i]['anglez'] = 0
							g_Entity[i]['active'] = 0
							g_Entity[i]['activated'] = 0
							g_Entity[i]['collected'] = 0
							g_Entity[i]['haskey'] = 0
							g_Entity[i]['health'] = 0
							g_Entity[i]['frame'] = 0
							g_EntityExtra[i] = {}
							--g_EntityExtra[i]['visible'] = -1
							--g_EntityExtra[i]['spawnatstart'] = 0
							--g_EntityExtra[i]['clonedsincelevelstart'] = 0
						end
					end
				  end
				end				
				
			end
			strField = file:read()
		end
		-- end of file
		file:close()
	end
	return successful --successful = 1 success set in file
end

function bS (o) -- covert to safe strings
  if type(o) ==  "number" then
    return tostring(o)
  else   -- assume it is a string
    return string.format("%q", o) -- using %q rather than %s to cater for embended stuff
  end
end

function save (name, value, saved) -- based on example in manual
	saved = saved or {}       -- initial value
	io.write(name, "=")
	if type(value) == "number" or type(value) == "string" then
		io.write(bS(value), "\n")
	elseif type(value) == "table" then
		if saved[value] then    -- value already saved?
			io.write(saved[value], "\n")  -- use the previous name
		else
			saved[value] = name   -- save name use next time
			io.write("{}\n")     -- create new table
			for k,v in pairs(value) do      -- save its fields
				local fieldname = string.format("%s[%s]", name, bS(k))
				save(fieldname, v, saved) -- recursion be very careful
			end
		end
	else
		io.write("Error ".. name," -- cannot save a " .. type(value).."\n") -- this will fail as bad line on load
	end
end

return gamedata
