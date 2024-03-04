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
		if (!Replication.IsServer() || !CheckPayment(player)) 
			return false;
		
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventory) return false;
		
		bool didRemoveCurrency = ADM_CurrencyComponent.RemoveCurrencyFromInventory(inventory, m_Quantity * quantity);
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
		return string.Format("$%1", Math.AbsInt(m_Quantity * quantity));
	}
	
	override ResourceName GetDisplayEntity()
	{
		return "{B0E67230AEEE2DF3}Prefabs/Items/Wallet.et";
	}
	
	override bool Equals(ADM_PaymentMethodBase other)
	{
		ADM_PaymentMethodCurrency otherCurrency = ADM_PaymentMethodCurrency.Cast(other);
		if (!otherCurrency)
			return false;
		
		return true;
	}
	
	override bool Add(ADM_PaymentMethodBase other, int quantity = 1)
	{
		if (!this.Equals(other))
			return false;
		
		ADM_PaymentMethodCurrency otherCurrency = ADM_PaymentMethodCurrency.Cast(other);
		if (!otherCurrency)
			return false;
				
		this.m_Quantity += otherCurrency.m_Quantity * quantity;
		
		return true;
	}
}