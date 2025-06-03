-- Cover Zone v2 by Lee and Necrym59
-- DESCRIPTION: Cover zone for NPCs to find and use.
-- DESCRIPTION: [SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this zone
-- DESCRIPTION: [@CoverType=1(1=Standing, 2=Crouched, 3=Prone)]

g_coverzone 		= {}
local spawnatstart 	= {}
local covertype		= {}
local status 		= {}

function coverzone_properties(e, spawnatstart, covertype)
	g_coverzone[e].spawnatstart = spawnatstart or 1
	g_coverzone[e].covertype = covertype or 1
end

function coverzone_init(e)
	g_coverzone[e] = {}
	g_coverzone[e].spawnatstart = 1
	g_coverzone[e].covertype = 1
	g_coverzone[e].status = "init"
end

function coverzone_main(e)
	if g_coverzone[e].status == "init" then
		if g_coverzone[e].spawnatstart == 1 then
			SetActivated(e,1)
			g_coverzone[e].status = "normal"
		end
		if g_coverzone[e].spawnatstart == 0 then
			SetActivated(e,0)
			g_coverzone[e].status = "deactivated"
		end
	end

	if g_Entity[e]['activated'] == 1 then g_coverzone[e].status = "normal" end
	if g_Entity[e]['activated'] == 0 then g_coverzone[e].status = "deactivated" end
	
	if g_coverzone[e].status == "normal" then
		-- cover zone welcomes visitors
		--end
	end	
		
	if g_EntityExtra[e]['restoremenow'] ~= nil then
		if g_EntityExtra[e]['restoremenow'] == 1 then
			g_EntityExtra[e]['restoremenow'] = 0
			g_coverzone[e].status = "init"
		end
	end	
end