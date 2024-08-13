-- Resolutions module

local restoregame = {}

g_restoregame_mode = 0

function restoregame.mode(mode)
 g_restoregame_mode = mode
end

function restoregame.now()
 -- trigger entire game to restore to saved game state
 if g_restoregame_mode == 0 then
  SetGamePlayerStateCamMouseMoveX(0)
  SetGamePlayerStateCamMouseMoveY(0)
  if iPlayerAngX > 180 then iPlayerAngX = iPlayerAngX - 360 end
  if iPlayerAngX < -180 then iPlayerAngX = iPlayerAngX + 360 end
  SetFreezePosition(iPlayerPosX,iPlayerPosY,iPlayerPosZ)
  SetFreezeAngle(iPlayerAngX,iPlayerAngY,iPlayerAngZ)
  TransportToFreezePosition()
 end
 SetPlayerHealth(iPlayerHealth)
 SetPlayerLives(iPlayerLives)
 -- restore weapon stats
 ResetWeaponSystems()
 for i = 1, 10, 1 do
  SetWeaponSlot ( i, g_WeaponSlotGot[i], g_WeaponSlotPref[i] )
 end 
 for i = 1, 20, 1 do
  SetWeaponAmmo ( i, g_WeaponAmmo[i] )
  SetWeaponClipAmmo ( i, g_WeaponClipAmmo[i] )
 end 
 for i = 0, 100, 1 do
  SetWeaponPoolAmmo ( i, g_WeaponPoolAmmo[i] )
 end 
 ChangePlayerWeapon(strPlayerGunName)
 -- restore entity stats
 CreateEntityIfNotPresent(g_EntityElementMax)
 for i = 1, g_EntityElementMax, 1 do
  if g_Entity[i] ~= nil then
  if g_Entity[i]['x'] ~= nil then
   if g_EntityExtra[i]['collision'] ~= nil then 
    if g_EntityExtra[i]['collision'] == 1 then 
     CollisionOn(i) 
    else
     CollisionOff(i) 
    end
   end
   ResetPosition ( i, g_Entity[i]['x'], g_Entity[i]['y'], g_Entity[i]['z'] )
   ResetRotation ( i, g_Entity[i]['anglex'], g_Entity[i]['angley'], g_Entity[i]['anglez'] )
   SetEntityActive ( i, g_Entity[i]['active'] )
   SetEntityActivated ( i, g_Entity[i]['activated'] )
   SetEntityCollected ( i, math.abs(g_Entity[i]['collected'])*-1 )
   SetEntityHasKey ( i, g_Entity[i]['haskey'] )
   SetEntityHealth ( i, g_Entity[i]['health'] )
   local regularorclonerefresh = 0
   if g_EntityExtra[i]['clonedsincelevelstart'] ~= nil then 
    if g_EntityExtra[i]['clonedsincelevelstart'] > 0 then 
	 -- this entiy was cloned since level start (rabbits, etc) so must be created before can be restored
	 RefreshEntityFromParent ( i, g_EntityExtra[i]['clonedsincelevelstart'] )
	 regularorclonerefresh = 1
    end
   end
   if regularorclonerefresh == 0 then
    RefreshEntity ( i )
   end
   -- RefreshEntity restores collision to default, so override again
   if g_EntityExtra[i]['collision'] ~= nil then 
    if g_EntityExtra[i]['collision'] == 1 then 
     CollisionOn(i) 
    else
     CollisionOff(i) 
    end
   end
   SetAnimationFrame ( i, g_Entity[i]['frame'] )
   if g_EntityExtra[i]['visible'] ~= nil then 
    if g_EntityExtra[i]['visible']~=-1 then
	 if g_EntityExtra[i]['visible']==1 then
	  Show ( i )
	 else
	  Hide ( i )
	 end
	end
   end
   if g_EntityExtra[i]['spawnatstart'] ~= nil then 
    SetEntitySpawnAtStart ( i, g_EntityExtra[i]['spawnatstart'] )
    if g_EntityExtra[i]['spawnatstart']==2 and g_Entity[i]['health'] > 0 then
     Spawn ( i )
    end
   end  
   -- Trigger lights refresh to last known active state
   if g_module_lightcontrol ~= nil then
	if g_module_lightcontrol[i] ~= nil then
	 if g_module_lightcontrol[i]['activestate'] ~= nil then
	  g_module_lightcontrol[i]['initialstate'] = g_module_lightcontrol[i]['activestate']
	 end
    end
   end
   -- Custom/New Scripts too numerous to anticipate restoration sequence - give job to script!
   g_EntityExtra[i]['restoremenow'] = 1
  end
  end
 end
 -- restore all level containers (first two are always players main and hotkeys)
 if g_UserContainerTotal ~= nil then
	DeleteAllInventoryContainers()
	local invtotal = g_UserContainerTotal
	for c = 0, invtotal-1, 1 do
		inventorycontainer = g_UserContainerName[c]
		MakeInventoryContainer(inventorycontainer)
		-- main and hotkeys cannot use E as they are level-specific, so use collectionindex and spawning
		local invindex = 1
		if c == 1 then invindex = 2 end
		if c  > 1 then invindex = 3 end
		if c == 0 or c == 1 then
			-- player specific, use collection index
			local tinventoryqty = g_UserContainerCount[c]
			for tinventoryindex = 1, tinventoryqty, 1 do
				local fulloffset = (c*100000)+tinventoryindex
				if g_UserContainerIndex[fulloffset] ~= nil then
					local tcollectione = g_UserContainerE[fulloffset]
					local qty = g_UserContainerQty[fulloffset]
					local slot = g_UserContainerSlot[fulloffset]
					if tcollectione > 0 then
						-- entity known
						SetEntityCollectedForce(tcollectione,invindex,slot)
						SetEntityQuantity(tcollectione,qty)
					else
						-- entity not known
						local tcollectionindex = g_UserContainerIndex[fulloffset]
						local tname = GetCollectionItemAttribute(tcollectionindex,"title")
						local anyee = 0
						for ee = 1, g_EntityElementMax, 1 do
							if g_Entity[ee] ~= nil then
								if g_Entity[ee]['active'] > 0 then
									if GetEntityName(ee) == tname then
										anyee = ee
										break
									end
								end
							end
						end
						if anyee > 0 then
							-- can use thisd spawned one as parent loaded in the level
							local newe = SpawnNewEntity(anyee)
							SetEntityCollectedForce(newe,invindex,slot)
							SetEntityQuantity(newe,qty)
						else
							-- is not yet spawned or existing, so include in inventory via tcollectionindex
							SetEntityCollectedForce(0,invindex,slot,inventorycontainer,tcollectionindex)
						end
					end
				end
			end
			-- also check if player start weapon in hotkeys - only scenario where entity zero is okay
			if c == 1 then
				local tinventoryqty = g_UserContainerCount[c]
				for tinventoryindex = 1, tinventoryqty, 1 do
					local fulloffset = (c*100000)+tinventoryindex
					if g_UserContainerIndex[fulloffset] ~= nil then
						local tcollectionindex = g_UserContainerIndex[fulloffset]
						local tcollectione = g_UserContainerE[fulloffset]
						if tcollectionindex > 0 and tcollectione == 0 then
							local slot = g_UserContainerSlot[fulloffset]
							AddInventoryItem("inventory:hotkeys",tcollectionindex,0,slot)
						end
					end
				end
			end
		else
			-- level specific (maybe use E in the future)
			local tinventoryqty = g_UserContainerCount[c]
			for tinventoryindex = 1, tinventoryqty, 1 do
				local fulloffset = (c*100000)+tinventoryindex
				if g_UserContainerIndex[fulloffset] ~= nil then
					local tcollectione = g_UserContainerE[fulloffset]
					local slot = g_UserContainerSlot[fulloffset]
					local qty = g_UserContainerQty[fulloffset]
					--if tcollectione > 0 then
					--	-- entity known
					--	SetEntityCollectedForce(tcollectione,invindex,slot,inventorycontainer)
					--	SetEntityQuantity(tcollectione,qty)
					--else
					-- entity not known
					local tcollectionindex = g_UserContainerIndex[fulloffset]
					--local tname = GetCollectionItemAttribute(tcollectionindex,"title")
					SetEntityCollectedForce(tcollectione,3,slot,inventorycontainer,tcollectionindex,qty)
					--end
				end
			end
		end
	end
 end
end

return restoregame
