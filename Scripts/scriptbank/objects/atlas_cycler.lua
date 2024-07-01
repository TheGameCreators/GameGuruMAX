-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Atlas_Cycler v3
-- DESCRIPTION: Cycles an atlas images onto a object. Attach to the object.
-- DESCRIPTION: Can be activated from a trigger or switch or set to always on.
-- DESCRIPTION: [IMAGEFILE$="imagebank\\misc\\testimages\\red.png"]
-- DESCRIPTION: [DISPLAY_SPEED=50(0,100)]
-- DESCRIPTION: [ATLAS_COLUMNS=3]
-- DESCRIPTION: [ATLAS_ROWS=3]
-- DESCRIPTION: [#EMISSIVE_STRENGTH=300(0,1000)]
-- DESCRIPTION: [@DISPLAY_MODE=1(1=Cycle Play, 2=Single Play)]
-- DESCRIPTION: [@ALWAYS_ON=1(1=Yes, 2=No)]


local atlasobject 		= {}
local imagefile 		= {}
local display_speed		= {}
local atlas_columns		= {}
local atlas_rows		= {}
local emissive_strength	= {}
local display_mode		= {}
local always_on			= {}

local image_time 	= {}
local frame_time	= {}
local change_frame 	= {}
local totalframes 	= {}
local rowindex 		= {}
local rowsize 		= {}
local columnindex 	= {}
local columnsize 	= {}
local doonce 		= {}
local status		= {}

function atlas_cycler_properties(e, imagefile, display_speed, atlas_columns, atlas_rows, emissive_strength, display_mode, always_on)
	atlasobject[e] = g_Entity[e]
	atlasobject[e].imagefile = imagefile
	atlasobject[e].display_speed = display_speed
	atlasobject[e].atlas_columns = atlas_columns
	atlasobject[e].atlas_rows = atlas_rows
	atlasobject[e].emissive_strength = emissive_strength
	atlasobject[e].display_mode = display_mode
	atlasobject[e].always_on = always_on
	atlasobject[e].imagefileid = LoadImage(imagefile)
	if string.len(imagefile)>0 then 
		SetEntityTexture(e,atlasobject[e].imagefileid)
		SetEntityEmissiveStrength(e,atlasobject[e].emissive_strength)
	end
	SetEntityTextureScale(e,1,1)
	SetEntityTextureOffset(e,0,0)	
end

function atlas_cycler_init(e)
	atlasobject[e] = g_Entity[e]
	atlasobject[e].imagefile = ""
	atlasobject[e].display_speed = 5
	atlasobject[e].atlas_columns = 0
	atlasobject[e].atlas_rows = 0
	atlasobject[e].emissive_strength = 300
	atlasobject[e].display_mode = 1
	atlasobject[e].always_on = 1
	atlasobject[e].imagefileid = LoadImage(imagefile)
	status[e] = "init"
	image_time[e] = 0
	frame_time[e] = GetTimer(e)
	change_frame[e] = 0
	totalframes[e] = 0
	rowindex[e] = 0
	rowsize[e] = 0
	columnindex[e] = 0
	columnsize[e] = 0
	doonce[e] = 0
end

function atlas_cycler_main(e)
	atlasobject[e] = g_Entity[e]
	if status[e] == "init" then
		if atlasobject[e].always_on == 1 then SetActivated(e,1) end
		if atlasobject[e].always_on == 2 then SetActivated(e,0) end
		SetEntityTextureScale(e,1,1)
		SetEntityTextureOffset(e,0,0)
		status[e] = "process"
	end
		
	if g_Entity[e]['activated'] == 1 then	
				
		if status[e] == "process" then
			if doonce[e] == 0 then
				frame_time[e] = g_Time + (100-atlasobject[e].display_speed)
				doonce[e] = 2
			end
			if string.len(atlasobject[e].imagefile)>0 then 							
				SetEntityTexture(e,atlasobject[e].imagefileid)
				SetEntityEmissiveStrength(e,atlasobject[e].emissive_strength)				
			end
			if g_Time > frame_time[e] then
				frame_time[e] = g_Time + (100-atlasobject[e].display_speed)
				if atlasobject[e].atlas_columns > 0 and atlasobject[e].atlas_rows > 0 then
					change_frame[e] = change_frame[e] + 1
					totalframes[e] = atlasobject[e].atlas_columns * atlasobject[e].atlas_rows
					if change_frame[e] >= totalframes[e] and atlasobject[e].display_mode == 1 then change_frame[e] = 0 end
					if change_frame[e] >= totalframes[e] and atlasobject[e].display_mode == 2 then
						change_frame[e] = change_frame[e] - 1
						PerformLogicConnections(e)
						status[e] = "end"
					end
					columnindex[e] = math.floor(change_frame[e] / atlasobject[e].atlas_columns)
					rowindex[e] = change_frame[e] - (columnindex[e]*atlasobject[e].atlas_columns)
					rowsize[e] = 1.0 / atlasobject[e].atlas_columns
					columnsize[e] = 1.0 / atlasobject[e].atlas_rows
					SetEntityTextureScale(e,rowsize[e],columnsize[e])
					SetEntityTextureOffset(e,math.fmod(rowindex[e]*rowsize[e],1),math.fmod(columnindex[e]*columnsize[e],1))						
				end				
			end
		end	
	end	
end

function atlas_cycler_exit(e)
end
