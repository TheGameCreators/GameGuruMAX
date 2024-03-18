-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- DecalFacing v5
-- DESCRIPTION: Will show a decal facing the player
-- DESCRIPTION: Attach to the Decal object(s)

function decal_facing_properties(e)
end

function decal_facing_init(e)
end

function decal_facing_main(e)
	GravityOff(e)
	RotateToCamera(e)
end

