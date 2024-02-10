#pragma once

#include <GLFW/glfw3.h>

// uncomment and modify defines under for customize ImGuiFileDialog

#define USE_IMGUI_TABLES
//#define MAX_FILE_DIALOG_NAME_BUFFER 1024
//#define MAX_PATH_BUFFER_SIZE 1024

#define USE_THUMBNAILS
//the thumbnail generation use the stb_image and stb_resize lib who need to define the implementation
//btw if you already use them in your app, you can have compiler error due to "implemntation found in double"
//so uncomment these line for prevent the creation of implementation of these libs again
#define DONT_DEFINE_AGAIN__STB_IMAGE_IMPLEMENTATION
#define DONT_DEFINE_AGAIN__STB_IMAGE_RESIZE_IMPLEMENTATION
//#define IMGUI_RADIO_BUTTON RadioButton
//#define DisplayMode_ThumbailsList_ImageHeight 32.0f
#define DisplayMode_FilesList_ButtonString "FL"
#define DisplayMode_FilesList_ButtonHelp "File List"
#define DisplayMode_ThumbailsList_ButtonString "TL"
#define DisplayMode_ThumbailsList_ButtonHelp "Thumbnails List"
#define DisplayMode_ThumbailsSmall_ButtonString "ST"
#define DisplayMode_ThumbailsSmall_ButtonHelp "Small Thumbnails"
#define DisplayMode_ThumbailsBig_ButtonString "BT"
#define DisplayMode_ThumbailsBig_ButtonHelp "Big Thumbnails"

#define USE_EXPLORATION_BY_KEYS
// Up key for explore to the top
#define IGFD_KEY_UP GLFW_KEY_UP
// Down key for explore to the bottom
#define IGFD_KEY_DOWN GLFW_KEY_DOWN
// Enter key for open directory
#define IGFD_KEY_ENTER GLFW_KEY_ENTER
// BackSpace for comming back to the last directory
#define IGFD_KEY_BACKSPACE GLFW_KEY_BACKSPACE

// by ex you can quit the dialog by pressing the key excape
#define USE_DIALOG_EXIT_WITH_KEY
#define IGFD_EXIT_KEY GLFW_KEY_ESCAPE

// widget
// filter combobox width
//#define FILTER_COMBO_WIDTH 120.0f
// button widget use for compose path
//#define IMGUI_PATH_BUTTON ImGui::Button
// standar button
//#define IMGUI_BUTTON ImGui::Button

// locales string
//~ #define createDirButtonString ICON_IGFD_ADD
//~ #define okButtonString ICON_IGFD_OK " Select"
//~ #define cancelButtonString ICON_IGFD_CANCEL " Cancel"
//~ #define resetButtonString ICON_IGFD_RESET
//~ #define drivesButtonString ICON_IGFD_DRIVES
//~ #define editPathButtonString ICON_IGFD_EDIT
//~ #define searchString ICON_IGFD_SEARCH
//~ #define dirEntryString ICON_IGFD_FOLDER
//~ #define linkEntryString ICON_IGFD_LINK
//~ #define fileEntryString ICON_IGFD_FILE
//#define buttonResetSearchString "Reset search"
//#define buttonDriveString "Drives"
//#define buttonEditPathString "Edit path\nYou can also right click on path buttons"
//#define buttonResetPathString "Reset to current directory"
//#define buttonCreateDirString "Create Directory"
//~ #define OverWriteDialogTitleString "A file with this name already exists!"
//~ #define OverWriteDialogMessageString "Overwrite it?"
//~ #define OverWriteDialogConfirmButtonString ICON_IGFD_OK " Confirm"
//~ #define OverWriteDialogCancelButtonString ICON_IGFD_CANCEL " Cancel"

// see strftime functionin <ctime> for customize
// "%Y/%m/%d %H:%M:%S" give 2021:01:22 11:47:10
// "%Y/%m/%d %i:%M%p" give 2021:01:22 11:45PM
//#define DateTimeFormat "%Y/%m/%d %i:%M%p"

// theses icons will appear in table headers
//~ #define USE_CUSTOM_SORTING_ICON
//~ #define tableHeaderAscendingIcon ICON_IGFD_CHEVRON_UP
//~ #define tableHeaderDescendingIcon ICON_IGFD_CHEVRON_DOWN
//#define tableHeaderFileNameString " File name"
//#define tableHeaderFileTypeString " Type"
//#define tableHeaderFileSizeString " Size"
//#define tableHeaderFileDateString " Date"

//#define USE_BOOKMARK
//#define bookmarkPaneWith 150.0f
//#define IMGUI_TOGGLE_BUTTON ToggleButton
//~ #define bookmarksButtonString ICON_IGFD_BOOKMARK
//~ //#define bookmarksButtonHelpString "bookmark"
//~ #define addBookmarkButtonString ICON_IGFD_ADD
//~ #define removeBookmarkButtonString ICON_IGFD_REMOVE
