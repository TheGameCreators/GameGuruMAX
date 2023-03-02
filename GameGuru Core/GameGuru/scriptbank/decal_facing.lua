-- DESCRIPTION: Makes the decal always face the camera in game.

function decal_facing_init(e)
end

function decal_facing_main(e)
 RotateToCamera(e)
 if g_Entity[e]['activated'] == 1 then
  Destroy(e)
 end
end
