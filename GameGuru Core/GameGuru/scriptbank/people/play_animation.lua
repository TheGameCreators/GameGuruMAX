-- DESCRIPTION: When the player is within [RANGE=400], it will play an [@ActionPlay=2(0=AnimSetList)] stored in the object, then loop [@ActionLoop=2(0=AnimSetList)]. When further, it will play [@IdlePlay=1(0=AnimSetList)], then loop [@IdleLoop=2(0=AnimSetList)] animation using <Default Animations>. It will also play <Sound0> if triggered, and can optionally hold a <Melee Weapon>.

master_interpreter_core = require "scriptbank\\masterinterpreter"

g_play_animation = {}
g_play_animation_behavior = {}
g_play_animation_behavior_count = 0

function play_animation_init(e)
 g_play_animation[e] = {}
 g_play_animation[e]["bycfilename"] = "scriptbank\\people\\play_animation.byc"
 g_play_animation_behavior_count = master_interpreter_core.masterinterpreter_load (g_play_animation[e], g_play_animation_behavior )
 play_animation_properties(e,400,2,2,1,1)
end

function play_animation_properties(e, range, actionplay, actionloop, idleplay, idleloop)
 g_play_animation[e]['range'] = range
 g_play_animation[e]['actionplay'] = actionplay
 g_play_animation[e]['actionloop'] = actionloop
 g_play_animation[e]['idleplay'] = idleplay
 g_play_animation[e]['idleloop'] = idleloop
 master_interpreter_core.masterinterpreter_restart (g_play_animation[e], g_Entity[e])
end

function play_animation_main(e)
 if g_play_animation[e] ~= nil and g_play_animation_behavior_count > 0 then
  g_play_animation_behavior_count = master_interpreter_core.masterinterpreter (g_play_animation_behavior, g_play_animation_behavior_count, e, g_play_animation[e], g_Entity[e])
 end
end

