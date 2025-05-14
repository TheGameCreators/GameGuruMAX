-- Loop 3d Sound v2
-- DESCRIPTION: Loop the audio <Sound0> at the volume based on the 
-- DESCRIPTION: [RANGE=1000(100,3000)] distance the player is away from the object, between
-- DESCRIPTION: [MinVolume=0(0,99)] and
-- DESCRIPTION: [MaxVolume=100(1,100)]
-- DESCRIPTION: [ActivateAtStart!=1] if unchecked use a switch or zone trigger to activate.

module_activationcontrol = require "scriptbank\\module_activationcontrol"
g_loop3dsound = {}
g_loop3dsound = {}
local status = {}

function loop3dsound_properties(e, range, minvolume, maxvolume, ActivateAtStart)
	g_loop3dsound[e]['range'] = range 
	g_loop3dsound[e]['minvolume'] = minvolume
	g_loop3dsound[e]['maxvolume'] = maxvolume
	g_loop3dsound[e]['ActivateAtStart'] = ActivateAtStart
end
 
function loop3dsound_init(e)
	g_loop3dsound[e] = {}
	g_loop3dsound[e]['range'] = 1000
	g_loop3dsound[e]['minvolume'] = 0
	g_loop3dsound[e]['maxvolume'] = 100
 	g_loop3dsound[e]['ActivateAtStart'] = 1
	
	status[e] = "init"
end	

function loop3dsound_main(e)

	if status[e] == "init" then
		if g_loop3dsound[e]['ActivateAtStart'] == 0 then SetActivated(e,0) end
		if g_loop3dsound[e]['ActivateAtStart'] == 1 then SetActivated(e,1) end
		status[e] = "endinit"
	end

	if g_Entity[e].activated == 1 then
		module_activationcontrol.init(e,1)
		if module_activationcontrol.control(e) == 1 then
			local PlayerDist = GetPlayerDistance(e)
			local nrange = g_loop3dsound[e]['range']
			if PlayerDist < nrange or GetEntityAlwaysActive(e) == 1 then	
				local nminvol = g_loop3dsound[e]['minvolume']
				local nmaxvol = g_loop3dsound[e]['maxvolume']
				local MaxDist = nrange
				local SndVol = 0
				local SndPerc = (MaxDist-PlayerDist)/MaxDist
				if SndPerc > 0 then
					SndVol = (SndPerc*100)
				else
					SndVol = 0
				end	
				LoopSound(e,0) --LoopNon3DSound(e,0) 
				if SndVol < nminvol then 
					SndVol = nminvol
				elseif SndVol > nmaxvol then 
					SndVol = nmaxvol
				end		
				SetSoundVolume(SndVol)
			else
				if GetEntityAlwaysActive(e) == 0 then
					StopSound(e,0)
				end
			end
		else
			if GetEntityAlwaysActive(e) == 0 then
				StopSound(e,0)
			end
		end
	end
	if g_Entity[e].activated == 0 then		
		StopSound(e,0)		
		module_activationcontrol.init(e,0)
		SetActivated(e,0)
	end	
end
