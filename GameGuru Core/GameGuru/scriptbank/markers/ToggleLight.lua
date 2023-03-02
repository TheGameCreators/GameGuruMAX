-- DESCRIPTION: Light will toggle on and off when it is activated by another object or game element. Use the [LightOn!=1] state to decide if the light is initially off or on.

module_lightcontrol = require "scriptbank\\markers\\module_lightcontrol"

g_togglelight = {}

function ToggleLight_properties(e, lighton)
 module_lightcontrol.init(e, lighton)
end 

function ToggleLight_init(e)
end

function ToggleLight_main(e)
 module_lightcontrol.control(e)
end
