-- DESCRIPTION: Assign to a weapon object to be collected

weapon_therecanbeonlyone = 0
weapon_temprompttimer = 0

function weapon_init_name(e,name)
	weapon_name[e] = name
end

function weapon_main(e)
 if weapon_therecanbeonlyone==-1 then
	if g_KeyPressE == 0 and g_InKey == ""  then
		weapon_therecanbeonlyone = 0
	end
 end
 PlayerDist = GetPlayerDistance(e)
 if PlayerDist < 75 and g_PlayerHealth > 0 and g_PlayerThirdPerson==0 then

    local LookingAt = GetPlrLookingAtEx(e,1)

	if LookingAt == 1 and weapon_therecanbeonlyone==0 then
		weapon_therecanbeonlyone = e
	end
	if LookingAt == 0 and weapon_therecanbeonlyone==e then
		weapon_therecanbeonlyone = 0
	end
	
	if LookingAt == 1 and weapon_therecanbeonlyone==e then
		--with inventory you can collect as many weapons as you like
		if weapon_temprompttimer > 0 then
			Prompt("Cannot collect the " .. weapon_name[e] .. " into preferred slot")
			if Timer() > weapon_temprompttimer + 3000 then weapon_temprompttimer = 0 end
		else
			if g_PlayerGunID > 0 then
				if g_PlayerController==0 then
					if GetHeadTracker() == 1 then
						Prompt("Right trigger to pick up the " .. weapon_name[e] )
					else
						Prompt("Press E to pick up the " .. weapon_name[e] .. " or T to replace" )
					end
				else
					Prompt("Press Y Button to pick up the " .. weapon_name[e] )
				end
			else
				if g_PlayerController==0 then
					if GetHeadTracker() == 1 then
						Prompt("Right trigger to pick up the " .. weapon_name[e] )
					else
						Prompt("Press E to pick up the " .. weapon_name[e] )
					end
				else
					Prompt("Press Y Button to pick up the " .. weapon_name[e] )
				end
			end
			if g_KeyPressE == 1 then
			
				--Collect into hotkeys inventory
				local collectedweapon = 1
				local preferredslot = -1
				local weaponID = GetEntityWeaponID(e)
				for slot = 1, 10, 1 do
					local thisWeaponPref = GetWeaponSlotPref(slot)
					if thisWeaponPref == weaponID then
						preferredslot = slot-1
						break
					end
				end
				if preferredslot > -1 then
				   local suggestedslot = SetEntityCollected(e,2,preferredslot)
				   if preferredslot == suggestedslot then
						-- must go in preferred slot
						AddPlayerWeaponSuggestSlot(e,1+preferredslot)
				   else
						-- or cannot collect (something in way)
						weapon_temprompttimer = Timer()
						collectedweapon = 0
				   end
				else
				   local suggestedslot = SetEntityCollected(e,2,preferredslot)
				   if suggestedslot >= 0 then
					-- can add to weapon slots
					AddPlayerWeaponSuggestSlot(e,1+suggestedslot)
				   else
					-- else add to general inventory
					SetEntityCollected(e,1,-1)
				   end
				end
				if collectedweapon == 1 then
					PlaySound(e,0)
					PerformLogicConnections(e)
					ActivateIfUsed(e)
				end
				weapon_therecanbeonlyone = -1
			end
		end
   end
 else
	if weapon_therecanbeonlyone==e then
		weapon_therecanbeonlyone = 0
	end
 end
end
