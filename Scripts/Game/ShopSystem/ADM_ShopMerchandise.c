//TODO: setup as config so you can drag & drop
[BaseContainerProps()]
class ADM_ShopMerchandise: ScriptAndConfig
{
	[Attribute(defvalue: "", desc: "Merchandise to sell", uiwidget: UIWidgets.Object, params: "et", category: "Physical Shop")]
	protected ref ADM_MerchandiseType m_Merchandise;
	
	[Attribute(category: "Physical Shop")]
	protected ref array<ref ADM_PaymentMethodBase> m_RequiredPayment;
	
	ADM_MerchandiseType GetMerchandise() { return m_Merchandise; }
	array<ref ADM_PaymentMethodBase> GetRequiredPaymentToBuy() { return m_RequiredPayment; }
	int GetQuantityCanBuy() { return 1; }
	
	//! Takes snapshot and encodes it into packet using as few bits as possible.
	static void Encode(SSnapSerializerBase snapshot, ScriptCtx ctx, ScriptBitSerializer packet);

	//! Takes packet and decodes it into snapshot. Returns true on success or false when an error occurs.
	static bool Decode(ScriptBitSerializer packet, ScriptCtx ctx, SSnapSerializerBase snapshot);

	//! Compares two snapshots. Returns true when they match or false otherwise.
	static bool SnapCompare(SSnapSerializerBase lhs, SSnapSerializerBase rhs, ScriptCtx ctx);

	//! Compares instance against snapshot. Returns true when they match or false otherwise.
	static bool PropCompare(ADM_ShopMerchandise instance, SSnapSerializerBase snapshot, ScriptCtx ctx);

	//! Writes data from an instance into snapshot. Opposite of Inject().
	static bool Extract(ADM_ShopMerchandise instance, ScriptCtx ctx, SSnapSerializerBase snapshot);

	//! Writes data from snapshot into instance. Opposite of Extract().
	static bool Inject(SSnapSerializerBase snapshot, ScriptCtx ctx, ADM_ShopMerchandise instance);
}