class ADM_ShopBase: ScriptAndConfig
{
	[Attribute(defvalue: "", desc: "Prefab to sell", uiwidget: UIWidgets.ResourceNamePicker, params: "et")]
	protected ResourceName m_Prefab;
	
	ResourceName GetPrefab() { return m_Prefab; }
	bool CanDeliver(IEntity player, ADM_PhysicalShopComponent shop) { return false; }
	bool Deliver(IEntity player, ADM_PhysicalShopComponent shop) { return false; }
	bool CanRespawn(ADM_PhysicalShopComponent shop) { return false; }
	
	static string GetPrefabDisplayName(ResourceName resourcePath)
	{
		if (resourcePath == ResourceName.Empty)
			return string.Empty;

		Resource resource = Resource.Load(resourcePath);
		if (!resource)
			return string.Empty;

		BaseResourceObject prefabBase = resource.GetResource();
		if (!prefabBase)
			return string.Empty;
		
		BaseContainer prefabSrc = prefabBase.ToBaseContainer();
		if (!prefabSrc)
			return string.Empty;

		BaseContainerList components = prefabSrc.GetObjectArray("components");
		if (!components)
			return string.Empty;
		
		array<string> acceptableComponents = {"InventoryItemComponent", "WeaponComponent", "SCR_EditableVehicleComponent"};
		
		// Prefer InventoryItemComponent over any other (for grenades its more descriptive, maybe its only for grenades)
		BaseContainer itemComponent = null;
		for (int c = components.Count() - 1; c >= 0; c--)
		{
			itemComponent = components.Get(c);
			if (acceptableComponents.Contains(itemComponent.GetClassName())) break;
			
			itemComponent = null;
		}

		if (!itemComponent) return string.Empty;
		
		//use this to find variables of a component
		/*int numVars = itemComponent.GetNumVars();
		for (int i = 0; i < numVars; i++)
		{
			Print(itemComponent.GetVarName(i));
		}*/

		string uiInfoVar, displayName = string.Empty;
		switch (itemComponent.GetClassName())
		{
			case "InventoryItemComponent":
			{
				UIInfo uiInfo;
				itemComponent.Get("UIInfo", uiInfo);
				if (uiInfo) displayName = uiInfo.GetName();	
				
				break;
			}
			case "WeaponComponent":
			{
				UIInfo uiInfo;
				itemComponent.Get("UIInfo", uiInfo);
				if (uiInfo) displayName = uiInfo.GetName();	
				
				break;
			}
			case "SCR_EditableVehicleComponent":
			{
				SCR_EditableVehicleUIInfo uiInfo;
				itemComponent.Get("m_UIInfo", uiInfo);
				
				if (uiInfo) displayName = uiInfo.GetName();
				
				break;
			}
		}
		
		return displayName;
	}
}