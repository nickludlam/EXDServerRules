// SRV_RulesConfigManager - Handles server_rules.json configuration
class SRV_RulesConfigManager
{
	// Singleton
	protected static ref SRV_RulesConfigManager s_Instance;
	
	// Config
	protected static ref SRV_RulesConfig s_RulesConfig = new SRV_RulesConfig();
	
	// Paths
	protected static const string CONFIG_DIR = "$profile:server_rules/";
	protected static const string CONFIG_FILE = "$profile:server_rules/server_rules.json";
	protected static const string AGREED_DIR = "$profile:server_rules/agreed/";
	
	protected bool m_bLoaded = false;
	
	//------------------------------------------------------------------------------------------------
	static ref SRV_RulesConfigManager GetInstance()
	{
		if (s_Instance)
			return s_Instance;
		
		s_Instance = new SRV_RulesConfigManager();
		return s_Instance;
	}
	
	//------------------------------------------------------------------------------------------------
	void Initialize()
	{
		if (m_bLoaded) {
			Print("[Server Rules] Loaded already");
			return;
		}
		
        if (CreateOrLoadConfig())
        {
            Print("[Server Rules] Config loaded successfully during initialization");
        }
        else
        {
            Print("[Server Rules] Failed to load config during initialization", LogLevel.ERROR);
        }
		
		m_bLoaded = true;
		Print("[Server Rules] Config initialized - Server: " + System.IsConsoleApp().ToString() + ", DisplayAlways: " + s_RulesConfig.IsDisplayAlways().ToString());
	}

	//------------------------------------------------------------------------------------------------
	bool CreateOrLoadConfig()
	{
		if (!System.IsConsoleApp())
		{
			return false;
		}

		CreateDirectories();

		if (!LoadConfig())
		{
			Print("[Server Rules] Failed to load rules config", LogLevel.ERROR);

			if (!FileIO.FileExists(CONFIG_FILE))
			{
				Print("[Server Rules] Creating first-run default config");
				CreateDefaultConfig();
				return LoadConfig();
			}

            ShowLoadErrors();
			return false;
		}

		Print("[Server Rules] Rules config reloaded successfully");
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void CreateDirectories()
	{
		// Only create on server
		if (!System.IsConsoleApp())
			return;
		
		// Create main directory
		if (!FileIO.FileExists(CONFIG_DIR))
			FileIO.MakeDirectory(CONFIG_DIR);
		
		// Create agreed directory
		if (!FileIO.FileExists(AGREED_DIR))
			FileIO.MakeDirectory(AGREED_DIR);
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool LoadConfig()
	{
	    SCR_JsonLoadContext context = new SCR_JsonLoadContext();
	
	    // 1. Load the file
	    if (!context.LoadFromFile(CONFIG_FILE))
	    {
	        Print("Failed to load JSON file!", LogLevel.ERROR);
	        return false;
	    }
	
	    // 2. Map the JSON to the class
	    if (!s_RulesConfig.RegV(context))
	    {
	        Print("JSON structure mismatch!", LogLevel.WARNING);
	        return false;
	    }
	
        int rulesLineCount = s_RulesConfig.GetRules().Count();
	    Print("[Server Rules] Loaded: " + s_RulesConfig.GetTitle() + " (" + rulesLineCount + " rule lines)");
		return true;
	}

    protected void ShowLoadErrors()
    {
	    SCR_JsonLoadContext context = new SCR_JsonLoadContext();
	    if (!context.LoadFromFile(CONFIG_FILE))
        {
            Print("[Server Rules] Failed to load JSON file for validation!", LogLevel.ERROR);
            return;
        }

        ref array<string> errors = {};

        if (!s_RulesConfig.Validate(context, errors))
        {
            Print("[Server Rules] Config validation errors:");
            foreach (string error : errors)
            {
                Print(" - " + error);
            }
        } else {
            Print("[Server Rules] No validation errors found, but loading still failed. Check JSON structure and types.");
        }
    }
	
	//------------------------------------------------------------------------------------------------
	protected void CreateDefaultConfig()
	{
        s_RulesConfig.SetFirstRun();

        // Now use the standard JSON serializer to save the default config
        SCR_JsonSaveContext saveContext = new SCR_JsonSaveContext();
        s_RulesConfig.Save(saveContext);
        if (saveContext.SaveToFile(CONFIG_FILE))
        {
            Print("[Server Rules] Created default config at: " + CONFIG_FILE);
        }
        else
        {
            Print("[Server Rules] Failed to create default config at: " + CONFIG_FILE, LogLevel.ERROR);
        }
	}
	
	//------------------------------------------------------------------------------------------------
	ref SRV_RulesConfig GetConfig() { return s_RulesConfig; }
	string GetAgreedDir() { return AGREED_DIR; }
}
