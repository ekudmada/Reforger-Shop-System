class ADM_MerchandiseType: ScriptAndConfig
{
	[Attribute(defvalue: "", desc: "Prefab to sell", uiwidget: UIWidgets.ResourceNamePicker, params: "et")]
	protected ResourceName m_sPrefab;
	
	[Attribute(defvalue: "-1", params: "0 inf")]
	protected int m_iMaxQuantity;
	
	ResourceName GetPrefab() 
	{ 
		return m_sPrefab; 
	}
	
	int GetMaxQuantity() 
	{ 		
		return m_iMaxQuantity; 
	}
	
	void SetMaxQuantity(int quantity)
	{
		m_iMaxQuantity = quantity;
	}
	
	bool CanDeliver(IEntity player, ADM_ShopBaseComponent shop, int quantity = 1, array<IEntity> ignoreEntities = null) 
	{ 
		return false; 
	}
	
	bool CanRespawn(ADM_ShopBaseComponent shop, int quantity = 1, array<IEntity> ignoreEntities = null) 
	{ 
		return false; 
	}
	
	bool Deliver(IEntity player, ADM_ShopBaseComponent shop, int quantity = 1) 
	{
		 return false; 
	}
}