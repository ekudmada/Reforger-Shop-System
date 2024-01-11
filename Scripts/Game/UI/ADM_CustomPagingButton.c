class ADM_VisualPagingButton: SCR_PagingButtonComponent
{
	override void SetAction(string name)
	{
		if (!m_wText)
			return;

		//GetGame().GetInputManager().RemoveActionListener(m_sActionName, EActionTrigger.DOWN, OnMenuSelect);
		m_sActionName = name;

		// Handle case of empty action
		name = name.Trim();
		if (name == string.Empty)
		{
			if (m_wText)
				m_wText.SetText(" ");
		}
		else
		{
			//GetGame().GetInputManager().AddActionListener(m_sActionName, EActionTrigger.DOWN, OnMenuSelect);
			m_wText.SetTextFormat("<action name='%1' scale='1.5'/>", m_sActionName);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override bool OnMouseEnter(Widget w, int x, int y)
	{
		super.OnMouseEnter(w, x, y);

		if (!m_bUseColorization)
			return false;

		PlaySound(m_sSoundHovered);

		if (m_bUseActionHint)
			AnimateWidget.Color(m_wText, m_BackgroundHovered, m_fAnimationRate);
		else
			AnimateWidget.Color(m_wBackgroundImage, m_BackgroundHovered, m_fAnimationRate);

		return false;
	}

	//------------------------------------------------------------------------------------------------
	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		super.OnMouseLeave(w, enterW, x, y);

		if (!m_bUseColorization)
			return false;

		if (m_bUseActionHint)
			AnimateWidget.Color(m_wText, m_BackgroundDefault, m_fAnimationRate);
		else
			AnimateWidget.Color(m_wBackgroundImage, m_BackgroundDefault, m_fAnimationRate);

		return false;
	}
}