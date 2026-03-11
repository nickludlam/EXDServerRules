modded class SCR_PlayerController
{
    void SRV_RequestReloadRulesFromClient()
    {
        //Print("[Server Rules] Sending reload request RPC to server");
        Rpc(SRV_RpcAsk_ReloadRules);
    }

    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void SRV_RpcAsk_ReloadRules()
    {
        //Print("[Server Rules] SRV_RpcAsk_ReloadRules entered. IsServer=" + Replication.IsServer().ToString() + " IsConsole=" + System.IsConsoleApp().ToString());

        if (!Replication.IsServer() || !System.IsConsoleApp())
            return;

        int senderId = GetPlayerId();
        //Print("[Server Rules] Server received reload RPC from player: " + senderId.ToString());

        SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
        if (!gameMode)
        {
            //Print("[Server Rules] Reload failed - game mode missing", LogLevel.ERROR);
            return;
        }

        gameMode.SRV_RequestRulesReloadFromClient(senderId);
    }
}