#ifndef __UDPENNY_H__
#define __UDPENNY_H__

/*************************************************************
*
*   FILE:             UDPENNY.H
*
*   (C) Copyright 1999 Artech Digital Entertainments.
*                      All rights reserved.
*
*************************************************************/

/************************************************************/
/* DEFINES                                                  */
/************************************************************/

/************************************************************/

/************************************************************/
/* ENUMS                                                    */
/************************************************************/
enum
{
    PB_WelcomeGame_old = 0,                                      // WAV_pb001         Hello! Welcome to Monopoly. I'm Rich Uncle Pennybags and this is my little friend.
    PB_Menu_old,                                                 // WAV_pb002         These menus will let you do things like; change some options, answer your questions or if you like, quit the game.
    PB_ChooseNetworkOrLocalGame_old,                             // WAV_pb003         First of all, would you like to start a network game?
    PB_NoUserInputForXSeconds_old,                               // WAV_pb004         What would you like to do?
                                                                 // WAV_pb005         What's it going to be?
                                                                 // WAV_pb006         Your choice?
                                                                 // WAV_pb007         It's up to you.
                                                                 // WAV_pb008         Come on!
    PB_HumanPlayerPickToken_old,                                 // WAV_pb009         What token would you like?
                                                                 // WAV_pb010         Pick a token. Any token!
                                                                 // WAV_pb011         What's your favorite token?
                                                                 // WAV_pb012         Which token would you like to be?
                                                                 // WAV_pb013         What token would you like to play with?
    PB_ChoosePlayer_HumanOrComputer_old,                         // WAV_pb014         Are you playing with someone else or would you like us to provide a computer opponent?
    PB_ChooseTokenForComputerPlayer_old,                         // WAV_pb015         Which token would you like your computer opponent to use?
                                                                 // WAV_pb016         Your computer opponent can be any token - pick one!
    PB_NewPlayerEnterNameOrPickFromList_old,                     // WAV_pb017         Alright! Now enter your name or pick from the list.
    PB_NewPlayerEnterName_old,                                   // WAV_pb018         O.K.! Simply enter your name.
    PB_ChooseAIDifficultyLevel_old,                              // WAV_pb019         Pick the skill level of your computer opponent.
                                                                 // WAV_pb020         So how tough do you want this opponent to be?
    PB_ChooseClassicOrCityBoard_AnyVersion_old,                  // WAV_pb021         Would you like to play on the classic Monopoly board or pick another city?
    PB_ChooseStandardOrCustomRules_old,                          // WAV_pb022         You can use the standard Monopoly rules or we have some custom variations if you like?
    PB_SummaryScreen_old,                                        // WAV_pb023         OK then. Add more players or start the game?
    PB_GameStarted_old,                                          // WAV_pb024         It's Monopoly time!  Let's advance to GO!
    PB_NegotiateRules_old,                                       // WAV_pb025         Someone wants to play with some special rules. That's fine! Once everyone agrees, we can get this show on the road.
    PB_PromptEachPlayerForCustomRulesConfirmation_old,           // WAV_pb026         Cannon, do you agree to these rules?
                                                                 // WAV_pb027         Racecar, do you agree to these rules?
                                                                 //                   - MISSING DOG // MIKE/MMH
                                                                 // WAV_pb028         These rules okay with you, Top Hat?
                                                                 // WAV_pb029         Iron, do you agree to these rules?
                                                                 // WAV_pb030         These rules all right with you, Horse and Rider?
                                                                 // WAV_pb031         Battleship, you okay with all this?
                                                                 // WAV_pb032         Shoe, these rules okay with you?
                                                                 // WAV_pb033         Thimble, do you agree to these rules?
                                                                 // WAV_pb034         Wheelbarrow, do you agree to these rules?
                                                                 // WAV_pb035         Do you agree to these rules, Sack of Money?
    PB_CustomRulesAgreedOnStartGame_old,                         // WAV_pb036         Alright then.  Everyone's in agreement. So let's advance to GO!
    PB_MainScreenIntro_old,                                      // WAV_pb037         This is the main Monopoly screen.
                                                                 // WAV_pb038         You can click on the buttons at the bottom to roll the dice, buy property or just about anything else you'd like to do.
                                                                 // WAV_pb039         You can even watch the tokens move around the board from different cameras.
                                                                 // WAV_pb040         Everyone ready? Come on let's play Monopoly!
    PB_RollDice_Cannon,                                          // WAV_pb041         Cannon. You take a shot at it.
                                                                 // WAV_pb042         Time for the heavy artillery. Let's go Cannon.
                                                                 // WAV_pb043         Cannon. Roll those dice.
                                                                 // WAV_pb044         Bringing out the big guns. Your turn, Cannon.
                                                                 // WAV_pb045         You have control of the dice, Cannon.
                                                                 // WAV_pb046         Cannon. Ten-hut!
    PB_RollDice_Car,                                             // WAV_pb047         Racecar, let's roll.
                                                                 // WAV_pb048         Racecar, burn a little rubber.
                                                                 // WAV_pb049         Racecar, get your motor runnin!
                                                                 // WAV_pb050         Let's motor, Racecar.
                                                                 // WAV_pb051         Racecar, start your engine.
    PB_RollDice_Dog,                                             // WAV_pb052         It's the dog's turn.
                                                                 // WAV_pb053         O.K. doggie. Good luck.
                                                                 // WAV_pb054         Your turn doggie.
                                                                 // WAV_pb055         Roll those bones, doggie.
                                                                 // WAV_pb056         It's over to you, dog.
    PB_RollDice_Hat,                                             // WAV_pb057         All right Top Hat, it's your turn to roll.
                                                                 // WAV_pb058         Top Hat, it's up to you.
                                                                 // WAV_pb059         Top Hat, roll and good luck.
                                                                 // WAV_pb060         Top Hat, toss those ivories.
                                                                 // WAV_pb066         You've got control of the dice, Top Hat.
    PB_RollDice_Iron,                                            // WAV_pb067         OK Iron, click to roll.
                                                                 // WAV_pb068         Your turn Iron.
                                                                 // WAV_pb069         Good luck, Iron.
                                                                 // WAV_pb070         OK Iron, press on.
                                                                 // WAV_pb071         It's Iron's turn.
    PB_RollDice_Horse,                                           // WAV_pb072         Horse and rider. You're at the starting gate.
                                                                 // WAV_pb073         Horse and rider. You have the dice.
                                                                 // WAV_pb074         Good luck, Horse and rider.
                                                                 // WAV_pb075         It's your turn Horse and Rider.
                                                                 // WAV_pb076         Show us your stuff, Horse and rider.
    PB_RollDice_Ship,                                            // WAV_pb077         Battleship! Fire when ready!
                                                                 // WAV_pb078         Battleship! You're up.
                                                                 // WAV_pb079         Battleship! Full speed ahead.
                                                                 // WAV_pb080         You have the dice Battleship.
                                                                 // WAV_pb081         Toss those cubes, Battleship
    PB_RollDice_Shoe,                                            // WAV_pb082         O.K. Shoe.  Step on it.
                                                                 // WAV_pb083         Shoe.  Let's hit the bricks.
                                                                 // WAV_pb084         O.K. Shoe.  Roll those dice.
                                                                 // WAV_pb085         It's your turn shoe.
                                                                 // WAV_pb086         You're up, shoe. Good luck.
    PB_RollDice_Thimble,                                         // WAV_pb087         All right Thimble, roll them bones.
                                                                 // WAV_pb088         Thimble, you're up.
                                                                 // WAV_pb089         Good luck. Thimble.
                                                                 // WAV_pb090         It's Thimble's turn.
                                                                 // WAV_pb091         OK thimble, good luck.
    PB_RollDice_Wheelbarrow,                                     // WAV_pb092         Wheelbarrow, you got the dice.
                                                                 // WAV_pb093         It's Wheelbarrow's turn.
                                                                 // WAV_pb094         Wheelbarrow!  Good luck! You're up.
                                                                 // WAV_pb095         Let's roll, Wheelbarrow.
                                                                 // WAV_pb096         Wheelbarrow, toss those dice.
    PB_RollDice_SackOfMoney,                                     // WAV_pb097         It's  Sack of Moneys turn.
                                                                 // WAV_pb098         Sack of Money! Roll those dice.
                                                                 // WAV_pb099         Sack of Money. Show us what you can do.
                                                                 // WAV_pb100         Good luck, Sack of Money.
                                                                 // WAV_pb101         Show me the money, Sack!
    PB_RollDice_Generic,                                         // WAV_pb102         Roll those dice.
                                                                 // WAV_pb103         Let's go!
                                                                 // WAV_pb104         C'mon, let's play!
                                                                 // WAV_pb105         Let's get going!
                                                                 // WAV_pb106         Let's do it.
                                                                 // WAV_pb107         Toss those cubes.
                                                                 // WAV_pb108         Roll the ivories.
                                                                 // WAV_pb109         Let's keep this game rolling.
                                                                 // WAV_pb110         Move along here. Let's go!
                                                                 // WAV_pb111         Who's turn is it! Come on!
    PB_SayDiceRoll_2,                                            // WAV_pb112         Two!
                                                                 // WAV_pb113         Snake eyes!
                                                                 // WAV_pb114         A deuce!
    PB_SayDiceRoll_3,                                            // WAV_pb115         Three!
                                                                 // WAV_pb116         You rolled a three!
                                                                 // WAV_pb117         Ace, deuce = three
    PB_SayDiceRoll_4,                                            // WAV_pb118         Four!
                                                                 // WAV_pb119         Looks like a four to me.
                                                                 // WAV_pb120         We got four!
    PB_SayDiceRoll_5,                                            // WAV_pb121         Five!
                                                                 // WAV_pb122         We got five!
                                                                 // WAV_pb123         A nickel.
                                                                 // WAV_pb124         Cinco!
    PB_SayDiceRoll_6,                                            // WAV_pb125         Six!
                                                                 // WAV_pb126         Half a dozen!
                                                                 // WAV_pb127         We got six.
    PB_SayDiceRoll_7,                                            // WAV_pb128         Seven!
                                                                 // WAV_pb129         Lucky seven!
                                                                 // WAV_pb130         Looks like a seven to me!
    PB_SayDiceRoll_8,                                            // WAV_pb131         Eight!
                                                                 // WAV_pb132         Eight! Looking great!
                                                                 // WAV_pb133         Eight! Let's see where that takes us!
                                                                 // WAV_pb134         Eighter from Decatur!
    PB_SayDiceRoll_9,                                            // WAV_pb135         Nine!
                                                                 // WAV_pb136         Niner!
                                                                 // WAV_pb137         Looks like a nine. Where does that take us?
    PB_SayDiceRoll_10,                                           // WAV_pb138         Ten!
                                                                 // WAV_pb139         Both hands!
                                                                 // WAV_pb140         We got ten!
    PB_SayDiceRoll_11,                                           // WAV_pb141         Eleven!
                                                                 // WAV_pb142         Looks like eleven to me!
                                                                 // WAV_pb143         Six and five! Eleven!
    PB_SayDiceRoll_12,                                           // WAV_pb144         Twelve!
                                                                 // WAV_pb145         Hey! Doubles! Twelve!
                                                                 // WAV_pb146         A pair of sixes! Twelve!
                                                                 // WAV_pb147         Twelve! Box cars!
    PB_TokenLandsOnUnownedProperty,                              // WAV_pb148         Are you going to buy this property or put it up for auction?
                                                                 // WAV_pb149         Buy or auction? What'll it be?
                                                                 // WAV_pb150         You going to buy it or do we have an auction?
                                                                 // WAV_pb151         Buy or auction? You decide.
                                                                 // WAV_pb152         What to do? Buy it or put it under the hammer?
                                                                 // WAV_pb153         What'll it be? Buy - or is it auction time?
                                                                 // WAV_pb154         Auction it or keep it? What do you want to do?
                                                                 // WAV_pb155         It's up to you? Auction it or keep it for yourself?
                                                                 // WAV_pb156         Sell at auction or buy it? You decide!
                                                                 // WAV_pb157         Two choices - buy or auction? What'll it be?
    PB_BuyBoardSide1,                                            // WAV_pb158         Good choice. Even these properties can be valuable.
                                                                 // WAV_pb159         It's a real steal!
                                                                 // WAV_pb160         Cheap at twice the price!
    PB_BuyBoardSide2,                                            // WAV_pb161         Nice property!
                                                                 // WAV_pb162         Good buy!
                                                                 // WAV_pb163         Wise decision to buy.
    PB_BuyBoardSide3,                                            // WAV_pb164         I'll bet that purchase pays off later!
                                                                 // WAV_pb165         Good decision.
                                                                 // WAV_pb166         Very good move.
    PB_BuyBoardSide4,                                            // WAV_pb167         These are certainly the best properties to acquire alright.
                                                                 // WAV_pb168         This is prime real estate. Way to go!
                                                                 // WAV_pb169         Excellent! Very good neighbourhood!
    PB_BuyRailroad,                                              // WAV_pb170         You'll be a railroad tycoon in no time.
                                                                 // WAV_pb171         You're definitely on the right track!
                                                                 // WAV_pb172         Keep that "train" of thought!
    PB_BuyUtility,                                               // WAV_pb173         Smart buy!  They're also useful for trading
                                                                 // WAV_pb174         Utilities are always a clever investment.
                                                                 // WAV_pb175         You can't go wrong with a utility!
    PB_LandOn_Chance,                                            // WAV_pb176         Chance!
                                                                 // WAV_pb177         Chance. Let's see what it is.
                                                                 // WAV_pb178         Chance! I love these! Let's look!
                                                                 // WAV_pb179         Chance! You feeling lucky?
                                                                 // WAV_pb180         Chance! I love surprises!
    PB_LandOn_CommunityChest,                                    // WAV_pb181         Community Chest! What will it be?  Pay out or be paid?
                                                                 // WAV_pb182         Community Chest! Let's see what it is!
                                                                 // WAV_pb183         Community Chest! Let's look!
                                                                 // WAV_pb184         Community Chest! Show me that card!
                                                                 // WAV_pb185         Community Chest! What's it going to be?
    PB_ChanceCard,                                               // WAV_pb186         Advance to go  (collect $200)
                                                                 // WAV_pb187         Advance to St. Charles Place.  If you pass go collect $200
                                                                 // WAV_pb188         Advance token to nearest utility.  If unowned you may buy it from the bank.  If owned, throw dice and pay owner a total ten times ...
                                                                 // WAV_pb189         Your building and loan matures.  Collect $150
                                                                 // WAV_pb190         Bank pays you dividend of $50
                                                                 // WAV_pb191         You have been elected chairman of the board. Pay each player $50
                                                                 // WAV_pb192         Take a walk on the Board Walk.  Advance token to board walk.
                                                                 // WAV_pb193         Take a ride on the Reading.  If you pass GO collect $200.
                                                                 // WAV_pb194         Advance token to the nearest Railroad and pay owner twice the rental to which he is otherwise entitled.  If railroad is unowned, ...
                                                                 // WAV_pb195         Advance token to the nearest Railroad and pay owner twice the rental to which he is otherwise entitled.  If railroad is unowned, ...
                                                                 // WAV_pb196         Make general repairs on your property.  For each house pay $25.  For each hotel $100.
                                                                 // WAV_pb197         This card may be kept until needed or sold.  Get out of jail free
                                                                 // WAV_pb198         Pay poor tax of $15.
                                                                 // WAV_pb199         Advance to Illinois Ave.
                                                                 // WAV_pb200         Go to jail.  Go directly to jail. Do not pass GO.  Do not collect $200.
                                                                 // WAV_pb201         Go back 3 spaces
    PB_CommunityChestCard,                                       // WAV_pb202         Life insurance matures.  Collect $100
                                                                 // WAV_pb203         You inherit $100.
                                                                 // WAV_pb204         Xmas fund matures.  Collect $100.
                                                                 // WAV_pb205         You have won second prize in a beauty contest.  Collect $10.
                                                                 // WAV_pb206         Get out of jail, free.  This card may be kept until needed or sold.
                                                                 // WAV_pb207         Doctor's fee.  Pay $50.
                                                                 // WAV_pb208         Bank error in your favour.  Collect $200.
                                                                 // WAV_pb209         Income tax refund Collect $20
                                                                 // WAV_pb210         You are assessed for street repairs.  $40 per house.  $115 per hotel.
                                                                 // WAV_pb211         Go to jail.  Go directly to jail. Do not pass GO.  Do not collect $200.
                                                                 // WAV_pb212         From sale of stock you get $45.
                                                                 // WAV_pb213         Grand opera opening.  Collect $50 from every player for opening night seats.
                                                                 // WAV_pb214         Advance to go. (collect $200)
                                                                 // WAV_pb215         Pay school tax of $150.
                                                                 // WAV_pb216         Receive for services $25
                                                                 // WAV_pb217         Pay hospital $100.
    PB_LandOn_JustVisiting,                                      // WAV_pb218         Just visiting the jail.
                                                                 // WAV_pb219         Just visiting - this time anyway.
                                                                 // WAV_pb220         Only a visit this time.
    PB_LandOn_FreeParking,                                       // WAV_pb221         Free parking!
                                                                 // WAV_pb222         Free Parking! Take a break.
                                                                 // WAV_pb223         Take a rest, you can park it for free.
    PB_LandOn_FreeParkingWithFreeParkingRuleOn,                  // WAV_pb224         Jackpot!  Congratulations!
                                                                 // WAV_pb225         Aren't you the lucky one!
                                                                 // WAV_pb226         Way to go! You get the jackpot!
    PB_LandOn_GoToJail_Negative,                                 // WAV_pb227         Oh oh!  Go to jail. Too bad.
                                                                 // WAV_pb228         Go to jail! Tough luck!
                                                                 // WAV_pb229         Go to jail! Bad break!
    PB_LandOn_GoToJail_Positive,                                 // WAV_pb230         Well, this is safe anyway.
                                                                 // WAV_pb231         You dodged a bullet there.
                                                                 // WAV_pb232         Go to Jail!  Sometimes you get lucky!
    PB_JailPayMoneyOrTryForDoubles,                              // WAV_pb233         So.  Would you like to pay $50 or try to roll doubles?
                                                                 // WAV_pb234         What'll it be? Roll the dice or pay the 50$.
                                                                 // WAV_pb235         Roll or pay up?
    PB_CollectMoney_Go,                                          // WAV_pb236         Here's your salary. 200 dollars!
                                                                 // WAV_pb237         200 dollars! That's easy money!
                                                                 // WAV_pb237b        Money for nothing! Here you go! 200 dollars.
                                                                 // WAV_pb238         Around the board again! Here's your two hundred dollars!
                                                                 // WAV_pb239         200 dollars! And you didn't even do anything.
                                                                 // WAV_pb240         Back so soon? Here's your 200 dollars!
                                                                 // WAV_pb241         Pass GO - collect 200 dollars! It's easy!
                                                                 // WAV_pb242         Here’s 200$ - just for looking so good!
                                                                 // WAV_pb243         Another turn around the board - another 200 dollars!
                                                                 // WAV_pb244         Here's your salary again! Just for passing GO!
                                                                 // WAV_pb245         Here's some easy money!
    PB_CollectMoney_LandedOnGoAndDoublePayRuleInEffect,          // WAV_pb246         Here's double your salary for landing on GO this time
                                                                 // WAV_pb247         You get twice the amount this time for landing on go.
                                                                 // WAV_pb248         And an extra $200! That should help the cause!
                                                                 // WAV_pb249
    PB_LandOn_LuxuryTax,                                         // WAV_pb250         That will be $75 please
                                                                 // WAV_pb251         Luxury Tax! Too bad! But pay up!
                                                                 // WAV_pb252         Luxury tax! Them's the breaks!
    PB_LandOn_IncomeTax,                                         // WAV_pb253         $200 or 10% of your assets?
                                                                 // WAV_pb254         You can't hide from the Taxman.
                                                                 // WAV_pb255         Tough luck! But you gotta pay!
    PB_Auction,                                                  // WAV_pb256         The bank will now auction this property.
                                                                 // WAV_pb257         It's auction time!
                                                                 // WAV_pb258         This property will go to the highest bidder.
    PB_Auction_ExplanationOnButtonsToIncreaseBid,                // WAV_pb259         Just click these buttons to increase your bid
                                                                 // WAV_pb260         To increase your bid, click on these buttons.
                                                                 // WAV_pb261         The bidding is done by clicking on these buttons.
    PB_Auction_OpeningBid,                                       // WAV_pb262         Who would like to make an opening bid?
                                                                 // WAV_pb263         Who's going to start us off?
                                                                 // WAV_pb263b        Who's going to open the bidding?
    PB_Auction_CallOutTokenWithHighestBid,                       // WAV_pb264         Cannon -
                                                                 // WAV_pb265         Racecar -
                                                                 // WAV_pb266         Dog -
                                                                 // WAV_pb266b        Top Hat -
                                                                 // WAV_pb267         Iron -
                                                                 // WAV_pb268         Horse and rider -
                                                                 // WAV_pb269         Battleship -
                                                                 // WAV_pb270         Shoe -
                                                                 // WAV_pb271         Thimble -
                                                                 // WAV_pb272         Wheel barrow -
                                                                 // WAV_pb273         Sack of Money -
    PB_Auction_GoingOnce,                                        // WAV_pb274         Going once -
    PB_Auction_GoingTwice,                                       // WAV_pb275         Going twice -
    PB_Auction_GoingSold,                                        // WAV_pb276         Sold!
                                                                 // WAV_pb277         Gone!
    PB_Auction_CongratulateWinner,                               // WAV_pb278         Congratulations on buying a splendid property.
                                                                 // WAV_pb279         Way to go! You were the highest bidder!
                                                                 // WAV_pb280         You are the owner of a fine new property!
    PB_PlayerClicks_Mortgage,                                    // WAV_pb281         Which property would you like to mortgage?
                                                                 // WAV_pb282         Mortgage! For which property?
                                                                 // WAV_pb283         And you'd like to mortgage which property?
    PB_PlayerClicks_UnMortgage,                                  // WAV_pb284         Unmortgage! Which property?
                                                                 // WAV_pb285         For which property would you like to pay off the mortgage?
                                                                 // WAV_pb286         And which property would you like to unmortgage?
    PB_PlayerClicks_BuyHouseHotel,                               // WAV_pb287         On which property would you like to build?
                                                                 // WAV_pb288         Buy? For which property?
                                                                 // WAV_pb289         So you're going to buy a house or hotel? For which property?
    PB_PlayerClicks_SellHouseHotel,                              // WAV_pb290         Which building would you like to sell?
                                                                 // WAV_pb291         What are you going to sell?
                                                                 // WAV_pb292         Sell? Which property?
    PB_PlayerAcquiredMonopoly,                                   // WAV_pb293         A Monopoly!  All right!
                                                                 // WAV_pb294         A Monopoly! We all know what that means - double the rent on all these properties.
                                                                 // WAV_pb295         A Monopoly! Just let anybody land there now!
    PB_PlayerBuiltFirstHouse,                                    // WAV_pb296         Ahh a house.  Maybe the first of many!
                                                                 // WAV_pb297         Your first house! Terrific!
                                                                 // WAV_pb298         Your first house! You have to start building that empire sometime.
    PB_PlayerBuiltFirstHotel,                                    // WAV_pb299         Your first hotel!
                                                                 // WAV_pb300         Hotel number one!
                                                                 // WAV_pb301         Hopefully this will be the first of many!
    PB_StatusScreen_FirstTime,                                   // WAV_pb302         Let's see how everybody's doing.
    PB_StatusScreen_FirstTimeDeeds,                              // WAV_pb303         Here are all the deeds.  You can see who owns what.
    PB_StatusScreen_FirstTimeBankAssets,                         // WAV_pb304         Here are the assets remaining in the bank.
    PB_Calculator,                                               // WAV_pb305         Wondering "What if?" Just try clicking on these buttons.
    PB_TradeScreen,                                              // WAV_pb306         Are you ready to do some trading?
                                                                 // WAV_pb307         Click and drag the properties you want to offer into this area.
                                                                 // WAV_pb308         Properties you want to receive go here.
    PB_TradeScreen_ProposeClickedButOneSideHasNotOfferedAnything,// WAV_pb309         During a trade, the players involved must both give up and receive something
                                                                 // WAV_pb310         The players trading have to give as well as they get.
                                                                 // WAV_pb311         You'll have to give something up if you’re going to get anything during a trade.
    PB_TradeScreen_ThirdPartyTradeExplanation,                   // WAV_pb312         If  there's a third player you'd like involved in the trade, click "Conditional"
    PB_TradeScreen_PickTradePartner,                             // WAV_pb313         Who would you like to trade with?
                                                                 // WAV_pb314         Who has a property you'd like to trade for?
                                                                 // WAV_pb315         And you'd like to trade with whom?
    PB_TradeScreen_TradeComplete,                                // WAV_pb316         The trade is complete!
                                                                 // WAV_pb317         Everyone accepts the trade! Well done!
                                                                 // WAV_pb318         A successful trade! Yes!
                                                                 // WAV_pb319         It's a done deal!
                                                                 // WAV_pb320         Trade concluded!
    PB_TradeScreen_TradeCancelled,                               // WAV_pb321         Uh oh.  The trade is cancelled.
                                                                 // WAV_pb322         Looks like the deal's off.
                                                                 // WAV_pb324         Negotiations have been suspended, sorry!
    PB_HousingShortage_FirstTimeHouses,                          // WAV_pb325         Oh oh! There's a housing shortage.  Your opponent wants to buy one of the last ones.  Would you like to bid on this house at auction
    PB_HousingShortage_FirstTimeHotels,                          // WAV_pb326         Unfortunately, there's a hotel shortage.  And your  opponent wants to buy a remaining hotel.  Would you like to bid on this hotel ...
    PB_HousingShortage_NotFirstTime,                             // WAV_pb327         The bank is running low on those buildings.  Should it hold an auction?
                                                                 // WAV_pb328         Last one!  Want to bid?
                                                                 // WAV_pb329         There's still a shortage.  Would you like to bid?
    PB_Auction_HurryPlayersBid,                                  // WAV_pb330         Quick now, bid or not?
    PB_Auction_Building,                                         // WAV_pb331         Auction this building?
                                                                 // WAV_pb332         Would you like to have an auction for this building?
    PB_Auction_Once,                                             // WAV_pb333         Once
    PB_Auction_Twice,                                            // WAV_pb334         Twice
    PB_Auction_LostTimeUp,                                       // WAV_pb335         Sorry, they get it!
                                                                 // WAV_pb336         Time's up. Sorry!
                                                                 // WAV_pb337         Sorry, we’re all out of time!
    PB_RaisingMoneySuggestion,                                   // WAV_pb338         Short of cash? You'll need to raise some money then.
                                                                 // WAV_pb339         You'll have to raise money by selling buildings, mortgaging property, or trading
                                                                 // WAV_pb340         Strapped for cash? You can always sell buildings, mortgage property or trade!
    PB_HumanReplacedByComputerPlayer,                            // WAV_pb341         It seems like someone has quit our game.  They'll be replaced by a computer opponent.
                                                                 // WAV_pb342         Somebody had to leave? No problem. They'll be replaced by a computer opponent.
                                                                 // WAV_pb343         Somebody drop out? We'll replace them with a computer opponent then.
    PB_Trade_FirstTimeFuture,                                    // WAV_pb344         A future! This means the holder, not the owner, would collect all future rent on the property, for a number of turns.
                                                                 // WAV_pb345         Future consideration! This means the holder, not the owner, would collect future rent on the property, for a certain number of times
                                                                 // WAV_pb346         A future! For a number of turns, when anyone lands on the propety, the holder, not the owner will collect the rent.
    PB_Trade_FirstTimeImmunity,                                  // WAV_pb347         An Immunity!  This means the holder would be immune from paying rent on the property, for a number of times they land on it.
                                                                 // WAV_pb348         An Immunity means the holder wouldn’t have to pay rent on that property, for a certain number of times.
                                                                 // WAV_pb349         The holder of an Immunity would be exempt from paying rent on the property, for a number of times.
    PB_Bankrupt,                                                 // WAV_pb350         Your luck just ran out. Sorry!
                                                                 // WAV_pb351         That's it for you. Better luck next time!
                                                                 // WAV_pb352         If you can't pay, you can't play. Sorry!
                                                                 // WAV_pb353         Game over!
    PB_GameOver,                                                 // WAV_pb354         Alright!  We have a winner!
                                                                 // WAV_pb355         It's all over! We have a champion!
                                                                 // WAV_pb356         Another Monopoly champ is crowned! Way to go!
    PB_PlayAgain,                                                // WAV_pb357         Would you like to play again?
                                                                 // WAV_pb358         Want another shot at it?
                                                                 // WAV_pb359         How about another game?
    PB_Exit,                                                     // WAV_pb360         See you next time!
                                                                 // WAV_pb361         Come back soon!
                                                                 // WAV_pb362         Bye Bye!
                                                                 // WAV_pb363         So long!  Let’s play again real soon!
                                                                 // WAV_pb364         Don't be a stranger!
    PB_Proposal_GoodDeal,                                        // WAV_pb365         What a steal! Grab it!
                                                                 // WAV_pb366         Sounds like a good deal to me!
                                                                 // WAV_pb367         That seems pretty fair to me.
                                                                 // WAV_pb368         I'd grab that offer if I were you.
                                                                 // WAV_pb369         That's a once in a lifetime offer.
                                                                 // WAV_pb370         That's fair. Sure!
    PB_Proposal_NotGoodDeal,                                     // WAV_pb371         Hmmm. I don’t know about this deal?
                                                                 // WAV_pb372         Are you sure you want to do this?
                                                                 // WAV_pb373         I'm not sure this offer is even worth considering.
                                                                 // WAV_pb374         I don't think it’s such a good proposal.
                                                                 // WAV_pb375         That doesn't seem very fair.
                                                                 // WAV_pb376         Are they joking? Don’t even consider it!
    PB_BoardSquareNames_US,                                      // WAV_pb377         Mediterranean Avenue
                                                                 // WAV_pb378         Baltic Avenue
                                                                 // WAV_pb379         Oriental Avenue
                                                                 // WAV_pb380         Vermont Avenue
                                                                 // WAV_pb381         Connecticut Avenue
                                                                 // WAV_pb382         St. Charles Place
                                                                 // WAV_pb383         States Avenue
                                                                 // WAV_pb384         Virginia Avenue
                                                                 // WAV_pb385         St. James Place
                                                                 // WAV_pb386         Tennesee Avenue
                                                                 // WAV_pb387         New York Avenue
                                                                 // WAV_pb388         Kentucky Avenue
                                                                 // WAV_pb389         Indiana Avenue
                                                                 // WAV_pb390         Atlantic Avenue
                                                                 // WAV_pb391         Ventnor Avenue
                                                                 // WAV_pb392         Marvin Gardens
                                                                 // WAV_pb393         Pacific Avenue
                                                                 // WAV_pb394         North Carloina Avenue
                                                                 // WAV_pb395         Pennsylvania Avenue
                                                                 // WAV_pb396         Park Place
                                                                 // WAV_pb397         Board Walk
                                                                 // WAV_pb398         Reading Railroad
                                                                 // WAV_pb399         Pennsylvania Railroad
                                                                 // WAV_pb400         B&O Railroad
                                                                 // WAV_pb401         Short Line
                                                                 // WAV_pb402         NY Bronx
                                                                 // WAV_pb403         NY etc.
                                                                 // WAV_pb404         NY
                                                                 // WAV_pb405         NY
                                                                 // WAV_pb406         NY
                                                                 // WAV_pb407         NY
                                                                 // WAV_pb408         NY
                                                                 // WAV_pb409         NY
                                                                 // WAV_pb410         NY
                                                                 // WAV_pb411         NY
                                                                 // WAV_pb412         NY
                                                                 // WAV_pb413         NY
                                                                 // WAV_pb414         NY
                                                                 // WAV_pb415         NY
                                                                 // WAV_pb416         NY
                                                                 // WAV_pb417         NY
                                                                 // WAV_pb418         NY
                                                                 // WAV_pb419         NY
                                                                 // WAV_pb420         NY
                                                                 // WAV_pb421         NY
                                                                 // WAV_pb422         NY
                                                                 // WAV_pb423         NY
                                                                 // WAV_pb424         NY
                                                                 // WAV_pb425         NY
                                                                 // WAV_pb426         NY
                                                                 // WAV_pb427         NY
    PB_WelcomeGame,                                              // WAV_pba01         Hello there!  You can call me Mr. Monopoly. Welcome!
                                                                 // WAV_pba02         Mr. Monopoly - that's me.  Welcome!
                                                                 // WAV_pba03         Hi! I'm Mr. Monopoly!  Welcome to the game.
                                                                 // WAV_pba04         Hello!  I'm Mr. Monopoly and I'd like to welcome you to the game.
                                                                 // WAV_pba05         Hi! I'm Mr. Monopoly! Let me help you get started.
                                                                 // WAV_pba06         Welcome to Monopoly!
                                                                 // WAV_pba07         Welcome!
    PB_AnnouncePreviousHighScoresIfAny,                          // WAV_pbb01         You too could be among these top players!
                                                                 // WAV_pbb02         Now here's an inspiring sight … previous winners!
                                                                 // WAV_pbb03         Here are some top scores.
                                                                 // WAV_pbb04         Take a look at how some of the previous winners did.
                                                                 // WAV_pbb05         Here's how some of the previous winners did.
                                                                 // WAV_pbb06         Here are the scores for some previous winners.
    PB_ChooseNetworkOrLocalGame,                                 // WAV_pbc01         So what will it be?  Play right here or over a network?
                                                                 // WAV_pbc02         Would you like to play a network or a local game?
                                                                 // WAV_pbc03         Now first, want to play over a network or just a local game?
                                                                 // WAV_pbc04         Would you like to play over a network or just on this local computer?
                                                                 // WAV_pbc05         To begin, you can play on this local computer or connect over a network.
    PB_NewPlayerClickNameOrPressButton,                          // WAV_pbd01         See your name? Click it.  New player?  Click the button.
                                                                 // WAV_pbd02         If you've played here before, you should see your name.  Or click "New Player".
                                                                 // WAV_pbd03         If your name's on the list click on it. Or click "New Player".
                                                                 // WAV_pbd04         If you see your name, click on it. Or click on "New Player".
                                                                 // WAV_pbd05         Have a look through the cards for your name.  If you're new, click "New player".
    PB_NewPlayerEnterName,                                       // WAV_pbe01         Ah, new are you?  Can you enter your name?
                                                                 // WAV_pbe02         Just type in your name, please.
                                                                 // WAV_pbe03         OK. What's your name?
                                                                 // WAV_pbe04         New Player?  Just type your name.
                                                                 // WAV_pbe05         Alright, name please?
    PB_NameComputerPlayer,                                       // WAV_pbf01         What's a good name for this player?
                                                                 // WAV_pbf02         And who is this player?
                                                                 // WAV_pbf03         What should we call this player?
                                                                 // WAV_pbf04         And who's this?
                                                                 // WAV_pbf05         What name would you like to use for this player?
    PB_HumanPlayerPickToken,                                     // WAV_pbg01         Please pick a token.
                                                                 // WAV_pbg02         What token would you like?
                                                                 // WAV_pbg03         Which token would you like to be?
                                                                 // WAV_pbg04         Pick a token any token.
                                                                 // WAV_pbg05         What's your favorite token? C'mon choose!
    PB_ChooseTokenForComputerPlayer,                             // WAV_pbh01         Which token should we use for this player?
                                                                 // WAV_pbh02         Which token suits this player?
                                                                 // WAV_pbh03         And as a token ?
                                                                 // WAV_pbh04         And this player needs a token
                                                                 // WAV_pbh05         Which token will it be?
    PB_ChoosePlayer_HumanOrComputer,                             // WAV_pbi01         OK.  Is this player going to be human or a computer AI?
                                                                 // WAV_pbi02         Another player?  OK. A real human or a computer opponent?
                                                                 // WAV_pbi03         Alright. Is this player human or computer?
                                                                 // WAV_pbi04         Human or AI?
                                                                 // WAV_pbi05         Human or Computer for this player?
    PB_ChooseAIDifficultyLevel,                                  // WAV_pbj01         What's the strength of this computer opponent?
                                                                 // WAV_pbj02         Well, if it's a computer opponent, how tough are they?
                                                                 // WAV_pbj03         Is this opponent easy, difficult or somewhere in between?
                                                                 // WAV_pbj04         How tough do you want this computer opponent to be?
                                                                 // WAV_pbj05         You can pick the skill level of this opponent - will it be "First time buyer" "Entrepreneur" or "Tycoon"
                                                                 // WAV_pbj06         Pick the strength of this computer opponent: Easy, medium or difficult.
                                                                 // WAV_pbj07         Novice, intermediate or expert?
                                                                 // WAV_pbj08         How's their strength?  Low, medium or high?


#if 0
// For information purposes:
// Coders doing other languages may need to know that this group has been divided into
// three, and a new wave file added.  This is what it looked like before:
    PB_SummaryScreen,                                            // WAV_pbk01         So, add more players or start the game?
                                                                 // WAV_pbk02         Alright then.  Add more players or click "Start".
                                                                 // WAV_pbk03         When everybody's ready click "start".
                                                                 // WAV_pbk04         You can add more players or click "Start" when you're ready.
                                                                 // WAV_pbk05         Ready? Just click start then.
#endif

    PB_SummaryScreenWithLessThanTwoPlayers,                      // WAV_pbk01         So, add more players...
    PB_SummaryScreenWithTwoToFivePlayers,                        // WAV_pbk02         So, add more players or start the game?
                                                                 // WAV_pbk03         Alright then.  Add more players or click "Start".
                                                                 // WAV_pbk04         You can add more players or click "Start" when you're ready.
    PB_SummaryScreenWithSixPlayers,                              // WAV_pbk05         When everybody's ready click "start".
                                                                 // WAV_pbk06         Ready? Just click start then.



    PB_ChooseClassicOrCityBoard_AnyVersion,                      // WAV_pbl01         Go ahead, pick the board.
                                                                 // WAV_pbl02         Which board would you like to play on?
                                                                 // WAV_pbl03         Would you like to change the board?
                                                                 // WAV_pbl04         Which board will it be this time?
                                                                 // WAV_pbl05         Any changes to the board, or stick with the Classic?
                                                                 // WAV_pbm01         Would you like to use a special board?
                                                                 // WAV_pbm02         Would you like a special Monopoly board?
                                                                 // WAV_pbm03         And how about a special Monopoly board?
                                                                 // WAV_pbm04         What board would you like to use?
                                                                 // WAV_pbm05         Interested in using a special board?
    PB_ChooseClassicOrCityBoard_Europe,                          // WAV_pbn01         Which country would you like to play in?
                                                                 // WAV_pbn02         You can play the classic board or a board from another country.
                                                                 // WAV_pbn03         Here’s where you can choose your classic board or one from another country.
                                                                 // WAV_pbn04         Your classic board or another country?
                                                                 // WAV_pbn05         Which country will it be?
                                                                 // WAV_pbn06         You could use a customized board or another country board.
                                                                 // WAV_pbn07         What will it be?  Classic, country or customized board?
    PB_ChooseClassicOrCityBoard_US,                              // WAV_pbo01         Pick the city you'd like to play in  - or use the classic Monopoly board.
                                                                 // WAV_pbo02         What'll it be - the classic Monopoly board or one of our city boards?
                                                                 // WAV_pbo03         You can play on the classic Monopoly board or pick a special city.
                                                                 // WAV_pbo04         Any interest in changing the board to a different city?
                                                                 // WAV_pbo05         And what about a board from another city?
                                                                 // WAV_pbo06         You could use a customized board or another city board.
                                                                 // WAV_pbo07         What will it be?  Classic, city or customized board?
    PB_ChooseCurrency,                                           // WAV_pbp01         What currency would you like?
                                                                 // WAV_pbp02         Pick a currency for this board.
                                                                 // WAV_pbp03         Choose the currency you'd like for this game.
                                                                 // WAV_pbp04         You've got three choices of currency for that board.
                                                                 // WAV_pbp05         If you’re playing that country, you can pick the currency.
    PB_ChooseStandardOrCustomRules,                              // WAV_pbq01         Would you like to play with the standard rules or use some custom variations?
                                                                 // WAV_pbq02         Play with the classic Monopoly rules or add some custom features?
                                                                 // WAV_pbq03         Classic Monopoly rules or add some special variations?
                                                                 // WAV_pbq04         Standard or Custom rules?
    PB_NegotiateRules,                                           // WAV_pbr01         Once everyone agrees on the special rules we can advance to GO.
                                                                 // WAV_pbr02         If everyone's agreed on the custom rules we can get going.
                                                                 // WAV_pbr03         We can get started as soon as everyone agrees on the custom rules.
#if USA_VERSION
    PB_Doubles,                                                  // WAV_pbs01         You rolled doubles so you get to roll again.
                                                                 // WAV_pbs02         Doubles.  Roll again.
                                                                 // WAV_pbs03         Doubles.  That makes it still your turn.
                                                                 // WAV_pbs04         Your last roll was doubles.  Have another one.
                                                                 // WAV_pbs05         Doubles means roll again.
    PB_RollAgain,                                                // WAV_pbt01         Have another roll.
                                                                 // WAV_pbt02         OK.  Roll again.
                                                                 // WAV_pbt03         You can roll again.
                                                                 // WAV_pbt04         Take another roll.
                                                                 // WAV_pbt05         Alright, you can roll again.
    PB_SecondTimeRollingDoubles,                                 // WAV_pbu01         That's twice you've rolled doubles.  Once more and you'll be in jail.
                                                                 // WAV_pbu02         Oops.  One more doubles will send you to jail.
                                                                 // WAV_pbu03         Oooo... One more and you'll land in jail.
                                                                 // WAV_pbu04         Doubles... twice in a row!
                                                                 // WAV_pbu05         Doubles... again.
    PB_ThirdTimeRollingDoubles,                                  // WAV_pbv01         Third doubles.  Sorry, off to jail!
                                                                 // WAV_pbv02         That's three doubles.  Bye bye!
                                                                 // WAV_pbv03         Three doubles in a row.  I'm afraid that means some jail time!
                                                                 // WAV_pbv04         Sorry, Go to Jail.
                                                                 // WAV_pbv05         Another doubles.  That means you go to jail.
    PB_CustomRulesScreenHost,                                    // WAV_pbw01         Since you're the network host, you decide the rules.
                                                                 // WAV_pbw02         You're hosting this network game so you can decide the custom rules.
                                                                 // WAV_pbw03         OK host, which special rules do you want?
                                                                 // WAV_pbw04         As the host, you can change the rules.
                                                                 // WAV_pbw05         Custom rules.  What would the host like?
    PB_NonhostPlayersWatchRuleChange,                            // WAV_pbx01         The network host is deciding the rules.
                                                                 // WAV_pbx02         The custom rules are now being determined by the host.
                                                                 // WAV_pbx03         You can see which rules the host is changing.
                                                                 // WAV_pbx04         When the network host has adjusted the rules, we'll start.
                                                                 // WAV_pbx05         Your host is changing the rules.  One moment please.
    PB_HumanInitiatesTrade,                                      // WAV_pby01         Someone's proposing a trade.
                                                                 // WAV_pby02         Somebody wants to trade.
                                                                 // WAV_pby03         Looks like somebody wants a trade.
                                                                 // WAV_pby04         Another player wants to trade.
                                                                 // WAV_pby05         Sonmeone wants a trade.
    PB_AIInitiatesTrade,                                         // WAV_pbz01         A computer opponent is proposing a trade.
                                                                 // WAV_pbz02         An AI player wants a trade.
#endif
    PB_VOICE_MAX
};
/************************************************************/



extern LE_DATA_DataId UDSOUND_Pennybags_Sound[PB_VOICE_MAX][2];



/************************************************************/
/* PROTOS                                                   */
/************************************************************/

BOOL    UDPENNY_IsThisPropertyAMonopoly                 (int property);
void    UDPENNY_BoughtProperty                          (int property, int player);
void    UDPENNY_BuyOrAuction                            (int player);
void    UDPENNY_ChooseToAuctionProperty                 (int property, int player);
void    UDPENNY_LandedOnSquare                          (int player, int square);
void    UDPENNY_NextPlayer                              (int player);
void    UDPENNY_AnnounceRoll                            (int roll);
void    UDPENNY_PassedGo                                (void);
void    UDPENNY_PennybagsGeneral                        (int soundIndex, int clipaction);
void    UDPENNY_PennybagsSpecific                       (int sound, int clipaction);
void    UDPENNY_ReactToLandingOnOtherPlayersProperty    (int player, int square);
void    UDPENNY_StartGame                               (void);
void    UDPENNY_GoToJail                                (void);

LE_DATA_DataId  UDPENNY_CreateDeed(short square, short language, short board, short system, BOOL front);
    //   square   - logical square on the board, 0 to 39
    //              ex. Reading Railroad = square 5
    //   language - pass in value of -1 to use current game setting
    //              otherwise pass in appropriate value from define
    //              found in lang.h (ex. NOTW_LANG_ENGLISH_US)
    //   board    - pass in value of -1 to use current game setting
    //              otherwise pass in appropriate value found in
    //              UDPENNY.H (ex. NOTW_BORD_UK)
    //   system   - pass in value of -1 to use current game setting
    //              otherwise pass in appropriate value found in
    //              UDPENNY.H (ex. NOTW_MONA_UK)
    //   front    - bool determining if the front or the back of
    //              this deed is to be created
    //
    //   return_value - if not successfull returns LED_EI
    //                - if successfull returns the data id of a
    //                  graphics object created.
    //                  User is responsible for cleaning this
    //                  object up with a call to
    //                  LE_DATA_FreeRuntimeDataID (OBJECT_ID);

void UDPENNY_ReturnMoneyValueBasedOnMonatarySystem(int default_value, short monatary_system, ACHAR *StringPntr, BOOL print_symbol);
int UDPENNY_GetCorrectCardCNK(int lastCardIn);

/************************************************************/

// Misc Defines
#define     T_MAX_CURRENCY_FACTORS  4   // used for the four different factors in currencies
#define     T_MAX_LANGS     10
#define     T_MAX_BORDS     13
#define     DEED_WIDTH      199
#define     DEED_HEIGHT     227

// Front of property deeds
#define FP2_Y        18                         // Print Property Name
#define FP2_H        32
#define FP3_Y        (FP2_Y+FP2_H+6)            // Print Rent Value
#define FP3_H        12
#define FP4_Y        (FP3_Y+FP3_H+ 0)           // Print House Rent - Line 1
#define FP4_H        12
#define FP5_Y        (FP4_Y+FP4_H+ 0)           // Print House Rent - Line 2
#define FP5_H        12
#define FP6_Y        (FP5_Y+FP5_H+ 0)           // Print House Rent - Line 3
#define FP6_H        12
#define FP7_Y        (FP6_Y+FP6_H+ 0)           // Print House Rent - Line 4
#define FP7_H        12
#define FP8_Y        (FP7_Y+FP7_H+ 0)           // Print Hotel Rent
#define FP8_H        12
#define FP9_Y        (FP8_Y+FP8_H+ 2)           // Print Mortgage Value
#define FP9_H        12
#define FPa_Y        (FP9_Y+FP9_H+ 0)           // Print Houses Cost
#define FPa_H        12
#define FPb_Y        (FPa_Y+FPa_H+ 0)           // Print Hotels Cost
#define FPb_H        12
#define FPc_Y        (FPb_Y+FPb_H+ 0)           // Print "with 4 houses"
#define FPc_H        12
#define FPd_Y        (FPc_Y+FPc_H+ 2)           // Print "Simple Monopoly Test"
#define FPd_H        24
#define FPe_Y        (DEED_HEIGHT - 22)         // Print "Copywrite"
#define FPe_H        4

// Front of RailRoad deeds
#define FR1_Y        72                         // RailRoad Name
#define FR1_H        12
#define FR2_Y        (FR1_Y+FR1_H+ 8)           // Rent
#define FR2_H        14
#define FR3_Y        (FR2_Y+FR2_H+ 4)           // "If two railroads are owned..."
#define FR3_H        14
#define FR4_Y        (FR3_Y+FR3_H+ 4)           // "If three railroads are owned..."
#define FR4_H        14
#define FR5_Y        (FR4_Y+FR4_H+ 4)           // "If four railroads are owned..."
#define FR5_H        14
#define FR6_Y        (FR5_Y+FR5_H+ 4)           // Mortgage value
#define FR6_H        14
#define FR7_Y        FPe_Y                      // Print "Copywrite"
#define FR7_H        FPe_H

// Front of utility deeds
#define FU1_Y        72                         // Utility Name
#define FU1_H        12
#define FU2_Y        (FU1_Y+FU1_H+ 8)           // "If one utility oned..."
#define FU2_H        54
#define FU3_Y        (FU2_Y+FU2_H+ 0)           // "If both utilites are owned..."
#define FU3_H        54
#define FU4_Y        (FU3_Y+FU3_H+ 0)           // Mortgage value
#define FU4_H        12
#define FU5_Y        FPe_Y                      // Print "Copywrite"
#define FU5_H        FPe_H

// Back of cards
#define B1_Y        27
#define B1_H        30
#define B2_Y        (B1_Y+B1_H+35)
#define B2_H        15
#define B3_Y        (B2_Y+B2_H+2)
#define B3_H        15
#define B4_Y        (B3_Y+B3_H+29)
#define B4_H        30

// Country Board Defines
#define NOTW_BORD_UK                0 // Boards
#define NOTW_BORD_FRENCH            1
#define NOTW_BORD_GERMAN            2
#define NOTW_BORD_SPANISH           3
#define NOTW_BORD_DUTCH             4
#define NOTW_BORD_SWEDISH           5
#define NOTW_BORD_FINNISH           6
#define NOTW_BORD_DANISH            7
#define NOTW_BORD_NORWEGIAN         8
#define NOTW_BORD_BELGIUM           9
#define NOTW_BORD_SINGAPORE         10
#define NOTW_BORD_AUSTRALIAN        11
#define NOTW_BORD_MAX               12
#define NOTW_BORD_NOT_INITIALIZED   -1

// Monatary System Defines
#define NOTW_MONA_UK                NOTW_BORD_UK
#define NOTW_MONA_FRENCH            NOTW_BORD_FRENCH
#define NOTW_MONA_GERMAN            NOTW_BORD_GERMAN
#define NOTW_MONA_SPANISH           NOTW_BORD_SPANISH
#define NOTW_MONA_DUTCH             NOTW_BORD_DUTCH
#define NOTW_MONA_SWEDISH           NOTW_BORD_SWEDISH
#define NOTW_MONA_FINNISH           NOTW_BORD_FINNISH
#define NOTW_MONA_DANISH            NOTW_BORD_DANISH
#define NOTW_MONA_NORWEGIAN         NOTW_BORD_NORWEGIAN
#define NOTW_MONA_AUSTRALIAN        NOTW_BORD_AUSTRALIAN
#define NOTW_MONA_BELGIUM           NOTW_BORD_BELGIUM
#define NOTW_MONA_SINGAPORE         NOTW_BORD_SINGAPORE
#define NOTW_MONA_EURO              12
#define NOTW_MONA_US                13
#define NOTW_MONA_MAX               14
#define NOTW_MONA_NOT_INITIALIZED   -1

struct UDPENNY_PrintInfo
{
    short       y;
    short       h;
    DWORD       Colour;
    short       justification;
    short       vertical_leeway;
    BOOL        bold;
    BOOL        italic;
    BOOL        vert_center;
    short       font_size;
};

/************************************************************/

#endif //  __UDPENNY_H__
