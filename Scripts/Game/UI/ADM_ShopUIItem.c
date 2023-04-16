class ADM_ShopUIItem : SCR_ScriptedWidgetComponent
{
	[Attribute(defvalue: "1")]
	protected int m_quantity = 1;

	protected ADM_ShopComponent m_Shop;
	protected ref ADM_ShopMerchandise m_Merchandise;
	
	int GetQuantity()
	{
		return m_quantity;
	}
	
	void SetQuantity(int quantity)
	{
		m_quantity = quantity;
		OnUpdate(this.m_wRoot);
	}
	
	void UpdateQuantity(int amount)
	{
		m_quantity = m_quantity + amount;
		OnUpdate(this.m_wRoot);
	}
	
	ADM_ShopMerchandise GetMerchandise()
	{
		return m_Merchandise;
	}
	
	void SetMerchandise(ADM_ShopMerchandise merch)
	{
		m_Merchandise = merch;
	}
	
	override bool OnUpdate(Widget w)
	{
		if (m_quantity < 1) m_quantity = 1;
		
		TextWidget quantity = TextWidget.Cast(w.FindWidget("Row.QuantityContainer.HorizontalLayout0.Quantity"));
		if (quantity) quantity.SetTextFormat("%1", m_quantity);
	
		return true;
	}
}