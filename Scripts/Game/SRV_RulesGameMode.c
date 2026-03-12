// SRV_RulesGameMode - Hooks into game mode to show rules on player connect
modded class SCR_BaseGameMode : BaseGameMode
{
	protected ref map<int, string> m_mSRVPendingAgreements = new map<int, string>();
	protected ref map<int, string> m_mSRVPendingNames = new map<int, string>();
	
	//------------------------------------------------------------------------------------------------
	override void OnGameStart()
	{
		super.OnGameStart();
		
		Print("[Server Rules] GameMode OnGameStart - Initializing");
		
		// Initialize config on server start
		if (System.IsConsoleApp())
		{
			Print("[Server Rules] Running on dedicated server");
			SRV_RulesConfigManager.GetInstance().Initialize();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPlayerRegistered(int playerId)
	{
		super.OnPlayerRegistered(playerId);
		
		Print("[Server Rules] OnPlayerRegistered called for player: " + playerId.ToString());
		
		// Only run on server
		if (!System.IsConsoleApp())
			return;
		
		// Delay to let player identity be ready
		GetGame().GetCallqueue().CallLater(SRV_CheckPlayerRules, 2000, false, playerId);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPlayerSpawned(int playerId, IEntity controlledEntity)
	{
		super.OnPlayerSpawned(playerId, controlledEntity);
		
		// Only run on server
		if (!System.IsConsoleApp())
			return;
		
		Print("[Server Rules] OnPlayerSpawned for player: " + playerId.ToString());
		
		// If player was pending agreement and they spawned, they agreed
		if (m_mSRVPendingAgreements.Contains(playerId))
		{
			string playerGUID = m_mSRVPendingAgreements.Get(playerId);
			string name = m_mSRVPendingNames.Get(playerId);
			
			Print("[Server Rules] Player spawned - saving agreement for: " + name);
			SRV_RulesTracker.SaveAgreement(playerGUID, name);
			
			m_mSRVPendingAgreements.Remove(playerId);
			m_mSRVPendingNames.Remove(playerId);
		}
	}

	//------------------------------------------------------------------------------------------------
	void SRV_RefreshRulesCacheForAllPlayers()
	{
		if (!System.IsConsoleApp())
			return;

		SRV_RulesConfig config = SRV_RulesConfigManager.GetInstance().GetConfig();
		if (!config || !config.IsEnabled())
		{
			Print("[Server Rules] Rules disabled after reload - skipping cache refresh");
			return;
		}

		string title = config.GetTitle();
		string rulesText = "";
		array<string> rules = config.GetRules();
		foreach (string rule : rules)
		{
			rulesText += rule + "\n\n";
		}

		PlayerManager playerManager = GetGame().GetPlayerManager();
		if (!playerManager)
			return;

		array<int> playerIds = {};
		playerManager.GetPlayers(playerIds);

		foreach (int playerId : playerIds)
			Rpc(SRV_RpcDo_CacheRulesOnly, playerId, title, rulesText, config.UseLargeDialog());

		Print("[Server Rules] Refreshed rules cache for " + playerIds.Count().ToString() + " players");
	}

	//------------------------------------------------------------------------------------------------
	void SRV_RequestRulesReloadFromClient(int senderId)
	{
		Print("[Server Rules] SRV_RequestRulesReloadFromClient senderId=" + senderId.ToString() + " IsServer=" + Replication.IsServer().ToString() + " IsConsole=" + System.IsConsoleApp().ToString());

		if (!Replication.IsServer() || !System.IsConsoleApp())
			return;

		SRV_RulesConfig config = SRV_RulesConfigManager.GetInstance().GetConfig();
		if (!config.IsRulesReloadCommandEnabled())
		{
			Print("[Server Rules] Reload command ignored - disabled in config (enable_rules_reload_command=false)", LogLevel.WARNING);
			return;
		}

		bool reloaded = SRV_RulesConfigManager.GetInstance().CreateOrLoadConfig();
		if (!reloaded)
		{
			Print("[Server Rules] Reload command failed", LogLevel.ERROR);
			return;
		}

		SRV_RefreshRulesCacheForAllPlayers();
		Print("[Server Rules] Reload command completed successfully");
	}
	
	//------------------------------------------------------------------------------------------------
	protected void SRV_CheckPlayerRules(int playerId)
	{
		Print("[Server Rules] CheckPlayerRules for player: " + playerId.ToString());
		
		// Check if rules are enabled
		SRV_RulesConfig config = SRV_RulesConfigManager.GetInstance().GetConfig();
		if (!config.IsEnabled())
		{
			Print("[Server Rules] Rules are disabled");
			// Send empty/disabled indicator to client so !rules command knows rules are off
			Rpc(SRV_RpcDo_RulesDisabled, playerId);
			return;
		}
		
		// Get player info
		PlayerManager playerManager = GetGame().GetPlayerManager();
		if (!playerManager)
		{
			Print("[Server Rules] PlayerManager is null");
			return;
		}
		
		string playerName = playerManager.GetPlayerName(playerId);
		Print("[Server Rules] Player name: " + playerName);
		
		// Get player identity ID using BackendApi
		string playerGUID = SCR_PlayerIdentityUtils.GetPlayerIdentityId(playerId);
		Print("[Server Rules] Player GUID: " + playerGUID);
		
		if (!playerGUID || playerGUID.Length() == 0)
		{
			Print("[Server Rules] Player GUID is empty, can only track by player name");
			//return;
		}
		
		// Build rules text on server
		string title = config.GetTitle();
		string rulesText = "";
		array<string> rules = config.GetRules();
		foreach (string rule : rules)
		{
			rulesText += rule + "\n";
		}
		
		// Check if display_always is enabled
		bool displayAlways = config.IsDisplayAlways();
		
		// Check if player already agreed
		bool hasAgreed = SRV_RulesTracker.HasPlayerAgreed(playerGUID);
		
		if (hasAgreed && !displayAlways)
		{
			// Player already agreed - just send rules for caching (for !rules command)
			Print("[Server Rules] Player already agreed: " + playerName + " - caching rules only");
			Rpc(SRV_RpcDo_CacheRulesOnly, playerId, title, rulesText, config.UseLargeDialog());
			return;
		}
		
		if (displayAlways)
		{
			Print("[Server Rules] Display always enabled - showing rules to: " + playerName);
		}
		else
		{
			Print("[Server Rules] New player needs to agree: " + playerName);
		}
		
		// Store pending agreement - will be saved when player spawns
		m_mSRVPendingAgreements.Set(playerId, playerGUID);
		m_mSRVPendingNames.Set(playerId, playerName);
		
		Print("[Server Rules] Sending rules to client - Title: " + title + " Rules length: " + rulesText.Length().ToString());
		
		// Request client to show dialog via RPC - send title and rules
		Rpc(SRV_RpcDo_ShowRulesDialog, playerId, playerGUID, playerName, title, rulesText, config.UseLargeDialog());
	}
	
	//------------------------------------------------------------------------------------------------
	// Server -> Client: Just cache rules (for !rules command) without showing dialog
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void SRV_RpcDo_CacheRulesOnly(int targetPlayerId, string title, string rulesText, bool useLargeDialog)
	{
		// Only run on the target client
		PlayerController localPlayer = GetGame().GetPlayerController();
		if (!localPlayer)
			return;
		
		int localPlayerId = localPlayer.GetPlayerId();
		if (localPlayerId != targetPlayerId)
			return;
		
		Print("[Server Rules] Caching rules for !rules command");
		SRV_RulesDialogUI.CacheRules(title, rulesText);
	}
	
	//------------------------------------------------------------------------------------------------
	// Server -> Client: Show the rules dialog with rules text
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void SRV_RpcDo_ShowRulesDialog(int targetPlayerId, string playerGUID, string playerName, string title, string rulesText, bool useLargeDialog)
	{
		Print("[Server Rules] RpcDo_ShowRulesDialog received for: " + targetPlayerId.ToString());
		
		// Only run on the target client
		PlayerController localPlayer = GetGame().GetPlayerController();
		if (!localPlayer)
		{
			Print("[Server Rules] No local player controller");
			return;
		}
		
		int localPlayerId = localPlayer.GetPlayerId();
		
		if (localPlayerId != targetPlayerId)
			return;
		
		Print("[Server Rules] Showing dialog for: " + playerName + " with title: " + title);
		
		// Show dialog with title and rules from server
		SRV_RulesDialogUI.ShowWithRules(playerGUID, playerName, title, rulesText, useLargeDialog);
	}
	
	//------------------------------------------------------------------------------------------------
	// Server -> Client: Rules are disabled on server
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void SRV_RpcDo_RulesDisabled(int targetPlayerId)
	{
		// Only run on the target client
		PlayerController localPlayer = GetGame().GetPlayerController();
		if (!localPlayer)
			return;
		
		int localPlayerId = localPlayer.GetPlayerId();
		if (localPlayerId != targetPlayerId)
			return;
		
		Print("[Server Rules] Rules are disabled on server");
		SRV_RulesDialogUI.SetRulesDisabled();
	}
}
