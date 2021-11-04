var Constructor = function()
{
    
    this.loadSprites = function(building, neutral)
    {
        if (building.getOwnerID() >= 0 && !neutral)
        {
            // none neutral player
            building.loadSprite("temporary_harbour", false);
            building.loadSpriteV2("temporary_harbour+mask", GameEnums.Recoloring_Matrix);
        }
        else
        {
            // neutral player
            building.loadSprite("temporary_harbour+neutral", false);
        }
    };
    this.addCaptureAnimationBuilding = function(animation, building, startPlayer, capturedPlayer)
    {
        animation.addBuildingSprite("harbour+mask", startPlayer , capturedPlayer, GameEnums.Recoloring_Table);
        animation.addBuildingSprite("harbour", startPlayer , capturedPlayer, GameEnums.Recoloring_None);
    };
    this.getName = function()
    {
        return qsTr("Temporary Harbour");
    };
    this.actionList = [];
    this.getBaseIncome = function()
    {
        return 0;
    };
    this.constructionList = ["CANNONBOAT", "BLACK_BOAT", "LANDER", "CRUISER", "SUBMARINE", "DESTROYER", "BATTLESHIP", "AIRCRAFTCARRIER"];
    this.getConstructionList = function(building)
    {
        return TEMPORARY_HARBOUR.constructionList;
    };
    this.getTerrainAnimationForeground = function(unit, terrain)
    {
        return "";
    };

    this.getTerrainAnimationBackground = function(unit, terrain)
    {
        return "back_harbour";
    };
    this.getDefense = function()
    {
        return 1;
    };
    this.getDescription = function()
    {
        return qsTr("<r>Harbour made by APC. </r><div c='#00ff00'>Resupplying </div><r>of </r><div c='#00ff00'>naval </div><r>units is possible.</r>");
    };

    this.getVisionHide = function(building)
    {
        return true;
    };

    this.getRepairTypes = function(building)
    {
        return [GameEnums.UnitType_Hovercraft, GameEnums.UnitType_Naval];
    };
	this.onWeatherChanged = function(building, weather)
	{	
		var weatherId = weather.getWeatherId();
		if (weatherId === "WEATHER_SNOW")
		{
			building.loadWeatherOverlaySpriteV2("temporary_harbour+snow", false);
		};
	};
}

Constructor.prototype = BUILDING;
var TEMPORARY_HARBOUR = new Constructor();
