-- Corpse Fader v2 by Smallg modified by Necrym59
-- DESCRIPTION: Fades out entity corpses after a set delay. 
-- DESCRIPTION: Attach to an object and set Always Active On
-- DESCRIPTION: Set [#FADE_DELAY=5(0,100)] seconds after death of entity.
-- DESCRIPTION: Set [#FADE_TIME=5(0,100)] seconds for fade.

local corpse		= {}
local fade_delay 	= {}
local fade_time 	= {}

local fade_elapsed	= {}

function corpse_fader_properties(e, fade_delay, fade_time)
	corpse[e].fade_delay = fade_delay
	corpse[e].fade_time = fade_time
end 

function corpse_fader_init(e)
	corpse[e] = {}
	corpse[e].fade_delay = fade_delay
	corpse[e].fade_time = fade_time	
	
	fade_elapsed[e] = 0
end

function corpse_fader_main(e)

	fade_elapsed[e] = GetElapsedTime()
	
	for a,b in pairs (g_Entity) do 
		if GetEntityAllegiance(a) >= 0 then
			if b.health > 0 then 
			elseif b.startfading == nil then 
				b.startfading = corpse[e].fade_delay
			elseif b.startfading > 0 then  
				b.startfading = b.startfading - fade_elapsed[e]
			elseif b.corpsetimer == nil then 
				b.corpsetimer = corpse[e].fade_time
				SetEntityTransparency(a,1)
				b.alpha = GetEntityBaseAlpha(a) or 1
			elseif b.corpsetimer > 0 then
				b.corpsetimer = b.corpsetimer - fade_elapsed[e]
				local perc = b.corpsetimer / corpse[e].fade_time 
				perc = math.max(0,perc)
				SetEntityBaseAlpha(a, perc * b.alpha)
			else
				if b.isdestroyed == nil then
					Hide(a)
					Destroy(a)
					b.isdestroyed = true
				end
			end
		end
	end 
end 