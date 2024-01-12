class ADM_ShopUI_Item : SCR_ModularButtonComponent
{
	[Attribute(defvalue: "MenuNavLeft")]
	protected string m_sQuantityActionLess;
	
	[Attribute(defvalue: "MenuNavRight")]
	protected string m_sQuantityActionMore;
	
	[Attribute(defvalue: "MenuLeft")]
	protected string m_sScrollActionLeft;
	
	[Attribute(defvalue: "MenuRight")]
	protected string m_sScrollActionRight;
	
	[Attribute(defvalue: "1")]
	protected int m_quantity = 1;
	
	protected bool m_bHasActionListeners;
	protected ref ADM_ShopMerchandise m_Merchandise;
	protected SCR_PagingButtonComponent m_ButtonLeft;
	protected SCR_PagingButtonComponent m_ButtonRight;
	protected ScrollLayoutWidget m_wPriceScrollLayout;
	protected bool m_bBuyOrSell; // true buy, false sell
	
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
		if (m_quantity < 1)
			m_quantity = 1;
		
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
	
	protected void OnQuantityLess()
	{
		if (GetGame().GetWorkspace().GetFocusedWidget() != m_wRoot)
			return;

		UpdateQuantity(-1);
	}

	protected void OnQuantityMore()
	{
		if (GetGame().GetWorkspace().GetFocusedWidget() != m_wRoot)
			return;

		UpdateQuantity(1);
	}
	
	protected void OnScrollLeft()
	{
		if (GetGame().GetWorkspace().GetFocusedWidget() != m_wRoot || !m_wPriceScrollLayout)
			return;
		
		float x,y;
		m_wPriceScrollLayout.GetSliderPos(x,y);
		m_wPriceScrollLayout.SetSliderPos(x-0.1,y);
	}
	
	protected void OnScrollRight()
	{
		if (GetGame().GetWorkspace().GetFocusedWidget() != m_wRoot || !m_wPriceScrollLayout)
			return;
		
		float x,y;
		m_wPriceScrollLayout.GetSliderPos(x,y);
		m_wPriceScrollLayout.SetSliderPos(x+0.1,y);
	}
	
	void SetBuyOrSell(bool b)
	{
		m_bBuyOrSell = b;
	}
	
	bool GetBuyOrSell()
	{
		return m_bBuyOrSell;
	}
	
	protected void AddToCart()
	{
		if (!m_Merchandise)
			return;
		
		PrintFormat("add to %1 cart", m_bBuyOrSell);
	}
	
	override void HandlerAttached(Widget w)
	{
		super.HandlerAttached(w);
		
		Widget left = w.FindAnyWidget("ButtonLeft");
		Widget right = w.FindAnyWidget("ButtonRight");
		m_wPriceScrollLayout = ScrollLayoutWidget.Cast(w.FindAnyWidget("ScrollLayout0"));

		if (left)
		{
			left.SetVisible(false);
			m_ButtonLeft = SCR_PagingButtonComponent.Cast(left.FindHandler(SCR_PagingButtonComponent));
			if (m_ButtonLeft)
				m_ButtonLeft.m_OnActivated.Insert(OnQuantityLess);
		}

		if (right)
		{
			right.SetVisible(false);
			m_ButtonRight = SCR_PagingButtonComponent.Cast(right.FindHandler(SCR_PagingButtonComponent));
			if (m_ButtonRight)
				m_ButtonRight.m_OnActivated.Insert(OnQuantityMore);
		}
		
		ADM_ShopUI_Item mainBtn = ADM_ShopUI_Item.Cast(w.FindHandler(ADM_ShopUI_Item));
		if (mainBtn)
		{
			mainBtn.m_OnClicked.Insert(AddToCart);
		}
		
		OnUpdate(m_wRoot);
	}
	
	void AddActionListeners()
	{
		if (m_bHasActionListeners)
			return;
		
		GetGame().GetInputManager().AddActionListener(m_sQuantityActionLess, EActionTrigger.DOWN, OnQuantityLess);
		GetGame().GetInputManager().AddActionListener(m_sQuantityActionMore, EActionTrigger.DOWN, OnQuantityMore);
		
		GetGame().GetInputManager().AddActionListener(m_sScrollActionLeft, EActionTrigger.DOWN, OnScrollLeft);
		GetGame().GetInputManager().AddActionListener(m_sScrollActionRight, EActionTrigger.DOWN, OnScrollRight);
		
		m_bHasActionListeners = true;
	}
	
	void RemoveActionListeners()
	{
		if (!m_bHasActionListeners)
			return;
		
		GetGame().GetInputManager().RemoveActionListener(m_sQuantityActionLess, EActionTrigger.DOWN, OnQuantityLess);
		GetGame().GetInputManager().RemoveActionListener(m_sQuantityActionMore, EActionTrigger.DOWN, OnQuantityMore);
		
		GetGame().GetInputManager().RemoveActionListener(m_sScrollActionLeft, EActionTrigger.DOWN, OnScrollLeft);
		GetGame().GetInputManager().RemoveActionListener(m_sScrollActionRight, EActionTrigger.DOWN, OnScrollRight);
		
		m_bHasActionListeners = false;
	}
	
	override bool OnFocus(Widget w, int x, int y)
	{
		super.OnFocus(w, x, y);

		AddActionListeners();

		Widget left = w.FindAnyWidget("ButtonLeft");
		if (left)
		{
			left.SetVisible(true);
		}

		Widget right = w.FindAnyWidget("ButtonRight");
		if (right)
		{
			right.SetVisible(true);
		}
		
		return false;
	}

	override bool OnFocusLost(Widget w, int x, int y)
	{
		super.OnFocusLost(w, x, y);

		RemoveActionListeners();

		Widget left = w.FindAnyWidget("ButtonLeft");
		if (left)
		{
			left.SetVisible(false);
		}

		Widget right = w.FindAnyWidget("ButtonRight");
		if (right)
		{
			right.SetVisible(false);
		}

		return false;
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

class ADM_IconBarterTooltip : ScriptedWidgetComponent
{
	protected Widget m_wRoot;
	protected SCR_HoverDetectorComponent m_HoverDetector;
	protected ref ADM_PaymentMethodBase m_PaymentMethod;
	
	protected ItemPreviewWidget m_wPreviewWidget;
	protected TextWidget m_wQuantityWidget;
	
	[Attribute()]
	protected ref SCR_ScriptedWidgetTooltipPreset m_wTooltipPreset;
	
	void SetPayment(ADM_PaymentMethodBase payment)
	{
		m_PaymentMethod = payment;
		
		if (!m_wPreviewWidget)
			return;
		
		ChimeraWorld world = ChimeraWorld.CastFrom(GetGame().GetWorld());
		if (!world)
			return;
		
		ItemPreviewManagerEntity manager = world.GetItemPreviewManager();
		if (!manager)
			return;
		
		manager.SetPreviewItemFromPrefab(m_wPreviewWidget, m_PaymentMethod.GetDisplayEntity(), null, false);		
		
		if (!m_wQuantityWidget)
			return;
		
		int quantity = 1;
		ADM_ShopUI_Item item = ADM_ShopUI.FindShopUIItem(m_wRoot);
		if (item)
			quantity = item.GetQuantity();
			
		string displayString = m_PaymentMethod.GetDisplayString(quantity);
		if (m_PaymentMethod.Type() == ADM_PaymentMethodItem)
		{
			ADM_PaymentMethodItem paymentMethodItem = ADM_PaymentMethodItem.Cast(m_PaymentMethod);
			displayString = string.Format("x%1", paymentMethodItem.GetItemQuantity() * item.GetQuantity());
		}
		
		m_wQuantityWidget.SetText(string.Format("%1", displayString));
	}
	
	override void HandlerAttached(Widget w)
	{
		m_wRoot = w;
		 
		ChimeraWorld world = ChimeraWorld.CastFrom(GetGame().GetWorld());
		if (!world)
			return;
		
		ItemPreviewManagerEntity manager = world.GetItemPreviewManager();
		if (!manager)
			return;
		
		m_HoverDetector = SCR_HoverDetectorComponent.Cast(w.FindHandler(SCR_HoverDetectorComponent));	
		m_wPreviewWidget = ItemPreviewWidget.Cast(w.FindAnyWidget("ItemPreview0"));	
		m_wQuantityWidget = TextWidget.Cast(w.FindAnyWidget("Quantity"));		
		m_HoverDetector.m_OnHoverDetected.Insert(OnHoverDetected);
		m_HoverDetector.m_OnMouseLeave.Insert(OnMouseLeaveTooltip);
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
		ItemPreviewWidget wRenderTarget = ItemPreviewWidget.Cast(w.FindAnyWidget("ItemPreview"));
		if (m_PaymentMethod) {
			int quantity = 1;
			ADM_ShopUI_Item item = ADM_ShopUI.FindShopUIItem(m_wRoot);
			if (item)
				quantity = item.GetQuantity();
			
			string displayString = m_PaymentMethod.GetDisplayString(quantity);
			wText.SetText(displayString);
			
			if (wRenderTarget && previewManager) 
				previewManager.SetPreviewItemFromPrefab(wRenderTarget, m_PaymentMethod.GetDisplayEntity(), null, false);
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
	protected TextWidget m_wHeaderText;
	protected TextWidget m_wMoneyText;
	
	protected Widget m_wBuySellTabWidget;
	protected SCR_TabViewComponent m_BuySellTabComponent;
	protected SCR_TabViewContent m_wBuyTabView;
	protected SCR_TabViewContent m_wSellTabView;
	
	protected Widget m_wCartTabWidget;
	protected SCR_TabViewComponent m_CartTabComponent;
	protected SCR_TabViewContent m_wCartTabView;
	
	protected ref map<ADM_ShopMerchandise, int> m_BuyShoppingCart = new map<ADM_ShopMerchandise, int>;
	protected ref map<ADM_ShopMerchandise, int> m_SellShoppingCart = new map<ADM_ShopMerchandise, int>;
	
	void AddToCart(ADM_ShopUI_Item item)
	{
		ADM_ShopMerchandise merch = item.GetMerchandise();
		if (!merch)
			return;
		
		map<ADM_ShopMerchandise, int> shoppingCart = m_BuyShoppingCart;
		if (!item.GetBuyOrSell())
			shoppingCart = m_SellShoppingCart;
		
		bool inShoppingCart = shoppingCart.Contains(merch);
		if (inShoppingCart)
		{
			int curQuantity = shoppingCart.Get(merch);
			int newQuantity = curQuantity + item.GetQuantity();
			shoppingCart.Set(merch, newQuantity);
		} else {
			shoppingCart.Insert(merch, item.GetQuantity());
		}
		
		//PopulateCartTab(m_wCartTabView);
	}
	
	/*protected void PopulateCartTab(SCR_TabViewContent wTabView)
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
			
			ADM_MerchandiseType merchType = merch.GetType();
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
			//ADM_ShopUI_Item item = ADM_ShopUI_Item.Cast(lbItem.GetRootWidget().FindHandler(ADM_ShopUI_Item));
			//if (item) {
			//	item.SetQuantity(1);
			//	item.SetMerchandise(merch);
			//}
		}
	}*/
	
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
	
	protected void SelectCategory(SCR_TabViewContent wTabView, int index, array<ref ADM_ShopMerchandise> items, string search = "", bool buyOrSell = true)
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
				if (validItems.Contains(merch.GetType().GetPrefab())) {
					sortedItems.Insert(merch);
				}
			}
		}
		
		if (search != string.Empty)
			sortedItems = ProcessSearch(search, sortedItems);
		
		PopulateTab(wTabView, sortedItems, buyOrSell);
	}
	
	protected void SelectCategoryBuy(string search = "")
	{		
		SelectCategory(m_wBuyTabView, m_iCategoryBuy, m_Shop.GetMerchandiseBuy(), search, true);
	}
	
	protected void SelectCategorySell(string search = "")
	{		
		SelectCategory(m_wSellTabView, m_iCategorySell, m_Shop.GetMerchandiseSell(), search, false);
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
	
	protected void PopulateTab(SCR_TabViewContent wTabView, array<ref ADM_ShopMerchandise> merchandise, bool buyOrSell = true)
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
			
			ADM_MerchandiseType merchType = merch.GetType();
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
				item.SetBuyOrSell(buyOrSell);
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
			string itemName = ADM_Utils.GetPrefabDisplayName(merch.GetType().GetPrefab());
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
		if (!m_wBuySellTabWidget) 
			return;
		
		m_BuySellTabComponent = SCR_TabViewComponent.Cast(m_wBuySellTabWidget.FindHandler(SCR_TabViewComponent));
		if (!m_BuySellTabComponent) 
			return;
		
		if (m_BuySellTabComponent.GetTabCount() == 2)
		{
			m_wBuyTabView = m_BuySellTabComponent.GetEntryContent(0);
			m_wSellTabView = m_BuySellTabComponent.GetEntryContent(1);
		}
		
		m_wCartTabWidget = GetRootWidget().FindAnyWidget("CartTabView");
		if (!m_wCartTabWidget) 
			return;
		
		m_CartTabComponent = SCR_TabViewComponent.Cast(m_wCartTabWidget.FindHandler(SCR_TabViewComponent));
		if (!m_CartTabComponent) 
			return;
		
		if (m_CartTabComponent.GetTabCount() == 1)
		{
			m_wCartTabView = m_CartTabComponent.GetEntryContent(0);
		}	
		
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