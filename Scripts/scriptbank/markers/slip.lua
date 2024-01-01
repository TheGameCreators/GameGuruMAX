-- Slip v3 - thanks to Necrym59
-- DESCRIPTION: Player will slip in this zone by [VELOCITY=1(1,20)] and slowed by [COUNTER_FORCE=2(1,5)] will show [PROMPT_TEXT$="It's slippery"].

g_slip = {}	
local velocity = {}
local counter_force = {}
local prompt_text = {}	
local SlipAngleY = {}
	
function slip_init(e)
	g_slip[e] = {}
	slip_properties(e,3,2,"It's slippery")	
	SlipAngleY[e] = 0
end

function slip_properties(e, velocity, counter_force, prompt_text)
	g_slip[e]['velocity'] = velocity
	g_slip[e]['counter_force'] = counter_force/1000
	g_slip[e]['prompt_text'] = prompt_text
end

function slip_main(e)	
	if g_Entity[e]['plrinzone']==0 then
		SlipAngleY[e] = g_PlayerAngY
		if g_slip[e]['velocity'] < GetGamePlayerControlSpeed() then g_slip[e]['velocity'] = GetGamePlayerControlSpeed() end
	end
	if g_Entity[e]['plrinzone']==1 then
		Prompt(g_slip[e]['prompt_text'])				
		ForcePlayer(SlipAngleY[e],g_slip[e]['velocity'])		
		if g_KeyPressW == 1 then			
			if g_slip[e]['velocity'] < 0.5 then SlipAngleY[e] = g_PlayerAngY end
			g_slip[e]['velocity'] = g_slip[e]['velocity'] + g_slip[e]['counter_force']
		else
			g_slip[e]['velocity'] = g_slip[e]['velocity']
		end
		if g_KeyPressS == 1 and g_slip[e]['velocity'] < 0.5 then
			SlipAngleY[e] = g_PlayerAngY-180
			g_slip[e]['velocity'] = g_slip[e]['velocity'] + g_slip[e]['counter_force']
		end
		if g_KeyPressS == 1 and g_slip[e]['velocity'] > 0 then					
			g_slip[e]['velocity'] = g_slip[e]['velocity'] - g_slip[e]['counter_force']
		else
			g_slip[e]['velocity'] = g_slip[e]['velocity']
		end		
		if g_KeyPressA == 1  or g_KeyPressD == 1 then			
			g_slip[e]['velocity'] = g_slip[e]['velocity']
		end
		if g_slip[e]['velocity'] < 0 then g_slip[e]['velocity'] = 0 end
	end	
end