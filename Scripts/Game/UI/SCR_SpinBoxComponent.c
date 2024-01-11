modded class SCR_SpinBoxComponent
{
	[Attribute(defvalue: "MenuLeft")]
	protected string m_sMenuActionLeft;
	
	[Attribute(defvalue: "MenuRight")]
	protected string m_sMenuActionRight;
	
	//------------------------------------------------------------------------------------------------
	override void AddActionListeners()
	{
		if (m_bHasActionListeners)
			return;
		
		GetGame().GetInputManager().AddActionListener(m_sMenuActionLeft, EActionTrigger.DOWN, OnMenuLeft);
		GetGame().GetInputManager().AddActionListener(m_sMenuActionRight, EActionTrigger.DOWN, OnMenuRight);
		
		m_bHasActionListeners = true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void RemoveActionListeners()
	{
		if (!m_bHasActionListeners)
			return;
		
		GetGame().GetInputManager().RemoveActionListener(m_sMenuActionLeft, EActionTrigger.DOWN, OnMenuLeft);
		GetGame().GetInputManager().RemoveActionListener(m_sMenuActionRight, EActionTrigger.DOWN, OnMenuRight);
		
		m_bHasActionListeners = false;
	}
}