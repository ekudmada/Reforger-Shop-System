class ADM_MerchandisePrefab: ADM_MerchandiseType
{
	[Attribute(defvalue: "", desc: "Prefab to sell", uiwidget: UIWidgets.ResourceNamePicker, params: "et")]
	protected ResourceName m_sPrefab;
	
	/*
		Prefab of merchandise
	*/
	ResourceName GetPrefab() { return m_sPrefab; }
	
	/*
		How to display this payment method in menus
	*/
	override string GetDisplayName(int quantity = 1) { return ADM_Utils.GetPrefabDisplayName(m_sPrefab); }
	
	/*
		How to display this payment method in menus
	*/
	override ResourceName GetDisplayEntity() { return m_sPrefab; }
}