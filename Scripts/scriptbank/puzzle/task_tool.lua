-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Task Tool v6 by Necrym59
-- DESCRIPTION: This object will give the player a designated task-tool if collected.
-- DESCRIPTION: [PROMPT_TEXT$="E to collect"]
-- DESCRIPTION: [PICKUP_RANGE=90(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=1(1=Automatic, 2=Manual)]
-- DESCRIPTION: [@TOOL_TYPE=1(1=Crowbar, 2=Screwdriver, 3=Spanner, 4=Cutter, 5=Named Tool)]
-- DESCRIPTION: [TOOL_NAME$="Named Tool"]
-- DESCRIPTION: Play the audio <Sound0> when picked up.

local U = require "scriptbank\\utillib"

g_tasktool				= {}
g_tasktoolname			= {}
local tasktool			= {}
local prompt_text		= {}
local pickup_range 		= {}
local pickup_style 		= {}
local tool_type			= {}
local tool_name 		= {}

local collected 		= {}
local status 			= {}
local tEnt				= {}
local selectobj			= {}

function task_tool_properties(e, prompt_text, pickup_range, pickup_style, tool_type, tool_name)
	tasktool[e] = g_Entity[e]
	tasktool[e].prompt_text = prompt_text
	tasktool[e].pickup_range = pickup_range
	tasktool[e].pickup_style = pickup_style
	tasktool[e].tool_type = tool_type
	tasktool[e].tool_name = tool_name
end

function task_tool_init(e)	
	tasktool[e] = {}
	tasktool[e].prompt_text = "to collect"
	tasktool[e].pickup_range = 80
	tasktool[e].pickup_style = 1
	tasktool[e].tool_type = 1
	tasktool[e].tool_name = ""
	g_tasktool = 0
	collected[e] = 0
	tEnt[e] = 0
	selectobj[e] = 0
	status[e] = "init"
end

function task_tool_main(e)
	tasktool[e] = g_Entity[e]
	if status[e] == "init" then	
		if tasktool[e].tool_type == 1 then tasktool[e].tool_name = "Crowbar" end
		if tasktool[e].tool_type == 2 then tasktool[e].tool_name = "Screwdriver" end
		if tasktool[e].tool_type == 3 then tasktool[e].tool_name = "Spanner" end
		if tasktool[e].tool_type == 4 then tasktool[e].tool_name = "Cutter" end
		if tasktool[e].tool_type == 5 then tasktool[e].tool_name = tasktool[e].tool_name end
		status[e] = "endinit"
	end
	
	local PlayerDist = GetPlayerDistance(e)
	
	if tasktool[e].pickup_style == 1 then 
		if PlayerDist < tasktool[e].pickup_range and collected[e] == 0 then
			PlaySound(e,0)
			PromptLocal(e,tasktool[e].tool_name.. " collected")
			PerformLogicConnections(e)
			g_tasktool = tasktool[e].tool_type
			collected[e] = 1
			Destroy(e)			
		end
	end
	
	if tasktool[e].pickup_style == 2 then 
		local LookingAt = GetPlrLookingAtEx(e,1)
		if LookingAt == 1 and PlayerDist < tasktool[e].pickup_range then
			-- pinpoint select object--
			local px, py, pz = GetCameraPositionX(0), GetCameraPositionY(0), GetCameraPositionZ(0)
			local rayX, rayY, rayZ = 0,0,tasktool[e].pickup_range
			local paX, paY, paZ = math.rad(GetCameraAngleX(0)), math.rad(GetCameraAngleY(0)), math.rad(GetCameraAngleZ(0))
			rayX, rayY, rayZ = U.Rotate3D(rayX, rayY, rayZ, paX, paY, paZ)
			selectobj[e]=IntersectAll(px,py,pz, px+rayX, py+rayY, pz+rayZ,e)
			if selectobj[e] ~= 0 or selectobj[e] ~= nil then
				if g_Entity[e]['obj'] == selectobj[e] then
					TextCenterOnXColor(50-0.01,50,3,"+",255,255,255)
					tEnt[e] = e
				else
					tEnt[e] = 0
				end
			end
			if selectobj[e] == 0 or selectobj[e] == nil then
				tEnt[e] = 0
			end
			--end pinpoint select object--
		end

		if PlayerDist < tasktool[e].pickup_range and collected[e] == 0 and tEnt[e] ~= 0 then
			PromptLocal(e,tasktool[e].prompt_text)
			if g_KeyPressE == 1 then
				PlaySound(e,0)
				PromptLocal(e,tasktool[e].tool_name.. " collected")
				PerformLogicConnections(e)
				g_tasktool = tasktool[e].tool_type
				g_tasktoolname = tasktool[e].tool_name
				collected[e] = 1
				Destroy(e)
			end
		end
	end	
end

