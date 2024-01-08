class ADM_ShopUI_Item : SCR_ScriptedWidgetComponent
{
	[Attribute(defvalue: "1")]
	protected int m_quantity = 1;
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
		
		TextWidget quantity = TextWidget.Cast(w.FindAnyWidget("Quantity"));
		if (quantity) 
			quantity.SetTextFormat("%1", m_quantity);
	
		return true;
	}
}

class ADM_ShopQuantityButton : SCR_ButtonImageComponent
{
	[Attribute()]
	protected int m_iAmount;
	
	int GetAmount()
	{
		return m_iAmount;
	}
}

class ADM_IconBarterTooltip : ScriptedWidgetComponent
{
	protected Widget m_wRoot;
	protected SCR_HoverDetectorComponent m_HoverDetector;
	protected ref ADM_PaymentMethodBase m_PaymentMethod;
	
	[Attribute()]
	protected ref SCR_ScriptedWidgetTooltipPreset m_wTooltipPreset;
	
	//---------------------------------------------------------------------------------------------------
	override void HandlerAttached(Widget w)
	{
		m_wRoot = w;
		
		// Get manager and render preview 
		ChimeraWorld world = ChimeraWorld.CastFrom(GetGame().GetWorld());
		if (!world)
			return;
		
		ItemPreviewManagerEntity manager = world.GetItemPreviewManager();
		if (!manager)
			return;
		
		m_HoverDetector = SCR_HoverDetectorComponent.Cast(w.FindHandler(SCR_HoverDetectorComponent));		
		m_HoverDetector.m_OnHoverDetected.Insert(OnHoverDetected);
		m_HoverDetector.m_OnMouseLeave.Insert(OnMouseLeaveTooltip);
	}
	
	void SetPayment(ADM_PaymentMethodBase payment)
	{
		m_PaymentMethod = payment;
	}
	
	void OnHoverDetected()
	{
		ChimeraWorld world = GetGame().GetWorld();
		if (!world)
			return;
		
		ItemPreviewManagerEntity previewManager = world.GetItemPreviewManager();
		if (!previewManager)
			return;
		
		if (!m_wRoot || !m_wTooltipPreset)
			return;
		
		Widget w = SCR_TooltipManagerEntity.CreateTooltip(m_wTooltipPreset, m_wRoot);
		TextWidget wText = TextWidget.Cast(w.FindAnyWidget("Text"));
		ItemPreviewWidget wRenderTarget = ItemPreviewWidget.Cast(w.FindWidget("Overlay.VerticalLayout0.SizeLayout0.PreviewImage"));
		if (m_PaymentMethod) {
			int quantity = 1;
			ADM_ShopUI_Item item = ADM_ShopUI.FindShopUIItem(m_wRoot);
			if (item)
				quantity = item.GetQuantity();
			
			string displayString = m_PaymentMethod.GetDisplayString(quantity);
			wText.SetText(displayString);
			
			if (wRenderTarget && previewManager) 
				previewManager.SetPreviewItemFromPrefab(wRenderTarget, m_PaymentMethod.GetDisplayEntity(), null, true);
		}
	}
	
	void OnMouseLeaveTooltip()
	{
		
	}
}

class ADM_ShopUI: ChimeraMenuBase
{
	static const ResourceName m_ListBoxPrefab = "{906F7BCCC662C19D}UI/Layouts/Menus/ShopMenu/ListBoxElement_ShopItem.layout";
	static protected ResourceName m_BarterIconPrefab = "{BBEF0EBB9F35B19F}UI/Layouts/Menus/BasicShopMenu/MarketBarterItemIcon.layout";
	
	protected ADM_ShopComponent m_Shop;
	protected ItemPreviewManagerEntity m_ItemPreviewManager;
	
	protected bool m_bFocused = false;
	protected int m_iCategoryBuy = -1;
	protected int m_iCategorySell = -1;
	protected string m_sSearchBuy = "";
	protected string m_sSearchSell = "";
	
	protected Widget m_wRoot;
	protected Widget m_wBuySellTabWidget;
	protected TextWidget m_wHeaderText;
	protected TextWidget m_wMoneyText;
	protected SCR_TabViewComponent m_BuySellTabView;
	protected SCR_TabViewContent m_wBuyTabView;
	protected SCR_TabViewContent m_wSellTabView;
	
	void SetShop(ADM_ShopComponent shop)
	{
		m_Shop = shop;
		
		if (m_wHeaderText)
		{
			m_wHeaderText.SetText(m_Shop.GetName());
		}
		
		ConfigureTabs();
	}
	
	protected void ConfigureCategory(SCR_SpinBoxComponent component, array<ADM_ShopCategory> categories)
	{
		if (!component || !categories)
			return;
		
		component.ClearAll();	
		
		component.AddItem("All");
		foreach (int i, ADM_ShopCategory category : categories)
		{
			component.AddItem(category.m_DisplayName);
		}
	}
	
	protected void SelectCategory(SCR_TabViewContent wTabView, int index, array<ref ADM_ShopMerchandise> items, string search = "")
	{
		if (!m_Shop || !items || items.Count() < 1) 
			return;
		
		array<ref ADM_ShopCategory> categories = m_Shop.GetCategories();
		array<ref ADM_ShopMerchandise> sortedItems = {};
		
		// All category
		if (index == -1) 
		{
			sortedItems = items;
		} else {
			array<ResourceName> validItems = {};
			if (categories[index])
				validItems = categories[index].GetItems();
			
			foreach (ADM_ShopMerchandise merch : items)
			{
				if (validItems.Contains(merch.GetMerchandise().GetPrefab())) {
					sortedItems.Insert(merch);
				}
			}
		}
		
		if (search != string.Empty)
			sortedItems = ProcessSearch(search, sortedItems);
		
		PopulateTab(wTabView, sortedItems);
	}
	
	protected void SelectCategoryBuy(string search = "")
	{		
		SelectCategory(m_wBuyTabView, m_iCategoryBuy, m_Shop.GetMerchandiseBuy(), search);
	}
	
	protected void SelectCategorySell(string search = "")
	{		
		SelectCategory(m_wSellTabView, m_iCategorySell, m_Shop.GetMerchandiseSell(), search);
	}
	
	protected void ChangeCategoryBuy(SCR_SpinBoxComponent comp, int itemIndex)
	{
		m_iCategoryBuy = itemIndex - 1;
		SelectCategoryBuy(m_sSearchBuy);
	}
	
	protected void ChangeCategorySell(SCR_SpinBoxComponent comp, int itemIndex)
	{
		m_iCategorySell = itemIndex - 1;
		SelectCategorySell(m_sSearchSell);
	}
	
	protected void ConfigureTabs()
	{
		if (!m_wBuyTabView || !m_wSellTabView || !m_Shop)
			return;
		
		array<ADM_ShopCategory> categories = {};
		foreach (ADM_ShopCategory category : m_Shop.GetCategories())
		{
			categories.Insert(category);
		}
		
		Widget wCategoryBuy = m_wBuyTabView.m_wTab.FindAnyWidget("CategoriesBox");
		SCR_SpinBoxComponent spinBoxComponentBuy = SCR_SpinBoxComponent.Cast(wCategoryBuy.FindHandler(SCR_SpinBoxComponent));
		ConfigureCategory(spinBoxComponentBuy, categories);
		spinBoxComponentBuy.m_OnChanged.Insert(ChangeCategoryBuy);
			
		Widget wCategorySell = m_wSellTabView.m_wTab.FindAnyWidget("CategoriesBox");
		SCR_SpinBoxComponent spinBoxComponentSell = SCR_SpinBoxComponent.Cast(wCategorySell.FindHandler(SCR_SpinBoxComponent));
		ConfigureCategory(spinBoxComponentSell, categories);
		spinBoxComponentSell.m_OnChanged.Insert(ChangeCategorySell);
		
		Widget wSearchBuy = m_wBuyTabView.m_wTab.FindAnyWidget("EditBoxSearch0");
		SCR_EditBoxSearchComponent searchBuy = SCR_EditBoxSearchComponent.Cast(wSearchBuy.FindHandler(SCR_EditBoxSearchComponent));
		if (searchBuy && searchBuy.m_OnConfirm)
		{
			searchBuy.m_OnConfirm.Insert(ProcessSearchBuy);
		}
		
		Widget wSearchSell = m_wSellTabView.m_wTab.FindAnyWidget("EditBoxSearch0");
		SCR_EditBoxSearchComponent searchSell = SCR_EditBoxSearchComponent.Cast(wSearchSell.FindHandler(SCR_EditBoxSearchComponent));
		if (searchSell && searchSell.m_OnConfirm)
		{
			searchSell.m_OnConfirm.Insert(ProcessSearchSell);
		}
		
		SelectCategoryBuy(m_sSearchBuy);
		SelectCategorySell(m_sSearchSell);
	}
	
	protected void ClearTab(SCR_TabViewContent wTabView)
	{
		if (!wTabView)
			return;
		
		Widget wListbox = wTabView.m_wTab.FindAnyWidget("ListBox0");
		if (!wListbox)
			return;
		
		SCR_ListBoxComponent listbox = SCR_ListBoxComponent.Cast(wListbox.FindHandler(SCR_ListBoxComponent));
		if (!listbox)
			return;
		
		listbox.Clear();
	}
	
	protected void PopulateTab(SCR_TabViewContent wTabView, array<ref ADM_ShopMerchandise> merchandise)
	{
		Widget wListbox = wTabView.m_wTab.FindAnyWidget("ListBox0");
		if (!wListbox)
			return;
		
		SCR_ListBoxComponent listbox = SCR_ListBoxComponent.Cast(wListbox.FindHandler(SCR_ListBoxComponent));
		if (!listbox)
			return;
		
		ClearTab(wTabView);
		foreach (ADM_ShopMerchandise merch : merchandise)
		{
			if (!merch)
				continue;
			
			ADM_MerchandiseType merchType = merch.GetMerchandise();
			if (!merchType)
				continue;
			
			// Item Name
			string itemName = ADM_Utils.GetPrefabDisplayName(merchType.GetPrefab());
			if (!itemName)
				itemName = "Item";
			
			int itemIdx = listbox.AddItem(itemName);
			SCR_ListBoxElementComponent lbItem = listbox.GetElementComponent(itemIdx);
			if (!lbItem)
				return;
			
			// Item Preview
			ItemPreviewWidget wItemPreview = ItemPreviewWidget.Cast(lbItem.GetRootWidget().FindAnyWidget("ItemPreview0"));
			if (wItemPreview)
			{
				m_ItemPreviewManager.SetPreviewItemFromPrefab(wItemPreview, merchType.GetPrefab(), null, false);
			}
			
			// Cost
			TextWidget wItemPrice = TextWidget.Cast(lbItem.GetRootWidget().FindAnyWidget("Cost"));
			array<ref ADM_PaymentMethodBase> paymentRequirement = merch.GetBuyPayment();
			if (ADM_ShopComponent.IsBuyPaymentOnlyCurrency(merch) || paymentRequirement.Count() == 0)
			{
				if (wItemPrice) {
					wItemPrice.SetVisible(true);
					if (paymentRequirement.Count() == 0)
					{
						wItemPrice.SetTextFormat("Free");
					} else {
						ADM_PaymentMethodCurrency paymentMethod = ADM_PaymentMethodCurrency.Cast(paymentRequirement[0]);
						int quantity = paymentMethod.GetQuantity();
						if (quantity > 0)
						{
							wItemPrice.SetTextFormat("$%1", quantity);
						} else {
							wItemPrice.SetTextFormat("Free");
						}
					}
				}
			} else {
				Widget priceParent = lbItem.GetRootWidget().FindAnyWidget("CostOverlay");
				if (wItemPrice)
				{ 
					wItemPrice.SetVisible(false);
				}
				
				for (int i = 0; i < paymentRequirement.Count(); i++)
				{
					Widget icon = GetGame().GetWorkspace().CreateWidgets(m_BarterIconPrefab, priceParent);
					ADM_IconBarterTooltip barterItemIcon = ADM_IconBarterTooltip.Cast(icon.FindHandler(ADM_IconBarterTooltip));
					if (barterItemIcon)
					{
						barterItemIcon.SetPayment(paymentRequirement[i]);
					}
				}
			}
			
			// Quantity
			ADM_ShopUI_Item item = ADM_ShopUI_Item.Cast(lbItem.GetRootWidget().FindHandler(ADM_ShopUI_Item));
			if (item) {
				item.SetQuantity(1);
				item.SetMerchandise(merch);
			}
				
			Widget lessBtnWidget = lbItem.GetRootWidget().FindAnyWidget("QuantityLess");
			Widget moreBtnWidget = lbItem.GetRootWidget().FindAnyWidget("QuantityMore");
			
			if (lessBtnWidget && moreBtnWidget)
			{
				if (!merch.GetMerchandise().CanPurchaseMultiple()) 
				{
					lessBtnWidget.SetEnabled(false);
					moreBtnWidget.SetEnabled(false);
					lessBtnWidget.SetVisible(false);
					moreBtnWidget.SetVisible(false);
				} else {
					ADM_ShopQuantityButton lessQuantityBtn = ADM_ShopQuantityButton.Cast(lessBtnWidget.FindHandler(ADM_ShopQuantityButton));
					if (lessQuantityBtn)
					{
						lessQuantityBtn.m_OnClicked.Clear();
						lessQuantityBtn.m_OnClicked.Insert(UpdateItemQuantity);
					}
					
					ADM_ShopQuantityButton moreQuantityBtn = ADM_ShopQuantityButton.Cast(moreBtnWidget.FindHandler(ADM_ShopQuantityButton));
					if (moreQuantityBtn)
					{
						moreQuantityBtn.m_OnClicked.Clear();
						moreQuantityBtn.m_OnClicked.Insert(UpdateItemQuantity);
					}
				}
			}
		}
	}
	
	protected array<ref ADM_ShopMerchandise> ProcessSearch(string search, array<ref ADM_ShopMerchandise> merchandise)
	{
		search = search.Trim();
		search.ToLower();
		
		if (search.IsEmpty())
		{
			return merchandise;
		}
		
		array<ref ADM_ShopMerchandise> matchedMerchandise = {};
		foreach (ADM_ShopMerchandise merch : merchandise)
		{
			string itemName = ADM_Utils.GetPrefabDisplayName(merch.GetMerchandise().GetPrefab());
			itemName.ToLower();
			
			if (itemName.Contains(search))
			{
				matchedMerchandise.Insert(merch);
			}
		}
		
		return matchedMerchandise;
	}
	
	protected void ProcessSearchBuy(SCR_EditBoxSearchComponent searchbox, string search)
	{
		m_sSearchBuy = search;
		SelectCategoryBuy(m_sSearchBuy);
	}
	
	protected void ProcessSearchSell(SCR_EditBoxSearchComponent searchbox, string search)
	{
		m_sSearchSell = search;
		SelectCategorySell(m_sSearchSell);
	}
	
	private int m_iCachedMoneyCount = 0;
	private float m_fCachedMoneyTime = -5000000;
	protected void UpdateMoneyText()
	{
		if (!m_wMoneyText)
			return;
		
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		if (!playerController)
			return;
		
		IEntity player = playerController.GetMainEntity();
		if (!player)
			return;
		
		SCR_InventoryStorageManagerComponent inventoryManager = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventoryManager)
			return;
		
		float time = System.GetTickCount();
		if (time - m_fCachedMoneyTime > 1000)
		{
			m_iCachedMoneyCount = ADM_CurrencyComponent.FindTotalCurrencyInInventory(inventoryManager);
			m_fCachedMoneyTime = time;
		}
			
		string money = string.Format("$%1", m_iCachedMoneyCount);
		m_wMoneyText.SetText(money);
	}
	
	void UpdateItemQuantity(ADM_ShopQuantityButton button)
	{
		ADM_ShopUI_Item uiItem = FindShopUIItem(button.GetRootWidget());
		if (uiItem) 
			uiItem.UpdateQuantity(button.GetAmount());
	}
	
	static ADM_ShopUI_Item FindShopUIItem(Widget w)
	{
		Widget parent = w.GetParent();
		while (parent)
		{
			ADM_ShopUI_Item uiItem = ADM_ShopUI_Item.Cast(parent.FindHandler(ADM_ShopUI_Item));
			if (uiItem)
				return uiItem;
			else
				parent = parent.GetParent();
		}
		
		return null;
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
		
		m_wBuySellTabWidget = GetRootWidget().FindAnyWidget("BuySellTabView");
		if (!m_wBuySellTabWidget) return;
		
		m_BuySellTabView = SCR_TabViewComponent.Cast(m_wBuySellTabWidget.FindHandler(SCR_TabViewComponent));
		if (!m_BuySellTabView) return;
		
		m_wBuyTabView = m_BuySellTabView.GetEntryContent(0);
		m_wSellTabView = m_BuySellTabView.GetEntryContent(1);
		
		m_wHeaderText = TextWidget.Cast(m_wRoot.FindAnyWidget("HeaderText"));
		m_wMoneyText = TextWidget.Cast(m_wRoot.FindAnyWidget("TotalMoneyText"));
		
		Widget closeWidget = ButtonWidget.Cast(m_wRoot.FindAnyWidget("Back"));
		if (closeWidget)
		{
			SCR_InputButtonComponent closeButton = SCR_InputButtonComponent.Cast(closeWidget.FindHandler(SCR_InputButtonComponent));
			if (closeButton)
			{
				closeButton.m_OnActivated.Insert(Close);
			}
		}
		
		ConfigureTabs();
		UpdateMoneyText();
		
		ChimeraWorld world = ChimeraWorld.CastFrom(GetGame().GetWorld());
		if (world)
		{
			m_ItemPreviewManager = world.GetItemPreviewManager();
		}
	}
}