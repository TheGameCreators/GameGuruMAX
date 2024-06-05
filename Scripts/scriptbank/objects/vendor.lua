-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- vendoror v12 by Necrym59
-- DESCRIPTION: Allows to use this object as a vendor to give the player the selected item.
-- DESCRIPTION: [PROMPT_TEXT$="E to dispense item"]
-- DESCRIPTION: [PROMPT_RANGE=90(0,100)]
-- DESCRIPTION: [NOISE_RANGE=500(0-5000)]
-- DESCRIPTION: [@VENDOR_ANIMATION=-1(0=AnimSetList)]
-- DESCRIPTION: [VENDORED_MAX_QUANTITY=1(1,50)]
-- DESCRIPTION: [VENDORED_ENTITY_COST=0(0,100)]
-- DESCRIPTION: [VENDORED_ENTITY_LIFESPAN=1(0,100)] Minutes (0=Eternal)
-- DESCRIPTION: [VENDORED_ENTITY_NAME$=""]
-- DESCRIPTION: [USER_GLOBAL_AFFECTED$="MyMoney"]
-- DESCRIPTION: [@WHEN_EMPTY=1(1=Do Nothing, 2=Destroy Vendor)]
-- DESCRIPTION: <Sound0> Vending sound
-- DESCRIPTION: <Sound1> Empty sounds

local lower = string.lower

local vendor 					= {}
local prompt_text 				= {}
local prompt_range				= {}
local noise_range 				= {}
local vendor_animation			= {}
local vendored_max_quantity		= {}
local vendored_entity_cost		= {}
local vendored_entity_lifespan	= {}
local vendored_entity_name		= {}
local vendored_entity_no		= {}
local user_global_affected 		= {}
local when_empty				= {}


local origin_x		= {}
local origin_y		= {}
local origin_z		= {}
local newEntn		= {}
local cntEntn		= {}
local status		= {}
local doonce		= {}
local isempty		= {}
local playonce		= {}
local pressed		= {}
local vendlist		= {}
local dispensed		= {}
local lifetimer		= {}
local eternal		= {}
local currentvalue	= {}
local wait			= {}

function vendor_properties(e, prompt_text, prompt_range, noise_range, vendor_animation, vendored_max_quantity, vendored_entity_cost, vendored_entity_lifespan, vendored_entity_name, user_global_affected, when_empty)
	vendor[e].prompt_text = prompt_text or ""
	vendor[e].prompt_range = prompt_range
	vendor[e].noise_range = noise_range
	vendor[e].vendor_animation = "=" .. tostring(vendor_animation)
	vendor[e].vendored_max_quantity = vendored_max_quantity
	vendor[e].vendored_entity_cost = vendored_entity_cost	
	vendor[e].vendored_entity_lifespan = vendored_entity_lifespan
	vendor[e].vendored_entity_name = lower(vendored_entity_name)
	vendor[e].user_global_affected = "MyMoney"
	vendor[e].when_empty = when_empty
	vendor[e].vendored_entity_no = 0	
end

function vendor_init(e)
	vendor[e] = {}
	vendor[e].prompt_text = "E to dispense item"
	vendor[e].prompt_range = 80
	vendor[e].noise_range = 500
	vendor[e].vendor_animation = ""
	vendor[e].vendored_max_quantity = 1
	vendor[e].vendored_entity_cost = 0	
	vendor[e].vendored_entity_lifespan = 1	
	vendor[e].vendored_entity_name = ""
	vendor[e].user_global_affected = "MyMoney"
	vendor[e].when_empty = 1
	vendor[e].vendored_entity_no = 0	

	status[e] = "init"
	newEntn[e] = 0
	cntEntn[e] = 0
	lifetimer[e] = 0
	eternal[e] = 0
	doonce[e] = 0
	playonce[e] = 0	
	pressed[e] = 0
	dispensed[e] = 0
	isempty[e] = 0
	currentvalue[e] = 0
	wait[e] = math.huge
end

function vendor_main(e)

	if status[e] == "init" then		
		if vendor[e].user_global_affected > "" then
			if _G["g_UserGlobal['"..vendor[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..vendor[e].user_global_affected.."']"] end
		end				
		if vendor[e].vendored_max_quantity > 50 then vendor[e].vendored_max_quantity = 50 end
		if vendor[e].vendored_entity_lifespan > 100 then vendor[e].vendored_entity_lifespan = 100 end
		if vendor[e].vendored_entity_no == 0 or vendor[e].vendored_entity_no == nil then
			for ee = 1, g_EntityElementMax do
				if ee ~= nil and e~= ee and g_Entity[ee] ~= nil then
					if lower(GetEntityName(ee)) == lower(vendor[e].vendored_entity_name) then
						vendor[e].vendored_entity_no = ee
						origin_x[e] = g_Entity[ee]['x']
						origin_y[e] = g_Entity[ee]['y']
						origin_z[e] = g_Entity[ee]['z']
						CollisionOff(ee)
						Hide(ee)						
						status[e] = "vendor"
						break
					end
				end
			end
		end
	end
	
	local PlayerDist = GetPlayerDistance(e)
	if status[e] == "vendor" then
		if vendor[e].user_global_affected > "" then
			if _G["g_UserGlobal['"..vendor[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..vendor[e].user_global_affected.."']"] end
		end
		if PlayerDist < vendor[e].prompt_range then
			if dispensed[e] < vendor[e].vendored_max_quantity then PromptLocal(e,vendor[e].prompt_text) end
			if dispensed[e] == vendor[e].vendored_max_quantity then PromptLocal(e,"") end
			if g_KeyPressE == 1 and currentvalue[e] < vendor[e].vendored_entity_cost then PromptLocal(e,"Insufficent Funds") end
			if currentvalue[e] >= vendor[e].vendored_entity_cost then				
				if g_KeyPressE == 1 and pressed[e] == 0 then
					SetAnimationName(e,vendor[e].break_animation)
					PlayAnimation(e)
					if dispensed[e] < vendor[e].vendored_max_quantity then
						PlaySound(e,0)
						pressed[e] = 0
						if vendor[e].user_global_affected > "" then
							if _G["g_UserGlobal['"..vendor[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..vendor[e].user_global_affected.."']"] end
							_G["g_UserGlobal['"..vendor[e].user_global_affected.."']"] = currentvalue[e] - vendor[e].vendored_entity_cost
							status[e] = "vendored"
						end
						if vendor[e].vendored_entity_cost == 0 then
							status[e] = "vendored"							
						end
					end
					if dispensed[e] == vendor[e].vendored_max_quantity then
						PlaySound(e,1)
						pressed[e] = 1
						status[e] = "vendor"
					end	
					if vendor[e].noise_range > 0 then MakeAISound(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,vendor[e].noise_range,1,e) end				
				end
			end
		end
	end

	if status[e] == "vendored" then
		if doonce[e] == 0 and dispensed[e] < vendor[e].vendored_max_quantity then			
			local etoclone = vendor[e].vendored_entity_no
			newEntn[e] = SpawnNewEntity(etoclone)			
			Show(newEntn[e])
			GravityOff(newEntn[e])
			CollisionOff(newEntn[e])
			table.insert(vendlist,newEntn[e])
			local newposx = origin_x[e]
			local newposy = origin_y[e]
			local newposz = origin_z[e]
			ResetPosition(newEntn[e],newposx,newposy,newposz)
			CollisionOn(newEntn[e])
			GravityOn(newEntn[e])
			cntEntn[e] = cntEntn[e] + 1
			dispensed[e] = dispensed[e] +1
			if vendor[e].vendored_entity_lifespan > 0 then
				lifetimer[e] = g_Time + (vendor[e].vendored_entity_lifespan * 1000)*60
				eternal[e] = 0
			end
			if vendor[e].vendored_entity_lifespan == 0 then eternal[e] = 1 end
			wait[e] = g_Time + 600
			doonce[e] = 1
		end	
		if g_Time > wait[e] then status[e] = "vended" end
	end
	
	if status[e] == "vended" then		
		if dispensed[e] == vendor[e].vendored_max_quantity then
			if vendor[e].when_empty == 1 then
				if isempty[e] == 0 then
					ActivateIfUsed(e)
					PerformLogicConnections(e)
					isempty[e] = 1
				end	
			end
			if vendor[e].when_empty == 2 then 
				ActivateIfUsed(e)
				PerformLogicConnections(e)
				Hide(e)
				CollisionOff(e)
				Destroy(e)
			end	
		end		
		pressed[e] = 0
		doonce[e] = 0
		status[e] = "vendor"
	end
	
	if dispensed[e] == vendor[e].vendored_max_quantity and eternal[e] == 0 then
		if g_Time > lifetimer[e] then				
			if cntEntn[e] > 0 then					
				for a,b in pairs (vendlist) do 
					if g_Entity[b] ~= nil then
						CollisionOff(e)
						Destroy(b)
						DeleteNewEntity(b)
						vendlist[a] = nil
					end
				end
			end	
			if cntEntn[e] == 0 then
				pressed[e] = 0
				doonce[e] = 0
				status[e] = "vendor"
			end
		end
	end
end