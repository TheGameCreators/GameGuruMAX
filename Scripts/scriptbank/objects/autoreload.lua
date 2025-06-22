-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Auto Reload v4 by Necrym59
-- DESCRIPTION: Allows the auto-reload of a weapon upon empty clip
-- DESCRIPTION: Attach to an object set Always Active ON
-- DESCRIPTION: [@MODE$=1(1=On, 2=Off, 3=Prompt)]
-- DESCRIPTION: [PROMPT$="Select Y or N for Weapon Auto-Reloading"]

g_autoReload		= {}
local autoreload	= {}
local prompt		= {}
local mode			= {}

local doonce		= {}
local status		= {}

function autoreload_properties(e, mode, prompt)
	autoreload[e].mode = mode
	autoreload[e].prompt = prompt
end

function autoreload_init(e)
	autoreload[e] = {}
	autoreload[e].mode = 1
	autoreload[e].prompt = "Select Y or N for Weapon Auto-Reloading"		
	
	doonce[e] = 0
	status[e] = "init"
end

function autoreload_main(e)

	if status[e] == "init" then		
		if autoreload[e].mode == 1 then	g_autoReload = 1 end
		if autoreload[e].mode == 2 then g_autoReload = 0 end
		if autoreload[e].mode == 3 then g_autoReload = nil end
		status[e] = "endinit"
	end
	
	if autoreload[e].mode == 3 and g_autoReload == nil then
		Prompt(autoreload[e].prompt)
		if GetInKey() == "Y" or GetInKey() == "y" then
			g_autoReload = 1
			PromptDuration("Weapon Auto-Reloading ON",1000)
		end
		if GetInKey() == "N" or GetInKey() == "n" then
			g_autoReload = 0
			PromptDuration("Weapon Auto-Reloading OFF",1000)
		end
	end		
	
	if g_autoReload == 1 then
		if g_PlayerGunAmmoCount == 0 then
			--Force Reload weapon
			SetGamePlayerStateFiringMode(2)
		end
	end
end

function autoreload_exit(e)
end