-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Envirozone v6 by Necrym59
-- DESCRIPTION: The player will be effected with the selected environmental mode while in this Zone
-- DESCRIPTION: [PROMPT_TEXT$="In Environment Effected Zone"]
-- DESCRIPTION: [@EFFECT=1(1=Adjust Walk/Run, 2=Health Loss, 3=Hurt, 4=No Weapons, 5=No Jumping, 6=Sniper Hit, 7=Alert Nearby Enemies , 8=No Flashlight)]
-- DESCRIPTION: [#WALK_RUN_ADJUST=8.0(0.0,20.0)] 
-- DESCRIPTION: [ZONEHEIGHT=100(0,1000)]
-- DESCRIPTION: [SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this zone
-- DESCRIPTION: <Sound0> - Effect Sound

local envirozone 		= {}
local prompt_text 		= {}
local effect 			= {}
local walk_run_adjust	= {}
local zoneheight		= {}
local SpawnAtStart		= {}
local gunsoff			= {}
local last_gun     		= {}
local max_jump     		= {}
local random_no			= {}
local played			= {}
local status			= {}

function envirozone_properties(e, prompt_text, effect, walk_run_adjust, zoneheight, SpawnAtStart)
	envirozone[e].prompt_text = prompt_text
	envirozone[e].effect = effect
	envirozone[e].walk_run_adjust = walk_run_adjust
	envirozone[e].zoneheight = zoneheight or 100
	envirozone[e].SpawnAtStart = SpawnAtStart
end

function envirozone_init(e)
	envirozone[e] = {}
	envirozone[e].prompt_text = "In Environment Effected Zone"
	envirozone[e].effect = 1
	envirozone[e].walk_run_adjust = 8
	envirozone[e].zoneheight = 100
	envirozone[e].SpawnAtStart = 1
	gunsoff[e]	= 0
	last_gun[e] = g_PlayerGunName
	max_jump[e] = GetGamePlayerControlJumpmax()
	StartTimer(e)
	played[e] = 0
	status[e] = "init"
end

function envirozone_main(e)

	if status[e] == "init" then
		if envirozone[e].SpawnAtStart == 1 then	SetActivated(e,1) end
		if envirozone[e].SpawnAtStart == 0 then SetActivated(e,0) end
		status[e] = "endinit"
	end

	if g_Entity[e]['activated'] == 1 then
		if g_Entity[e]['plrinzone'] == 1 and g_PlayerHealth > 0 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+envirozone[e].zoneheight then

			if envirozone[e].effect == 1 then	--Inhibited Walk/Run
				Prompt(envirozone[e].prompt_text)
				SetGamePlayerControlSpeed(GetGamePlayerControlSpeed()/envirozone[e].walk_run_adjust)
			end
			if envirozone[e].effect == 2 then	--Health Loss
				Prompt(envirozone[e].prompt_text)
				if GetTimer(e) > 1000 then
					PlaySound(e,0)
					SetPlayerHealth(g_PlayerHealth-1)
					StartTimer(e)
				end
			end
			if envirozone[e].effect == 3 then	--Hurt
				Prompt(envirozone[e].prompt_text)
				if GetTimer(e) > 1000 then
					PlaySound(e,0)
					HurtPlayer(-1,1)
					StartTimer(e)
				end
			end
			if envirozone[e].effect == 4 then	--No Weapons
				Prompt(envirozone[e].prompt_text)
				if gunsoff[e] == 0 then
					last_gun[e] = g_PlayerGunName
					SetPlayerWeapons(0)
					gunsoff[e] = 1
				end
			end
			if envirozone[e].effect == 5 then	--No Jumping
				Prompt(envirozone[e].prompt_text)
				SetGamePlayerControlJumpmax(0)
			end
			if envirozone[e].effect == 6 then	--Sniper Hit
				Prompt(envirozone[e].prompt_text)
				random_no = math.random(1,597) --Random number
				if random_no==2 or random_no==7 then
					PlaySound(e,0)
					HurtPlayer(e,1)
				end
			end
			if envirozone[e].effect == 7 then	--Alert nearby
				Prompt(envirozone[e].prompt_text)
				MakeAISound(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,2000,1,-1)
				if played[e] == 0 then
					PlaySound(e,0)
					played[e] = 1
				end
			end
			if envirozone[e].effect == 8 then	--No Flashlight
				Prompt(envirozone[e].prompt_text)
				SetFlashLight(0)
				SetFlashLightKeyEnabled(0)
				if played[e] == 0 then
					PlaySound(e,0)
					played[e] = 1
				end
			end
		end

		if g_Entity[e]['plrinzone'] == 0 then
			if gunsoff[e] == 1 then
				ChangePlayerWeapon(last_gun[e])
				SetPlayerWeapons(1)
				gunsoff[e] = 0
			end
			SetGamePlayerControlJumpmax(max_jump[e])
			SetFlashLightKeyEnabled(1)
			StopSound(e,0)
			played[e] = 0
		end
	end
end

function envirozone_exit(e)
end

