var Constructor = function()
{
    this.getOffensiveBonus = function(co, attacker, atkPosX, atkPosY,
                                 defender, defPosX, defPosY, isDefender, action, luckmode, map)
    {
		if (CO_PERK.isActive(co))
		{
			if (map !== null)
			{
				if (map.onMap(atkPosX, atkPosY))
				{
					var terrainID = map.getTerrain(atkPosX, atkPosY).getID();
                    var isForest = 	(terrainID === "FOREST") ||
                                    (terrainID === "FOREST1") ||
                                    (terrainID === "FOREST2") ||
                                    (terrainID === "FOREST3") ||
									(terrainID === "DESERT_FOREST") ||
                                    (terrainID === "DESERT_FOREST1") ||
                                    (terrainID === "SNOW_FOREST") ||
                                    (terrainID === "SNOW_FOREST1") ||
                                    (terrainID === "SNOW_FOREST2");
                    if (isForest)
                    {
                        return 10;
                    }
				}
			}
		}
        return 0;
    };
	// Perk - Intel
    this.getDescription = function()
    {
        return qsTr("Increases the attack from woods by 10%.");
    };
    this.getIcon = function(map)
    {
        return "ranger";
    };
    this.getName = function()
    {
        return qsTr("Ranger");
    };
    this.getGroup = function()
    {
        return qsTr("Terrain");
    };
};

Constructor.prototype = CO_PERK;
var CO_PERK_RANGER = new Constructor();
