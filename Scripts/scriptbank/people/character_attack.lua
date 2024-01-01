-- DESCRIPTION: This will control a character using <Soldier Animations> to engage their enemy within view, can also [!FollowAPath=1] at the start,and is capable of attacking their target using a <Shooting Weapon> and [!CanRetreat=1] to the [RetreatRange=500(200,1000)] when hurt. Sound effects are alerted in <Sound0>, seen target in <Sound1>, lost target in <Sound2> and killed target in <Sound3>. Have this character [!StandGround=0] to defend their position, and when not using a [!RandomFlankMode=1], can optionally [@FlankTarget=1(1=Stay Back, 2=Get Close, 3=Wide Flank)], start [!Alerted=0], [AllowHeadshot!=1], [*CombatTime=15000] and [!CanHearSound=1] within [HearingRange=1250]. Optionally [!StarterAnimation=0] using [@StartAnimation=1(0=AnimSetList)].
master_interpreter_core = require "scriptbank\\masterinterpreter"

g_character_attack = {}
g_character_attack_behavior = {}
g_character_attack_behavior_count = 0

function character_attack_init(e)
 g_character_attack[e] = {}
 g_character_attack[e]["bycfilename"] = "scriptbank\\people\\character_attack.byc"
 g_character_attack_behavior_count = master_interpreter_core.masterinterpreter_load (g_character_attack[e], g_character_attack_behavior )
 character_attack_properties(e,1,1,500,0,1,1,0,1,15000,1,1250)
end

function character_attack_properties(e, followapath, canretreat, retreatrange, standground, randomflankmode, flanktarget, alerted, allowheadshot, combattime, canhearsound, hearingrange, starteranimation, startanimation)
 g_character_attack[e]['followapath'] = followapath
 g_character_attack[e]['canretreat'] = canretreat
 g_character_attack[e]['retreatrange'] = retreatrange
 g_character_attack[e]['standground'] = standground
 g_character_attack[e]['randomflankmode'] = randomflankmode
 g_character_attack[e]['flanktarget'] = flanktarget
 g_character_attack[e]['alerted'] = alerted
 g_character_attack[e]['allowheadshot'] = allowheadshot
 g_character_attack[e]['combattime'] = combattime
 g_character_attack[e]['canhearsound'] = canhearsound
 g_character_attack[e]['hearingrange'] = hearingrange 
 g_character_attack[e]['starteranimation'] = starteranimation
 g_character_attack[e]['startanimation'] = startanimation
 master_interpreter_core.masterinterpreter_restart (g_character_attack[e], g_Entity[e])
end

function character_attack_main(e)
 if g_character_attack[e] ~= nil and g_character_attack_behavior_count > 0 then
  local returnvalue = master_interpreter_core.masterinterpreter (g_character_attack_behavior, g_character_attack_behavior_count, e, g_character_attack[e], g_Entity[e])
  if returnvalue >= 0 then
   g_character_attack_behavior_count = returnvalue
  else
   local rapiddamageprocessingcount = math.abs(returnvalue)
   for rapiddamageprocessing = 1, rapiddamageprocessingcount do
    local returnvalue = master_interpreter_core.masterinterpreter (g_character_attack_behavior, g_character_attack_behavior_count, e, g_character_attack[e], g_Entity[e])
    if returnvalue >= 0 then
     g_character_attack_behavior_count = returnvalue
	end
   end
  end
 end
end
