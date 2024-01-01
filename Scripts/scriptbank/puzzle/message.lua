-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Message v8: by Necrym59 with special thanks to Smallg and Amen Moses 

-- DESCRIPTION: Will display a Message on screen when triggered from a zone or switch.
-- DESCRIPTION: Attach to an object and link to a zone or switch.
-- DESCRIPTION: [TEXT1$="MISSION DEBRIEF: Operation X5s"]
-- DESCRIPTION: [TEXT2$="Cpt. Hughes - Infiltration Unit 6"]
-- DESCRIPTION: [TEXT3$="Deploy and infiltrate Maersk Operations Complex"]
-- DESCRIPTION: [TEXT4$="Seek and destroy valid targets and recover"]
-- DESCRIPTION: [TEXT5$="classified documents from Dr. Gebchev"]
-- DESCRIPTION: [X=10]
-- DESCRIPTION: [Y=10]
-- DESCRIPTION: [@COLOUR = 1(1=White, 2=Green, 3=Blue, 4=Yellow, 5=Orange, 6=Red)]
-- DESCRIPTION: [SIZE=3(1,5)]
-- DESCRIPTION: [@STYLE=1(1=Left, 2=Center)]
-- DESCRIPTION: [LINES_SHOWN=5(1,5)]
-- DESCRIPTION: [LETTER_DELAY#=0.05]
-- DESCRIPTION: [LINES_DELAY#=0.8]
-- DESCRIPTION: [DISPLAY_TIME=5(1,10)]
-- DESCRIPTION: [LOCK_PLAYER!=0]
-- DESCRIPTION: Play <Sound0> when message typed

g_message = {}
local sub = string.sub

function message_properties( e, text1, text2, text3, text4, text5, x, y, colour, size, style, lines_shown, letter_delay, lines_delay, display_time, lock_player )
	g_message[e] = 
	     { message_x    = x,
	       message_y    = y,
	       text_colour  = colour,
	       text_size    = size,
	       text_style   = style,
	       lines_shown  = lines_shown,	
	       letter_delay = letter_delay,
	       lines_delay  = lines_delay,
	       display_time = display_time,
	       lock         = ( lock_player == 1 ),
	       state        = "init",
           msglines     = { text1, text2, text3, text4, text5 },
		   msgtext = {"", "", "", "", ""}
         }		   
end

function message_init(e)
	g_message[e] = {}		
end 

local colors = { { r = 255, g = 255, b = 255 },  -- White
                 { r = 100, g = 255, b = 100 },  -- Green
				 { r =   0, g = 100, b = 255 },  -- Blue
				 { r = 255, g = 255, b =   0 },  -- Yellow
				 { r = 255, g = 170, b =   0 },  -- Orange
				 { r = 255, g =   0, b =   0 }   -- Red
			   }

function message_main(e)
	local im = g_message[e]
	if im == nil or im.state == nil then return end
	
	local Ent = g_Entity[e]
	if Ent == nil then return end
	
	if Ent.activated == 1 then	    
		if im.state == "init" then 
			if im.lock then SetCameraOverride(3) end
			im.lettertimer   = 0
			im.linetimer     = 0
			im.currentline   = 1
			im.currentletter = 1
			im.msgtext[im.currentline] = sub(im.msglines[im.currentline],im.currentletter,im.currentletter)		
			im.state = "message"
			return			
		
		elseif
		   im.state == "message" then			
			local eltime = GetElapsedTime()
			if im.currentletter <= #(im.msglines[im.currentline]) then 
				if im.lettertimer < im.letter_delay then 
					im.lettertimer = im.lettertimer + eltime 				
				else 
					im.lettertimer = 0				
					im.currentletter = im.currentletter + 1
					im.msgtext[im.currentline] = im.msgtext[im.currentline] .. 
					                             sub(im.msglines[im.currentline],im.currentletter,im.currentletter)
					PlaySound(e,0)					
				end 
			else				
				if im.linetimer < im.lines_delay then 
					im.linetimer = im.linetimer + eltime
				else
					if im.currentline == 4 then im.lines_delay = im.lines_delay * im.display_time end
					im.currentline = im.currentline + 1
					if im.currentline > #im.msglines then
						SetCameraOverride(0)
						im.state = "done"
						return
					else 
						im.linetimer     = 0
						im.lettertimer   = 0
						im.currentletter = 1
						im.msgtext[im.currentline] = sub(im.msglines[im.currentline], im.currentletter, im.currentletter)						
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
				local cs = colors[ im.text_colour ]
				if cs then
					if im.text_style == 1 then
						TextColor( im.message_x, ty, im.text_size, im.msgtext[a], cs.r, cs.g, cs.b )
					else 
						TextCenterOnXColor( im.message_x, ty, im.text_size, im.msgtext[a], cs.r, cs.g, cs.b ) 
					end

				end
				ty = ty + im.text_size - 0.5
			end			
		end		
	end
end 
