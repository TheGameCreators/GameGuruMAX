-- Fade Out Sound v2
-- DESCRIPTION: Fade out the audio <Sound0> based on the [RANGE=1000(100,3000)] distance the player is from the object.
-- DESCRIPTION: The sound will fade out between the [MINVOLUME=0(0,99)] and the [MAXVOLUME=100(1,100)] values.

g_fadesound_instance = 0
g_fadesound_volume = 0
g_fadeoutsound = {}

local range 	= {}
local minvolume = {}
local maxvolume = {}

local nrange	= {}
local nminvol	= {}
local nmaxvol	= {}
local MaxDist	= {}
local SndPerc	= {}
local SndVol	= {}

function fadeoutsound_properties(e, range, minvolume, maxvolume)
	g_fadeoutsound[e]['range'] = range
	g_fadeoutsound[e]['minvolume'] = minvolume
	g_fadeoutsound[e]['maxvolume'] = maxvolume
end

function fadeoutsound_init(e)
	g_fadeoutsound[e] = {}
	g_fadeoutsound[e]['range'] = 1000
	g_fadeoutsound[e]['minvolume'] = 0
	g_fadeoutsound[e]['maxvolume'] = 100
end

function fadeoutsound_main(e)
	local PlayerDist = GetPlayerDistance(e)
	nrange[e] = g_fadeoutsound[e]['range']

	if PlayerDist < nrange[e] then
		nminvol[e] = g_fadeoutsound[e]['minvolume']
		nmaxvol[e] = g_fadeoutsound[e]['maxvolume']
		MaxDist[e] = nrange[e]
		SndPerc[e] = (MaxDist[e]-PlayerDist)/MaxDist[e]
		SndVol[e] = 0
		if SndPerc[e] > 0 then
			SndVol[e] = 100-(SndPerc[e]*100)
		else
			SndVol[e] = 100
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
