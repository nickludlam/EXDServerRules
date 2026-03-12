// SRV_RulesTracker - Tracks player agreement to rules
class SRV_RulesTracker
{
	//------------------------------------------------------------------------------------------------
	// Check if player has already agreed to rules
	static bool HasPlayerAgreed(string playerGUID)
	{
		if (!playerGUID || playerGUID.Length() == 0)
		{
			Print("[Server Rules] HasPlayerAgreed: Empty GUID");
			return false;
		}
		
		string filePath = SRV_RulesConfigManager.GetInstance().GetAgreedDir() + playerGUID + ".json";
		bool exists = FileIO.FileExists(filePath);
		Print("[Server Rules] Checking agreement file: " + filePath + " exists: " + exists.ToString());
		return exists;
	}
	
	//------------------------------------------------------------------------------------------------
	// Save player agreement
	static void SaveAgreement(string playerGUID, string playerName)
	{
		Print("[Server Rules] SaveAgreement called - GUID: " + playerGUID + " Name: " + playerName);
		
		if (!playerGUID || playerGUID.Length() == 0)
		{
			Print("[Server Rules] ERROR: Empty GUID, cannot save agreement");
			return;
		}
		
		// Ensure directory exists
		string agreedDir = SRV_RulesConfigManager.GetInstance().GetAgreedDir();
		Print("[Server Rules] Agreed directory: " + agreedDir);
		
		// Ensure config is initialized (creates directories)
		SRV_RulesConfigManager.GetInstance().Initialize();
		
		string filePath = agreedDir + playerGUID + ".json";
		Print("[Server Rules] Saving to file: " + filePath);
		
		string dateStr = GetISO8601Timestamp();

        // Now update this to use SRV_RulesAccepted struct and JsonApiStruct saving
		SRV_RulesAccepted agreementData = new SRV_RulesAccepted(true, dateStr, playerName, playerGUID);
        SCR_JsonSaveContext saveContext = new SCR_JsonSaveContext();
        agreementData.Save(saveContext);
        if (saveContext.SaveToFile(filePath))
        {
            Print("[Server Rules] SUCCESS: Agreement saved to " + filePath);
        }
        else
        {
            Print("[Server Rules] ERROR: Failed to save agreement to file: " + filePath);
        }
	}

    static string GetISO8601Timestamp()
    {
        int y, m, d, hh, mm, ss;
        
        // Get date and time components
        System.GetYearMonthDayUTC(y, m, d);
        System.GetHourMinuteSecondUTC(hh, mm, ss);

        // Format with leading zeros: YYYY-MM-DDTHH:MM:SSZ
        // %02d ensures 2 digits with a leading zero (e.g., "03" instead of "3")
        return string.Format("%1-%2-%3T%4:%5:%6Z", 
            y, 
            m.ToString(2), 
            d.ToString(2), 
            hh.ToString(2), 
            mm.ToString(2), 
            ss.ToString(2)
        );
    }
}
