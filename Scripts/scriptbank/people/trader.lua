-- DESCRIPTION: When player is within [RANGE=100] distance,
-- DESCRIPTION: show [TRADE_PROMPT$="Press E to trade"] and when E is pressed,
-- DESCRIPTION: character will trade with [TRADE_SCREEN$="HUD Screen 5"] using [SHOP_CONTAINER$=""] container
-- DESCRIPTION: and play [SPEECH1$=""].
-- DESCRIPTION: The shop is called [TRADE_SHOPNAME$="The Shop"]


master_interpreter_core = require "scriptbank\\masterinterpreter"

g_trader = {}
g_trader_behavior = {}
g_trader_behavior_count = 0

function trader_init(e)
 g_trader[e] = {}
 g_trader[e]["bycfilename"] = "scriptbank\\people\\trader.byc"
 g_trader_behavior_count = master_interpreter_core.masterinterpreter_load (g_trader[e], g_trader_behavior )
 trader_properties(e,100,"Press E to trade","HUD Screen 5","","","The Shop")
end

function trader_properties(e, range, tradeprompt, tradescreen, shopcontainer, speech1, tradeshopname)
 g_trader[e]['range'] = range
 g_trader[e]['tradeprompt'] = tradeprompt
 g_trader[e]['tradescreen'] = tradescreen
 g_trader[e]['shopcontainer'] = shopcontainer
 g_trader[e]['speech1'] = speech1
 g_trader[e]['tradeshopname'] = tradeshopname
 master_interpreter_core.masterinterpreter_restart (g_trader[e], g_Entity[e])
end

function trader_main(e)
 if g_trader[e] ~= nil and g_trader_behavior_count > 0 then
  g_trader_behavior_count = master_interpreter_core.masterinterpreter (g_trader_behavior, g_trader_behavior_count, e, g_trader[e], g_Entity[e])
 end
end
