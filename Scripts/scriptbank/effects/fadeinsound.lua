-- Fade In Sound v2
-- DESCRIPTION: Plays the audio <Sound0> at the volume based on the [RANGE=1000(100,3000)] distance the player is away from the object.
-- DESCRIPTION: The sound will fade in between the [MINVOLUME=0(0,99)] and the [MAXVOLUME=100(1,100)] values.

g_fadesound_instance = 0
g_fadesound_volume = 0
g_fadeinsound = {}

local range 	= {}
local minvolume = {}
local maxvolume = {}

local nrange	= {}
local nminvol	= {}
local nmaxvol	= {}
local MaxDist	= {}
local SndPerc	= {}
local SndVol	= {}

function fadeinsound_properties(e, range, minvolume, maxvolume)
	g_fadeinsound[e]['range'] = range
	g_fadeinsound[e]['minvolume'] = minvolume
	g_fadeinsound[e]['maxvolume'] = maxvolume
end

function fadeinsound_init(e)
	g_fadeinsound[e] = {}
	g_fadeinsound[e]['range'] = 1000
	g_fadeinsound[e]['minvolume'] = 0
	g_fadeinsound[e]['maxvolume'] = 100
end

function fadeinsound_main(e)
	local PlayerDist = GetPlayerDistance(e)
	nrange[e] = g_fadeinsound[e]['range']

	if PlayerDist < nrange[e] then
		nminvol[e] = g_fadeinsound[e]['minvolume']
		nmaxvol[e] = g_fadeinsound[e]['maxvolume']
		MaxDist[e] = nrange[e]
		SndPerc[e] = (MaxDist[e]-PlayerDist)/MaxDist[e]
		SndVol[e] = 0
		if SndPerc[e] > 0 then
			SndVol[e] = SndPerc[e]*100
		else
			SndVol[e] = 0
		end
		if g_fadesound_instance == 0 then
			g_fadesound_instance = e
		end
		if g_fadesound_instance == e then
			LoopNon3DSound(e,0)
			if SndVol[e] < nminvol[e] then
				SndVol[e] = nminvol[e]
			elseif SndVol[e] > nmaxvol[e] then
				SndVol[e] = nmaxvol[e]
			end
			g_fadesound_volume = SndVol[e]
			SetSoundVolume(g_fadesound_volume)
		end
	end
end
