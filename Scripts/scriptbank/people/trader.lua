-- Trader v2 by Necrym59 and Lee
-- DESCRIPTION: When player is within [RANGE=100] distance,
-- DESCRIPTION: will show [TRADE_PROMPT$="Press E to trade"] and when E is pressed, 
-- DESCRIPTION: character will trade with [@@TRADE_SCREEN$="HUD Screen 5"(0=hudscreenlist)] using [SHOP_CONTAINER$=""] container
-- DESCRIPTION: and play [SPEECH1$=""].
-- DESCRIPTION: The shop is called [TRADE_SHOPNAME$="The Shop"]


master_interpreter_core = require "scriptbank\\masterinterpreter"

g_trader = {}
g_trader_behavior = {}
g_trader_behavior_count = 0

function trader_init_file(e,scriptfile)
 g_trader[e] = {}
 g_trader[e]["bycfilename"] = "scriptbank\\" .. scriptfile .. ".byc"
 g_trader_behavior_count = master_interpreter_core.masterinterpreter_load (g_trader[e], g_trader_behavior )
 trader_properties(e,100,"Press E to trade","HUD Screen 5","","","The Shop")
end

function trader_properties(e, range, trade_prompt, trade_screen, shop_container, speech1, trade_shopname)
 g_trader[e]['range'] = range
 g_trader[e]['tradeprompt'] = trade_prompt
 g_trader[e]['tradescreen'] = trade_screen
 g_trader[e]['shopcontainer'] = shop_container
 g_trader[e]['speech1'] = speech1
 g_trader[e]['tradeshopname'] = trade_shopname
 master_interpreter_core.masterinterpreter_restart (g_trader[e], g_Entity[e])
end

function trader_main(e)
 if g_trader[e] ~= nil and g_trader_behavior_count > 0 then
  g_trader_behavior_count = master_interpreter_core.masterinterpreter (g_trader_behavior, g_trader_behavior_count, e, g_trader[e], g_Entity[e])
 end
end
