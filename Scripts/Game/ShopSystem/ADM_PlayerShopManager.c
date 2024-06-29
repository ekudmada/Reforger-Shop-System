class ADM_PlayerShopManagerComponentClass: ScriptComponentClass {}

class ADM_PlayerShopManagerComponent: ScriptComponent {
	protected string m_sTransactionMessage;
	
	void SetTransactionMessage(string message)
	{
		m_sTransactionMessage = message;
	}
	
	void AskProcessCart(ADM_ShopBaseComponent shop, map<ADM_ShopMerchandise, int> buyShoppingCart, map<ADM_ShopMerchandise, int> sellShoppingCart)
	{
		// I can foresee issues of weight/volume if a player receives items for a sale. It would be nice to precompute given all desired
		// sales and purchases if it is possible to do in one go. I am too lazy to figure this out, so for now I will leave it as an edge case.
		 
		// First sell everything requested first to clear as much space as possible in inventory
		foreach(ADM_ShopMerchandise merch, int quantity : sellShoppingCart)
		{
			this.AskSell(shop, merch, quantity);
		}
			
		// Second buy everything requested
		foreach(ADM_ShopMerchandise merch, int quantity : buyShoppingCart)
		{
			this.AskPurchase(shop, merch, quantity);
		}
	}
	
	void AskPurchase(ADM_ShopBaseComponent shop, ADM_ShopMerchandise merchandise, int quantity = 1)
	{
		PlayerController player = GetGame().GetPlayerController();
		RplId shopID = Replication.FindId(shop);
		Rpc(Rpc_AskPurchase, player.GetPlayerId(), shopID, shop.GetMerchandiseBuy().Find(merchandise), quantity);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void Rpc_AskPurchase(int playerId, RplId shopID, int merchandiseIndex, int quantity)
	{
		if (merchandiseIndex == -1) 
			return;
		
		ADM_ShopBaseComponent shop = ADM_ShopBaseComponent.Cast(Replication.FindItem(shopID));
		if (!shop || shopID == Replication.INVALID_ID) 
		{
			Print("Error! Couldn't find shop entity!", LogLevel.ERROR); 
			return;
		}
		
		SCR_PlayerController scrPlayerController = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
		if (!scrPlayerController) 
		{
			Print("Error! Could not find player controller.", LogLevel.ERROR);
			return;
		}
		
		IEntity player = scrPlayerController.GetMainEntity();
		if (!player) 
		{
			Print("Error! Could not find player entity.", LogLevel.ERROR);
			return;
		}
		
		ADM_ShopMerchandise merchandise = shop.GetMerchandiseBuy()[merchandiseIndex];
		if (!merchandise)
		{
			Print("Error! Could not find merchandise.", LogLevel.ERROR);
			return;
		}
		
		bool success = shop.AskPurchase(player, this, merchandise, quantity);
		Rpc(RpcDo_Transaction, m_sTransactionMessage);
	}
	
	void AskSell(ADM_ShopBaseComponent shop, ADM_ShopMerchandise merchandise, int quantity = 1)
	{
		PlayerController player = GetGame().GetPlayerController();
		RplId shopID = Replication.FindId(shop);
		
		Print(player,shopID);
		Rpc(Rpc_AskSell, player.GetPlayerId(), shopID, shop.GetMerchandiseSell().Find(merchandise), quantity);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void Rpc_AskSell(int playerId, RplId shopID, int merchandiseIndex, int quantity)
	{
		if (merchandiseIndex == -1) 
			return;
		
		ADM_ShopBaseComponent shop = ADM_ShopBaseComponent.Cast(Replication.FindItem(shopID));
		if (!shop || shopID == Replication.INVALID_ID) 
		{
			Print("Error! Couldn't find shop entity!", LogLevel.ERROR); 
			return;
		}
		
		SCR_PlayerController scrPlayerController = SCR_PlayerController.Cast(GetGame().GetPlayerManager().GetPlayerController(playerId));
		if (!scrPlayerController) 
		{
			Print("Error! Could not find player controller.", LogLevel.ERROR);
			return;
		}
		
		IEntity player = scrPlayerController.GetMainEntity();
		if (!player) 
		{
			Print("Error! Could not find player entity.", LogLevel.ERROR);
			return;
		}
				
		ADM_ShopMerchandise merchandise = shop.GetMerchandiseSell()[merchandiseIndex];
		if (!merchandise)
		{
			Print("Error! Could not find merchandise.", LogLevel.ERROR);
			return;
		}
		
		bool success = shop.AskSell(player, this, merchandise, quantity);
		
		Rpc(RpcDo_Transaction, m_sTransactionMessage);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_Transaction(string message)
	{
		SCR_HintManagerComponent.ShowCustomHint(message);
	}
}