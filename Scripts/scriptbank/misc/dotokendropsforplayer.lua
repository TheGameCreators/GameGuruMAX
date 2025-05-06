-- Do Token Drop v1 - Lee
-- DESCRIPTION: Creates a drop token every 5 seconds where the player stands

g_dotokendropsforplayer = 0

function dotokendropsforplayer_init(e)
	g_dotokendropsforplayer = 0
end

function dotokendropsforplayer_main(e)
	SetEntityAlwaysActive(e,1)
	if g_dotokendropsforplayer == 0 then
		g_dotokendropsforplayer = 1
		StartTimer(e)
	end
	if g_dotokendropsforplayer == 1 then
		if GetTimer(e) > 100 then
			local surfaceY = RDGetYFromMeshPosition(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ)
			DoTokenDrop(g_PlayerPosX,surfaceY,g_PlayerPosZ,1,5000)
			g_dotokendropsforplayer = 0
		end
	end
end

