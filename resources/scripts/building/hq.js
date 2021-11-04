var Constructor = function()
{

    this.buildingData = [["ac", "ac"],
                         ["bd", "bd"],
                         ["bh", "bh"],
                         ["bg", "bg"],
                         ["bm", "bm"],
                         ["dm", "dm"],
                         ["ge", "ge"],
                         ["gs", "gs"],
                         ["ma", "ma"],
                         ["os", "os"],
                         ["pf", "pf"],
                         ["ti", "ti"],
                         ["yc", "yc"],];
    
    this.loadSprites = function(building, neutral)
    {
        if (building.getOwnerID() >= 0 && !neutral)
        {
            var player = building.getOwner();
            var armyName = Global.getArmyNameFromPlayerTable(player, HQ.buildingData);
            building.loadSprite("hq+" + armyName, false);
            building.loadSpriteV2("hq+" + armyName + "+mask", GameEnums.Recoloring_Matrix);
        }
        else
        {
            // neutral player
            building.loadSprite("hq+neutral", false);
        }
    };
    this.getName = function()
    {
        return qsTr("HQ");
    };
    this.armyData = [["os", "os"],
                     ["bm", "bm"],
                     ["ge", "ge"],
                     ["yc", "yc"],
                     ["bh", "bh"],
                     ["bg", "bh"],];
    this.addCaptureAnimationBuilding = function(animation, building, startPlayer, capturedPlayer)
    {
        var player = building.getOwner();
        // get army name
        var armyName = "os";
        if (player !== null)
        {
            armyName = Global.getArmyNameFromPlayerTable(player, HQ.armyData);
        }

        animation.addBuildingSprite("hq+" + armyName + "+mask", startPlayer , capturedPlayer, GameEnums.Recoloring_Matrix);
        if (armyName === "bh")
        {
            animation.addBuildingSprite("hq+" + armyName, startPlayer , capturedPlayer, GameEnums.Recoloring_None);
        }
    };
    this.getDefense = function()
    {
        return 4;
    };
    this.constructionList = ["INFANTRY", "MECH", "MOTORBIKE", "SNIPER", "RECON", "APC", "FLARE",
                             "ARTILLERY", "LIGHT_TANK", "HOVERCRAFT", "FLAK", "ARTILLERYCRAFT", "HOVERFLAK",
                             "ANTITANKCANNON", "HEAVY_TANK", "HEAVY_HOVERCRAFT", "NEOTANK",
                             "MISSILE", "ROCKETTHROWER", "MEGATANK", "PIPERUNNER"];
    this.getConstructionList = function(building)
    {
        return HQ.constructionList;
    };
    this.getMiniMapIcon = function()
    {
        return "minimap_hq";
    };

    this.getTerrainAnimationForeground = function(unit, terrain)
    {
        return "";
    };

    this.getTerrainAnimationBackground = function(unit, terrain)
    {
        var player = terrain.getBuilding().getOwner();
        // get army name
        var armyName = "";
        if (player !== null)
        {
            armyName = Global.getArmyNameFromPlayerTable(player, HQ.armyData);
        }
        var baseId = terrain.getBaseTerrainID();
        var weatherModifier = "";
        if (baseId === "DESERT")
        {
            weatherModifier= "desert";
        }
        else if (baseId === "SNOW")
        {
            weatherModifier= "snow";
        }
        else if (baseId === "WASTE")
        {
            weatherModifier= "waste";
        }
        else
        {
            weatherModifier = TERRAIN.getWeatherModifier();
        }
        return "back_" + weatherModifier + "hq+" + armyName;
    };

    this.getDescription = function()
    {
        return qsTr("<div c='#00ff00'>Battle ends </div><r>when an army's last HQ is captured. </r><div c='#00ff00'>Ground </div><r>units can </r><div c='#00ff00'>resupply.</div>");
    };

    this.getVisionHide = function(building)
    {
        return true;
    };

    this.getRepairTypes = function(building)
    {
        return [GameEnums.UnitType_Ground, GameEnums.UnitType_Hovercraft, GameEnums.UnitType_Infantry];
    };
	this.onWeatherChanged = function(building, weather)
	{	
		var weatherId = weather.getWeatherId();
		if (weatherId === "WEATHER_SNOW")
			{
				if (building.getOwnerID() >= 0)
				{
					var player = building.getOwner();
					var armyName = Global.getArmyNameFromPlayerTable(player, HQ.buildingData);
					building.loadWeatherOverlaySpriteV2("hq+" + armyName + "+snow", false);
				}
				else
				{
					building.loadWeatherOverlaySpriteV2("hq+neutral+snow", false);
				};
			};
	};
}

Constructor.prototype = BUILDING;
var HQ = new Constructor();
