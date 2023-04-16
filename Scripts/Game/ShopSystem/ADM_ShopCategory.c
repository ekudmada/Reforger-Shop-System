class ADM_ShopCategory: ScriptAndConfig {
	[Attribute()]
	string m_DisplayName;
	
	[Attribute(defvalue: "", desc: "Prefabs in category", uiwidget: UIWidgets.Object)]
	ref array<ResourceName> m_Items;
	
	array<ResourceName> GetItems()
	{
		return m_Items;
	}
	
	static ADM_ShopCategory GetConfig(string resourcePath)
	{
		Resource holder = BaseContainerTools.LoadContainer(resourcePath);
		if (holder)
		{
			ADM_ShopCategory obj = ADM_ShopCategory.Cast(BaseContainerTools.CreateInstanceFromContainer(holder.GetResource().ToBaseContainer()));
			if (obj)
			{
				return obj;
			}
		}
		
		return null;
	}
}