-- DESCRIPTION: While the player is within the zone the [PROMPTTEXT$=""] is displayed on screen. [ZONEHEIGHT=100] controls how far above the zone the player can be before the zone is not triggered. Set[!ONLYONCE=0] for the text zone to be destroyed after activation.

g_textinzone = {}

function textinzone_properties(e, prompttext, zoneheight, onlyonce)
 g_textinzone[e]['prompttext'] = prompttext
 g_textinzone[e]['zoneheight'] = zoneheight
 g_textinzone[e]['onlyonce'] = onlyonce
end 

function textinzone_init(e)
 g_textinzone[e] = {}
 g_textinzone[e]['prompttext'] = ""
 g_textinzone[e]['zoneheight'] = 100
 g_textinzone[e]['onlyonce'] = 0
end

function textinzone_main(e)
 if g_textinzone[e]['zoneheight'] == nil then g_textinzone[e]['zoneheight'] = 100 end
 if g_textinzone[e]['zoneheight'] ~= nil then
  if g_Entity[e]['plrinzone']==1 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+g_textinzone[e]['zoneheight'] then
   PerformLogicConnections(e)
   local nText = g_textinzone[e]['prompttext']
   Prompt(nText)
   if g_textinzone[e]['onlyonce'] == 1 then
   Destroy(e)
   end
  end
 end
end
