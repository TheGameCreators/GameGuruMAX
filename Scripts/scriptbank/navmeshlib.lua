-- Navmesh module for GameGuru MAX
-- Written by Lee

local NAVMESH = {}

function NAVMESH.HandleBlocker ( e, blockingmode, originalx, originaly, originalz )
	-- navmesh blocker system (account for bounding size and rotation of entity)
	if blockingmode ~= 0 then
		local blockmode = 0
		if blockingmode == 1 then blockmode = 1 end
		if blockingmode == 2 then blockmode = 0 end
		local minx,miny,minz,maxx,maxy,maxz = GetEntityColBox(e)
		local sizex = math.abs(maxx-minx)
		local sizez = math.abs(maxz-minz)
		colx,coly,colz = GetObjectColCentre( g_Entity[e]['obj'] )
		originalx = originalx + colx
		originalz = originalz + colz
		RDBlockNavMeshWithShape(originalx,originaly,originalz,sizex,blockmode,sizez,g_Entity[e]['angley'])
		blockingmode = 0
	end
	return blockingmode
end

return NAVMESH
