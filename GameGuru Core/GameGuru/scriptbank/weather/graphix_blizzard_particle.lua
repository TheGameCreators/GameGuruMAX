-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- DESCRIPTION: Graphix's blizzard script
-- DESCRIPTION: [FOLLOWPLAYER!=1] (1 = yes, 0 = no - static placement)
-- DESCRIPTION: SNOW SETTINGS:
-- DESCRIPTION: [SANIMSPD=1], [SSTARTRANDANGLE=0], [SFREQUENCY=24], [SMINLIFE=2000], [SMAXLIFE=2000]
-- DESCRIPTION: [SOFFX1=-500], [SOFFY1=80], [SOFFZ1=-500], [SOFFX2=500], [SOFFY2=100], [SOFFZ2=500],
-- DESCRIPTION: [SSPDX1=0], [SSPDY1=-0.8], [SSPDZ1=0], [SSPDX2=0], [SSPDY2=-1.2], [SSPDZ2=0],
-- DESCRIPTION: [SROTATEX=0], [SROTATEZ=0], [SSCALESTARTMIN=300], [SSCALESTARTMAX=400], [SSCALEENDMIN=30], [SSCALEENDMAX=60]
-- DESCRIPTION: [SALPHASTARTMIN=90], [SALPHASTARTMAX=190], [SALPHAENDMIN=80], [SALPHAENDMAX=100]
-- DESCRIPTION: MIST SETTINGS:
-- DESCRIPTION: [MANIMSPD=1], [MSTARTRANDANGLE=0], [MFREQUENCY=16], [MMINLIFE=2000], [MMAXLIFE=2000]
-- DESCRIPTION: [MOFFX1=-700], [MOFFY1=80], [MOFFZ1=-700], [MOFFX2=700], [MOFFY2=100], [MOFFZ2=700],
-- DESCRIPTION: [MSPDX1=0], [MSPDY1=-0.8], [MSPDZ1=0], [MSPDX2=0], [MSPDY2=-1.2], [MSPDZ2=0],
-- DESCRIPTION: [MROTATEX=0], [MROTATEZ=0], [MSCALESTARTMIN=500], [MSCALESTARTMAX=600], [MSCALEENDMIN=300], [MSCALEENDMAX=400]
-- DESCRIPTION: [MALPHASTARTMIN=8], [MALPHASTARTMAX=16], [MALPHAENDMIN=8], [MALPHAENDMAX=16]

g_graphix_blizzard_particle = {}

function graphix_blizzard_particle_properties(e, followplayer, sanimspd, sstartrandangle, sfrequency, sminlife, smaxlife, soffx1, soffy1, soffz1, soffx2, soffy2, soffz2, 
	sspdx1, sspdy1, sspdz1, sspdx2, sspdy2, sspdz2, srotatex, srotatez, sscalestartmin, sscalestartmax, sscaleendmin, sscaleendmax,
	salphastartmin, salphastartmax, salphaendmin, salphaendmax, manimspd, mstartrandangle, mfrequency, mminlife, mmaxlife, moffx1, moffy1, moffz1, moffx2, moffy2, moffz2, 
	mspdx1, mspdy1, mspdz1, mspdx2, mspdy2, mspdz2, mrotatex, mrotatez, mscalestartmin, mscalestartmax, mscaleendmin, mscaleendmax,
	malphastartmin, malphastartmax, malphaendmin, malphaendmax)
	g_graphix_blizzard_particle[e]['followplayer'] = followplayer
	g_graphix_blizzard_particle[e]['sanimspd'] = sanimspd
	g_graphix_blizzard_particle[e]['sstartrandangle'] = sstartrandangle
	g_graphix_blizzard_particle[e]['sfrequency'] = sfrequency
	g_graphix_blizzard_particle[e]['sminlife'] = sminlife
	g_graphix_blizzard_particle[e]['smaxlife'] = smaxlife
	g_graphix_blizzard_particle[e]['soffx1'] = soffx1
	g_graphix_blizzard_particle[e]['soffy1'] = soffy1
	g_graphix_blizzard_particle[e]['soffz1'] = soffz1
	g_graphix_blizzard_particle[e]['soffx2'] = soffx2
	g_graphix_blizzard_particle[e]['soffy2'] = soffy2
	g_graphix_blizzard_particle[e]['soffz2'] = soffz2
	g_graphix_blizzard_particle[e]['sspdx1'] = sspdx1
	g_graphix_blizzard_particle[e]['sspdy1'] = sspdy1
	g_graphix_blizzard_particle[e]['sspdz1'] = sspdz1
	g_graphix_blizzard_particle[e]['sspdx2'] = sspdx2
	g_graphix_blizzard_particle[e]['sspdy2'] = sspdy2
	g_graphix_blizzard_particle[e]['sspdz2'] = sspdz2
	g_graphix_blizzard_particle[e]['srotatex'] = srotatex
	g_graphix_blizzard_particle[e]['srotatez'] = srotatez
	g_graphix_blizzard_particle[e]['sscalestartmin'] = sscalestartmin
	g_graphix_blizzard_particle[e]['sscalestartmax'] = sscalestartmax
	g_graphix_blizzard_particle[e]['sscaleendmin'] = sscaleendmin
	g_graphix_blizzard_particle[e]['sscaleendmax'] = sscaleendmax
	g_graphix_blizzard_particle[e]['salphastartmin'] = salphastartmin
	g_graphix_blizzard_particle[e]['salphastartmax'] = salphastartmax
	g_graphix_blizzard_particle[e]['salphaendmin'] = salphaendmin
	g_graphix_blizzard_particle[e]['salphaendmax'] = salphaendmax
	
	g_graphix_blizzard_particle[e]['manimspd'] = manimspd
	g_graphix_blizzard_particle[e]['mstartrandangle'] = mstartrandangle
	g_graphix_blizzard_particle[e]['mfrequency'] = mfrequency
	g_graphix_blizzard_particle[e]['mminlife'] = mminlife
	g_graphix_blizzard_particle[e]['mmaxlife'] = mmaxlife
	g_graphix_blizzard_particle[e]['moffx1'] = moffx1
	g_graphix_blizzard_particle[e]['moffy1'] = moffy1
	g_graphix_blizzard_particle[e]['moffz1'] = moffz1
	g_graphix_blizzard_particle[e]['moffx2'] = moffx2
	g_graphix_blizzard_particle[e]['moffy2'] = moffy2
	g_graphix_blizzard_particle[e]['moffz2'] = moffz2
	g_graphix_blizzard_particle[e]['mspdx1'] = mspdx1
	g_graphix_blizzard_particle[e]['mspdy1'] = mspdy1
	g_graphix_blizzard_particle[e]['mspdz1'] = mspdz1
	g_graphix_blizzard_particle[e]['mspdx2'] = mspdx2
	g_graphix_blizzard_particle[e]['mspdy2'] = mspdy2
	g_graphix_blizzard_particle[e]['mspdz2'] = mspdz2
	g_graphix_blizzard_particle[e]['mrotatex'] = mrotatex
	g_graphix_blizzard_particle[e]['mrotatez'] = mrotatez
	g_graphix_blizzard_particle[e]['mscalestartmin'] = mscalestartmin
	g_graphix_blizzard_particle[e]['mscalestartmax'] = mscalestartmax
	g_graphix_blizzard_particle[e]['mscaleendmin'] = mscaleendmin
	g_graphix_blizzard_particle[e]['mscaleendmax'] = mscaleendmax
	g_graphix_blizzard_particle[e]['malphastartmin'] = malphastartmin
	g_graphix_blizzard_particle[e]['malphastartmax'] = malphastartmax
	g_graphix_blizzard_particle[e]['malphaendmin'] = malphaendmin
	g_graphix_blizzard_particle[e]['malphaendmax'] = malphaendmax
	
	if g_graphix_blizzard_particle[e]['followplayer'] == 1 then 
		g_graphix_blizzard_particle[e]['followplayer'] = -1
	else 
		g_graphix_blizzard_particle[e]['followplayer'] = e 
	end 	
	
end
	

local emitterList = {}

-- caller must load image
local function PE_CreateNamedEmitter(e, name, image, frames, entity )
	local epos = {}
	local escl = {}
	local espd = {}
	local erot = {}
	local elife = {}
	local ealpha = {}
	local easpd = 0
	local esrot = 0
	local efreq = 0
	local efollowp = g_graphix_blizzard_particle[e]['followplayer']
	if name == 'graphixsnow' then 
		epos[1] = g_graphix_blizzard_particle[e]['soffx1']
		epos[2] = g_graphix_blizzard_particle[e]['soffy1']
		epos[3] = g_graphix_blizzard_particle[e]['soffz1']
		epos[4] = g_graphix_blizzard_particle[e]['soffx2']
		epos[5] = g_graphix_blizzard_particle[e]['soffy2']
		epos[6] = g_graphix_blizzard_particle[e]['soffz2']
		escl[1] = g_graphix_blizzard_particle[e]['sscalestartmin']
		escl[2] = g_graphix_blizzard_particle[e]['sscalestartmax']
		escl[3] = g_graphix_blizzard_particle[e]['sscaleendmin']
		escl[4] = g_graphix_blizzard_particle[e]['sscaleendmin']
		espd[1] = g_graphix_blizzard_particle[e]['sspdx1']
		espd[2] = g_graphix_blizzard_particle[e]['sspdy1']
		espd[3] = g_graphix_blizzard_particle[e]['sspdz1']
		espd[4] = g_graphix_blizzard_particle[e]['sspdx2']
		espd[5] = g_graphix_blizzard_particle[e]['sspdy2']
		espd[6] = g_graphix_blizzard_particle[e]['sspdz2']
		erot[1] = g_graphix_blizzard_particle[e]['srotatex']
		erot[2] = g_graphix_blizzard_particle[e]['srotatez']
		elife[1] = g_graphix_blizzard_particle[e]['sminlife']
		elife[2] = g_graphix_blizzard_particle[e]['smaxlife']
		ealpha[1] = g_graphix_blizzard_particle[e]['salphastartmin']
		ealpha[2] = g_graphix_blizzard_particle[e]['salphastartmax']
		ealpha[3] = g_graphix_blizzard_particle[e]['salphaendmin']
		ealpha[4] = g_graphix_blizzard_particle[e]['salphaendmax']
		easpd = g_graphix_blizzard_particle[e]['sanimspd']
		esrot = g_graphix_blizzard_particle[e]['sstartrandangle']
		efreq = g_graphix_blizzard_particle[e]['sfrequency']		
	elseif name == 'graphixmist' then 
		epos[1] = g_graphix_blizzard_particle[e]['moffx1']
		epos[2] = g_graphix_blizzard_particle[e]['moffy1']
		epos[3] = g_graphix_blizzard_particle[e]['moffz1']
		epos[4] = g_graphix_blizzard_particle[e]['moffx2']
		epos[5] = g_graphix_blizzard_particle[e]['moffy2']
		epos[6] = g_graphix_blizzard_particle[e]['moffz2']
		escl[1] = g_graphix_blizzard_particle[e]['mscalestartmin']
		escl[2] = g_graphix_blizzard_particle[e]['mscalestartmax']
		escl[3] = g_graphix_blizzard_particle[e]['mscaleendmin']
		escl[4] = g_graphix_blizzard_particle[e]['mscaleendmin']
		espd[1] = g_graphix_blizzard_particle[e]['mspdx1']
		espd[2] = g_graphix_blizzard_particle[e]['mspdy1']
		espd[3] = g_graphix_blizzard_particle[e]['mspdz1']
		espd[4] = g_graphix_blizzard_particle[e]['mspdx2']
		espd[5] = g_graphix_blizzard_particle[e]['mspdy2']
		espd[6] = g_graphix_blizzard_particle[e]['mspdz2']
		erot[1] = g_graphix_blizzard_particle[e]['mrotatex']
		erot[2] = g_graphix_blizzard_particle[e]['mrotatez']
		elife[1] = g_graphix_blizzard_particle[e]['mminlife']
		elife[2] = g_graphix_blizzard_particle[e]['mmaxlife']
		ealpha[1] = g_graphix_blizzard_particle[e]['malphastartmin']
		ealpha[2] = g_graphix_blizzard_particle[e]['malphastartmax']
		ealpha[3] = g_graphix_blizzard_particle[e]['malphaendmin']
		ealpha[4] = g_graphix_blizzard_particle[e]['malphaendmax']
		easpd = g_graphix_blizzard_particle[e]['manimspd']
		esrot = g_graphix_blizzard_particle[e]['mstartrandangle']
		efreq = g_graphix_blizzard_particle[e]['mfrequency']
	end 	
	local emitter = ParticlesGetFreeEmitter()
	if emitter == -1 then return end
	ParticlesAddEmitterEx( emitter, easpd, esrot, 
											 epos[1], epos[2], epos[3], epos[4], epos[5], epos[6],
						 escl[1], escl[2], escl[3], escl[4],
						 espd[1], espd[2], espd[3], espd[4], espd[5], espd[6],
						 erot[1], erot[2], elife[1], elife[2],
						 ealpha[1], ealpha[2], ealpha[3], ealpha[4],
						 efreq, efollowp, 0, image, frames )
	return emitter
	
end
								
function graphix_blizzard_particle_init(e)
	g_graphix_blizzard_particle[e] = {}
	g_graphix_blizzard_particle[e]['followplayer'] = 1
	
	g_graphix_blizzard_particle[e]['sanimspd'] = 1
	g_graphix_blizzard_particle[e]['sstartrandangle'] = 0
	g_graphix_blizzard_particle[e]['sfrequency'] = 16
	g_graphix_blizzard_particle[e]['sminlife'] = 2000
	g_graphix_blizzard_particle[e]['smaxlife'] = 2000
	g_graphix_blizzard_particle[e]['soffx1'] = -500
	g_graphix_blizzard_particle[e]['soffy1'] = 80
	g_graphix_blizzard_particle[e]['soffz1'] = -500
	g_graphix_blizzard_particle[e]['soffx2'] = 500
	g_graphix_blizzard_particle[e]['soffy2'] = 100
	g_graphix_blizzard_particle[e]['soffz2'] = 500
	g_graphix_blizzard_particle[e]['sspdx1'] = 0
	g_graphix_blizzard_particle[e]['sspdy1'] = -0.8
	g_graphix_blizzard_particle[e]['sspdz1'] = 0
	g_graphix_blizzard_particle[e]['sspdx2'] = 0
	g_graphix_blizzard_particle[e]['sspdy2'] = -1.2
	g_graphix_blizzard_particle[e]['sspdz2'] = 0
	g_graphix_blizzard_particle[e]['srotatex'] = 0
	g_graphix_blizzard_particle[e]['srotatez'] = 0
	g_graphix_blizzard_particle[e]['sscalestartmin'] = 200
	g_graphix_blizzard_particle[e]['sscalestartmax'] = 300
	g_graphix_blizzard_particle[e]['sscaleendmin'] = 20
	g_graphix_blizzard_particle[e]['sscaleendmax'] = 50
	g_graphix_blizzard_particle[e]['salphastartmin'] = 90
	g_graphix_blizzard_particle[e]['salphastartmax'] = 190
	g_graphix_blizzard_particle[e]['salphaendmin'] = 80
	g_graphix_blizzard_particle[e]['salphaendmax'] = 100
	
	g_graphix_blizzard_particle[e]['manimspd'] = 0.06
	g_graphix_blizzard_particle[e]['mstartrandangle'] = 0
	g_graphix_blizzard_particle[e]['mfrequency'] = 16
	g_graphix_blizzard_particle[e]['mminlife'] = 2000
	g_graphix_blizzard_particle[e]['mmaxlife'] = 2000
	g_graphix_blizzard_particle[e]['moffx1'] = -700
	g_graphix_blizzard_particle[e]['moffy1'] = 100
	g_graphix_blizzard_particle[e]['moffz1'] = -700
	g_graphix_blizzard_particle[e]['moffx2'] = 700
	g_graphix_blizzard_particle[e]['moffy2'] = 100
	g_graphix_blizzard_particle[e]['moffz2'] = 700
	g_graphix_blizzard_particle[e]['mspdx1'] = 0
	g_graphix_blizzard_particle[e]['mspdy1'] = -0.8
	g_graphix_blizzard_particle[e]['mspdz1'] = 0
	g_graphix_blizzard_particle[e]['mspdx2'] = 0
	g_graphix_blizzard_particle[e]['mspdy2'] = -1.2
	g_graphix_blizzard_particle[e]['mspdz2'] = 0
	g_graphix_blizzard_particle[e]['mrotatex'] = 0
	g_graphix_blizzard_particle[e]['mrotatez'] = 0
	g_graphix_blizzard_particle[e]['mscalestartmin'] = 400
	g_graphix_blizzard_particle[e]['mscalestartmax'] = 500
	g_graphix_blizzard_particle[e]['mscaleendmin'] = 200
	g_graphix_blizzard_particle[e]['mscaleendmax'] = 300
	g_graphix_blizzard_particle[e]['malphastartmin'] = 6
	g_graphix_blizzard_particle[e]['malphastartmax'] = 12
	g_graphix_blizzard_particle[e]['malphaendmin'] = 6
	g_graphix_blizzard_particle[e]['malphaendmax'] = 12
end

local mistImg  = mistImg  or ParticlesLoadImage( "effectbank\\particles\\weather\\graphixmist.dds", 8 )
local snowImg = snowImg or ParticlesLoadImage( "effectbank\\particles\\weather\\graphixsnow.dds", 9 )

function graphix_blizzard_particle_main(e)
	local nfollowplayer = g_graphix_blizzard_particle[e]['followplayer']
	local emitter = emitterList[e]

	if emitter == nil then
		emitterList[e] = { used = false }
		
	else	
		
		if not emitter.used then 

						
			local foggy = PE_CreateNamedEmitter(e, 'graphixmist',  mistImg, 64, nfollowplayer)		
			local flurry = PE_CreateNamedEmitter(e, 'graphixsnow', snowImg, 64, nfollowplayer)
			
			emitter.used = true
					
			ParticlesSetWindVector( 0.02, 0.03 )
		end
	end
end
