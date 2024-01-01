-- DESCRIPTION: Attach this script to entity to add objective blips on the radar
-- DESCRIPTION: [@Colour=1(1=Red, 2=Blue, 3=Green, 4=Yellow, 5=White, 6=Purple, 7=Orange)]
-- DESCRIPTION: [Hide!=0]
-- DESCRIPTION: [Destroy!=0]

local U = require "scriptbank\\utillib"

local objectives = {}
local colours = {'red', 'blue', 'green', 'yellow', 'white', 'purple', 'orange'}

function radar_objective_init_name( e, name )
	objectives[ e ] = { name = name, reached = false }
	SetEntityHealth( e, 30000 )
end

function radar_objective_properties( e, colour, hide, destroy, prompt_text)
	objectives[ e ].colour  	= colours[ colour ]
	objectives[ e ].hide    	= hide    == 1
	objectives[ e ].destroy 	= destroy == 1
end

function isObjective( e )
	if objectives[ e ] ~= nil then
		if not objectives[ e ].reached or
           not objectives[ e ].hide    then
			return objectives[ e ].colour
		end
	end
end

local timer = 0

function radar_objective_main( e )
		
	if g_Time > timer then
		timer = timer + 1000  -- check every second
		
		for k, v in pairs( objectives ) do
			if not v.reached and 
			   U.PlayerCloserThan( k, 150 ) then
				PromptDuration( "You have reached objective " .. v.name, 3000 )
				PlaySound( k, 0 )
				if v.destroy then
					Destroy( k )
					objectives[ k ] = nil
				else
					v.reached = true
				end
				break
			end
		end
	end 
end
