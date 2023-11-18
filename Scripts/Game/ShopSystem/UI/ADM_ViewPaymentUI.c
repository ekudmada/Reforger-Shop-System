class ADM_ViewPaymentUI: ChimeraMenuBase
{
	protected Widget m_wRoot;
	
	protected ADM_PhysicalShopComponent m_shop;
	protected ADM_PhysicalShopAction m_originalAction;
	
	protected SCR_InputButtonComponent m_okayButton;
	protected SCR_InputButtonComponent m_cancelButton;
	
	protected OverlayWidget m_ListBoxOverlay;
    protected SCR_ListBoxComponent m_ListBoxComponent;
	
	override void OnMenuOpen() 
	{
		super.OnMenuOpen();
		
		m_wRoot = GetRootWidget();
		
        m_okayButton = SCR_InputButtonComponent.GetInputButtonComponent("Confirm", m_wRoot);
        m_cancelButton = SCR_InputButtonComponent.GetInputButtonComponent("Cancel", m_wRoot);
		
		if (m_okayButton)
		{
			m_okayButton.m_OnActivated.Clear();
			m_okayButton.m_OnActivated.Insert(Confirm);
			GetGame().GetWorkspace().SetFocusedWidget(m_okayButton.GetRootWidget());
		}
		
		if (m_cancelButton)
		{
			m_cancelButton.m_OnActivated.Clear();
			m_cancelButton.m_OnActivated.Insert(Close);
			GetGame().GetWorkspace().SetFocusedWidget(m_cancelButton.GetRootWidget());
		}
		
		m_ListBoxOverlay = OverlayWidget.Cast(m_wRoot.FindAnyWidget("ListBox0"));
        m_ListBoxComponent = SCR_ListBoxComponent.Cast(m_ListBoxOverlay.FindHandler(SCR_ListBoxComponent));
	}
	
	void Confirm()
	{
		if (!m_shop || m_shop.GetMerchandise().Count() <= 0 || !m_originalAction) 
			return;
		
		PlayerController playerController = GetGame().GetPlayerController();
		ADM_PlayerShopManagerComponent playerShopManager = ADM_PlayerShopManagerComponent.Cast(playerController.FindComponent(ADM_PlayerShopManagerComponent));
		if (!playerShopManager) return;
		
		// Physical shops should only have one item defined, so just grab the first one in the merchandise array
		playerShopManager.AskPurchase(m_shop, m_shop.GetMerchandise()[0], m_originalAction.GetTargetValue());

		GetGame().GetMenuManager().CloseMenu(this);
	}
	
	void SetShop(ADM_PhysicalShopComponent shop)
	{
		m_shop = shop;
		UpdatePaymentMethods();
	}
	
	void SetAction(ADM_PhysicalShopAction action)
	{
		m_originalAction = action;
	}
	
	void UpdatePaymentMethods()
	{
		if (!m_shop || m_shop.GetMerchandise().Count() <= 0) return;
		
		foreach (ADM_PaymentMethodBase paymentMethod : m_shop.GetMerchandise()[0].GetRequiredPayment())
		{
			//TODO: fix icon
			m_ListBoxComponent.AddItemAndIcon(paymentMethod.GetDisplayString(), "", "");
		}
	}
}