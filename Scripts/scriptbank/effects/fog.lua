-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Fog v6 by Necrym59
-- DESCRIPTION: When activated will create incoming fog to the required settings or recede to default fog setting in the fog panel.
-- DESCRIPTION: Add to an object then link to a switch or multi-trigger zone to activate/de-activate.
-- DESCRIPTION: [PROMPT_TEXT$="Fog strength is changing"]
-- DESCRIPTION: [FOG_NEAREST=1(0,1000)] [FOG_DISTANCE=5(0,1000)] [FOG_SPEED=10(1,100)]

	local fog 				= {}
	local prompt_text 		= {}
	local fog_nearest		= {}
	local fog_distance		= {}
	local fog_speed			= {}

	local status 			= {}
	local default_fogn		= {}
	local default_fogd		= {}	
	local fognear			= {}
	local fogdist			= {}
	local fbspeed			= {}
	local fogswitch			= {}
	local doonce			= {}	
	local played			= {}

function fog_properties(e, prompt_text, fog_nearest, fog_distance, fog_speed)
	fog[e].prompt_text = prompt_text
	fog[e].fog_nearest = fog_nearest
	fog[e].fog_distance	= fog_distance
	fog[e].fog_speed = fog_speed
end

function fog_init(e)
	fog[e] = {}
	fog[e].prompt_text = "Fog strength is changing"
	fog[e].fog_nearest = 1
	fog[e].fog_distance	= 5
	fog[e].fog_speed = 10
	
	fognear[e] = 0
	fogdist[e] = 0
	fbspeed[e] = fog[e].fog_speed
	fogswitch[e] = 0
	doonce[e] = 0
	status[e] = "init"
	played[e] = 0
end

function fog_main(e)

	if status[e] == "init" then
		fogswitch[e] = 0
		default_fogn[e]	= GetFogNearest()
		default_fogd[e]	= GetFogDistance()
		fognear[e] = (default_fogn[e])
		fogdist[e] = (default_fogd[e])
		fbspeed[e] = fog[e].fog_speed
		if fogdist[e] > fog[e].fog_distance then fogswitch[e] = 0 end
		if fogdist[e] < default_fogd[e] then fogswitch[e] = 1 end
		SetActivated(e,0)
		status[e] = "endinit"
	end
	
	if g_Entity[e]['activated'] == 1 then		

		if fogswitch[e] == 0 then
			Prompt(fog[e].prompt_text)			
			if fognear[e] > fog[e].fog_nearest then
				fognear[e] = fognear[e] - fbspeed[e]
				SetFogNearest(fognear[e])
				if fognear[e] <= fog[e].fog_nearest*100 then fognear[e] = fog[e].fog_nearest*100 end
			end
			if fogdist[e] > fog[e].fog_distance then
				fogdist[e] = fogdist[e] - fbspeed[e]*10
				SetFogDistance(fogdist[e])
				if fogdist[e] <= fog[e].fog_distance*100 then fogdist[e] = fog[e].fog_distance*100 end
			end
			
			if fognear[e] == fog[e].fog_nearest*100 and fogdist[e] == fog[e].fog_distance*100 then
				fogswitch[e] = 1
				SetActivated(e,0)
			end
		end

		if fogswitch[e] == 1 then
			Prompt(fog[e].prompt_text)
			if fognear[e] < default_fogn[e] then
				fognear[e] = fognear[e] + fbspeed[e]/10
				SetFogNearest(fognear[e])
				if fognear[e] >= default_fogn[e] then fognear[e] = default_fogn[e] end
			end
			if fogdist[e] < default_fogd[e] then
				fogdist[e] = fogdist[e] + fbspeed[e]*10
				SetFogDistance(fogdist[e])
				if fogdist[e] >= default_fogd[e] then fogdist[e] = default_fogd[e] end
			end
			
			if fognear[e] == default_fogn[e] and fogdist[e] == default_fogd[e] then
				fogswitch[e] = 0
				SetActivated(e,0)
			end
		end
	end
end
