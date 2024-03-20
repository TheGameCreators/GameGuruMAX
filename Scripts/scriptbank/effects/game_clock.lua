-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Game Clock v3 by Necrym59
-- DESCRIPTION: A global game clock for use by other scripts or for in-game. Set ALLWAYS ON
-- DESCRIPTION: [@DISPLAY_CLOCK=1(1=Yes, 2=No)]
-- DESCRIPTION: [@DISPLAY_TYPE=1(1=24 Hour, 2=12 Hours)]
-- DESCRIPTION: [DISPLAY_X=5(1,100)], [DISPLAY_Y=5(1,100)]
-- DESCRIPTION: [DISPLAY_TEXT_SIZE=3(1,5)]
-- DESCRIPTION: [START_HOURS=1(1,23)]
-- DESCRIPTION: [START_MINUTES=1(1,59)]
-- DESCRIPTION: [START_SECONDS=1(1,59)]

g_clock = {}
g_clock_seconds = {}
g_clock_minutes = {}
g_clock_hours = {}
g_clock_althours = {}
g_clock_timeofday = {}
timer = {}

local display_clock = {}
local display_type = {}
local display_x = {}
local display_y = {}
local display_text_size = {}
local start_hours = {}
local start_minutes = {}
local start_seconds = {}
local nullh = {}
local nullm = {}
local nulls = {}
local status = {}

function game_clock_properties(e, display_clock, display_type, display_x, display_y, display_text_size, start_hours, start_minutes, start_seconds)
    g_clock[e]["display_clock"] = display_clock
    g_clock[e]["display_type"] = display_type
	g_clock[e]["display_x"] = display_x
	g_clock[e]["display_y"] = display_y
	g_clock[e]["display_text_size"] = display_text_size
	g_clock[e]["start_hours"] = start_hours
	g_clock[e]["start_minutes"] = start_minutes
	g_clock[e]["start_seconds"] = start_seconds
end

function game_clock_init(e)
	g_clock[e] = {}
	g_clock[e]["display_clock"] = 1
    g_clock[e]["display_type"] = 1
	g_clock[e]["display_x"] = 5
	g_clock[e]["display_y"] = 5
	g_clock[e]["display_text_size"] = 3
	g_clock[e]["start_hours"] = 0
	g_clock[e]["start_minutes"] = 0
	g_clock[e]["start_seconds"] = 0
	g_clock_hours = 0
	g_clock_minutes = 0
	g_clock_seconds = 0
	timer[1] = 0
	status[e] = "init"
end

function game_clock_main(e)	

	if status[e] == "init" then
		g_clock_hours = g_clock[e]["start_hours"]
		g_clock_minutes = g_clock[e]["start_minutes"]
		g_clock_seconds = g_clock[e]["start_seconds"]
		status[e] = "endinit"
	end
	
    if timer[1] == nil then
		timer[1] = g_Time + 1000
    elseif g_Time > timer[1] then
        timer[1] = g_Time + 1000	
        g_clock_seconds = g_clock_seconds + 1		
	end
    if g_clock_seconds == 60 then
		g_clock_minutes = g_clock_minutes + 1
        g_clock_seconds = 0
	end
	if g_clock_minutes == 60 then
		g_clock_hours = g_clock_hours + 1
		g_clock_minutes = 0
	end
	if g_clock_hours == 24 then
		g_clock_hours = 0
	end
	if g_clock_hours < 12 then
		g_clock_timeofday = "AM"
	else
		g_clock_timeofday = "PM"		
	end
	if g_clock_hours  > 12 then
		g_clock_althours = g_clock_hours -12
	else
		g_clock_althours = g_clock_hours
	end
	
	----- Display Game Clock -----
    if g_clock[e]["display_clock"] == 1 then		
		if g_clock_hours < 10 then nullh = "0" else nullh = "" end
		if g_clock_minutes < 10 then nullm = "0" else nullm = "" end
		if g_clock_seconds < 10 then nulls = "0" else nulls = "" end		
		if g_clock[e]["display_type"] == 1 then
			Text(g_clock[e]["display_x"],g_clock[e]["display_y"],g_clock[e]["display_text_size"],nullh.. "" ..g_clock_hours.. ":" ..nullm.. "" ..g_clock_minutes.. ":" ..nulls.. "" ..g_clock_seconds.. " " ..g_clock_timeofday)
		end
		if g_clock[e]["display_type"] == 2 then
			Text(g_clock[e]["display_x"],g_clock[e]["display_y"],g_clock[e]["display_text_size"],nullh.. "" ..g_clock_althours.. ":" ..nullm.. "" ..g_clock_minutes.. ":" ..nulls.. "" ..g_clock_seconds.. " " ..g_clock_timeofday)
		end
	end
end

function game_clock_exit(e)
end