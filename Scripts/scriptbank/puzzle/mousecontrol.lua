--Mouse Control v11 by Necrym59
--DESCRIPTION:Will allow mouse control for game actions
--DESCRIPTION:Apply to an object, set Always Active ON
--DESCRIPTION:Set the [RANGE=100(1-300)]
--DESCRIPTION:[TIMER_ADJUST=500(1-5000)]
--DESCRIPTION:[@ACTION_CONTROL=1(1=Left Button, 2=Right Button, 3=Left or Right Button, 4=Left and Right Buttons)]
--DESCRIPTION:[@POINTER=1(1=On, 2=Off, 3=Crosshair, 4=Mouse, 5=Adaptive)]
--DESCRIPTION:<Sound0> on mouse click

local U = require "scriptbank\\utillib"
local P = require "scriptbank\\physlib"
local rad  = math.rad

local mousecontrol = {}
local range = {}
local timer_adjust = {}
local action_control = {}
local last_gun = {}
local pointer = {}
local cursor = {}
local npccursor1 = {}
local npccursor2 = {}

local selectobj = {}
local pEnt = {}
local pName = {}
local pResult = {}
local pAllegiance = {}
local status = {}
local clicked = {}
local clicktime = {}
local last_gun = {}
local gunstate = {}
	
function mousecontrol_properties(e, range, timer_adjust, action_control, pointer)
	mousecontrol[e] = g_Entity[e]
	mousecontrol[e].range = range
	mousecontrol[e].timer_adjust = timer_adjust
	mousecontrol[e].action_control = action_control
	mousecontrol[e].pointer = pointer
end 

function mousecontrol_init(e)
	mousecontrol[e] = g_Entity[e]
	mousecontrol[e].range = 300
	mousecontrol[e].timer_adjust = 500
	mousecontrol[e].action_control = 1
	mousecontrol[e].pointer = 1
	selectobj[e] = 0	
	pEnt = 0
	pName = ""
	pResult = 0
	status[e] = "init"
	clicked[e] = 0
	clicktime[e] = 0
	last_gun = g_PlayerGunName
	gunstate = 0
end
 
function mousecontrol_main(e)
	mousecontrol[e] = g_Entity[e]
	PlayerDist = GetPlayerDistance(e)
	if status[e] == "init" then
		if mousecontrol[e].pointer ~=5 then 
			if mousecontrol[e].action_control == 1 then cursor = CreateSprite(LoadImage("\\imagebank\\misc\\testimages\\mousecontrol\\lmb.png")) end
			if mousecontrol[e].action_control == 2 then cursor = CreateSprite(LoadImage("\\imagebank\\misc\\testimages\\mousecontrol\\rmb.png")) end
			if mousecontrol[e].action_control == 3 then cursor = CreateSprite(LoadImage("\\imagebank\\misc\\testimages\\mousecontrol\\lrmb1.png")) end
			if mousecontrol[e].action_control == 4 then cursor = CreateSprite(LoadImage("\\imagebank\\misc\\testimages\\mousecontrol\\lrmb2.png")) end
		end
		-- Active Sprite Cursors
		if mousecontrol[e].pointer == 5 then cursor = CreateSprite(LoadImage("\\imagebank\\misc\\testimages\\mousecontrol\\dot.png")) end -- Default pointer
		SetSpriteSize(cursor,-1,-1)	
		SetSpriteDepth(cursor,100)
		SetSpritePosition(cursor,200,200)
		
		--npc cursors
		cursor1 = CreateSprite(LoadImage("\\imagebank\\misc\\testimages\\mousecontrol\\speak.png"))
		SetSpriteSize(cursor1,-1,-1)	
		SetSpriteDepth(cursor1,100)
		SetSpritePosition(cursor1,200,200)
		cursor2 = CreateSprite(LoadImage("\\imagebank\\misc\\testimages\\mousecontrol\\fight.png"))
		SetSpriteSize(cursor2,-1,-1)	
		SetSpriteDepth(cursor2,100)
		SetSpritePosition(cursor2,200,200)		
		
		--action cursors (when rpg activated)
		--cursor3 = CreateSprite(LoadImage("\\imagebank\\misc\\testimages\\mousecontrol\\eat.png.png"))
		--SetSpriteSize(cursor3,-1,-1)	
		--SetSpriteDepth(cursor3,100)
		--SetSpritePosition(cursor3,200,200)
		--cursor4 = CreateSprite(LoadImage("\\imagebank\\misc\\testimages\\mousecontrol\\drink.png"))
		--SetSpriteSize(cursor4,-1,-1)	
		--SetSpriteDepth(cursor4,100)
		--SetSpritePosition(cursor4,200,200)
		cursor5 = CreateSprite(LoadImage("\\imagebank\\misc\\testimages\\mousecontrol\\carry.png"))
		SetSpriteSize(cursor5,-1,-1)	
		SetSpriteDepth(cursor5,100)
		SetSpritePosition(cursor5,200,200)
		--cursor6 = CreateSprite(LoadImage("\\imagebank\\misc\\testimages\\mousecontrol\\climb.png"))
		--SetSpriteSize(cursor6,-1,-1)	
		--SetSpriteDepth(cursor6,100)
		--SetSpritePosition(cursor6,200,200)
		--cursor7 = CreateSprite(LoadImage("\\imagebank\\misc\\testimages\\mousecontrol\\break.png"))
		--SetSpriteSize(cursor7,-1,-1)	
		--SetSpriteDepth(cursor7,100)
		--SetSpritePosition(cursor7,200,200)
		cursor8 = CreateSprite(LoadImage("\\imagebank\\misc\\testimages\\mousecontrol\\hand.png"))
		SetSpriteSize(cursor8,-1,-1)	
		SetSpriteDepth(cursor8,100)
		SetSpritePosition(cursor8,200,200)
		
		status[e] = "mouseplay"
	end
	
	if status[e] == "mouseplay" then	
	
		if mousecontrol[e].pointer == 1 then TextCenterOnXColor(50,50,3,"< >",120,120,120) end -- Pointer Guide here	
		
		--- Find Oject in view range ---
		local px, py, pz = GetCameraPositionX(0), GetCameraPositionY(0), GetCameraPositionZ(0)
		local rayX, rayY, rayZ = 0,0,mousecontrol[e].range
		local paX, paY, paZ = math.rad(GetCameraAngleX(0)), math.rad(GetCameraAngleY(0)), math.rad(GetCameraAngleZ(0))
		rayX, rayY, rayZ = U.Rotate3D(rayX, rayY, rayZ, paX, paY, paZ)
		selectobj[e]=IntersectAll(px,py,pz, px+rayX, py+rayY, pz+rayZ,e)
		if selectobj[e] == 0 then g_Entity[e]['obj'] = 0 end
		for a = 1, g_EntityElementMax do
			if a ~= nil and g_Entity[a] ~= nil then				
				if g_Entity[a]['obj'] == selectobj[e] then
					local allegiance = GetEntityAllegiance(a)				
					pAllegiance = allegiance
					pEnt = a
					pName = GetEntityName(a)				
					break
				end
			end
		end	
		
		-- Selected Object in range ----------------------------------------------------------------------------------		
		if g_Entity[pEnt]['obj'] > 0 and pEnt ~= e and GetPlrLookingAtEx(pEnt,1) == 1 then
				
			if mousecontrol[e].pointer == 1 then TextCenterOnX(50,50,3,"+") end -- Crosshair Pointer here
			if mousecontrol[e].pointer == 2 then TextCenterOnX(50,50,3,"") end -- Crosshair Pointer here
			if mousecontrol[e].pointer == 3 then TextCenterOnX(50,50,3,"+") end -- Crosshair Pointer here
			
			if mousecontrol[e].pointer == 4 and pAllegiance < 0 then PasteSpritePosition(cursor,50,50) end -- Graphic Pointer here
			if mousecontrol[e].pointer == 5 and pAllegiance < 0 then PasteSpritePosition(cursor,50,50) end -- Graphic Pointer here
			if mousecontrol[e].pointer == 5 and pAllegiance == 1 then PasteSpritePosition(cursor1,50,50) end -- NPC Pointer here
			if mousecontrol[e].pointer == 5 and pAllegiance == 2 then PasteSpritePosition(cursor1,50,50) end -- NPC Pointer here
			if mousecontrol[e].pointer == 5 and pAllegiance == 0 then PasteSpritePosition(cursor2,50,50) end -- NPC Pointer here
			
			if mousecontrol[e].pointer == 5 and pAllegiance == -1 and GetEntityCollectable(pEnt) == 1 then PasteSpritePosition(cursor5,50,50) end -- NPC Pointer here
			
			if g_MouseClick == 1 and clicked[e] == 0 and mousecontrol[e].action_control == 1 then
				if pAllegiance < 0 then  						--<<<<<<<< Will change when allegiances expand (eg: breakables will be higher than -1)
					CurrentlyHeldWeaponID = GetPlayerWeaponID()
					SetPlayerWeapons(0)
					gunstate = 1
				end
				SetGamePlayerStatePlrKeyForceKeystate(18)
				--PlaySound(e,0)	-- making noise even if nothing actually collected!		
				clicked[e] = 1
				clicktime[e] = GetTimer(e)			
			end
			if g_MouseClick == 1 and clicked[e] == 0 and mousecontrol[e].action_control == 3 then
				if pAllegiance < 0 then  
					CurrentlyHeldWeaponID = GetPlayerWeaponID()
					SetPlayerWeapons(0)
					gunstate = 1
				end		
				SetGamePlayerStatePlrKeyForceKeystate(18)
				--PlaySound(e,0)	-- making noise even if nothing actually collected!		
				clicked[e] = 1
				clicktime[e] = GetTimer(e)			
			end		
			if g_MouseClick == 2 and clicked[e] == 0 and mousecontrol[e].action_control == 2 then
				SetGamePlayerStatePlrKeyForceKeystate(18)
				--PlaySound(e,0)	-- making noise even if nothing actually collected!		
				clicked[e] = 1
				clicktime[e] = GetTimer(e)
			end
			if g_MouseClick == 2 and clicked[e] == 0 and mousecontrol[e].action_control == 3 then			
				SetGamePlayerStatePlrKeyForceKeystate(18)
				--PlaySound(e,0)	-- making noise even if nothing actually collected!		
				clicked[e] = 1
				clicktime[e] = GetTimer(e)
			end		
			if g_MouseClick == 3  and clicked[e] == 0 and mousecontrol[e].action_control == 4 then			
				SetGamePlayerStatePlrKeyForceKeystate(18)
				--PlaySound(e,0)	-- making noise even if nothing actually collected!		
				clicked[e] = 1
				clicktime[e] = GetTimer(e)
			end			
		end		
		if GetTimer(e) > clicktime[e] + mousecontrol[e].timer_adjust and clicked[e] == 1 and g_MouseClick == 0 then
			SetGamePlayerStatePlrKeyForceKeystate(0)
			clicked[e] = 0
			status[e] = "reset"				
		end			
	end	
	if status[e] == "reset" then
		if clicked[e] == 0 and gunstate == 1 then
			ChangePlayerWeapon(last_gun)
			SetPlayerWeapons(1)
			ChangePlayerWeaponID(CurrentlyHeldWeaponID)
			gunstate = 0
		end		
		status[e] = "mouseplay"
	end
end
