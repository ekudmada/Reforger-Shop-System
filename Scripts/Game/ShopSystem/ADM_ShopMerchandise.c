[BaseContainerProps()]
class ADM_ShopMerchandise: ScriptAndConfig
{
	[Attribute(desc: "Merchandise of shop", uiwidget: UIWidgets.Object)]
	protected ref ADM_MerchandiseType m_Merchandise;
	
	[Attribute(desc: "Payment that a user will give to shop when buying merchandise.")]
	protected ref array<ref ADM_PaymentMethodBase> m_BuyPayment;
	
	[Attribute(desc: "Payment that a user will receive when selling merchandise, if no payment is specified then selling to the shop is disabled for this merchandise.")]
	protected ref array<ref ADM_PaymentMethodBase> m_SellPayment;
	
	ADM_MerchandiseType GetType() 
	{ 
		return m_Merchandise; 
	}
	
	array<ref ADM_PaymentMethodBase> GetBuyPayment() 
	{ 
		return m_BuyPayment; 
	}
	
	array<ref ADM_PaymentMethodBase> GetSellPayment()
	{
		return m_SellPayment;
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