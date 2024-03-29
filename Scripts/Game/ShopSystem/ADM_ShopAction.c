class ADM_ShopAction : ScriptedUserAction
{
	protected ADM_ShopComponent m_Shop;
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent) 
	{
		m_Shop = ADM_ShopComponent.Cast(pOwnerEntity.FindComponent(ADM_ShopComponent));
		if (!m_Shop) 
		{
			Print("Error! Could not find shop component for this action.", LogLevel.ERROR);
			return;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{	
		if (!m_Shop || m_Shop.GetMerchandiseBuy().Count() <= 0) return;
		
		SCR_PlayerController scrPlayerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		if (!scrPlayerController || pUserEntity != scrPlayerController.GetMainEntity()) 
			return;
		
		MenuBase menuBase = GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.ADM_ShopMenu); 
		ADM_ShopUI menu = ADM_ShopUI.Cast(menuBase);
		menu.SetShop(m_Shop);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (!m_Shop || !m_Shop.GetMerchandiseAll() || m_Shop.GetMerchandiseAll().Count() <= 0) 
			return false;
			
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (!m_Shop || m_Shop.GetMerchandiseAll().Count() <= 0) 
			return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = "View Shop";
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool HasLocalEffectOnlyScript()
	{
		return true;
	}
	
	//----------------------------------------------------------------------------------
	override bool CanBroadcastScript() 
	{ 
		return false; 
	}
}