-- Add_Fx v8 by Necrym59 and Lee
-- DESCRIPTION: Will add the selected effects to the named object.
-- DESCRIPTION: Attach to an object. Set Always active ON
-- DESCRIPTION: [ObjectName$=""]
-- DESCRIPTION: [Hover!=1]
-- DESCRIPTION: [HoverHeight=20(1,100)]
-- DESCRIPTION: [LowerHeight=0(0,100)]
-- DESCRIPTION: [HoverSpeed=100(1,200)]
-- DESCRIPTION: [Spin!=0]
-- DESCRIPTION: [SpinSpeed=10(1,500)]
-- DESCRIPTION: [spin_x!=0]
-- DESCRIPTION: [spin_y!=1]
-- DESCRIPTION: [spin_z!=0]
-- DESCRIPTION: [AntiClockwise!=0]
-- DESCRIPTION: [Glow!=0]
-- DESCRIPTION: [EmmisiveStrength=100(0,1000)]
-- DESCRIPTION: [ActiveAtStart!=1] if unchecked use a switch or zone trigger to activate.
-- DESCRIPTION: [PulseGlow!=0]
-- DESCRIPTION: [#PulseSpeed=0.5(0.1,10.0)]

local lower = string.lower

local addfx 			= {}
local objectname		= {}
local objectno			= {}
local hover				= {}
local hoverheight		= {}
local hoverlowerheight	= {}
local hoverspeed		= {}
local spin				= {}
local spinspeed			= {}
local spin_x			= {}
local spin_y			= {}
local spin_z			= {}
local anticlockwise		= {}
local glow				= {}
local pulseglow			= {}
local pulsespeed		= {}
local emissivestrength	= {}
local ActivateAtStart	= {}

local status			= {}
local hover_currentY	= {}
local hover_heightangle = {}
local glow_currentEM 	= {}
local rampingEM 		= {}
local tspinspeed		= {}
local objEnt			= {}

function add_fx_properties(e, objectname, hover, hoverheight, lowerheight, hoverspeed, spin, spinspeed, spin_x, spin_y, spin_z, anticlockwise, glow, emissivestrength, ActivateAtStart, pulseglow, pulsespeed)
	addfx[e].objectname = lower(objectname)
	addfx[e].objectno = 0
	addfx[e].hover = hover
	addfx[e].hoverheight = hoverheight
	addfx[e].hoverlowerheight = lowerheight
	addfx[e].hoverspeed = hoverspeed
	addfx[e].spin = spin
	addfx[e].spinspeed = spinspeed
	addfx[e].spin_x = spin_x
	addfx[e].spin_y = spin_y
	addfx[e].spin_z = spin_z
	addfx[e].anticlockwise = anticlockwise
	addfx[e].glow = glow
	addfx[e].pulseglow = pulseglow
	addfx[e].pulsespeed = pulsespeed
	addfx[e].emissivestrength = emissivestrength
	addfx[e].ActivateAtStart = ActivateAtStart
end

function add_fx_init(e)
	addfx[e] = {}
	addfx[e].objectname = ""
	addfx[e].objectno = 0
	addfx[e].hover = 1
	addfx[e].hoverheight = 20
	addfx[e].hoverlowerheight = 0
	addfx[e].hoverspeed = 100
	addfx[e].spin = 0
	addfx[e].spinspeed = 10
	addfx[e].spin_x = 0
	addfx[e].spin_y = 1
	addfx[e].spin_z = 0
	addfx[e].anticlockwise = 0
	addfx[e].glow = 0
	addfx[e].pulseglow = 0
	addfx[e].pulsespeed = 0
	addfx[e].emissivestrength = 0
	addfx[e].ActivateAtStart = 1
	status[e] = "init"
	rampingEM[e] = 0
	objEnt[e] = 0
end

function add_fx_main(e)
	if status[e] == "init" then
		hover_currentY[e] = addfx[e].hoverlowerheight
		hover_heightangle[e] = 0
		tspinspeed[e] = 0
		glow_currentEM[e] = 0
		if addfx[e].objectno == 0 then
			for n = 1, g_EntityElementMax do
				if n ~= nil and g_Entity[n] ~= nil then
					if lower(GetEntityName(n)) == addfx[e].objectname then
						addfx[e].objectno = n
						objEnt[e] = n
						break
					end
				end
			end
		end
		SetEntityEmissiveStrength(objEnt[e],0)
		if addfx[e].objectno ~= 0 or nil then
			if addfx[e].ActivateAtStart == 1 then SetActivated(e,1) end
			if addfx[e].ActivateAtStart == 0 then SetActivated(e,0) end
		end
		status[e] = "start"
	end

	if g_Entity[e].activated == 1 then

		if addfx[e].hover == 1 then
			GravityOff(objEnt[e])
			if status[e] == "start" then
				local nhoverheight = addfx[e].hoverheight
				local lhoverheight = addfx[e].hoverlowerheight
				if hover_currentY[e] < addfx[e].hoverheight then
					hover_heightangle[e] = hover_heightangle[e] + (GetAnimationSpeed(objEnt[e])/addfx[e].hoverspeed)
					local fFinalY = (GetGroundHeight(g_Entity[objEnt[e]].x,g_Entity[objEnt[e]].z)+ lhoverheight) + nhoverheight - (math.cos(hover_heightangle[e])*nhoverheight)
					CollisionOff(objEnt[e])
					SetPosition(objEnt[e],g_Entity[objEnt[e]].x,fFinalY,g_Entity[objEnt[e]].z)
					CollisionOn(objEnt[e])
				end
				if hover_currentY[e] >= addfx[e].hoverheight then status[e] = "hovering" end
			end
			if status[e] == "hovering" then
				local nhoverheight = addfx[e].hoverheight
				local lhoverheight = addfx[e].hoverlowerheight
				hover_heightangle[e] = hover_heightangle[e] + (GetAnimationSpeed(objEnt[e])/addfx[e].hoverspeed)
				local fFinalY = (GetGroundHeight(g_Entity[objEnt[e]].x,g_Entity[objEnt[e]].z)+ lhoverheight) + nhoverheight + (math.cos(hover_heightangle[e])*nhoverheight)
				CollisionOff(objEnt[e])
				SetPosition(objEnt[e],g_Entity[objEnt[e]].x,fFinalY,g_Entity[objEnt[e]].z)
				CollisionOn(objEnt[e])
			end
		end

		if addfx[e].spin == 1 then
			CollisionOff(objEnt[e])
			if addfx[e].anticlockwise == 0 then
				tspinspeed[e] = addfx[e].spinspeed
			else
				tspinspeed[e] = addfx[e].spinspeed * -1
			end
			if addfx[e].spin_x == 1 then
				CollisionOff(objEnt[e])
				RotateX(objEnt[e],GetAnimationSpeed(objEnt[e])*tspinspeed[e])
				CollisionOn(objEnt[e])
			end
			if addfx[e].spin_y == 1 then
				CollisionOff(objEnt[e])
				RotateY(objEnt[e],GetAnimationSpeed(objEnt[e])*tspinspeed[e])
				CollisionOn(objEnt[e])
			end
			if addfx[e].spin_z == 1 then
				CollisionOff(objEnt[e])
				RotateZ(objEnt[e],GetAnimationSpeed(objEnt[e])*tspinspeed[e])
				CollisionOn(objEnt[e])
			end
		end

		if addfx[e].glow == 1 then
			if glow_currentEM[e] < addfx[e].emissivestrength then
				SetEntityEmissiveStrength(objEnt[e],glow_currentEM[e]*30)
				glow_currentEM[e] = glow_currentEM[e]+0.5
			end
		end
		if addfx[e].pulseglow == 1 then
			if glow_currentEM[e] < addfx[e].emissivestrength and rampingEM[e] == 0 then
				SetEntityEmissiveStrength(objEnt[e],glow_currentEM[e])
				glow_currentEM[e] = glow_currentEM[e]+addfx[e].pulsespeed
				if glow_currentEM[e] >= addfx[e].emissivestrength then rampingEM[e] = 1 end
			end
			if glow_currentEM[e] > 0 and rampingEM[e] == 1 then
				SetEntityEmissiveStrength(objEnt[e],glow_currentEM[e])
				glow_currentEM[e] = glow_currentEM[e]-addfx[e].pulsespeed
				if glow_currentEM[e] <= 0 then rampingEM[e] = 0 end
			end
		end
	end
end