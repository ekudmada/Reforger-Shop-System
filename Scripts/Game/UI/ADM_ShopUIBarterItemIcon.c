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