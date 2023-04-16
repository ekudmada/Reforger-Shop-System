class ADM_ShopUICategory : SCR_ScriptedWidgetComponent
{
	[Attribute(defvalue: "0")]
	protected int m_index = 0;

	int GetIndex()
	{
		return m_index;
	}
	
	void SetIndex(int index)
	{
		m_index = index;
	}
}