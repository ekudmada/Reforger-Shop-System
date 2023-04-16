class ADM_ShopAction : ScriptedUserAction
{
	protected IEntity m_Owner;
	protected ADM_ShopComponent m_Shop;
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent) 
	{
		m_Owner = pOwnerEntity;
		m_Shop = ADM_ShopComponent.Cast(m_Owner.FindComponent(ADM_ShopComponent));
		if (!m_Shop) 
		{
			Print("Error! Could not find shop component for this action.", LogLevel.ERROR);
			return;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{	
		if (!m_Shop || m_Shop.GetMerchandise().Count() <= 0) return;
		
		SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		if (pUserEntity != playerController.GetMainEntity()) return;
		
		MenuBase menuBase = GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.ADM_Shop); 
		ADM_ShopUI menu = ADM_ShopUI.Cast(menuBase);
		menu.SetShop(m_Shop);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (!m_Shop || m_Shop.GetMerchandise().Count() <= 0) return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (!m_Shop || m_Shop.GetMerchandise().Count() <= 0) return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = "View Shop";
		
		return true;
	}
}