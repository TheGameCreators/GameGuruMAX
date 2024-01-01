-- ActiveHud v2   by Necrym59
-- DESCRIPTION: Makes a hidden Game Hud active based on certain condition.
-- DESCRIPTION: Attach to an object and set AlwaysActive = ON
-- DESCRIPTION: [@SHOW_CONDITION=1(1=Home Key, 2=Health Damage, 3=Shooting, 4=Gun Zoom)]
-- DESCRIPTION: [SHOW_TIME=8(1,30)] Seconds

local activehud			= {}
local show_condition	= {}
local show_time			= {}
	
local wait				= {}
local doonce			= {}
local curhealth			= {}
local status			= {}
	
	
function activehud_properties(e, show_condition, show_time)
	activehud[e] = g_Entity[e]
	activehud[e].show_condition = show_condition
	activehud[e].show_time = show_time
end 

function activehud_init(e)
	activehud[e] = {}
	activehud[e].show_condition = 1
	activehud[e].show_time = 8
	wait[e] = math.huge
	doonce[e] = 0
	status[e] = "init"
end

function activehud_main(e)
	activehud[e] = g_Entity[e]
	if status[e] == "init" then
		curhealth[e] = g_PlayerHealth
		HideHuds()		
		status[e] = "endinit"
	end
	
	if activehud[e].show_condition == 1 then
		if g_Scancode == 199 then		
			ShowHuds()
			wait[e] = g_Time + (activehud[e].show_time*1000)
		end
	end
	
	if activehud[e].show_condition == 2 then
		if g_PlayerHealth < curhealth[e] and doonce[e] == 0 then
			ShowHuds()
			curhealth[e] = g_PlayerHealth
			wait[e] = g_Time + (activehud[e].show_time*1000)
			doonce[e] = 1
		end
	end
	
	if activehud[e].show_condition == 3 then
		if GetGamePlayerStateFiringMode() == 1 and doonce[e] == 0 then
			ShowHuds()
			wait[e] = g_Time + (activehud[e].show_time*1000)
			doonce[e] = 1
		end
	end
	
	if activehud[e].show_condition == 4 then
		if GetGamePlayerStateGunZoomMode() > 0 and doonce[e] == 0 then
			ShowHuds()
			wait[e] = g_Time + (activehud[e].show_time*1000)
			doonce[e] = 1
		end
	end
	
	if g_Time > wait[e] then
		HideHuds()
		doonce[e] = 0
	end
end
