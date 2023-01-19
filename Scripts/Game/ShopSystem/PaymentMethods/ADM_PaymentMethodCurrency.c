[BaseContainerProps()]
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
		if (!inventory) return false;
		
		int totalCurrency = ADM_CurrencyComponent.FindTotalCurrencyInInventory(inventory);
		if (totalCurrency < m_Quantity * quantity) return false;
		
		return true;
	}
	
	override bool CollectPayment(IEntity player, int quantity = 1)
	{
		if (RplSession.Mode() == RplMode.Client) return false;
		
		//check if player has the desired payment
		if (!CheckPayment(player)) return false;
		
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventory) return false;
		
		bool didRemoveCurrency = ADM_CurrencyComponent.RemoveCurrencyFromInventory(inventory, m_Quantity * quantity);
		return didRemoveCurrency;
	}
	
	override bool ReturnPayment(IEntity player, int quantity = 1)
	{
		if (RplSession.Mode() == RplMode.Client) return false;
		
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventory) return false;
		
		bool didAddCurrency = ADM_CurrencyComponent.AddCurrencyToInventory(inventory, m_Quantity * quantity);
		return didAddCurrency;
	}
	
	override string GetDisplayString(int quantity = 1)
	{
		return string.Format("$%1", m_Quantity * quantity);
	}
	
	override ResourceName GetDisplayIcon()
	{
		return "{DC35FA14CD7B6750}Assets/Items/Money/BankNote/data/dinara1.edds";
	}
}