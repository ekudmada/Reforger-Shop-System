class ADM_PlayerShopManagerComponentClass: ScriptComponentClass {}

class ADM_PlayerShopManagerComponent: ScriptComponent {
	protected string m_sPurchaseMessage = string.Empty;
	
	void SetPurchaseMessage(string message)
	{
		m_sPurchaseMessage = message;
	}
	
	void AskPurchase(ADM_ShopBaseComponent shop, ADM_ShopMerchandise merchandise, int quantity = 1)
	{
		PlayerController player = GetGame().GetPlayerController();
		RplId shopID = Replication.FindId(shop);
		Rpc(Rpc_AskPurchase, player.GetPlayerId(), shopID, shop.GetMerchandise().Find(merchandise), quantity);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void Rpc_AskPurchase(int playerId, RplId shopID, int merchandiseIndex, int quantity)
	{
		if (merchandiseIndex == -1) return;
		
		ADM_ShopBaseComponent shop = ADM_ShopBaseComponent.Cast(Replication.FindItem(shopID));
		if (!shop || shopID == Replication.INVALID_ID) {
			Print("Error! couldn't find shop entity!", LogLevel.ERROR); 
			return;
		}
		
		IEntity player = GetGame().GetPlayerManager().GetPlayerController(playerId).GetControlledEntity();
		if (!player) return;
		
		ADM_ShopMerchandise merchandise = shop.GetMerchandise()[merchandiseIndex];
		bool success = shop.AskPurchase(player, shop, merchandise, quantity, this);
		
		Rpc(RpcDo_Transaction, m_sPurchaseMessage);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_Transaction(string message)
	{
		SCR_HintManagerComponent.GetInstance().ShowCustom(message);
	}
	
	void ADM_PlayerShopManagerComponent() 
	{
		
	}
}