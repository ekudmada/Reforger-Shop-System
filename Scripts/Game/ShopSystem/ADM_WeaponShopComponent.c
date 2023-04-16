class ADM_WeaponShopComponentClass: ScriptComponentClass {}
class ADM_WeaponShopComponent: ScriptComponent
{
	[Attribute(uiwidget: UIWidgets.Object)]
	protected ref array<IEntity> m_WeaponShops;
}