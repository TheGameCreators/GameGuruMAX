-- Weight Switch v11 - Necrym59 with special thanks to Amen Moses
-- DESCRIPTION: Attach to the Weight Switch Object? This object will be treated as a switch object for activating other objects or game elements.
-- DESCRIPTION: Set Object to Physics=ON, Collision=BOX, IsImobile=ON. Use AlphaClipping to make invisible if required.
-- DESCRIPTION: [PROMPT_TEXT$="Weight needed to activate"]
-- DESCRIPTION: [ACTIVATION_TEXT$="You have activated the switch"]
-- DESCRIPTION: [ACTIVATION_WEIGHT=300(0,1000)]
-- DESCRIPTION: [PLAYER_WEIGHT=100(0,100)]
-- DESCRIPTION: [MOVEMENT=3(1,10)]
-- DESCRIPTION: Play <Sound0> when used.

local U = require "scriptbank\\utillib"
local P = require "scriptbank\\physlib"
local max = math.max
local rad = math.rad

local switches = {}
local rayCast      = IntersectAll
local getObjPosAng = GetObjectPosAng
local doonce = {}
	
function weight_switch_properties(e, prompt_text, activation_text, activation_weight, player_weight, switch_movement)
	local switch = switches[e]
	switch.prompt_text       = prompt_text	
	switch.activation_text   = activation_text
	switch.activation_weight = activation_weight
	switch.player_weight     = player_weight
	switch.switch_movement   = switch_movement
end 

function weight_switch_init(e)
	switches[e] = { prompt_text       = "Weight needed to activate",
	                activation_text   = "You have activated the switch",
	                activation_weight = 300,
	                player_weight     = 100,
                    switch_movement   = 10,
					playerOn          = false,
	                state             = 'init'
				  }
				  
	doonce[e] = 0
end 

local gEnt = g_Entity

local function getCentre(p,x,y,z,xa,ya,za)
	local dims = p.dims
	local xo,yo,zo = U.Rotate3D(dims.cx,dims.cy,dims.cz,xa,ya,za)
	return x+xo,y+yo,z+zo
end

local function isSatOn(pb,bpx,bpy,bpz,bax,bay,baz,pt,tpx,tpy,tpz,tax,tay,taz)
	if tpy < bpy then return false end
	local tcx, tcy, tcz = getCentre( pt, tpx, tpy, tpz, tax, tay, taz )
	local lowestY = tcy - ( max( pt.dims.h, pt.dims.w, pt.dims.l ) / 2 + 5 )
	local bObj = rayCast( tcx, tcy, tcz, tcx, lowestY , tcz, pt.obj )
	if bObj == pb.obj then return true end
	bObj = rayCast( tcx + 1, tcy, tcz + 1, tcx + 1, lowestY, tcz + 1, pt.obj )
	if bObj == pb.obj then return true end
	bObj = rayCast( tcx + 1, tcy, tcz - 1, tcx + 1, lowestY, tcz - 1, pt.obj )
	if bObj == pb.obj then return true end
	bObj = rayCast( tcx - 1, tcy, tcz + 1, tcx + 1, lowestY, tcz + 1, pt.obj )
	if bObj == pb.obj then return true end
	bObj = rayCast( tcx - 1, tcy, tcz - 1, tcx + 1, lowestY, tcz - 1, pt.obj )
	if bObj == pb.obj then return true end
end

local function buildStackList(e,elist,p,pb)
	local pbx,pby,pbz,abx,aby,abz = getObjPosAng(p.obj)
	local StackList = {}
	for k, v in pairs(elist) do
		if v ~= e and
		   (not PU_GetEntityCarried or PU_GetEntityCarried() ~= v) then
			local Ent = gEnt[ v ]
			local t = {obj = Ent.obj, dims = P.GetObjectDimensions(Ent.obj)}
			local tpx, tpy, tpz, tax, tay, taz = getObjPosAng(t.obj)			
			if isSatOn(p,pbx,pby,pbz,rad(abx),rad(aby),rad(abz),t,tpx,tpy,tpz,rad(tax),rad(tay),rad(taz)) then
				StackList[#StackList + 1] = {ent = v,obj = t.obj}
				pb.stackList[#pb.stackList + 1] = {ent = v,obj = t.obj} 
				elist[k] = nil
			end
		end
	end		
	if #StackList > 0 then
		for _, v in pairs(StackList) do
			buildStackList(v.ent,elist,v,pb)
		end
	end
end

local function checkWeight(e,switch,Ent)
	switch.stackList = {}
	local elist = U.ClosestEntities(switch.maxd,50,Ent.x,Ent.z)
	if #elist > 1 then
		buildStackList(e,elist,switch,switch)
	end
	switch.accrued_weight = 0
	for _, v in pairs(switch.stackList) do
		switch.accrued_weight = switch.accrued_weight + GetEntityWeight(v.ent)
	end
	if U.PlayerCloserThanPos(Ent.x,Ent.y + switch.dims.h, Ent.z, switch.maxd) then
		switch.playerOn = true
		switch.accrued_weight = switch.accrued_weight + switch.player_weight					
	end
end

function weight_switch_main(e)
	local switch = switches[e]
	if switch == nil then return end
	local Ent = gEnt[e]
	if switch.state == 'init' then
		switch.obj  = Ent.obj
		switch.ypos = Ent.y
		switch.dims = P.GetObjectDimensions(switch.obj)
		switch.maxd = max(switch.dims.l,switch.dims.w) * 0.75
		switch.activated = false		
		switch.state = 'checkweight'
	elseif 
	   switch.state == 'checkweight' then
		checkWeight( e, switch, Ent )
		switch.timer = g_Time + 200
		switch.state = 'active'	
	elseif 
	   switch.state == 'active' then
		if g_Time > switch.timer then
			if switch.accrued_weight < switch.activation_weight then			
				if switch.playerOn then PromptDuration(switch.prompt_text,500) end
				switch.playerOn = false
				switch.state = 'checkweight'
			elseif 
			   not switch.activated then
				PromptDuration(switch.activation_text,3000)
				ActivateIfUsed(e)
				if PerformLogicConnections ~= nil then
					PerformLogicConnections(e)
				end
				PlaySound(e,0)
				switch.activated = true
				switch.state = 'activated'
				return
			end
		end		
	elseif 
	   switch.state == 'activated' then
		for _, v in pairs( switch.stackList ) do
			PushObject(v.obj,0,0,0)
		end
		if g_Time > switch.timer then
			checkWeight(e,switch,Ent)
			switch.timer = g_Time + 200
			if switch.accrued_weight < switch.activation_weight then
				switch.state = 'checkweight'
				switch.activated = false
			end
			switch.timer = g_Time + 200
		end
	end

	if switch.state == 'active' and 
	   Ent.y < switch.ypos then
		CollisionOff(e)
		PositionObject(switch.obj,Ent.x,Ent.y + 0.5,Ent.z)
		CollisionOn(e)
		
	elseif
	   switch.state == 'activated' and 
	   Ent.y > switch.ypos - switch.switch_movement then
		CollisionOff(e)
		PositionObject( switch.obj,Ent.x,Ent.y - 0.5,Ent.z)
		CollisionOn(e)
		switch.state = 'end'
	end
end
