-- Cursor Control module

local cursorcontrol = {}

function cursorcontrol.getinput(ptrX,ptrY,ptrClick)
 local vrcontrol = 0
 if GetHeadTracker() == 1 then
  if GetGamePlayerStateMotionController() == 1 and GetGamePlayerStateMotionControllerType() == 2 then -- OPENXR
   vrcontrol = 1
  end
 end
 if vrcontrol == 1 then 
  -- VR controllers laser
  hitit, hitx, hity, hitz = CombatControllerLaserGuidedHit(5997)
  ptrX = ((hitx+19.0)/38.0) * 100
  ptrY = ((11.0-hity)/22.0) * 100
  if CombatControllerTrigger() > 0.5 then
   ptrClick = 1
  else
   ptrClick = 0
  end
 else 
  if ( GetGamePlayerStateXBOX() == 1 ) then 
   -- Game Controller
   if ( GetGamePlayerStateXBOXControllerType() == 2 ) then 
    -- Logitech F310
    controllerDeltaX = (JoystickX()/400.0) * g_TimeElapsed
    controllerDeltaY = (JoystickY()/400.0) * g_TimeElapsed
    if math.abs(controllerDeltaX) < 0.015 then controllerDeltaX = 0.0 end
    if math.abs(controllerDeltaY) < 0.015 then controllerDeltaY = 0.0 end
    ptrX = ptrX + controllerDeltaY
    ptrY = ptrY + controllerDeltaX
    if ptrX < 0 then ptrX = 0 end
    if ptrY < 0 then ptrY = 0 end
    if ptrX > 99 then ptrX = 99 end
    if ptrY > 99 then ptrY = 99 end
    if ptrClick == 0 then
     if JoystickFireXL(0) == 1 or JoystickFireXL(1) == 1 then ptrClick = 2 end
     if JoystickFireXL(2) == 1 or JoystickFireXL(3) == 1 then ptrClick = 2 end
    else
     if ptrClick == 2 then
      if JoystickFireXL(0)+JoystickFireXL(1)+JoystickFireXL(2)+JoystickFireXL(3) == 0 then ptrClick = 1 end 
	 else  
	  ptrClick = 0
	 end
    end
   end
  else
   -- Regular Mouse
   ptrX = g_MouseX
   ptrY = g_MouseY
   ptrClick = g_MouseClick
  end
 end
 return ptrX, ptrY, ptrClick
end

return cursorcontrol
