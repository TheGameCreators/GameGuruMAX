-- v2
-- DESCRIPTION: Changes the object texture when activated by another object.
-- DESCRIPTION: Default texture is [IMAGEFILE1$=""] and the alternative texture is [IMAGEFILE2$=""].
-- DESCRIPTION: Control texture UV scale using [#TEXTURE_SCALE_U=1.0] and [#TEXTURE_SCALE_V=1.0].
-- DESCRIPTION: Control texture UV offset using [#TEXTURE_OFFSET_U=0.0] and [#TEXTURE_OFFSET_V=0.0].
-- DESCRIPTION: Control texture UV direction using [#SCROLL_DIRECTION_U=0.0] and [#SCROLL_DIRECTION_V=0.0].
-- DESCRIPTION: Control texture UV speed using [#TEXTURE_SPEED=0.0].
-- DESCRIPTION: Control texture atlas animation using [atlas_texture_rows=0] and [atlas_texture_columns=0].


g_change_texture = {}

function change_texture_properties(e,imagefile1,imagefile2,texture_scale_u,texture_scale_v,texture_offset_u,texture_offset_v,scroll_direction_u,scroll_direction_v,texture_speed,atlas_texture_rows,atlas_texture_columns)
	g_change_texture[e]['imagefile1'] = imagefile1
	g_change_texture[e]['imagefile2'] = imagefile2
	g_change_texture[e]['texture_scale_u'] = texture_scale_u
	g_change_texture[e]['texture_scale_v'] = texture_scale_v
	g_change_texture[e]['texture_offset_u'] = texture_offset_u
	g_change_texture[e]['texture_offset_v'] = texture_offset_v
	g_change_texture[e]['scroll_direction_u'] = scroll_direction_u
	g_change_texture[e]['scroll_direction_v'] = scroll_direction_v
	g_change_texture[e]['texture_speed'] = texture_speed
	g_change_texture[e]['atlas_texture_rows'] = atlas_texture_rows
	g_change_texture[e]['atlas_texture_columns'] = atlas_texture_columns
	g_change_texture[e]['imagefile1id'] = LoadImage(imagefile1)
	g_change_texture[e]['imagefile2id'] = LoadImage(imagefile2)
	if string.len(imagefile1)>0 then SetEntityTexture(e,g_change_texture[e]['imagefile1id']) end
	SetEntityTextureScale(e,texture_scale_u,texture_scale_v)
	SetEntityTextureOffset(e,texture_offset_u,texture_offset_v)
end

function change_texture_init_name(e,name)
	g_change_texture[e] = {}
	g_change_texture[e]['imagefile1'] = ""
	g_change_texture[e]['imagefile2'] = ""
	g_change_texture[e]['texture_scale_u'] = 1
	g_change_texture[e]['texture_scale_v'] = 1
	g_change_texture[e]['texture_offset_u'] = 0
	g_change_texture[e]['texture_offset_v'] = 0
	g_change_texture[e]['scroll_direction_u'] = 0
	g_change_texture[e]['scroll_direction_v'] = 0
	g_change_texture[e]['texture_speed'] = 0
	g_change_texture[e]['atlas_texture_rows'] = 0
	g_change_texture[e]['atlas_texture_columns'] = 0
	g_change_texture[e]['imagefile1id'] = 0
	g_change_texture[e]['imagefile2id'] = 0
	g_change_texture[e]['imagemode'] = 1
	g_change_texture[e]['textimer'] = GetTimer(e)
	g_change_texture[e]['texframe'] = 0
end

function change_texture_main(e)
 if g_Entity[e]['activated'] == 1 then
  if g_change_texture[e]['imagemode'] == 1 then
   if string.len(g_change_texture[e]['imagefile2'])>0 then SetEntityTexture(e,g_change_texture[e]['imagefile2id']) end
   g_change_texture[e]['imagemode'] = 2
  end
 else
  if g_change_texture[e]['imagemode'] == 2 then
   if string.len(g_change_texture[e]['imagefile1'])>0 then SetEntityTexture(e,g_change_texture[e]['imagefile1id']) end
   g_change_texture[e]['imagemode'] = 1
  end
 end
 if g_change_texture[e]['texture_speed'] > 0 then
  if GetTimer(e) > g_change_texture[e]['textimer'] then 
   g_change_texture[e]['textimer'] = GetTimer(e) + g_change_texture[e]['texture_speed']
   if g_change_texture[e]['atlas_texture_rows'] > 0 and g_change_texture[e]['atlas_texture_columns'] > 0 then
    -- frame animation
	g_change_texture[e]['texframe'] = g_change_texture[e]['texframe'] + 1
	local totalframes = g_change_texture[e]['atlas_texture_rows'] * g_change_texture[e]['atlas_texture_columns']
	if g_change_texture[e]['texframe'] >= totalframes then 
	 g_change_texture[e]['texframe'] = 0
	end
	local columnindex = math.floor(g_change_texture[e]['texframe'] / g_change_texture[e]['atlas_texture_rows'])
	local rowindex = g_change_texture[e]['texframe'] - (columnindex*g_change_texture[e]['atlas_texture_rows'])
	local rowsize = 1.0 / g_change_texture[e]['atlas_texture_rows']
	local columnsize = 1.0 / g_change_texture[e]['atlas_texture_columns']
    SetEntityTextureScale(e,rowsize,columnsize)
    g_change_texture[e]['texture_offset_u'] = math.fmod(rowindex*rowsize,1)	
    g_change_texture[e]['texture_offset_v'] = math.fmod(columnindex*columnsize,1)
   else
    -- simple scrolling
    SetEntityTextureScale(e,g_change_texture[e]['texture_scale_u'],g_change_texture[e]['texture_scale_v'])
    g_change_texture[e]['texture_offset_u'] = math.fmod(g_change_texture[e]['texture_offset_u'] + g_change_texture[e]['scroll_direction_u'],1)
    g_change_texture[e]['texture_offset_v'] = math.fmod(g_change_texture[e]['texture_offset_v'] + g_change_texture[e]['scroll_direction_v'],1)
   end
   SetEntityTextureOffset(e,g_change_texture[e]['texture_offset_u'],g_change_texture[e]['texture_offset_v'])
  end
 end
end