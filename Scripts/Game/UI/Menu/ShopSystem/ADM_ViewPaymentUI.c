class ADM_ViewPaymentUI: ChimeraMenuBase
{	
	static const ResourceName m_BarterIconPrefab = "{BBEF0EBB9F35B19F}UI/Layouts/Menus/BasicShopMenu/MarketBarterItemIcon.layout";
	
	protected ADM_PhysicalShopComponent m_Shop;
	protected ADM_PhysicalShopAction m_OriginalAction;
	
	protected Widget m_wRoot;
	protected bool m_bFocused;
	protected TextWidget m_wHeaderText;
	protected ItemPreviewManagerEntity m_ItemPreviewManager;
	
	protected Widget m_wTabWidget;
	protected SCR_TabViewComponent m_wTabComponent;
	protected SCR_TabViewContent m_wTabView;
	
	void SetShop(ADM_PhysicalShopComponent shop, ADM_PhysicalShopAction action)
	{
		m_Shop = shop;
		m_OriginalAction = action;
		
		if (m_Shop && m_OriginalAction && m_wHeaderText && m_Shop.GetMerchandiseBuy().Count() > 0)
		{
			ADM_ShopMerchandise merch = m_Shop.GetMerchandiseBuy()[0];
			string shopName = string.Empty;
			if (m_Shop.GetName() != string.Empty)
			{
				shopName = string.Format("[%1]", m_Shop.GetName());
			}
			
			m_wHeaderText.SetTextFormat("%1%2 x%3", shopName, merch.GetType().GetDisplayName(), m_OriginalAction.GetTargetValue());
		}
		
		PopulateTab();
	}
	
	void Purchase()
	{
		if (!m_Shop || m_Shop.GetMerchandiseBuy().Count() <= 0 || !m_OriginalAction) 
			return;
		
		PlayerController playerController = GetGame().GetPlayerController();
		ADM_PlayerShopManagerComponent playerShopManager = ADM_PlayerShopManagerComponent.Cast(playerController.FindComponent(ADM_PlayerShopManagerComponent));
		if (!playerShopManager) return;
		
		// Physical shops should only have one item defined, so just grab the first one in the merchandise array
		playerShopManager.AskPurchase(m_Shop, m_Shop.GetMerchandiseBuy()[0], m_OriginalAction.GetTargetValue());

		GetGame().GetMenuManager().CloseMenu(this);
	}
	
	protected void CreateCartListboxItem(SCR_ListBoxComponent listbox, ADM_PaymentMethodBase payment, int quantity = 1)
	{
		string itemName = payment.GetDisplayName(quantity);
		if (!itemName)
		{
			itemName = "Payment Method";
		}
				
		int itemIdx = listbox.AddItem(itemName);
		SCR_ListBoxElementComponent lbItem = listbox.GetElementComponent(itemIdx);
		if (!lbItem)
		{
			return;
		}		
	
		ItemPreviewWidget wItemPreview = ItemPreviewWidget.Cast(lbItem.GetRootWidget().FindAnyWidget("ItemPreview0"));
		if (wItemPreview)
		{
			m_ItemPreviewManager.SetPreviewItemFromPrefab(wItemPreview, payment.GetDisplayEntity(), null, false);
		}
	}
	
	protected void ClearTab(SCR_TabViewContent wTabView)
	{
		if (!wTabView)
			return;
		
		Widget wListbox = wTabView.m_wTab.FindAnyWidget("ListBox0");
		if (wListbox)
		{
			SCR_ListBoxComponent listbox = SCR_ListBoxComponent.Cast(wListbox.FindHandler(SCR_ListBoxComponent));
			if (listbox)
				listbox.Clear();
		}
		
		Widget wListbox1 = wTabView.m_wTab.FindAnyWidget("ListBox1");
		if (wListbox1)
		{
			SCR_ListBoxComponent listbox1 = SCR_ListBoxComponent.Cast(wListbox1.FindHandler(SCR_ListBoxComponent));
			if (listbox1)
				listbox1.Clear();
		}
	}
	
	protected void PopulateTab()
	{
		if (!m_Shop || !m_wTabView || !m_wTabView.m_wTab) return;
		
		Widget wListbox = m_wTabView.m_wTab.FindAnyWidget("ListBox0");
		if (!wListbox)
			return;
		
		SCR_ListBoxComponent listbox = SCR_ListBoxComponent.Cast(wListbox.FindHandler(SCR_ListBoxComponent));
		if (!listbox)
			return;
		
		ClearTab(m_wTabView);
		
		int requestedQuantity = m_OriginalAction.GetTargetValue();
		array<ref ADM_PaymentMethodBase> totalCost = m_Shop.GetMerchandiseBuy()[0].GetBuyPayment();
		foreach (ADM_PaymentMethodBase payment : totalCost)
		{
			CreateCartListboxItem(listbox, payment, requestedQuantity);
		}
		
		Widget totalCostOverlay = m_wTabView.m_wTab.FindAnyWidget("TotalCostOverlay");
		Widget barterParent = totalCostOverlay.FindAnyWidget("BarterItemContainer");
		TextWidget wItemPrice = TextWidget.Cast(totalCostOverlay.FindAnyWidget("TotalCostCurrencyText"));
		if ((totalCost.Count() == 1 && totalCost[0].Type().IsInherited(ADM_PaymentMethodCurrency)) || totalCost.Count() == 0)
		{
			if (barterParent)
			{
				barterParent.GetParent().SetVisible(false);
			}
			
			if (wItemPrice) {
				wItemPrice.SetColor(Color.White);
				wItemPrice.SetVisible(true);
				if (totalCost.Count() == 0)
				{
					wItemPrice.SetTextFormat("Free");
				} else {
					ADM_PaymentMethodCurrency paymentMethod = ADM_PaymentMethodCurrency.Cast(totalCost[0]);
					int quantity = paymentMethod.GetQuantity();
					if (Math.AbsInt(quantity) > 0)
					{
						wItemPrice.SetTextFormat("$%1", Math.AbsInt(quantity) * requestedQuantity);
						
						if (quantity < 0)
						{
							wItemPrice.SetColor(Color.Green);
						}
					} else {
						wItemPrice.SetTextFormat("Free");
					}
				}
			}
		} else {
			if (wItemPrice)
			{ 
				wItemPrice.SetVisible(false);
			}
			
			if (barterParent)
			{
				barterParent.GetParent().SetVisible(true);
				SCR_WidgetHelper.RemoveAllChildren(barterParent);
			}
			
			for (int i = 0; i < totalCost.Count(); i++)
			{
				Widget icon = GetGame().GetWorkspace().CreateWidgets(m_BarterIconPrefab, barterParent);
				ADM_IconBarterTooltip barterItemIcon = ADM_IconBarterTooltip.Cast(icon.FindHandler(ADM_IconBarterTooltip));
				if (barterItemIcon)
				{
					barterItemIcon.SetPayment(totalCost[i], Color.White, requestedQuantity);
				}
			}
		}
		
		TextWidget emptyText = TextWidget.Cast(m_wTabView.m_wTab.FindAnyWidget("Empty Text"));
		if (totalCost.Count() == 0)
		{
			emptyText.SetVisible(true);
			emptyText.SetText("No payment required!");
			emptyText.Update();
		} else {
			emptyText.SetVisible(false);
			emptyText.Update();
		}
	}
	
	override void OnMenuFocusLost()
	{
		m_bFocused = false;
		
		InputManager inputManager = GetGame().GetInputManager();
		if (inputManager)
		{
			inputManager.RemoveActionListener("MenuOpen", EActionTrigger.DOWN, Close);
			inputManager.RemoveActionListener("MenuBack", EActionTrigger.DOWN, Close);
			#ifdef WORKBENCH
				inputManager.RemoveActionListener("MenuOpenWB", EActionTrigger.DOWN, Close);
				inputManager.RemoveActionListener("MenuBackWB", EActionTrigger.DOWN, Close);
			#endif
		}
	}

	override void OnMenuFocusGained()
	{
		m_bFocused = true;
		
		InputManager inputManager = GetGame().GetInputManager();
		if (inputManager)
		{
			inputManager.AddActionListener("MenuOpen", EActionTrigger.DOWN, Close);
			inputManager.AddActionListener("MenuBack", EActionTrigger.DOWN, Close);
			#ifdef WORKBENCH
				inputManager.AddActionListener("MenuOpenWB", EActionTrigger.DOWN, Close);
				inputManager.AddActionListener("MenuBackWB", EActionTrigger.DOWN, Close);
			#endif
		}
	}
	
	override void HandlerAttached(Widget w)
	{
		super.HandlerAttached(w);
		m_wRoot = w;
		
		m_wTabWidget = GetRootWidget().FindAnyWidget("TabView");
		if (!m_wTabWidget) 
			return;
		
		m_wTabComponent = SCR_TabViewComponent.Cast(m_wTabWidget.FindHandler(SCR_TabViewComponent));
		if (!m_wTabComponent) 
			return;
		
		if (m_wTabComponent.GetTabCount() == 1)
		{
			m_wTabView = m_wTabComponent.GetEntryContent(0);
		}	
		
		m_wHeaderText = TextWidget.Cast(m_wRoot.FindAnyWidget("HeaderText"));
		
		Widget closeWidget = ButtonWidget.Cast(m_wRoot.FindAnyWidget("Back"));
		if (closeWidget)
		{
			SCR_InputButtonComponent closeButton = SCR_InputButtonComponent.Cast(closeWidget.FindHandler(SCR_InputButtonComponent));
			if (closeButton)
			{
				closeButton.m_OnActivated.Insert(Close);
			}
		}
		
		Widget purchaseWidget = ButtonWidget.Cast(m_wRoot.FindAnyWidget("Purchase"));
		if (purchaseWidget)
		{
			SCR_InputButtonComponent purchaseButton = SCR_InputButtonComponent.Cast(purchaseWidget.FindHandler(SCR_InputButtonComponent));
			if (purchaseButton)
			{
				purchaseButton.m_OnActivated.Insert(Purchase);
			}
		}
		
		PopulateTab();
		
		ChimeraWorld world = ChimeraWorld.CastFrom(GetGame().GetWorld());
		if (world)
		{
			m_ItemPreviewManager = world.GetItemPreviewManager();
		}
	}
}