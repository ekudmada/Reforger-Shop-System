class ADM_CurrencyHUD: SCR_InfoDisplay
{
	private TextWidget m_Text;
	private IEntity m_Player;
	private SCR_InventoryStorageManagerComponent m_Inventory;

	override event void OnStartDraw(IEntity owner)
	{
		super.OnStartDraw(owner);
		
		if (!m_Text) m_Text = TextWidget.Cast(m_wRoot.FindAnyWidget("Text0"));
		if (!m_Player) m_Player = SCR_PlayerController.Cast(GetGame().GetPlayerController()).GetMainEntity();
		if (!m_Inventory) m_Inventory = SCR_InventoryStorageManagerComponent.Cast(m_Player.FindComponent(SCR_InventoryStorageManagerComponent));
	}
	
	private float m_lastUpdateTime = -1;
	private float m_updateDt = 1000;
	override event void UpdateValues(IEntity owner, float timeSlice)
	{
		float dt = System.GetTickCount() - m_lastUpdateTime;
		if (dt > m_updateDt)
		{
			m_lastUpdateTime = System.GetTickCount();
			if (!m_Player) m_Player = SCR_PlayerController.Cast(GetGame().GetPlayerController()).GetMainEntity();
			if (!m_Inventory) m_Inventory = SCR_InventoryStorageManagerComponent.Cast(m_Player.FindComponent(SCR_InventoryStorageManagerComponent));
			if (!m_Inventory) return;
			
			int totalCurrency = ADM_CurrencyComponent.FindTotalCurrencyInInventory(m_Inventory);
			m_Text.SetText(string.Format("$%1", totalCurrency));
		}
	}	
}