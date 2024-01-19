-- Change Texture v6
-- DESCRIPTION: Changes the object texture when activated by another object.
-- DESCRIPTION: Default texture is [IMAGEFILE1$=""] and the alternative texture is [IMAGEFILE2$=""]. 
-- DESCRIPTION: Control texture UV scale using [#TEXTURE_SCALE_U=1.0] and [#TEXTURE_SCALE_V=1.0].
-- DESCRIPTION: Control texture UV offset using [#TEXTURE_OFFSET_U=0.0] and [#TEXTURE_OFFSET_V=0.0].
-- DESCRIPTION: Control texture UV direction using [#SCROLL_DIRECTION_U=0.0] and [#SCROLL_DIRECTION_V=0.0].
-- DESCRIPTION: Control texture UV speed using [#TEXTURE_SPEED=0.0].
-- DESCRIPTION: Control texture atlas animation using [atlas_texture_rows=0] and [atlas_texture_columns=0].
-- DESCRIPTION: If PBR textures exist alongside the image files, they will be applied to the object as well.
-- DESCRIPTION: Control material emissive strength using [#MATERIAL1_EMISSIVE_STRENGTH=300.0] and [#MATERIAL2_EMISSIVE_STRENGTH=300.0].

local change_texture = {}
local imagefile1					= {}
local imagefile2 					= {}
local texture_scale_u 				= {}
local texture_scale_v 				= {}
local texture_offset_u 				= {}
local texture_offset_v 				= {}
local scroll_direction_u 			= {}
local scroll_direction_v 			= {}
local texture_speed 				= {}
local atlas_texture_rows 			= {}
local atlas_texture_columns 		= {}
local imagefile1id 					= {}
local imagefile2id 					= {}
local material1_emissive_strength 	= {}
local material2_emissive_strength 	= {}
local status						= {}

function change_texture_properties(e,imagefile1,imagefile2,texture_scale_u,texture_scale_v,texture_offset_u,texture_offset_v,scroll_direction_u,scroll_direction_v,texture_speed,atlas_texture_rows,atlas_texture_columns, material1_emissive_strength, material2_emissive_strength)
	change_texture[e] = g_Entity[e]
	change_texture[e].imagefile1 = imagefile1
	change_texture[e].imagefile2 = imagefile2
	change_texture[e].texture_scale_u = texture_scale_u
	change_texture[e].texture_scale_v = texture_scale_v
	change_texture[e].texture_offset_u = texture_offset_u
	change_texture[e].texture_offset_v = texture_offset_v
	change_texture[e].scroll_direction_u = scroll_direction_u
	change_texture[e].scroll_direction_v = scroll_direction_v
	change_texture[e].texture_speed = texture_speed
	change_texture[e].atlas_texture_rows = atlas_texture_rows
	change_texture[e].atlas_texture_columns = atlas_texture_columns
	change_texture[e].imagefile1id = LoadImage(imagefile1)
	change_texture[e].imagefile2id = LoadImage(imagefile2)
	change_texture[e].material1_emissive_strength = material1_emissive_strength
	change_texture[e].material2_emissive_strength = material2_emissive_strength
	if string.len(imagefile1)>0 then 
		SetEntityTexture(e,change_texture[e].imagefile1id) 
		SetEntityEmissiveStrength(e,change_texture[e].material1_emissive_strength)
	end
	if string.len(imagefile2)>0 then 
		SetEntityTexture(e,change_texture[e].imagefile2id) 
		SetEntityEmissiveStrength(e,change_texture[e].material2_emissive_strength)
	end
	SetEntityTextureScale(e,texture_scale_u,texture_scale_v)
	SetEntityTextureOffset(e,texture_offset_u,texture_offset_v)
end

function change_texture_init_name(e,name)
	change_texture[e] = g_Entity[e]
	change_texture[e].imagefile1 = ""
	change_texture[e].imagefile2 = ""
	change_texture[e].texture_scale_u = 1
	change_texture[e].texture_scale_v = 1
	change_texture[e].texture_offset_u = 0
	change_texture[e].texture_offset_v = 0
	change_texture[e].scroll_direction_u = 0
	change_texture[e].scroll_direction_v = 0
	change_texture[e].texture_speed = 0
	change_texture[e].atlas_texture_rows = 0
	change_texture[e].atlas_texture_columns = 0
	change_texture[e].imagefile1id = 0
	change_texture[e].imagefile2id = 0
	change_texture[e].imagemode = 2
	change_texture[e].textimer = GetTimer(e)
	change_texture[e].texframe = 0
	change_texture[e].material1_emissive_strength = 300
	change_texture[e].material2_emissive_strength = 300
	status[e] = "init"
end

function change_texture_main(e)
	change_texture[e] = g_Entity[e]
	if status[e] == "init" then
		status[e] = "endinit"
	end
 
	if g_Entity[e].activated == 1 then
		if change_texture[e].imagemode == 1 then
			if string.len(change_texture[e].imagefile2)>0 then 
				SetEntityTexture(e,change_texture[e].imagefile2id) 
				SetEntityEmissiveStrength(e,change_texture[e].material2_emissive_strength)
			end
			change_texture[e].imagemode = 2
		end
	else
		if change_texture[e].imagemode == 2 then
			if string.len(change_texture[e].imagefile1)>0 then 
				SetEntityTexture(e,change_texture[e].imagefile1id) 
				SetEntityEmissiveStrength(e,change_texture[e].material1_emissive_strength)
			end
			change_texture[e].imagemode = 1
		end
	end
	if change_texture[e].texture_speed > 0 then
		if GetTimer(e) > change_texture[e].textimer then 
			change_texture[e].textimer = GetTimer(e) + change_texture[e].texture_speed
			if change_texture[e].atlas_texture_rows > 0 and change_texture[e].atlas_texture_columns > 0 then
				-- frame animation
				change_texture[e].texframe = change_texture[e].texframe + 1
				local totalframes = change_texture[e].atlas_texture_rows * change_texture[e].atlas_texture_columns
				if change_texture[e].texframe >= totalframes then 
					change_texture[e].texframe = 0
				end
				local columnindex = math.floor(change_texture[e].texframe / change_texture[e].atlas_texture_rows)
				local rowindex = change_texture[e].texframe - (columnindex*change_texture[e].atlas_texture_rows)
				local rowsize = 1.0 / change_texture[e].atlas_texture_rows
				local columnsize = 1.0 / change_texture[e].atlas_texture_columns
				SetEntityTextureScale(e,rowsize,columnsize)
				change_texture[e].texture_offset_u = math.fmod(rowindex*rowsize,1)	
				change_texture[e].texture_offset_v = math.fmod(columnindex*columnsize,1)
			else
				-- simple scrolling
				SetEntityTextureScale(e,change_texture[e].texture_scale_u,change_texture[e].texture_scale_v)
				change_texture[e].texture_offset_u = math.fmod(change_texture[e].texture_offset_u + change_texture[e].scroll_direction_u,1)
				change_texture[e].texture_offset_v = math.fmod(change_texture[e].texture_offset_v + change_texture[e].scroll_direction_v,1)
			end
			SetEntityTextureOffset(e,change_texture[e].texture_offset_u,change_texture[e].texture_offset_v)
		end
	end
end