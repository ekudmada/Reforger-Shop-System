class ADM_PaymentMethodBase: ScriptAndConfig
{
	bool CheckPayment(IEntity player) { return false; };
	bool CollectPayment(IEntity player) { return false; };
}

[BaseContainerProps()]
class ADM_PaymentMethodItem: ADM_PaymentMethodBase
{
	[Attribute(defvalue: "", desc: "Prefab of item", uiwidget: UIWidgets.ResourceNamePicker, params: "et")]
	protected ResourceName m_ItemPrefab;
	
	[Attribute(defvalue: "1", desc: "Number of item to sell", params: "1 inf")]
	protected int m_ItemQuantity;
	
	ResourceName GetItemPrefab()
	{
		return m_ItemPrefab;
	}
	
	int GetItemQuantity()
	{
		return m_ItemQuantity;
	}
	
	override bool CheckPayment(IEntity player)
	{
		//Print("check payment for item payment");
		return false;
	}
	
	override bool CollectPayment(IEntity player)
	{
		//Print("collect payment ItemPayment");
		return false;
	}
}

[BaseContainerProps()]
class ADM_PaymentMethodCurrency: ADM_PaymentMethodBase
{
	[Attribute()]
	int m_Quantity;
	
	int GetQuantity()
	{
		return m_Quantity;
	}
	
	override bool CheckPayment(IEntity player)
	{
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventory)
			return false;
		
		int totalCurrency = ADM_CurrencyComponent.FindTotalCurrencyInInventory(inventory);
		if (totalCurrency < m_Quantity) return false;
		
		return true;
	}
	
	override bool CollectPayment(IEntity player)
	{
		//check if player has the desired payment
		if (!CheckPayment(player)) return false;
		
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventory)
			return false;
		
		bool didRemoveCurrency = ADM_CurrencyComponent.RemoveCurrencyFromInventory(inventory, m_Quantity);
		return didRemoveCurrency;
	}
}