class SRV_RulesAccepted : JsonApiStruct
{
	bool rules_agreed;
    string date;
    string username;
    string game_identity_id;

    void SRV_RulesAccepted(bool agreed = false, string dateStr = "", string usernameStr = "", string gameId = "")
    {
        rules_agreed = agreed;
        date = dateStr;
        username = usernameStr;
        game_identity_id = gameId;
    }

    bool GetRulesAgreed() { return rules_agreed; }
    string GetDate() { return date; }
    string GetUsername() { return username; }
    string GetGameIdentityId() { return game_identity_id; }

    // This method handles the mapping
    bool RegV(SCR_JsonLoadContext ctx)
    {
        return ctx.ReadValue("rules_agreed", rules_agreed) &&
               ctx.ReadValue("date", date) &&
               ctx.ReadValue("username", username) &&
               ctx.ReadValue("game_identity_id", game_identity_id);
    }

    bool Save(SCR_JsonSaveContext ctx)
    {
        return ctx.WriteValue("rules_agreed", rules_agreed) &&
               ctx.WriteValue("date", date) &&
               ctx.WriteValue("username", username) &&
               ctx.WriteValue("game_identity_id", game_identity_id);
    }
}
