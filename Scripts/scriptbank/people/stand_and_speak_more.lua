-- DESCRIPTION: Stand and speak with multiple speaks. 
-- DESCRIPTION: [UseEKey!=0]
-- DESCRIPTION: [Volume=80(0,100)]
-- DESCRIPTION: [Delay=1] (seconds)
-- DESCRIPTION: [NumberOfSpeaks=1(1,4)]
-- DESCRIPTION: Speech file (.wav) for lip-sync [SPEECH0$=""]
-- DESCRIPTION: Speech file (.wav) for lip-sync [SPEECH1$=""]
-- DESCRIPTION: Speech file (.wav) for lip-sync [SPEECH2$=""]
-- DESCRIPTION: Speech file (.wav) for lip-sync [SPEECH3$=""]

local peeps = {}

function stand_and_speak_more_properties( e, usekey, volume, delay, num )
	local peep = peeps[ e ]
	if peep == nil then return end
	peep.UseEKey = usekey == 1
	peep.Volume  = volume
	peep.Delay   = delay * 1000
	peep.NumSpks = num
end				   

local U = require "scriptbank\\utillib"

function stand_and_speak_more_init( e )
	local Ent = g_Entity[ e ] 
	peeps[ e ] = { state  = 'idle',
                   speech = 0				   
				  }
					   
	CharacterControlManual( e )
	CharacterControlUnarmed( e )
	AISetEntityControl( Ent.obj, AI_MANUAL )
	SetEntityHealthSilent( e, 999999 )
	SetAnimationName( e, "Idle" )
	LoopAnimation( e )
end

local function Speak( e, num, vol )
	PlaySpeech( e, num )
	SetSound( e, num )
	SetSoundVolume( vol  )
end
	
function stand_and_speak_more_main( e )
	local peep = peeps[ e ]
	if peep == nil then return end
	
	if peep.state == 'idle' and
	   U.PlayerLookingNear( e, 100, 150 ) then

		if peep.speech == 0 or
		   ( peep.UseEKey and g_KeyPressE == 1 ) then
			peep.state = 'speak'
		elseif
		   peep.delay ~= nil and
		   g_Time > peep.delay then
			peep.state = 'speak'
		end
		
	elseif
	   peep.state == 'speak' then
		Speak( e, peep.speech, peep.Volume )
		peep.state = 'wait'
	elseif
	   peep.state == 'wait' then
		LookAtTarget(e,0)
		LookAtPlayer(e,0)	   

		if GetSpeech( e ) == 0 then
			peep.speech = peep.speech + 1
			if peep.speech > peep.NumSpks then 
				peep.state = 'done'
			else
				if not peep.UseEKey then
					peep.delay = g_Time + peep.Delay
				else
					peep.delay = math.huge
				end
				peep.state = 'idle'
			end
		end
	end
	--PromptLocal( e, peep.state .. ", " .. peep.speech .. ", " .. GetSpeech( e )  )
end
 

	
	


