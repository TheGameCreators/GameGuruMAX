-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Atlas_Changer v1
-- DESCRIPTION: Loops 3 atlas images onto a object. Attach to the object.
-- DESCRIPTION: Can be activated from a trigger or switch or set to always on.
-- DESCRIPTION: [IMAGEFILE1$="imagebank\\misc\\testimages\\red.png"]
-- DESCRIPTION: [DISPLAY_TIME1=5(0,100)]
-- DESCRIPTION: [IMAGEFILE2$="imagebank\\misc\\testimages\\orange.png"]
-- DESCRIPTION: [DISPLAY_TIME2=5(0,100)]
-- DESCRIPTION: [IMAGEFILE3$="imagebank\\misc\\testimages\\green.png"]
-- DESCRIPTION: [DISPLAY_TIME3=5(0,100)]
-- DESCRIPTION: [ATLAS_ROWS=2]
-- DESCRIPTION: [ATLAS_COLUMNS=2]
-- DESCRIPTION: [#EMISSIVE_STRENGTH=300(0,1000)]
-- DESCRIPTION: [LOOP_DELAY=5(0,100)]
-- DESCRIPTION: [@ALWAYS_ON=1(1=Yes, 2=No)]
-- DESCRIPTION: [DIAGNOSTICS!=0]

local atlasobject 	= {}
local imagefile1 	= {}
local display_time1	= {}
local imagefile2 	= {}
local display_time2	= {}
local imagefile3 	= {}
local display_time3	= {}
local atlas_rows	= {}
local atlas_columns	= {}
local loop_delay	= {}
local always_on		= {}
local diagnostics	= {}

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

function atlas_changer_properties(e, imagefile1, display_time1, imagefile2, display_time2, imagefile3, display_time3, atlas_rows, atlas_columns, emissive_strength, loop_delay, always_on, diagnostics)
	atlasobject[e] = g_Entity[e]
	atlasobject[e].imagefile1 = imagefile1
	atlasobject[e].display_time1 = display_time1
	atlasobject[e].imagefile2 = imagefile2
	atlasobject[e].display_time2 = display_time2
	atlasobject[e].imagefile3 = imagefile3
	atlasobject[e].display_time3 = display_time3
	atlasobject[e].atlas_rows = atlas_rows
	atlasobject[e].atlas_columns = atlas_columns
	atlasobject[e].emissive_strength = emissive_strength
	atlasobject[e].loop_delay = loop_delay
	atlasobject[e].always_on = always_on
	atlasobject[e].diagnostics = diagnostics
	atlasobject[e].imagefile1id = LoadImage(imagefile1)
	atlasobject[e].imagefile2id = LoadImage(imagefile2)
	atlasobject[e].imagefile3id = LoadImage(imagefile3)
	if string.len(imagefile1)>0 then 
		SetEntityTexture(e,atlasobject[e].imagefile1id)
		SetEntityEmissiveStrength(e,atlasobject[e].emissive_strength)
	end
	SetEntityTextureScale(e,1,1)
	SetEntityTextureOffset(e,0,0)	
end

function atlas_changer_init(e)
	atlasobject[e] = g_Entity[e]
	atlasobject[e].imagefile1 = ""
	atlasobject[e].display_time1 = 5
	atlasobject[e].imagefile2 = ""
	atlasobject[e].display_time2 = 5
	atlasobject[e].imagefile3 = ""
	atlasobject[e].display_time3 = 5
	atlasobject[e].atlas_rows = 0
	atlasobject[e].atlas_columns = 0
	atlasobject[e].emissive_strength = 300
	atlasobject[e].loop_delay = 5
	atlasobject[e].always_on = 1
	atlasobject[e].diagnostics = 0
	atlasobject[e].imagefile1id = LoadImage(imagefile1)
	atlasobject[e].imagefile2id = LoadImage(imagefile2)
	atlasobject[e].imagefile3id = LoadImage(imagefile3)
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
	StartTimer(e)
end

function atlas_changer_main(e)
	atlasobject[e] = g_Entity[e]
	if status[e] == "init" then
		if atlasobject[e].always_on == 1 then SetActivated(e,1) end
		if atlasobject[e].always_on == 2 then SetActivated(e,0) end
		SetEntityTextureScale(e,1,1)
		SetEntityTextureOffset(e,0,0)
		status[e] = "process1"
	end
		
	if g_Entity[e]['activated'] == 1 then	
				
		if status[e] == "process1" then
			if doonce[e] == 0 then
				image_time[e] = GetTimer(e) + (atlasobject[e].display_time1 * 1000)				
				doonce[e] = 2
			end
			if string.len(atlasobject[e].imagefile1)>0 then 							
				SetEntityTexture(e,atlasobject[e].imagefile1id)
				SetEntityEmissiveStrength(e,atlasobject[e].emissive_strength)				
			end
			if GetTimer(e) > frame_time[e] then
				frame_time[e] = GetTimer(e) + 1000
				if atlasobject[e].atlas_rows > 0 and atlasobject[e].atlas_columns > 0 then
					change_frame[e] = change_frame[e] + 1
					totalframes[e] = atlasobject[e].atlas_rows * atlasobject[e].atlas_columns
					if change_frame[e] >= totalframes[e] then change_frame[e] = 0 end
					columnindex[e] = math.floor(change_frame[e] / atlasobject[e].atlas_rows)
					rowindex[e] = change_frame[e] - (columnindex[e]*atlasobject[e].atlas_rows)
					rowsize[e] = 1.0 / atlasobject[e].atlas_rows
					columnsize[e] = 1.0 / atlasobject[e].atlas_columns
					SetEntityTextureScale(e,rowsize[e],columnsize[e])
					SetEntityTextureOffset(e,math.fmod(rowindex[e]*rowsize[e],1),math.fmod(columnindex[e]*columnsize[e],1))	
				end
			end
			if g_Time > image_time[e] then
				status[e] = "process2"
			end
		end
		if status[e] == "process2" then
			if doonce[e] == 2 then
				image_time[e] = GetTimer(e) + (atlasobject[e].display_time2 * 1000)							
				doonce[e] = 3
			end
			if string.len(atlasobject[e].imagefile2)>0 then 	
				SetEntityTexture(e,atlasobject[e].imagefile2id)
				SetEntityEmissiveStrength(e,atlasobject[e].emissive_strength)				
			end
			if GetTimer(e) > frame_time[e] then
				frame_time[e] = GetTimer(e) + 1000
				if atlasobject[e].atlas_rows > 0 and atlasobject[e].atlas_columns > 0 then
					change_frame[e] = change_frame[e] + 1
					totalframes[e] = atlasobject[e].atlas_rows * atlasobject[e].atlas_columns
					if change_frame[e] >= totalframes[e] then change_frame[e] = 0 end
					columnindex[e] = math.floor(change_frame[e] / atlasobject[e].atlas_rows)
					rowindex[e] = change_frame[e] - (columnindex[e]*atlasobject[e].atlas_rows)
					rowsize[e] = 1.0 / atlasobject[e].atlas_rows
					columnsize[e] = 1.0 / atlasobject[e].atlas_columns
					SetEntityTextureScale(e,rowsize[e],columnsize[e])
					SetEntityTextureOffset(e,math.fmod(rowindex[e]*rowsize[e],1),math.fmod(columnindex[e]*columnsize[e],1))	
				end
			end
			if g_Time > image_time[e] then
				status[e] = "process3"				
			end			
		end
		if status[e] == "process3" then
			if doonce[e] == 3 then
				image_time[e] = GetTimer(e) + (atlasobject[e].display_time3 * 1000)				
				doonce[e] = 0
			end
			if string.len(atlasobject[e].imagefile3)>0 then		
				SetEntityTexture(e,atlasobject[e].imagefile3id)
				SetEntityEmissiveStrength(e,atlasobject[e].emissive_strength)				
			end
			if GetTimer(e) > frame_time[e] then
				frame_time[e] = GetTimer(e) + 1000
				if atlasobject[e].atlas_rows > 0 and atlasobject[e].atlas_columns > 0 then
					change_frame[e] = change_frame[e] + 1
					totalframes[e] = atlasobject[e].atlas_rows * atlasobject[e].atlas_columns
					if change_frame[e] >= totalframes[e] then change_frame[e] = 0 end
					columnindex[e] = math.floor(change_frame[e] / atlasobject[e].atlas_rows)
					rowindex[e] = change_frame[e] - (columnindex[e]*atlasobject[e].atlas_rows)
					rowsize[e] = 1.0 / atlasobject[e].atlas_rows
					columnsize[e] = 1.0 / atlasobject[e].atlas_columns
					SetEntityTextureScale(e,rowsize[e],columnsize[e])
					SetEntityTextureOffset(e,math.fmod(rowindex[e]*rowsize[e],1),math.fmod(columnindex[e]*columnsize[e],1))	
				end
			end
			if g_Time > image_time[e] then				
				status[e] = "processloop"
				image_time[e] = GetTimer(e) + (atlasobject[e].loop_delay * 1000)
			end			
		end
		
		if status[e] == "processloop" then			
			if g_Time > image_time[e] then status[e] = "process1" end			
			PerformLogicConnections(e)			
		end
		
		--Diagnostic text -----------------------------------
		if atlasobject[e].diagnostics == 1 then
			Text(50,50,3,"Main G-Timer: " ..GetTimer(e)/1000)
			Text(50,52,3,"Change Image: " ..image_time[e]/1000)
			Text(50,55,3,"Status: " ..status[e])
		end
	end	
end

function atlas_changer_exit(e)
end
