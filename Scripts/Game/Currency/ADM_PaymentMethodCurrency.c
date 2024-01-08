class ADM_PaymentMethodCurrency: ADM_PaymentMethodBase
{
	[Attribute()]
	int m_Quantity;
	
	int GetQuantity()
	{
		return m_Quantity;
	}
	
	override bool CheckPayment(IEntity player, int quantity = 1)
	{
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventory) 
			return false;
		
		int totalCurrency = ADM_CurrencyComponent.FindTotalCurrencyInInventory(inventory);
		if (totalCurrency < m_Quantity * quantity) 
			return false;
		
		return true;
	}
	
	override bool CollectPayment(IEntity player, int quantity = 1)
	{
		Print("collect payment");
		if (!Replication.IsServer() || !CheckPayment(player)) 
			return false;
		
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventory) return false;
		
		Print("found inventory");
		bool didRemoveCurrency = ADM_CurrencyComponent.RemoveCurrencyFromInventory(inventory, m_Quantity * quantity);
		Print(didRemoveCurrency);
		return didRemoveCurrency;
	}
	
	override bool DistributePayment(IEntity player, int quantity = 1)
	{
		if (!Replication.IsServer()) 
			return false;
		
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventory) 
			return false;
		
		bool didAddCurrency = ADM_CurrencyComponent.AddCurrencyToInventory(inventory, m_Quantity * quantity);
		return didAddCurrency;
	}
	
	override string GetDisplayString(int quantity = 1)
	{
		return string.Format("$%1", m_Quantity * quantity);
	}
	
	override ResourceName GetDisplayEntity()
	{
		return "{51D31AB07AE3C1C7}Prefabs/Props/Commercial/CashierShop_01.et";
	}
}