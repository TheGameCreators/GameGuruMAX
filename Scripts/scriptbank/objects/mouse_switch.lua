-- Mouse Switch v4
-- DESCRIPTION: This object will be treated as a switch object for activating other objects or game elements with a mouse.
-- DESCRIPTION: [USE_RANGE=80(1,200)] distance
-- DESCRIPTION: [USE_PROMPT$="Press Mouse Button to use"]
-- DESCRIPTION: [@MOUSE_ACTIVATION=1(1=Left Button, 2=Right Button, 3=Left+Right Buttons, 4=Middle Button)]
-- DESCRIPTION: [@ON_ANIMATION$=1(0=AnimSetList)] Select ON animation (Default=ON)
-- DESCRIPTION: [@OFF_ANIMATION$=2(0=AnimSetList)] Select OFF animation (Default=OFF)
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None, 1=Shape, 2=Outline, 3=Icon)] Use emmisive color for shape option
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\hand.png"]
-- DESCRIPTION: <Sound0> when switch activates/deactivates


local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local switchmb 			= {}
local use_range			= {}
local use_prompt		= {}
local mouse_activation	= {}
local on_animation 		= {}
local off_animation 	= {}
local prompt_display	= {}
local item_highlight	= {}
local highlight_icon	= {}

local status 			= {}
local hl_icon			= {}
local hl_imgwidth		= {}
local hl_imgheight		= {}
local doonce 			= {}
local mbreset			= {}
local tEnt 				= {}
local selectobj 		= {}
local weaponstate		= {}
local last_gun			= {}


function mouse_switch_properties(e, use_range, use_prompt, mouse_activation, on_animation, off_animation, prompt_display, item_highlight, highlight_icon_imagefile)
	switchmb[e].use_range = use_range
	switchmb[e].use_prompt = use_prompt
	switchmb[e].mouse_activation = mouse_activation
	switchmb[e].on_animation = "=" .. tostring(on_animation)	
	switchmb[e].off_animation	= "=" .. tostring(off_animation)	
	switchmb[e].prompt_display = prompt_display
	switchmb[e].item_highlight = item_highlight or 0
	switchmb[e].highlight_icon = highlight_icon_imagefile	
end 

function mouse_switch_init(e)
	switchmb[e] = {}
	switchmb[e].use_range = 100
	switchmb[e].use_prompt = "Button to use"
	switchmb[e].mouse_activation = 1	
	switchmb[e].on_animation = ""
	switchmb[e].off_animation = ""	
	switchmb[e].prompt_display = 1
	switchmb[e].item_highlight = 0
	switchmb[e].highlight_icon = "imagebank\\icons\\hand.png"
	
	status[e] = "init"
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0	
	doonce[e] = 0
	weaponstate[e] = 0
	tEnt[e] = 0
	g_tEnt = 0
	selectobj[e] = 0
	g_MouseClick = 0
	mbreset[e] = math.huge
end
	 
function mouse_switch_main(e)

	if status[e] == "init" then
		if switchmb[e].item_highlight == 3 and switchmb[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(switchmb[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(switchmb[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(switchmb[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end
		status[e] = "endinit"
	end

	local PlayerDist = GetPlayerDistance(e)
	
	if PlayerDist <= switchmb[e].use_range then
		--pinpoint select object--
		module_misclib.pinpoint(e,switchmb[e].use_range, switchmb[e].item_highlight,hl_icon[e])
		tEnt[e] = g_tEnt
		--end pinpoint select object--
		if PlayerDist <= switchmb[e].use_range and tEnt[e] == e then
			if switchmb[e].prompt_display == 1 then TextCenterOnX(50,54,1,switchmb[e].use_prompt) end
			if switchmb[e].prompt_display == 2 then Prompt(switchmb[e].use_prompt) end			
			if switchmb[e].mouse_activation == 1 then
				if g_MouseClick == 1 then
					if doonce[e] == 0 then 
						mbreset[e] = g_Time + 250				
						SetAnimationName(e,switchmb[e].on_animation)
						PlayAnimation(e)					
						StopAnimation(e)
						PlaySound(e,0)
						PerformLogicConnections(e)
						doonce[e] = 1
					end
				end
			end	
			if switchmb[e].mouse_activation == 2 then
				if g_MouseClick == 2 then
					if doonce[e] == 0 then
						mbreset[e] = g_Time + 250
						SetAnimationName(e,switchmb[e].on_animation)
						PlayAnimation(e)					
						StopAnimation(e)
						PlaySound(e,0)
						PerformLogicConnections(e)
						doonce[e] = 1
					end
				end	
			end
			if switchmb[e].mouse_activation == 3 then
				if g_MouseClick == 3 then
					if doonce[e] == 0 then 
						mbreset[e] = g_Time + 250
						SetAnimationName(e,switchmb[e].on_animation)
						PlayAnimation(e)					
						StopAnimation(e)
						PlaySound(e,0)
						PerformLogicConnections(e)
						doonce[e] = 1
					end
				end	
			end		
			if switchmb[e].mouse_activation == 4 then
				if g_MouseClick == 4 then
					if doonce[e] == 0 then 
						mbreset[e] = g_Time + 250
						SetAnimationName(e,switchmb[e].on_animation)
						PlayAnimation(e)					
						StopAnimation(e)
						PlaySound(e,0)
						PerformLogicConnections(e)
						doonce[e] = 1
					end
				end	
			end
			
			if g_Time > mbreset[e] then	
				if doonce[e] == 1 then
					SetAnimationName(e,switchmb[e].off_animation)
					PlayAnimation(e)			
					doonce[e] = 0
				end
			end
		end	
	end
	
	if PlayerDist <= switchmb[e].use_range and weaponstate[e] == 0 then
		last_gun[e] = g_PlayerGunName
		SetPlayerWeapons(0)
		weaponstate[e] = 1
	end

	if PlayerDist > switchmb[e].use_range and weaponstate[e] == 1 then	
		ChangePlayerWeapon(last_gun[e])
		SetPlayerWeapons(1)
		weaponstate[e] = 0
	end
end
