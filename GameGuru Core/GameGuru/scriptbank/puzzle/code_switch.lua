-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Code Switch v7 By Necrym59
-- DESCRIPTION: This object will be treated as a code switch object for activating other objects or game elements. Set Always Active = On
-- DESCRIPTION: Customise the [PROMPT_TEXT$ = "to enter code"] [USE_RANGE=70(0,100)] Enter the [SWITCH_CODE=123(001,999)]
-- DESCRIPTION: Set the [READOUT_X=50(1,100)], [READOUT_Y=15(1,100)] position
-- DESCRIPTION: [USEAGE_TEXT$ = "Q-Select, Z-Change or enter Numbers, X-Submit"], [INCORRECT_TEXT$="Code is Incorrect"] and [CORRECT_TEXT$="Access Granted"]
-- DESCRIPTION: Code Lock [@STATUS=1(1=Locked, 2=Unlocked)]
-- DESCRIPTION: [@FAIL_ALARM=1(1=None, 2=Three Attempts)]
-- DESCRIPTION: [ALARM_RANGE=500(0,2000)]
-- DESCRIPTION: Play the audio <Sound0> when entering the codes.
-- DESCRIPTION: Play the audio <Sound1> when the code is entered correctly.
-- DESCRIPTION: Play the audio <Sound2> when the code is entered incorrect.
-- DESCRIPTION: Play the audio <Sound3> for the alarm sound.

g_codeswitch = {}
local prompt_text = {}
local use_range = {}
local switch_code = {}
local useage_text = {}
local incorrect_text = {}
local correct_text = {}
local entered_code = {}
local status = {}
local fail_alarm = {}
local alarm_range = {}
local codebar = {}
local codeinput_1 = {}
local codeinput_2 = {}
local codeinput_3 = {}
local readout_x = {}
local readout_y = {}
local readout_spacing = {}
local codepad = {}
local switchsprite = {}
local incorrectcount = {}
local wait = {}
local status = {}
local played = {}

function code_switch_properties(e, prompt_text, use_range, switch_code, readout_x, readout_y, useage_text, incorrect_text, correct_text, status, fail_alarm, alarm_range)
	g_codeswitch[e] = g_Entity[e]
	g_codeswitch[e]['prompt_text'] = prompt_text
	g_codeswitch[e]['use_range'] = use_range
	g_codeswitch[e]['switch_code'] = switch_code
	g_codeswitch[e]['readout_x'] = readout_x
	g_codeswitch[e]['readout_y'] = readout_y
	g_codeswitch[e]['useage_text'] = useage_text	
	g_codeswitch[e]['incorrect_text'] = incorrect_text
	g_codeswitch[e]['correct_text'] = correct_text
	g_codeswitch[e]['status'] = status
	g_codeswitch[e]['fail_alarm'] = fail_alarm
	g_codeswitch[e]['alarm_range'] = alarm_range
end 

function code_switch_init(e)
	g_codeswitch[e] = g_Entity[e]
	g_codeswitch[e]['prompt_text'] = "to enter code"
	g_codeswitch[e]['use_range'] = 70
	g_codeswitch[e]['switch_code'] = 001
	g_codeswitch[e]['readout_x'] = 0
	g_codeswitch[e]['readout_y'] = 0
	g_codeswitch[e]['useage_text'] = "Q-Select, Z-Change or enter Numbers, X-Submit"
	g_codeswitch[e]['incorrect_text'] = "Code is Incorrect"
	g_codeswitch[e]['correct_text'] = "Access Granted"
	g_codeswitch[e]['status'] = 1
	g_codeswitch[e]['fail_alarm'] = 1
	g_codeswitch[e]['alarm_range'] = 500
	readout_spacing[e] = 0
	entered_code[e] = 0
	codebar[e] = 1
	codeinput_1[e] = 0
	codeinput_2[e] = 0
	codeinput_3[e] = 0
	incorrectcount[e] = 0
	wait[e] = math.huge
	played[e] = 0
	codepad[e] = "inactive"
	status[e] = "init"
end

function code_switch_main(e)
	g_codeswitch[e] = g_Entity[e]
	
	if status[e] == "init" then 
		if GetDeviceWidth() < 2560 then readout_spacing[e] = 3 end
		if GetDeviceWidth() >= 2560 then readout_spacing[e] = 2 end
		if GetDeviceWidth() >= 3840 then readout_spacing[e] = 1 end
		status[e]= "endinit"
	end

	local PlayerDist = GetPlayerDistance(e)
	if PlayerDist < g_codeswitch[e]['use_range'] and g_PlayerHealth > 0 then
		local LookingAt = GetPlrLookingAtEx(e,1)
		
		if g_codeswitch[e]['status'] == 1 and codepad[e] == "inactive" then  --Locked
			if LookingAt == 1 then
				if GetGamePlayerStateXBOX() == 1 then
					PromptLocalForVR(e,"Y Button " .. g_codeswitch[e]['prompt_text'],3)				
				else
					if GetHeadTracker() == 1 then
						PromptLocalForVR(e,"Trigger " .. g_codeswitch[e]['prompt_text'],3)
					else
						PromptLocalForVR(e,"E " .. g_codeswitch[e]['prompt_text'],3)
					end
				end
				if g_KeyPressE == 1 then 
					codepad[e] = "active"
					PlaySound(e,0)
				end
			end
		end
		
		if codepad[e] == "active" then
			local key = GetKeyPressed(e,g_InKey)
			if key == "q" and codebar[e] > 0 then		
				PlaySound(e,0)					
				played[e] = 1
				codebar[e] = codebar[e] + 1	
				if codebar[e] > 3 then codebar[e] = 1 end				
			end			
			if codebar[e] == 1 then 
				if key == "1" or key == "2" or key == "3" or key == "4" or key == "1" or key == "5" or key == "6" or key == "7" or key == "8" or key == "9" or key == "0" then codeinput_1[e] = key	end
			end
			if codebar[e] == 2 then 
				if key == "1" or key == "2" or key == "3" or key == "4" or key == "1" or key == "5" or key == "6" or key == "7" or key == "8" or key == "9" or key == "0" then codeinput_2[e] = key end
			end	
			if codebar[e] == 3 then 
				if key == "1" or key == "2" or key == "3" or key == "4" or key == "1" or key == "5" or key == "6" or key == "7" or key == "8" or key == "9" or key == "0" then codeinput_3[e] = key end
			end				
			if key == "z" and codebar[e] == 1 then
				PlaySound(e,0)
				codeinput_1[e] = codeinput_1[e] + 1				
				if codeinput_1[e] > 9 then codeinput_1[e] = 0 end
			end
			if key == "z" and codebar[e] == 2 then
				PlaySound(e,0)
				codeinput_2[e] = codeinput_2[e] + 1
				if codeinput_2[e] > 9 then codeinput_2[e] = 0 end	
			end
			if key == "z" and codebar[e] == 3 then
				PlaySound(e,0)
				codeinput_3[e] = codeinput_3[e] + 1
				if codeinput_3[e] > 9 then codeinput_3[e] = 0 end	
			end			
			TextCenterOnXColor(g_codeswitch[e]['readout_x'],g_codeswitch[e]['readout_y']-2,3,g_codeswitch[e]['useage_text'],255,255,255)
			if codebar[e] == 1 then 			
				TextCenterOnXColor(g_codeswitch[e]['readout_x']-readout_spacing[e],g_codeswitch[e]['readout_y'],5,codeinput_1[e],0,255,0)
				TextCenterOnXColor(g_codeswitch[e]['readout_x'],g_codeswitch[e]['readout_y'],5,codeinput_2[e],255,255,255)
				TextCenterOnXColor(g_codeswitch[e]['readout_x']+readout_spacing[e],g_codeswitch[e]['readout_y'],5,codeinput_3[e],255,255,255)
			end
			if codebar[e] == 2 then
				TextCenterOnXColor(g_codeswitch[e]['readout_x']-readout_spacing[e],g_codeswitch[e]['readout_y'],5,codeinput_1[e],255,255,255)
				TextCenterOnXColor(g_codeswitch[e]['readout_x'],g_codeswitch[e]['readout_y'],5,codeinput_2[e],0,255,0)
				TextCenterOnXColor(g_codeswitch[e]['readout_x']+readout_spacing[e],g_codeswitch[e]['readout_y'],5,codeinput_3[e],255,255,255)
			end
			if codebar[e] == 3 then
				TextCenterOnXColor(g_codeswitch[e]['readout_x']-readout_spacing[e],g_codeswitch[e]['readout_y'],5,codeinput_1[e],255,255,255)
				TextCenterOnXColor(g_codeswitch[e]['readout_x'],g_codeswitch[e]['readout_y'],5,codeinput_2[e],255,255,255)
				TextCenterOnXColor(g_codeswitch[e]['readout_x']+readout_spacing[e],g_codeswitch[e]['readout_y'],5,codeinput_3[e],0,255,0)
			end
			entered_code[e] =(""..codeinput_1[e]..""..codeinput_2[e]..""..codeinput_3[e])
			if key == "x" then				
				if entered_code[e]-g_codeswitch[e]['switch_code'] == 0 then
					codepad[e] = "unlocked"
					SetActivatedWithMP(e,201)
					PromptLocalForVR(e,g_codeswitch[e]['correct_text'],3)
					StopSound(e,3)
					PlaySound(e,1)
					PerformLogicConnections(e)					
					SetAnimationName(e,"on")
					PlayAnimation(e)
					codeinput_1[e] = 0
					codeinput_2[e] = 0
					codeinput_3[e] = 0					
				else
					PromptLocalForVR(e,g_codeswitch[e]['incorrect_text'],3)
					PlaySound(e,2)
					incorrectcount[e] = incorrectcount[e] + 1
					if g_codeswitch[e]['fail_alarm'] == 2 and incorrectcount[e] == 3 then
						wait[e] = g_Time + 15000
						LoopSound(e,3)
						MakeAISound(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,g_codeswitch[e]['alarm_range'],1,e)
						codepad[e] = "alarm"
					end
				end				
			end
			played[e] = 0
		end		
		if g_codeswitch[e]['status'] == 2 then --Unlocked
			PromptLocalForVR(e,g_codeswitch[e]['correct_text'],3)
			if g_KeyPressE == 1 then
				SetActivatedWithMP(e,201)			
				PlaySound(e,1)
				SetAnimationName(e,"on")
				PlayAnimation(e)
				PerformLogicConnections(e)
			end	
		end
	end
	if codepad[e] == "alarm" then
		if g_Time > wait[e] then
			StopSound(e,3)
			incorrectcount[e] = 0
			codepad[e] = "active"
		end	
	end		
end

function GetKeyPressed(e, key, ignorecase)
    key = key or ""
    ignorecase = ignorecase or false 
    lastpressed = lastpressed or {}
    e = e or 0
    lastpressed[e] = lastpressed[e] or {}
    local inkey = g_InKey
    if ignorecase then
        key = string.lower(key)
        inkey = string.lower(g_InKey)
    end
    local waspressed
    if inkey == key and lastpressed[e] ~= g_InKey then
        waspressed = g_InKey 
    else
        waspressed = "false"
    end
    lastpressed[e] = g_InKey
    return waspressed
end
