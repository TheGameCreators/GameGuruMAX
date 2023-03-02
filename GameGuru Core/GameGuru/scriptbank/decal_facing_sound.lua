-- DESCRIPTION: Makes the decal always face the camera in game and loops.

g_decal_facing_sound = {}

function decal_facing_sound_properties(e, speech)
	g_decal_facing_sound[e]['speech'] = speech
end 

function decal_facing_sound_init(e)
	g_decal_facing_sound[e] = {}
	g_decal_facing_sound[e]['speech'] = ""
	g_decal_facing_sound[e] = 0
end

function decal_facing_sound_main(e)
 RotateToCamera(e)
 if g_decal_facing_sound[e] == 0 then
  g_decal_facing_sound[e] = 1
  LoopSound(e,0)
 end
 if g_Entity[e]['activated'] == 1 then
  Destroy(e)
 end
end
