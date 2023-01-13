class ADM_PhysicalShopBase: ScriptAndConfig
{
	[Attribute(defvalue: "", desc: "Prefab of thing to sell", uiwidget: UIWidgets.ResourceNamePicker, params: "et")]
	protected ResourceName m_Prefab;
	
	ResourceName GetPrefab()
	{
		return m_Prefab;
	}
	
	bool CanDeliver(IEntity player, ADM_PhysicalShopComponent shop) { return false; }
	bool Deliver(IEntity player, ADM_PhysicalShopComponent shop) { return false; }
	bool CanRespawn(ADM_PhysicalShopComponent shop) { return false; }
}