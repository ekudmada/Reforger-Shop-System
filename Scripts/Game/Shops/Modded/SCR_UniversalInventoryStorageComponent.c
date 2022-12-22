modded class SCR_UniversalInventoryStorageComponent
{
	float GetWeight()
	{
		return m_fWeight;
	}
	
	float GetMaxWeight()
	{
		return m_fMaxWeight;
	}
	
	float GetVolume2()
	{
		return m_fVolume;
	}
	
	float GetMaxVolume()
	{
		return GetMaxVolumeCapacity();
	}
}