class ADM_PhysicalShopBase: ScriptAndConfig
{
	[Attribute(defvalue: "", desc: "Prefab of thing to sell", uiwidget: UIWidgets.ResourceNamePicker, params: "et")]
	protected ResourceName m_Prefab;
	
	[Attribute(defvalue: "1", desc: "Number of thing to sell", params: "1 inf")]
	protected int m_Quantity;
	
	ResourceName GetPrefab()
	{
		return m_Prefab;
	}
	
	int GetQuantity()
	{
		return m_Quantity;
	}
	
	bool CanDeliver(IEntity player) { return false; }
	bool Deliver(IEntity player) { return false; }
}

[BaseContainerProps()]
class ADM_ItemShop: ADM_PhysicalShopBase
{
	[Attribute(defvalue: "1", desc: "If this item is equipable, and there is space available to equip it, then the item will auto-equip.", uiwidget: UIWidgets.CheckBox, params: "et", category: "Physical Shop")]
	protected bool m_AutoEquip;
	
	override bool CanDeliver(IEntity player)
	{
		// Check if player can fit the item in their inventory
		
		return false;
	}
	
	override bool Deliver(IEntity player)
	{
		
	}
}