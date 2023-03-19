-- Resolutions module

local restoregame = {}

g_restoregame_mode = 0

function restoregame.mode(mode)
 g_restoregame_mode = mode
end

function restoregame.now()
 -- trigger entire game to restore to saved game state
 if g_restoregame_mode == 0 then
  SetFreezePosition(iPlayerPosX,iPlayerPosY,iPlayerPosZ)
  SetFreezeAngle(iPlayerAngX,iPlayerAngY,iPlayerAngZ)
  TransportToFreezePosition()
 end
 SetPlayerHealth(iPlayerHealth)
 SetPlayerLives(iPlayerLives)
 -- restore weapon stats
 ResetWeaponSystems()
 ChangePlayerWeapon(strPlayerGunName)
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
 -- restore entity stats
 for i = 1, g_EntityElementMax, 1 do
  if g_Entity[i] ~= nil then
   ResetPosition ( i, g_Entity[i]['x'], g_Entity[i]['y'], g_Entity[i]['z'] )
   ResetRotation ( i, g_Entity[i]['anglex'], g_Entity[i]['angley'], g_Entity[i]['anglez'] )
   SetEntityActive ( i, g_Entity[i]['active'] )
   SetEntityActivated ( i, g_Entity[i]['activated'] )
   SetEntityCollected ( i, math.abs(g_Entity[i]['collected'])*-1 )
   SetEntityHasKey ( i, g_Entity[i]['haskey'] )
   SetEntityHealth ( i, g_Entity[i]['health'] )
   RefreshEntity ( i )
   SetAnimationFrame ( i, g_Entity[i]['frame'] )
   if g_EntityExtra[i]['visible']==1 then
    Show ( i )
   else
    Hide ( i )
   end
   SetEntitySpawnAtStart ( i, g_EntityExtra[i]['spawnatstart'] )
   if g_EntityExtra[i]['spawnatstart']==2 and g_Entity[i]['health'] > 0 then
    Spawn ( i )
   end
  end
 end 
 -- restore all level containers (first two are always players main and hotkeys)
 if g_UserContainerTotal ~= nil then
	DeleteAllInventoryContainers()
	local invtotal = g_UserContainerTotal
	--if invtotal > 1 then invtotal = 2 end --for now just the players to get it working
	for c = 0, invtotal-1, 1 do
		inventorycontainer = g_UserContainerName[c]
		MakeInventoryContainer(inventorycontainer)
		local tinventoryqty = g_UserContainerCount[c]
		for tinventoryindex = 1, tinventoryqty, 1 do
			local fulloffset = (c*100000)+tinventoryindex
			if g_UserContainerIndex[fulloffset] ~= nil then
				local tcollectionindex = g_UserContainerIndex[fulloffset]
				local qty = g_UserContainerQty[fulloffset]
				local slot = g_UserContainerSlot[fulloffset]
				local anyee = 0
				local tname = GetCollectionItemAttribute(tcollectionindex,"title")
				for ee = 1, g_EntityElementMax, 1 do
					if e ~= ee then
						if g_Entity[ee] ~= nil then
							if g_Entity[ee]['active'] > 0 then
								if GetEntityName(ee) == tname then
									anyee = ee
									break
								end
							end
						end
					end
				end
				if anyee > 0 then
					local newe = SpawnNewEntity(anyee)
					if c == 0 then invindex = 1 end
					if c == 1 then invindex = 2 end
					if c >= 2 then invindex = 3 end
					if invindex > 2 then
						SetEntityCollected(newe,invindex,slot,inventorycontainer)
					else
						SetEntityCollected(newe,invindex,slot)
					end
				end
			end
		end
	end
 end
end

return restoregame
