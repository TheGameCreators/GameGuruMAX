-- Save Point v6 by Necrym59
-- DESCRIPTION: Saves a save point
-- DESCRIPTION: Attach to an object, switch or trigger zone to activate
-- DESCRIPTION: [@ACTIVATION=1(1=By Object/Switch, 2=By Zone)]
-- DESCRIPTION: [USE_RANGE=80(1,200)]
-- DESCRIPTION: [PROMPT_MESSAGE$="Press E to save"]
-- DESCRIPTION: [SAVE_MESSAGE$="Saving.."]
-- DESCRIPTION: <Sound0> when activated

local U = require "scriptbank\\utillib"

local savepoint 		= {}
local activation 		= {}
local use_range 		= {}
local save_message 		= {}

local played			= {}
local doonce			= {}
local wait				= {}
local animonce			= {}
local tEnt 				= {}
local selectobj 		= {}
local status			= {}

function save_point_properties(e, activation, use_range, prompt_message, save_message)
	savepoint[e] = g_Entity[e]
	savepoint[e].activation = activation
	savepoint[e].use_range = use_range
	savepoint[e].prompt_message = prompt_message
	savepoint[e].save_message = save_message
end

function save_point_init(e)
	savepoint[e] = {}
	savepoint[e].activation = 1
	savepoint[e].use_range = 80
	savepoint[e].prompt_message = "Press E to save"
	savepoint[e].save_message = "Saving.."
	played[e] = 0
	doonce[e] = 0
	animonce[e] = 0
	tEnt[e] = 0
	selectobj[e] = 0	
	wait[e] = math.huge
	status[e] = "init"
end

function save_point_main(e)

	if status[e] == "init" then
		status[e] = "activate"
	end

	if savepoint[e].activation == 1 and status[e] == "activate" then
		local PlayerDist = GetPlayerDistance(e)
		if PlayerDist < savepoint[e].use_range then
			-- pinpoint select object--			
			local px, py, pz = GetCameraPositionX(0), GetCameraPositionY(0), GetCameraPositionZ(0)
			local rayX, rayY, rayZ = 0,0,savepoint[e].use_range
			local paX, paY, paZ = math.rad(GetCameraAngleX(0)), math.rad(GetCameraAngleY(0)), math.rad(GetCameraAngleZ(0))
			rayX, rayY, rayZ = U.Rotate3D(rayX, rayY, rayZ, paX, paY, paZ)
			selectobj[e]=IntersectAll(px,py,pz, px+rayX, py+rayY, pz+rayZ,e)
			if selectobj[e] ~= 0 or selectobj[e] ~= nil then
				if g_Entity[e].obj == selectobj[e] then
					TextCenterOnXColor(50-0.01,50,3,"+",255,255,255) --highliting (with crosshair at present)
					tEnt[e] = e
				else 
					tEnt[e] = 0			
				end
				if g_Entity[e]['obj'] ~= selectobj[e] then selectobj[e] = 0 end
			end
			if selectobj[e] == 0 or selectobj[e] == nil then
				tEnt[e] = 0
				TextCenterOnXColor(50-0.01,50,3,"+",155,155,155) --highliting (with crosshair at present)
			end
			--end pinpoint select object--
		end
		if PlayerDist < savepoint[e].use_range and tEnt[e] ~= 0 then
			PromptLocal(e,savepoint[e].prompt_message)
			if g_KeyPressE == 1 then
				status[e] = "save"
				SetActivated(e,1)
			end
		end		
	end
	if savepoint[e].activation == 2 and status[e] == "activate" then
		if g_Entity[e].plrinzone == 1 and g_PlayerPosY < g_Entity[e]['y']+100 then
			status[e] = "save"
			SetActivated(e,1)
		end
	end
	
	if g_Entity[e]['activated'] == 1 then		
		if status[e] == "save" then	
			if doonce[e] == 0 then 
				if played[e] == 0 then
					if savepoint[e].activation == 1 then
						PromptLocal(e,savepoint[e].save_message)
						SetAnimationName(e,"on")
						PlayAnimation(e)
						PlaySound(e,0)
					end
					if savepoint[e].activation == 2 then
						Prompt(savepoint[e].save_message)
						PlaySound(e,0)
						Destroy(e)
					end
					played[e] = 1
				end
				-- set game slot and name and save --				
				strNewSlotName = "Save Point: " .. os.date()
				gamedata = require "titlesbank\\gamedata"
				gamedata.save(8,strNewSlotName)
				-- and also save state of all player attributes (health, weapon, containers, userglobals)
				gamedata.mode(2)
				gamedata.save("0-globals","playerstate")
				gamedata.mode(0)
				---------------------------------------
				ResumeGame()
				PerformLogicConnections(e)
				doonce[e] = 1
				SetActivated(e,0)
				wait[e] = g_Time + 8000
				animonce[e] = 1
			end	
		end
	end
	
	if g_Entity[e]['activated'] == 0 then
		if g_Time > wait[e] then
			doonce[e] = 0
			played[e] = 0
			if animonce[e] == 1 then
				SetAnimationName(e,"off")				
				PlayAnimation(e)
				animonce[e] = 0
				PerformLogicConnections(e)
			end	
			status[e] = "activate"
		end
	end
end