class ADM_ShopBase: ScriptAndConfig
{
	[Attribute(defvalue: "", desc: "Prefab to sell", uiwidget: UIWidgets.ResourceNamePicker, params: "et")]
	protected ResourceName m_Prefab;
	protected ref Resource m_PrefabResource;
	
	ResourceName GetPrefab() { return m_Prefab; }
	Resource GetPrefabResource() { return m_PrefabResource; }
	void SetPrefabResource(Resource res) { m_PrefabResource = res; }
	
	bool CanDeliver(IEntity player, ADM_PhysicalShopComponent shop) { return false; }
	bool CanRespawn(ADM_PhysicalShopComponent shop) { return false; }
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	bool Deliver(IEntity player, ADM_PhysicalShopComponent shop) { return false; }
}