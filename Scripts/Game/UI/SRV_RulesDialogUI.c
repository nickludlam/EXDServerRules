// SRV_RulesDialogUI - Modal dialog with Agree and Quit buttons
class SRV_RulesDialogUI
{
	protected static const string PRESET_SERVER_RULES = "server_rules";
	protected static const string PRESET_SERVER_RULES_LARGE = "server_rules_large";

	protected static ref SCR_ConfigurableDialogUi s_Dialog;
	protected static string s_PlayerGUID;
	protected static string s_PlayerName;
	
	// Cache for chat command
	protected static string s_CachedTitle = "Server Rules";
	protected static string s_CachedRules = "";
    protected static bool s_bCachedUseLargeDialog = false;
	protected static bool s_bRulesDisabled = false;
	
	
	//------------------------------------------------------------------------------------------------
	// New method that receives rules from server
	static void ShowWithRules(string playerGUID, string playerName, string title, string rulesText, bool useLargeDialog = false)
	{
		Print("[Server Rules] Opening dialog for: " + playerName + " Title: " + title);
		
		s_PlayerGUID = playerGUID;
		s_PlayerName = playerName;
		
		// Cache the rules for chat command
		s_CachedTitle = title;
		s_CachedRules = rulesText;
        s_bCachedUseLargeDialog = useLargeDialog;

		string presetName = PRESET_SERVER_RULES;
		if (s_bCachedUseLargeDialog)
			presetName = PRESET_SERVER_RULES_LARGE;
		
		// Build full message with instructions
		string fullMessage = "\n" + rulesText + "\nBy clicking Confirm, you agree to and acknowledge the server rules.\nBy clicking Cancel, you disagree with the rules and will return to the main menu.\n";
		
		// Here we deviate from the original and create our own custom dialog called 'accept_rules'
		s_Dialog = SCR_ConfigurableDialogUi.CreateFromPreset(
			"Configs/Dialogs/CustomDialogs.conf",
			presetName
		);
		
		if (s_Dialog)
		{
			Print("[Server Rules] Instantiated dialog");
			s_Dialog.SetTitle(title);
			s_Dialog.SetMessage(fullMessage);
			s_Dialog.m_OnConfirm.Insert(OnAgree);
			s_Dialog.m_OnCancel.Insert(OnQuit);
			return;
		}

		// If no two-button preset found, use error dialog as fallback
		Print("[Server Rules] No two-button preset found, using ErrorDialog");
		DialogUI errorDialog = DialogUI.Cast(GetGame().GetMenuManager().OpenDialog(ChimeraMenuPreset.ErrorDialog));
		if (errorDialog)
		{
			errorDialog.SetTitle(title);
			errorDialog.SetMessage(fullMessage);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	static void OnAgree()
	{
		Print("[Server Rules] Player clicked AGREE: " + s_PlayerName);
		// Agreement will be saved by server when player spawns
		
		if (s_Dialog)
		{
			s_Dialog.Close();
			s_Dialog = null;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	static void OnQuit()
	{
		Print("[Server Rules] Player clicked CANCEL - disconnecting: " + s_PlayerName);
		
		if (s_Dialog)
		{
			s_Dialog.Close();
			s_Dialog = null;
		}
		
		// Disconnect to main menu
		GameStateTransitions.RequestGameplayEndTransition();
	}
	
	//------------------------------------------------------------------------------------------------
	// View-only mode for chat command - just shows rules with OK button
	static void ShowViewOnly(string title, string rulesText)
	{
		Print("[Server Rules] Opening view-only dialog");
		
		// Build message
		string fullMessage = "\n" + rulesText;
		
		string presetName = PRESET_SERVER_RULES;
		if (s_bCachedUseLargeDialog)
			presetName = PRESET_SERVER_RULES_LARGE;
		
		SCR_ConfigurableDialogUi dialog = SCR_ConfigurableDialogUi.CreateFromPreset(
			"Configs/Dialogs/CustomDialogs.conf",
			presetName
		);
		
		if (dialog)
		{
			dialog.SetTitle(title);
			dialog.SetMessage(fullMessage);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	// Set rules as disabled (no rules to show)
	static void SetRulesDisabled()
	{
		Print("[Server Rules] Rules marked as disabled");
		s_bRulesDisabled = true;
		s_CachedRules = "";
	}
	
	//------------------------------------------------------------------------------------------------
	// Cache rules without showing dialog (for players who already agreed)
	static void CacheRules(string title, string rulesText)
	{
		Print("[Server Rules] Caching rules - Title: " + title);
		s_CachedTitle = title;
		s_CachedRules = rulesText;
		s_bRulesDisabled = false;
	}
	
	//------------------------------------------------------------------------------------------------
	// Show cached rules from chat command
	static void ShowCachedRules()
	{
		Print("[Server Rules] Showing cached rules from chat command");
		
		// Don't show anything if rules are disabled
		if (s_bRulesDisabled)
		{
			Print("[Server Rules] Rules are disabled, not showing dialog");
			return;
		}
		
		if (s_CachedRules.Length() == 0)
		{
			ShowViewOnly("Server Rules", "Rules not yet loaded.\n\nRules are shown when you first connect to the server.");
			return;
		}
		
		ShowViewOnly(s_CachedTitle, s_CachedRules);
	}
	
	//------------------------------------------------------------------------------------------------
	// Check if rules are cached
	static bool HasCachedRules()
	{
		return s_CachedRules.Length() > 0;
	}
}
