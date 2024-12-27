[EPF_ComponentSaveDataType(EL_CharacterOwnerSaveDataClass), BaseContainerProps()]
class EL_CharacterOwnerSaveDataClass : EPF_ComponentSaveDataClass
{
    // You can add more properties or settings if needed
};

[EDF_DbName.Automatic()]
class EL_CharacterOwnerSaveData : EPF_ComponentSaveData
{
    protected string m_sCharacterId;

    //------------------------------------------------------------------------------------------------
    bool ReadFrom(notnull GenericComponent worldEntityComponent)
    {
        EL_CharacterOwnerComponent charOwner = EL_CharacterOwnerComponent.Cast(worldEntityComponent);
        m_sCharacterId = charOwner.GetCharacterOwner();

        return true;
    }

    //------------------------------------------------------------------------------------------------
    bool ApplyTo(notnull GenericComponent worldEntityComponent)
    {
        EL_CharacterOwnerComponent charOwner = EL_CharacterOwnerComponent.Cast(worldEntityComponent);
        charOwner.SetCharacterOwner(m_sCharacterId);

        return true;
    }
}
