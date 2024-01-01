-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Npc Flashlight v4
-- DESCRIPTION: Attach to a spotlight and edit the settings.
-- DESCRIPTION: [FLASHLIGHT_RANGE=3000]
-- DESCRIPTION: Attach to [NPC_OBJECT_NAME$="Tony1"]
-- DESCRIPTION: Select [@FLASHLIGHT_POSITION=1(1=Hand, 2=Shoulder, 3=Head)]

	module_lightcontrol = require "scriptbank\\markers\\module_lightcontrol"
	local rad = math.rad
	
	g_npcflashlight = {}
	local flashlight = {}
	local flashlight_range = {}	
	local npc_object_name = {}
	local npc_object = {}
	local flashlight_position = {}
	local status = {}
	local attachTo = {}
	local flashattached = {}	
	local lightNum = GetEntityLightNumber(e)
	local tpositionx = {}
	local tpositiony = {}
	local tpositionz = {}

function npc_flashlight_properties(e, flashlight_range, npc_object_name, npc_object, flashlight_position)	
	module_lightcontrol.init(e,1)	
	g_npcflashlight[e]['flashlight_range'] = flashlight_range				-- Range of lightbeam
	g_npcflashlight[e]['npc_object_name'] = string.lower(npc_object_name)	-- name of entity light attached to	
	g_npcflashlight[e]['npc_object'] = 0									-- no of entity light attached to
	g_npcflashlight[e]['flashlight_position'] = flashlight_position
end -- End properties

function npc_flashlight_init(e)
	g_npcflashlight[e] = {}		
	g_npcflashlight[e]['flashlight_range'] = 5000
	g_npcflashlight[e]['npc_object_name'] = "Tony1"
	g_npcflashlight[e]['npc_object'] = 0
	g_npcflashlight[e]['flashlight_position'] = 1
	attachTo[e] = 0	
	tpositionx[e] = 0
	tpositiony[e] = 0
	tpositionz[e] = 0
	lightNum = GetEntityLightNumber(e)
	flashattached[e] = 0
	status[e] = "init"
end
	
function npc_flashlight_main(e)	
	if status[e] == "init" then
		xv, yv, zv = GetLightAngle(lightNum)
		if g_npcflashlight[e]['npc_object'] == 0 or nil then
			for a = 1, g_EntityElementMax do			
				if a ~= nil and g_Entity[a] ~= nil then		
					if string.lower(GetEntityName(a)) == string.lower(g_npcflashlight[e]['npc_object_name']) then
						g_npcflashlight[e]['npc_object'] = a
						attachTo[e] = g_npcflashlight[e]['npc_object']
						break
					end					
				end
			end
		end	
		if g_npcflashlight[e]['flashlight_position'] == nil then g_npcflashlight[e]['flashlight_position'] = 1 end
		if g_npcflashlight[e]['flashlight_position'] == 1 then --Hand
			tpositionx[e] = 10
			tpositiony[e] = 50
			tpositionz[e] = 1
		end
		if g_npcflashlight[e]['flashlight_position'] == 2 then --Shoulder
			tpositionx[e] = 8
			tpositiony[e] = 45
			tpositionz[e] = 1
		end
		if g_npcflashlight[e]['flashlight_position'] == 3 then --Head
			tpositionx[e] = 0
			tpositiony[e] = 40
			tpositionz[e] = 1
		end		
		status[e] = "initdone"
	end
	if attachTo[e] == -1 then return end
	if flashattached[e] == 0 then		
		lightNum = GetEntityLightNumber( e )
		local x,y,z,Ax,Ay,Az = GetEntityPosAng(attachTo[e])
		Ax=Ax-(xv*6)
		local xA,yA,zA = rad(Ax),rad(Ay),rad(Az)
		x=x+math.sin(math.rad(Ay))*20
		y=y-math.sin(math.rad(Ax))*20
		z=z+math.cos(math.rad(Ay))*20
		--Set Light Position
		SetLightPosition(lightNum, x - tpositionx[e], y + tpositiony[e], z - tpositionz[e])
		SetLightAngle(lightNum,Ax,-yA/3,zA)		
		SetLightRange(lightNum,g_npcflashlight[e]['flashlight_range'])
		flashattached[e] = 1
	end
	if flashattached[e] == 1 then	
		lightNum = GetEntityLightNumber( e )		
		local x,y,z,Ax,Ay,Az = GetEntityPosAng(attachTo[e])		
		Ax=Ax-(xv*6)
		local xA,yA,zA = rad(Ax),rad(Ay),rad(Az)
		x=x+math.sin(math.rad(Ay))*20
		y=y-math.sin(math.rad(Ax))*20
		z=z+math.cos(math.rad(Ay))*20
		--Set Light Position
		SetLightPosition(lightNum, x - tpositionx[e], y + tpositiony[e], z - tpositionz[e])
		SetLightAngle(lightNum,xA,-yA/3,zA)
		SetLightRange(lightNum,g_npcflashlight[e]['flashlight_range'])
	end	
	if g_Entity[g_npcflashlight[e]['npc_object']]['health'] <= 80 then
		lightNum = GetEntityLightNumber( e )
		local nRandom = math.random(0,2000)
		if nRandom > 50 then
			SetLightRange(lightNum,50)			
		else   
			SetLightRange(lightNum,g_npcflashlight[e]['flashlight_range'])
		end
	end	
	if g_Entity[g_npcflashlight[e]['npc_object']]['health'] <= 10 then
		lightNum = GetEntityLightNumber( e )
		flashattached[e] = 2
		SetLightRange(lightNum,-1)
		module_lightcontrol.control(lightNum,0)
		Destroy(lightNum)
		Hide(lightNum)
	end
end