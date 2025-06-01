-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Global Set String v2 by Lee and Necrym59
-- DESCRIPTION: Sets a user defined global text to a new string
-- DESCRIPTION: [@@USER_GLOBAL$=""(0=globallist)] eg: MyRewardText
-- DESCRIPTION: [NEW_STRING$=""]

g_global_set_string = {}

function global_set_string_properties(e, user_global, new_string)
	g_global_set_string[e].user_global = user_global
	g_global_set_string[e].new_string = new_string
end

function global_set_string_init(e)
	g_global_set_string[e] = {}
	g_global_set_string[e].user_global = ""
	g_global_set_string[e].new_string = ""
	g_global_set_string[e].initcomplete = 0
end

function global_set_string_main(e)
	if g_global_set_string[e].initcomplete == 0 then
		local globvar = g_global_set_string[e].user_global
		if globvar > "" then
			_G["g_UserGlobal['"..globvar.."']"] = g_global_set_string[e].new_string
		end
		g_global_set_string[e].initcomplete = 1
	end
end
