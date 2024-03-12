-- Textinzone v4  
-- DESCRIPTION: While the player is within the zone the [PROMPTTEXT$=""] is displayed on screen. 
-- DESCRIPTION: [ZONEHEIGHT=100] controls how far above the zone the player can be before the zone is not triggered.
-- DESCRIPTION: Set[!ONLYONCE=0] for the text zone to be destroyed after activation.
-- DESCRIPTION: [SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this zone
-- DESCRIPTION: [PERIOD=1] time in seconds to display prompt

local textinzone = {}

function textinzone_properties(e, prompttext, zoneheight, onlyonce, spawnatstart, period)
	local tiz = textinzone[e]
	tiz.prompttext   = prompttext   or ""
	tiz.zoneheight   = zoneheight   or 100
	tiz.onlyonce     = onlyonce == 1
	tiz.spawnatstart = spawnatstart or 1
	tiz.period = period * 1000
end 

function textinzone_init(e)
	textinzone[e] = 
	    { prompttext	= "",
	      zoneheight	= 100,
          onlyonce		= false,
	      spawnatstart	= 1,
	      period		= 1000,
		  status		= 'init'
		}
end

function textinzone_main(e)
	local tiz = textinzone[e]
	if tiz == nil then return end
	
	if tiz.status == "init" then
	    SetActivated(e, tiz.spawnatstart)
		tiz.status = 'ready'	
	elseif
	   tiz.status == 'ready' then
		local Ent = g_Entity[e]
		if Ent == nil then return end
		if Ent.activated == 0 then return end
		if Ent.plrinzone == 1 and g_PlayerPosY > Ent.y and g_PlayerPosY < Ent.y + tiz.zoneheight then
			tiz.wait = g_Time + tiz.period
			PerformLogicConnections(e)
			tiz.status = 'show'
		end
	
	elseif
	   tiz.status == 'show' then
		if g_Time < tiz.wait then
			Prompt(tiz.prompttext)
		elseif tiz.onlyonce then
			tiz.status = 'finish'
			Destroy(e)
		else
			tiz.status = 'inzone'
		end
		
	elseif
		tiz.status == 'inzone' then
		local Ent = g_Entity[e]
		if Ent == nil then return end	   
		if Ent.plrinzone == 0 then tiz.status = 'ready' end
	end
end
