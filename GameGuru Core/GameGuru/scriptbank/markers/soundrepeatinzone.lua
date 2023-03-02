-- DESCRIPTION: Loops the audio from <Sound0> when the player enters this zone, and stop other zone audio from playing.

g_soundrepeatinzone_currentlyplayinge = 0
g_soundrepeatinzone_played = {}

function soundrepeatinzone_init(e)
	g_soundrepeatinzone_played[e] = 0
end

function soundrepeatinzone_main(e)
 if g_Entity[e]['plrinzone']==1 then
  if g_soundinzone_currentlyplayinge ~= e then
   if g_soundrepeatinzone_played[e] == 1 then
   	if GetGamePlayerStateXBOX() == 1 then
     Prompt("Press Y button to repeat audio")
	else
	 if GetHeadTracker() == 1 then
      Prompt("Trigger to repeat audio")
	 else
      Prompt("Press E key to repeat audio")
	 end
	end
    if g_KeyPressE == 1 then 
	 g_soundrepeatinzone_played[e] = 0
    end
   end
   if g_soundrepeatinzone_played[e] == 0 then
    if g_soundinzone_currentlyplayinge ~= 0 then
     StopSound(g_soundinzone_currentlyplayinge,0)
	 StopSound(g_soundinzone_currentlyplayinge,1)
	 StopSound(g_soundinzone_currentlyplayinge,2)
	 StopSound(g_soundinzone_currentlyplayinge,3)
     g_soundinzone_currentlyplayinge = 0
    end
    g_soundinzone_currentlyplayinge = e
    g_soundrepeatinzone_played[e] = 1
	PlaySound(e,0)
    PerformLogicConnections(e)
   end
  end
 end
end
