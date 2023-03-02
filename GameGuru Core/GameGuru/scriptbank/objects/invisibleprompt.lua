-- DESCRIPTION: This object will be made invisible and a text prompt will be shown when the player is within [RANGE=100(60,3000)] of the object. The prompt text used will be taken from the `Name` filed in the entities properties.

g_invisibleprompt_text = {}
g_invisibleprompt = {}

function invisibleprompt_properties(e,range)
	g_invisibleprompt[e]['range'] = range
end 

function invisibleprompt_init_name(e,name)
	g_invisibleprompt[e] = {}
	g_invisibleprompt[e]['range'] = 100
	g_invisibleprompt_text[e] = name
end
function invisibleprompt_main(e)
 Hide(e)
 local PlayerDist = GetPlayerDistance(e)
 local nrange = g_invisibleprompt[e]['range']
 if PlayerDist < nrange and g_PlayerHealth > 0 then
  PromptLocal ( e, g_invisibleprompt_text[e] )
 end
end
