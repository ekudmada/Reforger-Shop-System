class ADM_MerchandiseType: ScriptAndConfig
{
	[Attribute(defvalue: "", desc: "Prefab to sell", uiwidget: UIWidgets.ResourceNamePicker, params: "et")]
	protected ResourceName m_Prefab;
	protected bool m_CanPurchaseMultiple = true;
	
	ResourceName GetPrefab() { return m_Prefab; }
	bool CanPurchaseMultiple() { return m_CanPurchaseMultiple; }
	
	bool CanDeliver(IEntity player, ADM_ShopBaseComponent shop, int quantity = 1) { return false; }
	bool CanRespawn(ADM_ShopBaseComponent shop, int quantity = 1) { return false; }
	bool Deliver(IEntity player, ADM_ShopBaseComponent shop, int quantity = 1) { return false; }
}