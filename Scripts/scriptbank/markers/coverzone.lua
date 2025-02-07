-- Cover Zone v1 by Lee
-- DESCRIPTION: Cover zone for NPCs to find and use.
-- DESCRIPTION: [SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this zone

g_coverzone = {}

function coverzone_properties(e, spawnatstart)
	g_coverzone[e].spawnatstart = spawnatstart or 1
end

function coverzone_init(e)
	g_coverzone[e] = {}
	g_coverzone[e].spawnatstart = 1
	g_coverzone[e].status = "init"
end

function coverzone_main(e)
	if g_coverzone[e].status == "init" then
		if g_coverzone[e].spawnatstart == 1 then
			g_coverzone[e].status = "normal"
		else
			if g_Entity[e]['activated'] ~= 0 then 
				g_coverzone[e].status = "normal"
			end
		end
	end
	if g_coverzone[e].status == "normal" then
		-- cover zone welcomes visitors
	end
	if g_EntityExtra[e]['restoremenow'] ~= nil then
     if g_EntityExtra[e]['restoremenow'] == 1 then
      g_EntityExtra[e]['restoremenow'] = 0
	  g_coverzone[e].status = "init"
     end
	end	
end
