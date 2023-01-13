class ADM_PaymentMethodBase: ScriptAndConfig
{
	bool CheckPayment(IEntity player) { return false; }
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	bool CollectPayment(IEntity player) { return false; }
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	bool ReturnPayment(IEntity player) { return false; }
}