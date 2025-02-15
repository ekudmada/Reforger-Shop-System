class ADM_ShopUI_Item : SCR_ModularButtonComponent
{
	[Attribute(defvalue: "MenuNavLeft_NoScroll")]
	protected string m_sQuantityActionLess;
	
	[Attribute(defvalue: "MenuNavRight_NoScroll")]
	protected string m_sQuantityActionMore;
	
	[Attribute(defvalue: "MenuLeft")]
	protected string m_sScrollActionLeft;
	
	[Attribute(defvalue: "MenuRight")]
	protected string m_sScrollActionRight;
	
	[Attribute(defvalue: "1")]
	protected int m_quantity = 1;
	
	protected bool m_bHasActionListeners;
	protected ref ADM_ShopMerchandise m_Merchandise;
	protected ADM_ShopUI m_ShopUI;
	protected SCR_PagingButtonComponent m_ButtonLeft;
	protected SCR_PagingButtonComponent m_ButtonRight;
	protected ScrollLayoutWidget m_wPriceScrollLayout;
	protected bool m_bBuyOrSell; 
	protected int m_iMaxQuantity = 0;
	protected bool m_bIsCartItem = false;
	
	int GetQuantity()
	{
		return m_quantity;
	}
	
	void SetQuantity(int quantity)
	{
		m_quantity = quantity;
		UpdateItem(this.m_wRoot);
	}
	
	void UpdateQuantity(int amount)
	{
		m_quantity = m_quantity + amount;
		if (m_quantity < 1)
			m_quantity = 1;
		
		UpdateItem(this.m_wRoot);
	}
	
	void SetMaxQuantity(int quantity)
	{
		m_iMaxQuantity = quantity;
	}
	
	ADM_ShopMerchandise GetMerchandise()
	{
		return m_Merchandise;
	}
	
	void SetMerchandise(ADM_ShopMerchandise merch)
	{
		m_Merchandise = merch;
	}
	
	void SetShopUI(ADM_ShopUI shopUI)
	{
		m_ShopUI = shopUI;
	}
	
	protected void OnQuantityLess()
	{
		if (GetGame().GetWorkspace().GetFocusedWidget() != m_wRoot)
			return;

		if (m_bIsCartItem)
			return;
		
		UpdateQuantity(-1);
	}

	protected void OnQuantityMore()
	{
		if (GetGame().GetWorkspace().GetFocusedWidget() != m_wRoot)
			return;

		if (m_bIsCartItem)
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
		
		UpdateItem(m_wRoot);
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
	
	void SetCartItem(bool b)
	{
		m_bIsCartItem = b;
	}
	
	bool CanPurchase()
	{
		if (GetBuyOrSell())
		{
			int additionalQuantity = m_quantity;
			if (m_bIsCartItem)
			{
				additionalQuantity = 0;
			}
			
			return m_ShopUI.CanPurchase(m_Merchandise, additionalQuantity);
		} else {
			return true;
		}
	}
	
	bool UpdateItem(Widget w)
	{
		if (m_quantity < 1) m_quantity = 1;

		if (m_iMaxQuantity > 0 && m_quantity > m_iMaxQuantity)
		{
			m_quantity = m_iMaxQuantity;
		}
		
		TextWidget quantity = TextWidget.Cast(w.FindAnyWidget("Quantity"));
		if (quantity) 
		{
			quantity.SetTextFormat("x%1", m_quantity);
			
			if (m_Merchandise && !CanPurchase())
			{
				quantity.SetColor(Color.Red);
			} else {
				quantity.SetColor(Color.White);
			}
			
			quantity.Update();
		}
		
		return true;
	}
}

class ADM_IconBarterTooltip : ScriptedWidgetComponent
{
	protected Widget m_wRoot;
	protected SCR_EventHandlerComponent m_HoverDetector;
	protected ref ADM_PaymentMethodBase m_PaymentMethod;
	
	protected ItemPreviewWidget m_wPreviewWidget;
	protected TextWidget m_wQuantityWidget;
	
	[Attribute()]
	protected ref SCR_ScriptedWidgetTooltipPreset m_wTooltipPreset;
	
	protected SCR_ScriptedWidgetTooltip m_Tooltip;
	
	void SetPayment(ADM_PaymentMethodBase payment, Color texCol = Color.White, int requestedQuantity = 1)
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
		
		m_wQuantityWidget.SetColor(texCol);
		
		int quantity = 1;
		string displayString = m_PaymentMethod.GetDisplayName(quantity * requestedQuantity);
		if (m_PaymentMethod.Type() == ADM_PaymentMethodItem)
		{
			ADM_PaymentMethodItem paymentMethodItem = ADM_PaymentMethodItem.Cast(m_PaymentMethod);
			if (paymentMethodItem)
			{
				quantity = Math.AbsInt(paymentMethodItem.GetItemQuantity());
			}
			
			if (paymentMethodItem.GetItemQuantity() < 0)
			{
				m_wQuantityWidget.SetColor(Color.Green);
			}
			
			displayString = string.Format("x%1", quantity * requestedQuantity);
		}
		
		if (m_PaymentMethod.Type() == ADM_PaymentMethodCurrency)
		{
			ADM_PaymentMethodCurrency paymentMethodCurrency = ADM_PaymentMethodCurrency.Cast(m_PaymentMethod);
			if (paymentMethodCurrency && paymentMethodCurrency.GetQuantity() < 0)
			{
				m_wQuantityWidget.SetColor(Color.Green);
			}
		}
		
		m_wQuantityWidget.SetText(string.Format("%1", displayString));
		
		if (quantity == 0)
		{
			delete m_wRoot;
		}
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
		
		//m_HoverDetector = SCR_EventHandlerComponent.Cast(w.FindHandler(SCR_EventHandlerComponent));	
		m_wPreviewWidget = ItemPreviewWidget.Cast(w.FindAnyWidget("ItemPreview0"));	
		m_wQuantityWidget = TextWidget.Cast(w.FindAnyWidget("BarterIconQuantity"));		
		
		//SCR_ScriptedWidgetTooltip.GetOnTooltipShow().Insert(OnTooltipShow);
	}
	
	void OnTooltipShow(SCR_ScriptedWidgetTooltip tooltip)
	{
		//Print(tooltip);
		//m_Tooltip = tooltip;
		/*ChimeraWorld world = GetGame().GetWorld();
		if (!world)
			return;
		
		ItemPreviewManagerEntity previewManager = world.GetItemPreviewManager();
		if (!previewManager)
			return;
		
		if (!m_wRoot || !m_wTooltipPreset)
			return;
		
		/*Widget w = SCR_TooltipManagerEntity.CreateTooltip(m_wTooltipPreset, m_wRoot);
		TextWidget wText = TextWidget.Cast(w.FindAnyWidget("Text"));
		ItemPreviewWidget wRenderTarget = ItemPreviewWidget.Cast(w.FindAnyWidget("ItemPreview"));
		if (m_PaymentMethod) {
			int quantity = 1;
			string displayString = m_PaymentMethod.GetDisplayName(quantity);
			wText.SetText(displayString);
			
			if (wRenderTarget && previewManager) 
				previewManager.SetPreviewItemFromPrefab(wRenderTarget, m_PaymentMethod.GetDisplayEntity(), null, false);
		}*/
	}
}

class ADM_ShopUI: ChimeraMenuBase
{
	static const ResourceName m_BarterIconPrefab = "{BBEF0EBB9F35B19F}UI/Layouts/Menus/BasicShopMenu/MarketBarterItemIcon.layout";
	
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
	
	void SetShop(ADM_ShopComponent shop)
	{
		m_Shop = shop;
		
		if (m_wHeaderText)
		{
			m_wHeaderText.SetText(m_Shop.GetName());
		}
		
		ConfigureTabs();
	}
	
	bool CanPurchase(ADM_ShopMerchandise additionalMerchandise = null, int additionalQuantity = 0)
	{
		// this method isn't perfect because it doesn't keep track of money spent as it iterates all merchandise in the cart
		// so at best it only returns false when an item of the same type exceeds the amount possible
		
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		if (!playerController)
			return false;
		
		IEntity player = playerController.GetMainEntity();
		if (!player)
			return false;
		
		int sameMerchandiseQuantity = 0;
		foreach(ADM_ShopMerchandise merch, int quantity : m_BuyShoppingCart)
		{
			if (additionalMerchandise && additionalMerchandise == merch)
				sameMerchandiseQuantity = quantity;
			
			if (!m_Shop.CanPurchase(player, merch.GetBuyPayment(), quantity))
			{
				return false;
			}
		}
		
		if (additionalMerchandise && !m_Shop.CanPurchase(player, additionalMerchandise.GetBuyPayment(), additionalQuantity+sameMerchandiseQuantity))
			return false;
		
		return true;
	}
	
	int GetCartQuantity(ADM_ShopMerchandise merchandise, bool buyOrSell = true)
	{
		map<ADM_ShopMerchandise, int> shoppingCart = m_BuyShoppingCart;
		if (!buyOrSell)
		{
			shoppingCart = m_SellShoppingCart;
		}
			
		int quantity = shoppingCart.Get(merchandise);
		return quantity;
	}
	
	void AddToCart(ADM_ShopUI_Item item)
	{
		ADM_ShopMerchandise merch = item.GetMerchandise();
		if (!merch)
			return;
		
		if (!item.CanPurchase())
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
		
		PopulateCartTab(m_wCartTabView);		
	}
	
	void RemoveFromCart(ADM_ShopUI_Item item)
	{
		if (!item)
			return;
		
		ADM_ShopMerchandise merch = item.GetMerchandise();
		if (!merch)
			return;
		
		if (!item.GetBuyOrSell())
		{
			m_SellShoppingCart.Remove(merch);
		} else {
			m_BuyShoppingCart.Remove(merch);
		}
		
		PopulateCartTab(m_wCartTabView);
	}
	
	protected void CreateCartListboxItem(SCR_ListBoxComponent listbox, ADM_ShopMerchandise merch, bool buyOrSell = true)
	{
		ADM_MerchandiseType merchType = merch.GetType();
		if (!merchType)
			return;
		
		// Item Name
		string itemName = merchType.GetDisplayName();
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
			m_ItemPreviewManager.SetPreviewItemFromPrefab(wItemPreview, merchType.GetDisplayEntity(), null, false);
		}
			
		// Cost
		TextWidget wItemPrice = TextWidget.Cast(lbItem.GetRootWidget().FindAnyWidget("Cost"));
		array<ref ADM_PaymentMethodBase> paymentRequirement = merch.GetBuyPayment();
		bool isCurrencyOnly = ADM_ShopComponent.IsBuyPaymentOnlyCurrency(merch);
		if (!buyOrSell)
		{
			paymentRequirement = merch.GetSellPayment();
			isCurrencyOnly = ADM_ShopComponent.IsSellPaymentOnlyCurrency(merch);
		}
		
		if (isCurrencyOnly || paymentRequirement.Count() == 0)
		{
			if (wItemPrice) {
				string posOrMinus = "+";
				Color textCol = Color.Green;
				if (buyOrSell)
				{
					textCol = Color.Red;
					posOrMinus = "-";
				}
				wItemPrice.SetColor(textCol);
				
				wItemPrice.SetVisible(true);
				if (paymentRequirement.Count() == 0)
				{
					wItemPrice.SetColor(Color.Green);
					wItemPrice.SetText("Free");
				} else {
					ADM_PaymentMethodCurrency paymentMethod = ADM_PaymentMethodCurrency.Cast(paymentRequirement[0]);
					int quantity = paymentMethod.GetQuantity();
					if (quantity > 0)
					{
						wItemPrice.SetTextFormat("%1$%2", posOrMinus, quantity);
					} else {
						wItemPrice.SetColor(Color.White);
						wItemPrice.SetText("Free");
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
					Color texColor = Color.White;
					if (!buyOrSell)
					{
						texColor = Color.Green;
					}
					
					barterItemIcon.SetPayment(paymentRequirement[i], texColor);
				}
			}
		}
		
		// Quantity
		ADM_ShopUI_Item item = ADM_ShopUI_Item.Cast(lbItem.GetRootWidget().FindHandler(ADM_ShopUI_Item));
		if (item) {
			int quantity = GetCartQuantity(merch, buyOrSell);
			item.SetShopUI(this);
			item.SetCartItem(true);
			item.SetMerchandise(merch);
			item.SetBuyOrSell(buyOrSell);
			item.SetQuantity(quantity);
			item.SetMaxQuantity(merch.GetType().GetMaxPurchaseQuantity());
			
			item.m_OnClicked.Insert(RemoveFromCart);
		}
	}
	
	protected void PopulateCartTab(SCR_TabViewContent wTabView)
	{
		Widget wListboxBuy = wTabView.m_wTab.FindAnyWidget("ListBox0");
		if (!wListboxBuy)
			return;
		
		SCR_ListBoxComponent listboxBuy = SCR_ListBoxComponent.Cast(wListboxBuy.FindHandler(SCR_ListBoxComponent));
		if (!listboxBuy)
			return;
		
		Widget wListboxSell = wTabView.m_wTab.FindAnyWidget("ListBox1");
		if (!wListboxSell)
			return;
		
		SCR_ListBoxComponent listboxSell = SCR_ListBoxComponent.Cast(wListboxSell.FindHandler(SCR_ListBoxComponent));
		if (!listboxSell)
			return;
		
		ClearTab(wTabView);
		
		TextWidget buyText = TextWidget.Cast(wTabView.m_wTab.FindAnyWidget("Text0"));
		if (m_BuyShoppingCart.Count() > 0)
		{
			buyText.SetVisible(true);
		} else {
			buyText.SetVisible(false);
		}
		
		TextWidget sellText = TextWidget.Cast(wTabView.m_wTab.FindAnyWidget("Text1"));
		if (m_SellShoppingCart.Count() > 0)
		{
			sellText.SetVisible(true);
		} else {
			sellText.SetVisible(false);
		}
		
		array<ref ADM_PaymentMethodBase> totalCost = {};
		foreach (ADM_ShopMerchandise merch, int quantity : m_BuyShoppingCart)
		{
			if (!merch)
				continue;
				
			ADM_MerchandiseType merchType = merch.GetType();
			if (!merchType)
				continue;
			
			CreateCartListboxItem(listboxBuy, merch, true);
			
			foreach(ADM_PaymentMethodBase paymentMethod : merch.GetBuyPayment())
			{
				bool didAddToTotal = false;
				foreach(ADM_PaymentMethodBase existing : totalCost)
				{
					if (didAddToTotal)
					{
						continue;
					}
					
					bool isSame = existing.Equals(paymentMethod);
					didAddToTotal = existing.Add(paymentMethod, quantity);
				}
				
				if (!didAddToTotal)
				{
					ADM_PaymentMethodBase clone = ADM_PaymentMethodBase.Cast(paymentMethod.Clone());
					int idx = totalCost.Insert(clone);
					clone.Add(paymentMethod, quantity-1);
				}
			}
		}
		
		foreach (ADM_ShopMerchandise merch, int quantity : m_SellShoppingCart)
		{
			if (!merch)
				continue;
			
			ADM_MerchandiseType merchType = merch.GetType();
			if (!merchType)
				continue;
			
			CreateCartListboxItem(listboxSell, merch, false);
			
			foreach(ADM_PaymentMethodBase paymentMethod : merch.GetSellPayment())
			{
				bool didAddToTotal = false;
				foreach(ADM_PaymentMethodBase existing : totalCost)
				{
					if (didAddToTotal)
					{
						continue;
					}
					
					bool isSame = existing.Equals(paymentMethod);
					didAddToTotal = existing.Add(paymentMethod, quantity*-1);
				}
				
				if (!didAddToTotal)
				{
					ADM_PaymentMethodBase clone = ADM_PaymentMethodBase.Cast(paymentMethod.Clone());
					int idx = totalCost.Insert(clone);
					clone.Add(paymentMethod, (quantity+1)*-1);
				}
			}
		}
		
		Widget totalCostOverlay = wTabView.m_wTab.FindAnyWidget("TotalCostOverlay");
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
						wItemPrice.SetTextFormat("$%1", Math.AbsInt(quantity));
						
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
					barterItemIcon.SetPayment(totalCost[i]);
				}
			}
		}
		
		TextWidget emptyText = TextWidget.Cast(wTabView.m_wTab.FindAnyWidget("Empty Text"));
		if (m_BuyShoppingCart.Count() == 0 && m_SellShoppingCart.Count() == 0)
		{
			emptyText.SetVisible(true);
			emptyText.SetText("No items in shopping cart.");
			emptyText.Update();
		} else {
			emptyText.SetVisible(false);
			emptyText.Update();
		}
	}
	
	protected void Checkout(SCR_InputButtonComponent btn)
	{
		PlayerController playerController = GetGame().GetPlayerController();
		if (!playerController)
			return;
		
		ADM_PlayerShopManagerComponent playerShopManager = ADM_PlayerShopManagerComponent.Cast(playerController.FindComponent(ADM_PlayerShopManagerComponent));
		if (!playerShopManager) 
			return;
		
		playerShopManager.AskProcessCart(m_Shop, m_BuyShoppingCart, m_SellShoppingCart);
		
		Close();
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
				ADM_MerchandisePrefab prefabMerch = ADM_MerchandisePrefab.Cast(merch.GetType());
				if (prefabMerch && validItems.Contains(prefabMerch.GetPrefab())) {
					sortedItems.Insert(merch);
				}
			}
		}
		
		if (search != string.Empty)
		{
			sortedItems = ProcessSearch(search, sortedItems);
		}
			
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
	
	protected void CreateListboxItem(SCR_ListBoxComponent listbox, ADM_ShopMerchandise merch, bool buyOrSell = true)
	{			
		ADM_MerchandiseType merchType = merch.GetType();
		if (!merchType)
			return;
		
		// Item Name
		string itemName = merchType.GetDisplayName();
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
			m_ItemPreviewManager.SetPreviewItemFromPrefab(wItemPreview, merchType.GetDisplayEntity(), null, false);
		}
		
		// On click
		ADM_ShopUI_Item mainBtn = ADM_ShopUI_Item.Cast(lbItem.GetRootWidget().FindHandler(ADM_ShopUI_Item));
		if (mainBtn)
		{
			mainBtn.m_OnClicked.Insert(AddToCart);
		}
			
		// Cost
		TextWidget wItemPrice = TextWidget.Cast(lbItem.GetRootWidget().FindAnyWidget("Cost"));
		array<ref ADM_PaymentMethodBase> paymentRequirement = merch.GetBuyPayment();
		bool isOnlyCurrency = ADM_ShopComponent.IsBuyPaymentOnlyCurrency(merch);
		if (!buyOrSell)
		{
			paymentRequirement = merch.GetSellPayment(); 
			isOnlyCurrency = ADM_ShopComponent.IsSellPaymentOnlyCurrency(merch);
		}
		
		if (isOnlyCurrency || paymentRequirement.Count() == 0)
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
			item.SetShopUI(this);
			item.SetMerchandise(merch);
			item.SetBuyOrSell(buyOrSell);
			item.SetMaxQuantity(merch.GetType().GetMaxPurchaseQuantity());
			item.SetQuantity(1);
		}
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
			
			CreateListboxItem(listbox, merch, buyOrSell);
		}
		
		TextWidget emptyText = TextWidget.Cast(wTabView.m_wTab.FindAnyWidget("Empty Text"));
		if (merchandise.Count() == 0)
		{
			emptyText.SetVisible(true);
			emptyText.SetText("No items available.");
			emptyText.Update();
		} else {
			emptyText.SetVisible(false);
			emptyText.Update();
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
			string itemName = merch.GetType().GetDisplayName();
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
		
		Widget checkoutWidget = ButtonWidget.Cast(m_wRoot.FindAnyWidget("Checkout"));
		if (checkoutWidget)
		{
			SCR_InputButtonComponent checkoutButton = SCR_InputButtonComponent.Cast(checkoutWidget.FindHandler(SCR_InputButtonComponent));
			if (checkoutButton)
			{
				checkoutButton.m_OnActivated.Insert(Checkout);
			}
		}
		
		ConfigureTabs();
		UpdateMoneyText();
		PopulateCartTab(m_wCartTabView);
		
		ChimeraWorld world = ChimeraWorld.CastFrom(GetGame().GetWorld());
		if (world)
		{
			m_ItemPreviewManager = world.GetItemPreviewManager();
		}
	}
}