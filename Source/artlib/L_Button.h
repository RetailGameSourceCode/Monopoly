/*******************************************************************************
*
*   FILE:
*   AUTHOR:
*   DESCRIPTION:
*
*   (C) Copyright 1995 Artech Digital Entertainment.  All rights reserved.
*
*******************************************************************************/

#ifndef BUTTON_H
#define BUTTON_H

#include "l_inc.h"

#define LE_BUTTON_BLANK_TRANSITION  { 0, 0, 0, 0, 0, 0, LED_EI, 0, 0, LED_EI, { 0, 0, 0, 0 } }


/*****************************************************************************
 * ENUMS, STRUCTURES etc.
 *****************************************************************************/

typedef enum BUTTON_TypeEnum
{
	BUTTON_TYPE_HOTSPOT = 0,
		/* A hotspot type button is one that displays no graphics but is
		still sensitive to user interaction. This button is always considered 
    visible and will force it to that state even if it has been requested
    to be hidden. If you want to disable a hotspot, you can use isDisabled
    field in the BUTTON_DefinitionRecord. */
	BUTTON_TYPE_MOMENTARY = 1,
		/* A momentary button is one that has an OFF state and a PRESSED state.
		When a mouse button ( or key ) is pressed down, the button displays the
		pressed image. When the button or key is released, the button goes back
		to the OFF state. */
	BUTTON_TYPE_TOGGLE,
		/* A toggle button is one that can either be OFF or ON, and is toggled
		whenever the button is pressed. When the button is defined, it can
		start in the ON state if the isOn flag is set in the button definition 
    record. If no image is provided for either ON or OFF ( onImage
		and offImage respectively ), the button will still be allowed to enter
		those states but will become disabled and invisible. You then must
		manually reset the state to reactivate the button. */
	BUTTON_TYPE_RADIO,
	BUTTON_TYPE_LAST_RADIO,
		/* Defines a set of radio buttons. Radio buttons are essentially a set of
		toggle buttons where only one button in the set is allowed to be in the ON
		state. Pressing a button in the set that is currently OFF will change that
		button to the ON state and cause all other buttons in the set to change to
		the OFF state. If the button that is currently ON is pressed, it will
		remain ON and other buttons in the set will not be affected. When you are 
		defining a set of radio buttons, all the buttons in the set must be type
		BUTTON_TYPE_RADIO, except for the last one, which is type 
		BUTTON_TYPE_LAST_RADIO signifying that it is the last one in the set. */
	BUTTON_TYPE_CYCLE,
	BUTTON_TYPE_LAST_CYCLE,
		/* Defines a set of cycle buttons. Cycle buttons are a set of toggle
		buttons that have no OFF state image that occupy the same area on the
		screen. The effect is that it creates a multistate button that toggles
		between several states. Only one of the buttons in the set is ever ON, the
		others are OFF. Because the others have no image defined, they will be
		invisible and deactivated leaving only the ON button visible and active.
		When that button is pressed and released, it will be turned OFF, becoming
		invisible and inactive and the next button in the set will be set to the ON
		state. This will cause it to become visible and active. If a graphic is 
    provided for the OFF state of a cycle button, it will be ignored. */
	BUTTON_NUMBER_OF_BUTTON_TYPES,

} LE_BUTTON_Type;


typedef enum BUTTON_ActivationEnum
{
	BUTTON_ACTIVATE_LMB = 0,			// Activated by left mouse button.
	BUTTON_ACTIVATE_MMB,					// Activated by middle mouse button.
	BUTTON_ACTIVATE_RMB,					// Activated by right mouse button.
	BUTTON_ACTIVATE_KEY,					// Activated by equivalent key character.
	BUTTON_ACTIVATE_SW						// Activated by software.

} LE_BUTTON_ActivationType;
	/* Defines the different ways a button can be activated. If a button generates
	an event, the numberC parameter in the event message will contain one of these 
  values indicating how the button was activated. */


typedef struct BUTTON_TextInfoStruct
{
	short dimension;
		/* Square dimension for all 9 graphics defined below. Each graphic
		must be a perfect square and they should all be the same size. */

	LE_DATA_DataId topLeftCorner;
	LE_DATA_DataId leftEdge;
	LE_DATA_DataId bottomLeftCorner;
	LE_DATA_DataId bottomEdge;
	LE_DATA_DataId bottomRightCorner;
	LE_DATA_DataId rightEdge;
	LE_DATA_DataId topRightCorner;
	LE_DATA_DataId topEdge;
	LE_DATA_DataId center;
		/* Graphic identifier for all pieces of the text button. As mentioned
		above, each piece must be a square and they all must be the same size. 
    You can define any or all of these items as LED_EI for buttons that 
    have no frame. */

	DWORD offColor;
	DWORD onColor;
	DWORD disabledColor;
		/* Colors used to draw the text inside the text buttons. The colors are
		created using the LE_GRAFIX_MakeColorRef() macro. */

} LE_BUTTON_TextInfoRecord, *LE_BUTTON_TextInfoPointer;
	/* Record that defines values used to draw text buttons. The user
	supplies one of these records for each button list that contains
	text buttons. This information will then be used to render all 
  text buttons defined in that list. */


typedef struct BUTTON_TransitionStruct
{
  unsigned int transitionSequenceDefined : 1;
		/* If TRUE, the value contained in transitionSequence contains a valid
    ID for a transition to be played between the current display state of 
    the button and the next display state. */

  unsigned int nextImageDefined : 1;
    /* If TRUE, the field nextImage contains a graphic ID to an image that
    will replace the current graphic for the buttons next stable state. For
    instance, suppose you define a transition when the button is being 
    pressed. If you supply a graphic for the next image, the buttons current
    pressed image will be replaced with the graphic given in nextImage. */

  unsigned int nextActiveAreaDefined : 1;
    /* If TRUE, the field nextActiveArea defines a new active area for the
    button. This will also be used to display the image. Used if you want
    to move the button. */

  unsigned int redrawButton : 1;
    /* If TRUE, the button will be redrawn in its next stable state. If a 
    transition was started, the next image will be chained to the end of 
    the transition image. */

  unsigned int hideButton : 1;
    /* If TRUE, the button will become hidden. If a transition sequence is
    defined, the button will only be hidden after the transition sequence
    has ended. */

  unsigned int disableButton : 1;
    /* If TRUE, the button will become disabled as soon as the transition
    sequence starts. If the button has a valid disabled image, or an image
    is supplied in nextImage, that image will be chained to the end of the
    transition sequence. */

	LE_DATA_DataId transitionSequence;
		/* Transition sequence to be played. This image really only makes
		sense if it is a sequence because it will be played between two
		looping button images which must be chained.  */

	short transitionX, transitionY;
		/* Starting location to play transition sequence. This is independant
    of the current position of the button. */

	LE_DATA_DataId nextImage;
    /* Image to replace buttons next stable graphic state. This value is
    only used if the nextImageDefined flag is TRUE. */

	RECT nextActiveArea;
    /* Rectangle defining the new active area for the button. This value
    is only used if the nextActiveAreaDefined flag is TRUE. */

} LE_BUTTON_TransitionRecord, *LE_BUTTON_TransitionPointer;


typedef BOOL FUNC_BUTTONNOTIFY( int, int, LE_UIMSG_MessageType, LE_BUTTON_ActivationType );
typedef FUNC_BUTTONNOTIFY *PFUNC_BUTTONNOTIFY;
  /* Defines a prototype for a callback function that can be supplied
	in the callbackFunction field in the BUTTON_DefinitionRecord. The
	routine will be called with the first parameter being the button list
	number and the second parameter the button number. This identifies the
	button that is being activated. The third and fourth parameter are the
	type of button event and the activation method respectively. You can 
  define transitions for one or more buttons which will be started as soon
  as the callback function returns. If you define any transitions, you
  must return TRUE. If no transitions were defined, return FALSE. */


typedef struct BUTTON_DefinitionStruct
{
	unsigned int buttonType : 3;
		/* This describes what kind of button this is.
    BUTTON_TYPE_HOTSPOT = 0
    for buttons that have no graphics but are still sensitive to mouse clicks.
		BUTTON_TYPE_MOMENTARY = 1
		for buttons which you can press (shows pressedImage) but never stays on 
		(goes back to offImage when released).  It also switches to pressed when a 
		roll-over enters it, and released when it exits (if you have that activation 
		method turned on).	
    BUTTON_TYPE_TOGGLE = 2 
    for buttons that have two states - pressing the button shows the pressedImage, 
    and releasing it switches the button to the alternate image (onImage or offImage
    as appropriate). Typically this will be used for checkbox style buttons.
		BUTTON_TYPE_RADIO = 3 and BUTTON_TYPE_LAST_RADIO = 4
		for radio buttons (the last button in the group is marked so you can tell
		where the group ends).	Turning on one button in the radio button
		collection will force all the other buttons in the bunch to turn
		off.  If you click on one that is already on, it stays on.  
		BUTTON_TYPE_CYCLE = 5 and BUTTON_TYPE_LAST_CYCLE = 6
		for a group of buttons used to simulate cycle buttons (think of a button
		that switches between Easy / Medium / Hard). Just make all the buttons in
		the group use the same spot on the screen, and make the offImage
		LE_DATA_EmptyItem and the onImages the various different states in the
		cycle.	Every time the user presses the button, it shows the pressed image
		then when he releases it, it turns off the current button and turns on
		the next one. */

	unsigned int isNewState : 1;
		/* If this value is TRUE, the next for fields ( starting with isXXX )
		represent a new state for the button. When a button is first created, this
		field is automatically forced to TRUE so it is important that the four
		fields are set appropriately. If you wish to manually change the state
		of a button, you must set this value.
		There are a couple of things to note when it comes to setting the state
		manually for a button. If you want to change the state of a button that
		is active ( not disabled ), you should first get the current state of
		the button using LE_BUTTON_GetState() and place the results inside your
		local copy of the BUTTON_DefinitionRecord used to create the button. Then
		set this field to TRUE, and set the next four fields as desired. You can
		use the macros defined below to examine and set the various fields.
		Finally, call the LE_BUTTON_UpdateButtonList() function to apply the
		changes to the active button ( only after all desired changes are made
		to all buttons in the list ). If the button you are changing state is
		disabled, the settings from the active button will be the same as the
		settings in your local copy so it is not necessary to call the
		LE_BUTTON_GetState() prior to making your changes.
		If this value is FALSE, the next four fields are ignored. */

  unsigned int isOn : 1;
		/* If the button is any other type than BUTTON_TYPE_MOMENTARY, or 
    BUTTON_TYPE_HOTSPOT, this field is used to mark the button as being in the 
    ON state. Only one button in either a group of RADIO or CYCLE buttons can 
    be ON at one time, so if you set more than one to ON, only the first one 
    will allowed to be ON, the others will be forced to the OFF state. If no
    button in a group is set to ON, the last button in the group will be 
    forced to ON. */

  unsigned int isPressed : 1; 
		/* TRUE if this button is currently being pressed. If a pressed image is 
    defined, it will be shown. Setting this field is really only useful to
    simulate the AI pressing a button. */

	unsigned int isVisible : 1;
		/* TRUE if this button is to be displayed on the screen.	FALSE makes the
		button invisible.  It doesn't show on the screen and isn't checked for
		mouse clicks */

	unsigned int isDisabled : 1;
		/* When TRUE, the button is still visible but does not respond to mouse
		actions. The disabledImage is used to display the button in this state if
		a valid image is supplied. Normally this will be a grayed out version
		of the offImage.	*/

	unsigned int activateLMB : 1;
	unsigned int activateMMB : 1;
	unsigned int activateRMB : 1;
		/* Determines what mouse buttons will activate this button. These flags
		activate the left, middle and right mouse buttons respectively. You can
		set any or all of these for any one button. In each case, both a
		UIMSG_BUTTON_PRESSED and a UIMSG_BUTTON_RELEASED is sent. If a callback
    routine is defined, it will be called BEFORE the button changes state 
    and a message is placed on the event queue. */

	unsigned int activateRollOver : 1;
		/* If TRUE, a message will be sent when the mouse first enters the active
		area for this button. A message will also be sent when the mouse first
		leaves the area. If a valid image is provided in rollOverImage, it will 
    be displayed while the mouse remains inside the active area. If a callback
    routine is defined, it will be called both when the mouse first enters
    the active area and when it leaves. */

	unsigned int activateTrack : 1;
		/* If TRUE, mouse tracking will be active when the mouse is over the active
		region of the button. A message will be sent whenever the mouse moves while
		over the button. Be careful, this will generate a lot of messages. The 
    callback function is NOT called if one is defined. */

  unsigned int transparentClicks : 1;
		/* If TRUE then buttons underneath this one can be activated too.  If FALSE
		then this button obscures other ones.  This means that a big roll-over
		button with a smaller non-transparent button on top of it will get
		deactivated (an exit rollover message is sent) while the mouse is over
		the small non-transparent button. */

	unsigned int simulatePress : 1;
		/* If TRUE, the button will simulate being pressed by offsetting its
		current image a few pixels to the right and down. This option really only
		makes sense if you are using TABs or objects to display button states and
		don't want to specify a pressed image. Mind you, this would still work
		with sequences although the effect may not be perfect. */

  unsigned int isNoisy : 1;
		/* If this value is TRUE, this button will make a sound when it is pressed. 
		Sound effects are defined for each button list. The default sound effects
		are empty so you must define them using BUTTON_SetSFX(). You can define a
		sound when the button is pressed, and/or when it is released. */

  RECT rectangle;
		/*	Used to identify where on the screen the mouse has to be in order to
		activate this button.  For animated buttons (offImage is a sequence), the
		rectangle specifies the starting spot for the animation and the bounding
		box for the current frame is the place where the mouse click will work.
		Also used to control text positioning (center text within the rectangle,
		grow rectangle if needed) when drawing text buttons (offImage is a
		Unicode string). */

	LE_DATA_DataId offImage;
		/* This is either a single bitmap (possibly with alpha transparency - 
    clicks don't count on the fully transparent parts), or an animation 
    Sequence, or is a Unicode text string data item (drawn in black on 
    a nice bevelled background, often the same data item is in all images).  
    Bitmaps and strings are positioned on the screen according to rectangle.  
    Sequences use the activation information to set their starting point.  This 
    image is shown when the button is in the unpressed off state. If set to
    LE_DATA_EmptyItem then nothing is shown. */

	LE_DATA_DataId pressedImage;
		/* This is like offImage and is shown when the button is being 
    pressed (mouse held down or keyboard button held down ) It doesn't matter 
    if the button being pressed is on or off, you always get the same pressed 
    image (usually the button image pushed extra deep into the screen). */

	LE_DATA_DataId onImage;
		/* Like offImage except shown when the button is in the ON but 
    unpressed state. */

	LE_DATA_DataId disabledImage;
		/* Used for buttons that are disabled, like offImage. Typically will 
    be a gray version of offImage. For text, it gets drawn in gray rather 
    than black or white. If you don't supply a disabled image, the button
    will still be disabled, but will show the same image as its last state. */

	short imagePriority;
		/* Graphical priority of the button. This is related directly to the
		priorities used by the Animation Engine. You must be careful if you
		have more than one button that uses the same graphic that they have
		different priorities. The Animation Engine uses a combination of the
		graphic identifier and priority to identify an animation. If you don't
		have a different priority, only one of the images will be shown. */

	BOOL transitionDefined;
    /* If TRUE, the information contained in the transition field ( next )
    contains a valid transition. Once the transition is processed, this 
    field will automatically be set to FALSE in both the internal and
    local user records to allieviate the user the bother of doing it. */

	LE_BUTTON_TransitionRecord transition;
    /* Transition to play for this button if the value of transitionDefined
    is TRUE. The transition will be played during the next call to 
    LE_BUTTON_UpdateButtonList() or if it was defined during a callback
    function, it will play as soon as the callback function returns. When you
    are defining your local array of buttons, if you don't want to specify an 
    initial transition, you can use the define LE_BUTTON_BLANK_TRANSITION */

  PFUNC_BUTTONNOTIFY callbackFunction;
    /* Optional callback function that is called whenever this button 
    generates an event. The callback function should be treated like an
    interrupt routine, thus should be very small. You can specify transitions
    for the button that was activated, as well as for other buttons in the
    list which will be activated when the callback function returns. The 
    callback function is called when the button is pressed, released and
    for rollover entry and exit. */

	union
	{
		struct
		{
			unsigned short keyboardCharacter;
				 /* Which Unicode keyboard character will trigger this button on PCs. */

			unsigned char alwaysIntercept;
				/* If TRUE then intercept keys even if there is a chat box or player
				name entry windows dialog box up. If FALSE then only accept keys when
				the game window has the keyboard focus. */

		} PC;

		struct
		{
			unsigned short buttons;
				/* Which buttons on the game console control pad will press this
				virtual button. This is a bitset with a 1 bit for the relevant
				buttons. */

			unsigned char pads;
				/* Identifies which pads will allow input, set to all 1's for all
				control pads. Each bit in the char represent a game pad where the LSB
				is game pad 0. */

			unsigned char comboMode;
				/*	If zero then the buttons field means that any button in the set
				will trigger the virtual button (OR mode).	If 1 then you need to press
				all the buttons marked with 1 bits and not press the other buttons
				(EXACT mode).  If 2 then you need to press all the buttons marked,
				but it doesn't care about the other buttons (AND mode). */

		} gameConsole;

	} physicalButtons;

} LE_BUTTON_DefinitionRecord, *LE_BUTTON_DefinitionPointer;


/*****************************************************************************
 * INTERNAL FUNCTION PROTOTYPES
 *****************************************************************************/

BOOL LI_BUTTON_InitializeSystem( void );
	/* Initialize the button system. This routine must be called ONCE and only
	ONCE before creating any buttons. All global structures ( blech! ) are
	initialized and/or allocated for each button list. A Windoze mutex is
	created for each button list to provide mutual exclusivity. After calling
	this routine, you create buttons using the BUTTON_UpdateButtonList()
	routine. TRUE is returned if initialization was successful. This routine is
  usually called internally by the library initialization code. */


BOOL LI_BUTTON_RemoveSystem( void );
	/* Cleanup and remove the button system. This routine must be called ONCE 
  and only ONCE when buttons are no longer needed. After this call, all button 
  routines will no longer work except BUTTON_Initialize(). TRUE is returned if 
  the button system was cleaned up successfully. This routine is usually called
  internally by the library initialization code. This call will NOT clean up
  any button lists so its a good idea to free all your button lists, using 
  LE_BUTTON_UpdateButtonLists() with 0 as parameter for number of buttons. */


void LI_BUTTON_ProcessInputMessage (LE_UIMSG_MessageType MessageCode, int MouseX, int MouseY, UNS16 KeyCode);
void LI_BUTTON_ProcessWindowsMessage( UINT message, WPARAM wParam, LPARAM lParam );
	/* Processes all button lists based on the given Windows or UIMSG message. The message
	can be either a mouse type message ( WM_LBUTTONDOWN, WM_MOUSEMOVE etc. ) or a
	keyboard message ( WM_KEYDOWN or WM_KEYUP ). Processing begins at the first
	button in the first list and continues through each button in that list. Then
	the next list is processed from the first button to the last and so on
	through all lists. Processing stops for a list as soon as a button is
	activated that does not support transparent clicks. When a button is
	activated, it will generate one or more events and place them on the event
	queue. It is the responsibility of the user to periodically check the queue
	to remove and process events. */


/*****************************************************************************
 * EXTERNAL FUNCTION PROTOTYPES
 *****************************************************************************/

BOOL LE_BUTTON_UpdateButtonList( int buttonList, int numButtons, LE_BUTTON_DefinitionPointer buttonArray );
  /* This routine is used to define, free, or modify a list of buttons. The action
  taken is based on the current state of the button list and the parameters 
  passed. 
		The 'buttonList' parameter identifies the button list to modify. It must be a value 
	between 0 and CE_ARTLIB_NumberOfButtonLists-1. The 'numButtons' parameter is the
	number of buttons defined in the passed array 'buttonArray' which contains a 
	LE_BUTTON_DefinitionRecord for each button.
		If the parameter 'numButtons' is 0, all buttons that are currently in the list
	will be removed from the display and freed from memory. At this point, you can
	define a new list of buttons if so desired. It is a really good idea to use this
  type of call to free all button lists at the end of your program. The value of 
  buttonArray is ignored.
		If the button list is currently empty, the buttons defined in the passed array
	will become the new button list. Memory will be allocated and buttons will be
	created based on the information in each record. Buttons will also be made visible
	if the field 'isVisible' in the record is TRUE.
		If the current button list already has buttons defined, it will be merged with
	the buttons defined in 'buttonArray'. Merging proceeds as follows. If the current
	list has more buttons than the passed array, the extra ones will be removed from
	the display and freed from memory. If the current list has fewer buttons, the extra
	buttons in the array will be treated as new buttons and will have memory allocated
	for them. All other buttons that overlap will take the definition given in 
  buttonArray[] as the new definition for the button.
    All changes will be applied to the buttons immediately. If any transitions are
  defined for any buttons, they will be played. The routine will return TRUE if the 
  button list was updated correctly. */


void LE_BUTTON_GetState( int buttonList, int button, LE_BUTTON_DefinitionPointer butDef );
	/* Get the current state of the button and place the information inside
	the passed pointer to an LE_BUTTON_DefinitionRecord. This will overwrite
	any information that is already inside the record. The pointer to the record
  should be the actual record of the particular button, NOT the pointer to the
  start of the entire button list. It should be noted that all information in
  the button record is replaced, so you should do this call before making any
  modifications. */


/* Defined below are macros that allow you to examine and update the fields 
   in the users local button definition record array. Whenever you want to 
   make any modifications to the array, you can use these. In all cases, the
   parameter 'b' is an instance of LE_BUTTON_DefinitionRecord which is the
   instance of the button from the users local button definition array. */

#define LE_BUTTON_IsPressed(b)						( b.isPressed )
#define LE_BUTTON_IsOn(b) 								( b.isOn )
#define LE_BUTTON_IsVisible(b)						( b.isVisible )
#define LE_BUTTON_IsDisabled(b) 					( b.isDisabled )
  /* Determine the state of a button. Each will return TRUE if the 
  associated attribute is activated. Useful only after a call to
  LE_BUTTON_GetState(). */

#define LE_BUTTON_SetNewState(b,t)				( b.isNewState = t )
#define LE_BUTTON_SetPressed(b,t) 				( b.isPressed = t )
#define LE_BUTTON_SetOn(b,t)							( b.isOn = t )
#define LE_BUTTON_SetVisible(b,t) 				( b.isVisible = t )
#define LE_BUTTON_SetDisabled(b,t)				( b.isDisabled = t )
  /* Used for specifying a new state for a button. Before using these
  macros, you should call LE_BUTTON_GetState() to get the current
  state for the particular button. You then can examine the states
  using the macros defined just above and make any modifications. If
  you make any modifications, make sure you set the new state flag with
  LE_BUTTON_SetNewState(). In all cases, the parameter 't' is a BOOL
  where TRUE is used to set the flag and FALSE will clear it. */
  
#define LE_BUTTON_UseLMB(b,f) 						( b.activateLMB = f )
#define LE_BUTTON_UseMMB(b,f) 						( b.activateMMB = f )
#define LE_BUTTON_UseRMB(b,f) 						( b.activateRMB = f )
#define LE_BUTTON_UseRollOver(b,f)				( b.activateRollOver = f )
#define LE_BUTTON_UseTracking(b,f)				( b.activateTrack = f )
#define LE_BUTTON_UseTransparent(b,f) 		( b.transparentClicks = f )
#define LE_BUTTON_UseSound(b,f) 					( b.isNoisy = f )
  /* Used for defining activation attributes to the button. In all
  cases, the parameter 'f' is a BOOL where TRUE will activate
  the attribute and FALSE will clear it. */

#define LE_BUTTON_SetActiveArea(b,r)			( b.rectangle = r )
  /* Defines the active area for the button. The parameter 'r'
  is an instance of the Windows RECT structure. This will also
  determine the location of the graphic image used to display
  the button. */

#define LE_BUTTON_SetOffImage(b,i)				( b.offImage = i )
#define LE_BUTTON_SetPressedImage(b,i)		( b.pressedImage = i )
#define LE_BUTTON_SetOnImage(b,i) 				( b.onImage = i )
#define LE_BUTTON_SetDisabledImage(b,i) 	( b.disabledImage = i )
  /* Defines the four stable state images for the button. The parameter
  'i' can be a TAB, TCB an object created with LE_GRAFIX_ObjectCreate()
  or a sequence. If it is a sequence, it will be played so it loops. */

#define LE_BUTTON_SetTransDefined(b,t)		( b.transitionDefined = t )

#define LE_BUTTON_SetTransSeqDef(b,t) 		( b.transition.transitionSequenceDefined = t )
#define LE_BUTTON_SetTransSeq(b,s)				( b.transition.transitionSequence = s )
#define LE_BUTTON_SetTransX(b,x)					( b.transition.transitionX = x )
#define LE_BUTTON_SetTransY(b,y)					( b.transition.transitionY = y )

#define LE_BUTTON_SetTransImgDef(b,t) 		( b.transition.nextImageDefined = t )
#define LE_BUTTON_SetTransImg(b,i)				( b.transition.nextImage = i )

#define LE_BUTTON_SetTransAreaDef(b,t)		( b.transition.nextActiveAreaDefined = t )
#define LE_BUTTON_SetTransArea(b,a) 			( b.transition.nextActiveArea = a )

#define LE_BUTTON_SetTransRedraw(b,t) 		( b.transition.redrawButton = t )
#define LE_BUTTON_SetTransHide(b,t) 			( b.transition.hideButton = t )
#define LE_BUTTON_SetTransDisable(b,t)		( b.transition.disableButton = t )
  /* Macros for defining transition features. In all cases, if you
  define any transition features, make sure you activate those changes
  using LE_BUTTON_SetTransDefined(). */

#define LE_BUTTON_SetPCKey(b,k) 					( b.physicalButtons.PC.keyboardCharacter = k )
#define LE_BUTTON_SetPCIntercept(b,i) 		( b.physicalButtons.PC.alwaysIntercept = i )
  /* Macros for setting keyboard activation character on PC version
  of buttons. The parameter 'k' is an unsigned short, which is usually
  the Unicode character that will activate the button. */

#define LE_BUTTON_SetConsoleButtons(b,i)	( b.physicalButtons.gameConsole.buttons = i )
#define LE_BUTTON_SetConsolePads(b,i) 		( b.physicalButtons.gameConsole.pads = i )
#define LE_BUTTON_SetConsoleMode(b,i) 		( b.physicalButtons.gameConsole.comboMode = i )
  /* Macros for setting game console buttons. Not currently supported. */


void LE_BUTTON_SetSFX( int buttonList, LE_DATA_DataId pressedSFX, LE_DATA_DataId releasedSFX );
	/* Modify the sounds played for all buttons contained in this list that
	have the isNoisy flag set to TRUE. The 'pressedSFX' sound will be played
	when a button is pressed, and 'releasedSFX' will be played when the button
	is released. You do not have to specify a sound for both values. Therefore
	if you wanted buttons to play a sound when pressed, but not when released,
	just call this routine with 'releasedSFX' set to LED_EI. When the button
	list is first created, the pressed and released sound effects are defined
	as LED_EI, which means no sound is played. */

void LE_BUTTON_Activate( int buttonList, BOOL active );
  /* Activates an entire button list. When a button list is inactive, none
  of the buttons are processed, regardless of their state. A button list
  remains active or inactive until the next call to activate. When a button
  list is updated or created with LE_BUTTON_UpdateButtonList(), it is
  automatically activated. */

#if CE_ARTLIB_EnableSystemSequencer
void LE_BUTTON_SetRenderSlot( int buttonList, LE_REND_RenderSlot renderSlot );
	/* Sets the render slot for the specified list of buttons. You need to set
	the render slot so button world coordinates get properly interpreted when
	the display scrolls or changes. */
#endif


void LE_BUTTON_SetTextInfo( int buttonList, LE_BUTTON_TextInfoPointer textInfo );
	/* Set the text info structure for the specified button list. Because
	text buttons are created during the LE_BUTTON_UpdateButtonList() routine,
	all existing text buttons will be unaffected by this call. Only new
	buttons created or modified during the next call to update the button
	list will use the new information. It is best therefore to call this
	routine before calling LE_BUTTON_UpdateButtonList(). If you do not supply
  any text info, the default is to have white text for OFF buttons, and
  gray text for DISABLED buttons and no background for the text. */

#endif
