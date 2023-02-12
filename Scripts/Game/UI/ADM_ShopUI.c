modded enum ChimeraMenuPreset
{
	ADM_Shop
}

class ADM_ShopUI: ChimeraMenuBase
{
	static protected ResourceName m_ItemPrefab = "{7C575A0E989A1B9E}UI/Layouts/ShopSystem/MarketItem.layout";
	static protected ResourceName m_CategoryPrefab = "{5429F3944440C60B}UI/Layouts/ShopSystem/MarketCategory.layout";
	static protected ResourceName m_NonCurrencyPaymentIconPrefab = "{4BCF093733E02270}UI/Layouts/ShopSystem/MarketNonCurrencyIcon.layout";
	
	protected ref array<IEntity> m_PreviewEntities = new array<IEntity>();
	protected ItemPreviewManagerEntity m_PreviewManager;
	protected WorkspaceWidget m_wWorkspace;
	protected Widget m_wRoot;
	protected Widget m_wConfirmation;
	protected Widget m_wCategoryParent;
	protected Widget m_wContentParent;
	
	protected ADM_ShopComponent m_Shop;
	
	void SetShop(ADM_ShopComponent shop)
	{
		m_Shop = shop;
		Reset();
	}
	
	void ClearCategories()
	{
		// Remove all children from the categories list
		while (m_wCategoryParent.GetChildren() != null)
		{
			delete m_wCategoryParent.GetChildren();
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

	void SelectCategory(int index = 0)
	{
		// Clear main content panel
		ClearContent();
		
		//Load items for category
		if (!m_Shop) return;
		foreach (ADM_ShopMerchandise merch : m_Shop.GetMerchandise())
		{
			array<ref ADM_PaymentMethodBase> paymentRequirement = merch.GetRequiredPaymentToBuy();
			
			ADM_MerchandiseType merchType = merch.GetMerchandise();
			Widget newRow = m_wWorkspace.CreateWidgets(m_ItemPrefab, m_wContentParent);
			
			IEntity previewEntity = GetGame().SpawnEntityPrefabLocal(Resource.Load(merchType.GetPrefab()), null, null);
			m_PreviewEntities.Insert(previewEntity);
			
			ItemPreviewWidget wRenderTarget = ItemPreviewWidget.Cast(newRow.FindWidget("Item.Offer.Preview Image"));
			if (wRenderTarget && m_PreviewManager) m_PreviewManager.SetPreviewItem(wRenderTarget, previewEntity, null, true);
			
			TextWidget wItemName = TextWidget.Cast(newRow.FindWidget("Item.Offer.Information Container.Name"));
			TextWidget wItemDescription = TextWidget.Cast(newRow.FindWidget("Item.Offer.Information Container.Description"));
			
			UIInfo itemUI = ADM_Utils.GetItemUIInfo(merchType.GetPrefab());
			string itemName = itemUI.GetName();
			string description = itemUI.GetDescription();
			
			if (wItemName) wItemName.SetTextFormat(itemName);
			if (wItemDescription) wItemDescription.SetTextFormat(description);
			
			TextWidget wItemPrice = TextWidget.Cast(newRow.FindWidget("Price.HorizontalLayout0.Text0"));
			if (ADM_ShopComponent.IsPaymentOnlyCurrency(merch) || paymentRequirement.Count() == 0)
			{
				wItemPrice.SetVisible(true);
				if (paymentRequirement.Count() == 0)
				{
					wItemPrice.SetTextFormat("Free");
				} else {
					ADM_PaymentMethodCurrency paymentMethod = ADM_PaymentMethodCurrency.Cast(paymentRequirement[0]);
					wItemPrice.SetTextFormat("$%1", paymentMethod.GetQuantity());
				}
			} else {
				Widget priceParent = newRow.FindWidget("Price.HorizontalLayout0");
				wItemPrice.SetVisible(false);
				
				int numIcons = Math.Clamp(paymentRequirement.Count(), 0, 5);
				for (int i = 0; i < numIcons; i++)
				{
					Widget icon = m_wWorkspace.CreateWidgets(m_NonCurrencyPaymentIconPrefab, priceParent);
				}
			}
		}
	}
	
	void Reset()
	{
		ClearCategories();
		ClearContent();
		SelectCategory(0);
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
		m_wConfirmation = m_wRoot.FindWidget("Confirmation");
		m_wCategoryParent = m_wRoot.FindAnyWidget("CategoriesContainer");
		m_wContentParent = m_wRoot.FindAnyWidget("ContentContainer");
		
		if (!m_wRoot) {
			Print("ADM_ShopUI: Couldn't find m_wRoot widget!", LogLevel.ERROR);
			return;
		}
		
		if (!m_wConfirmation) {
			Print("ADM_ShopUI: Couldn't find m_wConfirmation widget!", LogLevel.ERROR);
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