class ADM_WeaponShopComponentClass: ScriptComponentClass {}
class ADM_WeaponShopComponent: ADM_ShopBaseComponent
{
	[Attribute(uiwidget: UIWidgets.Object)]
	protected ref array<IEntity> m_WeaponShops;
}