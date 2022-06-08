[EntityEditorProps(style: "box", category: "EveronLife/Utils", description: "Region for spawning", color: "251 91 0 255", dynamicBox: true)]
class EL_AreaSpawnerComponentClass : ScriptComponentClass 
{
}

class EL_AreaSpawnerComponent : ScriptComponent
{
	
	[Attribute("1 1 1", desc: "Size of the spawning area", category: "Spawn Area")]
	private vector m_BoxSize;
	
	[Attribute("0 0 0", desc: "Size of the spawning area", category: "Spawn Area", params: "inf inf 0 purposeCoords spaceEntity")]
	private vector m_BoxOffset;
	
	[Attribute(defvalue: "", uiwidget: UIWidgets.ResourcePickerThumbnail, desc: "Prefab to Spawn", "et", category: "Spawn Area")]
	private ResourceName m_PrefabToSpawn;	
	
	[Attribute(defvalue: "1", uiwidget: UIWidgets.Auto, desc: "Prefab to Spawn", "et", category: "Spawn Area")]
	private int m_AmountToSpawn;
	
	[Attribute("false", UIWidgets.CheckBox, "Spawn all on init?", category: "Spawn Area")]
	private bool m_SpawnOnInit;
	
	//------------------------------------------------------------------------------------------------
	void SpawnPrefabs(IEntity parent) 
	{
		for (int i = 0; i < m_AmountToSpawn; i++) 
		{
			EL_Utils.SpawnEntityPrefab(m_PrefabToSpawn, GetRandomInArea(), vector.Zero, parent);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! EntityEditorProps size is diameter -> divide by 2
	private vector GetRandomInArea()
	{
		float rndX = Math.RandomFloatInclusive(-m_BoxSize[0], m_BoxSize[0]);
		float rndY = Math.RandomFloatInclusive(-m_BoxSize[1], m_BoxSize[1]);
		float rndZ = Math.RandomFloatInclusive(-m_BoxSize[2], m_BoxSize[2]);
		
		vector rndPos = Vector(rndX, rndY, rndZ);
		return GetOwner().CoordToParent(rndPos + m_BoxOffset);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.INIT | EntityEvent.FRAME);
		owner.SetFlags(EntityFlags.ACTIVE, true);
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		if (!m_PrefabToSpawn)
			return;
		
		if (m_SpawnOnInit)
			SpawnPrefabs(owner);
		
	}
}
