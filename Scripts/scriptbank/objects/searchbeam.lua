-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Search Beam v12
-- DESCRIPTION: Edit the settings, [BEAM_RANGE=3000], Attach to [BEAM_OBJECT_NAME$=""] (eg: "searchlight1")

module_lightcontrol = require "scriptbank\\markers\\module_lightcontrol"
local rad = math.rad

g_searchbeam = {}
local beam = {}
local beam_range = {}
local beam_object_name = {}
local beam_object = {}
local status = {}
local attachTo = {}
local beamattached = {}	
local lightNum = GetEntityLightNumber( e )
local anglex = {}
local Axv = {}
local Ayv = {}
local Azv = {}

function searchbeam_properties(e, beam_range, beam_object_name, beam_object)	
	module_lightcontrol.init(e,1)	
	g_searchbeam[e]['beam_range'] = beam_range								-- Range of lightbeam
	g_searchbeam[e]['beam_object_name'] = string.lower(beam_object_name)	-- name of entity light attached to	
	g_searchbeam[e]['beam_object'] = 0							-- no of entity light attached to
end -- End properties

function searchbeam_init(e)
	g_searchbeam[e] = {}		
	g_searchbeam[e]['beam_range'] = 5000
	g_searchbeam[e]['beam_object_name'] = ""
	g_searchbeam[e]['beam_object'] = 0
	attachTo[e] = 0	
	anglex[e] = 45
	Axv[e] = 0	
	Ayv[e] = 0	
	Azv[e] = 0	
	lightNum = GetEntityLightNumber( e )
	beamattached[e] = 0
	status[e] = "init"
	SetEntityAlwaysActive(e,1)
end
	
function searchbeam_main(e)	
	if status[e] == "init" then
		Axv[e], Ayv[e], Azv[e] = GetLightAngle(lightNum)
		if g_searchbeam[e]['beam_object'] == 0 or nil then
			for a = 1, g_EntityElementMax do			
				if a ~= nil and g_Entity[a] ~= nil then		
					if string.lower(GetEntityName(a)) == string.lower(g_searchbeam[e]['beam_object_name']) then
						g_searchbeam[e]['beam_object'] = a
						attachTo[e] = g_searchbeam[e]['beam_object']
						break
					end					
				end
			end
		end
		status[e] = "initdone"
	end
	
	if attachTo[e] == -1 then return end
	if beamattached[e] == 0 then	
		lightNum = GetEntityLightNumber( e )
		local x,y,z,Ax,Ay,Az = GetEntityPosAng(attachTo[e])
		Ax=Ax-(Axv[e]*9)
		local xA,yA,zA = rad(Ax),rad(Ay),rad(Az)
		x=x+math.sin(math.rad(Ay))*20
		y=y-math.sin(math.rad(Ax))*20
		z=z+math.cos(math.rad(Ay))*20
		--Set Light Position
		SetLightPosition(lightNum,x,y,z)
		SetLightAngle(lightNum,Ax,-yA/3,zA)		
		SetLightRange(lightNum,g_searchbeam[e]['beam_range'])
		beamattached[e] = 1
	end
	if beamattached[e] == 1 then	
		lightNum = GetEntityLightNumber( e )		
		local x,y,z,Ax,Ay,Az = GetEntityPosAng(attachTo[e])		
		Ax=Ax-(Axv[e]*9)
		local xA,yA,zA = rad(Ax),rad(Ay),rad(Az)
		x=x+math.sin(math.rad(Ay))*20
		y=y-math.sin(math.rad(Ax))*20
		z=z+math.cos(math.rad(Ay))*20
		--Set Light Position
		SetLightPosition(lightNum,x,y,z)
		SetLightAngle(lightNum,xA,-yA/3,zA)
		SetLightRange(lightNum,g_searchbeam[e]['beam_range'])
	end	
	if g_Entity[g_searchbeam[e]['beam_object']]['health'] <= 80 then
		lightNum = GetEntityLightNumber( e )
		local nRandom = math.random(0,2000)
		if nRandom > 50 then
			SetLightRange(lightNum,50)			
		else   
			SetLightRange(lightNum,g_searchbeam[e]['beam_range'])
		end
	end	
	if g_Entity[g_searchbeam[e]['beam_object']]['health'] <= 10 then
		lightNum = GetEntityLightNumber( e )
		beamattached[e] = 2
		SetLightRange(lightNum,-1)
		module_lightcontrol.control(lightNum,0)
		Destroy(lightNum)
		Hide(lightNum)
	end
end