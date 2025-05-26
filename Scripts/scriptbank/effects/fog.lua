-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Fog v8 by Necrym59
-- DESCRIPTION: When activated will change fog to the required settings or return to default fog setting in the weather panel.
-- DESCRIPTION: Add to an object then link to a switch or multi-trigger zone to activate/de-activate.
-- DESCRIPTION: [PROMPT_TEXT$="Fog strength is changing"]
-- DESCRIPTION: [FOG_NEAREST=1(0,1000)]
-- DESCRIPTION: [FOG_DISTANCE=5(0,1000)]
-- DESCRIPTION: [FOG_SPEED=25(1,100)]
-- DESCRIPTION: [@FOG_COLOUR_CHANGE=1(1=Off, 2=On)]
-- DESCRIPTION: [FOG_RED=0(0,255)]
-- DESCRIPTION: [FOG_GREEN=0(0,255)]
-- DESCRIPTION: [FOG_BLUE=0(0,255)]

	local fog 				= {}
	local prompt_text 		= {}
	local fog_nearest		= {}
	local fog_distance		= {}
	local fog_speed			= {}
	local fog_colour_change	= {}
	local fog_red			= {}
	local fog_green			= {}
	local fog_blue			= {}	

	local status 			= {}
	local default_fogn		= {}
	local default_fogd		= {}	
	local default_fog_r		= {}
	local default_fog_g		= {}
	local default_fog_b		= {}
	local fognear			= {}
	local fogdist			= {}
	local fbspeed			= {}
	local fogswitch			= {}
	local doonce			= {}	
	local played			= {}
	local current_fog_r		= {}
	local current_fog_g		= {}
	local current_fog_b		= {}
	local fog_r_set			= {}
	local fog_g_set			= {}
	local fog_b_set			= {}
	local fog_rgbset		= {}

function fog_properties(e, prompt_text, fog_nearest, fog_distance, fog_speed, fog_colour_change, fog_red, fog_green, fog_blue)
	fog[e].prompt_text = prompt_text
	fog[e].fog_nearest = fog_nearest
	fog[e].fog_distance	= fog_distance
	fog[e].fog_speed = fog_speed
	fog[e].fog_colour_change = fog_colour_change or 1
	fog[e].fog_red = fog_red
	fog[e].fog_green = fog_green
	fog[e].fog_blue = fog_blue	
end

function fog_init(e)
	fog[e] = {}
	fog[e].prompt_text = "Fog strength is changing"
	fog[e].fog_nearest = 1
	fog[e].fog_distance	= 5
	fog[e].fog_speed = 10
	fog[e].fog_colour_change = 1
	fog[e].fog_red = GetFogRed()
	fog[e].fog_green = GetFogGreen()
	fog[e].fog_blue = GetFogBlue()
	
	fognear[e] = 0
	fogdist[e] = 0
	current_fog_r[e] = 0
	current_fog_g[e] = 0
	current_fog_b[e] = 0
	fog_r_set[e] = 0
	fog_g_set[e] = 0
	fog_b_set[e] = 0
	fog_rgbset[e] = 0
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
		default_fog_r[e] = GetFogRed()
		default_fog_g[e] = GetFogGreen()
		default_fog_b[e] = GetFogBlue()
		current_fog_r[e] = default_fog_r[e]
		current_fog_g[e] = default_fog_g[e]
		current_fog_b[e] = default_fog_b[e]	
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
			if fognear[e] < fog[e].fog_nearest then
				fognear[e] = fognear[e] + fbspeed[e]
				SetFogNearest(fognear[e])
				if fognear[e] >= fog[e].fog_nearest*100 then fognear[e] = fog[e].fog_nearest*100 end
			end
			if fogdist[e] > fog[e].fog_distance then
				fogdist[e] = fogdist[e] - fbspeed[e]
				SetFogDistance(fogdist[e])
				if fogdist[e] <= fog[e].fog_distance*100 then fogdist[e] = fog[e].fog_distance*100 end
			end
			if fogdist[e] < fog[e].fog_distance then
				fogdist[e] = fogdist[e] + fbspeed[e]
				SetFogDistance(fogdist[e])
				if fogdist[e] >= fog[e].fog_distance*100 then fogdist[e] = fog[e].fog_distance*100 end
			end
			
			if fog[e].fog_colour_change == 1 then fog_rgbset[e] = 3 end
			if fog[e].fog_colour_change == 2 then 				
				if current_fog_r[e] < fog[e].fog_red and fog_r_set[e] == 0 then
					SetFogRed(current_fog_r[e])
					current_fog_r[e] = current_fog_r[e] + fbspeed[e]/100
					if fog[e].fog_red <= current_fog_r[e] then
						current_fog_r[e] = fog[e].fog_red
						fog_r_set[e] = 1
					end
				end
				if current_fog_r[e] > fog[e].fog_red and fog_r_set[e] == 0 then
					SetFogRed(current_fog_r[e])
					current_fog_r[e] = current_fog_r[e] - fbspeed[e]/100
					if fog[e].fog_red >= current_fog_r[e] then
						fog_r_set[e] = 1
						current_fog_r[e] = fog[e].fog_red
					end
				end
				
				if current_fog_g[e] < fog[e].fog_green and fog_g_set[e] == 0 then
					SetFogGreen(current_fog_g[e])
					current_fog_g[e] = current_fog_g[e] + fbspeed[e]/100
					if fog[e].fog_green <= current_fog_g[e] then
						fog_g_set[e] = 1
						current_fog_g[e] = fog[e].fog_green
					end
				end
				if current_fog_g[e] > fog[e].fog_green and fog_g_set[e] == 0 then
					SetFogGreen(current_fog_g[e])
					current_fog_g[e] = current_fog_g[e] - fbspeed[e]/100
					if fog[e].fog_green >= current_fog_g[e] then
						fog_g_set[e] = 1
						current_fog_g[e] = fog[e].fog_green
					end
				end
				
				if current_fog_b[e] < fog[e].fog_blue and fog_b_set[e] == 0 then
					SetFogBlue(current_fog_b[e])
					current_fog_b[e] = current_fog_b[e] + fbspeed[e]/100
					if fog[e].fog_blue <= current_fog_b[e] then
						fog_b_set[e] = 1
						current_fog_b[e] = fog[e].fog_blue
					end
				end
				if current_fog_b[e] > fog[e].fog_blue and fog_b_set[e] == 0 then
					SetFogBlue(current_fog_b[e])
					current_fog_b[e] = current_fog_b[e] - fbspeed[e]/100
					if fog[e].fog_blue >= current_fog_b[e] then
						fog_b_set[e] = 1
						current_fog_b[e] = fog[e].fog_blue
					end
				end
						
				if current_fog_r[e] == fog[e].fog_red then fog_r_set[e] = 1 end
				if current_fog_g[e] == fog[e].fog_green then fog_g_set[e] = 1 end
				if current_fog_b[e] == fog[e].fog_blue then fog_b_set[e] = 1 end			
				fog_rgbset[e] = fog_r_set[e] + fog_g_set[e] + fog_b_set[e]
			end		
						
			if fognear[e] == fog[e].fog_nearest*100 and fogdist[e] == fog[e].fog_distance*100 and fog_rgbset[e] == 3 then
				fogswitch[e] = 1				
				fog_r_set[e] = 0
				fog_g_set[e] = 0
				fog_b_set[e] = 0
				fog_rgbset[e] = 0
				SetActivated(e,0)
			end
		end

		if fogswitch[e] == 1 then
			Prompt(fog[e].prompt_text)
			if fognear[e] < default_fogn[e] then
				fognear[e] = fognear[e] + fbspeed[e]
				SetFogNearest(fognear[e])
				if fognear[e] >= default_fogn[e] then fognear[e] = default_fogn[e] end
			end
			if fognear[e] > default_fogn[e] then
				fognear[e] = fognear[e] - fbspeed[e]
				SetFogNearest(fognear[e])
				if fognear[e] <= default_fogn[e] then fognear[e] = default_fogn[e] end
			end			
			if fogdist[e] < default_fogd[e] then
				fogdist[e] = fogdist[e] + fbspeed[e]
				SetFogDistance(fogdist[e])
				if fogdist[e] >= default_fogd[e] then fogdist[e] = default_fogd[e] end
			end
			if fogdist[e] > default_fogd[e] then
				fogdist[e] = fogdist[e] - fbspeed[e]
				SetFogDistance(fogdist[e])
				if fogdist[e] >= default_fogd[e] then fogdist[e] = default_fogd[e] end
			end
			
			if fog[e].fog_colour_change == 1 then fog_rgbset[e] = 3 end
			if fog[e].fog_colour_change == 2 then
			
				if current_fog_r[e] < default_fog_r[e] and fog_r_set[e] == 0 then
					SetFogRed(current_fog_r[e])
					current_fog_r[e] = current_fog_r[e] + 0.1
					if default_fog_r[e] <= current_fog_r[e] then					
						fog_r_set[e] = 1
						current_fog_r[e] = default_fog_r[e]
					end
				end
				if current_fog_r[e] > default_fog_r[e] and fog_r_set[e] == 0 then
					SetFogRed(current_fog_r[e])
					current_fog_r[e] = current_fog_r[e] - 0.1
					if default_fog_r[e] >= current_fog_r[e] then
						fog_r_set[e] = 1
						current_fog_r[e] = default_fog_r[e]
					end
				end
				
				if current_fog_g[e] < default_fog_g[e] and fog_g_set[e] == 0 then
					SetFogGreen(current_fog_g[e])
					current_fog_g[e] = current_fog_g[e] + 0.1
					if default_fog_g[e] <= current_fog_g[e] then
						fog_g_set[e] = 1
						current_fog_g[e] = default_fog_g[e]
					end
				end
				if current_fog_g[e] > default_fog_g[e] and fog_g_set[e] == 0 then
					SetFogGreen(current_fog_g[e])
					current_fog_g[e] = current_fog_g[e] - 0.2
					if default_fog_g[e] >= current_fog_g[e] then
						fog_g_set[e] = 1
						current_fog_g[e] = default_fog_g[e]
					end
				end
				
				if current_fog_b[e] < default_fog_b[e] and fog_b_set[e] == 0 then
					SetFogBlue(current_fog_b[e])
					current_fog_b[e] = current_fog_b[e] + 0.2
					if default_fog_b[e] <= current_fog_b[e] then
						fog_b_set[e] = 1
						current_fog_b[e] = default_fog_b[e]
					end
				end
				if current_fog_b[e] > default_fog_b[e] and fog_b_set[e] == 0 then
					SetFogBlue(current_fog_b[e])
					current_fog_b[e] = current_fog_b[e] - 0.2
					if default_fog_b[e] >= current_fog_b[e] then
						fog_b_set[e] = 1
						current_fog_b[e] = default_fog_b[e]
					end
				end
			
				if current_fog_r[e] == fog[e].fog_red then fog_r_set[e] = 1 end
				if current_fog_g[e] == fog[e].fog_green then fog_g_set[e] = 1 end
				if current_fog_b[e] == fog[e].fog_blue then fog_b_set[e] = 1 end			
				fog_rgbset[e] = fog_r_set[e] + fog_g_set[e] + fog_b_set[e]
			end
						
			if fognear[e] == default_fogn[e] and fogdist[e] == default_fogd[e] and fog_rgbset[e] == 3 then
				fogswitch[e] = 0
				fog_r_set[e] = 0
				fog_g_set[e] = 0
				fog_b_set[e] = 0
				fog_rgbset[e] = 0				
				SetActivated(e,0)
			end
		end
	end
end
