-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Pulse Control v6 by Necrym59
-- DESCRIPTION: Modifies the emmissiveness of an object when is Set to ON or activated by zone or switch.
-- DESCRIPTION: Attach this behavior to an object and set Physics ON/OFF, Always active ON.
-- DESCRIPTION: Set the behavior to ON. Or set to OFF if you wish to use a trigger zone or switch and logic link to the object for activation.
-- DESCRIPTION: Change [PROMPT_TEXT$="Strange lights"]
-- DESCRIPTION: Set the[@MODE=1(1=Increase Emission, 2=Decrease Emission, 3=Pulse Emission)]
-- DESCRIPTION: and the [LEVEL=500(1,1500)], and [#SPEED=5.50(0,50)],
-- DESCRIPTION: [@SET=1(1=Off, 2=On)]
-- DESCRIPTION: [COLOR_CHANGE!=0]
-- DESCRIPTION: Change emmisive [COLOR_R=0(0,255)], [COLOR_G=0(0,255)],[COLOR_B=0(0,255)]
-- DESCRIPTION: <Sound0> for activation
-- DESCRIPTION: <Sound1> for pulsing loop

local pulsecontrol	= {}
local prompt_text	= {}
local mode 		 	= {}
local level		 	= {}
local speed 	 	= {}
local set 	 		= {}

local emR 	 		= {}
local emG 	 		= {}
local emB 	 		= {}
local emStrength	= {}
local current_level = {}
local status 		= {}
local doonce 		= {}
local colchange		= {}
local empulse 		= {}	

function pulsecontrol_properties(e, prompt_text, mode, level, speed, set, color_change, color_r, color_g, color_b)
	pulsecontrol[e] = g_Entity[e]
	pulsecontrol[e].prompt_text 	= prompt_text
	pulsecontrol[e].mode			= mode
	pulsecontrol[e].level 			= level
	pulsecontrol[e].speed 			= speed	
	pulsecontrol[e].set 			= set or 1
	pulsecontrol[e].color_change	= color_change or 0	
	pulsecontrol[e].color_r			= color_r
	pulsecontrol[e].color_g			= color_g
	pulsecontrol[e].color_b			= color_b	
end 

function pulsecontrol_init(e)
	pulsecontrol[e] = g_Entity[e]
	pulsecontrol[e].prompt_text 	= ""
	pulsecontrol[e].mode			= 1
	pulsecontrol[e].level 			= 80
	pulsecontrol[e].speed			= 1
	pulsecontrol[e].set				= 1
	pulsecontrol[e].color_change	= 0
	pulsecontrol[e].color_r			= 0
	pulsecontrol[e].color_g			= 0
	pulsecontrol[e].color_b			= 0
	status[e] = "init"
	doonce[e] = 0
	colchange[e] = 0	
	emR, emG, emB = GetEntityEmissiveColor(e)
	emStrength = GetEntityEmissiveStrength(e)
	current_level[e] = emStrength
	empulse[e] = 0
end

function pulsecontrol_main(e)
	pulsecontrol[e] = g_Entity[e]
	
	if status[e] == "init" then		
		status[e] = "end"
	end
	if pulsecontrol[e].set == 1 then g_Entity[e]['activated'] = 0 end
	if pulsecontrol[e].set == 2 and pulsecontrol[e].mode == 3 then g_Entity[e]['activated'] = 1 end
	
	if g_Entity[e]['activated'] == 1 then
		if doonce[e] == 0 then
			Prompt(pulsecontrol[e].prompt_text)		
			PlaySound(e,0)
			doonce[e] = 1
		end
		if pulsecontrol[e].mode == 1 then -- Increase Emission
			if current_level[e] <= pulsecontrol[e].level then				
				SetEntityEmissiveStrength(e,current_level[e])				
				current_level[e] = current_level[e] + pulsecontrol[e].speed
			end
		end
		if pulsecontrol[e].mode == 2 then -- Decrease Emission
			if current_level[e] >= pulsecontrol[e].level then				
				SetEntityEmissiveStrength(e,current_level[e])
				current_level[e] = current_level[e] - pulsecontrol[e].speed
			end	
		end
		if pulsecontrol[e].mode == 3 then -- Pulse Emission
			LoopSound(e,1)
			if empulse[e] == 0 then
				if current_level[e] <= pulsecontrol[e].level then				
					SetEntityEmissiveStrength(e,current_level[e])
					current_level[e] = current_level[e] + pulsecontrol[e].speed
					if current_level[e] >= pulsecontrol[e].level then
						empulse[e] = 1
						if colchange[e] == 0 then colchange[e] = 1 end
						if colchange[e] == 2 then colchange[e] = 0 end
					end
				end
			end
			if empulse[e] == 1 then
				if current_level[e] >= emStrength then				
					SetEntityEmissiveStrength(e,current_level[e])
					current_level[e] = current_level[e] - pulsecontrol[e].speed
					if current_level[e] <= emStrength then
						empulse[e] = 0
						if pulsecontrol[e].color_change	== 1 then
							if colchange[e] == 1 then
								SetEntityEmissiveColor(e,pulsecontrol[e].color_r,pulsecontrol[e].color_g,pulsecontrol[e].color_b)
								colchange[e] = 2
							end
							if colchange[e] == 0 then
								SetEntityEmissiveColor(e,emR, emG, emB)
								colchange[e] = 0
							end
						end
					end
				end
			end
		end
	end
	if g_Entity[e]['activated'] == 0 then
		StopSound(e,0)
		StopSound(e,1)
		doonce[e] = 0
		SetEntityEmissiveStrength(e,emStrength)
	end
end
