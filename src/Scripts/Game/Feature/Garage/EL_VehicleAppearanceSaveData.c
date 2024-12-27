[EPF_ComponentSaveDataType(EL_VehicleAppearanceComponent), BaseContainerProps()]
class EPF_VehicleAppearanceSaveDataClass : EPF_ComponentSaveDataClass
{
}

[EDF_DbName.Automatic()]
class EPF_VehicleAppearanceSaveData : EPF_ComponentSaveData
{
    protected int m_iVehicleColor = -1;
    protected ResourceName m_VehicleTexture;

    //------------------------------------------------------------------------------------------------
    override EPF_EReadResult ReadFrom(IEntity owner, GenericComponent component, EPF_ComponentSaveDataClass attributes)
    {
        EL_VehicleAppearanceComponent vehicleAppearance = EL_VehicleAppearanceComponent.Cast(component);
        m_iVehicleColor = vehicleAppearance.GetVehicleColor();
        m_VehicleTexture = vehicleAppearance.GetVehicleTexture();

        return EPF_EReadResult.OK;
    }

    //------------------------------------------------------------------------------------------------
    override EPF_EApplyResult ApplyTo(IEntity owner, GenericComponent component, EPF_ComponentSaveDataClass attributes)
    {
        EL_VehicleAppearanceComponent vehicleAppearance = EL_VehicleAppearanceComponent.Cast(component);
        vehicleAppearance.SetVehicleColor(m_iVehicleColor);
        vehicleAppearance.SetVehicleTexture(m_VehicleTexture);

        return EPF_EApplyResult.OK;
    }

    //------------------------------------------------------------------------------------------------
    override bool Equals(notnull EPF_ComponentSaveData other)
    {
        EPF_VehicleAppearanceSaveData otherData = EPF_VehicleAppearanceSaveData.Cast(other);
        return m_iVehicleColor == otherData.m_iVehicleColor && m_VehicleTexture == otherData.m_VehicleTexture;
    }
}

