class ADM_ShopComponentClass: ADM_ShopBaseComponentClass {}
class ADM_ShopComponent: ADM_ShopBaseComponent
{
	[Attribute("", UIWidgets.ResourceNamePicker, "Config", "conf", category: "Shop")]
	ref array<ResourceName> m_Categories;
	
	[Attribute(defvalue: "", desc: "Additional merchandise not defined in config", uiwidget: UIWidgets.Object, category: "Shop")]
	protected ref array<ref ADM_ShopMerchandise> m_AdditionalMerchandise;
	
	ref array<ref ADM_ShopCategory> m_LoadedCategories = {};
	
	array<ref ADM_ShopCategory> GetCategories()
	{
		return m_LoadedCategories;
	}
	
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		foreach (ADM_ShopMerchandise merch : m_AdditionalMerchandise) 
		{
			m_Merchandise.Insert(merch);
		}
		
		foreach (ResourceName category : m_Categories) 
		{
			m_LoadedCategories.Insert(ADM_ShopCategory.GetConfig(category));
		}
	}
}