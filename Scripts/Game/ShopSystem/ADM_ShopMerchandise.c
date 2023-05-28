[BaseContainerProps()]
class ADM_ShopMerchandise: ScriptAndConfig
{
	[Attribute(defvalue: "", desc: "Merchandise to sell", uiwidget: UIWidgets.Object, params: "et", category: "Physical Shop")]
	protected ref ADM_MerchandiseType m_Merchandise;
	
	[Attribute(category: "Physical Shop")]
	protected ref array<ref ADM_PaymentMethodBase> m_RequiredPayment;
	
	ADM_MerchandiseType GetMerchandise() 
	{ 
		return m_Merchandise; 
	}
	
	array<ref ADM_PaymentMethodBase> GetRequiredPaymentToBuy() 
	{ 
		return m_RequiredPayment; 
	}
	
	static ADM_ShopMerchandise GetConfig(string resourcePath)
	{
		Resource holder = BaseContainerTools.LoadContainer(resourcePath);
		if (holder)
		{
			ADM_ShopMerchandise obj = ADM_ShopMerchandise.Cast(BaseContainerTools.CreateInstanceFromContainer(holder.GetResource().ToBaseContainer()));
			if (obj)
			{
				return obj;
			}
		}
		
		return null;
	}
}