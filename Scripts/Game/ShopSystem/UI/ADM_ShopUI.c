class ADM_ShopQuantityButton : SCR_ButtonComponent
{
	[Attribute()]
	protected int m_iAmount;
	
	int GetAmount()
	{
		return m_iAmount;
	}
}

class ADM_ShopUIBarterItemIcon : ScriptedWidgetComponent
{
	protected SCR_HoverDetectorComponent m_HoverDetector;
	protected ref ADM_PaymentMethodBase m_PaymentMethod;
	protected ItemPreviewManagerEntity m_PreviewManager;
	protected Widget m_wRoot;
	
	//---------------------------------------------------------------------------------------------------
	override void HandlerAttached(Widget w)
	{
		m_wRoot = w;
		m_PreviewManager = GetGame().GetItemPreviewManager();
		m_HoverDetector = SCR_HoverDetectorComponent.Cast(w.FindHandler(SCR_HoverDetectorComponent));		
		m_HoverDetector.m_OnHoverDetected.Insert(OnHoverDetected);
		m_HoverDetector.m_OnMouseLeave.Insert(OnMouseLeaveTooltip);
	}
	
	void SetPayment(ADM_PaymentMethodBase payment)
	{
		m_PaymentMethod = payment;
	}
	
	//---------------------------------------------------------------------------------------------------
	IEntity m_iPreviewEntity;
	void OnHoverDetected()
	{
		Widget w = SCR_TooltipManagerEntity.CreateTooltip("{459F0B580A8E2CD7}UI/Layouts/ShopSystem/BarterItemTooltip.layout", m_wRoot);
		TextWidget wText = TextWidget.Cast(w.FindAnyWidget("Text"));
		ItemPreviewWidget wRenderTarget = ItemPreviewWidget.Cast(w.FindWidget("Overlay.VerticalLayout0.SizeLayout0.PreviewImage"));
		
		if (m_PaymentMethod) {
			int quantity = ADM_ShopUI.FindShopUIItem(m_wRoot).GetQuantity();
			string displayString = m_PaymentMethod.GetDisplayString(quantity);
			wText.SetText(displayString);
			
			m_iPreviewEntity = GetGame().SpawnEntityPrefabLocal(Resource.Load(m_PaymentMethod.GetDisplayEntity()), null, null);
			if (wRenderTarget && m_PreviewManager && m_iPreviewEntity) m_PreviewManager.SetPreviewItem(wRenderTarget, m_iPreviewEntity, null, true);
		}
	}
	
	void OnMouseLeaveTooltip()
	{
		if (m_iPreviewEntity)
			SCR_EntityHelper.DeleteEntityAndChildren(m_iPreviewEntity);
	}
}

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

class ADM_ShopUIItem : SCR_ScriptedWidgetComponent
{
	[Attribute(defvalue: "1")]
	protected int m_quantity = 1;

	protected ADM_ShopComponent m_Shop;
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
		
		TextWidget quantity = TextWidget.Cast(w.FindWidget("Row.QuantityContainer.HorizontalLayout0.Quantity"));
		if (quantity) quantity.SetTextFormat("%1", m_quantity);
	
		return true;
	}
}

class ADM_ShopUI: ChimeraMenuBase
{
	static protected ResourceName m_ItemPrefab = "{7C575A0E989A1B9D}UI/Layouts/ShopSystem/MarketItem.layout";
	static protected ResourceName m_CategoryPrefab = "{5429F3944440C60B}UI/Layouts/ShopSystem/MarketCategory.layout";
	static protected ResourceName m_BarterIconPrefab = "{BBEF0EBB9F35B19F}UI/Layouts/ShopSystem/MarketBarterItemIcon.layout";
	
	protected ref array<IEntity> m_PreviewEntities = new array<IEntity>();
	protected ItemPreviewManagerEntity m_PreviewManager;
	protected WorkspaceWidget m_wWorkspace;
	protected Widget m_wRoot;
	protected Widget m_wCategoryParent;
	protected Widget m_wContentParent;
	protected SCR_NavigationButtonComponent m_closeButton;
	
	protected ADM_ShopComponent m_Shop;
	
	void SetShop(ADM_ShopComponent shop)
	{
		m_Shop = shop;
		Reset();
		
		PrintFormat("set shop to %1", shop);
	}
	
	void ClearCategories()
	{
		// Remove all children from the categories list
		while (m_wCategoryParent.GetChildren() != null)
		{
			delete m_wCategoryParent.GetChildren();
		}
	}
	
	void CreateCategory(ADM_ShopCategory category, int index)
	{
		Widget wCategory = m_wWorkspace.CreateWidgets(m_CategoryPrefab, m_wCategoryParent);
		TextWidget wName = TextWidget.Cast(wCategory.FindWidget("Text0"));
		wName.SetText(category.m_DisplayName);
		
		SCR_ButtonComponent bCategory = SCR_ButtonComponent.Cast(wCategory.FindHandler(SCR_ButtonComponent));
		if (bCategory)
		{
			bCategory.m_OnClicked.Clear();
			bCategory.m_OnClicked.Insert(SelectCategoryButton);
		}
		
		ADM_ShopUICategory shopCategory = ADM_ShopUICategory.Cast(wCategory.FindHandler(ADM_ShopUICategory));
		if (shopCategory)
		{
			shopCategory.SetIndex(index);
		}
	}
	
	void PopulateCategories()
	{
		if (!m_Shop) return;
		ClearCategories();
		
		ADM_ShopCategory allCategory = new ADM_ShopCategory();
		allCategory.m_DisplayName = "All";
		CreateCategory(allCategory, -1);
		
		array<ref ADM_ShopCategory> categories = m_Shop.GetCategories();
		foreach (int i, ADM_ShopCategory category : categories)
		{
			CreateCategory(category, i);
		}
	}
	
	void ClearContent()
	{
		// Remove all children from the content list
		while (m_wContentParent.GetChildren() != null)
		{

			delete m_wContentParent.GetChildren();
			
			foreach (IEntity previewEntity : m_PreviewEntities)
			{
				delete previewEntity;
			}
			m_PreviewEntities.Clear();
		}
	}

	static ADM_ShopUIItem FindShopUIItem(Widget w)
	{
		Widget parent = w.GetParent();
		while (parent)
		{
			ADM_ShopUIItem uiItem = ADM_ShopUIItem.Cast(parent.FindHandler(ADM_ShopUIItem));
			if (uiItem)
				return uiItem;
			else
				parent = parent.GetParent();
		}
		
		return null;
	}
	
	void PurchaseItem(SCR_ButtonComponent button)
	{
		ADM_ShopUIItem uiItem = FindShopUIItem(button.GetRootWidget());
		if (uiItem)
		{
			PlayerController playerController = GetGame().GetPlayerController();
			ADM_PlayerShopManagerComponent playerShopManager = ADM_PlayerShopManagerComponent.Cast(playerController.FindComponent(ADM_PlayerShopManagerComponent));
			if (!playerShopManager) return;
			
			ADM_ShopMerchandise merch = uiItem.GetMerchandise();
			playerShopManager.AskPurchase(m_Shop, merch, uiItem.GetQuantity());
		}
	}
	
	void UpdateItemQuantity(ADM_ShopQuantityButton button)
	{
		ADM_ShopUIItem uiItem = FindShopUIItem(button.GetRootWidget());
		if (uiItem) uiItem.UpdateQuantity(button.GetAmount());
	}
		
	void SelectCategoryButton(SCR_ButtonComponent button)
	{
		ADM_ShopUICategory shopCategory = ADM_ShopUICategory.Cast(button.GetRootWidget().FindHandler(ADM_ShopUICategory));
		if (shopCategory)
		{
			SelectCategory(shopCategory.GetIndex());
		}
	}
	
	void CreateRow(ADM_ShopMerchandise merch)
	{
		ADM_MerchandiseType merchType = merch.GetMerchandise();
		Widget newRow = m_wWorkspace.CreateWidgets(m_ItemPrefab, m_wContentParent);
		
		IEntity previewEntity = GetGame().SpawnEntityPrefabLocal(Resource.Load(merchType.GetPrefab()), null, null);
		m_PreviewEntities.Insert(previewEntity);
		
		ItemPreviewWidget wRenderTarget = ItemPreviewWidget.Cast(newRow.FindWidget("Row.Item.Offer.Preview Image"));
		if (wRenderTarget && m_PreviewManager) m_PreviewManager.SetPreviewItem(wRenderTarget, previewEntity, null, true);
		
		TextWidget wItemName = TextWidget.Cast(newRow.FindWidget("Row.Item.Offer.Information Container.Name"));
		TextWidget wItemDescription = TextWidget.Cast(newRow.FindWidget("Row.Item.Offer.Information Container.Description"));
		
		string itemName = ADM_Utils.GetPrefabDisplayName(merchType.GetPrefab());
		string description = ADM_Utils.GetPrefabDescription(merchType.GetPrefab());
		
		if (wItemName) wItemName.SetTextFormat(itemName);
		if (wItemDescription) wItemDescription.SetTextFormat(description);
		
		TextWidget wItemPrice = TextWidget.Cast(newRow.FindWidget("Row.Price.HorizontalLayout0.Price"));
		array<ref ADM_PaymentMethodBase> paymentRequirement = merch.GetRequiredPayment();
		if (ADM_ShopComponent.IsPaymentOnlyCurrency(merch) || paymentRequirement.Count() == 0)
		{
			if (wItemPrice) {
				wItemPrice.SetVisible(true);
				if (paymentRequirement.Count() == 0)
				{
					wItemPrice.SetTextFormat("Free");
				} else {
					ADM_PaymentMethodCurrency paymentMethod = ADM_PaymentMethodCurrency.Cast(paymentRequirement[0]);
					wItemPrice.SetTextFormat("$%1", paymentMethod.GetQuantity());
				}
			}
		} else {
			Widget priceParent = newRow.FindWidget("Row.Price.HorizontalLayout0");
			if (wItemPrice) wItemPrice.SetVisible(false);
			
			int numIcons = Math.Clamp(paymentRequirement.Count(), 0, 5);
			for (int i = 0; i < numIcons; i++)
			{
				Widget icon = m_wWorkspace.CreateWidgets(m_BarterIconPrefab, priceParent);
				ADM_ShopUIBarterItemIcon barterItemIcon = ADM_ShopUIBarterItemIcon.Cast(icon.FindHandler(ADM_ShopUIBarterItemIcon));
				if (barterItemIcon)
				{
					barterItemIcon.SetPayment(paymentRequirement[i]);
				}
			}
		}
		
		ADM_ShopUIItem item = ADM_ShopUIItem.Cast(newRow.FindHandler(ADM_ShopUIItem));
		if (item) {
			item.SetQuantity(1);
			item.SetMerchandise(merch);
		}
			
		Widget lessBtnWidget = newRow.FindWidget("Row.QuantityContainer.HorizontalLayout0.Less");
		Widget moreBtnWidget = newRow.FindWidget("Row.QuantityContainer.HorizontalLayout0.More");
		
		if (!merch.GetMerchandise().CanPurchaseMultiple()) {
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
		
		Widget purchaseBtnWidget = newRow.FindWidget("Row.Purchase.PurchaseButton");
		SCR_ButtonComponent purchaseButton = SCR_ButtonComponent.Cast(purchaseBtnWidget.FindHandler(SCR_ButtonComponent));
		if (purchaseButton)
		{
			purchaseButton.m_OnClicked.Clear();
			purchaseButton.m_OnClicked.Insert(PurchaseItem);
		}
	}
	
	int m_iCurrentCategory = -1;
	void SelectCategory(int index = 0)
	{
		if (!m_Shop) return;
		ClearContent();

		array<ref ADM_ShopCategory> categories = m_Shop.GetCategories();
		array<ref ADM_ShopMerchandise> items = m_Shop.GetMerchandise();
		array<ref ADM_ShopMerchandise> sortedItems = {};
		array<ResourceName> validItems = {};
		
		// All category
		if (index == -1) 
		{
			sortedItems = items;
		} else {
			if (categories[index])
				validItems = categories[index].GetItems();
			
			foreach (ADM_ShopMerchandise merch : items)
			{
				if (validItems.Contains(merch.GetMerchandise().GetPrefab())) {
					sortedItems.Insert(merch);
				}
			}
		}
		
		foreach (ADM_ShopMerchandise merch : sortedItems)
		{
			CreateRow(merch);
		}
		
		m_iCurrentCategory = index;
	}
	
	void Reset()
	{
		PopulateCategories();
		SelectCategory(-1);
	}
	
	void ProcessSearch(SCR_EditBoxSearchComponent searchbox, string search)
	{
		search = search.Trim();
		search.ToLower();
		
		if (search.IsEmpty())
		{
			SelectCategory(m_iCurrentCategory);
			return;
		}
		
		ClearContent();
		
		array<ADM_ShopMerchandise> matchedMerchandise = {};
		foreach (ADM_ShopMerchandise merch : m_Shop.GetMerchandise())
		{
			string itemName = ADM_Utils.GetPrefabDisplayName(merch.GetMerchandise().GetPrefab());
			itemName.ToLower();
			
			if (itemName.Contains(search))
			{
				matchedMerchandise.Insert(merch);
			}
		}
		
		foreach (ADM_ShopMerchandise merch : matchedMerchandise)
		{
			CreateRow(merch);
		}
	}
	
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
	}
	
	override void OnMenuOpened() 
	{
		super.OnMenuOpened();
	}
	
	override void OnMenuClose() 
	{
		super.OnMenuClose();
		ClearContent();
		ClearCategories();
	}
	
	override void OnMenuInit() 
	{
		super.OnMenuInit();
		
		m_PreviewManager = GetGame().GetItemPreviewManager();
		m_wWorkspace = GetGame().GetWorkspace();
		m_wRoot = GetRootWidget();
		m_wCategoryParent = m_wRoot.FindAnyWidget("CategoriesContainer");
		m_wContentParent = m_wRoot.FindAnyWidget("ContentContainer");
        m_closeButton = SCR_NavigationButtonComponent.GetNavigationButtonComponent("Close", m_wRoot);
		
		if (!m_wRoot) {
			Print("ADM_ShopUI: Couldn't find m_wRoot widget!", LogLevel.ERROR);
			return;
		}
		
		if (!m_wCategoryParent) {
			Print("ADM_ShopUI: Couldn't find m_wCategoryParent widget!", LogLevel.ERROR);
			return;
		}
		
		if (!m_wContentParent) {
			Print("ADM_ShopUI: Couldn't find m_wContentParent widget!", LogLevel.ERROR);
			return;
		}
	
#ifdef WORKBENCH
		GetGame().GetInputManager().AddActionListener("MenuBackWB", EActionTrigger.DOWN, Close);
#else
		GetGame().GetInputManager().AddActionListener("MenuBack", EActionTrigger.DOWN, Close);
#endif
		
		if (m_closeButton)
		{
			m_closeButton.m_OnClicked.Clear();
			m_closeButton.m_OnClicked.Insert(Close);
		}
		
		SCR_EditBoxSearchComponent search = SCR_EditBoxSearchComponent.Cast(SCR_EditBoxSearchComponent.GetEditBoxComponent("Searchbar", m_wRoot));
		if (search)
		{
			if (search.m_OnConfirm)
				search.m_OnConfirm.Insert(ProcessSearch);
		}
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