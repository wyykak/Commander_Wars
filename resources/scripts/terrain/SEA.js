var Constructor = function()
{
    this.getTerrainGroup = function()
    {
        return 0;
    };
    // loader for stuff which needs C++ Support
    this.init = function (terrain)
    {
        terrain.setTerrainName(SEA.getName());
    };

    this.getName = function()
    {
        return qsTr("Sea");
    };
    this.loadBaseSprite = function(terrain, currentTerrainID)
    {
        var surroundingsPlains = terrain.getSurroundings("PLAINS,SNOW,WASTE,DESERT", true, false, GameEnums.Directions_Direct, false);

        var surroundingsBride = terrain.getSurroundings("BRIDGE", false, false, GameEnums.Directions_Direct, false, true);
        if (surroundingsBride !== "" && terrain.existsResAnim("sea" + surroundingsPlains + "+bridge"))
        {
            GameConsole.print("using bridge", 1);
            surroundingsPlains += "+bridge";
        }
        else
        {
            var surroundings = terrain.getSurroundings("SEA", true, true, GameEnums.Directions_Direct, false);
            // load overlay north east
            if (!surroundings.includes("+N") && !surroundings.includes("+E"))
            {
                var surroundingsNE = terrain.getSurroundings("SEA", true, true, GameEnums.Directions_NorthEast, false);
                if (surroundingsNE !== "")
                {
                    surroundingsPlains += surroundingsNE;
                }
            }
            // load overlay south east
            if (!surroundings.includes("+S") && !surroundings.includes("+E"))
            {
                var surroundingsSE = terrain.getSurroundings("SEA", true, true, GameEnums.Directions_SouthEast, false);
                if (surroundingsSE !== "")
                {
                    surroundingsPlains += surroundingsSE;
                }
            }
            // load overlay south west
            if (!surroundings.includes("+S") && !surroundings.includes("+W"))
            {
                var surroundingsSW = terrain.getSurroundings("SEA", true, true, GameEnums.Directions_SouthWest, false);
                if (surroundingsSW !== "")
                {
                    surroundingsPlains +=  surroundingsSW;
                }
            }
            // load overlay north west
            if (!surroundings.includes("+N") && !surroundings.includes("+W"))
            {
                var surroundingsNW = terrain.getSurroundings("SEA", true, true, GameEnums.Directions_NorthWest, false);
                if (surroundingsNW !== "")
                {
                    surroundingsPlains += surroundingsNW;
                }
            }
        }
        terrain.loadBaseSprite("sea" + surroundingsPlains);
    };
    this.loadOverlaySprite = function(terrain)
    {
        var surroundingsSnow = terrain.getSurroundings("SNOW", true, false, GameEnums.Directions_Direct, false);
        if (surroundingsSnow !== "")
        {
            terrain.loadOverlaySprite("sea+snow" + surroundingsSnow);
        }
        var surroundingsWaste = terrain.getSurroundings("WASTE", true, false, GameEnums.Directions_Direct, false);
        if (surroundingsWaste !== "")
        {
            terrain.loadOverlaySprite("sea+waste" + surroundingsWaste);
        }
        var surroundingsDesert = terrain.getSurroundings("DESERT", true, false, GameEnums.Directions_Direct, false);
        if (surroundingsDesert !== "")
        {
            terrain.loadOverlaySprite("sea+desert" + surroundingsDesert);
        }

        // load river overlay
        var surroundingsSea = terrain.getSurroundings("SEA", true, false, GameEnums.Directions_All, false);
        var surroundingsRiver = terrain.getSurroundings("RIVER", false, false, GameEnums.Directions_Direct, false);
        var surroundingsRiver2 = terrain.getSurroundings("RIVER", true, false, GameEnums.Directions_Direct, false, false, 1);
        // load overlay north
        if ((surroundingsRiver.includes("+N") || surroundingsRiver2.includes("+N")) &&
             surroundingsSea.includes("+E") && surroundingsSea.includes("+W"))
        {
            terrain.loadOverlaySprite("riverending+N");
        }
        // load overlay east
        if ((surroundingsRiver.includes("+E") || surroundingsRiver2.includes("+E")) &&
             surroundingsSea.includes("+S") && surroundingsSea.includes("+N"))
        {
            terrain.loadOverlaySprite("riverending+E");
        }
        // load overlay south
        if ((surroundingsRiver.includes("+S") || surroundingsRiver2.includes("+S")) &&
             surroundingsSea.includes("+E") && surroundingsSea.includes("+W"))
        {
            terrain.loadOverlaySprite("riverending+S");
        }
        // load overlay west
        if ((surroundingsRiver.includes("+W") || surroundingsRiver2.includes("+W")) &&
             surroundingsSea.includes("+S") && surroundingsSea.includes("+N"))
        {
            terrain.loadOverlaySprite("riverending+W");
        }
    };
    this.getMiniMapIcon = function()
    {
        return "minimap_sea";
    };
    this.getTerrainAnimationForeground = function(unit, terrain)
    {
        return "";
    };
    this.getTerrainAnimationBackground = function(unit, terrain)
    {
        var weatherModifier = TERRAIN.getWeatherModifier();
        return "back_" + weatherModifier +"sea";
    };
    this.getTerrainAnimationMoveSpeed = function()
    {
        return 1;
    };
    this.getDescription = function()
    {
        return qsTr("Calm sea. Naval and air forces have good mobility.");
    };

    this.getTerrainSprites = function()
    {
        // array of sprites that can be selected as fix sprites for this terrain
        return ["sea",
                "sea+N",
                "sea+E",
                "sea+S",
                "sea+W",
                "sea+E+SW+NW",
                "sea+E+SW",
                "sea+E+NW",
                "sea+N+SE+SW",
                "sea+N+SE",
                "sea+N+SW",
                "sea+W+NE+SE",
                "sea+W+NE",
                "sea+W+SE",
                "sea+S+NE+NW",
                "sea+S+NE",
                "sea+S+NW",
                "sea+N+E",
                "sea+E+S",
                "sea+E+W",
                "sea+N+S",
                "sea+N+W",
                "sea+S+W",
                "sea+N+W+SE",
                "sea+E+S+NW",
                "sea+S+W+NE",
                "sea+N+E+SW",
                "sea+E+S+W",
                "sea+N+E+S",
                "sea+N+E+W",
                "sea+N+S+W",
                "sea+N+E+S+W",
                "sea+NE+SE+SW+NW",
                "sea+NE+SW+NW",
                "sea+NE+SE+SW",
                "sea+SE+SW+NW",
                "sea+NE+SE+NW",
                "sea+NE+NW",
                "sea+NE+SE",
                "sea+NE+SW",
                "sea+SE+NW",
                "sea+SW+NW",
                "sea+SE+SW",
                "sea+NE",
                "sea+NW",
                "sea+SE",
                "sea+SW",];
    };

};
Constructor.prototype = TERRAIN;
var SEA = new Constructor();
