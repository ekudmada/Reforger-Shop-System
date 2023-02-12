modded enum ChimeraMenuPreset
{
	ADM_ViewPaymentMenu
}

class ADM_ViewPaymentUI: ChimeraMenuBase
{
	protected Widget m_wRoot;
	
	protected ADM_PhysicalShopComponent m_shop;
	
	protected SCR_NavigationButtonComponent m_okayButton;
	protected SCR_NavigationButtonComponent m_cancelButton;
	
	protected OverlayWidget m_ListBoxOverlay;
    protected SCR_ListBoxComponent m_ListBoxComponent;
	
	override void OnMenuFocusGained() 
	{
		super.OnMenuFocusGained();
	}
	
	override void OnMenuFocusLost() 
	{
		super.OnMenuFocusLost();
	}
	
	override void OnMenuShow() 
	{
		super.OnMenuShow();
	}
	
	override void OnMenuHide() 
	{
		super.OnMenuHide();
	}
	
	override void OnMenuOpen() 
	{
		super.OnMenuOpen();
		
		m_wRoot = GetRootWidget();
		
        m_okayButton = SCR_NavigationButtonComponent.GetNavigationButtonComponent("Confirm", m_wRoot);
        m_cancelButton = SCR_NavigationButtonComponent.GetNavigationButtonComponent("Cancel", m_wRoot);
		
		if (m_okayButton)
		{
			m_okayButton.m_OnClicked.Clear();
			m_okayButton.m_OnClicked.Insert(Confirm);
		}
		
		if (m_cancelButton)
		{
			m_cancelButton.m_OnClicked.Clear();
			m_cancelButton.m_OnClicked.Insert(Cancel);
		}
		
		m_ListBoxOverlay = OverlayWidget.Cast(m_wRoot.FindAnyWidget("ListBox0"));
        m_ListBoxComponent = SCR_ListBoxComponent.Cast(m_ListBoxOverlay.FindHandler(SCR_ListBoxComponent));
	}
	
	void Confirm()
	{
		if (!m_shop || m_shop.GetMerchandise().Count() <= 0) return;
		
		PlayerController playerController = GetGame().GetPlayerController();
		ADM_PlayerShopManagerComponent playerShopManager = ADM_PlayerShopManagerComponent.Cast(playerController.FindComponent(ADM_PlayerShopManagerComponent));
		if (!playerShopManager) return;
		
		// Physical shops should only have one item defined, so just grab the first one in the merchandise array
		playerShopManager.AskPurchase(m_shop, m_shop.GetMerchandise()[0]);

		GetGame().GetMenuManager().CloseMenu(this);
	}
	
	void Cancel()
	{
		GetGame().GetMenuManager().CloseMenu(this);
	}
	
	void SetShop(ADM_PhysicalShopComponent shop)
	{
		m_shop = shop;
		UpdatePaymentMethods();
	}
	
	void UpdatePaymentMethods()
	{
		if (!m_shop || m_shop.GetMerchandise().Count() <= 0) return;
		
		foreach (ADM_PaymentMethodBase paymentMethod : m_shop.GetMerchandise()[0].GetRequiredPaymentToBuy())
		{
			m_ListBoxComponent.AddItemAndIcon(paymentMethod.GetDisplayString(), paymentMethod.GetDisplayIcon(), "");
		}
	}
	
	override void OnMenuOpened() 
	{
		super.OnMenuOpened();
	}
	
	override void OnMenuClose() 
	{
		super.OnMenuClose();
	}
	
	override void OnMenuInit() 
	{
		super.OnMenuInit();
	}
	
	override void OnMenuUpdate(float tDelta) 
	{
		super.OnMenuUpdate(tDelta);
	}
	
	override void OnMenuItem(string menuItemName, bool changed, bool finished) 
	{
		super.OnMenuItem(menuItemName, changed, finished);
	}
}