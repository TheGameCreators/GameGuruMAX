-- DESCRIPTION: This behavior controls an attacking Zombie with <Zombie Animations> using <Melee Weapon>. It will engage in [range=300], and will stop pursuit beyond the [maximumrange=800]. The zombie starting speed can be selected randomly between [minspeed=60] and [maxspeed=65]. Sound effects are eating in <Sound0>, alerted in <Sound1>, attacking in <Sound2> and dying in <Sound3>. You can start the zombie off in a [!Feeding=0] state, and different [@ChaseModes=1(1=Arms Out, 2=Gammy Hand, 3=Bad Back, 4=Dash, 5=Hunchback)].
-- DESCRIPTION: Also set the [RecoveryTime=3000] when lunging and [!CanHearSound=1] within [HearingRange=750] and may [!GetWinded=0] and [!AllowHeadshot=1].
-- DESCRIPTION: [!AllowBrainFeed=1].

master_interpreter_core = require "scriptbank\\masterinterpreter"

g_zombie_attack = {}
g_zombie_attack_behavior = {}
g_zombie_attack_behavior_count = 0

function zombie_attack_init(e)
 g_zombie_attack[e] = {}
 g_zombie_attack[e]['bycfilename'] = "scriptbank\\people\\zombie_attack.byc"
 g_zombie_attack_behavior_count = master_interpreter_core.masterinterpreter_load (g_zombie_attack[e], g_zombie_attack_behavior )
 zombie_attack_properties(e,300,900, 60, 65, 0, 1, 3000, 1, 750, 0, 1)
end

function zombie_attack_properties(e, range, maximumrange, minspeed, maxspeed, feeding, chasemodes, recoverytime, canhearsound, hearingrange, getwinded, allowheadshot, allowbrainfeed)
 g_zombie_attack[e]['range'] = range
 g_zombie_attack[e]['maximumrange'] = maximumrange
 g_zombie_attack[e]['feeding'] = feeding
 g_zombie_attack[e]['chasemodes'] = chasemodes
 g_zombie_attack[e]['recoverytime'] = recoverytime
 g_zombie_attack[e]['canhearsound'] = canhearsound
 g_zombie_attack[e]['hearingrange'] = hearingrange
 g_zombie_attack[e]['getwinded'] = getwinded 
 g_zombie_attack[e]['allowheadshot'] = allowheadshot 
 g_zombie_attack[e]['allowbrainfeed'] = allowbrainfeed 
 SetEntityMoveSpeed(e,minspeed + math.random(0,(maxspeed-minspeed)))
 master_interpreter_core.masterinterpreter_restart (g_zombie_attack[e], g_Entity[e])
end

function zombie_attack_main(e)
 if g_zombie_attack[e] ~= nil and g_zombie_attack_behavior_count > 0 then
  local returnvalue = master_interpreter_core.masterinterpreter (g_zombie_attack_behavior, g_zombie_attack_behavior_count, e, g_zombie_attack[e], g_Entity[e])
  if returnvalue >= 0 then
   g_zombie_attack_behavior_count = returnvalue
  else
   local rapiddamageprocessingcount = math.abs(returnvalue)
   for rapiddamageprocessing = 1, rapiddamageprocessingcount do
    local returnvalue = master_interpreter_core.masterinterpreter (g_zombie_attack_behavior, g_zombie_attack_behavior_count, e, g_zombie_attack[e], g_Entity[e])
    if returnvalue >= 0 then
     g_zombie_attack_behavior_count = returnvalue
	end
   end
  end
 end
end
