-- Teleport v6 - thanks to Necrym59
-- DESCRIPTION: Set [TELEPORT_ZONEHEIGHT=100][@TELEPORT_TYPE=1(1=Instant, 2=Delayed, 3=Delayed + Countdown)][@TELEPORT_MODE=1(1=Single-use, 2=Re-useable)][TELEPORT_DELAY=5(1,5))][!TELEPORT_EFFECT=0] [@TELEPORT_DESTINATION=1(1=Local, 2=Level)] [@GoToLevelMode=1(1=Use Storyboard Logic,2=Go to Specific Level)] controls whether the next level in the Storyboard, or another level is loaded after entry to the zone. Will play <Sound0> for local teleports to the location of the object you connected with the zone. Will play <Sound1> for level teleport. It is better to use a small object or a flat object to avoid getting stuck when you reappear in the level.

g_teleport = {}	

local teleport_target = {}
local teleport_zoneheight = {}
local teleport_type = {}
local teleport_mode = {}
local teleport_delay = {}	
local teleport_timer = {}
local teleport_effect = {}	
local teleport_timer = {}
local effect = {}
local played = {}
	
function teleport_properties(e, teleport_zoneheight, teleport_type, teleport_mode, teleport_delay, teleport_effect, teleport_destination)
	g_teleport[e] = g_Entity[e]
	g_teleport[e].teleport_target 		= GetEntityString(e,0)
	g_teleport[e].teleport_zoneheight 	= teleport_zoneheight
	g_teleport[e].teleport_type 		= teleport_type
	g_teleport[e].teleport_mode 		= teleport_mode
	g_teleport[e].teleport_delay 		= teleport_delay
	g_teleport[e].teleport_effect		= teleport_effect
	g_teleport[e].teleport_destination	= teleport_destination
end

function teleport_init(e)
	g_teleport[e] = g_Entity[e]
	g_teleport[e].teleport_target 		= GetEntityString(e,0)
	g_teleport[e].teleport_zoneheight 	= 100
	g_teleport[e].teleport_type 		= 1
	g_teleport[e].teleport_delay 		= 0
	g_teleport[e].teleport_mode			= 2
	g_teleport[e].teleport_effect		= 0
	g_teleport[e].teleport_timer 		= 0
	g_teleport[e].teleport_destination	= 1
	fov = g_PlayerFOV
	effect = 0
	played[e] = 0
end

function teleport_main(e)
	g_teleport[e] = g_Entity[e]
	if g_teleport[e].teleport_type == 1 then
		if g_teleport[e].teleport_destination == 1 then
			if g_Entity[e]['plrinzone']==1 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+g_teleport[e].teleport_zoneheight then
				if g_teleport[e].teleport_target ~= nil then
					if g_teleport[e].teleport_effect == 1 then					
						if effect > -50 then
							SetPlayerFOV(fov+effect)
							effect = effect - 5						
						end
						if effect == -50 then
							PlaySound(e,0)
							TransportToIfUsed(e)
							SetPlayerFOV(fov)
							effect = 0
							if teleport_mode == 1 then Destroy(e) end
						end
					else
						PlaySound(e,0)
						TransportToIfUsed(e)
						SetPlayerFOV(fov)
						effect = 0
						if teleport_mode == 1 then Destroy(e) end					
					end	
				end
			end
		end
		if g_teleport[e].teleport_destination == 2 then
			if g_Entity[e]['plrinzone']==1 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+g_teleport[e].teleport_zoneheight then
				if played[e] == 0 then					
					PlaySound(e,1)
					played[e] = 1
				end				
				PerformLogicConnections(e)
				JumpToLevelIfUsed(e)					
			end
		end
	end
	
	if g_teleport[e].teleport_type == 2 or 3 then
		if g_teleport[e].teleport_destination == 1 then
			if g_Entity[e]['plrinzone']==1 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+g_teleport[e].teleport_zoneheight then
				if g_teleport[e].teleport_target ~= nil then
					g_teleport[e].teleport_timer = g_teleport[e].teleport_timer + GetElapsedTime()
					if g_teleport[e].teleport_type == 3 then Prompt("Teleport activating in "..math.floor(g_teleport[e].teleport_delay - g_teleport[e].teleport_timer).." seconds")	end
					if g_teleport[e].teleport_timer >= g_teleport[e].teleport_delay then
						Prompt("")
						if g_teleport[e].teleport_effect == 1 then					
							if effect > -50 then
								SetPlayerFOV(fov+effect)
								effect = effect - 5							
							end
							if effect == -50 then							
								PlaySound(e,0)
								TransportToIfUsed(e)
								SetPlayerFOV(fov)
								PerformLogicConnections(e)
								effect = 0
								if teleport_mode == 1 then Destroy(e) end
								g_teleport[e].teleport_timer = 0
							end
						else						
							PlaySound(e,0)
							TransportToIfUsed(e)
							SetPlayerFOV(fov)
							PerformLogicConnections(e)
							effect = 0
							if teleport_mode == 1 then Destroy(e) end
							g_teleport[e].teleport_timer = 0
						end						
					end
				end
			end
		end
		if g_teleport[e].teleport_destination == 2 then
			if g_Entity[e]['plrinzone']==1 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+g_teleport[e].teleport_zoneheight then
				g_teleport[e].teleport_timer = g_teleport[e].teleport_timer + GetElapsedTime()
				if g_teleport[e].teleport_type == 3 then Prompt("Level Teleport in "..math.floor(g_teleport[e].teleport_delay - g_teleport[e].teleport_timer).." seconds") end
				if g_teleport[e].teleport_timer >= g_teleport[e].teleport_delay then
					if played[e] == 0 then					
						PlaySound(e,1)
						played[e] = 1
					end	
					PerformLogicConnections(e)
					JumpToLevelIfUsed(e)					
				end
			end
		end		
	end			
end