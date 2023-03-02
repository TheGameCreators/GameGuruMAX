-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Message v7: by Necrym59 with special thanks to smallg
-- DESCRIPTION: Will display a Message on screen when triggered from a zone or switch.
-- DESCRIPTION: Attach to an object and link to a zone or switch.
-- DESCRIPTION: [MESSAGE_TEXT1$="MISSON DEBRIEF: Operation X5s"]
-- DESCRIPTION: [MESSAGE_TEXT2$="Cpt. Hughes - Infiltration Unit 6"]
-- DESCRIPTION: [MESSAGE_TEXT3$="Deploy and infiltrate Maersk Operations Complex"]
-- DESCRIPTION: [MESSAGE_TEXT4$="Seek and destroy valid targets and recover"]
-- DESCRIPTION: [MESSAGE_TEXT5$="classified documents from Dr. Gebchev"]
-- DESCRIPTION: [MESSAGE_X=10]
-- DESCRIPTION: [MESSAGE_Y=10]
-- DESCRIPTION: [@TEXT_COLOUR = 1(1=White, 2=Green, 3=Blue, 4=Yellow, 5=Orange, 6=Red)]
-- DESCRIPTION: [TEXT_SIZE=3(1,5)]
-- DESCRIPTION: [@TEXT_STYLE=1(1=Left, 2=Center)]
-- DESCRIPTION: [LINES_SHOWN=5(1,5)]
-- DESCRIPTION: [LETTER_DELAY#=0.05]
-- DESCRIPTION: [LINES_DELAY#=0.8]
-- DESCRIPTION: [DISPLAY_TIME=5(1,10)]
-- DESCRIPTION: [LOCK_PLAYER!=0]
-- DESCRIPTION: Play <Sound0> when message typed

g_message = {}
function message_properties(e, message_text1, message_text2, message_text3, message_text4, message_text5, message_x, message_y, text_colour, text_size, text_style, lines_shown, letter_delay, lines_delay, display_time, lock_player) 																		
	local im = g_message[e]
	im.message_text1 = message_text1
	im.message_text2 = message_text2
	im.message_text3 = message_text3
	im.message_text4 = message_text4
	im.message_text5 = message_text5
	im.message_x = message_x
	im.message_y = message_y
	im.text_colour = text_colour
	im.text_size = text_size
	im.text_style = text_style
	im.lines_shown = lines_shown	
	im.letter_delay = letter_delay
	im.lines_delay = lines_delay
	im.display_time = display_time
	im.lock_player = lock_player or 0
	im.state = "init"
	im.lines = {}
	table.insert(im.lines,im.message_text1)
	table.insert(im.lines,im.message_text2)
	table.insert(im.lines,im.message_text3)
	table.insert(im.lines,im.message_text4)
	table.insert(im.lines,im.message_text5)
	im.text = {}	
end 

function message_init(e)
	g_message[e] = {}		
end 

function message_main(e)
	local im = g_message[e]
	local checktime = {}
		
	if g_Entity[e]['activated'] == 1 then
		if im.state == "init" then 
			if im.lock_player == 1 then SetCameraOverride(3) end
			im.lettertimer = 0
			im.linetimer = 0
			im.currentline = 1
			im.currentletter = 1
			im.text[im.currentline] = string.sub(im.lines[im.currentline],im.currentletter,im.currentletter)		
			im.state = "message"
			return			
		end
		if  im.state == "message" then			
			if im.currentline > #im.lines then				
				return
			end
			local eltime = GetElapsedTime()
			if im.currentletter <= string.len(im.lines[im.currentline]) then 
				if im.lettertimer < im.letter_delay then 
					im.lettertimer = im.lettertimer + eltime 				
				else 
					im.lettertimer = 0				
					im.currentletter = im.currentletter + 1
					im.text[im.currentline] = im.text[im.currentline]..string.sub(im.lines[im.currentline],im.currentletter,im.currentletter)
					PlaySound(e,0)					
				end 
			else				
				if im.linetimer < im.lines_delay then 
					im.linetimer = im.linetimer + eltime
				else
					if im.currentline == 4 then im.lines_delay = im.lines_delay*im.display_time end
					im.currentline = im.currentline + 1
					if im.currentline > #im.lines then
						SetCameraOverride(0)
						return
					else 
						im.linetimer = 0
						im.lettertimer = 0
						im.currentletter = 1
						im.text[im.currentline] = string.sub(im.lines[im.currentline],im.currentletter,im.currentletter)						
					end					
				end
			end
			
			local ty = im.message_y
			local startline = 1
			local endline = im.lines_shown
			if im.currentline > im.lines_shown then				
				endline = im.currentline
				startline = endline - im.lines_shown + 1
			end
			for a = startline, endline do
				if im.text_colour == 1 then
					if im.text_style == 1 then TextColor(im.message_x, ty, im.text_size, im.text[a],255,255,255) end
					if im.text_style == 2 then TextCenterOnXColor(im.message_x, ty, im.text_size, im.text[a],255,255,255) end
				end
				if im.text_colour == 2 then 
					if im.text_style == 1 then TextColor(im.message_x, ty, im.text_size, im.text[a],100,255,100) end
					if im.text_style == 2 then TextCenterOnXColor(im.message_x, ty, im.text_size, im.text[a],100,255,100) end
				end
				if im.text_colour == 3 then 
					if im.text_style == 1 then TextColor(im.message_x, ty, im.text_size, im.text[a],0,100,255) end
					if im.text_style == 2 then TextCenterOnXColor(im.message_x, ty, im.text_size, im.text[a],0,100,255) end
				end
				if im.text_colour == 4 then 
					if im.text_style == 1 then TextColor(im.message_x, ty, im.text_size, im.text[a],255,255,0) end
					if im.text_style == 2 then TextCenterOnXColor(im.message_x, ty, im.text_size, im.text[a],255,255,0) end
				end
				if im.text_colour == 5 then 
					if im.text_style == 1 then TextColor(im.message_x, ty, im.text_size, im.text[a],255,170,0) end
					if im.text_style == 2 then TextCenterOnXColor(im.message_x, ty, im.text_size, im.text[a],255,170,0) end
				end
				if im.text_colour == 6 then 
					if im.text_style == 1 then TextColor(im.message_x, ty, im.text_size, im.text[a],255,0,0) end
					if im.text_style == 2 then TextCenterOnXColor(im.message_x, ty, im.text_size, im.text[a],255,0,0) end
				end
				ty = ty + im.text_size - 0.5
			end			
		end		
	end
end 
