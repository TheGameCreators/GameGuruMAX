-- Water Control v10 - by Necrym59
-- DESCRIPTION: Control water height when activated by switch linked to an object or a zone.
-- DESCRIPTION: Attach to an object and set to Physics=ON/OFF. Link to a switch or zone.
-- DESCRIPTION: Can be also be attached to a zone directly.
-- DESCRIPTION: [PROMPT_TEXT$="Don't drown"]
-- DESCRIPTION: [@ACTION=1(1=Raise, 2=Lower)] the water level
-- DESCRIPTION: [ADJUSTMENT_AMOUNT=80]
-- DESCRIPTION: [#ADJUSTMENT_SPEED=0.02(0,5)]
-- DESCRIPTION: [NPC_DROWNING!=0] If on will drown all NPC's under water level.
-- DESCRIPTION: and play <Sound0>

local g_waterControlMasterE = 0
local watercontrol = {}
local prompt_text = {}
local status = {}
local state = {}

function watercontrol_properties(e, prompt_text, action, adjustment_amount, adjustment_speed, npc_drowning)
	watercontrol[e].prompt_text = prompt_text
	watercontrol[e].action = action or 1
	watercontrol[e].adjustment_amount = adjustment_amount or 80
	watercontrol[e].adjustment_speed = adjustment_speed or 1
	watercontrol[e].npc_drowning = npc_drowning or 0
end 

function watercontrol_init(e)
	watercontrol[e] = {}
	watercontrol[e].prompt_text = "Don't drown"
	watercontrol[e].action = 1
	watercontrol[e].adjustment_amount = 80
	watercontrol[e].adjustment_speed = 1
	watercontrol[e].npc_drowning = 0	
	status[e] = "init"
	state[e] = 0
end

function watercontrol_main(e)
	if status[e] == "init" then
		status[e] = "end"
	end
	if g_Entity[e]['plrinzone']==1 and g_Entity[e]['active'] == 1 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+100 then
		if g_Entity[e]['activated'] == 0 then
			SetEntityActivated(e,1)
		end
	end	
	if g_Entity[e]['activated'] == 1 then
		if state[e] == 0 then
			state[e] = 1
			Prompt(watercontrol[e].prompt_text)		
			PlaySound(e,0)
			PerformLogicConnections(e)
			if watercontrol[e].action == 1 then watercontrol[e].adjustment_amount = (GetWaterHeight() + watercontrol[e].adjustment_amount) end
			if watercontrol[e].action == 2 then watercontrol[e].adjustment_amount = (GetWaterHeight() - watercontrol[e].adjustment_amount) end
			if g_waterControlMasterE ~= e then
				SetEntityActivated(g_waterControlMasterE,0)
				g_waterControlMasterE = 0
			end
			g_waterControlMasterE = e
		end
		if g_waterControlMasterE == e then			
			local current_adjustment_amount = GetWaterHeight()
			if watercontrol[e].action == 1 then -- Raise Water Height
				if watercontrol[e].npc_drowning == 0 then
					for a = 1, g_EntityElementMax do
						if a ~= nil and g_Entity[a] ~= nil then
							local allegiance = GetEntityAllegiance(a) -- (0-enemy, 1-ally, 2-neutral)
							if allegiance == 0 or allegiance == 1 or allegiance == 2 then
								SetEntityUnderwaterMode(a,1)
							end
						end
					end
				end	
				if current_adjustment_amount < watercontrol[e].adjustment_amount then
					current_adjustment_amount = current_adjustment_amount + watercontrol[e].adjustment_speed
					SetWaterHeight(current_adjustment_amount)
					ShowWater()
				else
					SetEntityActivated(e,0)
				end			
			end
			if watercontrol[e].action == 2 then -- Lower Water Height
				if current_adjustment_amount > watercontrol[e].adjustment_amount then
					current_adjustment_amount = current_adjustment_amount - watercontrol[e].adjustment_speed
					SetWaterHeight(current_adjustment_amount)
					ShowWater()
				else
					SetEntityActivated(e,0)
				end
			end
		end
	end
end
