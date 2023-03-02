-- DESCRIPTION: If any object enters zone, activate <If Used> for the 'THE OBJECT THAT ENTERED THE ZONE'. Sets <If Used> activated to [IFACTIVATED=1] and 'entered object' activated to [EEACTIVATED=1]. Also plays <Sound0> when triggered.

g_entityinzone = {}

function entityinzone_properties(e, ifactivated, eeactivated, speech)
	g_entityinzone[e]['ifactivated'] = ifactivated
	g_entityinzone[e]['eeactivated'] = eeactivated
	g_entityinzone[e]['speech'] = speech
end 

function entityinzone_init(e)
	g_entityinzone[e] = {}
	g_entityinzone[e]['ifactivated'] = 1
	g_entityinzone[e]['eeactivated'] = 1
	g_entityinzone[e]['speech'] = ""
end

function entityinzone_main(e)
 GetEntityInZone(e)
 local EntityID = g_Entity[e]['entityinzone']
 if EntityID > 0 then
  if g_Entity[e]['activated'] == 0 then
   PlaySound(e,0)
   ActivateIfUsed(e)
   SetActivated(e,ifactivated)
  end
  SetActivated(EntityID,eeactivated)
 end
end
