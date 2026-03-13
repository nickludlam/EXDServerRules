class SRV_RulesConfig : JsonApiStruct
{
	bool enabled;
    bool display_always;
    string title;
    ref array<string> rules = {};
    bool use_large_dialog;
    bool enable_rules_reload_command;

    bool IsEnabled() { return enabled; }
    bool IsDisplayAlways() { return display_always; }
    string GetTitle() { return title; }
    array<string> GetRules() { return rules; }
    bool UseLargeDialog() { return use_large_dialog; }
    bool IsRulesReloadCommandEnabled() { return enable_rules_reload_command; }

    // This method handles the mapping
    bool RegV(SCR_JsonLoadContext ctx)
    {
        return ctx.ReadValue("enabled", enabled) &&
               ctx.ReadValue("display_always", display_always) &&
               ctx.ReadValue("title", title) &&
               ctx.ReadValue("rules", rules) &&
               ctx.ReadValue("use_large_dialog", use_large_dialog) &&
               ctx.ReadValue("enable_rules_reload_command", enable_rules_reload_command);
    }

    bool Save(SCR_JsonSaveContext ctx)
    {
        return ctx.WriteValue("enabled", enabled) &&
               ctx.WriteValue("display_always", display_always) &&
               ctx.WriteValue("title", title) &&
               ctx.WriteValue("rules", rules) &&
               ctx.WriteValue("use_large_dialog", use_large_dialog) &&
               ctx.WriteValue("enable_rules_reload_command", enable_rules_reload_command);
    }

    bool Validate(SCR_JsonLoadContext ctx, out array<string> errors)
    {
        // We want to go through each property and validate its presence
        if (!ctx.DoesKeyExist("enabled"))
            errors.Insert("Missing required property: enabled");
        if (!ctx.DoesKeyExist("display_always"))
            errors.Insert("Missing required property: display_always");
        if (!ctx.DoesKeyExist("title"))
            errors.Insert("Missing required property: title");
        if (!ctx.DoesKeyExist("rules"))
            errors.Insert("Missing required property: rules");
        if (!ctx.DoesKeyExist("use_large_dialog"))
            errors.Insert("Missing required property: use_large_dialog");
        if (!ctx.DoesKeyExist("enable_rules_reload_command"))
            errors.Insert("Missing required property: enable_rules_reload_command");

        return errors.Count() == 0;
    }

    void SetFirstRun()
    {
        enabled = true;
        display_always = false;
        title = "Example Server Rules";
        rules.Clear();
        rules.Insert("Example Rule #1");
        rules.Insert("<color rgba='255,100,100,255'>Example Rule #2 (colored)</color>");
        rules.Insert("<b>Example Rule #3 (bold)</b>");
        rules.Insert("This will look like a bulleted list:<br/> - Bullet point 1<br/> - Bullet point 2<br/> - Bullet point 3");
        rules.Insert("Edit rules in your server profile folder/server_rules/server_rules.json");
        rules.Insert("To disable showing the rules, set enabled to false in server_rules.json.");
        rules.Insert("To show rules always when player connects, set display_always to true.");
        rules.Insert("To view rules ingame, type in chat command !rules");
        rules.Insert("To reload rules from the server for development, type in chat command !rulesreload (must be enabled in config)");
        rules.Insert("To use a larger dialog for rules, set use_large_dialog to true in config");
        rules.Insert("Mod by Bewilderbeest, build on the original 'Simple Server Rules' from Wil3son");
        enable_rules_reload_command = true;
        use_large_dialog = true;
    }
}
