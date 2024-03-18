-- Loop 3d Sound Multiple v2
-- DESCRIPTION: Loop all audio specified in <Sound0>, <Sound1>, <Sound2>, and <Sound3> at the volume based on the
-- DESCRIPTION: [RANGE=1000(100,3000)] distance the player is away from the object, between
-- DESCRIPTION: [MinVolume=0(0,99)] and [MaxVolume=100(1,100)]
-- DESCRIPTION: [ActivateAtStart!=1] if unchecked use a switch or zone trigger to activate.

module_activationcontrol = require "scriptbank\\module_activationcontrol"
loop3dsound_multiple = {}
local status = {}

function loop3dsound_multiple_properties(e, range, minvolume, maxvolume, ActivateAtStart)
	module_activationcontrol.init(e,1)
	loop3dsound_multiple[e]['range'] = range 
	loop3dsound_multiple[e]['minvolume'] = minvolume
	loop3dsound_multiple[e]['maxvolume'] = maxvolume
	loop3dsound_multiple[e]['ActivateAtStart'] = ActivateAtStart	
end 

function loop3dsound_multiple_init(e)
	loop3dsound_multiple[e] = {}
	loop3dsound_multiple[e]['range'] = 1000
	loop3dsound_multiple[e]['minvolume'] = 0
	loop3dsound_multiple[e]['maxvolume'] = 100
	loop3dsound_multiple[e]['ActivateAtStart'] = 1
	
	status[e] = "init"
end

function loop3dsound_multiple_main(e)

	if status[e] == "init" then
		if loop3dsound_multiple[e]['ActivateAtStart'] == 0 then SetActivated(e,0) end
		if loop3dsound_multiple[e]['ActivateAtStart'] == 1 then SetActivated(e,1) end
		status[e] = "endinit"
	end

	if g_Entity[e].activated == 1 then
		module_activationcontrol.init(e,1)
		if module_activationcontrol.control(e) == 1 then
			local PlayerDist = GetPlayerDistance(e)
			local nrange = loop3dsound_multiple[e]['range']
			if PlayerDist < nrange then	
				local nminvol = loop3dsound_multiple[e]['minvolume']
				local nmaxvol = loop3dsound_multiple[e]['maxvolume']
				local MaxDist = nrange
				local SndVol = 0
				local SndPerc = (MaxDist-PlayerDist)/MaxDist
				if SndPerc > 0 then
					SndVol = 50+(SndPerc*50)
				else
					SndVol = 0
				end	
				LoopSound(e,0) --LoopNon3DSound(e,0)
				LoopSound(e,1) --LoopNon3DSound(e,1)
				LoopSound(e,2) --LoopNon3DSound(e,2)
				LoopSound(e,3) --LoopNon3DSound(e,3)
				if SndVol < nminvol then 
					SndVol = nminv
				elseif SndVol > nmaxvol then 
					SndVol = nmaxvol
				end		
				SetSoundVolume(SndVol)
			else
				StopSound(e,0)
				StopSound(e,1)
				StopSound(e,2)
				StopSound(e,3)
			end
		else
			StopSound(e,0)
			StopSound(e,1)
			StopSound(e,2)
			StopSound(e,3)
		end
	end
	
	if g_Entity[e].activated == 0 then		
		StopSound(e,0)
		StopSound(e,1)
		StopSound(e,2)
		StopSound(e,3)
		module_activationcontrol.init(e,0)
		SetActivated(e,0)
	end		
end
