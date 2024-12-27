[EPF_ComponentSaveDataType(EL_GarageSaveDataClass), BaseContainerProps()]
class EL_GarageSaveDataClass : EPF_EntitySaveDataClass
{
    // You can add more properties or settings if needed
};

[EDF_DbName.Automatic()]
class EL_GarageSaveData : EPF_EntitySaveData
{
	protected ref map<string, ref array<string>> m_mSavedVehicles = new map<string, ref array<string>>;
	
	//------------------------------------------------------------------------------------------------
	bool ReadFrom(notnull IEntity worldEntity)
	{
		//Meta data
		EPF_PersistenceComponent persistenceComponent = EPF_PersistenceComponent.Cast(worldEntity.FindComponent(EPF_PersistenceComponent));
		SetId(persistenceComponent.GetPersistentId());
		m_iLastSaved = persistenceComponent.GetLastSaved();

		//Actual Data
		EL_GarageManagerComponent garage = EL_GarageManagerComponent.Cast(worldEntity.FindComponent(EL_GarageManagerComponent));
		m_mSavedVehicles = garage.GetAllVehicles();

		return true;
	}

	//------------------------------------------------------------------------------------------------
	bool ApplyTo(notnull IEntity worldEntity)
	{
		EL_GarageManagerComponent garage = EL_GarageManagerComponent.Cast(worldEntity.FindComponent(EL_GarageManagerComponent));
		garage.SetVehicles(m_mSavedVehicles);

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override protected bool SerializationSave(BaseSerializationSaveContext saveContext)
	{
		//Meta data
		saveContext.WriteValue("m_iDataLayoutVersion", m_iDataLayoutVersion);
		saveContext.WriteValue("m_sId", GetId());
		saveContext.WriteValue("m_iLastSaved", m_iLastSaved);

		//Actual Data
		saveContext.WriteValue("m_mSavedVehicles", m_mSavedVehicles);

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override protected bool SerializationLoad(BaseSerializationLoadContext loadContext)
	{
		//Meta data
		loadContext.ReadValue("m_iDataLayoutVersion", m_iDataLayoutVersion);
		string id;
		loadContext.ReadValue("m_sId", id);
		SetId(id);
		loadContext.ReadValue("m_iLastSaved", m_iLastSaved);

		//Actual Data
		loadContext.ReadValue("m_mSavedVehicles", m_mSavedVehicles);

		return true;
	}
}
