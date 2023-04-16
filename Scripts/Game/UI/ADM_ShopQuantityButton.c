class ADM_ShopQuantityButton : SCR_ButtonComponent
{
	[Attribute()]
	protected int m_iAmount;
	
	int GetAmount()
	{
		return m_iAmount;
	}
}