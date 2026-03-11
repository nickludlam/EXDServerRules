// SRV_RulesChatCommand - Handles /rules chat command
enum SRV_ChatCommandType
{
	None,
	Rules,
	RulesReload
}

modded class SCR_ChatComponent : BaseChatComponent
{
	//------------------------------------------------------------------------------------------------
	override void OnNewMessage(string msg, int channelId, int senderId)
	{
		super.OnNewMessage(msg, channelId, senderId);

		// Normalise command text
		string processedMsg = msg;
		processedMsg.Trim();
		processedMsg.ToLower();

		// This callback is client-side chat/UI; server reload is requested via RPC
		if (System.IsConsoleApp())
			return;

		SRV_ChatCommandType commandType = SRV_ChatCommandType.None;

		if (processedMsg == "/rulesreload" || processedMsg == "!rulesreload")
			commandType = SRV_ChatCommandType.RulesReload;
        else if (processedMsg == "/rules" || processedMsg == "!rules")
			commandType = SRV_ChatCommandType.Rules;

		if (commandType == SRV_ChatCommandType.None)
            return;

        PlayerController localPlayer = GetGame().GetPlayerController();
        if (!localPlayer)
            return;

        int localPlayerId = localPlayer.GetPlayerId();
        if (localPlayerId != senderId)
            return;

		if (commandType == SRV_ChatCommandType.RulesReload)
		{
			GetGame().GetCallqueue().CallLater(RequestServerRulesReload, 100, false);
		}
        else if (commandType == SRV_ChatCommandType.Rules)
		{			
			GetGame().GetCallqueue().CallLater(ShowRulesDelayed, 100, false);
		}
	}

	//------------------------------------------------------------------------------------------------
    protected void RequestServerRulesReload()
    {
        PlayerController pc = GetGame().GetPlayerController();
        if (!pc)
            return;
        
        SCR_PlayerController rulesPc = SCR_PlayerController.Cast(pc);
        if (!rulesPc)
            return;
        
        rulesPc.SRV_RequestReloadRulesFromClient();
    }
	
	//------------------------------------------------------------------------------------------------
	protected void ShowRulesDelayed()
	{
		Print("[Server Rules] Showing rules from chat command (delayed)");
		SRV_RulesDialogUI.ShowCachedRules();
	}
}
