[BaseContainerProps()]
class ADM_ShopConfig
{
	[Attribute(defvalue: "", desc: "Merchandise offered by the shop.", uiwidget: UIWidgets.Object)]
	ref array<ref ADM_ShopMerchandise> m_Merchandise;
	
	static ADM_ShopConfig GetConfig(string resourcePath)
	{
		Resource holder = BaseContainerTools.LoadContainer(resourcePath);
		if (holder)
		{
			ADM_ShopConfig obj = ADM_ShopConfig.Cast(BaseContainerTools.CreateInstanceFromContainer(holder.GetResource().ToBaseContainer()));
			if (obj)
			{
				return obj;
			}
		}
		
		return null;
	}
}