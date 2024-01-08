class ADM_ShopUI: ChimeraMenuBase
{
	static const ResourceName m_ListBoxPrefab = "{906F7BCCC662C19D}UI/Layouts/Menus/ShopMenu/ListBoxElement_ShopItem.layout";
	
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
		if (!component || !categories || categories.Count() < 1)
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
			
			// Item Preview
			SCR_ListBoxElementComponent item = listbox.GetElementComponent(itemIdx);
			if (item)
			{
				ItemPreviewWidget wItemPreview = ItemPreviewWidget.Cast(item.GetRootWidget().FindAnyWidget("ItemPreview0"));
				if (wItemPreview)
				{
					m_ItemPreviewManager.SetPreviewItemFromPrefab(wItemPreview, merchType.GetPrefab(), null, false);
				}
			}
			
			// Cost
			
			
			// Quantity
			
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
		
		ConfigureTabs();
		
		m_wHeaderText = TextWidget.Cast(m_wRoot.FindAnyWidget("HeaderText"));
		m_wMoneyText = TextWidget.Cast(m_wRoot.FindAnyWidget("TotalMoneyText"));
		
		UpdateMoneyText();
		
		ChimeraWorld world = ChimeraWorld.CastFrom(GetGame().GetWorld());
		if (world)
		{
			m_ItemPreviewManager = world.GetItemPreviewManager();
		}
	}
	
	override void HandlerDeattached(Widget w)
	{
		super.HandlerDeattached(w);
	}
	
	override void OnMenuInit() 
	{
		super.OnMenuInit();
	}
	
	override void OnMenuUpdate(float tDelta) 
	{
		super.OnMenuUpdate(tDelta);
	}
	
	override void OnMenuClose() 
	{
		super.OnMenuClose();
	}
}