/************************************************************/
/*                                                          */
/* FILE:        UDPENNY.cpp                                 */
/*              subfile of Display.cpp & UserIfce.cpp       */
/* DESCRIPTION: Handles starting, stopping and pausing      */
/*              sounds.                                     */
/*              Structured like the other UD subfiles.      */
/*                                                          */
/* © Copyright 1999 Artech Digital Entertainments.          */
/*   All rights reserved.                                   */
/************************************************************/

#include "gameinc.h"
extern RULE_GameStateRecord UICurrentGameState;

/************************************************************/
/* GLOBALS                                                  */
/************************************************************/
BOOL PassedGoFlag = FALSE; // Hack, flags passed go so landed on square can skip announcement (collision avoidance)


ACHAR TRANS_PROP[28][T_MAX_BORDS][44] =
{
    {
        /* US        */ A_T("MEDITERRANEAN AVE."),
        /* UK        */ A_T("OLD KENT ROAD"),
        /* French    */ A_T("BOULEVARD DE BELLEVILLE"),
        /* German    */ A_T("BADSTRASSE"),
        /* Spanish   */ A_T("RONDA DE VALENCIA"),
        /* Dutch     */ A_T("DORPSSTRAAT ONS DORP"),
        /* Swedish   */ A_T("VÄSTERLÅNGGATAN"),
        /* Finnish   */ A_T("KORKEAVUOREN- KATU"),
        /* Danish    */ A_T("FINSENSVEJ"),
        /* Norwegian */ A_T("PARKVEIEN"),
        /* Belgium   */ A_T("RUE GRANDE DINANT"),
        /* Singapore */ A_T("GEYLANG ROAD"),
        /* Aust      */ A_T("TODD STREET"),
    },
    {
        /* US        */ A_T("BALTIC AVE."),
        /* UK        */ A_T("WHITE CHAPEL ROAD"),
        /* French    */ A_T("RUE LECOURBE"),
        /* German    */ A_T("TURMSTRASSE"),
        /* Spanish   */ A_T("PLAZA LAVAPIÉS"),
        /* Dutch     */ A_T("BRINK ONS DORP"),
        /* Swedish   */ A_T("HORNSGATAN"),
        /* Finnish   */ A_T("KASARMIKATU"),
        /* Danish    */ A_T("PETER BANGS VEJ"),
        /* Norwegian */ A_T("KIRKEVEIEN"),
        /* Belgium   */ A_T("DIESTSESTRAAT LEUVEN"),
        /* Singapore */ A_T("SERANGOON ROAD"),
        /* Aust      */ A_T("SMITH STREET"),
    },
    {
        /* US        */ A_T("ORIENTAL AVE."),
        /* UK        */ A_T("THE ANGEL, ISLINGTON"),
        /* French    */ A_T("RUE DE VAUGIRARD"),
        /* German    */ A_T("CHAUSSEESTRASSE"),
        /* Spanish   */ A_T("GLORIETA CUATRO CAMINOS"),
        /* Dutch     */ A_T("STEENSTRAAT ARNHEM"),
        /* Swedish   */ A_T("FOLKUNGAGATAN"),
        /* Finnish   */ A_T("RANTATIE"),
        /* Danish    */ A_T("JAGTVEJ"),
        /* Norwegian */ A_T("KONGENS GATE"),
        /* Belgium   */ A_T("STEENSTRAAT BRUGGE"),
        /* Singapore */ A_T("FARRER ROAD"),
        /* Aust      */ A_T("SALAMANCA PLACE"),
    },
    {
        /* US        */ A_T("VERMONT AVE."),
        /* UK        */ A_T("EUSTON ROAD"),
        /* French    */ A_T("RUE DE COURCELLES"),
        /* German    */ A_T("ELISENSTRASSE"),
        /* Spanish   */ A_T("AVENIDA REINA VICTORIA"),
        /* Dutch     */ A_T("KETELSTRAAT ARNHEM"),
        /* Swedish   */ A_T("GÖTGATAN"),
        /* Finnish   */ A_T("KAUPPATORI"),
        /* Danish    */ A_T("FALKONÉR ALLÉ"),
        /* Norwegian */ A_T("PRINSENS GATE"),
        /* Belgium   */ A_T("PLACE DU MONUMENT SPA"),
        /* Singapore */ A_T("BRADDELL ROAD"),
        /* Aust      */ A_T("DAVEY STREET"),
    },
    {
        /* US        */ A_T("CONNETICUT AVE."),
        /* UK        */ A_T("PENTONVILLE ROAD"),
        /* French    */ A_T("AVENUE DE LA RÉPUBLIQUE"),
        /* German    */ A_T("POSTSTRASSE"),
        /* Spanish   */ A_T("CALLE BRAVO MURILLO"),
        /* Dutch     */ A_T("VELPERPLEIN ARNHEM"),
        /* Swedish   */ A_T("RINGVÄGEN"),
        /* Finnish   */ A_T("ESPLANADI"),
        /* Danish    */ A_T("GODTHÅBSVEJ"),
        /* Norwegian */ A_T("ØVRE SLOTTSGATE"),
        /* Belgium   */ A_T("KAPELLESTRAAT OOSTENDE"),
        /* Singapore */ A_T("THOMSON ROAD"),
        /* Aust      */ A_T("MAQUARIE STREET"),
    },
    {
        /* US        */ A_T("ST. CHARLES PLACE"),
        /* UK        */ A_T("PALL MALL"),
        /* French    */ A_T("BOULEVARD DE LA VILLETTE"),
        /* German    */ A_T("SEESTRASSE"),
        /* Spanish   */ A_T("GLORIETA DE BILBAO"),
        /* Dutch     */ A_T("BARTELJORISSTRAAT HAARLEM"),
        /* Swedish   */ A_T("S:T ERIKSGATAN"),
        /* Finnish   */ A_T("HÄMEENTIE"),
        /* Danish    */ A_T("STRAND-BOULEVARDEN"),
        /* Norwegian */ A_T("NEDRE SLOTTSGATE"),
        /* Belgium   */ A_T("RUE DE DIEKIRCH ARLON"),
        /* Singapore */ A_T("BATTERY ROAD"),
        /* Aust      */ A_T("WILLIAM STREET"),
    },
    {
        /* US        */ A_T("STATES AVE."),
        /* UK        */ A_T("WHITEHALL"),
        /* French    */ A_T("AVENUE DE NEUILLY"),
        /* German    */ A_T("HAFENSTRASSE"),
        /* Spanish   */ A_T("CALLE ALBERTO AGUILERA"),
        /* Dutch     */ A_T("ZIJLWEG HAARLEM"),
        /* Swedish   */ A_T("ODENGATAN"),
        /* Finnish   */ A_T("SILTASAARI"),
        /* Danish    */ A_T("CLASSENSGADE"),
        /* Norwegian */ A_T("TRONDHEIMSVEIEN"),
        /* Belgium   */ A_T("BRUUL MECHELEN"),
        /* Singapore */ A_T("EMPRESS PLACE"),
        /* Aust      */ A_T("BARRACK STREET"),
    },
    {
        /* US        */ A_T("VIRGINIA AVE."),
        /* UK        */ A_T("NORTHUMBERLAND AVENUE"),
        /* French    */ A_T("RUE DE PARADIS"),
        /* German    */ A_T("NEUE STRASSE"),
        /* Spanish   */ A_T("CALLE FUENCARRAL"),
        /* Dutch     */ A_T("HOUTSTRAAT HAARLEM"),
        /* Swedish   */ A_T("VALHALLAVÄGEN"),
        /* Finnish   */ A_T("KAISANIEMENKATU"),
        /* Danish    */ A_T("ØSTBANEGADE"),
        /* Norwegian */ A_T("NOBELS GATE"),
        /* Belgium   */ A_T("PLACE VERTE VERVIERS"),
        /* Singapore */ A_T("CONNAUGHT DRIVE"),
        /* Aust      */ A_T("HAY STREET"),
    },
    {
        /* US        */ A_T("ST. JAMES PLACE"),
        /* UK        */ A_T("BOW STREET"),
        /* French    */ A_T("AVENUE MOZART"),
        /* German    */ A_T("MÜNCHENER STRASSE"),
        /* Spanish   */ A_T("AVENIDA FELIPE II"),
        /* Dutch     */ A_T("NEUDE UTRECHT"),
        /* Swedish   */ A_T("STUREGATAN"),
        /* Finnish   */ A_T("LIISANKATU"),
        /* Danish    */ A_T("H.C.ØRSTEDSVEJ"),
        /* Norwegian */ A_T("GRENSEN"),
        /* Belgium   */ A_T("LIPPENSLAAN KNOKKE"),
        /* Singapore */ A_T("COLOMBO COURT"),
        /* Aust      */ A_T("NORTH TERRACE"),
    },
    {
        /* US        */ A_T("TENNESSEE AVE."),
        /* UK        */ A_T("MARLBOROUGH STREET"),
        /* French    */ A_T("BOULEVARD SAINT-MICHEL"),
        /* German    */ A_T("WIENER STRASSE"),
        /* Spanish   */ A_T("CALLE VELÁZQUEZ"),
        /* Dutch     */ A_T("BILTSTRAAT UTRECHT"),
        /* Swedish   */ A_T("KARLAVÄGEN"),
        /* Finnish   */ A_T("SNELLMANINKATU"),
        /* Danish    */ A_T("ROSENØRNS ALLÉ"),
        /* Norwegian */ A_T("GABELS GATE"),
        /* Belgium   */ A_T("RUE ROYALE TOURNAI"),
        /* Singapore */ A_T("HAVELOCK ROAD"),
        /* Aust      */ A_T("VICTORIA SQUARE"),
    },
    {
        /* US        */ A_T("NEW YORK AVE."),
        /* UK        */ A_T("VINE STREET"),
        /* French    */ A_T("PLACE PIGALLE"),
        /* German    */ A_T("BERLINER STRASSE"),
        /* Spanish   */ A_T("CALLE SERRANO"),
        /* Dutch     */ A_T("VREEBURG UTRECHT"),
        /* Swedish   */ A_T("NARVAVÄGEN"),
        /* Finnish   */ A_T("UNIONINKATU"),
        /* Danish    */ A_T("ÅBOULEVARD"),
        /* Norwegian */ A_T("RINGGATA"),
        /* Belgium   */ A_T("GROENPLAATS ANTWERPEN"),
        /* Singapore */ A_T("ST. ANDREWS ROAD"),
        /* Aust      */ A_T("RUNDLE MALL"),
    },
    {
        /* US        */ A_T("KENTUCKY AVE."),
        /* UK        */ A_T("STRAND"),
        /* French    */ A_T("AVENUE MATIGNON"),
        /* German    */ A_T("THEATERSTRASSE"),
        /* Spanish   */ A_T("AVENIDA DE AMÉRICA"),
        /* Dutch     */ A_T("A-KERKHOF GRONINGEN"),
        /* Swedish   */ A_T("STRANDVÄGEN"),
        /* Finnish   */ A_T("LÖNNROTINKATU"),
        /* Danish    */ A_T("HOLMENS KANAL"),
        /* Norwegian */ A_T("BYGDØY ALLÉ"),
        /* Belgium   */ A_T("RUE ST. LÉONARD LIEGE"),
        /* Singapore */ A_T("ROBINSON ROAD"),
        /* Aust      */ A_T("STANLEY STREET"),
    },
    {
        /* US        */ A_T("INDIANA AVE."),
        /* UK        */ A_T("FLEET STREET"),
        /* French    */ A_T("BOULEVARD MALESHERBES"),
        /* German    */ A_T("MUSEUMSTRASSE"),
        /* Spanish   */ A_T("CALLE MARÍA DE MOLINA"),
        /* Dutch     */ A_T("GROOTE MARKT GRONINGEN"),
        /* Swedish   */ A_T("KUNGSTRÄDGÅRDS- GATAN"),
        /* Finnish   */ A_T("ANNANKATU"),
        /* Danish    */ A_T("BØRSGADE"),
        /* Norwegian */ A_T("SKARPSNO"),
        /* Belgium   */ A_T("LANGE STEENSTRAAT KORTRIJK"),
        /* Singapore */ A_T("SHENTON WAY"),
        /* Aust      */ A_T("PETRIES BIGHT"),
    },
    {
        /* US        */ A_T("ILLINOIS AVE."),
        /* UK        */ A_T("TRAFALGAR SQUARE"),
        /* French    */ A_T("AVENUE HENRI-MARTIN"),
        /* German    */ A_T("OPERNPLATZ"),
        /* Spanish   */ A_T("CALLE CEA BERMUDEZ"),
        /* Dutch     */ A_T("HEERESTRAAT GRONINGEN"),
        /* Swedish   */ A_T("HAMNGATAN"),
        /* Finnish   */ A_T("SIMONKATU"),
        /* Danish    */ A_T("TORVEGADE"),
        /* Norwegian */ A_T("SLEMDAL"),
        /* Belgium   */ A_T("GRANDE PLACE MONS"),
        /* Singapore */ A_T("COLLYER QUAY"),
        /* Aust      */ A_T("WICKHAM TERRACE"),
    },
    {
        /* US        */ A_T("ATLANTIC AVE."),
        /* UK        */ A_T("LEICESTER SQUARE"),
        /* French    */ A_T("FAUBOURG SAINT-HONORÉ"),
        /* German    */ A_T("LESSINGSTRASSE"),
        /* Spanish   */ A_T("AVENIDA DE LOS REYES CATÓLICOS"),
        /* Dutch     */ A_T("SPUI ‘S-GRAVENHAGE"),
        /* Swedish   */ A_T("VASAGATAN"),
        /* Finnish   */ A_T("MIKONKATU"),
        /* Danish    */ A_T("STUDIESTRÆDE"),
        /* Norwegian */ A_T("KARL JOHANS GATE"),
        /* Belgium   */ A_T("GROTE MARKT HASSELT"),
        /* Singapore */ A_T("MARINA SQUARE"),
        /* Aust      */ A_T("COLLINS STREET"),
    },
    {
        /* US        */ A_T("VENTNOR AVE."),
        /* UK        */ A_T("COVENTRY STREET"),
        /* French    */ A_T("PLACE DE LA BOURSE"),
        /* German    */ A_T("SCHILLERSTRASSE"),
        /* Spanish   */ A_T("CALLE BAILÉN"),
        /* Dutch     */ A_T("PLEIN ‘S-GRAVENHAGE"),
        /* Swedish   */ A_T("KUNGSGATAN"),
        /* Finnish   */ A_T("ALEKSANTERINKATU"),
        /* Danish    */ A_T("KRYSTALGADE"),
        /* Norwegian */ A_T("STORTORGET"),
        /* Belgium   */ A_T("PLACE DE L'ANGE NAMUR"),
        /* Singapore */ A_T("EMERALD HILL"),
        /* Aust      */ A_T("ELIZABETH STREET"),
    },
    {
        /* US        */ A_T("MARVIN GARDENS"),
        /* UK        */ A_T("PICCADILLY"),
        /* French    */ A_T("RUE LA FAYETTE"),
        /* German    */ A_T("GOETHESTRASSE"),
        /* Spanish   */ A_T("PLAZA DE ESPAÑA"),
        /* Dutch     */ A_T("L. POTEN ‘S-GRAVENHAGE"),
        /* Swedish   */ A_T("STUREPLAN"),
        /* Finnish   */ A_T("KESKUSKATU"),
        /* Danish    */ A_T("FIOLSTRÆDE"),
        /* Norwegian */ A_T("TORGGATA"),
        /* Belgium   */ A_T("HOOGSTRAAT BRUSSEL"),
        /* Singapore */ A_T("RAFFLES CITY"),
        /* Aust      */ A_T("BOURKE STREET"),
    },
    {
        /* US        */ A_T("PACIFIC AVE."),
        /* UK        */ A_T("REGENT STREET"),
        /* French    */ A_T("AVENUE DE BRETEUIL"),
        /* German    */ A_T("RATHAUSPLATZ"),
        /* Spanish   */ A_T("PUERTA DEL SOL"),
        /* Dutch     */ A_T("HOFPLEIN ROTTERDAM"),
        /* Swedish   */ A_T("GUSTAV ADOLFS TORG"),
        /* Finnish   */ A_T("TEHTAANKATU"),
        /* Danish    */ A_T("LANGELINIE"),
        /* Norwegian */ A_T("TROSTERUDVEIEN"),
        /* Belgium   */ A_T("VELDSTRAAT GENT"),
        /* Singapore */ A_T("TANGLIN ROAD"),
        /* Aust      */ A_T("CASTLEREAGH STREET"),
    },
    {
        /* US        */ A_T("NO. CAROLINA AVE."),
        /* UK        */ A_T("OXFORD STREET"),
        /* French    */ A_T("AVENUE FOCH"),
        /* German    */ A_T("HAUPTSTRASSE"),
        /* Spanish   */ A_T("CALLE ALCALÁ"),
        /* Dutch     */ A_T("BLAAK ROTTERDAM"),
        /* Swedish   */ A_T("DROTTNINGGATAN"),
        /* Finnish   */ A_T("EIRA"),
        /* Danish    */ A_T("AMALIEGADE"),
        /* Norwegian */ A_T("PILESTREDET"),
        /* Belgium   */ A_T("BOULEVARD TIROU CHARLEROI"),
        /* Singapore */ A_T("ORCHARD ROAD"),
        /* Aust      */ A_T("GEORGE STREET"),
    },
    {
        /* US        */ A_T("PENNSYLVANIA AVE."),
        /* UK        */ A_T("BOND STREET"),
        /* French    */ A_T("BOULEVARD DES CAPUCINES"),
        /* German    */ A_T("BAHNHOFSTRASSE"),
        /* Spanish   */ A_T("GRAN VÍA"),
        /* Dutch     */ A_T("COOLSINGEL ROTTERDAM"),
        /* Swedish   */ A_T("DIPLOMATSTADEN"),
        /* Finnish   */ A_T("BULEVARDI"),
        /* Danish    */ A_T("HØJBRO PLADS"),
        /* Norwegian */ A_T("SINSEN"),
        /* Belgium   */ A_T("BOULEVARD D'AVROY LIEGE"),
        /* Singapore */ A_T("SCOTTS ROAD"),
        /* Aust      */ A_T("PITT STREET"),
    },
    {
        /* US        */ A_T("PARK PLACE"),
        /* UK        */ A_T("PARK LANE"),
        /* French    */ A_T("AVENUE DES CHAMPS-ÉLYSÉES"),
        /* German    */ A_T("PARKSTRASSE"),
        /* Spanish   */ A_T("PASEO DE LA CASTELLANA"),
        /* Dutch     */ A_T("LEIDSCHESTRAAT AMSTERDAM"),
        /* Swedish   */ A_T("CENTRUM"),
        /* Finnish   */ A_T("MANNERHEIMINTIE"),
        /* Danish    */ A_T("GAMMEL TORV"),
        /* Norwegian */ A_T("ULLEVÅL HAGEBY"),
        /* Belgium   */ A_T("MEIR ANTWERPEN"),
        /* Singapore */ A_T("NASSIM ROAD"),
        /* Aust      */ A_T("FLINDERS WAY"),
    },
    {
        /* US        */ A_T("BOARDWALK"),
        /* UK        */ A_T("MAYFAIR"),
        /* French    */ A_T("RUE DE LA PAIX"),
        /* German    */ A_T("SCHLOSSALLEE"),
        /* Spanish   */ A_T("PASEO DEL PRADO"),
        /* Dutch     */ A_T("KALVERSTRAAT AMSTERDAM"),
        /* Swedish   */ A_T("NORRMALMSTORG"),
        /* Finnish   */ A_T("EROTTAJA"),
        /* Danish    */ A_T("NYTORV"),
        /* Norwegian */ A_T("RÅDHUSPLASSEN"),
        /* Belgium   */ A_T("RUE NEUVE BRUXELLES"),
        /* Singapore */ A_T("QUEEN ASTRID PARK"),
        /* Aust      */ A_T("KINGS AVENUE"),
    },
    {
        /* US        */ A_T("READING RAILROAD"),
        /* UK        */ A_T("KINGS CROSS STATION"),
        /* French    */ A_T("GARE MONTPARNASSE"),
        /* German    */ A_T("SÜDBAHNHOF"),
        /* Spanish   */ A_T("ESTACIÓN DE GOYA"),
        /* Dutch     */ A_T("STATION ZUID"),
        /* Swedish   */ A_T("SÖDRA STATION"),
        /* Finnish   */ A_T("PASILAN ASEMA"),
        /* Danish    */ A_T("NØRREPORT STATION"),
        /* Norwegian */ A_T("OSLO S"),
        /* Belgium   */ A_T("NOORD STATION"),
        /* Singapore */ A_T("ANG MO KIO STATION"),
        /* Aust      */ A_T("PERTH STATION"),
    },
    {
        /* US        */ A_T("PENNSYLVANIA R. R."),
        /* UK        */ A_T("MARYLEBONE STATION"),
        /* French    */ A_T("GARE DE LYON"),
        /* German    */ A_T("WESTBAHNHOF"),
        /* Spanish   */ A_T("ESTACIÓN DE LAS DELICIAS"),
        /* Dutch     */ A_T("STATION WEST"),
        /* Swedish   */ A_T("ÖSTRA STATION"),
        /* Finnish   */ A_T("SÖRNÄISTEN ASEMA"),
        /* Danish    */ A_T("ØSTERPORT STATION"),
        /* Norwegian */ A_T("SKØYEN STASJON"),
        /* Belgium   */ A_T("GARE CENTRALE"),
        /* Singapore */ A_T("CITY HALL STATION"),
        /* Aust      */ A_T("ADELAIDE STATION"),
    },
    {
        /* US        */ A_T("B. & O. RAILROAD"),
        /* UK        */ A_T("FENCHURCH ST. STATION"),
        /* French    */ A_T("GARE DU NORD"),
        /* German    */ A_T("NORDBAHNHOF"),
        /* Spanish   */ A_T("ESTACIÓN DEL MEDIODÍA"),
        /* Dutch     */ A_T("STATION NOORD"),
        /* Swedish   */ A_T("CENTRAL STATION"),
        /* Finnish   */ A_T("RAUTATIEASEMA"),
        /* Danish    */ A_T("VESTERPORT STATION"),
        /* Norwegian */ A_T("GRORUD STASJON"),
        /* Belgium   */ A_T("BUURT SPOORWEGEN"),
        /* Singapore */ A_T("JURONG EAST STATION"),
        /* Aust      */ A_T("FLINDERS ST. STATION"),
    },
    {
        /* US        */ A_T("SHORT LINE R. R."),
        /* UK        */ A_T("LIVERPOOL ST. STATION"),
        /* French    */ A_T("GARE SAINT-LAZARE"),
        /* German    */ A_T("HAUPTBAHNHOF"),
        /* Spanish   */ A_T("ESTACIÓN DEL NORTE"),
        /* Dutch     */ A_T("STATION OOST"),
        /* Swedish   */ A_T("NORRA STATION"),
        /* Finnish   */ A_T("TAVARA-ASEMA"),
        /* Danish    */ A_T("HOVEDBANEGÅRDEN"),
        /* Norwegian */ A_T("BRYN STASJON"),
        /* Belgium   */ A_T("GARE DU MIDI"),
        /* Singapore */ A_T("BEDOK STATION"),
        /* Aust      */ A_T("SYDNEY STATION"),
    },
    {
        /* US        */ A_T("ELECTRIC COMPANY"),
        /* UK        */ A_T("ELECTRIC COMPANY"),
//        /* French    */ A_T("COMPAGNIE DE DISTRIBUTION D’ÉLECTICITÉ"),
        /* French    */ A_T("COMPAGNIE D’ÉLECTRICITÉ"),
        /* German    */ A_T("ELEKTRIZITÄTS-WERK"),
        /* Spanish   */ A_T("COMPAÑÍA DE ELECTRICIDAD"),
        /* Dutch     */ A_T("ELEKTRICITEITS-BEDRIJF"),
        /* Swedish   */ A_T("ELVERKET"),
        /* Finnish   */ A_T("SÄHKÖLAITOS"),
        /* Danish    */ A_T("ELVÆRKET"),
        /* Norwegian */ A_T("OSLO LYSVERKER"),
        /* Belgium   */ A_T("ELEKTRICITEITS - CENTRALE"),
        /* Singapore */ A_T("ELECTRIC COMPANY"),
        /* Aust      */ A_T("ELECTRIC COMPANY"),
    },
    {
        /* US        */ A_T("WATER WORKS"),
        /* UK        */ A_T("WATER WORKS"),
//        /* French    */ A_T("COMPAGNIE DE DISTRIBUTION DES EAUX"),
        /* French    */ A_T("COMPAGNIE DES EAUX"),
        /* German    */ A_T("WASSERWERK"),
        /* Spanish   */ A_T("COMPAÑÍA DE AGUAS"),
        /* Dutch     */ A_T("WATERLEIDING"),
        /* Swedish   */ A_T("VATTENLEDNINGSVERKET"),
        /* Finnish   */ A_T("VESIJOHTOLAITOS"),
        /* Danish    */ A_T("VANDVÆRKET"),
        /* Norwegian */ A_T("VANNVERKET"),
        /* Belgium   */ A_T("WATERMAATSCHAPPIJ"),
        /* Singapore */ A_T("WATER WORKS"),
        /* Aust      */ A_T("WATER WORKS"),
    },
};

ACHAR TRANS[16][T_MAX_LANGS][44] =
{
    {
        /* US        */ A_T("TITLE DEED"),
        /* UK        */ A_T("TITLE DEED"),
        /* French    */ A_T("TITRE DE PROPRIÉTÉ"),
        /* German    */ A_T("BESITZRECHTKARTE"),
        /* Spanish   */ A_T("Título de Propiedad"),
        /* Dutch     */ A_T("EIGENDOMSBEWIJS"),
        /* Swedish   */ A_T("Lagfartsbevis"),
        /* Finnish   */ A_T("LAINHUUDATUSTODISTUS"),
        /* Danish    */ A_T("SKØDE"),
        /* Norwegian */ A_T("Skjøtekort"),
    },
    {
        /* US        */ A_T("RENT"),
        /* UK        */ A_T("RENT - SITE ONLY"),
//        /* French    */ A_T("LOYER - TERRAIN NU"),
        /* French    */ A_T("LOYER"),
//        /* German    */ A_T("Miete - Grundstück allein"),
        /* German    */ A_T("Miete"),
        /* Spanish   */ A_T("Alquileres"),
        /* Dutch     */ A_T("HUUR-Onbebouwd"),
//        /* Swedish   */ A_T("Hyra for endast tomten"),
        /* Swedish   */ A_T("Hyra"),
        /* Finnish   */ A_T("VUOKRA TONTISTA"),
        /* Danish    */ A_T("LEJE - GRUNDEN"),
        /* Norwegian */ A_T("Leie av gate"),
    },
    {
        /* US        */ A_T("With 1 House"),
        /* UK        */ A_T("with 1 house"),
        /* French    */ A_T("avec 1 maison"),
        /* German    */ A_T("mit 1 Haus"),
        /* Spanish   */ A_T("Con 1 casa"),
        /* Dutch     */ A_T("Met 1 huis"),
        /* Swedish   */ A_T("med 1 hus"),
//        /* Finnish   */ A_T("med 1 hus"),
        /* Finnish   */ A_T("Tontista + 1 talosta"),
        /* Danish    */ A_T("MED 1 HUS"),
        /* Norwegian */ A_T("med 1 hus"),
    },
    {
        /* US        */ A_T("With 2 Houses"),
        /* UK        */ A_T("with 2 houses"),
        /* French    */ A_T("avec 2 maisons"),
        /* German    */ A_T("mit 2 Häusern"),
        /* Spanish   */ A_T("Con 2 casas"),
        /* Dutch     */ A_T("Met 2 huizen"),
        /* Swedish   */ A_T("med 2 hus"),
//        /* Finnish   */ A_T("med 2 hus"),
        /* Finnish   */ A_T("Tontista + 2 talosta"),
        /* Danish    */ A_T("MED 2 HUSE"),
        /* Norwegian */ A_T("med 2 hus"),
    },
    {
        /* US        */ A_T("With 3 Houses"),
        /* UK        */ A_T("with 3 houses"),
        /* French    */ A_T("avec 3 maisons"),
        /* German    */ A_T("mit 3 Häusern"),
        /* Spanish   */ A_T("Con 3 casas"),
        /* Dutch     */ A_T("Met 3 huizen"),
        /* Swedish   */ A_T("med 3 hus"),
//        /* Finnish   */ A_T("med 3 hus"),
        /* Finnish   */ A_T("Tontista + 3 talosta"),
        /* Danish    */ A_T("MED 3 HUSE"),
        /* Norwegian */ A_T("med 3 hus"),
    },
    {
        /* US        */ A_T("With 4 Houses"),
        /* UK        */ A_T("with 4 houses"),
        /* French    */ A_T("avec 4 maisons"),
        /* German    */ A_T("mit 4 Häusern"),
        /* Spanish   */ A_T("Con 4 casas"),
        /* Dutch     */ A_T("Met 4 huizen"),
        /* Swedish   */ A_T("med 4 hus"),
//        /* Finnish   */ A_T("med 4 hus"),
        /* Finnish   */ A_T("Tontista + 4 talosta"),
        /* Danish    */ A_T("MED 4 HUSE"),
        /* Norwegian */ A_T("med 4 hus"),
    },
    {
        /* US        */ A_T("With HOTEL"),
        /* UK        */ A_T("RENT WITH HOTEL"),
        /* French    */ A_T("LOYER AVEC HÔTEL"),
        /* German    */ A_T("mit HOTEL"),
        /* Spanish   */ A_T("Con hotel"),
        /* Dutch     */ A_T("Met Hotel"),
        /* Swedish   */ A_T("med hotell"),
//        /* Finnish   */ A_T("med hotell"),
        /* Finnish   */ A_T("Tontista + hotellista"),
        /* Danish    */ A_T("MED HOTEL"),
        /* Norwegian */ A_T("med hotell"),
    },
    {
        /* US        */ A_T("Mortgage Value"),
        /* UK        */ A_T("Mortgage Value"),
        /* French    */ A_T("Valeur Hypothécaire"),
        /* German    */ A_T("Hypothekenwert"),
        /* Spanish   */ A_T("Hipoteca"),
        /* Dutch     */ A_T("Grond (hyp)"),
        /* Swedish   */ A_T("Intecknas för"),
        /* Finnish   */ A_T("Kiinnitysarvo"),
        /* Danish    */ A_T("Belåningsværdi"),
        /* Norwegian */ A_T("Pantsettes for"),
    },
    {
        /* US        */ A_T("1 House costs:"),
        /* UK        */ A_T("Cost of houses"),
        /* French    */ A_T("Prix des maisons"),
        /* German    */ A_T("1 Haus kostet:"),
        /* Spanish   */ A_T("De cada casa"),
        /* Dutch     */ A_T("Prijs van een huis"),
        /* Swedish   */ A_T("1 HUS kostar"),
        /* Finnish   */ A_T("1 talo maksaa"),
        /* Danish    */ A_T("PRIS pr. hus"),
        /* Norwegian */ A_T("1 hus koster"),
    },
    {
        /* US        */ A_T("1 Hotel costs:"),
        /* UK        */ A_T("Cost of hotels"),
        /* French    */ A_T("Prix des hôtels"),
        /* German    */ A_T("1 Hotel kostet:"),
        /* Spanish   */ A_T("De los hoteles"),
        /* Dutch     */ A_T("Prijs van een hotel"),
        /* Swedish   */ A_T("1 HOTELL kostar"),
        /* Finnish   */ A_T("1 hotelli maksaa"),
//        /* Finnish   */ A_T("1 hotelli"),
        /* Danish    */ A_T("PRIS pr. hotel"),
        /* Norwegian */ A_T("1 hotell koster"),
    },
    {
        /* US        */ A_T("plus 4 Houses"),
        /* UK        */ A_T("plus 4 Houses"),
        /* French    */ A_T("plus 4 maisons"),
        /* German    */ A_T("plus 4 Häuser"),
        /* Spanish   */ A_T("Además 4 casas"),
        /* Dutch     */ A_T("Plus 4 huizen"),
        /* Swedish   */ A_T("plus 4 hus"),
        /* Finnish   */ A_T("+ 4 taloa"),
        /* Danish    */ A_T("Plus 4 huse"),
//        /* Norwegian */ A_T("pluss 4 hus"),
        /* Norwegian */ A_T("4 hus org"),
    },
    {
        /* US        */ A_T("Mortgaged"),
        /* UK        */ A_T("Mortgaged"),
        /* French    */ A_T("HYPOTHÉQUÉE"),
        /* German    */ A_T("HYPOTHEKENBRIEF"),
        /* Spanish   */ A_T("Hipotecado"),
        /* Dutch     */ A_T("Hypotheek"),
        /* Swedish   */ A_T("INTECKNAD"),
        /* Finnish   */ A_T("KIINNITETTY"),
        /* Danish    */ A_T("Pantsat"),
        /* Norwegian */ A_T("PANTSATT"),
    },
    {
        /* US        */ A_T("Mortgaged for"),
        /* UK        */ A_T("MORTGAGED FOR"),
        /* French    */ A_T("HYPOTHÉQUÉE POUR"),
        /* German    */ A_T("BELASTET MIT"),
        /* Spanish   */ A_T("Hipotecado por"),
        /* Dutch     */ A_T("Voor de som van:"),
        /* Swedish   */ A_T("INTECKNAD FÖR"),
//        /* Finnish   */ A_T("KIINNITETTY MK:STA"),
        /* Finnish   */ A_T("KIINNITETTY"),
//        /* Danish    */ A_T("Pantsat for"),
        /* Danish    */ A_T("Belant Med"),
        /* Norwegian */ A_T("PANTSATT FOR"),
    },
    {
        /* US        */ A_T("If 2 stations are owned"),
        /* UK        */ A_T("If 2 stations are owned"),
//        /* French    */ A_T("LOYER si vous avez 2 gares"),
        /* French    */ A_T("2"),
//        /* German    */ A_T("Wenn man 2 Bahnhöfe besitzt"),
        /* German    */ A_T("2"),
        /* Spanish   */ A_T("Si tiene 2 Estaciones"),
        /* Dutch     */ A_T("2 Stations in bezit:"),
//        /* Swedish   */ A_T("Om samma ägare har 2 stationer"),
        /* Swedish   */ A_T("2"),
//        /* Finnish   */ A_T("Jos samalla omistajalla on 2 asemaa"),
        /* Finnish   */ A_T("2 ASEMASTA"),
//        /* Danish    */ A_T("Ejer samme spiller 2 stationer"),
        /* Danish    */ A_T("2"),
//        /* Norwegian */ A_T("Hvis en spiller eier 2 jernbanestasjoner"),
        /* Norwegian */ A_T("2"),
    },
    {
        /* US        */ A_T("If 3 stations are owned"),
        /* UK        */ A_T("If 3 stations are owned"),
//        /* French    */ A_T("LOYER si vous avez 3 gares"),
        /* French    */ A_T("3"),
//        /* German    */ A_T("Wenn man 3 Bahnhöfe besitzt"),
        /* German    */ A_T("3"),
        /* Spanish   */ A_T("Si tiene 3 Estaciones"),
        /* Dutch     */ A_T("3 Stations in bezit:"),
//        /* Swedish   */ A_T("Om samma ägare har 3 stationer"),
        /* Swedish   */ A_T("3"),
//        /* Finnish   */ A_T("Jos samalla omistajalla on 3 asemaa"),
        /* Finnish   */ A_T("3 ASEMASTA"),
//        /* Danish    */ A_T("Ejer samme spiller 3 stationer"),
        /* Danish    */ A_T("3"),
//        /* Norwegian */ A_T("Hvis en spiller eier 3 jernbanestasjoner"),
        /* Norwegian */ A_T("3"),
    },
    {
        /* US        */ A_T("If 4 stations are owned"),
        /* UK        */ A_T("If 4 stations are owned"),
//        /* French    */ A_T("LOYER si vous avez 4 gares"),
        /* French    */ A_T("4"),
//        /* German    */ A_T("Wenn man 4 Bahnhöfe besitzt"),
        /* German    */ A_T("4"),
        /* Spanish   */ A_T("Si tiene 4 Estaciones"),
        /* Dutch     */ A_T("4 Stations in bezit:"),
//        /* Swedish   */ A_T("Om samma ägare har 4 stationer"),
        /* Swedish   */ A_T("4"),
//        /* Finnish   */ A_T("Jos samalla omistajalla on 4 asemaa"),
        /* Finnish   */ A_T("4 ASEMASTA"),
//        /* Danish    */ A_T("Ejer samme spiller 4 stationer"),
        /* Danish    */ A_T("4"),
//        /* Norwegian */ A_T("Hvis en spiller eier 4 jernbanestasjoner"),
        /* Norwegian */ A_T("4"),
    },
};

ACHAR TRANS_UTIL1[T_MAX_CURRENCY_FACTORS][T_MAX_LANGS][256] = {
{
    /* US        */ A_T("If one Utility is owned, rent is 4 times amount shown on dice."),
    /* UK        */ A_T("If one Utility is owned, rent is 4 times amount shown on dice."),
    /* French    */ A_T("Si l'on possède UNE carte de compagnie de Service Public, le loyer est 4 fois le montant indiqué par les dés."),
    /* German    */ A_T("Wenn man Besitzer des Elektrizitäts-Werkes ist, so ist die Miete 4mal so hoch, wie Augen auf den zwei Würfeln sind."),
    /* Spanish   */ A_T("Si se posee UNA carta de Compañía de Servicio Público, el Alquiler es de 4 veces el número salido en los dados."),
    /* Dutch     */ A_T("Wanneer u één NUTSBEDRIJF bezit: 4 x het aantal ogen van de worp."),
    /* Swedish   */ A_T("Om ni innehar EN anläggning, utgår hyran med 4 gånger det belopp som tärningarna visar."),
    /* Finnish   */ A_T("Jos samalla omistajalla on yksi laitos, on vuokra yhtä kuin 4 kertaa arpakuutioiden osoittama luku."),
    /* Danish    */ A_T("Ejes 1 forsyningsselskab, er lejen 4 gange det tal, terningerne viser."),
    /* Norwegian */ A_T("Eieren av Oslo Lysverker kan innkassere en leie som er 4 ganger så høy som tallet på terningene som ble brukt for å flytte til dette feltet."),
},
{
    /* US        */ A_T("If one Utility is owned, rent is 40 times amount shown on dice."),
    /* UK        */ A_T("If one Utility is owned, rent is 40 times amount shown on dice."),
    /* French    */ A_T("Si l'on possède UNE carte de compagnie de Service Public, le loyer est 40 fois le montant indiqué par les dés."),
    /* German    */ A_T("Wenn man Besitzer des Elektrizitäts-Werkes ist, so ist die Miete 40mal so hoch, wie Augen auf den zwei Würfeln sind."),
    /* Spanish   */ A_T("Si se posee UNA carta de Compañía de Servicio Público, el Alquiler es de 40 veces el número salido en los dados."),
    /* Dutch     */ A_T("Wanneer u één NUTSBEDRIJF bezit: 40 x het aantal ogen van de worp."),
    /* Swedish   */ A_T("Om ni innehar EN anläggning, utgår hyran med 40 gånger det belopp som tärningarna visar."),
    /* Finnish   */ A_T("Jos samalla omistajalla on yksi laitos, on vuokra yhtä kuin 40 kertaa arpakuutioiden osoittama luku."),
    /* Danish    */ A_T("Ejes 1 forsyningsselskab, er lejen 40 gange det tal, terningerne viser."),
    /* Norwegian */ A_T("Eieren av Oslo Lysverker kan innkassere en leie som er 40 ganger så høy som tallet på terningene som ble brukt for å flytte til dette feltet."),
},
{
    /* US        */ A_T("If one Utility is owned, rent is 80 times amount shown on dice."),
    /* UK        */ A_T("If one Utility is owned, rent is 80 times amount shown on dice."),
    /* French    */ A_T("Si l'on possède UNE carte de compagnie de Service Public, le loyer est 80 fois le montant indiqué par les dés."),
    /* German    */ A_T("Wenn man Besitzer des Elektrizitäts-Werkes ist, so ist die Miete 80mal so hoch, wie Augen auf den zwei Würfeln sind."),
    /* Spanish   */ A_T("Si se posee UNA carta de Compañía de Servicio Público, el Alquiler es de 80 veces el número salido en los dados."),
    /* Dutch     */ A_T("Wanneer u één NUTSBEDRIJF bezit: 80 x het aantal ogen van de worp."),
    /* Swedish   */ A_T("Om ni innehar EN anläggning, utgår hyran med 80 gånger det belopp som tärningarna visar."),
    /* Finnish   */ A_T("Jos samalla omistajalla on yksi laitos, on vuokra yhtä kuin 80 kertaa arpakuutioiden osoittama luku."),
    /* Danish    */ A_T("Ejes 1 forsyningsselskab, er lejen 80 gange det tal, terningerne viser."),
    /* Norwegian */ A_T("Eieren av Oslo Lysverker kan innkassere en leie som er 80 ganger så høy som tallet på terningene som ble brukt for å flytte til dette feltet."),
},
{
    /* US        */ A_T("If one Utility is owned, rent is 400 times amount shown on dice."),
    /* UK        */ A_T("If one Utility is owned, rent is 400 times amount shown on dice."),
    /* French    */ A_T("Si l'on possède UNE carte de compagnie de Service Public, le loyer est 400 fois le montant indiqué par les dés."),
    /* German    */ A_T("Wenn man Besitzer des Elektrizitäts-Werkes ist, so ist die Miete 400mal so hoch, wie Augen auf den zwei Würfeln sind."),
    /* Spanish   */ A_T("Si se posee UNA carta de Compañía de Servicio Público, el Alquiler es de 400 veces el número salido en los dados."),
    /* Dutch     */ A_T("Wanneer u één NUTSBEDRIJF bezit: 400 x het aantal ogen van de worp."),
    /* Swedish   */ A_T("Om ni innehar EN anläggning, utgår hyran med 400 gånger det belopp som tärningarna visar."),
    /* Finnish   */ A_T("Jos samalla omistajalla on yksi laitos, on vuokra yhtä kuin 400 kertaa arpakuutioiden osoittama luku."),
    /* Danish    */ A_T("Ejes 1 forsyningsselskab, er lejen 400 gange det tal, terningerne viser."),
    /* Norwegian */ A_T("Eieren av Oslo Lysverker kan innkassere en leie som er 400 ganger så høy som tallet på terningene som ble brukt for å flytte til dette feltet."),
}};

ACHAR TRANS_UTIL2[T_MAX_CURRENCY_FACTORS][T_MAX_LANGS][256] = {
{
    /* US        */ A_T("If both Utilities are owned, rent is 10 times amount shown on dice."),
    /* UK        */ A_T("If both Utilities are owned, rent is 10 times amount shown on dice."),
    /* French    */ A_T("Si l'on possède les DEUX cartes de compagnie de Service Public, le loyer est DIX fois le montant indiqué par les dés."),
    /* German    */ A_T("Wenn man Besitzer beider Werke ist, Wasser - und Elektrizitäts-Werk, so ist die Miete 10mal so hoch, wie Augen auf den zwei Würfeln sind."),
    /* Spanish   */ A_T("Si se poseen dos cartas de Compañías de Servicio Público, el Alquiler es 10 veces el número salido en los dados."),
    /* Dutch     */ A_T("Wanneer u beide NUTSBEDRIJVEN bezit: 10 x het aantal ogen van de worp."),
    /* Swedish   */ A_T("Om ni innehar BÅDA anläggningarna utgår hyran med 10 gånger det belopp som tärningarna visar."),
    /* Finnish   */ A_T("Jos samalla omistajalla on molemmat laitokset, on vuokra yhtä kuin 10 kertaa arpakuutioiden osoittama luku."), /* not properly changed yet */
    /* Danish    */ A_T("Ejes 2 forsyningsselskaber, er lejen 10 gange det tal, terningerne viser."),
    /* Norwegian */ A_T("Eier en spiller både Oslo Lysverker og Vannverket, skal det betales 10 ganger så mye som terningene viste."),
},
{
    /* US        */ A_T("If both Utilities are owned, rent is 100 times amount shown on dice."),
    /* UK        */ A_T("If both Utilities are owned, rent is 100 times amount shown on dice."),
    /* French    */ A_T("Si l'on possède les DEUX cartes de compagnie de Service Public, le loyer est CENT fois le montant indiqué par les dés."),
    /* German    */ A_T("Wenn man Besitzer beider Werke ist, Wasser - und Elektrizitäts-Werk, so ist die Miete 100mal so hoch, wie Augen auf den zwei Würfeln sind."),
    /* Spanish   */ A_T("Si se poseen dos cartas de Compañías de Servicio Público, el Alquiler es 100 veces el número salido en los dados."),
    /* Dutch     */ A_T("Wanneer u beide NUTSBEDRIJVEN bezit: 100 x het aantal ogen van de worp."),
    /* Swedish   */ A_T("Om ni innehar BÅDA anläggningarna utgår hyran med 100 gånger det belopp som tärningarna visar."),
    /* Finnish   */ A_T("Jos samalla omistajalla on molemmat laitokset, on vuokra yhtä kuin 100 kertaa arpakuutioiden osoittama luku."), /* not properly changed yet */
    /* Danish    */ A_T("Ejes 2 forsyningsselskaber, er lejen 100 gange det tal, terningerne viser."),
    /* Norwegian */ A_T("Eier en spiller både Oslo Lysverker og Vannverket, skal det betales 100 ganger så mye som terningene viste."),
},
{
    /* US        */ A_T("If both Utilities are owned, rent is 200 times amount shown on dice."),
    /* UK        */ A_T("If both Utilities are owned, rent is 200 times amount shown on dice."),
    /* French    */ A_T("Si l'on possède les DEUX cartes de compagnie de Service Public, le loyer est 200 fois le montant indiqué par les dés."),
    /* German    */ A_T("Wenn man Besitzer beider Werke ist, Wasser - und Elektrizitäts-Werk, so ist die Miete 200mal so hoch, wie Augen auf den zwei Würfeln sind."),
    /* Spanish   */ A_T("Si se poseen dos cartas de Compañías de Servicio Público, el Alquiler es 200 veces el número salido en los dados."),
    /* Dutch     */ A_T("Wanneer u beide NUTSBEDRIJVEN bezit: 200 x het aantal ogen van de worp."),
    /* Swedish   */ A_T("Om ni innehar BÅDA anläggningarna utgår hyran med 200 gånger det belopp som tärningarna visar."),
    /* Finnish   */ A_T("Jos samalla omistajalla on molemmat laitokset, on vuokra yhtä kuin 200 kertaa arpakuutioiden osoittama luku."), /* not properly changed yet */
    /* Danish    */ A_T("Ejes 2 forsyningsselskaber, er lejen 200 gange det tal, terningerne viser."),
    /* Norwegian */ A_T("Eier en spiller både Oslo Lysverker og Vannverket, skal det betales 200 ganger så mye som terningene viste."),
},
{
    /* US        */ A_T("If both Utilities are owned, rent is 1000 times amount shown on dice."),
    /* UK        */ A_T("If both Utilities are owned, rent is 1000 times amount shown on dice."),
    /* French    */ A_T("Si l'on possède les DEUX cartes de compagnie de Service Public, le loyer est MILLE fois le montant indiqué par les dés."),
    /* German    */ A_T("Wenn man Besitzer beider Werke ist, Wasser - und Elektrizitäts-Werk, so ist die Miete 1000mal so hoch, wie Augen auf den zwei Würfeln sind."),
    /* Spanish   */ A_T("Si se poseen dos cartas de Compañías de Servicio Público, el Alquiler es 1.000 veces el número salido en los dados."),
    /* Dutch     */ A_T("Wanneer u beide NUTSBEDRIJVEN bezit: 1.000 x het aantal ogen van de worp."),
    /* Swedish   */ A_T("Om ni innehar BÅDA anläggningarna utgår hyran med 1.000 gånger det belopp som tärningarna visar."),
    /* Finnish   */ A_T("Jos samalla omistajalla on molemmat laitokset, on vuokra yhtä kuin 1.000 kertaa arpakuutioiden osoittama luku."), /* not properly changed yet */
    /* Danish    */ A_T("Ejes 2 forsyningsselskaber, er lejen 1000 gange det tal, terningerne viser."),
    /* Norwegian */ A_T("Eier en spiller både Oslo Lysverker og Vannverket, skal det betales 1000 ganger så mye som terningene viste."),
}};

ACHAR TRANS_SIMPLE_MONO[T_MAX_LANGS][256] =
{
    /* US        */ A_T("If a player owns ALL the Lots of any Color-Group, the rent is Doubled on Unimproved Lots in that group."),
    /* UK        */ A_T("If a player owns all the sites of any colour-group, the rent is doubled on the unimproved sites in that group."),
    /* French    */ A_T("Si un joueur possède TOUS les terrains d'un Groupe de Couleur quelconque, le loyer des terrains nus de ce groupe est doublé."),
    /* German    */ A_T("Wenn ein Spieler alle Grundstücke einer Farbengruppe besitzt, so ist die Miete auf den unbebauten Grundstücken dieser Farbengruppe doppelt so hoch."),
    /* Spanish   */ A_T("Si un jugador posee todos los terrenos de un Grupo de color, el precio del alquiler se duplica. Precio de compra:"),
    /* Dutch     */ A_T("Wanneer een speler alle straten van een kleurgroep (stad) bezit, wordt de huur voor <<onbebouwd>> verdubbeld."),
    /* Swedish   */ A_T("Om en spelare äger ALLA tomterna inom samma färggrupp blir hyran fördubblad för de obebyggda tomterna inom denna grupp."),
    /* Finnish   */ A_T("Jos pelaaja omistaa KAIKKI samaan väriryhmään kuuluvat tontit, hän saa tähän ryhmään kuuluvista rakentamattomista tonteista kaksinkertaisen vuokran."),
    /* Danish    */ A_T("Hvis en spiller ejer alle grunde i en farvegruppe, skal der betales dobbelt eje af ubebyggede grunde i gruppen."),
    /* Norwegian */ A_T("Hvis en spiller eier alle gatene i en fargegruppe, fordobles leien av de ubebygde gatene i denne gruppen."),
};

ACHAR TRANS_MORT[T_MAX_LANGS][256] =
{
    /* US        */ A_T("Card must be turned this side up if property is mortgaged."),
    /* UK        */ A_T("Card must be turned this side up if the property is mortgaged"),
    /* French    */ A_T("Si cette propriété est hypothéquée, ce côté de la carte doit être mis en évidence sur la table."),
    /* German    */ A_T("Die Karte muß auf diese Seite umgedreht werden, wenn das Grundstück belastet ist."),
    /* Spanish   */ A_T("Si esta propiedad está hipotecada el Título debe quedar por este lado encima de la mesa."),
    /* Dutch     */ A_T("Deze kant van de kaart boven betekent: met hypotheek bezwaard."),
    /* Swedish   */ A_T("Kortet Måste Vändas Med Denna Sida Upp När Tomten Är Intecknad."),
    /* Finnish   */ A_T("Kun tontti on kiinnitetty, tämä puoli korttia on pidettävä näkyvissä."),
    /* Danish    */ A_T("Dette kort skal lægges med denne side op, hvis grunden belånes."),
    /* Norwegian */ A_T("Kortet skal ligge med denne siden opp når gata er pantsatt."),
};

ACHAR TRANS_COPYWRITE[T_MAX_LANGS][256] =
{
    /* US        */ A_T("1935 PARKER BROTHERS"),
    /* UK        */ A_T("1935 PARKER BROTHERS"),
    /* French    */ A_T("1935 PARKER BROTHERS"),
    /* German    */ A_T("1935 PARKER BROTHERS"),
    /* Spanish   */ A_T("1935 PARKER BROTHERS"),
    /* Dutch     */ A_T("1935 PARKER BROTHERS"),
    /* Swedish   */ A_T("1935 PARKER BROTHERS"),
    /* Finnish   */ A_T("1935 PARKER BROTHERS"),
    /* Danish    */ A_T("1935 PARKER BROTHERS"),
    /* Norwegian */ A_T("1935 PARKER BROTHERS"),
};


/************************************************************/

LE_DATA_DataId UDSOUND_Pennybags_Sound[PB_VOICE_MAX][2] =
{
   { WAV_pb001   , 1 },     // PB_WelcomeGame_old
   { WAV_pb002   , 1 },     // PB_Menu_old
   { WAV_pb003   , 1 },     // PB_ChooseNetworkOrLocalGame_old
   { WAV_pb004   , 5 },     // PB_NoUserInputForXSeconds_old
   { WAV_pb009   , 5 },     // PB_HumanPlayerPickToken_old
   { WAV_pb014   , 1 },     // PB_ChoosePlayer_HumanOrComputer_old
   { WAV_pb015   , 2 },     // PB_ChooseTokenForComputerPlayer_old
   { WAV_pb017   , 1 },     // PB_NewPlayerEnterNameOrPickFromList_old
   { WAV_pb018   , 1 },     // PB_NewPlayerEnterName_old
   { WAV_pb019   , 2 },     // PB_ChooseAIDifficultyLevel_old
   { WAV_pb021   , 1 },     // PB_ChooseClassicOrCityBoard_AnyVersion_old
   { WAV_pb022   , 1 },     // PB_ChooseStandardOrCustomRules_old
   { WAV_pb023   , 1 },     // PB_SummaryScreen_old
   { WAV_pb024   , 1 },     // PB_GameStarted_old
   { WAV_pb025   , 1 },     // PB_NegotiateRules_old
   { WAV_pb026   , 11},     // PB_PromptEachPlayerForCustomRulesConfirmation_old
   { WAV_pb036   , 1 },     // PB_CustomRulesAgreedOnStartGame_old
   { WAV_pb037   , 4 },     // PB_MainScreenIntro_old
   { WAV_pb041   , 6 },     // PB_RollDice_Cannon
   { WAV_pb047   , 5 },     // PB_RollDice_Car
   { WAV_pb052   , 5 },     // PB_RollDice_Dog
   { WAV_pb057   , 5 },     // PB_RollDice_Hat
   { WAV_pb067   , 5 },     // PB_RollDice_Iron
   { WAV_pb072   , 5 },     // PB_RollDice_Horse
   { WAV_pb077   , 5 },     // PB_RollDice_Ship
   { WAV_pb082   , 5 },     // PB_RollDice_Shoe
   { WAV_pb087   , 5 },     // PB_RollDice_Thimble
   { WAV_pb092   , 5 },     // PB_RollDice_Wheelbarrow
   { WAV_pb097   , 5 },     // PB_RollDice_SackOfMoney
   { WAV_pb102   , 10},     // PB_RollDice_Generic
   { WAV_pb112   , 3 },     // PB_SayDiceRoll_2
   { WAV_pb115   , 3 },     // PB_SayDiceRoll_3
   { WAV_pb118   , 3 },     // PB_SayDiceRoll_4
   { WAV_pb121   , 4 },     // PB_SayDiceRoll_5
   { WAV_pb125   , 3 },     // PB_SayDiceRoll_6
   { WAV_pb128   , 3 },     // PB_SayDiceRoll_7
   { WAV_pb131   , 4 },     // PB_SayDiceRoll_8
   { WAV_pb135   , 3 },     // PB_SayDiceRoll_9
   { WAV_pb138   , 3 },     // PB_SayDiceRoll_10
   { WAV_pb141   , 3 },     // PB_SayDiceRoll_11
   { WAV_pb144   , 4 },     // PB_SayDiceRoll_12
   { WAV_pb148   ,  9},     // PB_TokenLandsOnUnownedProperty ** last one (of 10) is a copy of 158 - dropped.
   { WAV_pb158   , 3 },     // PB_BuyBoardSide1
   { WAV_pb161   , 3 },     // PB_BuyBoardSide2
   { WAV_pb164   , 3 },     // PB_BuyBoardSide3
   { WAV_pb167   , 3 },     // PB_BuyBoardSide4
   { WAV_pb170   , 3 },     // PB_BuyRailroad
   { WAV_pb173   , 3 },     // PB_BuyUtility
   { WAV_pb176   , 5 },     // PB_LandOn_Chance
   { WAV_pb181   , 5 },     // PB_LandOn_CommunityChest
   { WAV_pb186   , 16},     // PB_ChanceCard
   { WAV_pb202   , 16},     // PB_CommunityChestCard
   { WAV_pb218   , 3 },     // PB_LandOn_JustVisiting
   { WAV_pb221   , 3 },     // PB_LandOn_FreeParking
   { WAV_pb224   , 3 },     // PB_LandOn_FreeParkingWithFreeParkingRuleOn
   { WAV_pb227   , 3 },     // PB_LandOn_GoToJail_Negative
   { WAV_pb230   , 3 },     // PB_LandOn_GoToJail_Positive
   { WAV_pb233   , 3 },     // PB_JailPayMoneyOrTryForDoubles
#if USA_VERSION
   { WAV_pb236   , 11},     // PB_CollectMoney_Go
   { WAV_pb246   , 3/*4*/ },     // PB_CollectMoney_LandedOnGoAndDoublePayRuleInEffect - last one lost
   { WAV_pb250   , 3 },     // PB_LandOn_LuxuryTax
   { WAV_pb253   , 3 },     // PB_LandOn_IncomeTax
#else // restrict lines - no currency
   { WAV_pb244   ,  2},     // PB_CollectMoney_Go
   { WAV_pb246   , 2/*4*/ },     // PB_CollectMoney_LandedOnGoAndDoublePayRuleInEffect - last one lost
   { WAV_pb251   , 2 },     // PB_LandOn_LuxuryTax
   { WAV_pb254   , 2 },     // PB_LandOn_IncomeTax
#endif
   { WAV_pb256   , 3 },     // PB_Auction
   { WAV_pb259   , 3 },     // PB_Auction_ExplanationOnButtonsToIncreaseBid
   { WAV_pb262   , 3 },     // PB_Auction_OpeningBid
   { WAV_pb264   , 11},     // PB_Auction_CallOutTokenWithHighestBid
   { WAV_pb274   , 1 },     // PB_Auction_GoingOnce
   { WAV_pb275   , 1 },     // PB_Auction_GoingTwice
   { WAV_pb276   , 2 },     // PB_Auction_GoingSold
   { WAV_pb278   , 3 },     // PB_Auction_CongratulateWinner
   { WAV_pb281   , 3 },     // PB_PlayerClicks_Mortgage
   { WAV_pb284   , 3 },     // PB_PlayerClicks_UnMortgage
   { WAV_pb287   , 3 },     // PB_PlayerClicks_BuyHouseHotel
   { WAV_pb290   , 3 },     // PB_PlayerClicks_SellHouseHotel
   { WAV_pb293   , 3 },     // PB_PlayerAcquiredMonopoly
   { WAV_pb296   , 3 },     // PB_PlayerBuiltFirstHouse
   { WAV_pb299   , 3 },     // PB_PlayerBuiltFirstHotel
   { WAV_pb302   , 1 },     // PB_StatusScreen_FirstTime
   { WAV_pb303   , 1 },     // PB_StatusScreen_FirstTimeDeeds
   { WAV_pb304   , 1 },     // PB_StatusScreen_FirstTimeBankAssets
   { WAV_pb305   , 1 },     // PB_Calculator
   { WAV_pb306   , 3 },     // PB_TradeScreen
   { WAV_pb309   , 3 },     // PB_TradeScreen_ProposeClickedButOneSideHasNotOfferedAnything
   { WAV_pb312   , 1 },     // PB_TradeScreen_ThirdPartyTradeExplanation
   { WAV_pb313   , 3 },     // PB_TradeScreen_PickTradePartner
   { WAV_pb316   , 5 },     // PB_TradeScreen_TradeComplete
   { WAV_pb321   , 3 },     // PB_TradeScreen_TradeCancelled
   { WAV_pb325   , 1 },     // PB_HousingShortage_FirstTimeHouses
   { WAV_pb326   , 1 },     // PB_HousingShortage_FirstTimeHotels
   { WAV_pb327   , 3 },     // PB_HousingShortage_NotFirstTime
   { WAV_pb330   , 1 },     // PB_Auction_HurryPlayersBid
   { WAV_pb331   , 2 },     // PB_Auction_Building
   { WAV_pb333   , 1 },     // PB_Auction_Once
   { WAV_pb334   , 1 },     // PB_Auction_Twice
   { WAV_pb335   , 3 },     // PB_Auction_LostTimeUp
   { WAV_pb338   , 3 },     // PB_RaisingMoneySuggestion
   { WAV_pb341   , 3 },     // PB_HumanReplacedByComputerPlayer
   { WAV_pb344   , 3 },     // PB_Trade_FirstTimeFuture
   { WAV_pb347   , 3 },     // PB_Trade_FirstTimeImmunity
   { WAV_pb350   , 4 },     // PB_Bankrupt
   { WAV_pb354   , 3 },     // PB_GameOver
   { WAV_pb357   , 3 },     // PB_PlayAgain
   { WAV_pb360   , 5 },     // PB_Exit
   { WAV_pb365   , 6 },     // PB_Proposal_GoodDeal
   { WAV_pb371   , 6 },     // PB_Proposal_NotGoodDeal
   { WAV_pb376+1 , 51},     // PB_BoardSquareNames_US - REMOVED, Unused (s_ series had em city specific)
   { WAV_pba01   , 7 },     // PB_WelcomeGame
   { WAV_pbb01   , 6 },     // PB_AnnouncePreviousHighScoresIfAny
   { WAV_pbc01   , 5 },     // PB_ChooseNetworkOrLocalGame
   { WAV_pbd01   , 5 },     // PB_NewPlayerClickNameOrPressButton
   { WAV_pbe01   , 5 },     // PB_NewPlayerEnterName
   { WAV_pbf01   , 5 },     // PB_NameComputerPlayer
   { WAV_pbg01   , 5 },     // PB_HumanPlayerPickToken
   { WAV_pbh01   , 5 },     // PB_ChooseTokenForComputerPlayer
   { WAV_pbi01   , 5 },     // PB_ChoosePlayer_HumanOrComputer
//   { WAV_pbj01   , 8 },     // PB_ChooseAIDifficultyLevel
   { WAV_pbj01   , 5 },     // PB_ChooseAIDifficultyLevel
   { WAV_pbk01   , 1 },     // PB_SummaryScreenWithLessThanTwoPlayers
   { WAV_pbk02   , 3 },     // PB_SummaryScreenWithTwoToFivePlayers
   { WAV_pbk05   , 2 },     // PB_SummaryScreenWithSixPlayers
   { WAV_pbl01   , 10},     // PB_ChooseClassicOrCityBoard_AnyVersion
   { WAV_pbn01   , 7 },     // PB_ChooseClassicOrCityBoard_Europe
   { WAV_pbo01   , 7 },     // PB_ChooseClassicOrCityBoard_US
   { WAV_pbp01   , 5 },     // PB_ChooseCurrency
   { WAV_pbq01   , 4 },     // PB_ChooseStandardOrCustomRules
   { WAV_pbr01   , 3 },     // PB_NegotiateRules
#if USA_VERSION
   { WAV_pbs01   , 5 },     // PB_Doubles
   { WAV_pbt01   , 5 },     // PB_RollAgain
   { WAV_pbu01   , 5 },     // PB_SecondTimeRollingDoubles
   { WAV_pbv01   , 5 },     // PB_ThirdTimeRollingDoubles
   { WAV_pbw01   , 5 },     // PB_CustomRulesScreenHost
   { WAV_pbx01   , 5 },     // PB_NonhostPlayersWatchRuleChange
   { WAV_pby01   , 5 },     // PB_HumanInitiatesTrade
   { WAV_pbz01   , 2 }      // PB_AIInitiatesTrade
#endif
};
/************************************************************/

/************************************************************/
/* UDPENNY_PennybagsSpecific                                */
/************************************************************/
void UDPENNY_PennybagsSpecific (int sound, int clipaction)
{
    BOOL            skip            = FALSE;
    BOOL            LockNewSound    = FALSE;
    BOOL            waitCheck       = FALSE;
    LE_DATA_DataId  tempId          = LED_EI;
//    int             numAlternates   = 0;

    if (sound == LED_EI) return;

    //TRACE("Penny speci*, action %d time %d\n", clipaction, LE_TIME_TickCount);
    // not sure if this will work yet
    // if user chose the option to remove host comment, then simply return
    if(!DISPLAY_state.IsOptionHostCommentsOn)   return;


    // This section determines the status of any existing sounds playing, including stopping
    // sounds to make the status 'all quiet' if clipping is requested.
    switch (clipaction)
    {
        case Sound_ClipOldSoundIfPlaying:
            UD_SOUND_KillAllTalkingSounds();
            break;

        case Sound_ClipOldSoundIfPlayingWithLock:
            UD_SOUND_KillAllTalkingSounds();
            LockNewSound = TRUE;
            break;

        case Sound_SkipIfOldSoundPlaying:
            skip = UD_SOUND_CheckForTalkingSounds();
            break;

        case Sound_WaitForAnyOldSoundThenPlay:
            waitCheck = UD_SOUND_CheckForTalkingSoundsWithLock(); // Note the new sound wont be locked - add another type if you need that feature.
            break;

        case Sound_WaitForAnyOldSoundThenPlayWithLock:
            waitCheck = UD_SOUND_CheckForTalkingSoundsWithLock();
            LockNewSound = TRUE;
            break;

        default:
            break;
    }

    if (!skip)
    {
        tempId = LED_IFT(DAT_LANGDIALOG, sound);
#if !USA_VERSION
        // Property announcments are in a different datafile.  Check for em.
        if( sound >= WAV_s_010201 )
          tempId = LED_IFT(DAT_BOARD, sound);
#endif
        if (waitCheck)
        {
            // Queue the sound - it will start when the lock clears.
            DISPLAY_state.SoundToPlayPostLock = tempId;
            DISPLAY_state.SoundPlayedPostVolume = 70;
            DISPLAY_state.SoundToPlayPostLockToken = RULE_MAX_PLAYERS; // We can use this to flag PB as the speaker.
        }
        else
        {
            DISPLAY_state.PennybagsLastSoundStarted = tempId;
            LE_SEQNCR_Start (DISPLAY_state.PennybagsLastSoundStarted, 0); // Warning sound
            LE_SEQNCR_SetVolume (DISPLAY_state.PennybagsLastSoundStarted, 0, 70);

            if (LockNewSound) UDSOUND_WatchSoundWithQueueLock (DISPLAY_state.PennybagsLastSoundStarted);
        }
        unprocessedSoundIssued = TRUE;
    }
  DISPLAY_state.SoundSystemClearToTest = FALSE;// Make sure sequences begin before escalation code runs.

}
/************************************************************/

/************************************************************/
/* UDPENNY_PennybagsGeneral                                 */
/************************************************************/
void UDPENNY_PennybagsGeneral (int soundIndex, int clipaction)
{
    BOOL            skip            = FALSE;
    BOOL            LockNewSound    = FALSE;
    BOOL            waitCheck       = FALSE;
    LE_DATA_DataId  tempId          = LED_EI;
    int             numAlternates   = 0;

    if (soundIndex == NULL) return;

    //TRACE("Penny sound, action %d time %d\n", clipaction, LE_TIME_TickCount);
    // not sure if this will work yet
    // if user chose the option to remove host comment, then simply return
    if(!DISPLAY_state.IsOptionHostCommentsOn)   return;


    // This section determines the status of any existing sounds playing, including stopping
    // sounds to make the status 'all quiet' if clipping is requested.
    //if( soundIndex >= 30 && soundIndex <= 40 )
    //  TRACE( "\n\n>>sound issued for %d", soundIndex );
    switch (clipaction)
    {
        case Sound_ClipOldSoundIfPlaying:
            UD_SOUND_KillAllTalkingSounds();
            break;

        case Sound_ClipOldSoundIfPlayingWithLock:
            UD_SOUND_KillAllTalkingSounds();
            LockNewSound = TRUE;
            break;

        case Sound_SkipIfOldSoundPlaying:
            skip = UD_SOUND_CheckForTalkingSounds();
            break;

        case Sound_WaitForAnyOldSoundThenPlay:
            waitCheck = UD_SOUND_CheckForTalkingSoundsWithLock(); // Note the new sound wont be locked - add another type if you need that feature.
            break;

        case Sound_WaitForAnyOldSoundThenPlayWithLock:
            waitCheck = UD_SOUND_CheckForTalkingSoundsWithLock(); // Note the new sound wont be locked - add another type if you need that feature.
            LockNewSound = TRUE;
            break;

        default:
            break;
    }

    if (!skip)
    {
        tempId          = UDSOUND_Pennybags_Sound[soundIndex][0];
        numAlternates   = (int) UDSOUND_Pennybags_Sound[soundIndex][1];

        if (numAlternates <= 0) numAlternates = 0;
        else                    tempId += rand()%numAlternates;

        if (tempId == 0 || numAlternates == 0) skip = TRUE;
    }
    // Special check, AI strength lines override
    if( soundIndex == PB_ChooseAIDifficultyLevel )
    {// Limited to 5 instead of 8 by array, but entry 3 is to be screened also
      if( tempId == UDSOUND_Pennybags_Sound[soundIndex][0] + 2 )
        tempId = UDSOUND_Pennybags_Sound[soundIndex][0];
    }

    if (!skip)
    {
        if (waitCheck)
        {
            // Queue the sound - it will start when the lock clears.
            DISPLAY_state.SoundToPlayPostLock = LED_IFT(DAT_LANGDIALOG, tempId);
            DISPLAY_state.SoundToPlayPostLockToken = RULE_MAX_PLAYERS; // We can use this to flag PB as the speaker.
            DISPLAY_state.SoundPlayedPostVolume = 70;
        }
        else
        {
            DISPLAY_state.PennybagsLastSoundStarted = LED_IFT(DAT_LANGDIALOG, tempId);
            LE_SEQNCR_Start (DISPLAY_state.PennybagsLastSoundStarted, 0); // Warning sound
            LE_SEQNCR_SetVolume (DISPLAY_state.PennybagsLastSoundStarted, 0, 70);

            if (LockNewSound) UDSOUND_WatchSoundWithQueueLock (DISPLAY_state.PennybagsLastSoundStarted);
        }
        unprocessedSoundIssued = TRUE;
    }

  DISPLAY_state.SoundSystemClearToTest = FALSE;// Make sure sequences begin before escalation code runs.
}
/************************************************************/

/************************************************************/
/* UDPENNY_StartGame                                        */
/************************************************************/
void UDPENNY_StartGame (void)
{
    // int clipaction = Sound_ClipOldSoundIfPlaying;
    int clipaction = Sound_ClipOldSoundIfPlayingWithLock;
    // int clipaction = Sound_SkipIfOldSoundPlaying;
    // int clipaction = Sound_WaitForAnyOldSoundThenPlay;

    // FIXME - E3 hack, don't be this lazy...
    UDSOUND_BackgroundMusicOn();

    UDPENNY_PennybagsGeneral(PB_WelcomeGame, clipaction);
}
/************************************************************/

/************************************************************/
/*  UDPENNY_PassedGo                                        */
/************************************************************/
void UDPENNY_PassedGo (void)
{// NOTE if we land on go, the land on sub handles this sound (@#$%) - we wont be called.
    // int clipaction = Sound_ClipOldSoundIfPlaying;
    // int clipaction = Sound_ClipOldSoundIfPlayingWithLock;
 //   int clipaction = Sound_SkipIfOldSoundPlaying;
      int clipaction = Sound_WaitForAnyOldSoundThenPlay;

    UDPENNY_PennybagsGeneral (PB_CollectMoney_Go , clipaction);
}
/************************************************************/

/************************************************************/
/* UDPENNY_NextPlayer                                       */
/************************************************************/
void UDPENNY_NextPlayer (int player)
{
  BOOL computer_player = UICurrentGameState.Players[player].AIPlayerLevel;//Warning - identifies non-local humans as human (for most sound, they are treated as AI)
  // int clipaction = Sound_ClipOldSoundIfPlaying;
  // int clipaction = Sound_ClipOldSoundIfPlayingWithLock;
  // int clipaction = Sound_SkipIfOldSoundPlaying;
  int clipaction = Sound_WaitForAnyOldSoundThenPlay;

  // We always do the PB introduction now -   FIXME - these reference dice, wrong when in jail.
  // Generic PennyBags dice roll prompt.
  if ((rand()%100) > 92)
  {
    UDPENNY_PennybagsGeneral (PB_RollDice_Generic, clipaction);
  }
  // Specific PennyBags dice roll prompt & process invalid player values into generic dice roll prompt
  else
  {
    if ((player >= 0)&&(player <= 10)) UDPENNY_PennybagsGeneral (PB_RollDice_Cannon+UICurrentGameState.Players[player].token, clipaction);
    else                               UDPENNY_PennybagsGeneral (PB_RollDice_Generic,       clipaction);
  }

  // Occasionally we will let the tokens give their lines - not local humans, not if there is already a sound lock.
  //if( /*!SlotIsALocalHumanPlayer[player] &&*/ DISPLAY_state.SoundToPlayPostLock == LED_EI ) //!DISPLAY_state.SoundLockGameQueueLockActive )
  {
    // If first move
    if (!RULE_PlayersFirstMoveWasMade(player))
    {// WARNING: Note that I am manually locking the intro sound - LE_SEQNCR has not yet been processed.
      // NOTE: this has been handled
      UDSOUND_WatchSoundWithQueueLock(DISPLAY_state.PennybagsLastSoundStarted);
      UDSOUND_TokenGenericPlaySound (UICurrentGameState.Players[player].token, TKS_Intro, clipaction);
    }
    else
    {
      if (computer_player && rand()%8 == 0)// Not too often
      {
        UDSOUND_WatchSoundWithQueueLock(DISPLAY_state.PennybagsLastSoundStarted);
        if (UICurrentGameState.Players[player].currentSquare > 30 && UICurrentGameState.Players[player].currentSquare < 40)
        {
          UDSOUND_TokenGenericPlaySound (UICurrentGameState.Players[player].token, TKS_StartSide4Only, clipaction);
        }
        else UDSOUND_TokenGenericPlaySound (UICurrentGameState.Players[player].token, TKS_StartTurnGeneric, clipaction);
      }
      else
      {
      }
    }
  }
}
/************************************************************/

/************************************************************/
/*  UDPENNY_AnnounceRoll                                        */
/************************************************************/
void UDPENNY_AnnounceRoll (int roll)
{//PB_SayDiceRoll_2
    int clipaction = Sound_WaitForAnyOldSoundThenPlay;

    //Skip the announcement if animations off as they get clipped.
    if( !DISPLAY_state.IsOptionTokenAnimationsOn )
      return;

    //TRACE( "\n %d doubles \n", UICurrentGameState.NumberOfDoublesRolled );// Not maintained in our copy of gamestate.
    // FIXME - we have to know if coubles have been rolled for lines.
    // WARNING: Custom rules can change how many doubles get you to jail - some lines say '2 down one to go...'
    UDPENNY_PennybagsGeneral (PB_SayDiceRoll_2 + roll - 2 , clipaction);
}
/************************************************************/

/************************************************************/
/* UDPENNY_BoughtProperty                                   */
/************************************************************/
void UDPENNY_BoughtProperty (int property, int player)
{
  BOOL computer_player = UICurrentGameState.Players[player].AIPlayerLevel;
  // int clipaction = Sound_ClipOldSoundIfPlaying;
  // int clipaction = Sound_ClipOldSoundIfPlayingWithLock;
  // int clipaction = Sound_SkipIfOldSoundPlaying;
  int clipaction = Sound_WaitForAnyOldSoundThenPlay;

  if( UDPENNY_IsThisPropertyAMonopoly( property ) )
  {// Then we must have just filled the monopoly
    UDPENNY_PennybagsGeneral (PB_PlayerAcquiredMonopoly, clipaction);
  } else
  {

      if ((property == 12)||(property == 28))
      {
        if (computer_player)    UDSOUND_TokenGenericPlaySound (UICurrentGameState.Players[player].token, TKS_GenericGood, clipaction);
        else                    UDPENNY_PennybagsGeneral (PB_BuyUtility, clipaction);
      }
      else if ((property == 5)||(property == 15)||(property == 25)||(property == 35))
      {
        if (computer_player)    UDSOUND_TokenGenericPlaySound (UICurrentGameState.Players[player].token, TKS_GenericGood, clipaction);
        else                    UDPENNY_PennybagsGeneral (PB_BuyRailroad, clipaction);
      }
      else if ((property >= 1)&&(property <= 9))
      {
        if (computer_player)    UDSOUND_TokenGenericPlaySound (UICurrentGameState.Players[player].token, TKS_BuySide1, clipaction);
        else                    UDPENNY_PennybagsGeneral (PB_BuyBoardSide1, clipaction);
      }
      else if ((property >= 11)&&(property <= 19))
      {
        if (computer_player)    UDSOUND_TokenGenericPlaySound (UICurrentGameState.Players[player].token, TKS_BuySide2, clipaction);
        else                    UDPENNY_PennybagsGeneral (PB_BuyBoardSide2, clipaction);
      }
      else if ((property >= 21)&&(property <= 29))
      {
        if (computer_player)    UDSOUND_TokenGenericPlaySound (UICurrentGameState.Players[player].token, TKS_BuySide3, clipaction);
        else                    UDPENNY_PennybagsGeneral (PB_BuyBoardSide3, clipaction);
      }
      else if ((property >= 31)&&(property <= 39))
      {
        if (computer_player)    UDSOUND_TokenGenericPlaySound (UICurrentGameState.Players[player].token, TKS_BuySide4, clipaction);
        else                    UDPENNY_PennybagsGeneral (PB_BuyBoardSide4, clipaction);
      }
  }
}
/************************************************************/

/************************************************************/
/* UDPENNY_ChooseToAuctionProperty                          */
/************************************************************/
void UDPENNY_ChooseToAuctionProperty( int property, int player )
{
    BOOL computer_player = UICurrentGameState.Players[player].AIPlayerLevel;
    // int clipaction = Sound_ClipOldSoundIfPlaying;
    // int clipaction = Sound_ClipOldSoundIfPlayingWithLock;
    int clipaction = Sound_SkipIfOldSoundPlaying;
    // int clipaction = Sound_WaitForAnyOldSoundThenPlay;

    if (computer_player)    UDSOUND_TokenGenericPlaySound (UICurrentGameState.Players[player].token, TKS_ChoseAuction, clipaction);
//    else                    UDPENNY_PennybagsGeneral (???, clipaction);   // Mike: Specific sound needed?
}
/************************************************************/

/************************************************************/
/* UDPENNY_BuyOrAuction                                     */
/************************************************************/
void UDPENNY_BuyOrAuction (int player)
{
    BOOL computer_player = UICurrentGameState.Players[player].AIPlayerLevel;
    // int clipaction = Sound_ClipOldSoundIfPlaying;
    // int clipaction = Sound_ClipOldSoundIfPlayingWithLock;
    // int clipaction = Sound_SkipIfOldSoundPlaying;
    int clipaction = Sound_WaitForAnyOldSoundThenPlay;

    if (computer_player)
    {
        // Mike: Specific sound needed?
    }
    else
    {
        UDPENNY_PennybagsGeneral (PB_TokenLandsOnUnownedProperty, clipaction);
    }
}
/************************************************************/

/************************************************************/
/* UDPENNY_LandedOnSquare                                   */
/************************************************************/
void UDPENNY_LandedOnSquare (int player, int square)
{
    int TokenT = UICurrentGameState.Players[player].token;
    BOOL computer_player = UICurrentGameState.Players[player].AIPlayerLevel;
    // int clipaction = Sound_ClipOldSoundIfPlaying;
  //int clipaction = Sound_ClipOldSoundIfPlayingWithLock;
    // int clipaction = Sound_SkipIfOldSoundPlaying;
     int clipaction = Sound_WaitForAnyOldSoundThenPlay;

    int totalWorth = AI_Get_Total_Worth(&UICurrentGameState, player);
    int t = 0;
    int s = 0;

    switch (square)
    {
        case  7:        // Chance
        case 22:        // Chance
        case 36:        // Chance
            // Mike: Should we handle the computer player differently from the human player?
            UDPENNY_PennybagsGeneral (PB_LandOn_Chance, clipaction);    // Mike: Specific sound needed?
            break;

        case  2:        // Community Chest
        case 17:        // Community Chest
        case 33:        // Community Chest
            // Mike: Should we handle the computer player differently from the human player?
            UDPENNY_PennybagsGeneral (PB_LandOn_CommunityChest, clipaction);    // Mike: Specific sound needed?
            break;

        case 20:        // Free Parking
            // Mike: Should we handle the computer player differently from the human player?
            if(UICurrentGameState.GameOptions.freeParkingPot)
            {
                if ((rand()%100) < 20)  UDPENNY_PennybagsGeneral(PB_LandOn_FreeParkingWithFreeParkingRuleOn, clipaction);
                else                    UDSOUND_TokenGenericPlaySound(TokenT, TKS_LandOnFreeParkingPaying, clipaction);
            }
            else
            {
                if ((rand()%100) < 20)  UDPENNY_PennybagsGeneral(PB_LandOn_FreeParking, clipaction);
                else                    UDSOUND_TokenGenericPlaySound(TokenT, TKS_LandFreeParkingNotPaying, clipaction);
            }
            break;

        case  0:        // Go NOTE: If passing go, there is UDPENNY_PassedGo that gives the sound.
            if (UICurrentGameState.GameOptions.doubleSalaryOnGo)
            {
                if (computer_player)    UDSOUND_TokenGenericPlaySound(TokenT, TKS_HitGOWithDoubleCashRule, clipaction);
                else                    UDPENNY_PennybagsGeneral (PB_CollectMoney_LandedOnGoAndDoublePayRuleInEffect, clipaction);
            }
            else UDPENNY_PennybagsGeneral (PB_CollectMoney_Go, clipaction);
            break;

        case 30:        // Go To Jail - handled by the go to jail message (notify jump forward) to catch all go to jails.
            break;

        case  4:        // Income Tax
            if (computer_player)
            {
                if(totalWorth < 1000) // pretty poor...
                {
                    UDSOUND_TokenGenericPlaySound(TokenT, TKS_LandOnIncomeTaxPoor, clipaction);
                }
                else
                {
                    UDSOUND_TokenGenericPlaySound(TokenT, TKS_LandOnIncomeTaxRich, clipaction);
                }
            }
            else // Human Player
            {
                UDPENNY_PennybagsGeneral(PB_LandOn_IncomeTax , clipaction);
            }
            break;

        case 10:        // Just Visiting
            s = 0;

            for (t = 0; t < RULE_MAX_PLAYERS; t++)
            {
                if (UICurrentGameState.Players[t].currentSquare == 40) s++;
            }

            if (s)   UDSOUND_TokenGenericPlaySound(TokenT, TKS_LandOnJailNotEmpty, clipaction);
            else
            {
                if (computer_player)    UDSOUND_TokenGenericPlaySound(TokenT, TKS_LandOnJailJustVisiting, clipaction);
                else                    UDPENNY_PennybagsGeneral(PB_LandOn_JustVisiting, clipaction);
            }
            break;

        case 38:        // Luxury Tax
            if (computer_player)
            {
                UDSOUND_TokenGenericPlaySound(TokenT, TKS_LandOnLuxuryTax, clipaction);
            }
            else // Human Player
            {
                UDPENNY_PennybagsGeneral(PB_LandOn_LuxuryTax, clipaction);
            }
            break;

        case 41: // Off board
          break;

        default:    // Must be a property square
            // Announce the square, & give comments (this will surely have collision problems)
              if ((square > 0)||(square < 39))        // But lets confirm the square number falls in the correct range anyway
              {
                // Pennybags announces square, always something else to say so put the lock in now (it hasn't started yet, this hack makes the second sound wait for the first which doesn't happen when both are ussued simultaneously otherwise)
#if USA_VERSION
                UDPENNY_PennybagsSpecific( WAV_s_010000 + square + max(DISPLAY_state.city, 0)*41, Sound_SkipIfOldSoundPlaying );// City specific in US version.
                //UDSOUND_WatchSoundWithQueueLock( DISPLAY_state.PennybagsLastSoundStarted );
#else
                // if board is a custom board, Display_state.city == -1, giving the wrong sound
                // so to fix, use the install langid for a custom board
                int tempCity = DISPLAY_state.city;
                if(tempCity == -1)
                    tempCity = iLangId -2;

                UDPENNY_PennybagsSpecific( WAV_s_010201 + propconv[square] + max(tempCity, 0)*28, Sound_SkipIfOldSoundPlaying );
                //UDSOUND_WatchSoundWithQueueLock(DISPLAY_state.PennybagsLastSoundStarted);
#endif
                // If the sound started, this will get a watch on it.
                UDSOUND_WatchSoundWithQueueLock (DISPLAY_state.PennybagsLastSoundStarted);
                clipaction = Sound_WaitForAnyOldSoundThenPlay;
                if (computer_player)
                {
                    if (UICurrentGameState.Squares[square].owner == RULE_NOBODY_PLAYER)     // If unowned
                    {
                        if      ((square >= 31) && (square <= 34))  UDSOUND_TokenGenericPlaySound(TokenT, TKS_LandOnUnownedGreen, clipaction);
                        else if ((square >= 37) && (square <= 39))  UDSOUND_TokenGenericPlaySound(TokenT, TKS_LandOnUnownedPPOrBoardwalk, clipaction);
                        else if ((square == 12) || (square == 28))
                        {
                            int t = 12;

                            if (square == 12) t = 28;

                            // this would give us the second utility
                            if (UICurrentGameState.Squares[t].owner == player)    UDSOUND_TokenGenericPlaySound(TokenT, TKS_LandOnUnownedUtilityHoldingOther, clipaction);
                            else                                                  UDSOUND_TokenGenericPlaySound(TokenT, TKS_LandOnUnownedUtility, clipaction);
                        }
                        else if ((square == 5) || (square == 15) || (square == 25) || (square == 35))
                        {
                            int s = 0;
                            int t;

                            for (t = 5; t < 40; t += 10)
                            {
                                if (UICurrentGameState.Squares[t].owner == player) s++;
                            }

                            // This would give him all 4 railroads
                            if (s == 3) UDSOUND_TokenGenericPlaySound(TokenT, TKS_LandOnUnownedRailroadHoldingOther3, clipaction);
                            else        UDSOUND_TokenGenericPlaySound(TokenT, TKS_LandOnUnownedRailroad, clipaction);
                        }
                        else UDSOUND_TokenGenericPlaySound(TokenT, TKS_GenericGood, clipaction);     // Mike: Specific sound needed?
                    }
                    else if (UICurrentGameState.Squares[square].owner == player)            // If owned by current player
                    {
                        UDSOUND_TokenGenericPlaySound(TokenT, TKS_LandOnOwnProperty , clipaction);
                    }
                    else                                                                    // Owned by another player
                    {// We have a problem if we got here because of a card - we don't pay til after a dice roll.  Just shut up.
                      if( !DISPLAY_state.JustReadACardHack )
                        UDPENNY_ReactToLandingOnOtherPlayersProperty(player, square);
                    }
                }
                else // Human Player
                {
                    if (UICurrentGameState.Squares[square].owner == RULE_NOBODY_PLAYER)     // If unowned
                    {
                        // Nothing to say?  Predict excitement FIXME
                    }
                    else if (UICurrentGameState.Squares[square].owner == player)            // If owned by current player
                    {
                        UDSOUND_TokenGenericPlaySound(TokenT, TKS_LandOnOwnProperty, clipaction);
                    }
                    else                                                                    // Owned by another player
                    {// We have a problem if we got here because of a card - we don't pay til after a dice roll.  Just shut up.
                      if( !DISPLAY_state.JustReadACardHack )
                        UDPENNY_ReactToLandingOnOtherPlayersProperty(player, square);
                    }
                }
            }
            #if CE_ARTLIB_EnableDebugMode
            else
            {
                wsprintf(LE_ERROR_DebugMessageBuffer, "UDPENNY_LandedOnSquare(): Not supposed to get here. Square = %d", square);
                LE_ERROR_DebugMsg(LE_ERROR_DebugMessageBuffer, 1);
            }
            #endif

            break;
     }
     DISPLAY_state.JustReadACardHack = FALSE;
}
/************************************************************/

/************************************************************/
/* UDPENNY_IsThisPropertyAMonopoly                          */
/************************************************************/
BOOL UDPENNY_IsThisPropertyAMonopoly(int property)
{
    BOOL return_value = FALSE;

    int x = UICurrentGameState.Squares[ property ].owner;

    if      (((property >=  1) && (property <=  3)) && (UICurrentGameState.Squares[ 1].owner == x && UICurrentGameState.Squares[ 3].owner == x))                                          return_value = TRUE;
    else if (((property >=  6) && (property <=  9)) && (UICurrentGameState.Squares[ 6].owner == x && UICurrentGameState.Squares[ 8].owner ==  x && UICurrentGameState.Squares[ 9].owner == x))  return_value = TRUE;
    else if (((property >= 11) && (property <= 14)) && (UICurrentGameState.Squares[11].owner == x && UICurrentGameState.Squares[13].owner ==  x && UICurrentGameState.Squares[14].owner == x))  return_value = TRUE;
    else if (((property >= 16) && (property <= 19)) && (UICurrentGameState.Squares[16].owner == x && UICurrentGameState.Squares[18].owner ==  x && UICurrentGameState.Squares[19].owner == x))  return_value = TRUE;
    else if (((property >= 21) && (property <= 24)) && (UICurrentGameState.Squares[21].owner == x && UICurrentGameState.Squares[23].owner ==  x && UICurrentGameState.Squares[24].owner == x))  return_value = TRUE;
    else if (((property >= 26) && (property <= 29)) && (UICurrentGameState.Squares[26].owner == x && UICurrentGameState.Squares[27].owner ==  x && UICurrentGameState.Squares[29].owner == x))  return_value = TRUE;
    else if (((property >= 31) && (property <= 34)) && (UICurrentGameState.Squares[31].owner == x && UICurrentGameState.Squares[32].owner ==  x && UICurrentGameState.Squares[34].owner == x))  return_value = TRUE;
    else if (((property >= 37) && (property <= 39)) && (UICurrentGameState.Squares[37].owner == x && UICurrentGameState.Squares[39].owner == x))                                          return_value = TRUE;

    //int x = property;
    //if ( property == x == 8 )
    if( return_value )
      property = 4; //testing

    return (return_value);
}
/************************************************************/

/************************************************************/
/* UDPENNY_ReactToLandingOnOtherPlayersProperty             */
/************************************************************/
void UDPENNY_ReactToLandingOnOtherPlayersProperty(int player, int square)
{
    int TokenT = UICurrentGameState.Players[player].token;
    BOOL computer_player = UICurrentGameState.Players[player].AIPlayerLevel;
    // int clipaction = Sound_ClipOldSoundIfPlaying;
    // int clipaction = Sound_ClipOldSoundIfPlayingWithLock;
    // int clipaction = Sound_SkipIfOldSoundPlaying;
    int clipaction = Sound_WaitForAnyOldSoundThenPlay;

    if (UICurrentGameState.Squares[square].mortgaged)
    {
        UDSOUND_TokenGenericPlaySound(TokenT, TKS_LandOnMorgagedProperty, clipaction);
    }
    else
    {
        if ((rand()%100 < 50) && UDPENNY_IsThisPropertyAMonopoly(square))    // React to house hotel situation
        {
            if (UICurrentGameState.Squares[square].houses == UICurrentGameState.GameOptions.housesPerHotel)  // A hotel
            {
                UDSOUND_TokenGenericPlaySound(TokenT, TKS_LandOnOtherHotel, clipaction);
            }
            else if (UICurrentGameState.Squares[square].houses == 4)  // 4 houses
            {
                UDSOUND_TokenGenericPlaySound(TokenT, TKS_LandOn4Houses, clipaction);
            }
            else if (UICurrentGameState.Squares[square].houses == 0)  // 0 houses
            {
                // Mike: Make sure that a simple monopoly exists.
                UDSOUND_TokenGenericPlaySound(TokenT, TKS_LandOn0Houses, clipaction);
            }
            else    // 1 to 3 houses
            {
                UDSOUND_TokenGenericPlaySound(TokenT, TKS_LandOn1To3Houses, clipaction);
            }

        }
        else                        // React to rent to be paid situation
        {
//            int wealth          = UDUTILS_CashPlayerCouldRaiseIfNeeded  (player);
            int wealth          = AI_Get_Total_Worth                    (&UICurrentGameState, player);
            int rent            = AI_Rent_If_Stepped_On                 (&UICurrentGameState, square, RULE_PropertySetOwnedByPlayer(&UICurrentGameState,player));

            if      ((rent * 100) >= (wealth * 75))  UDSOUND_TokenGenericPlaySound(TokenT, TKS_LandOnBiggerHit, clipaction);
            else if ((rent * 100) >= (wealth * 50))  UDSOUND_TokenGenericPlaySound(TokenT, TKS_LandOnBigHit   , clipaction);
            else if ((rent * 100) >= (wealth * 25))  UDSOUND_TokenGenericPlaySound(TokenT, TKS_LandOnLowHit   , clipaction);
            else if ((rent * 100) >= (wealth * 10))  UDSOUND_TokenGenericPlaySound(TokenT, TKS_LandOnLowerHit , clipaction);
            else if ((rent * 100) >= (wealth *  4))  UDSOUND_TokenGenericPlaySound(TokenT, TKS_LandOnLowLowHit, clipaction);
            else // Watch out - can be a utility and we may have gotten here by card, we don't pay yet. FIXME
                                                     UDSOUND_TokenGenericPlaySound(TokenT, TKS_LandOnLowestHit, clipaction);
        }
    }
}
/************************************************************/


/************************************************************/
/* UDPENNY_GoToJail
 *  PB or token comments on the Go To Jail
/************************************************************/
void UDPENNY_GoToJail( void )
{
  int s, t, TokenT = UICurrentGameState.Players[UICurrentGameState.CurrentPlayer].token;

  for (t = 0, s = 0; t < SQ_MAX_SQUARE_TYPES; t++)
  {
    if (UICurrentGameState.Squares[t].owner != RULE_NOBODY_PLAYER) s++;
  }

  // If late
  if (s > 25)
  {
    //if( !SlotIsALocalHumanPlayer[UICurrentGameState.CurrentPlayer] )
      UDSOUND_TokenGenericPlaySound(TokenT, TKS_LandOnGoToJailLate, Sound_WaitForAnyOldSoundThenPlay);
    //else
      // This is a dangerous one - we need to be sure we are 'happy' to go to jail first due to PB lines.
      //UDPENNY_PennybagsGeneral(PB_LandOn_GoToJail_Positive, Sound_WaitForAnyOldSoundThenPlay);
  }
  else
  {
    if( !SlotIsALocalHumanPlayer[UICurrentGameState.CurrentPlayer] )
      UDSOUND_TokenGenericPlaySound(TokenT, TKS_LandOnGoToJailEarly, Sound_WaitForAnyOldSoundThenPlay);
    else
      UDPENNY_PennybagsGeneral(PB_LandOn_GoToJail_Negative, Sound_WaitForAnyOldSoundThenPlay);
  }
};
/************************************************************/

// MMH: Deed Print Code Starts Here
/************************************************************/
/* UDPENNY_ReturnPropertyNumberFromLogicalSquare            */
/************************************************************/
short UDPENNY_ReturnTransIndexFromLogicalSquare(short square)
{
    short       return_value = -1;

    if ((square >= 0)||(square <= 39))
    {
        if      (square ==  1)  return_value =  0;          /* MEDITERRANEAN AVE.   */
        else if (square ==  3)  return_value =  1;          /* BALTIC AVE.          */
        else if (square ==  6)  return_value =  2;          /* ORIENTAL AVE.        */
        else if (square ==  8)  return_value =  3;          /* VERMONT AVE.         */
        else if (square ==  9)  return_value =  4;          /* CONNETICUT AVE.      */
        else if (square == 11)  return_value =  5;          /* ST. CHARLES PLACE    */
        else if (square == 13)  return_value =  6;          /* STATES AVE.          */
        else if (square == 14)  return_value =  7;          /* VIRGINIA AVE.        */
        else if (square == 16)  return_value =  8;          /* ST. JAMES PLACE      */
        else if (square == 18)  return_value =  9;          /* TENNESSEE AVE.       */
        else if (square == 19)  return_value = 10;          /* NEW YORK AVE.        */
        else if (square == 21)  return_value = 11;          /* KENTUCKY AVE.        */
        else if (square == 23)  return_value = 12;          /* INDIANA AVE.         */
        else if (square == 24)  return_value = 13;          /* ILLINOIS AVE.        */
        else if (square == 26)  return_value = 14;          /* ATLANTIC AVE.        */
        else if (square == 27)  return_value = 15;          /* VENTNOR AVE.         */
        else if (square == 29)  return_value = 16;          /* MARVIN GARDENS       */
        else if (square == 31)  return_value = 17;          /* PACIFIC AVE.         */
        else if (square == 32)  return_value = 18;          /* NO. CAROLINA AVE.    */
        else if (square == 34)  return_value = 19;          /* PENNSYLVANIA AVE.    */
        else if (square == 37)  return_value = 20;          /* PARK PLACE           */
        else if (square == 39)  return_value = 21;          /* BOARDWALK            */
        else if (square ==  5)  return_value = 22;          /* READING RAILROAD     */
        else if (square == 15)  return_value = 23;          /* PENNSYLVANIA R. R.   */
        else if (square == 25)  return_value = 24;          /* B. & O. RAILROAD     */
        else if (square == 35)  return_value = 25;          /* SHORT LINE R. R.     */
        else if (square == 12)  return_value = 26;          /* ELECTRIC COMPANY     */
        else if (square == 28)  return_value = 27;          /* WATER WORKS          */
        else
        {
            wsprintf(LE_ERROR_DebugMessageBuffer, "UDPENNY_ReturnTransIndexFromLogicalSquare(): Square value (%d) does not map to one of the property squares.",square);
            LE_ERROR_Msg (LE_ERROR_DebugMessageBuffer, LE_ERROR_OUTPUT_USER, FALSE);
        }
    }
    else
    {
        wsprintf(LE_ERROR_DebugMessageBuffer, "UDPENNY_ReturnTransIndexFromLogicalSquare(): Invalid range for input value (square = %d)",square);
        LE_ERROR_Msg (LE_ERROR_DebugMessageBuffer, LE_ERROR_OUTPUT_USER, FALSE);
    }

    return (return_value);
}
/************************************************************/

/************************************************************/
/* UDPENNY_ReturnColorForPropertySet                        */
/************************************************************/
DWORD UDPENNY_ReturnColorForPropertySet(short set_number, short language)
{
    DWORD return_value = LEG_MCR(  0,  0,  0);

    if (set_number == 0)
    {
        if      (language == NOTW_LANG_ENGLISH_US) return_value = LEG_MCR( 93, 40,133);
        else if (language == NOTW_LANG_FRENCH)     return_value = LEG_MCR(217, 91,187);
        else                                       return_value = LEG_MCR(134,103, 86);
    }
    else if (set_number == 1)
    {
        if      (language == NOTW_LANG_ENGLISH_US) return_value = LEG_MCR(213,235,250);
        else if (language == NOTW_LANG_FRENCH)     return_value = LEG_MCR( 35,158,206);
        else                                       return_value = LEG_MCR( 35,158,206);
    }
    else if (set_number == 2)
    {
        if      (language == NOTW_LANG_ENGLISH_US) return_value = LEG_MCR(214, 42,134);
        else if (language == NOTW_LANG_FRENCH)     return_value = LEG_MCR(134,  0,157);
        else                                       return_value = LEG_MCR(221,  3,109);
    }
    else if (set_number == 3)
    {
        if      (language == NOTW_LANG_ENGLISH_US) return_value = LEG_MCR(238,155,  0);
        else if (language == NOTW_LANG_FRENCH)     return_value = LEG_MCR(237,181,  0);
        else                                       return_value = LEG_MCR(253,128, 61);
    }
    else if (set_number == 4)
    {
        if      (language == NOTW_LANG_ENGLISH_US) return_value = LEG_MCR(223,  0, 41);
        else if (language == NOTW_LANG_FRENCH)     return_value = LEG_MCR(223,  0,  0);
        else                                       return_value = LEG_MCR(223,  0,  0);
    }
    else if (set_number == 5)
    {
        if      (language == NOTW_LANG_ENGLISH_US) return_value = LEG_MCR(249,244,  0);
        else if (language == NOTW_LANG_FRENCH)     return_value = LEG_MCR(255,255,  0);
        else                                       return_value = LEG_MCR(255,255,  0);
    }
    else if (set_number == 6)
    {
        if      (language == NOTW_LANG_ENGLISH_US) return_value = LEG_MCR(  0,164,112);
        else if (language == NOTW_LANG_FRENCH)     return_value = LEG_MCR(  0,128, 52);
        else                                       return_value = LEG_MCR( 37,154, 57);
    }
    else if (set_number == 7)
    {
        if      (language == NOTW_LANG_ENGLISH_US) return_value = LEG_MCR(  1,104,178);
        else if (language == NOTW_LANG_FRENCH)     return_value = LEG_MCR( 45, 65,144);
        else                                       return_value = LEG_MCR( 45, 65,144);
    }

    return (return_value);
}
/************************************************************/

/************************************************************/
/* UDPENNY_ReturnMoneyValueBasedOnMonatarySystem            */
/************************************************************/
// LAST MINUTE ADJUSTMENT(RK) - Value to INT since it is now used for score/trade/status etc cash display.
void UDPENNY_ReturnMoneyValueBasedOnMonatarySystem(int default_value, short monatary_system, ACHAR *StringPntr, BOOL print_symbol)
{
    long new_value;

#if USA_VERSION
    // Quick fix - for non-deed purposes life is easier to have this function operate correctly for US bucks.
    monatary_system = NOTW_MONA_US;
#endif

    if (monatary_system == NOTW_MONA_US)
    {
        new_value = (long)default_value;
        if (print_symbol)   Asprintf(StringPntr,A_T("$ %d"), new_value);
        else                Asprintf(StringPntr,A_T("%d"), new_value);
    }
    if (monatary_system == NOTW_MONA_UK)
    {
        new_value = (long)default_value;
        if (print_symbol)   Asprintf(StringPntr,A_T("£ %d"), new_value);
        else                Asprintf(StringPntr,A_T("%d"), new_value);
    }
    if (monatary_system == NOTW_MONA_FRENCH)
    {
        new_value = (long)(default_value*100);
        if (print_symbol)
        {
            // MH: Changed display to standardize thousands formatting characters
            //     Jonas said remove them as opposed to formatting the millions digits as well.
            //if (new_value/1000 > 0) Asprintf(StringPntr,A_T("F %d.%03d"), new_value/1000, new_value%1000);
            //else                    Asprintf(StringPntr,A_T("F %d"), new_value%1000);
            Asprintf(StringPntr,A_T("F %d"), new_value);
        }
        else
        {
            // MH: Changed display to standardize thousands formatting characters
            //     Jonas said remove them as opposed to formatting the millions digits as well.
            //if (new_value/1000 > 0) Asprintf(StringPntr,A_T("%d.%03d"), new_value/1000, new_value%1000);
            //else                    Asprintf(StringPntr,A_T("%d"), new_value%1000);
            Asprintf(StringPntr,A_T("%d"), new_value);
        }
    }
    if (monatary_system == NOTW_MONA_GERMAN)
    {
        new_value = (long)(default_value*20);
        if (print_symbol)
        {
            // MH: Changed display to standardize thousands formatting characters
            //     Jonas said remove them as opposed to formatting the millions digits as well.
            //if (new_value/1000 > 0) Asprintf(StringPntr,A_T("DM %d %03d"), new_value/1000, new_value%1000);
            //else                    Asprintf(StringPntr,A_T("DM %d"), new_value%1000);
            Asprintf(StringPntr,A_T("DM %d"), new_value);
        }
        else
        {
            // MH: Changed display to standardize thousands formatting characters
            //     Jonas said remove them as opposed to formatting the millions digits as well.
            //if (new_value/1000 > 0) Asprintf(StringPntr,A_T("%d %03d"), new_value/1000, new_value%1000);
            //else                    Asprintf(StringPntr,A_T("%d"), new_value%1000);
            Asprintf(StringPntr,A_T("%d"), new_value);
        }
    }
    if (monatary_system == NOTW_MONA_SPANISH)
    {
        new_value = (long)(default_value*100);
        // Spain doesn't have a symbol to print so no need to test print_symbol
        // MH: Changed display to standardize thousands formatting characters
        //     Jonas said remove them as opposed to formatting the millions digits as well.
        //if (new_value/1000 > 0) Asprintf(StringPntr,A_T("%d.%03d"), new_value/1000, new_value%1000);
        //else                    Asprintf(StringPntr,A_T("%d"), new_value%1000);
        Asprintf(StringPntr,A_T("%d"), new_value);
    }
    if (monatary_system == NOTW_MONA_DUTCH)
    {
        new_value = (long)(default_value*100);
        if (print_symbol)
        {
            // MH: Changed display to standardize thousands formatting characters
            //     Jonas said remove them as opposed to formatting the millions digits as well.
            //if (new_value/1000 > 0) Asprintf(StringPntr,A_T("f %d.%03d"), new_value/1000, new_value%1000);
            //else                    Asprintf(StringPntr,A_T("f %d"), new_value%1000);
            Asprintf(StringPntr,A_T("f %d"), new_value);
        }
        else
        {
            // MH: Changed display to standardize thousands formatting characters
            //     Jonas said remove them as opposed to formatting the millions digits as well.
            //if (new_value/1000 > 0) Asprintf(StringPntr,A_T("%d.%03d"), new_value/1000, new_value%1000);
            //else                    Asprintf(StringPntr,A_T("%d"), new_value%1000);
            Asprintf(StringPntr,A_T("%d"), new_value);
        }
    }
    if (monatary_system == NOTW_MONA_SWEDISH)
    {
        new_value = (long)(default_value*20);
        if (print_symbol)
        {
            // MH: Changed display to standardize thousands formatting characters
            //     Jonas said remove them as opposed to formatting the millions digits as well.
            //if (new_value/1000 > 0) Asprintf(StringPntr,A_T("KR %d %03d"), new_value/1000, new_value%1000);
            //else                    Asprintf(StringPntr,A_T("KR %d"), new_value%1000);
            Asprintf(StringPntr,A_T("KR %d"), new_value);
        }
        else
        {
            // MH: Changed display to standardize thousands formatting characters
            //     Jonas said remove them as opposed to formatting the millions digits as well.
            //if (new_value/1000 > 0) Asprintf(StringPntr,A_T("%d %03d"), new_value/1000, new_value%1000);
            //else                    Asprintf(StringPntr,A_T("%d"), new_value%1000);
            Asprintf(StringPntr,A_T("%d"), new_value);
        }
    }
    if (monatary_system == NOTW_MONA_FINNISH)
    {
        new_value = (long)(default_value*20);
        if (print_symbol)
        {
            // MH: Changed display to standardize thousands formatting characters
            //     Jonas said remove them as opposed to formatting the millions digits as well.
            //if (new_value/1000 > 0) Asprintf(StringPntr,A_T("%d.%03d MK"), new_value/1000, new_value%1000);
            //else                    Asprintf(StringPntr,A_T("%d MK"), new_value%1000);
            Asprintf(StringPntr,A_T("%d MK"), new_value);
        }
        else
        {
            // MH: Changed display to standardize thousands formatting characters
            //     Jonas said remove them as opposed to formatting the millions digits as well.
            //if (new_value/1000 > 0) Asprintf(StringPntr,A_T("%d.%03d"), new_value/1000, new_value%1000);
            //else                    Asprintf(StringPntr,A_T("%d"), new_value%1000);
            Asprintf(StringPntr,A_T("%d"), new_value);
        }
    }
    if (monatary_system == NOTW_MONA_DANISH)
    {
        new_value = (long)(default_value*20);
        if (print_symbol)
        {
            // MH: Changed display to standardize thousands formatting characters
            //     Jonas said remove them as opposed to formatting the millions digits as well.
            //if (new_value/1000 > 0) Asprintf(StringPntr,A_T("%d.%03d KR."), new_value/1000, new_value%1000);
            //else                    Asprintf(StringPntr,A_T("%d KR."), new_value%1000);
            Asprintf(StringPntr,A_T("%d KR."), new_value);
        }
        else
        {
            // MH: Changed display to standardize thousands formatting characters
            //     Jonas said remove them as opposed to formatting the millions digits as well.
            //if (new_value/1000 > 0) Asprintf(StringPntr,A_T("%d.%03d"), new_value/1000, new_value%1000);
            //else                    Asprintf(StringPntr,A_T("%d"), new_value%1000);
            Asprintf(StringPntr,A_T("%d"), new_value);
        }
    }
    if (monatary_system == NOTW_MONA_NORWEGIAN)
    {
        new_value = (long)(default_value*20);
        if (print_symbol)
        {
            // MH: Changed display to standardize thousands formatting characters
            //     Jonas said remove them as opposed to formatting the millions digits as well.
            //if (new_value/1000 > 0) Asprintf(StringPntr,A_T("KR. %d.%03d"), new_value/1000, new_value%1000);
            //else                    Asprintf(StringPntr,A_T("KR. %d"), new_value%1000);
            Asprintf(StringPntr,A_T("KR. %d"), new_value);
        }
        else
        {
            // MH: Changed display to standardize thousands formatting characters
            //     Jonas said remove them as opposed to formatting the millions digits as well.
            //if (new_value/1000 > 0) Asprintf(StringPntr,A_T("%d.%03d"), new_value/1000, new_value%1000);
            //else                    Asprintf(StringPntr,A_T("%d"), new_value%1000);
            Asprintf(StringPntr,A_T("%d"), new_value);
        }
    }
    if (monatary_system == NOTW_MONA_BELGIUM)
    {
        new_value = (long)(default_value*20);
        if (print_symbol)           Asprintf(StringPntr,A_T("F %d"), new_value);
        else                        Asprintf(StringPntr,A_T("%d"), new_value);
    }
    if (monatary_system == NOTW_MONA_SINGAPORE)
    {
        new_value = (long)(default_value*10);
        if (print_symbol)   Asprintf(StringPntr,A_T("$ %d"), new_value);
        else                Asprintf(StringPntr,A_T("%d"), new_value);
    }
    if (monatary_system == NOTW_MONA_AUSTRALIAN)
    {
        new_value = (long)(default_value);
        if (print_symbol)   Asprintf(StringPntr,A_T("$ %d"), new_value);
        else                Asprintf(StringPntr,A_T("%d"), new_value);
    }
    if (monatary_system == NOTW_MONA_EURO)
    {
        new_value = (long)(default_value);
        if (print_symbol)   Asprintf(StringPntr,A_T("€ %d"), new_value);
        else                Asprintf(StringPntr,A_T("%d"), new_value);
    }
}
/************************************************************/

/************************************************************/
/* UDPENNY_FitAndPrintJustifiedDeedText                     */
/************************************************************/
BOOL UDPENNY_FitAndPrintJustifiedDeedText(LE_DATA_DataId deedID, struct UDPENNY_PrintInfo *PI, ACHAR *StringPntr, BOOL test_do_not_print)
{
    #define MAGICAL_HORIZONTAL_SPACE        21
    BOOL    return_value = TRUE;
    short   width;
    ACHAR   *start;
    ACHAR   *end;
    ACHAR   *tmp;
    short   curr_y = PI->y;
    ACHAR   WORK_BUFFER[256];
    BOOL    vert_first = FALSE;

    if (PI->vert_center) vert_first = TRUE;

    LE_FONTS_ResetCharacteristics();
    if (PI->bold)   LE_FONTS_SetWeight(1000);
    if (PI->italic) LE_FONTS_SetItalic(TRUE);
    LE_FONTS_SetSize(PI->font_size);

    // Copy the string into a work buffer so the original is not destroyed
    #if CE_ARTLIB_UnicodeAlphabet
        wcscpy(WORK_BUFFER, StringPntr);
    #else
        strcpy(WORK_BUFFER, StringPntr);
    #endif
    start = WORK_BUFFER;

    // Find the end of the string.
    end = start;
    while (*end != 0) end ++;

    while (start<end)
    {
        width = LE_FONTS_GetStringWidth(start);
        tmp = end;

        while ((width - LE_FONTS_GetStringWidth(tmp)) > (DEED_WIDTH - (MAGICAL_HORIZONTAL_SPACE * 2)))
        {
            // Back up to the last space
            tmp --;     //
            while ((*tmp != 32)&&(tmp != start)) tmp --;

            if (tmp <= start)   // Then there are no space breaks that allow the current string to fit in this region
            {
                return_value = FALSE;
                break;
            }
        }

        if (return_value)
        {
            *tmp = 0;
            width = LE_FONTS_GetStringWidth(start);
            if (!test_do_not_print)
            {
                // If this line has to be vertically centered!
                if (PI->vert_center)
                {
                    // If this is the final part of the line
                    if (tmp+1>=end)
                    {
                        if (vert_first) // If there was only one part to the line
                        {
                            short hh    = LE_FONTS_GetStringHeight(start);
                            short diff  = (PI->h - hh)/2;

                            curr_y += diff;
                        }
                    }
                    vert_first = FALSE;
                }

                if      (PI->justification == 0)    LE_FONTS_Print(deedID, MAGICAL_HORIZONTAL_SPACE ,                   curr_y, PI->Colour, start);
                else if (PI->justification == 1)    LE_FONTS_Print(deedID, (DEED_WIDTH-width)/2,                        curr_y, PI->Colour, start);
                else                                LE_FONTS_Print(deedID, DEED_WIDTH-MAGICAL_HORIZONTAL_SPACE-width,   curr_y, PI->Colour, start);
            }
            curr_y += LE_FONTS_GetStringHeight(start);
            start = tmp + 1;
        }
        else break;
    }

    if ((test_do_not_print)&&(return_value))
    {
        short lower_y = PI->y + PI->h;

        // Test lower vertical boundry
        if (curr_y > lower_y)       // We have gone past our vertical boundry.
        {
            // If the difference is within our vertical_leeway
            if ((curr_y - PI->vertical_leeway) <= lower_y)
            {
              short t1 = lower_y - curr_y;

              if (t1 < PI->vertical_leeway) PI->y -= t1;                        // MMH: Be aware that this is modifying the PI structure
              else                          PI->y -= PI->vertical_leeway;       // MMH: Be aware that this is modifying the PI structure
            }
            else
            {
                return_value = FALSE;       // Failure
            }
        }
    }

    return (return_value);
}
/************************************************************/

/****************************************************************/
/* UDPENNY_CreateDeed                                           */
/****************************************************************/
LE_DATA_DataId UDPENNY_CreateDeed(short square, short language, short board, short system, BOOL front)
{
//#define     DATAFILE        DAT_TEST        // MMH: Datafile for my test code.
#define     DATAFILE        DAT_MAIN        // MMH: Datafile for Monopoly code.

    LE_DATA_DataId  return_value        = LED_EI;
    short           display_square;
    short           display_language;
    short           display_board;
    short           display_system;
    struct UDPENNY_PrintInfo PI;
    ACHAR    WORK_BUFFER[256];
    ACHAR    WORK_BUFFER2[256];

    LE_BLT_KeyGreenLimit = 255;     // MMH: Confirm with Russell this is going to be ok

    LE_FONTS_SetFont("Arial.ttf", "Arial"); // Because Monopoly is using this font.

    display_square = UDPENNY_ReturnTransIndexFromLogicalSquare(square);
    if (display_square == -1) return (return_value);

    // Get and test display_language value
    if (language != -1) display_language = language;
    else display_language = iLangId;         // MMH: Uncomment this for insertion into Monopoly Code
    // MMH: Uncomment this for insertion into Monopoly Code
    if (display_language == NOTW_LANG_ENGLISH_US)
    {
        wsprintf(LE_ERROR_DebugMessageBuffer, "UDPENNY_CreateDeed(): display_language equals NOTW_LANG_ENGLISH_US. I thought we were doing the US fully in graphics?");
        LE_ERROR_Msg (LE_ERROR_DebugMessageBuffer, LE_ERROR_OUTPUT_USER, FALSE);
        return (return_value);
    }
    else if ((display_language < NOTW_LANG_ENGLISH_UK)&&(display_language >= NOTW_LANG_MAX))
    {
        wsprintf(LE_ERROR_DebugMessageBuffer, "UDPENNY_CreateDeed(): Invalid range for display_language (%d)", display_language);
        LE_ERROR_Msg (LE_ERROR_DebugMessageBuffer, LE_ERROR_OUTPUT_USER, FALSE);
        return (return_value);
    }

    display_language --; // This is done to make it equal to the languages in the array. Unfortunately the define for US is 1.

    // Get and test display_board value
    if (board != -1) display_board = board;
    else display_board = DISPLAY_state.city;         // MMH: Uncomment this for insertion into Monopoly Code
    // now check to see if we are dealing with a custom board
    if(display_board == -1)     // DISPLAY_state.city == -1 means a custom board is loaded
    {
        // set to install board
        display_board = iLangId - 2;
    }
    if ((display_board < NOTW_BORD_UK)&&(display_board >= NOTW_BORD_MAX))
    {
        wsprintf(LE_ERROR_DebugMessageBuffer, "UDPENNY_CreateDeed(): Invalid range for display_board (%d)", display_board);
        LE_ERROR_Msg (LE_ERROR_DebugMessageBuffer, LE_ERROR_OUTPUT_USER, FALSE);
        return (return_value);
    }

    // Get and test display_system value
    if (system != -1) display_system = system;
    else display_system = DISPLAY_state.system;         // MMH: Uncomment this for insertion into Monopoly Code
    if ((display_system < NOTW_MONA_UK)&&(display_system >= NOTW_MONA_MAX))
    {
        wsprintf(LE_ERROR_DebugMessageBuffer, "UDPENNY_CreateDeed(): Invalid range for display_system (%d)", display_system);
        LE_ERROR_Msg (LE_ERROR_DebugMessageBuffer, LE_ERROR_OUTPUT_USER, FALSE);
        return (return_value);
    }

    return_value = LE_GRAFIX_ObjectCreate(DEED_WIDTH, DEED_HEIGHT, FALSE);
    if (return_value == LED_EI)
    {
        wsprintf(LE_ERROR_DebugMessageBuffer, "UDPENNY_CreateDeed(): Failed to create object.  LE_GRAFIX_ObjectCreate returned LED_EI.");
        LE_ERROR_Msg (LE_ERROR_DebugMessageBuffer, LE_ERROR_OUTPUT_USER, FALSE);
        return (return_value);
    }

    if (!front) // Back of card. All Backs look the same.
    {
        LE_GRAFIX_ColorArea(return_value, 0, 0, DEED_WIDTH, DEED_HEIGHT, LEG_MCR(255,255,255));
        LE_GRAFIX_ShowTCB  (return_value, 0, 0, LED_IFT(DATAFILE, TAB_back));

        // Print property name
        PI.y               = B1_Y;
        PI.h               = B1_H;
        PI.Colour          = LEG_MCR(  0,  0,  0);
        PI.justification   = 1;
        PI.vertical_leeway = 2;
        PI.bold            = TRUE;
        PI.italic          = FALSE;
        PI.font_size       = 15;        // MMH
        PI.font_size       = 10;
        PI.vert_center     = FALSE;
        while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS_PROP[display_square][display_board+1], TRUE))
        {
            PI.font_size --;
            if (PI.font_size < 2) break;
        }
        UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS_PROP[display_square][display_board+1], FALSE);

        // Print "Mortgaged For"
        PI.y               = B2_Y;
        PI.h               = B2_H;
        PI.Colour          = LEG_MCR(  0,  0,  0);
        PI.justification   = 1;
        PI.vertical_leeway = 2;
        PI.bold            = FALSE;
        PI.italic          = FALSE;
        PI.font_size       = 15;
        PI.vert_center     = FALSE;
        while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS[12] [display_language],TRUE))
        {
            PI.font_size --;
            if (PI.font_size < 2) break;
        }
        UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS[12] [display_language],FALSE);

        // Print Mortgaged value in current Monatary system
        PI.y               = B3_Y;
        PI.h               = B3_H;
        PI.Colour          = LEG_MCR(  0,  0,  0);
        PI.justification   = 1;
        PI.vertical_leeway = 2;
        PI.bold            = FALSE;
        PI.italic          = FALSE;
        PI.font_size       = 15;
        PI.vert_center     = FALSE;
        UDPENNY_ReturnMoneyValueBasedOnMonatarySystem((short)RULE_SquarePredefinedInfo[square].mortgageCost, display_system, WORK_BUFFER, TRUE);
#if !USA_VERSION
        if (display_language == NOTW_LANG_FINNISH - 1) // display_language is decremented above
          Astrcat(WORK_BUFFER, A_T(":STA"));
#endif
        while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, WORK_BUFFER,TRUE))
        {
            PI.font_size --;
            if (PI.font_size < 2) break;
        }
        UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, WORK_BUFFER,FALSE);

        // Print "Card must be turned this side up if property is mortgaged"
        PI.y               = B4_Y;
        PI.h               = B4_H;
        PI.Colour          = LEG_MCR(  0,  0,  0);
        PI.justification   = 1;
        PI.vertical_leeway = 2;
        PI.bold            = FALSE;
        PI.italic          = TRUE;
        PI.font_size       = 15;
        PI.vert_center     = FALSE;
        while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS_MORT[display_language],TRUE))
        {
            PI.font_size --;
            if (PI.font_size < 2) break;
        }
        UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS_MORT[display_language],FALSE);
    }
    else
    {
        if (display_square >= 26) // Utility
        {
            if (display_square == 26) LE_GRAFIX_ShowTCB(return_value, 0, 0, LED_IFT(DATAFILE, TAB_front_u1));
            else                      LE_GRAFIX_ShowTCB(return_value, 0, 0, LED_IFT(DATAFILE, TAB_front_u2));

            // --------------
            // Print Utility name
            PI.y               = FU1_Y;
            PI.h               = FU1_H;
            PI.justification   = 1;
            PI.vertical_leeway = 0;
            PI.bold            = FALSE;
            PI.italic          = FALSE;
            PI.font_size       = FU1_H;
            PI.vert_center     = FALSE;
            PI.Colour = LEG_MCR(  0,  0,  0);
            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS_PROP[display_square][display_board+1],TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS_PROP[display_square][display_board+1],FALSE);

            // determine which currency factor is used depending on the currency.  There are four different currency
            // factors to consider when printing the deeds
            int currency_factor=0;
              switch(DISPLAY_state.system)
              {
                case NOTW_MONA_UK:
                case NOTW_MONA_EURO:
                case NOTW_MONA_AUSTRALIAN:  currency_factor=0; break;

                case NOTW_MONA_SINGAPORE:   currency_factor=1; break;

                case NOTW_MONA_GERMAN:
                case NOTW_MONA_SWEDISH:
                case NOTW_MONA_FINNISH:
                case NOTW_MONA_DANISH:
                case NOTW_MONA_NORWEGIAN:
                case NOTW_MONA_BELGIUM:     currency_factor=2; break;

                case NOTW_MONA_FRENCH:
                case NOTW_MONA_SPANISH:
                case NOTW_MONA_DUTCH:       currency_factor=3; break;
              }

            // --------------
            // Print "If one utility is owned..."
            PI.y               = FU2_Y;
            PI.h               = FU2_H;
            PI.justification   = 1;
            PI.vertical_leeway = 0;
            PI.bold            = FALSE;
            PI.italic          = FALSE;
            PI.font_size       = FU2_H;
            PI.font_size       = 10;        // MMH
            PI.vert_center     = FALSE;
            PI.Colour = LEG_MCR(  0,  0,  0);
            if ((display_square == 27) && (display_language == NOTW_LANG_GERMAN - 1)) { // display_language is decremented above
              ACHAR TRANS_UTIL1g[T_MAX_CURRENCY_FACTORS][256] = {
                    A_T("Wenn man Besitzer des Wasserwerkes ist, so ist die Miete 4mal so hoch, wie Augen auf den zwei Würfeln sind."),
                    A_T("Wenn man Besitzer des Wasserwerkes ist, so ist die Miete 40mal so hoch, wie Augen auf den zwei Würfeln sind."),
                    A_T("Wenn man Besitzer des Wasserwerkes ist, so ist die Miete 80mal so hoch, wie Augen auf den zwei Würfeln sind."),
                    A_T("Wenn man Besitzer des Wasserwerkes ist, so ist die Miete 400mal so hoch, wie Augen auf den zwei Würfeln sind.")};
              while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS_UTIL1g[currency_factor],TRUE))
              {
                  PI.font_size --;
                  if (PI.font_size < 2) break;
              }
              UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS_UTIL1g[currency_factor],FALSE);
            } else if ((display_square == 27) && (display_language == NOTW_LANG_NORWEGIAN - 1)) { // display_language is decremented above
              ACHAR TRANS_UTIL1n[T_MAX_CURRENCY_FACTORS][256] = {
                  A_T("Eieren av Vannverket kan innkassere en leie som er 4 ganger så høy som tallet på terningene som ble brukt for å flytte til dette feltet."),
                  A_T("Eieren av Vannverket kan innkassere en leie som er 40 ganger så høy som tallet på terningene som ble brukt for å flytte til dette feltet."),
                  A_T("Eieren av Vannverket kan innkassere en leie som er 80 ganger så høy som tallet på terningene som ble brukt for å flytte til dette feltet."),
                  A_T("Eieren av Vannverket kan innkassere en leie som er 400 ganger så høy som tallet på terningene som ble brukt for å flytte til dette feltet.")};
              while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS_UTIL1n[currency_factor],TRUE))
              {
                  PI.font_size --;
                  if (PI.font_size < 2) break;
              }
              UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS_UTIL1n[currency_factor],FALSE);
            } else {

              while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS_UTIL1[currency_factor][display_language],TRUE))
              {
                  PI.font_size --;
                  if (PI.font_size < 2) break;
              }
              UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS_UTIL1[currency_factor][display_language],FALSE);
            }

            // --------------
            // Print "If two utilities are owned..."
            PI.y               = FU3_Y;
            PI.h               = FU3_H;
            PI.justification   = 1;
            PI.vertical_leeway = 0;
            PI.bold            = FALSE;
            PI.italic          = FALSE;
            PI.font_size       = FU3_H;
            PI.font_size       = 10;        // MMH
            PI.vert_center     = FALSE;
            PI.Colour = LEG_MCR(  0,  0,  0);
            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS_UTIL2[currency_factor][display_language],TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS_UTIL2[currency_factor][display_language],FALSE);

            // --------------
            // Print Mortgage Value
            PI.y               = FU4_Y;
            PI.h               = FU4_H;
            PI.justification   = 0;
            PI.vertical_leeway = 0;
            PI.bold            = FALSE;
            PI.italic          = FALSE;
            PI.font_size       = FU4_H;
            PI.vert_center     = FALSE;
            PI.Colour = LEG_MCR(  0,  0,  0);
            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS[7][display_language],TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS[7][display_language],FALSE);

            PI.justification   = 2;
            PI.font_size       = FU4_H;
            PI.vert_center     = FALSE;
            UDPENNY_ReturnMoneyValueBasedOnMonatarySystem((short)RULE_SquarePredefinedInfo[square].mortgageCost, display_system, WORK_BUFFER, TRUE);
            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, WORK_BUFFER,TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, WORK_BUFFER,FALSE);

            // --------------
            // Print "Copywrite"
            /*
            PI.y               = FU5_Y;
            PI.h               = FU5_H;
            PI.justification   = 0;
            PI.vertical_leeway = 0;
            PI.bold            = TRUE;
            PI.italic          = FALSE;
            PI.font_size       = FU5_H;
            PI.vert_center     = FALSE;
            PI.Colour = LEG_MCR(  0,  0,  0);
            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS_COPYWRITE[display_language],TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS_COPYWRITE[display_language],FALSE);
            */
        }
        else if (display_square >= 22) // RailRoad
        {
            LE_GRAFIX_ShowTCB(return_value, 0, 0, LED_IFT(DATAFILE, TAB_front_r));

            // --------------
            // Print RailRoad Name
            PI.y               = FR1_Y;
            PI.h               = FR1_H;
            PI.justification   = 1;
            PI.vertical_leeway = 0;
            PI.bold            = FALSE;
            PI.italic          = FALSE;
            PI.font_size       = FR1_H;
            PI.vert_center     = FALSE;
            PI.Colour = LEG_MCR(  0,  0,  0);
            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS_PROP[display_square][display_board+1],TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS_PROP[display_square][display_board+1],FALSE);

            // --------------
            // Print Rent
            PI.y               = FR2_Y;
            PI.h               = FR2_H;
            PI.justification   = 0;
            PI.vertical_leeway = 0;
            PI.bold            = FALSE;
            PI.italic          = FALSE;
            PI.font_size       = FR2_H;
            PI.vert_center     = FALSE;
            PI.Colour = LEG_MCR(  0,  0,  0);
#if !USA_VERSION
          // &)$^@%#
          if (display_language == NOTW_LANG_FINNISH - 1) { // display_language is decremented above
            ACHAR RailRoadRent[44] = A_T("VUOKRA ASEMASTA");
            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, RailRoadRent,TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, RailRoadRent,FALSE);
          } else {
#endif
            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS[1][display_language],TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS[1][display_language],FALSE);
#if !USA_VERSION
          }
#endif
            PI.justification   = 2;
            PI.font_size       = FR2_H;
            PI.vert_center     = FALSE;
            UDPENNY_ReturnMoneyValueBasedOnMonatarySystem((short)RULE_SquarePredefinedInfo[square].rent[1], display_system, WORK_BUFFER, TRUE);
            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, WORK_BUFFER,TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, WORK_BUFFER,FALSE);

            // --------------
            // Print Rent for 2 railroads
            PI.y               = FR3_Y;
            PI.h               = FR3_H;
            PI.justification   = 0;
            PI.vertical_leeway = 0;
            PI.bold            = FALSE;
            PI.italic          = FALSE;
            PI.font_size       = FR3_H;
            PI.vert_center     = FALSE;
            PI.Colour = LEG_MCR(  0,  0,  0);
            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS[13][display_language],TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS[13][display_language],FALSE);

            PI.justification   = 2;
            PI.font_size       = FR3_H;
            PI.vert_center     = FALSE;
            UDPENNY_ReturnMoneyValueBasedOnMonatarySystem((short)RULE_SquarePredefinedInfo[square].rent[2], display_system, WORK_BUFFER, FALSE);
            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, WORK_BUFFER,TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, WORK_BUFFER,FALSE);

            // --------------
            // Print Rent for 3 railroads
            PI.y               = FR4_Y;
            PI.h               = FR4_H;
            PI.justification   = 0;
            PI.vertical_leeway = 0;
            PI.bold            = FALSE;
            PI.italic          = FALSE;
            PI.font_size       = FR4_H;
            PI.vert_center     = FALSE;
            PI.Colour = LEG_MCR(  0,  0,  0);
            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS[14][display_language],TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS[14][display_language],FALSE);

            PI.justification   = 2;
            PI.font_size       = FR4_H;
            PI.vert_center     = FALSE;
            UDPENNY_ReturnMoneyValueBasedOnMonatarySystem((short)RULE_SquarePredefinedInfo[square].rent[3], display_system, WORK_BUFFER, FALSE);
            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, WORK_BUFFER,TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, WORK_BUFFER,FALSE);

            // --------------
            // Print Rent for 4 railroads
            PI.y               = FR5_Y;
            PI.h               = FR5_H;
            PI.justification   = 0;
            PI.vertical_leeway = 0;
            PI.bold            = FALSE;
            PI.italic          = FALSE;
            PI.font_size       = FR5_H;
            PI.vert_center     = FALSE;
            PI.Colour = LEG_MCR(  0,  0,  0);
            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS[15][display_language],TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS[15][display_language],FALSE);

            PI.justification   = 2;
            PI.font_size       = FR5_H;
            PI.vert_center     = FALSE;
            UDPENNY_ReturnMoneyValueBasedOnMonatarySystem((short)RULE_SquarePredefinedInfo[square].rent[4], display_system, WORK_BUFFER, FALSE);
            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, WORK_BUFFER,TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, WORK_BUFFER,FALSE);

            // --------------
            // Print Mortgage Value
            PI.y               = FR6_Y;
            PI.h               = FR6_H;
            PI.justification   = 0;
            PI.vertical_leeway = 0;
            PI.bold            = FALSE;
            PI.italic          = FALSE;
            PI.font_size       = FR6_H;
            PI.vert_center     = FALSE;
            PI.Colour = LEG_MCR(  0,  0,  0);
            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS[7][display_language],TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS[7][display_language],FALSE);

            PI.justification   = 2;
            PI.font_size       = FR6_H;
            PI.vert_center     = FALSE;
            UDPENNY_ReturnMoneyValueBasedOnMonatarySystem((short)RULE_SquarePredefinedInfo[square].mortgageCost, display_system, WORK_BUFFER, TRUE);
            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, WORK_BUFFER,TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, WORK_BUFFER,FALSE);

            // --------------
            // Print "Copywrite"
            PI.y               = FR7_Y;
            PI.h               = FR7_H;
            PI.justification   = 0;
            PI.vertical_leeway = 0;
            PI.bold            = TRUE;
            PI.italic          = FALSE;
            PI.font_size       = FR7_H;
            PI.vert_center     = FALSE;
            PI.Colour = LEG_MCR(  0,  0,  0);
            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS_COPYWRITE[display_language],TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS_COPYWRITE[display_language],FALSE);
        }
        else    // Property
        {
            DWORD ccolor;
            short sset;

            LE_GRAFIX_ShowTCB(return_value, 0, 0, LED_IFT(DATAFILE, TAB_front_p));

            if (square <=  3)   // BALTIC AVE.
            {
                sset = 0;
                PI.Colour = LEG_MCR(  0,  0,  0);
            }
            else if (square <=  9)   // CONNETICUT AVE.
            {
                sset = 1;
                PI.Colour = LEG_MCR(  0,  0,  0);
            }
            else if (square <= 14)   // VIRGINIA AVE.
            {
                sset = 2;
                PI.Colour = LEG_MCR(  0,  0,  0);
            }
            else if (square <= 19)   // NEW YORK AVE.
            {
                sset = 3;
                PI.Colour = LEG_MCR(  0,  0,  0);
            }
            else if (square <= 24)   // ILLINOIS AVE.
            {
                sset = 4;
                PI.Colour = LEG_MCR(  0,  0,  0);
            }
            else if (square <= 29)   // MARVIN GARDENS
            {
                sset = 5;
                PI.Colour = LEG_MCR(  0,  0,  0);
            }
            else if (square <= 34)   // PENNSYLVANIA AVE.
            {
                sset = 6;
                PI.Colour = LEG_MCR(  0,  0,  0);
            }
            else if (square <= 39)   // BOARDWALK
            {
                sset = 7;
                PI.Colour = LEG_MCR(  0,  0,  0);
            }

            // MMH: Adding 1 to display language to get it back equal to the defines.
            // Remember above we reduced it by one to get it equal to the trans index
            // of zero being UK
            ccolor = UDPENNY_ReturnColorForPropertySet(sset, display_language+1);
            LE_GRAFIX_ColorArea(return_value, 19, 16, 162, 36, ccolor);

            // --------------
            // Print Title Deed
            /*
            PI.y               = FP1_Y;
            PI.h               = FP1_H;
            PI.justification   = 1;
            PI.vertical_leeway = 0;
            PI.bold            = FALSE;
            PI.italic          = FALSE;
            PI.font_size       = FP1_H;
            PI.vert_center     = TRUE;
            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS[0][display_language],TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS[0][display_language],FALSE);
            */

            // --------------
            // Print Property Name
            PI.y               = FP2_Y;
            PI.h               = FP2_H;
            PI.justification   = 1;
            PI.vertical_leeway = 0;
            PI.bold            = TRUE;
            PI.italic          = FALSE;
            PI.font_size       = FP2_H;
            PI.font_size       = 10;
            PI.vert_center     = TRUE;
            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS_PROP[display_square][display_board+1],TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS_PROP[display_square][display_board+1],FALSE);

            // --------------
            // Print Rent Value
            PI.y               = FP3_Y;
            PI.h               = FP3_H;
            PI.justification   = 1;
            PI.vertical_leeway = 0;
            PI.bold            = TRUE;
            PI.italic          = FALSE;
            PI.font_size       = FP3_H;
            PI.vert_center     = FALSE;
            PI.Colour = LEG_MCR(  0,  0,  0);

            // Calculate rent in current monatary systema and concat it to "rent" string for print
            UDPENNY_ReturnMoneyValueBasedOnMonatarySystem((short)RULE_SquarePredefinedInfo[square].rent[0], display_system, WORK_BUFFER, TRUE);
            Asprintf(WORK_BUFFER2,A_T("%s %s"), TRANS[1][display_language],WORK_BUFFER);

            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, WORK_BUFFER2,TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, WORK_BUFFER2,FALSE);

            // --------------
            // Print House Rent - Line 1
            PI.y               = FP4_Y;
            PI.h               = FP4_H;
            PI.justification   = 0;
            PI.vertical_leeway = 0;
            PI.bold            = FALSE;
            PI.italic          = FALSE;
            PI.font_size       = FP4_H;
            PI.vert_center     = FALSE;
            PI.Colour = LEG_MCR(  0,  0,  0);
            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS[2][display_language],TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS[2][display_language],FALSE);

            PI.justification   = 2;
            PI.font_size       = FP4_H;
            PI.vert_center     = FALSE;
            UDPENNY_ReturnMoneyValueBasedOnMonatarySystem((short)RULE_SquarePredefinedInfo[square].rent[1], display_system, WORK_BUFFER, TRUE);
            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, WORK_BUFFER,TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, WORK_BUFFER,FALSE);

            // --------------
            // Print House Rent - Line 2
            PI.y               = FP5_Y;
            PI.h               = FP5_H;
            PI.justification   = 0;
            PI.vertical_leeway = 0;
            PI.bold            = FALSE;
            PI.italic          = FALSE;
            PI.font_size       = FP5_H;
            PI.vert_center     = FALSE;
            PI.Colour = LEG_MCR(  0,  0,  0);
            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS[3][display_language],TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS[3][display_language],FALSE);

            PI.justification   = 2;
            PI.font_size       = FP5_H;
            PI.vert_center     = FALSE;
            UDPENNY_ReturnMoneyValueBasedOnMonatarySystem((short)RULE_SquarePredefinedInfo[square].rent[2], display_system, WORK_BUFFER, FALSE);
            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, WORK_BUFFER,TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, WORK_BUFFER,FALSE);

            // --------------
            // Print House Rent - Line 3
            PI.y               = FP6_Y;
            PI.h               = FP6_H;
            PI.justification   = 0;
            PI.vertical_leeway = 0;
            PI.bold            = FALSE;
            PI.italic          = FALSE;
            PI.font_size       = FP6_H;
            PI.vert_center     = FALSE;
            PI.Colour = LEG_MCR(  0,  0,  0);
            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS[4][display_language],TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS[4][display_language],FALSE);

            PI.justification   = 2;
            PI.font_size       = FP6_H;
            PI.vert_center     = FALSE;
            UDPENNY_ReturnMoneyValueBasedOnMonatarySystem((short)RULE_SquarePredefinedInfo[square].rent[3], display_system, WORK_BUFFER, FALSE);
            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, WORK_BUFFER,TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, WORK_BUFFER,FALSE);

            // --------------
            // Print House Rent - Line 4
            PI.y               = FP7_Y;
            PI.h               = FP7_H;
            PI.justification   = 0;
            PI.vertical_leeway = 0;
            PI.bold            = FALSE;
            PI.italic          = FALSE;
            PI.font_size       = FP7_H;
            PI.vert_center     = FALSE;
            PI.Colour = LEG_MCR(  0,  0,  0);
            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS[5][display_language],TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS[5][display_language],FALSE);

            PI.justification   = 2;
            PI.font_size       = FP7_H;
            PI.vert_center     = FALSE;
            // Dante
            if (UICurrentGameState.GameOptions.housesPerHotel == 5) UDPENNY_ReturnMoneyValueBasedOnMonatarySystem((short)RULE_SquarePredefinedInfo[square].rent[4], display_system, WORK_BUFFER, FALSE);
            else                                                    UDPENNY_ReturnMoneyValueBasedOnMonatarySystem((short)RULE_SquarePredefinedInfo[square].rent[5], display_system, WORK_BUFFER, FALSE);
            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, WORK_BUFFER,TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, WORK_BUFFER,FALSE);

            // --------------
            // Print Hotel Rent
            PI.y               = FP8_Y;
            PI.h               = FP8_H;
            PI.justification   = 0;
            PI.vertical_leeway = 0;
            PI.bold            = FALSE;
            PI.italic          = FALSE;
            PI.font_size       = FP8_H;
            PI.vert_center     = FALSE;
            PI.Colour = LEG_MCR(  0,  0,  0);
            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS[6][display_language],TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS[6][display_language],FALSE);

            PI.justification   = 2;
            PI.font_size       = FP8_H;
            PI.vert_center     = FALSE;
            // Dante
            if (UICurrentGameState.GameOptions.housesPerHotel == 5) UDPENNY_ReturnMoneyValueBasedOnMonatarySystem((short)RULE_SquarePredefinedInfo[square].rent[5], display_system, WORK_BUFFER, FALSE);
            else                                                    UDPENNY_ReturnMoneyValueBasedOnMonatarySystem((short)RULE_SquarePredefinedInfo[square].rent[4], display_system, WORK_BUFFER, FALSE);
            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, WORK_BUFFER,TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, WORK_BUFFER,FALSE);

            // --------------
            // Print Mortgage Value
            PI.y               = FP9_Y;
            PI.h               = FP9_H;
            PI.justification   = 0;
            PI.vertical_leeway = 0;
            PI.bold            = FALSE;
            PI.italic          = FALSE;
            PI.font_size       = FP9_H;
            PI.vert_center     = FALSE;
            PI.Colour = LEG_MCR(  0,  0,  0);
            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS[7][display_language],TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS[7][display_language],FALSE);

            PI.justification   = 2;
            PI.font_size       = FP9_H;
            PI.vert_center     = FALSE;
            UDPENNY_ReturnMoneyValueBasedOnMonatarySystem((short)RULE_SquarePredefinedInfo[square].mortgageCost, display_system, WORK_BUFFER, TRUE);
            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, WORK_BUFFER,TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, WORK_BUFFER,FALSE);

            // --------------
            // Print Houses Cost
            PI.y               = FPa_Y;
            PI.h               = FPa_H;
            PI.justification   = 0;
            PI.vertical_leeway = 0;
            PI.bold            = FALSE;
            PI.italic          = FALSE;
            PI.font_size       = FPa_H;
            PI.vert_center     = FALSE;
            PI.Colour = LEG_MCR(  0,  0,  0);
            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS[8][display_language],TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS[8][display_language],FALSE);

            PI.justification   = 2;
            PI.font_size       = FPa_H;
            PI.vert_center     = FALSE;
            UDPENNY_ReturnMoneyValueBasedOnMonatarySystem((short)RULE_SquarePredefinedInfo[square].housePurchaseCost, display_system, WORK_BUFFER, TRUE);
            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, WORK_BUFFER,TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, WORK_BUFFER,FALSE);

            // --------------
            // Print Hotels Cost
            PI.y               = FPb_Y;
            PI.h               = FPb_H;
            PI.justification   = 0;
            PI.vertical_leeway = 0;
            PI.bold            = FALSE;
            PI.italic          = FALSE;
            PI.font_size       = FPb_H;
            PI.vert_center     = FALSE;
            PI.Colour = LEG_MCR(  0,  0,  0);
            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS[9][display_language],TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS[9][display_language],FALSE);

            PI.justification   = 2;
            PI.font_size       = FPb_H;
            PI.vert_center     = FALSE;
            UDPENNY_ReturnMoneyValueBasedOnMonatarySystem((short)RULE_SquarePredefinedInfo[square].housePurchaseCost, display_system, WORK_BUFFER, TRUE);
            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, WORK_BUFFER,TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, WORK_BUFFER,FALSE);

            // --------------
            // Print A_T("with 4 houses")
            PI.y               = FPc_Y;
            PI.h               = FPc_H;
            PI.justification   = 2;
            PI.vertical_leeway = 0;
            PI.bold            = FALSE;
            PI.italic          = FALSE;
            PI.font_size       = FPc_H;
            PI.vert_center     = FALSE;
            PI.Colour = LEG_MCR(  0,  0,  0);
            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS[10][display_language],TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS[10][display_language],FALSE);

            // --------------
            // Print "Simple Monopoly Test"
            PI.y               = FPd_Y;
            PI.h               = FPd_H;
            PI.justification   = 1;
            PI.vertical_leeway = 0;
            PI.bold            = FALSE;
            PI.italic          = TRUE;
            PI.font_size       = FPd_H;
            PI.vert_center     = FALSE;
            PI.Colour = LEG_MCR(  0,  0,  0);
            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS_SIMPLE_MONO[display_language],TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS_SIMPLE_MONO[display_language],FALSE);

            // --------------
            // Print "Copywrite"
            PI.y               = FPe_Y;
            PI.h               = FPe_H;
            PI.justification   = 0;
            PI.vertical_leeway = 0;
            PI.bold            = TRUE;
            PI.italic          = FALSE;
            PI.font_size       = FPe_H;
            PI.vert_center     = FALSE;
            PI.Colour = LEG_MCR(  0,  0,  0);
            while (!UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS_COPYWRITE[display_language],TRUE))
            {
                PI.font_size --;
                if (PI.font_size < 2) break;
            }
            UDPENNY_FitAndPrintJustifiedDeedText(return_value, &PI, TRANS_COPYWRITE[display_language],FALSE);
        }
    }

    return (return_value);
}
/************************************************************/






// function to properly return the card tag in euro versions
int UDPENNY_GetCorrectCardCNK(int lastCardIn)
{

    int seqTag = 0;

    // set up a temp variable to point to the correct city when
    // playing a custom board
    int tempCity = DISPLAY_state.city;
    if(tempCity == -1)
        tempCity = iLangId -2;

#if !USA_VERSION
    switch(lastCardIn)
    {
    // check chance cards
    case 0:     // switch on country
                switch(tempCity)
                {
                case NOTW_BORD_UK:          seqTag = CNK_cyca0102; break;
                case NOTW_BORD_FRENCH:      seqTag = CNK_cyca0103; break;
                case NOTW_BORD_GERMAN:      seqTag = CNK_cyca0104; break;
                case NOTW_BORD_SPANISH:     seqTag = CNK_cyca0105; break;
                case NOTW_BORD_DUTCH:       seqTag = CNK_cyca0106; break;
                case NOTW_BORD_SWEDISH:     seqTag = CNK_cyca0107; break;
                case NOTW_BORD_FINNISH:     seqTag = CNK_cyca0108; break;
                case NOTW_BORD_DANISH:      seqTag = CNK_cyca0109; break;
                case NOTW_BORD_NORWEGIAN:   seqTag = CNK_cyca0110; break;
                case NOTW_BORD_AUSTRALIAN:  seqTag = CNK_cyca0113; break;
                case NOTW_BORD_BELGIUM:     seqTag = CNK_cyca0111; break;
                case NOTW_BORD_SINGAPORE:   seqTag = CNK_cyca0112; break;
                }
                break;

    case 1:     // switch on monatary system
                switch(DISPLAY_state.system)
                {
                case NOTW_MONA_UK:          seqTag = CNK_cyca0202; break;
                case NOTW_MONA_FRENCH:      seqTag = CNK_cyca0203; break;
                case NOTW_MONA_GERMAN:      seqTag = CNK_cyca0204; break;
                case NOTW_MONA_SPANISH:     seqTag = CNK_cyca0205; break;
                case NOTW_MONA_DUTCH:       seqTag = CNK_cyca0206; break;
                case NOTW_MONA_SWEDISH:     seqTag = CNK_cyca0207; break;
                case NOTW_MONA_FINNISH:     seqTag = CNK_cyca0208; break;
                case NOTW_MONA_DANISH:      seqTag = CNK_cyca0209; break;
                case NOTW_MONA_NORWEGIAN:   seqTag = CNK_cyca0210; break;
                case NOTW_MONA_AUSTRALIAN:  seqTag = CNK_cyca0213; break;
                case NOTW_MONA_BELGIUM:     seqTag = CNK_cyca0211; break;
                case NOTW_MONA_SINGAPORE:   seqTag = CNK_cyca0212; break;
                case NOTW_MONA_EURO:        seqTag = CNK_cyca0214; break;
                }
                break;

    case 2:     // switch on country
                switch(DISPLAY_state.system)
                {
                case NOTW_MONA_UK:
                case NOTW_MONA_EURO:
                case NOTW_MONA_AUSTRALIAN:  seqTag = CNK_zzch0301; break;

                case NOTW_MONA_SINGAPORE:   seqTag = CNK_zzch0302; break;

                case NOTW_MONA_GERMAN:
                case NOTW_MONA_SWEDISH:
                case NOTW_MONA_FINNISH:
                case NOTW_MONA_DANISH:
                case NOTW_MONA_NORWEGIAN:
                case NOTW_MONA_BELGIUM:     seqTag = CNK_zzch0303; break;

                case NOTW_MONA_FRENCH:
                case NOTW_MONA_SPANISH:
                case NOTW_MONA_DUTCH:       seqTag = CNK_zzch0304; break;
                }
                break;

    case 3:     // switch on monatary system
                switch(DISPLAY_state.system)
                {
                case NOTW_MONA_UK:          seqTag = CNK_cyca0402; break;
                case NOTW_MONA_FRENCH:      seqTag = CNK_cyca0403; break;
                case NOTW_MONA_GERMAN:      seqTag = CNK_cyca0404; break;
                case NOTW_MONA_SPANISH:     seqTag = CNK_cyca0405; break;
                case NOTW_MONA_DUTCH:       seqTag = CNK_cyca0406; break;
                case NOTW_MONA_SWEDISH:     seqTag = CNK_cyca0407; break;
                case NOTW_MONA_FINNISH:     seqTag = CNK_cyca0408; break;
                case NOTW_MONA_DANISH:      seqTag = CNK_cyca0409; break;
                case NOTW_MONA_NORWEGIAN:   seqTag = CNK_cyca0410; break;
                case NOTW_MONA_AUSTRALIAN:  seqTag = CNK_cyca0413; break;
                case NOTW_MONA_BELGIUM:     seqTag = CNK_cyca0411; break;
                case NOTW_MONA_SINGAPORE:   seqTag = CNK_cyca0412; break;
                case NOTW_MONA_EURO:        seqTag = CNK_cyca0414; break;
                }
                break;


    case 4:     // switch on monatary system
                switch(DISPLAY_state.system)
                {
                case NOTW_MONA_UK:          seqTag = CNK_cyca0502; break;
                case NOTW_MONA_FRENCH:      seqTag = CNK_cyca0503; break;
                case NOTW_MONA_GERMAN:      seqTag = CNK_cyca0504; break;
                case NOTW_MONA_SPANISH:     seqTag = CNK_cyca0505; break;
                case NOTW_MONA_DUTCH:       seqTag = CNK_cyca0506; break;
                case NOTW_MONA_SWEDISH:     seqTag = CNK_cyca0507; break;
                case NOTW_MONA_FINNISH:     seqTag = CNK_cyca0508; break;
                case NOTW_MONA_DANISH:      seqTag = CNK_cyca0509; break;
                case NOTW_MONA_NORWEGIAN:   seqTag = CNK_cyca0510; break;
                case NOTW_MONA_AUSTRALIAN:  seqTag = CNK_cyca0513; break;
                case NOTW_MONA_BELGIUM:     seqTag = CNK_cyca0511; break;
                case NOTW_MONA_SINGAPORE:   seqTag = CNK_cyca0512; break;
                case NOTW_MONA_EURO:        seqTag = CNK_cyca0514; break;
                }
                break;


    case 5:     // switch on monatary system
                switch(DISPLAY_state.system)
                {
                case NOTW_MONA_UK:          seqTag = CNK_cyca0602; break;
                case NOTW_MONA_FRENCH:      seqTag = CNK_cyca0603; break;
                case NOTW_MONA_GERMAN:      seqTag = CNK_cyca0604; break;
                case NOTW_MONA_SPANISH:     seqTag = CNK_cyca0605; break;
                case NOTW_MONA_DUTCH:       seqTag = CNK_cyca0606; break;
                case NOTW_MONA_SWEDISH:     seqTag = CNK_cyca0607; break;
                case NOTW_MONA_FINNISH:     seqTag = CNK_cyca0608; break;
                case NOTW_MONA_DANISH:      seqTag = CNK_cyca0609; break;
                case NOTW_MONA_NORWEGIAN:   seqTag = CNK_cyca0610; break;
                case NOTW_MONA_AUSTRALIAN:  seqTag = CNK_cyca0613; break;
                case NOTW_MONA_BELGIUM:     seqTag = CNK_cyca0611; break;
                case NOTW_MONA_SINGAPORE:   seqTag = CNK_cyca0612; break;
                case NOTW_MONA_EURO:        seqTag = CNK_cyca0614; break;
                }
                break;



    case 6:     // switch on country
                switch(tempCity)
                {
                case NOTW_BORD_UK:          seqTag = CNK_cyca0702; break;
                case NOTW_BORD_FRENCH:      seqTag = CNK_cyca0703; break;
                case NOTW_BORD_GERMAN:      seqTag = CNK_cyca0704; break;
                case NOTW_BORD_SPANISH:     seqTag = CNK_cyca0705; break;
                case NOTW_BORD_DUTCH:       seqTag = CNK_cyca0706; break;
                case NOTW_BORD_SWEDISH:     seqTag = CNK_cyca0707; break;
                case NOTW_BORD_FINNISH:     seqTag = CNK_cyca0708; break;
                case NOTW_BORD_DANISH:      seqTag = CNK_cyca0709; break;
                case NOTW_BORD_NORWEGIAN:   seqTag = CNK_cyca0710; break;
                case NOTW_BORD_AUSTRALIAN:  seqTag = CNK_cyca0713; break;
                case NOTW_BORD_BELGIUM:     seqTag = CNK_cyca0711; break;
                case NOTW_BORD_SINGAPORE:   seqTag = CNK_cyca0712; break;
                default:    break;
                }
                break;

    case 7:     // switch on monatary system
                switch(DISPLAY_state.system)
                {
                case NOTW_MONA_UK:          seqTag = CNK_cyca0802; break;
                case NOTW_MONA_FRENCH:      seqTag = CNK_cyca0803; break;
                case NOTW_MONA_GERMAN:      seqTag = CNK_cyca0804; break;
                case NOTW_MONA_SPANISH:     seqTag = CNK_cyca0805; break;
                case NOTW_MONA_DUTCH:       seqTag = CNK_cyca0806; break;
                case NOTW_MONA_SWEDISH:     seqTag = CNK_cyca0807; break;
                case NOTW_MONA_FINNISH:     seqTag = CNK_cyca0808; break;
                case NOTW_MONA_DANISH:      seqTag = CNK_cyca0809; break;
                case NOTW_MONA_NORWEGIAN:   seqTag = CNK_cyca0810; break;
                case NOTW_MONA_AUSTRALIAN:  seqTag = CNK_cyca0813; break;
                case NOTW_MONA_BELGIUM:     seqTag = CNK_cyca0811; break;
                case NOTW_MONA_SINGAPORE:   seqTag = CNK_cyca0812; break;
                case NOTW_MONA_EURO:        seqTag = CNK_cyca0814; break;
                }
                break;

    case 8:     // switch on country
                switch(tempCity)
                {
                case NOTW_BORD_UK:          //seqTag = CNK_cyca0902; break;
                case NOTW_BORD_FRENCH:      //seqTag = CNK_cyca0903; break;
                case NOTW_BORD_GERMAN:      //seqTag = CNK_cyca0904; break;
                case NOTW_BORD_SPANISH:     //seqTag = CNK_cyca0905; break;
                case NOTW_BORD_DUTCH:       //seqTag = CNK_cyca0906; break;
                case NOTW_BORD_SWEDISH:     //seqTag = CNK_cyca0907; break;
                case NOTW_BORD_FINNISH:     //seqTag = CNK_cyca0908; break;
                case NOTW_BORD_DANISH:      //seqTag = CNK_cyca0909; break;
                case NOTW_BORD_NORWEGIAN:   //seqTag = CNK_cyca0910; break;
                case NOTW_BORD_AUSTRALIAN:  //seqTag = CNK_cyca0911; break;
                case NOTW_BORD_BELGIUM:     //seqTag = CNK_cyca0912; break;
                case NOTW_BORD_SINGAPORE:   //seqTag = CNK_cyca0913; break;
                default:                    seqTag = CNK_cyca0902; break;
                }
                break;

    case 9:     // switch on country
                switch(tempCity)
                {
                case NOTW_BORD_UK:          //seqTag = CNK_cyca1002; break;
                case NOTW_BORD_FRENCH:      //seqTag = CNK_cyca1003; break;
                case NOTW_BORD_GERMAN:      //seqTag = CNK_cyca1004; break;
                case NOTW_BORD_SPANISH:     //seqTag = CNK_cyca1005; break;
                case NOTW_BORD_DUTCH:       //seqTag = CNK_cyca1006; break;
                case NOTW_BORD_SWEDISH:     //seqTag = CNK_cyca1007; break;
                case NOTW_BORD_FINNISH:     //seqTag = CNK_cyca1008; break;
                case NOTW_BORD_DANISH:      //seqTag = CNK_cyca1009; break;
                case NOTW_BORD_NORWEGIAN:   //seqTag = CNK_cyca1010; break;
                case NOTW_BORD_AUSTRALIAN:  //seqTag = CNK_cyca1011; break;
                case NOTW_BORD_BELGIUM:     //seqTag = CNK_cyca1012; break;
                case NOTW_BORD_SINGAPORE:   //seqTag = CNK_cyca1013; break;
                default:                    seqTag = CNK_cyca1002; break;
                }
                break;

    case 10:    // switch on monatary system
                switch(DISPLAY_state.system)
                {
                case NOTW_MONA_UK:          seqTag = CNK_cyca1102; break;
                case NOTW_MONA_FRENCH:      seqTag = CNK_cyca1103; break;
                case NOTW_MONA_GERMAN:      seqTag = CNK_cyca1104; break;
                case NOTW_MONA_SPANISH:     seqTag = CNK_cyca1105; break;
                case NOTW_MONA_DUTCH:       seqTag = CNK_cyca1106; break;
                case NOTW_MONA_SWEDISH:     seqTag = CNK_cyca1107; break;
                case NOTW_MONA_FINNISH:     seqTag = CNK_cyca1108; break;
                case NOTW_MONA_DANISH:      seqTag = CNK_cyca1109; break;
                case NOTW_MONA_NORWEGIAN:   seqTag = CNK_cyca1110; break;
                case NOTW_MONA_AUSTRALIAN:  seqTag = CNK_cyca1113; break;
                case NOTW_MONA_BELGIUM:     seqTag = CNK_cyca1111; break;
                case NOTW_MONA_SINGAPORE:   seqTag = CNK_cyca1112; break;
                case NOTW_MONA_EURO:        seqTag = CNK_cyca1114; break;
                }
                break;



    case 11:    // switch on country
                switch(tempCity)
                {
                case NOTW_BORD_UK:          seqTag = CNK_cyca1202; break;
                case NOTW_BORD_FRENCH:      seqTag = CNK_cyca1203; break;
                case NOTW_BORD_GERMAN:      seqTag = CNK_cyca1204; break;
                case NOTW_BORD_SPANISH:     seqTag = CNK_cyca1205; break;
                case NOTW_BORD_DUTCH:       seqTag = CNK_cyca1206; break;
                case NOTW_BORD_SWEDISH:     seqTag = CNK_cyca1207; break;
                case NOTW_BORD_FINNISH:     seqTag = CNK_cyca1208; break;
                case NOTW_BORD_DANISH:      seqTag = CNK_cyca1209; break;
                case NOTW_BORD_NORWEGIAN:   seqTag = CNK_cyca1210; break;
                case NOTW_BORD_AUSTRALIAN:  seqTag = CNK_cyca1213; break;
                case NOTW_BORD_BELGIUM:     seqTag = CNK_cyca1211; break;
                case NOTW_BORD_SINGAPORE:   seqTag = CNK_cyca1212; break;
                default:    break;
                }
                break;

    case 12:    // switch on monatary system
                switch(DISPLAY_state.system)
                {
                case NOTW_MONA_UK:          seqTag = CNK_cyca1302; break;
                case NOTW_MONA_FRENCH:      seqTag = CNK_cyca1303; break;
                case NOTW_MONA_GERMAN:      seqTag = CNK_cyca1304; break;
                case NOTW_MONA_SPANISH:     seqTag = CNK_cyca1305; break;
                case NOTW_MONA_DUTCH:       seqTag = CNK_cyca1306; break;
                case NOTW_MONA_SWEDISH:     seqTag = CNK_cyca1307; break;
                case NOTW_MONA_FINNISH:     seqTag = CNK_cyca1308; break;
                case NOTW_MONA_DANISH:      seqTag = CNK_cyca1309; break;
                case NOTW_MONA_NORWEGIAN:   seqTag = CNK_cyca1310; break;
                case NOTW_MONA_AUSTRALIAN:  seqTag = CNK_cyca1313; break;
                case NOTW_MONA_BELGIUM:     seqTag = CNK_cyca1311; break;
                case NOTW_MONA_SINGAPORE:   seqTag = CNK_cyca1312; break;
                case NOTW_MONA_EURO:        seqTag = CNK_cyca1314; break;
                }
                break;


    case 13:    // switch on country
                switch(tempCity)
                {
                case NOTW_BORD_UK:          seqTag = CNK_cyca1402; break;
                case NOTW_BORD_FRENCH:      seqTag = CNK_cyca1403; break;
                case NOTW_BORD_GERMAN:      seqTag = CNK_cyca1404; break;
                case NOTW_BORD_SPANISH:     seqTag = CNK_cyca1405; break;
                case NOTW_BORD_DUTCH:       seqTag = CNK_cyca1406; break;
                case NOTW_BORD_SWEDISH:     seqTag = CNK_cyca1407; break;
                case NOTW_BORD_FINNISH:     seqTag = CNK_cyca1408; break;
                case NOTW_BORD_DANISH:      seqTag = CNK_cyca1409; break;
                case NOTW_BORD_NORWEGIAN:   seqTag = CNK_cyca1410; break;
                case NOTW_BORD_AUSTRALIAN:  seqTag = CNK_cyca1413; break;
                case NOTW_BORD_BELGIUM:     seqTag = CNK_cyca1411; break;
                case NOTW_BORD_SINGAPORE:   seqTag = CNK_cyca1412; break;
                default:    break;
                }
                break;

    case 14:    // switch on monatary system
                switch(DISPLAY_state.system)
                {
                case NOTW_MONA_UK:          seqTag = CNK_cyca1502; break;
                case NOTW_MONA_FRENCH:      seqTag = CNK_cyca1503; break;
                case NOTW_MONA_GERMAN:      seqTag = CNK_cyca1504; break;
                case NOTW_MONA_SPANISH:     seqTag = CNK_cyca1505; break;
                case NOTW_MONA_DUTCH:       seqTag = CNK_cyca1506; break;
                case NOTW_MONA_SWEDISH:     seqTag = CNK_cyca1507; break;
                case NOTW_MONA_FINNISH:     seqTag = CNK_cyca1508; break;
                case NOTW_MONA_DANISH:      seqTag = CNK_cyca1509; break;
                case NOTW_MONA_NORWEGIAN:   seqTag = CNK_cyca1510; break;
                case NOTW_MONA_AUSTRALIAN:  seqTag = CNK_cyca1513; break;
                case NOTW_MONA_BELGIUM:     seqTag = CNK_cyca1511; break;
                case NOTW_MONA_SINGAPORE:   seqTag = CNK_cyca1512; break;
                case NOTW_MONA_EURO:        seqTag = CNK_cyca1514; break;
                }
                break;

    case 15:    // switch on country
                switch(tempCity)
                {
                case NOTW_BORD_UK:          seqTag = CNK_cyca1602; break;
                case NOTW_BORD_FRENCH:      seqTag = CNK_cyca1603; break;
                case NOTW_BORD_GERMAN:      seqTag = CNK_cyca1604; break;
                case NOTW_BORD_SPANISH:     seqTag = CNK_cyca1605; break;
                case NOTW_BORD_DUTCH:       seqTag = CNK_cyca1606; break;
                case NOTW_BORD_SWEDISH:     seqTag = CNK_cyca1607; break;
                case NOTW_BORD_FINNISH:     seqTag = CNK_cyca1608; break;
                case NOTW_BORD_DANISH:      seqTag = CNK_cyca1609; break;
                case NOTW_BORD_NORWEGIAN:   seqTag = CNK_cyca1610; break;
                case NOTW_BORD_AUSTRALIAN:  seqTag = CNK_cyca1613; break;
                case NOTW_BORD_BELGIUM:     seqTag = CNK_cyca1611; break;
                case NOTW_BORD_SINGAPORE:   seqTag = CNK_cyca1612; break;
                default:    break;
                }
                break;

    // check community chest cards
    case 16:    // switch on monatary system
                switch(DISPLAY_state.system)
                {
                case NOTW_MONA_UK:          seqTag = CNK_cyya0102; break;
                case NOTW_MONA_FRENCH:      seqTag = CNK_cyya0103; break;
                case NOTW_MONA_GERMAN:      seqTag = CNK_cyya0104; break;
                case NOTW_MONA_SPANISH:     seqTag = CNK_cyya0105; break;
                case NOTW_MONA_DUTCH:       seqTag = CNK_cyya0106; break;
                case NOTW_MONA_SWEDISH:     seqTag = CNK_cyya0107; break;
                case NOTW_MONA_FINNISH:     seqTag = CNK_cyya0108; break;
                case NOTW_MONA_DANISH:      seqTag = CNK_cyya0109; break;
                case NOTW_MONA_NORWEGIAN:   seqTag = CNK_cyya0110; break;
                case NOTW_MONA_AUSTRALIAN:  seqTag = CNK_cyya0113; break;
                case NOTW_MONA_BELGIUM:     seqTag = CNK_cyya0111; break;
                case NOTW_MONA_SINGAPORE:   seqTag = CNK_cyya0112; break;
                case NOTW_MONA_EURO:        seqTag = CNK_cyya0114; break;
                }
                break;

    case 17:    // switch on monatary system
                switch(DISPLAY_state.system)
                {
                case NOTW_MONA_UK:          seqTag = CNK_cyya0202; break;
                case NOTW_MONA_FRENCH:      seqTag = CNK_cyya0203; break;
                case NOTW_MONA_GERMAN:      seqTag = CNK_cyya0204; break;
                case NOTW_MONA_SPANISH:     seqTag = CNK_cyya0205; break;
                case NOTW_MONA_DUTCH:       seqTag = CNK_cyya0206; break;
                case NOTW_MONA_SWEDISH:     seqTag = CNK_cyya0207; break;
                case NOTW_MONA_FINNISH:     seqTag = CNK_cyya0208; break;
                case NOTW_MONA_DANISH:      seqTag = CNK_cyya0209; break;
                case NOTW_MONA_NORWEGIAN:   seqTag = CNK_cyya0210; break;
                case NOTW_MONA_AUSTRALIAN:  seqTag = CNK_cyya0213; break;
                case NOTW_MONA_BELGIUM:     seqTag = CNK_cyya0211; break;
                case NOTW_MONA_SINGAPORE:   seqTag = CNK_cyya0212; break;
                case NOTW_MONA_EURO:        seqTag = CNK_cyya0214; break;
                }
                break;



    case 18:    // switch on monatary system
                switch(DISPLAY_state.system)
                {
                case NOTW_MONA_UK:          seqTag = CNK_cyya0302; break;
                case NOTW_MONA_FRENCH:      seqTag = CNK_cyya0303; break;
                case NOTW_MONA_GERMAN:      seqTag = CNK_cyya0304; break;
                case NOTW_MONA_SPANISH:     seqTag = CNK_cyya0305; break;
                case NOTW_MONA_DUTCH:       seqTag = CNK_cyya0306; break;
                case NOTW_MONA_SWEDISH:     seqTag = CNK_cyya0307; break;
                case NOTW_MONA_FINNISH:     seqTag = CNK_cyya0308; break;
                case NOTW_MONA_DANISH:      seqTag = CNK_cyya0309; break;
                case NOTW_MONA_NORWEGIAN:   seqTag = CNK_cyya0310; break;
                case NOTW_MONA_AUSTRALIAN:  seqTag = CNK_cyya0313; break;
                case NOTW_MONA_BELGIUM:     seqTag = CNK_cyya0311; break;
                case NOTW_MONA_SINGAPORE:   seqTag = CNK_cyya0312; break;
                case NOTW_MONA_EURO:        seqTag = CNK_cyya0314; break;
                }
                break;



    case 19:    // switch on monatary system
                switch(DISPLAY_state.system)
                {
                case NOTW_MONA_UK:          seqTag = CNK_cyya0402; break;
                case NOTW_MONA_FRENCH:      seqTag = CNK_cyya0403; break;
                case NOTW_MONA_GERMAN:      seqTag = CNK_cyya0404; break;
                case NOTW_MONA_SPANISH:     seqTag = CNK_cyya0405; break;
                case NOTW_MONA_DUTCH:       seqTag = CNK_cyya0406; break;
                case NOTW_MONA_SWEDISH:     seqTag = CNK_cyya0407; break;
                case NOTW_MONA_FINNISH:     seqTag = CNK_cyya0408; break;
                case NOTW_MONA_DANISH:      seqTag = CNK_cyya0409; break;
                case NOTW_MONA_NORWEGIAN:   seqTag = CNK_cyya0410; break;
                case NOTW_MONA_AUSTRALIAN:  seqTag = CNK_cyya0413; break;
                case NOTW_MONA_BELGIUM:     seqTag = CNK_cyya0411; break;
                case NOTW_MONA_SINGAPORE:   seqTag = CNK_cyya0412; break;
                case NOTW_MONA_EURO:        seqTag = CNK_cyya0414; break;
                }
                break;


    case 20:    // switch on country
                switch(tempCity)
                {
                case NOTW_BORD_UK:          seqTag = CNK_cyya0502; break;
                case NOTW_BORD_FRENCH:      seqTag = CNK_cyya0503; break;
                case NOTW_BORD_GERMAN:      seqTag = CNK_cyya0504; break;
                case NOTW_BORD_SPANISH:     seqTag = CNK_cyya0505; break;
                case NOTW_BORD_DUTCH:       seqTag = CNK_cyya0506; break;
                case NOTW_BORD_SWEDISH:     seqTag = CNK_cyya0507; break;
                case NOTW_BORD_FINNISH:     seqTag = CNK_cyya0508; break;
                case NOTW_BORD_DANISH:      seqTag = CNK_cyya0509; break;
                case NOTW_BORD_NORWEGIAN:   seqTag = CNK_cyya0510; break;
                case NOTW_BORD_AUSTRALIAN:  seqTag = CNK_cyya0513; break;
                case NOTW_BORD_BELGIUM:     seqTag = CNK_cyya0511; break;
                case NOTW_BORD_SINGAPORE:   seqTag = CNK_cyya0512; break;
                default:    break;
                }
                break;

    case 21:    // switch on monatary system
                switch(DISPLAY_state.system)
                {
                case NOTW_MONA_UK:          seqTag = CNK_cyya0602; break;
                case NOTW_MONA_FRENCH:      seqTag = CNK_cyya0603; break;
                case NOTW_MONA_GERMAN:      seqTag = CNK_cyya0604; break;
                case NOTW_MONA_SPANISH:     seqTag = CNK_cyya0605; break;
                case NOTW_MONA_DUTCH:       seqTag = CNK_cyya0606; break;
                case NOTW_MONA_SWEDISH:     seqTag = CNK_cyya0607; break;
                case NOTW_MONA_FINNISH:     seqTag = CNK_cyya0608; break;
                case NOTW_MONA_DANISH:      seqTag = CNK_cyya0609; break;
                case NOTW_MONA_NORWEGIAN:   seqTag = CNK_cyya0610; break;
                case NOTW_MONA_AUSTRALIAN:  seqTag = CNK_cyya0613; break;
                case NOTW_MONA_BELGIUM:     seqTag = CNK_cyya0611; break;
                case NOTW_MONA_SINGAPORE:   seqTag = CNK_cyya0612; break;
                case NOTW_MONA_EURO:        seqTag = CNK_cyya0614; break;
                }
                break;


    case 22:    // switch on monatary system
                switch(DISPLAY_state.system)
                {
                case NOTW_MONA_UK:          seqTag = CNK_cyya0702; break;
                case NOTW_MONA_FRENCH:      seqTag = CNK_cyya0703; break;
                case NOTW_MONA_GERMAN:      seqTag = CNK_cyya0704; break;
                case NOTW_MONA_SPANISH:     seqTag = CNK_cyya0705; break;
                case NOTW_MONA_DUTCH:       seqTag = CNK_cyya0706; break;
                case NOTW_MONA_SWEDISH:     seqTag = CNK_cyya0707; break;
                case NOTW_MONA_FINNISH:     seqTag = CNK_cyya0708; break;
                case NOTW_MONA_DANISH:      seqTag = CNK_cyya0709; break;
                case NOTW_MONA_NORWEGIAN:   seqTag = CNK_cyya0710; break;
                case NOTW_MONA_AUSTRALIAN:  seqTag = CNK_cyya0713; break;
                case NOTW_MONA_BELGIUM:     seqTag = CNK_cyya0711; break;
                case NOTW_MONA_SINGAPORE:   seqTag = CNK_cyya0712; break;
                case NOTW_MONA_EURO:        seqTag = CNK_cyya0714; break;
                }
                break;



    case 23:    // switch on monatary system
                switch(DISPLAY_state.system)
                {
                case NOTW_MONA_UK:          seqTag = CNK_cyya0802; break;
                case NOTW_MONA_FRENCH:      seqTag = CNK_cyya0803; break;
                case NOTW_MONA_GERMAN:      seqTag = CNK_cyya0804; break;
                case NOTW_MONA_SPANISH:     seqTag = CNK_cyya0805; break;
                case NOTW_MONA_DUTCH:       seqTag = CNK_cyya0806; break;
                case NOTW_MONA_SWEDISH:     seqTag = CNK_cyya0807; break;
                case NOTW_MONA_FINNISH:     seqTag = CNK_cyya0808; break;
                case NOTW_MONA_DANISH:      seqTag = CNK_cyya0809; break;
                case NOTW_MONA_NORWEGIAN:   seqTag = CNK_cyya0810; break;
                case NOTW_MONA_AUSTRALIAN:  seqTag = CNK_cyya0813; break;
                case NOTW_MONA_BELGIUM:     seqTag = CNK_cyya0811; break;
                case NOTW_MONA_SINGAPORE:   seqTag = CNK_cyya0812; break;
                case NOTW_MONA_EURO:        seqTag = CNK_cyya0814; break;
                }
                break;


    case 24:    // switch on monatary system
                switch(DISPLAY_state.system)
                {
                case NOTW_MONA_UK:          seqTag = CNK_cyya0902; break;
                case NOTW_MONA_FRENCH:      seqTag = CNK_cyya0903; break;
                case NOTW_MONA_GERMAN:      seqTag = CNK_cyya0904; break;
                case NOTW_MONA_SPANISH:     seqTag = CNK_cyya0905; break;
                case NOTW_MONA_DUTCH:       seqTag = CNK_cyya0906; break;
                case NOTW_MONA_SWEDISH:     seqTag = CNK_cyya0907; break;
                case NOTW_MONA_FINNISH:     seqTag = CNK_cyya0908; break;
                case NOTW_MONA_DANISH:      seqTag = CNK_cyya0909; break;
                case NOTW_MONA_NORWEGIAN:   seqTag = CNK_cyya0910; break;
                case NOTW_MONA_AUSTRALIAN:  seqTag = CNK_cyya0913; break;
                case NOTW_MONA_BELGIUM:     seqTag = CNK_cyya0911; break;
                case NOTW_MONA_SINGAPORE:   seqTag = CNK_cyya0912; break;
                case NOTW_MONA_EURO:        seqTag = CNK_cyya0914; break;
                }
                break;



    case 25:    // switch on monatary system
                switch(DISPLAY_state.system)
                {
                case NOTW_MONA_UK:          seqTag = CNK_cyya1002; break;
                case NOTW_MONA_FRENCH:      seqTag = CNK_cyya1003; break;
                case NOTW_MONA_GERMAN:      seqTag = CNK_cyya1004; break;
                case NOTW_MONA_SPANISH:     seqTag = CNK_cyya1005; break;
                case NOTW_MONA_DUTCH:       seqTag = CNK_cyya1006; break;
                case NOTW_MONA_SWEDISH:     seqTag = CNK_cyya1007; break;
                case NOTW_MONA_FINNISH:     seqTag = CNK_cyya1008; break;
                case NOTW_MONA_DANISH:      seqTag = CNK_cyya1009; break;
                case NOTW_MONA_NORWEGIAN:   seqTag = CNK_cyya1010; break;
                case NOTW_MONA_AUSTRALIAN:  seqTag = CNK_cyya1013; break;
                case NOTW_MONA_BELGIUM:     seqTag = CNK_cyya1011; break;
                case NOTW_MONA_SINGAPORE:   seqTag = CNK_cyya1012; break;
                case NOTW_MONA_EURO:        seqTag = CNK_cyya1014; break;
                }
                break;



    case 26:    // switch on monatary system
                switch(DISPLAY_state.system)
                {
                case NOTW_MONA_UK:          seqTag = CNK_cyya1102; break;
                case NOTW_MONA_FRENCH:      seqTag = CNK_cyya1103; break;
                case NOTW_MONA_GERMAN:      seqTag = CNK_cyya1104; break;
                case NOTW_MONA_SPANISH:     seqTag = CNK_cyya1105; break;
                case NOTW_MONA_DUTCH:       seqTag = CNK_cyya1106; break;
                case NOTW_MONA_SWEDISH:     seqTag = CNK_cyya1107; break;
                case NOTW_MONA_FINNISH:     seqTag = CNK_cyya1108; break;
                case NOTW_MONA_DANISH:      seqTag = CNK_cyya1109; break;
                case NOTW_MONA_NORWEGIAN:   seqTag = CNK_cyya1110; break;
                case NOTW_MONA_AUSTRALIAN:  seqTag = CNK_cyya1113; break;
                case NOTW_MONA_BELGIUM:     seqTag = CNK_cyya1111; break;
                case NOTW_MONA_SINGAPORE:   seqTag = CNK_cyya1112; break;
                case NOTW_MONA_EURO:        seqTag = CNK_cyya1114; break;
                }
                break;



    case 27:    // switch on monatary system
                switch(DISPLAY_state.system)
                {
                case NOTW_MONA_UK:          seqTag = CNK_cyya1202; break;
                case NOTW_MONA_FRENCH:      seqTag = CNK_cyya1203; break;
                case NOTW_MONA_GERMAN:      seqTag = CNK_cyya1204; break;
                case NOTW_MONA_SPANISH:     seqTag = CNK_cyya1205; break;
                case NOTW_MONA_DUTCH:       seqTag = CNK_cyya1206; break;
                case NOTW_MONA_SWEDISH:     seqTag = CNK_cyya1207; break;
                case NOTW_MONA_FINNISH:     seqTag = CNK_cyya1208; break;
                case NOTW_MONA_DANISH:      seqTag = CNK_cyya1209; break;
                case NOTW_MONA_NORWEGIAN:   seqTag = CNK_cyya1210; break;
                case NOTW_MONA_AUSTRALIAN:  seqTag = CNK_cyya1213; break;
                case NOTW_MONA_BELGIUM:     seqTag = CNK_cyya1211; break;
                case NOTW_MONA_SINGAPORE:   seqTag = CNK_cyya1212; break;
                case NOTW_MONA_EURO:        seqTag = CNK_cyya1214; break;
                }
                break;



    case 28:    // switch on monatary system
                switch(DISPLAY_state.system)
                {
                case NOTW_MONA_UK:          seqTag = CNK_cyya1302; break;
                case NOTW_MONA_FRENCH:      seqTag = CNK_cyya1303; break;
                case NOTW_MONA_GERMAN:      seqTag = CNK_cyya1304; break;
                case NOTW_MONA_SPANISH:     seqTag = CNK_cyya1305; break;
                case NOTW_MONA_DUTCH:       seqTag = CNK_cyya1306; break;
                case NOTW_MONA_SWEDISH:     seqTag = CNK_cyya1307; break;
                case NOTW_MONA_FINNISH:     seqTag = CNK_cyya1308; break;
                case NOTW_MONA_DANISH:      seqTag = CNK_cyya1309; break;
                case NOTW_MONA_NORWEGIAN:   seqTag = CNK_cyya1310; break;
                case NOTW_MONA_AUSTRALIAN:  seqTag = CNK_cyya1313; break;
                case NOTW_MONA_BELGIUM:     seqTag = CNK_cyya1311; break;
                case NOTW_MONA_SINGAPORE:   seqTag = CNK_cyya1312; break;
                case NOTW_MONA_EURO:        seqTag = CNK_cyya1314; break;
                }
                break;



    case 29:    // switch on monatary system
                switch(DISPLAY_state.system)
                {
                case NOTW_MONA_UK:          seqTag = CNK_cyya1402; break;
                case NOTW_MONA_FRENCH:      seqTag = CNK_cyya1403; break;
                case NOTW_MONA_GERMAN:      seqTag = CNK_cyya1404; break;
                case NOTW_MONA_SPANISH:     seqTag = CNK_cyya1405; break;
                case NOTW_MONA_DUTCH:       seqTag = CNK_cyya1406; break;
                case NOTW_MONA_SWEDISH:     seqTag = CNK_cyya1407; break;
                case NOTW_MONA_FINNISH:     seqTag = CNK_cyya1408; break;
                case NOTW_MONA_DANISH:      seqTag = CNK_cyya1409; break;
                case NOTW_MONA_NORWEGIAN:   seqTag = CNK_cyya1410; break;
                case NOTW_MONA_AUSTRALIAN:  seqTag = CNK_cyya1413; break;
                case NOTW_MONA_BELGIUM:     seqTag = CNK_cyya1411; break;
                case NOTW_MONA_SINGAPORE:   seqTag = CNK_cyya1412; break;
                case NOTW_MONA_EURO:        seqTag = CNK_cyya1414; break;
                }
                break;


    case 30:    // switch on monatary system
                switch(DISPLAY_state.system)
                {
                case NOTW_MONA_UK:          seqTag = CNK_cyya1502; break;
                case NOTW_MONA_FRENCH:      seqTag = CNK_cyya1503; break;
                case NOTW_MONA_GERMAN:      seqTag = CNK_cyya1504; break;
                case NOTW_MONA_SPANISH:     seqTag = CNK_cyya1505; break;
                case NOTW_MONA_DUTCH:       seqTag = CNK_cyya1506; break;
                case NOTW_MONA_SWEDISH:     seqTag = CNK_cyya1507; break;
                case NOTW_MONA_FINNISH:     seqTag = CNK_cyya1508; break;
                case NOTW_MONA_DANISH:      seqTag = CNK_cyya1509; break;
                case NOTW_MONA_NORWEGIAN:   seqTag = CNK_cyya1510; break;
                case NOTW_MONA_AUSTRALIAN:  seqTag = CNK_cyya1513; break;
                case NOTW_MONA_BELGIUM:     seqTag = CNK_cyya1511; break;
                case NOTW_MONA_SINGAPORE:   seqTag = CNK_cyya1512; break;
                case NOTW_MONA_EURO:        seqTag = CNK_cyya1514; break;
                }
                break;


    case 31:    // switch on monatary system
                switch(DISPLAY_state.system)
                {
                case NOTW_MONA_UK:          seqTag = CNK_cyya1602; break;
                case NOTW_MONA_FRENCH:      seqTag = CNK_cyya1603; break;
                case NOTW_MONA_GERMAN:      seqTag = CNK_cyya1604; break;
                case NOTW_MONA_SPANISH:     seqTag = CNK_cyya1605; break;
                case NOTW_MONA_DUTCH:       seqTag = CNK_cyya1606; break;
                case NOTW_MONA_SWEDISH:     seqTag = CNK_cyya1607; break;
                case NOTW_MONA_FINNISH:     seqTag = CNK_cyya1608; break;
                case NOTW_MONA_DANISH:      seqTag = CNK_cyya1609; break;
                case NOTW_MONA_NORWEGIAN:   seqTag = CNK_cyya1610; break;
                case NOTW_MONA_AUSTRALIAN:  seqTag = CNK_cyya1613; break;
                case NOTW_MONA_BELGIUM:     seqTag = CNK_cyya1611; break;
                case NOTW_MONA_SINGAPORE:   seqTag = CNK_cyya1612; break;
                case NOTW_MONA_EURO:        seqTag = CNK_cyya1614; break;
                }
                break;

    }

#endif  //!USA_VERSION

    return seqTag;
}
