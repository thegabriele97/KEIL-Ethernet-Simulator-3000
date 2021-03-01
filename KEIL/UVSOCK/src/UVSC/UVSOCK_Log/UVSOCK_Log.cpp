/******************************************************************************/
/* UVSOCK  -  UVSOCK interface                                                */
/******************************************************************************/
/* Name:    UVSOCK_Log.cpp                                                      */
/* Purpose: A class for logging UVSC messages                                 */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2004-2016 KEIL - An ARM Company. All rights reserved.        */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/


#include "UVSOCK_Log.h"

#include <stdio.h>
#include <time.h>



//---Command codes:
//--- General functions:
#define UV_NULL_CMD_STR                     "NULL_CMD             "     // not a command
#define UV_GEN_GET_VERSION_STR              "GEN_GET_VERSION      "     // get socket-interface Version number
#define UV_GEN_UI_UNLOCK_STR                "GEN_UI_UNLOCK        "     // Prevent message boxes from popping up in uVision
#define UV_GEN_UI_LOCK_STR                  "GEN_UI_LOCK          "     // Allow message boxes to pop up in uVision
#define UV_GEN_HIDE_STR                     "GEN_HIDE             "     // Completely hide the uVision window
#define UV_GEN_SHOW_STR                     "GEN_SHOW             "     // Show the uVision window
#define UV_GEN_RESTORE_STR                  "GEN_RESTORE          "     // Restore the uVision window
#define UV_GEN_MINIMIZE_STR                 "GEN_MINIMIZE         "     // Minimise the uVision window
#define UV_GEN_MAXIMIZE_STR                 "GEN_MAXIMIZE         "     // Maximise the uVision window
#define UV_GEN_EXIT_STR                     "GEN_EXIT             "     // Close uVision
#define UV_GEN_GET_EXTVERSION_STR           "GEN_GET_EXTVERSION   "     // Get extended version info
#define UV_GEN_CHECK_LICENSE_STR            "GEN_CHECK_LICENSE    "     // Check toolchain licensing
#define UV_GEN_CPLX_COMPLETE_STR            "GEN_CPLX_COMPLETE    "     // Complex command has completed
#define UV_GEN_SET_OPTIONS_STR              "GEN_SET_OPTIONS      "     // [TdB: 14.04.2014] Sets UVSOCK options

//--- Project functions:
#define UV_PRJ_LOAD_STR                     "PRJ_LOAD             "     // Load Project  /TBD/
#define UV_PRJ_CLOSE_STR                    "PRJ_CLOSE            "     // Load Project  /TBD/
#define UV_PRJ_ADD_GROUP_STR                "PRJ_ADD_GROUP        "     // Add a Group to current Project
#define UV_PRJ_DEL_GROUP_STR                "PRJ_DEL_GROUP        "     // Remove a Group from current Project
#define UV_PRJ_SET_TARGET_STR               "PRJ_SET_TARGET       "     // Add a Group to current Project
#define UV_PRJ_ADD_FILE_STR                 "PRJ_ADD_FILE         "     // Add one or more files to group
#define UV_PRJ_DEL_FILE_STR                 "PRJ_DEL_FILE         "     // Remove one or more files from group
#define UV_PRJ_BUILD_STR                    "PRJ_BUILD            "     // Build the current Project
#define UV_PRJ_REBUILD_STR                  "PRJ_REBUILD          "     // ReBuild the current Project
#define UV_PRJ_CLEAN_STR                    "PRJ_CLEAN            "     // Clean current Project
#define UV_PRJ_BUILD_CANCEL_STR             "PRJ_BUILD_CANCEL     "     // Stop the Build/ReBuild
#define UV_PRJ_FLASH_DOWNLOAD_STR           "PRJ_FLASH_DOWNLOAD   "     // Download library to flash
#define UV_PRJ_GET_DEBUG_TARGET_STR         "PRJ_GET_DEBUG_TARGET "     // set the debug target
#define UV_PRJ_SET_DEBUG_TARGET_STR         "PRJ_SET_DEBUG_TARGET "     // get the debug target
#define UV_PRJ_GET_OPTITEM_STR              "PRJ_GET_OPTITEM      "     // read Prj-option item
#define UV_PRJ_SET_OPTITEM_STR              "PRJ_SET_OPTITEM      "     // write Prj-option item
#define UV_PRJ_ENUM_GROUPS_STR              "PRJ_ENUM_GROUPS      "     // enumerate groups
#define UV_PRJ_ENUM_FILES_STR               "PRJ_ENUM_FILES       "     // enumerate files
#define UV_PRJ_CMD_PROGRESS_STR             "PRJ_CMD_PROGRESS     "     // Progress-Bar commands
#define UV_PRJ_ACTIVE_FILES_STR             "PRJ_ACTIVE_FILES     "     // get number of active files
#define UV_PRJ_FLASH_ERASE_STR              "PRJ_FLASH_ERASE      "     // Erase flash device
#define UV_PRJ_GET_OUTPUTNAME_STR           "PRJ_GET_OUTPUTNAME   "     // Get the executable/library output object name for the current project
#define UV_PRJ_ENUM_TARGETS_STR             "PRJ_ENUM_TARGETS      "    // enumerate groups
#define UV_PRJ_SET_TARGET_STR               "PRJ_SET_TARGET       "     // Set a target active in current Project
#define UV_PRJ_GET_CUR_TARGET_STR           "PRJ_GET_CUR_TARGET   "     // Get the current active target name for the current project
#define UV_PRJ_SET_OUTPUTNAME_STR           "PRJ_SET_OUTPUTNAME   "     // Set the executable/library output object name for the current project or specified project

//--- Debug functions:
#define UV_DBG_ENTER_STR                    "DBG_ENTER            "     // start debugger
#define UV_DBG_EXIT_STR                     "DBG_EXIT             "     // stop debugger
#define UV_DBG_START_EXECUTION_STR          "DBG_START_EXECUTION  "     // start execution
#define UV_DBG_RUN_TO_ADDRESS_STR           "DBG_RUN_TO_ADDRESS   "     // [TdB: 14.04.2014] Start target execution and run to specified address
#define UV_DBG_STOP_EXECUTION_STR           "DBG_STOP_EXECUTION   "     // stop execution
#define UV_DBG_STATUS_STR                   "DBG_STATUS           "     // is simulation running
#define UV_DBG_RESET_STR                    "DBG_RESET            "     // reset debugger
#define UV_DBG_STEP_HLL_STR                 "DBG_STEP_HLL         "     // Line-Step (HLL-Step)
#define UV_DBG_STEP_HLL_N_STR               "UV_DBG_STEP_HLL_N    "     // [TdB: 14.04.2014] Step N lines over HLL code(High Level Language code, eg C)
#define UV_DBG_STEP_INTO_STR                "DBG_STEP_INTO        "     // one HLL or Asm Step
#define UV_DBG_STEP_INTO_N_STR              "UV_DBG_STEP_INTO_N   "     // [TdB: 14.04.2014] Perform N steps into HLL code(High Level Language code, eg C)
#define UV_DBG_STEP_INSTRUCTION_STR         "DBG_STEP_INSTRUCTION "     // Step one Asm-Instruction
#define UV_DBG_STEP_INSTRUCTION_N_STR       "UV_DBG_STEP_INSTRUCTION_N" // [TdB: 14.04.2014] Step N ASM Instruction
#define UV_DBG_STEP_OUT_STR                 "DBG_STEP_OUT         "     // Step out of current function
#define UV_DBG_CALC_EXPRESSION_STR          "DBG_CALC_EXPRESSION  "     // calculate an expression
#define UV_DBG_MEM_READ_STR                 "DBG_MEM_READ         "     // Read a chunk of Memory
#define UV_DBG_MEM_WRITE_STR                "DBG_MEM_WRITE        "     // Write a chunk of Memory
#define UV_DBG_TIME_INFO_STR                "DBG_TIME_INFO        "     // get cycles and time-stamp
#define UV_DBG_SET_CALLBACK_STR             "DBG_SET_CALLBACK     "     // set time-interval
#define UV_DBG_VTR_GET_STR                  "DBG_VTR_GET          "     // read vtr-value
#define UV_DBG_VTR_SET_STR                  "DBG_VTR_SET          "     // write vtr-value
#define UV_DBG_SERIAL_GET_STR               "DBG_SERIAL_GET       "     // client wants serial output
#define UV_DBG_SERIAL_PUT_STR               "DBG_SERIAL_PUT       "     // App receives one or more BYTEs
#define UV_DBG_VERIFY_CODE_STR              "DBG_VERIFY_CODE      "     // Verify code in flash against built executable
#define UV_DBG_CREATE_BP_STR                "DBG_CREATE_BP        "     // create a breakpoint
#define UV_DBG_ENUMERATE_BP_STR             "DBG_ENUMERATE_BP     "     // client want's enumerated breakpoints
#define UV_DBG_CHANGE_BP_STR                "DBG_CHANGE_BP        "     // Bp changed (killed,enabled,disabled)
#define UV_DBG_ENUM_SYMTP_STR               "DBG_ENUM_SYMTP       "     // enumerate members
#define UV_DBG_ADR_TOFILELINE_STR           "DBG_ADR_TOFILELINE   "     // Map an address to code file & linenumber
#define UV_DBG_ENUM_STACK_STR               "DBG_ENUM_STACK       "     // Enumerate the call stack
#define UV_DBG_ENUM_VTR_STR                 "DBG_ENUM_VTR         "     // Enumerate all virtual registers (VTRs)
#define UV_DBG_UNUSED_STR                   "DBG_UNUSED           "     // Unused
#define UV_DBG_ADR_SHOWCODE_STR             "DBG_ADR_SHOWCODE     "     // Show disassembly and/or HLL (High Level Language) file for an address
#define UV_DBG_WAKE_STR                     "DBG_WAKE             "     // Set sleep callback and/or wake up simulation
#define UV_DBG_SLEEP_STR                    "DBG_SLEEP            "     // Sleep the simulation 
#define UV_MSGBOX_MSG_STR                   "UV_MSGBOX_MSG        "     // Notification from a message box
#define UV_DBG_EXEC_CMD_STR                 "DBG_EXEC_CMD         "     // Execute a command (as if via the command line)
#define UV_DBG_POWERSCALE_SHOWCODE_STR      "POWERSCALE_SHOWCODE  "     // Power Scale Show Code
#define UV_DBG_POWERSCALE_SHOWPOWER_STR     "UV_DBG_POWERSCALE_SHOWPOWER" // Show Power (UV --> PS)
#define POWERSCALE_OPEN_STR                 "POWERSCALE_OPEN      "     // [TdB: 14.04.2014] Register PowerScale device
#define UV_DBG_EVAL_EXPRESSION_TO_STR_STR   "DBG_EVAL_EXPRESSION_TO_STR  "   // evaluate an expression to string
#define UV_DBG_FILELINE_TO_ADR_STR          "DBG_FILELINE_TO_ADR  "    // Map file & linenumber to an address
#define UV_DBG_ITM_REGISTER_STR             "UV_DBG_ITM_REGISTER"       // Register an ITM Channel
#define UV_DBG_ITM_UNREGISTER_STR           "UV_DBG_ITM_UNREGISTER"     // Unregister an ITM Channel
#define UV_DBG_EVTR_REGISTER_STR            "UV_DBG_EVTR_REGISTER"      // Register for Event Recorder events
#define UV_DBG_EVTR_UNREGISTER_STR          "UV_DBG_EVTR_UNREGISTER"    // Unregister for Event Recorder events
#define UV_DBG_EVTR_ENUMSCVDFILES_STR       "UV_DBG_EVTR_ENUMSCVDFILES" // Enumerate Event Recorder related SCVD files

//---Registers:
#define UV_DBG_ENUM_REGISTER_GROUPS_STR     "UV_DBG_ENUM_REGISTER_GROUPS"    // [TdB: 14.04.2014] Enumerate register groups 
#define UV_DBG_ENUM_REGISTERS_STR           "UV_DBG_ENUM_REGISTERS"          // [TdB: 14.04.2014] Evaluate registers 
#define UV_DBG_READ_REGISTERS_STR           "UV_DBG_READ_REGISTERS"          // [TdB: 14.04.2014] Get register values
#define UV_DBG_REGISTER_SET_STR             "UV_DBG_REGISTER_SET  "          // [TdB: 14.04.2014] Set register value
#define UV_DBG_DSM_READ_STR                 "UV_DBG_DSM_READ      "          // [TdB: 14.04.2014] Read disassembly block
#define UV_DBG_EVAL_WATCH_EXPRESSION_STR    "UV_DBG_EVAL_WATCH_EXPRESSION"   // [TdB: 14.04.2014] Add watch expression / evaluate existing
#define UV_DBG_REMOVE_WATCH_EXPRESSION_STR  "UV_DBG_REMOVE_WATCH_EXPRESSION" // [TdB: 14.04.2014] Remove watch expression 
#define UV_DBG_ENUM_VARIABLES_STR           "UV_DBG_ENUM_VARIABLES"          // [TdB: 14.04.2014] Get variables for given stack frame, globals or struct/array members of a variable/watch
#define UV_DBG_VARIABLE_SET_STR             "UV_DBG_VARIABLE_SET  "          // [TdB: 14.04.2014] Set variable value
#define UV_DBG_ENUM_TASKS_STR               "UV_DBG_ENUM_TASKS    "          // [TdB: 14.04.2014] Enumerate task list - in non - Rtx case returns main thread
#define UV_DBG_ENUM_MENUS_STR               "UV_DBG_ENUM_MENUS    "          // [TdB: 14.04.2014] Enumerate available dynaic menus and peripheral dialogs 
#define UV_DBG_MENU_EXEC_STR                "UV_DBG_MENU_EXEC     "          // [TdB: 14.04.2014] Execute menu entry (for example to show view or or peripheral dialog) 

//---Answers/Error from Server to Client:
#define UV_CMD_RESPONSE_STR                 "CMD_RESPONSE         "   // Response to a command from the client

//---Asynchronous messages:
#define UV_ASYNC_MSG_STR                    "ASYNC_MSG            "   // Response to a command from the client

//--- Special Asynchronous messages:
// Project functions:
#define UV_PRJ_BUILD_COMPLETE_STR           "PRJ_BUILD_COMPLETE   "   // A build has completed
#define UV_PRJ_BUILD_OUTPUT_STR             "PRJ_BUILD_OUTPUT     "   // Send Build output to client

//--- Debug functions:
#define UV_DBG_CALLBACK_STR                 "DBG_CALLBACK         "   // The callback timeout has expired

//--- Response to UV_DBG_ENUMERATE_BP:      
#define UV_DBG_BP_ENUM_START_STR            "DBG_BP_ENUM_START    "   // Start of enumeration (no breakpoint info)
#define UV_DBG_BP_ENUMERATED_STR            "DBG_BP_ENUMERATED    "   // zero, one or more Response(s) with breakpoint info (BKRSP)
#define UV_DBG_BP_ENUM_END_STR              "DBG_BP_ENUM_END      "   // End of enumeration (no breakpoint info)

//--- Response to UV_PRJ_ENUM_GROUPS:
#define UV_PRJ_ENUM_GROUPS_START_STR        "PRJ_ENUM_GROUPS_START"   // Start of group enumeration
#define UV_PRJ_ENUM_GROUPS_ENU_STR          "PRJ_ENUM_GROUPS_ENU  "   // zero, one or more Responses with group name (SSTR)
#define UV_PRJ_ENUM_GROUPS_END_STR          "PRJ_ENUM_GROUPS_END  "   // End of group enumeration

//--- Response to UV_PRJ_ENUM_FILES:
#define UV_PRJ_ENUM_FILES_START_STR         "PRJ_ENUM_FILES_START "   // Start of files enumeration
#define UV_PRJ_ENUM_FILES_ENU_STR           "PRJ_ENUM_FILES_ENU   "   // zero, one or more Response(s) with file name (SSTR)
#define UV_PRJ_ENUM_FILES_END_STR           "PRJ_ENUM_FILES_END   "   // End of files enumeration

//--- Response to UV_DBG_EVTR_ENUMSCVDFILES:
#define UV_DBG_EVTR_ENUMSCVDFILES_START_STR  "DBG_EVTR_ENUMSCVDFILES_START"  // Start of group enumeration
#define UV_DBG_EVTR_ENUMSCVDFILES_ENU_STR    "DBG_EVTR_ENUMSCVDFILES_ENU  "  // zero, one or more Responses with group name (SSTR)
#define UV_DBG_EVTR_ENUMSCVDFILES_END_STR    "DBG_EVTR_ENUMSCVDFILES_END  "  // End of group enumeration

//--- Progress bar functions
#define UV_PRJ_PBAR_INIT_STR                "PRJ_PBAR_INIT        "   // Initialise the progress bar
#define UV_PRJ_PBAR_STOP_STR                "PRJ_PBAR_STOP        "   // Stop the progress bar
#define UV_PRJ_PBAR_SET_STR                 "PRJ_PBAR_SET         "   // Set the position of the progress bar
#define UV_PRJ_PBAR_TEXT_STR                "PRJ_PBAR_TEXT        "   // Set the text of the progress bar

//--- Response to UV_DBG_ENUM_SYMTP:
#define UV_DBG_ENUM_SYMTP_START_STR         "DBG_ENUM_SYMTP_START "   // Start of group enumeration
#define UV_DBG_ENUM_SYMTP_ENU_STR           "DBG_ENUM_SYMTP_ENU   "   // zero, one or more Responses with group name (SSTR)
#define UV_DBG_ENUM_SYMTP_END_STR           "DBG_ENUM_SYMTP_END   "   // End of group enumeration

//--- Response to UV_DBG_ENUM_STACK:
#define UV_DBG_ENUM_STACK_START_STR         "DBG_ENUM_STACK_START "   // Start of stack enumeration
#define UV_DBG_ENUM_STACK_ENU_STR           "DBG_ENUM_STACK_ENU   "   // Stack enumeration, one or more Response(s) of structure stackdat
#define UV_DBG_ENUM_STACK_END_STR           "DBG_ENUM_STACK_END   "   // End of stack enumeration

//--- Response to UV_DBG_ENUM_VTR:
#define UV_DBG_ENUM_VTR_START_STR           "DBG_ENUM_VTR_START   "   // Start of vtr enumeration
#define UV_DBG_ENUM_VTR_ENU_STR             "DBG_ENUM_VTR_ENU     "   // Vtr enumeration, one or more Response(s) of structure iAVTR
#define UV_DBG_ENUM_VTR_END_STR             "DBG_ENUM_VTR_END     "   // End of vtr enumeration

//--- Command Output
#define UV_DBG_CMD_OUTPUT_STR               "DBG_CMD_OUTPUT       "   // Send Command output to client
#define UV_DBG_SERIAL_OUTPUT_STR            "UV_DBG_SERIAL_OUTPUT "   // [TdB: 14.04.2014] Notification of a serial output (debug or UART 1, 2 or 3)
#define UV_DBG_ITM_OUTPUT_STR               "UV_DBG_ITM_OUTPUT    "   // [TdB: 24.05.2016] Notification of a serial output (debug or UART 1, 2 or 3)
#define UV_DBG_EVTR_OUTPUT_STR              "UV_DBG_EVTR_OUTPUT   "
//--- Response to UV_PRJ_ENUM_TARGETS:      
#define UV_PRJ_ENUM_TARGETS_START_STR       "PRJ_ENUM_TARGETS_START"  // Start of target enumeration
#define UV_PRJ_ENUM_TARGETS_ENU_STR         "PRJ_ENUM_TARGETS_ENU  "  // zero, one or more Responses with target name (SSTR)
#define UV_PRJ_ENUM_TARGETS_END_STR         "PRJ_ENUM_TARGETS_END  "  // End of target enumeration

//--- Response to UV_DBG_ENUM_REGISTER_GROUPS:
#define UV_DBG_ENUM_REGISTER_GROUPS_START_STR   "UV_DBG_ENUM_REGISTER_GROUPS_START"   // [TdB: 14.04.2014] Start of register group enumeration
#define UV_DBG_ENUM_REGISTER_GROUPS_ENU_STR     "UV_DBG_ENUM_REGISTER_GROUPS_ENU"     // [TdB: 14.04.2014] register group enumeration; one or more Response(s) of structure
#define UV_DBG_ENUM_REGISTER_GROUPS_END_STR     "UV_DBG_ENUM_REGISTER_GROUPS_END"     // [TdB: 14.04.2014] End of register group enumeration

//--- Response to UV_DBG_ENUM_REGISTERS:
#define UV_DBG_ENUM_REGISTERS_START_STR         "UV_DBG_ENUM_REGISTERS_START"         // [TdB: 14.04.2014] Start of register enumeration
#define UV_DBG_ENUM_REGISTERS_ENU_STR           "UV_DBG_ENUM_REGISTERS_ENU"           // [TdB: 14.04.2014] register enumeration; one or more Response(s) of structure
#define UV_DBG_ENUM_REGISTERS_END_STR           "UV_DBG_ENUM_REGISTERS_END"           // [TdB: 14.04.2014] End of register enumeration

//--- Response to UV_DBG_ENUM_VARIABLES:
#define UV_DBG_ENUM_VARIABLES_START_STR         "UV_DBG_ENUM_VARIABLES_START"         // [TdB: 14.04.2014] Start of variable enumeration
#define UV_DBG_ENUM_VARIABLES_ENU_STR           "UV_DBG_ENUM_VARIABLES_ENU"           // [TdB: 14.04.2014] variable enumeration; one or more Response(s) of structure
#define UV_DBG_ENUM_VARIABLES_END_STR           "UV_DBG_ENUM_VARIABLES_END"           // [TdB: 14.04.2014] End of variable enumeration

//--- Response to UV_DBG_ENUM_TASKS:
#define UV_DBG_ENUM_TASKS_START_STR             "UV_DBG_ENUM_TASKS_START"             // [TdB: 14.04.2014] Start of task list enumeration
#define UV_DBG_ENUM_TASKS_ENU_STR               "UV_DBG_ENUM_TASKS_ENU"               // [TdB: 14.04.2014] Task list enumeration; one or more Response(s) with stack frame information
#define UV_DBG_ENUM_TASKS_END_STR               "UV_DBG_ENUM_TASKS_END"               // [TdB: 14.04.2014] End of task list  enumeration

//--- Response to UV_DBG_ENUM_MENUS:
#define UV_DBG_ENUM_MENUS_START_STR             "UV_DBG_ENUM_MENUS_START"             // [TdB: 14.04.2014] Start of available view enumeration
#define UV_DBG_ENUM_MENUS_ENU_STR               "UV_DBG_ENUM_MENUS_ENU"               // [TdB: 14.04.2014] View list enumeration; one or more Response(s) with view item information
#define UV_DBG_ENUM_MENUS_END_STR               "UV_DBG_ENUM_MENUS_END"               // [TdB: 14.04.2014] End of task list  enumeration

//--- Real-Time Agent:
#define UV_RTA_MESSAGE_STR                  "RTA_MESSAGE          "   // send a RTAgent-message
#define UV_RTA_INCOMPATIBLE_STR             "RTA_INCOMPATIBLE     "   // sent when the RTA version in target is incompatible with uVision

//--- Unknown command code:
#define UV_UNKNOWN_COMMAND_STR              "UNKNOWN_COMMAND ???  "  

//--- Test definititions (for testing only):
#define UV_TST_1_STR                        "TST_1                "
#define UV_TST_2_STR                        "TST_2                "
#define UV_TST_3_STR                        "TST_3                "
#define UV_TST_4_STR                        "TST_4                "
#define UV_TST_5_STR                        "TST_5                "
#define UV_TST_6_STR                        "TST_6                "
#define UV_TST_7_STR                        "TST_7                "
#define UV_TST_8_STR                        "TST_8                "
#define UV_TST_9_STR                        "TST_9                "
#define UV_TST_10_STR                       "TST_10               "

// Empty string definition
static const char szEmptyString = 0;
#define UVSC_EMPTY_STRING   &szEmptyString

static const char szUVSC[][50] = {
  "UVSC_STATUS_SUCCESS         ",   ///< Success
  "UVSC_STATUS_FAILED          ",   ///< General failure
  "UVSC_STATUS_NOT_SUPPORTED   ",   ///< Request for an unsupported operation
  "UVSC_STATUS_NOT_INIT        ",   ///< DLL not initialised
  "UVSC_STATUS_TIMEOUT         ",   ///< Operation timed-out
  "UVSC_STATUS_INVALID_CONTEXT ",   ///< Function called from an invalid context (most likely the callback function)
  "UVSC_STATUS_INVALID_PARAM   ",   ///< Function called with one or more invalid parameters
  "UVSC_STATUS_BUFFER_TOO_SMALL",   ///< Function called with a buffer that was not big enough to hold the result from uVision
  "UVSC_STATUS_CALLBACK_IN_USE ",   ///< Function cannot be used when the callback is in use
  "UVSC_STATUS_COMMAND_ERROR   ",   ///< The command failed - call #UVSC_GetLastError to get more information on how the command failed
  "UVSC_STATUS_???             "    ///< Invalid status!
};

static const char szRt[][50] =  {
  "OK                        ",   ///< (0)  Command was executed successfully
  "UV_STATUS_FAILED          ", 	///< (1)  Command failed (generic error status)
  "UV_STATUS_NO_PROJECT      ",   ///< (2)  No project is loaded
  "UV_STATUS_WRITE_PROTECTED ",   ///< (3)  The current project is write protected
  "UV_STATUS_NO_TARGET       ",   ///< (4)  No target is selected
  "UV_STATUS_NO_TOOLSET      ",   ///< (5)  Toolset for current target is undefined
  "UV_STATUS_NOT_DEBUGGING   ",   ///< (6)  The target is blocked
  "UV_STATUS_ALREADY_PRESENT ",   ///< (7)  The group / file is already present
  "UV_STATUS_INVALID_NAME    ",   ///< (8)  Invalid group / file / project name
  "UV_STATUS_NOT_FOUND       ",   ///< (9)  File / group not found
  "UV_STATUS_DEBUGGING       ",   ///< (10) Operation not possible in this mode
  "UV_STATUS_TARGET_EXECUTING",   ///< (11) Operation not possible when target is executing
  "UV_STATUS_TARGET_STOPPED  ",   ///< (12) Operation not possible when target is stopped
  "UV_STATUS_PARSE_ERROR     ",   ///< (13) Error parsing data in request
  "UV_STATUS_OUT_OF_RANGE    ",   ///< (14) Requested data out of range

  "UV_STATUS_BP_CANCELLED    ",   ///< (15) Create Breakpoint has been cancelled
  "UV_STATUS_BP_BADADDRESS   ",   ///< (16) Create Breakpoint: invalid address
  "UV_STATUS_BP_NOTSUPPORTED ",   ///< (17) Type of Breakpoint is not supported
  "UV_STATUS_BP_FAILED       ",   ///< (18) Create Breakpoint failed
  "UV_STATUS_BP_REDEFINED    ",   ///< (19) Async: a bp has been redefined
  "UV_STATUS_BP_DISABLED     ",   ///< (20) Async: a bp has been disabled
  "UV_STATUS_BP_ENABLED      ",   ///< (21) Async: a bp has been enabled
  "UV_STATUS_BP_CREATED      ",   ///< (22) Async: a bp has been created
  "UV_STATUS_BP_DELETED      ",   ///< (23) Async: a bp has been deleted
  "UV_STATUS_BP_NOTFOUND     ",   ///< (24) ena/dis/kill breakpoint not found (nTickMark)

  "UV_STATUS_BUILD_OK_WARNING",   ///< (25) A build was successful, but with warnings
  "UV_STATUS_BUILD_FAILED    ",   ///< (26) A build failed with errors
  "UV_STATUS_BUILD_CANCELLED ",   ///< (27) A build was cancelled

  "UV_STATUS_NOT_SUPPORTED   ",   ///< (28) Request for an unsupported operation
  "UV_STATUS_TIMEOUT         ",   ///< (29) Request timed-out
  "UV_STATUS_UNEXPECTED_MSG  ",   ///< (30) An unexpected message type was returned

  "UV_STATUS_VERIFY_FAILED   ",   ///< (31) Verifying the downloaded code failed

  "UV_STATUS_NO_ADRMAP       ",   ///< (32) Map address to file/line failed
  "UV_STATUS_INFO            ",   ///< (33) This is an information only message

  "UV_STATUS_NO_MEM_ACCESS   ",   ///< (34) Operation failed: Memory access is blocked (most likely target does not support memory access while running)
  "UV_STATUS_FLASH_DOWNLOAD  ",   ///< (35) Operation failed: The target is downloading FLASH, this operation is not possible when FLASH is downloading
  "UV_STATUS_BUILDING        ",   ///< (36) Operation failed: A build is in progress, this operation is not possible when build is in progress
  "UV_STATUS_HARDWARE        ",   ///< (37) Operation failed: The debugger is debugging hardware, this operation is not possible when debugging a hardware target
  "UV_STATUS_SIMULATOR       ",   ///< (38) Operation failed: The debugger is debugging a simulation, this operation not possible when debugging a simulated target

  "UV_STATUS_BUFFER_TOO_SMALL",   ///< (39) Operation failed: Return buffer was too small (UVSOCK Client DLL only)

  "UV_STATUS_???             "    ///< Invalid status!
};

const char szBpt[][12]  =  {              // Breakpoint-Types
  "<unknown>",
  "Exec",                                 // Execution-break
  "Read",                                 // Read-Access-break
  "Write",                                // Write-Access-break
  "ReadWrite",                            // ReadWrite-Access-break
  "Complex",                              // re-evalute expr. after each step
  "???"                                   // invalid bptype!
};

static char szOjob[][20] =  {
  "",
  "OPT_LMISC",           // get Link-Misc string, valid for Target only
  "OPT_CMISC",           // get C-Misc string
  "OPT_AMISC",           // get Asm-Misc string
  "OPT_CINCL",           // get C-Include-path string
  "OPT_AINCL",           // get Asm-Include-path string
  "OPT_CDEF",            // get C-Defines string
  "OPT_ADEF",            // get Asm-Defines string
  "OPT_CUNDEF",          // get C-Undefines, valid for Group- and File-level only
  "/OPT_AUNDEF/",        // for completeness: Asm-Undefines, non-existant in uVision
  "OPT_COPTIMIZE",       // get C-Optimize [Level, Time]
  "OPT_CODEGEN",         // get/set ARM/Thumb mode for ARM7TDMI targets
  "OPT_MEMRANGES",       // get memory ranges
  "OPT_ASNMEMRANGES",    // get/set assigned ranges
  "???"                  // invalid mem range
};


static char szMType[][6] =  {
  "EMPTY", "ROM", "RAM", "???"
};

static char szBkChgType[][10] =  {
  "", "KILL", "ENABLE", "DISABLE", "???"
};

static char szStopReason[][12] = {
  "UNDEFINED", ///< (0x0000) Unknown / undefined stop reason
  "EXEC",      ///< (0x0001) Hit execution breakpoint
  "READ",      ///< (0x0002) Hit read access breakpoint
  "HIT_WRITE", ///< (0x0004) Hit write access breakpoint
  "HIT_COND",  ///< (0x0008) Hit conditional breakpoint
  "HIT_ESC",   ///< (0x0010) ESCape key has been pressed
  "HIT_VIOLA", ///< (0x0020) Memory access violation occurred (simulator only)
  "TIME_OVER", ///< (0x0040) Interval time set by #UV_DBG_SET_CALLBACK or #UV_DBG_WAKE elapsed
  "UNDEFINS",  ///< (0x0080) Undefined instruction occurred
  "PABT",      ///< (0x0100) (Instruction) prefetch abort occurred
  "DABT",      ///< (0x0200) Data abort occurred
  "NONALIGN",  ///< (0x0400) Non-aligned access occurred (simulator only)
};

static char szVTTType[][12] = {
  "VTT_void",     // 0   //
  "VTT_bit",      // 1   // val.ul & 1
  "VTT_char",     // 2   // val.sc
  "VTT_uchar",    // 3   // val.uc
  "VTT_int",      // 4   // val.i
  "VTT_uint",     // 5   // val.ul
  "VTT_short",    // 6   // val.i16
  "VTT_ushort",   // 7   // val.u16
  "VTT_long",     // 8   // val.l
  "VTT_ulong",    // 9   // val.ul
  "VTT_float",    //10   // val.f
  "VTT_double",   //11   // val.d
  "VTT_ptr",      //12   // val.ul
  "VTT_union",    //13   //
  "VTT_struct",   //14   //
  "VTT_func",     //15   //
  "VTT_string",   //16   //
  "VTT_enum",     //17   //
  "VTT_field",    //18   //
  "VTT_int64",    //19   // val.i64
  "VTT_uint64",   //20   // val.u64
  "VTT_???"       // invalid vtt type
};

const char *CUVSC_Log::RspCmd (UV_OPERATION cmd)  {
  const char *pS;

  switch (cmd) {
    //---Command codes:
    //--- General functions:
    case UV_NULL_CMD:             pS = UV_NULL_CMD_STR;                           break;
    case UV_GEN_GET_VERSION:      pS = UV_GEN_GET_VERSION_STR;                    break;
    case UV_GEN_UI_UNLOCK:        pS = UV_GEN_UI_UNLOCK_STR;                      break;
    case UV_GEN_UI_LOCK:          pS = UV_GEN_UI_LOCK_STR;                        break;
    case UV_GEN_HIDE:             pS = UV_GEN_HIDE_STR;                           break;
    case UV_GEN_SHOW:             pS = UV_GEN_SHOW_STR;                           break;
    case UV_GEN_RESTORE:          pS = UV_GEN_RESTORE_STR;                        break;
    case UV_GEN_MINIMIZE:         pS = UV_GEN_MINIMIZE_STR;                       break;
    case UV_GEN_MAXIMIZE:         pS = UV_GEN_MAXIMIZE_STR;                       break;
    case UV_GEN_EXIT:             pS = UV_GEN_EXIT_STR;                           break;
    case UV_GEN_GET_EXTVERSION:   pS = UV_GEN_GET_EXTVERSION_STR;                 break;
    case UV_GEN_CHECK_LICENSE:    pS = UV_GEN_CHECK_LICENSE_STR;                  break;
    case UV_GEN_CPLX_COMPLETE:    pS = UV_GEN_CPLX_COMPLETE_STR;                  break;
    case UV_GEN_SET_OPTIONS:      pS = UV_GEN_SET_OPTIONS_STR;                    break;  // [TdB: 14.04.2014] added

    //--- Project functions:
    case UV_PRJ_LOAD:             pS = UV_PRJ_LOAD_STR;                           break;
    case UV_PRJ_CLOSE:            pS = UV_PRJ_CLOSE_STR;                          break;
    case UV_PRJ_ADD_GROUP:        pS = UV_PRJ_ADD_GROUP_STR;                      break;
    case UV_PRJ_DEL_GROUP:        pS = UV_PRJ_DEL_GROUP_STR;                      break;
    case UV_PRJ_SET_TARGET:       pS = UV_PRJ_SET_TARGET_STR;                     break;
    case UV_PRJ_ADD_FILE:         pS = UV_PRJ_ADD_FILE_STR;                       break;
    case UV_PRJ_DEL_FILE:         pS = UV_PRJ_DEL_FILE_STR;                       break;
    case UV_PRJ_BUILD:            pS = UV_PRJ_BUILD_STR;                          break;
    case UV_PRJ_REBUILD:          pS = UV_PRJ_REBUILD_STR;                        break;
    case UV_PRJ_CLEAN:            pS = UV_PRJ_CLEAN_STR;                          break;
    case UV_PRJ_BUILD_CANCEL:     pS = UV_PRJ_BUILD_CANCEL_STR;                   break;
    case UV_PRJ_FLASH_DOWNLOAD:   pS = UV_PRJ_FLASH_DOWNLOAD_STR;                 break;
    case UV_PRJ_GET_DEBUG_TARGET: pS = UV_PRJ_GET_DEBUG_TARGET_STR;               break;
    case UV_PRJ_SET_DEBUG_TARGET: pS = UV_PRJ_SET_DEBUG_TARGET_STR;               break;
    case UV_PRJ_GET_OPTITEM:      pS = UV_PRJ_GET_OPTITEM_STR;                    break;
    case UV_PRJ_SET_OPTITEM:      pS = UV_PRJ_SET_OPTITEM_STR;                    break;
    case UV_PRJ_ENUM_GROUPS:      pS = UV_PRJ_ENUM_GROUPS_STR;                    break;
    case UV_PRJ_ENUM_TARGETS:     pS = UV_PRJ_ENUM_TARGETS_STR;                   break;
    case UV_PRJ_ENUM_FILES:       pS = UV_PRJ_ENUM_FILES_STR;                     break;
    case UV_PRJ_CMD_PROGRESS:     pS = UV_PRJ_CMD_PROGRESS_STR;                   break;
    case UV_PRJ_ACTIVE_FILES:     pS = UV_PRJ_ACTIVE_FILES_STR;                   break;
    case UV_PRJ_FLASH_ERASE:      pS = UV_PRJ_FLASH_ERASE_STR;                    break;
    case UV_PRJ_GET_OUTPUTNAME:   pS = UV_PRJ_GET_OUTPUTNAME_STR;                 break;
    case UV_PRJ_GET_CUR_TARGET:   pS = UV_PRJ_GET_CUR_TARGET_STR;                 break;
    case UV_PRJ_SET_OUTPUTNAME:   pS = UV_PRJ_SET_OUTPUTNAME_STR;                 break;
      
    //--- Debug functions:
    case UV_DBG_ENTER:            pS = UV_DBG_ENTER_STR;                          break;
    case UV_DBG_EXIT:             pS = UV_DBG_EXIT_STR;                           break;
    case UV_DBG_START_EXECUTION:  pS = UV_DBG_START_EXECUTION_STR;                break;
    case UV_DBG_RUN_TO_ADDRESS:   pS = UV_DBG_RUN_TO_ADDRESS_STR;                 break;  // [TdB: 14.04.2014] added
    case UV_DBG_STOP_EXECUTION:   pS = UV_DBG_STOP_EXECUTION_STR;                 break;
    case UV_DBG_STATUS:           pS = UV_DBG_STATUS_STR;                         break;
    case UV_DBG_RESET:            pS = UV_DBG_RESET_STR;                          break;
    case UV_DBG_STEP_HLL:         pS = UV_DBG_STEP_HLL_STR;                       break;
    case UV_DBG_STEP_HLL_N:       pS = UV_DBG_STEP_HLL_N_STR;                     break;  // [TdB: 14.04.2014] added
    case UV_DBG_STEP_INTO:        pS = UV_DBG_STEP_INTO_STR;                      break;
    case UV_DBG_STEP_INTO_N:      pS = UV_DBG_STEP_INTO_N_STR;                    break;  // [TdB: 14.04.2014] added
    case UV_DBG_STEP_INSTRUCTION: pS = UV_DBG_STEP_INSTRUCTION_STR;               break;
    case UV_DBG_STEP_INSTRUCTION_N: pS = UV_DBG_STEP_INSTRUCTION_N_STR;           break;  // [TdB: 14.04.2014] added
    case UV_DBG_STEP_OUT:         pS = UV_DBG_STEP_OUT_STR;                       break;
    case UV_DBG_CALC_EXPRESSION:  pS = UV_DBG_CALC_EXPRESSION_STR;                break;
    case UV_DBG_EVAL_EXPRESSION_TO_STR: pS = UV_DBG_EVAL_EXPRESSION_TO_STR_STR;   break;  // [TdB: 14.04.2014] break missing
    case UV_DBG_MEM_READ:         pS = UV_DBG_MEM_READ_STR;                       break;
    case UV_DBG_MEM_WRITE:        pS = UV_DBG_MEM_WRITE_STR;                      break;
    case UV_DBG_TIME_INFO:        pS = UV_DBG_TIME_INFO_STR;                      break;
    case UV_DBG_SET_CALLBACK:     pS = UV_DBG_SET_CALLBACK_STR;                   break;
    case UV_DBG_VTR_GET:          pS = UV_DBG_VTR_GET_STR;                        break;
    case UV_DBG_VTR_SET:          pS = UV_DBG_VTR_SET_STR;                        break;
    case UV_DBG_SERIAL_GET:       pS = UV_DBG_SERIAL_GET_STR;                     break;
    case UV_DBG_SERIAL_PUT:       pS = UV_DBG_SERIAL_PUT_STR;                     break;
    case UV_DBG_VERIFY_CODE:      pS = UV_DBG_VERIFY_CODE_STR;                    break;
    case UV_DBG_CREATE_BP:        pS = UV_DBG_CREATE_BP_STR;                      break;
    case UV_DBG_ENUMERATE_BP:     pS = UV_DBG_ENUMERATE_BP_STR;                   break;
    case UV_DBG_CHANGE_BP:        pS = UV_DBG_CHANGE_BP_STR;                      break;
    case UV_DBG_ENUM_SYMTP:       pS = UV_DBG_ENUM_SYMTP_STR;                     break;
    case UV_DBG_ADR_TOFILELINE:   pS = UV_DBG_ADR_TOFILELINE_STR;                 break;
    case UV_DBG_FILELINE_TO_ADR:  pS = UV_DBG_FILELINE_TO_ADR_STR;                break;
    case UV_DBG_ENUM_STACK:       pS = UV_DBG_ENUM_STACK_STR;                     break;
    case UV_DBG_ENUM_VTR:         pS = UV_DBG_ENUM_VTR_STR;                       break;
    case UV_DBG_ADR_SHOWCODE:     pS = UV_DBG_ADR_SHOWCODE_STR;                   break;
    case UV_DBG_POWERSCALE_SHOWCODE:  pS = UV_DBG_POWERSCALE_SHOWCODE_STR;        break;
    case UV_DBG_POWERSCALE_SHOWPOWER: pS = UV_DBG_POWERSCALE_SHOWPOWER_STR;       break;
    case POWERSCALE_OPEN:         pS = POWERSCALE_OPEN_STR;                       break;  // [TdB: 14.04.2014] added
    case UV_DBG_WAKE:             pS = UV_DBG_WAKE_STR;                           break;
    case UV_DBG_SLEEP:            pS = UV_DBG_SLEEP_STR;                          break;
    case UV_DBG_EXEC_CMD:         pS = UV_DBG_EXEC_CMD_STR;                       break;
    case UV_MSGBOX_MSG:           pS = UV_MSGBOX_MSG_STR;                         break;
    case UV_DBG_ITM_REGISTER:     pS = UV_DBG_ITM_REGISTER_STR;                   break;
    case UV_DBG_ITM_UNREGISTER:   pS = UV_DBG_ITM_UNREGISTER_STR;                 break;
    case UV_DBG_EVTR_REGISTER:    pS = UV_DBG_EVTR_REGISTER_STR;                  break;
    case UV_DBG_EVTR_UNREGISTER:  pS = UV_DBG_EVTR_UNREGISTER_STR;                break;
    case UV_DBG_EVTR_ENUMSCVDFILES: pS = UV_DBG_EVTR_ENUMSCVDFILES_STR;           break;

    //---Registers:
    case UV_DBG_ENUM_REGISTER_GROUPS:    pS = UV_DBG_ENUM_REGISTER_GROUPS_STR;    break;  // [TdB: 14.04.2014] added
    case UV_DBG_ENUM_REGISTERS:          pS = UV_DBG_ENUM_REGISTERS_STR;          break;  // [TdB: 14.04.2014] added
    case UV_DBG_READ_REGISTERS:          pS = UV_DBG_READ_REGISTERS_STR;          break;  // [TdB: 14.04.2014] added
    case UV_DBG_REGISTER_SET:            pS = UV_DBG_REGISTER_SET_STR;            break;  // [TdB: 14.04.2014] added
    case UV_DBG_DSM_READ:                pS = UV_DBG_DSM_READ_STR;                break;  // [TdB: 14.04.2014] added
    case UV_DBG_EVAL_WATCH_EXPRESSION:   pS = UV_DBG_EVAL_WATCH_EXPRESSION_STR;   break;  // [TdB: 14.04.2014] added
    case UV_DBG_REMOVE_WATCH_EXPRESSION: pS = UV_DBG_REMOVE_WATCH_EXPRESSION_STR; break;  // [TdB: 14.04.2014] added
    case UV_DBG_ENUM_VARIABLES:          pS = UV_DBG_ENUM_VARIABLES_STR;          break;  // [TdB: 14.04.2014] added
    case UV_DBG_VARIABLE_SET:            pS = UV_DBG_VARIABLE_SET_STR;            break;  // [TdB: 14.04.2014] added
    case UV_DBG_ENUM_TASKS:              pS = UV_DBG_ENUM_TASKS_STR;              break;  // [TdB: 14.04.2014] added
    case UV_DBG_ENUM_MENUS:              pS = UV_DBG_ENUM_MENUS_STR;              break;  // [TdB: 14.04.2014] added
    case UV_DBG_MENU_EXEC:               pS = UV_DBG_MENU_EXEC_STR;               break;  // [TdB: 14.04.2014] added

    //---Answers/Error from Server to Client:
    case UV_CMD_RESPONSE:         pS = UV_CMD_RESPONSE_STR;                       break;
    
    //---Asynchronous messages:
    case UV_ASYNC_MSG:            pS = UV_ASYNC_MSG_STR;                          break;
    
    //--- Special Asynchronous messages:
    //--- Project functions:
    case UV_PRJ_BUILD_COMPLETE:   pS = UV_PRJ_BUILD_COMPLETE_STR;                 break;
    case UV_PRJ_BUILD_OUTPUT:     pS = UV_PRJ_BUILD_OUTPUT_STR;                   break;
    
    //--- Debug functions:
    case UV_DBG_CALLBACK:         pS = UV_DBG_CALLBACK_STR;                       break;
    
    //--- Response to UV_DBG_ENUMERATE_BP:  
    case UV_DBG_BP_ENUM_START:    pS = UV_DBG_BP_ENUM_START_STR;                  break;
    case UV_DBG_BP_ENUMERATED:    pS = UV_DBG_BP_ENUMERATED_STR;                  break;
    case UV_DBG_BP_ENUM_END:      pS = UV_DBG_BP_ENUM_END_STR;                    break;

    //--- Response to UV_PRJ_ENUM_GROUPS:
    case UV_PRJ_ENUM_GROUPS_START:pS = UV_PRJ_ENUM_GROUPS_START_STR;              break;
    case UV_PRJ_ENUM_GROUPS_ENU:  pS = UV_PRJ_ENUM_GROUPS_ENU_STR;                break;
    case UV_PRJ_ENUM_GROUPS_END:  pS = UV_PRJ_ENUM_GROUPS_END_STR;                break;

    //--- Response to UV_PRJ_ENUM_TARGETS:
    case UV_PRJ_ENUM_TARGETS_START: pS = UV_PRJ_ENUM_TARGETS_START_STR;           break;
    case UV_PRJ_ENUM_TARGETS_ENU:   pS = UV_PRJ_ENUM_TARGETS_ENU_STR;             break;
    case UV_PRJ_ENUM_TARGETS_END:   pS = UV_PRJ_ENUM_TARGETS_END_STR;             break;

    //--- Response to UV_PRJ_ENUM_FILES:
    case UV_PRJ_ENUM_FILES_START: pS = UV_PRJ_ENUM_FILES_START_STR;               break;
    case UV_PRJ_ENUM_FILES_ENU:   pS = UV_PRJ_ENUM_FILES_ENU_STR;                 break;
    case UV_PRJ_ENUM_FILES_END:   pS = UV_PRJ_ENUM_FILES_END_STR;                 break;    

    //--- Response to UV_DBG_EVTR_ENUMSCVDFILES:
    case UV_DBG_EVTR_ENUMSCVDFILES_START:pS = UV_DBG_EVTR_ENUMSCVDFILES_START_STR;       break;
    case UV_DBG_EVTR_ENUMSCVDFILES_ENU:  pS = UV_DBG_EVTR_ENUMSCVDFILES_ENU_STR;         break;
    case UV_DBG_EVTR_ENUMSCVDFILES_END:  pS = UV_DBG_EVTR_ENUMSCVDFILES_END_STR;         break;

    //--- Progress bar functions
    case UV_PRJ_PBAR_INIT:        pS = UV_PRJ_PBAR_INIT_STR;                      break;
    case UV_PRJ_PBAR_STOP:        pS = UV_PRJ_PBAR_STOP_STR;                      break;
    case UV_PRJ_PBAR_SET:         pS = UV_PRJ_PBAR_SET_STR;                       break;
    case UV_PRJ_PBAR_TEXT:        pS = UV_PRJ_PBAR_TEXT_STR;                      break;

    //--- Response to UV_DBG_ENUM_SYMTP:
    case UV_DBG_ENUM_SYMTP_START: pS = UV_DBG_ENUM_SYMTP_START_STR;               break;
    case UV_DBG_ENUM_SYMTP_ENU:   pS = UV_DBG_ENUM_SYMTP_ENU_STR;                 break;
    case UV_DBG_ENUM_SYMTP_END:   pS = UV_DBG_ENUM_SYMTP_END_STR;                 break;

    //--- Response to UV_DBG_ENUM_STACK:
    case UV_DBG_ENUM_STACK_START: pS = UV_DBG_ENUM_STACK_START_STR;               break;
    case UV_DBG_ENUM_STACK_ENU:   pS = UV_DBG_ENUM_STACK_ENU_STR;                 break;
    case UV_DBG_ENUM_STACK_END:   pS = UV_DBG_ENUM_STACK_END_STR;                 break;

    //--- Response to UV_DBG_ENUM_VTR:
    case UV_DBG_ENUM_VTR_START:   pS = UV_DBG_ENUM_VTR_START_STR;                 break;
    case UV_DBG_ENUM_VTR_ENU:     pS = UV_DBG_ENUM_VTR_ENU_STR;                   break;
    case UV_DBG_ENUM_VTR_END:     pS = UV_DBG_ENUM_VTR_END_STR;                   break;

    //--- Command Window output:
    case UV_DBG_CMD_OUTPUT:       pS = UV_DBG_CMD_OUTPUT_STR;                     break;
    case UV_DBG_SERIAL_OUTPUT:    pS = UV_DBG_SERIAL_OUTPUT_STR;                  break;  // [TdB: 14.04.2014] added
    case UV_DBG_ITM_OUTPUT:       pS = UV_DBG_ITM_OUTPUT_STR;                     break;  // [TdB: 24.05.2016] added
    case UV_DBG_EVTR_OUTPUT:      pS = UV_DBG_EVTR_OUTPUT_STR;                    break;

    //--- Real-Time Agent:
    case UV_RTA_MESSAGE:          pS = UV_RTA_MESSAGE_STR;                        break;
    case UV_RTA_INCOMPATIBLE:     pS = UV_RTA_INCOMPATIBLE_STR;                   break;    

    //--- Response to UV_DBG_ENUM_REGISTER_GROUPS:
    case UV_DBG_ENUM_REGISTER_GROUPS_START:   pS = UV_DBG_ENUM_REGISTER_GROUPS_START_STR;    break;   // [TdB: 14.04.2014] added
    case UV_DBG_ENUM_REGISTER_GROUPS_ENU:     pS = UV_DBG_ENUM_REGISTER_GROUPS_ENU_STR;      break;   // [TdB: 14.04.2014] added
    case UV_DBG_ENUM_REGISTER_GROUPS_END:     pS = UV_DBG_ENUM_REGISTER_GROUPS_END_STR;      break;   // [TdB: 14.04.2014] added

    //--- Response to UV_DBG_ENUM_REGISTERS:
    case UV_DBG_ENUM_REGISTERS_START:         pS = UV_DBG_ENUM_REGISTERS_START_STR;          break;   // [TdB: 14.04.2014] added
    case UV_DBG_ENUM_REGISTERS_ENU:           pS = UV_DBG_ENUM_REGISTERS_ENU_STR;            break;   // [TdB: 14.04.2014] added
    case UV_DBG_ENUM_REGISTERS_END:           pS = UV_DBG_ENUM_REGISTERS_END_STR;            break;   // [TdB: 14.04.2014] added

    //--- Response to UV_DBG_ENUM_VARIABLES:
    case UV_DBG_ENUM_VARIABLES_START:         pS = UV_DBG_ENUM_VARIABLES_START_STR;          break;   // [TdB: 14.04.2014] added
    case UV_DBG_ENUM_VARIABLES_ENU:           pS = UV_DBG_ENUM_VARIABLES_ENU_STR;            break;   // [TdB: 14.04.2014] added
    case UV_DBG_ENUM_VARIABLES_END:           pS = UV_DBG_ENUM_VARIABLES_END_STR;            break;   // [TdB: 14.04.2014] added

    //--- Response to UV_DBG_ENUM_TASKS:
    case UV_DBG_ENUM_TASKS_START:             pS = UV_DBG_ENUM_TASKS_START_STR;              break;   // [TdB: 14.04.2014] added
    case UV_DBG_ENUM_TASKS_ENU:               pS = UV_DBG_ENUM_TASKS_ENU_STR;                break;   // [TdB: 14.04.2014] added
    case UV_DBG_ENUM_TASKS_END:               pS = UV_DBG_ENUM_TASKS_END_STR;                break;   // [TdB: 14.04.2014] added

    //--- Response to UV_DBG_ENUM_MENUS:
    case UV_DBG_ENUM_MENUS_START:             pS = UV_DBG_ENUM_MENUS_START_STR;              break;   // [TdB: 14.04.2014] added
    case UV_DBG_ENUM_MENUS_ENU:               pS = UV_DBG_ENUM_MENUS_ENU_STR;                break;   // [TdB: 14.04.2014] added
    case UV_DBG_ENUM_MENUS_END:               pS = UV_DBG_ENUM_MENUS_END_STR;                break;   // [TdB: 14.04.2014] added

    //--- Test definititions (for testing only):
    case UV_TST_1:                pS = UV_TST_1_STR;                              break;
    case UV_TST_2:                pS = UV_TST_2_STR;                              break;
    case UV_TST_3:                pS = UV_TST_3_STR;                              break;
    case UV_TST_4:                pS = UV_TST_4_STR;                              break;
    case UV_TST_5:                pS = UV_TST_5_STR;                              break;
    case UV_TST_6:                pS = UV_TST_6_STR;                              break;
    case UV_TST_7:                pS = UV_TST_7_STR;                              break;
    case UV_TST_8:                pS = UV_TST_8_STR;                              break;
    case UV_TST_9:                pS = UV_TST_9_STR;                              break;
    case UV_TST_10:               pS = UV_TST_10_STR;                             break;
     break;

    default:
     pS = UV_UNKNOWN_COMMAND_STR;
     break;
  }

  return (pS);
}


//===========================================================================
// UVSC Logging Class
//===========================================================================

// Mutex lock helper macros
#define LOG_LOCK()           WaitForSingleObject (m_logMtx, INFINITE)
#define LOG_UNLOCK()         ReleaseMutex (m_logMtx)
//#define LOG_LOCK()           
//#define LOG_UNLOCK()         

CUVSC_Log::CUVSC_Log() {
  m_fileHandle = INVALID_HANDLE_VALUE;
  m_logMtx = CreateMutex(NULL, FALSE, NULL);              // Don't name, otherwise all objects get same mutex!!!
  m_TimeZero = GetTickCount();
  m_EnableRaw = false;
  m_EnableTrace = true;
  m_Callback = NULL;
}

CUVSC_Log::~CUVSC_Log() {
  LOG_LOCK();
  LOG_UNLOCK();
  this->Stop();
  CloseHandle(m_logMtx);
}

void CUVSC_Log::Start (char *filename, bool append, log_cb callback) {
  DWORD dwCreationDisposition = CREATE_ALWAYS;

  LOG_LOCK();

  // Check params
  if (filename && (filename[0] != 0)) {
    // Set append
    if (append) dwCreationDisposition = OPEN_ALWAYS;
    
    // Open file
    m_fileHandle = CreateFile(filename,                     // LPCTSTR lpFileName,
                              GENERIC_READ | GENERIC_WRITE, // DWORD dwDesiredAccess,
                              FILE_SHARE_READ,              // DWORD dwShareMode,
                              NULL,                         // LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                              dwCreationDisposition,        // DWORD dwCreationDisposition,
                              FILE_ATTRIBUTE_NORMAL,        // DWORD dwFlagsAndAttributes,
                              NULL);                        // HANDLE hTemplateFile
  }
  else {
    m_fileHandle = INVALID_HANDLE_VALUE;
  }

  if (callback) {
    m_Callback = callback;
  }
  LOG_UNLOCK();
}

#define STEMP_MAX  (SOCK_NDATA*4)       // This is a LARGE string buffer - should be big enough for any message
void CUVSC_Log::Log (Direction dir, UVSOCK_CMD *msg, bool raw) {
  static char sTemp[STEMP_MAX+1]; 
  char *pStr;
  size_t bytesToWrite;
  DWORD bytesWritten;

  LOG_LOCK();

  if (msg != NULL) {
    sTemp[STEMP_MAX] = '\0';       // Ensure there is always a NULL terminator
    
    /* Format and log the message */

    pStr = sTemp;
    if (!raw) {                               // Decode, or show raw     
      pStr = DecodeMessage(pStr, msg, dir);
    }
    else {
      pStr = DecodeRaw(pStr, msg, dir);
    }
    *pStr = '\0';              // Ensure there is always a NULL terminator   
    bytesToWrite = strlen(sTemp); 

    if (m_fileHandle != INVALID_HANDLE_VALUE) {
      WriteFile(m_fileHandle,           // HANDLE hFile,
                sTemp,                  // LPCVOID lpBuffer,
                (DWORD)bytesToWrite,    // DWORD nNumberOfBytesToWrite,
                &bytesWritten,          // LPDWORD lpNumberOfBytesWritten,
                NULL);                  // LPOVERLAPPED lpOverlapped
    } // if (m_fileHandle != INVALID_HANDLE_VALUE) {      

    // +1/-1 on the callback is to ignore the '\n' at the start of the message
    if (m_Callback) m_Callback(sTemp+1, (int)bytesToWrite-1);

    // Log again in RAW mode (only if m_EnableRaw and not logging RAW already)    
    if ((!raw) && m_EnableRaw) {        
      pStr = sTemp;
      pStr = DecodeRaw(pStr, msg, dir);
    
      *pStr = '\0';              // Ensure there is always a NULL terminator   
      bytesToWrite = strlen(sTemp); 

      if (m_fileHandle != INVALID_HANDLE_VALUE) {
        WriteFile(m_fileHandle,           // HANDLE hFile,
                  sTemp,                  // LPCVOID lpBuffer,
                  (DWORD)bytesToWrite,    // DWORD nNumberOfBytesToWrite,
                  &bytesWritten,          // LPDWORD lpNumberOfBytesWritten,
                  NULL);                  // LPOVERLAPPED lpOverlapped
      } // if (m_fileHandle != INVALID_HANDLE_VALUE) {      

      // +1/-1 on the callback is to ignore the '\n' at the start of the message
      if (m_Callback) m_Callback(sTemp+1, (int)bytesToWrite-1);
    }


  } // if (msg != NULL) {

  LOG_UNLOCK();
}

void CUVSC_Log::Trace (TraceType type, const char *msg, UVSC_STATUS status) {
  static char sTemp[STEMP_MAX+1]; 
  char *pStr;
  size_t bytesToWrite;
  DWORD bytesWritten;

  LOG_LOCK();

  if ((m_EnableTrace) && (msg != NULL)) {
    sTemp[STEMP_MAX] = '\0';       // Ensure there is always a NULL terminator
    
    /*============================*/
    /* Format and log the trace   */
    /*============================*/

    pStr = sTemp;        
    pStr = DecodeTrace(pStr, msg, type, status);

    *pStr = '\0';              // Ensure there is always a NULL terminator   
    bytesToWrite = strlen(sTemp); 

    if (m_fileHandle != INVALID_HANDLE_VALUE) {
      WriteFile(m_fileHandle,           // HANDLE hFile,
                sTemp,                  // LPCVOID lpBuffer,
                (DWORD)bytesToWrite,    // DWORD nNumberOfBytesToWrite,
                &bytesWritten,          // LPDWORD lpNumberOfBytesWritten,
                NULL);                  // LPOVERLAPPED lpOverlapped
    } // if (m_fileHandle != INVALID_HANDLE_VALUE) {      

    // +1/-1 on the callback is to ignore the '\n' at the start of the message
    if (m_Callback) m_Callback(sTemp+1, (int)bytesToWrite-1);
  } // if ((m_EnableTrace) && (msg != NULL)) {

  LOG_UNLOCK();
}

void CUVSC_Log::Stop () {
  // Close the log file
  LOG_LOCK();

  if (m_fileHandle != INVALID_HANDLE_VALUE) {
    CloseHandle(m_fileHandle);
    m_fileHandle = INVALID_HANDLE_VALUE;
  }

  if (m_Callback) m_Callback = NULL;

  LOG_UNLOCK();
}

void CUVSC_Log::Control (bool enableRaw, bool enableTrace) {
  LOG_LOCK();

  m_EnableRaw = enableRaw;
  m_EnableTrace = enableTrace;

  LOG_UNLOCK();
}


const char *CUVSC_Log::RspText (UV_STATUS err)  {
  const char    *pT;

  if ((UINT)err < (UINT)UV_STATUS_END) {
    pT = szRt[err];
  }
  else {
    pT = szRt[UV_STATUS_END];;
  }

  return (pT);
}

const char *CUVSC_Log::UVSCStatusText (UVSC_STATUS status)  {
  const char    *pT;

  if ((UINT)status < (UINT)UVSC_STATUS_END) {
    pT = szUVSC[status];
  }
  else {
    pT = szUVSC[UVSC_STATUS_END];
  }

  return (pT);
}


const char *CUVSC_Log::UVSCStopReasonText (STOPREASON stopReason)  {
  const char    *pT;
  int stopIndex = 0;
  int stopR = (int)stopReason;

  if ((UINT)stopR < (UINT)STOPREASON_END) {
    if (stopR) {
      stopIndex = 1;
      while ((stopR & 0x01) == 0) {     
        stopR >>= 1;
        stopIndex++;
      }
    }
    pT = szStopReason[stopIndex];
  }
  else {
    pT = "STOPREASON_???";
  }

  return (pT);
}


const char *CUVSC_Log::UVSCBpTypeText (BKTYPE type)  {
  const char    *pT;

  if ((UINT)type < (UINT)BRKTYPE_END) {
    pT = szBpt[type];
  }
  else {
    pT = szBpt[BRKTYPE_END];
  }

  return (pT);
}


const char *CUVSC_Log::UVSCBpChgText (CHG_TYPE type)  {
  const char    *pT;

  if ((UINT)type < (UINT)CHG_END) {
    pT = szBkChgType[type];
  }
  else {
    pT = szBkChgType[CHG_END];
  }

  return (pT);
}


const char *CUVSC_Log::UVSCOptSelText (OPTSEL opt)  {
  const char    *pT;

  if ((UINT)opt < (UINT)OPT_END) {
    pT = szOjob[opt];
  }
  else {
    pT = szOjob[OPT_END];
  }

  return (pT);
}


const char *CUVSC_Log::UVSCVTTTypeText (VTT_TYPE vType)  {
  const char    *pT;

  if ((UINT)vType < (UINT)VTT_end) {
    pT = szVTTType[vType];
  }
  else {
    pT = szVTTType[VTT_end];
  }

  return (pT);
}


char *CUVSC_Log::DecodeError(char *pStr, UVSOCK_CMD_RESPONSE *rsp) {
  pStr += _snprintf(pStr, STEMP_MAX, "[%s", RspText(rsp->status));

  //if (rsp->status != UV_STATUS_SUCCESS) {
  //  pStr += _snprintf(pStr, STEMP_MAX, " [--++** !! ERROR !! **++--] ");
  //}

  if (rsp->err.StrLen > 0) {    
    pStr += _snprintf(pStr, STEMP_MAX, "(");
    pStr += _snprintf(pStr, rsp->err.StrLen, (const char *)rsp->err.str);
    pStr += _snprintf(pStr, STEMP_MAX, ")");
  }
  pStr += _snprintf(pStr, STEMP_MAX, "]");

  return (pStr);
} 


char *CUVSC_Log::DecodeBkParm(char *pStr, BKPARM *bkparm) {
  pStr += _snprintf(pStr, STEMP_MAX, "[(BKPARM) Type:%s Access Size:%d Count:%d", 
                    UVSCBpTypeText(bkparm->type), bkparm->accSize, bkparm->count);
        
  if (bkparm->nExpLen > 1)  {
    pStr += _snprintf(pStr, STEMP_MAX, " Expr:%s", bkparm->szBuffer);
  }
  if (bkparm->nCmdLen > 1)  {
    pStr += _snprintf(pStr, STEMP_MAX, " Cmd:%s", &bkparm->szBuffer[bkparm->nExpLen]);
  }

  pStr += _snprintf(pStr, STEMP_MAX, "]");
  return (pStr);
}

char *CUVSC_Log::DecodeBkRsp(char *pStr, BKRSP *bkrsp)  {   // Hp. /22.7.2009/
  pStr += _snprintf(pStr, STEMP_MAX, "[(BKRSP) Type:%s %sabled Addr:0x%08X Count:%d nTickMark:0x%08X",
                    UVSCBpTypeText(bkrsp->type), bkrsp->enabled ? "En" : "Dis", (DWORD) bkrsp->nAddress, bkrsp->count, bkrsp->nTickMark);
                            
  if (bkrsp->nExpLen > 1)  {
    pStr += _snprintf(pStr, STEMP_MAX, " Expr:%s", bkrsp->szBuffer);
  }

  pStr += _snprintf(pStr, STEMP_MAX, "]");

  return (pStr);
}


static const char szBpAct[][32] =  {
  "BP_CANCELLED",    // UV_STATUS_BP_CANCELLED        = 15, ///< Operation failed: Create new breakpoint has been cancelled
  "BP_BADADDRESS",   // UV_STATUS_BP_BADADDRESS       = 16, ///< Operation failed: Invalid address in create breakpoint
  "BP_NOTSUPPORTED", // UV_STATUS_BP_NOTSUPPORTED     = 17, ///< Operation failed: Type of breakpoint is not supported (by target)
  "BP_FAILED",       // UV_STATUS_BP_FAILED           = 18, ///< Operation failed: Breakpoint creation failed (syntax error, nested command etc.)
  "BP_REDEFINED",    // UV_STATUS_BP_REDEFINED        = 19, ///< Breakpoint Info: A breakpoint has been redefined
  "BP_DISABLED",     // UV_STATUS_BP_DISABLED         = 20, ///< Breakpoint Info: A breakpoint has been disabled
  "BP_ENABLED",      // UV_STATUS_BP_ENABLED          = 21, ///< Breakpoint Info: A breakpoint has been enabled
  "BP_CREATED",      // UV_STATUS_BP_CREATED          = 22, ///< Breakpoint Info: A breakpoint has been created
  "BP_DELETED",      // UV_STATUS_BP_DELETED          = 23, ///< Breakpoint Info: A breakpoint has been deleted
  "BP_NOTFOUND",     // UV_STATUS_BP_NOTFOUND         = 24, ///< Operation failed: Breakpoint with @a nTickMark cookie not found.
};

char *CUVSC_Log::DecodeBkRsp(char *pStr, BKRSP *bkrsp, UV_STATUS nBpCode) {
  const char *pAct;

  if (nBpCode >= UV_STATUS_BP_CANCELLED && UV_STATUS_BP_NOTFOUND)  {
    pAct = szBpAct [nBpCode - UV_STATUS_BP_CANCELLED];
  }
  else  {
    pAct = "BP_Unknown_Action";
  }
  pStr += _snprintf(pStr, STEMP_MAX, "[(BKRSP) Type:%s cmdRsp.status=%s Addr:0x%08X Count:%d nTickMark:0x%08X",
           UVSCBpTypeText(bkrsp->type), pAct, (DWORD) bkrsp->nAddress, bkrsp->count, bkrsp->nTickMark);

  if (bkrsp->nExpLen > 1)  {
    pStr += _snprintf(pStr, STEMP_MAX, " Expr:%s", bkrsp->szBuffer);
  }
  pStr += _snprintf(pStr, STEMP_MAX, "]");
  return (pStr);
}


char *CUVSC_Log::DecodeBkChg(char *pStr, BKCHG *bkchg) {
  pStr += _snprintf(pStr, STEMP_MAX, "[(BKCHG) Tick:0x%08X Change:%s]", bkchg->nTickMark, UVSCBpChgText(bkchg->type));
  return (pStr);
}

char *CUVSC_Log::DecodePrjActiveFiles(char *pStr, UINT nVal) {
  pStr += _snprintf(pStr, STEMP_MAX, "[(nVal) %d]", nVal);
  return (pStr);
}

char *CUVSC_Log::DecodeGenGetVersion(char *pStr, UINT nVal) {
  pStr += _snprintf(pStr, STEMP_MAX, "[(nVal) %d]", nVal);
  return (pStr);
}

char *CUVSC_Log::DecodeDbgStatus(char *pStr, UINT nVal) {
  pStr += _snprintf(pStr, STEMP_MAX, "[(nVal) %s]", nVal ? "Running" : "Stopped");
  return (pStr);
}

char *CUVSC_Log::DecodePrjData(char *pStr, PRJDATA *prjData) {
  UINT len, count = 0;

  pStr += _snprintf(pStr, STEMP_MAX, "[(PRJDATA) Code:%0X ", prjData->nCode);
  if (prjData->nLen > 0) {
    while (count < (prjData->nLen-1)) {
      len = _snprintf(pStr, STEMP_MAX, "%s ", &prjData->szNames[count]);
      count += len;
      pStr += len;
    }
  }
  pStr += _snprintf(pStr, STEMP_MAX, "]");

  return (pStr);
}

char *CUVSC_Log::DecodeTrnOpt(char *pStr, TRNOPT *trnopt) {
  pStr += _snprintf(pStr, STEMP_MAX, "[(TRNOPT) Opt:%s", UVSCOptSelText(trnopt->job));

  if (trnopt->iTarg) {
    pStr += _snprintf(pStr, STEMP_MAX, " Target:%s", &trnopt->szBuffer[trnopt->iTarg]);
  }
  if (trnopt->iGroup) {
    pStr += _snprintf(pStr, STEMP_MAX, " Group:%s", &trnopt->szBuffer[trnopt->iGroup]);
  }
  if (trnopt->iFile) {
    pStr += _snprintf(pStr, STEMP_MAX, " File:%s", &trnopt->szBuffer[trnopt->iFile]);
  }
  if (trnopt->iItem) {
    pStr += _snprintf(pStr, STEMP_MAX, " Item:%s", &trnopt->szBuffer[trnopt->iItem]);
  }

  pStr += _snprintf(pStr, STEMP_MAX, "]");
  return (pStr);
}

char *CUVSC_Log::DecodeSstr(char *pStr, SSTR *sstr) {
  pStr += _snprintf(pStr, STEMP_MAX, "[(SSTR)");
  if (sstr->nLen > 0) {
    pStr += _snprintf(pStr, STEMP_MAX, " %s", sstr->szStr);
  }
  pStr += _snprintf(pStr, STEMP_MAX, "]");

  return (pStr);
}

char *CUVSC_Log::DecodeExecCmd(char *pStr, EXECCMD *execcmd) {
  pStr += _snprintf(pStr, STEMP_MAX, "[(EXECCMD) bEcho:%d ", execcmd->bEcho);
  pStr = DecodeSstr(pStr, &execcmd->sCmd);
  pStr += _snprintf(pStr, STEMP_MAX, "]");

  return (pStr);
}

char *CUVSC_Log::DecodeTVal(char *pStr, TVAL *tval) {
  pStr += _snprintf(pStr, STEMP_MAX, "[(TVAL) %s ", UVSCVTTTypeText(tval->vType));
  
  switch (tval->vType) {
    case VTT_void:
    case VTT_bit:
    case VTT_ptr:
    case VTT_union:
    case VTT_struct:
    case VTT_func:
    case VTT_enum:
    case VTT_field:
      pStr += _snprintf(pStr, STEMP_MAX, "0x%08X", tval->v.ul);
      break;

    case VTT_char:
    case VTT_int:
    case VTT_short:
    case VTT_long:
      pStr += _snprintf(pStr, STEMP_MAX, "0x%08X (%d)", tval->v.ul, tval->v.l);
      break;

    case VTT_uchar:
    case VTT_uint:
    case VTT_ushort:
    case VTT_ulong:
      pStr += _snprintf(pStr, STEMP_MAX, "0x%08X (%d)", tval->v.ul, tval->v.ul);
      break;

    case VTT_float:
      pStr += _snprintf(pStr, STEMP_MAX, "%f", tval->v.f);
      break;

    case VTT_double:
      pStr += _snprintf(pStr, STEMP_MAX, "%lf", tval->v.d);
      break;
    
    case VTT_int64:
    case VTT_uint64:
      pStr += _snprintf(pStr, STEMP_MAX, "0x%08X%08X", (DWORD)(tval->v.u64 >> 32), (DWORD)(tval->v.u64 & 0xFFFFFFFF));
      break;

    default:
      pStr += _snprintf(pStr, STEMP_MAX, "???");
  }
  pStr += _snprintf(pStr, STEMP_MAX, "]");

  return (pStr);
}

char *CUVSC_Log::DecodeVSet(char *pStr, VSET *vset) {
  pStr += _snprintf(pStr, STEMP_MAX, "[(VSET) ");
  pStr = DecodeTVal(pStr, &vset->val);
  pStr = DecodeSstr(pStr, &vset->str);
  pStr += _snprintf(pStr, STEMP_MAX, "]");

  return (pStr);
}

char *CUVSC_Log::DecodeAMem(char *pStr, AMEM *amem, bool showData) {
  UINT i;

  pStr += _snprintf(pStr, STEMP_MAX, "[(AMEM) ");
  pStr += _snprintf(pStr, STEMP_MAX, "Addr:0x%08X, Len:%d ",
                    (DWORD)amem->nAddr,
                    amem->nBytes);
  if (amem->nErr) {
    pStr += _snprintf(pStr, STEMP_MAX, "Err:0x%08X", (DWORD)amem->ErrAddr);
  }
  else {
    if (showData) {
      pStr += _snprintf(pStr, STEMP_MAX, "Data: ");
      for (i=0; i<amem->nBytes ;i++) {
        pStr += _snprintf(pStr, STEMP_MAX, "%02X ", amem->aBytes[i]);
      }
    }
  }

  pStr += _snprintf(pStr, STEMP_MAX, "]");
  return (pStr);
}

char *CUVSC_Log::DecodeSerIO(char *pStr, SERIO *serIO, bool showData) {
  DWORD i;

  pStr += _snprintf(pStr, STEMP_MAX, "[(SERIO) ");
  pStr += _snprintf(pStr, STEMP_MAX, "Channel:%d Count:%000d", serIO->nChannel, serIO->nMany);
  if (showData && (serIO->nMany > 0)) {
    // Raw data
    pStr += _snprintf(pStr, STEMP_MAX, " Data: ");
    for (i=0; i<serIO->nMany ;i++) {
      pStr += _snprintf(pStr, STEMP_MAX, "%02X ", serIO->s.aBytes[i]);
    }

    // As string
    pStr += _snprintf(pStr, STEMP_MAX, " String:\"" /*, serIO->nChannel, serIO->nMany*/);
    pStr += _snprintf(pStr, serIO->nMany, "%s", (char *)&serIO->s.aBytes[0]);
    pStr += _snprintf(pStr, serIO->nMany, "\"");
  }

  pStr += _snprintf(pStr, STEMP_MAX, "]");
  return (pStr);
}



char *CUVSC_Log::DecodeItmOut(char *pStr, ITMOUT *itmIout, bool showData) {
  DWORD i;

  pStr += _snprintf(pStr, STEMP_MAX, "[(ITMOUT) ");
  pStr += _snprintf(pStr, STEMP_MAX, "Channel:%d Count:%000d", itmIout->nChannel, itmIout->nMany);
  if (showData && (itmIout->nMany > 0)) {
    // Raw data
    pStr += _snprintf(pStr, STEMP_MAX, " Data: ");
    for (i=0; i<itmIout->nMany ;i++) {
      pStr += _snprintf(pStr, STEMP_MAX, "%02X ", itmIout->s.aBytes[i]);
    }

    // As string
    pStr += _snprintf(pStr, STEMP_MAX, " String:\""/*, itmIout->nChannel, itmIout->nMany*/);
    pStr += _snprintf(pStr, itmIout->nMany, "%s", (char *)&itmIout->s.aBytes[0]);
    pStr += _snprintf(pStr, itmIout->nMany, "\"");
  }

  pStr += _snprintf(pStr, STEMP_MAX, "]");
  return (pStr);
}





char *CUVSC_Log::DecodeEvtrOut(char *pStr, EVTROUT *evtrOut, bool showData) {
//DWORD i;
  char *str = pStr;

  pStr += _snprintf(pStr, STEMP_MAX, "[(EVTROUT) ");
  pStr += _snprintf(pStr, STEMP_MAX, "Size: %d", evtrOut->nMany);

  int nMany = 0;
  EVTR_PACK *pack = &evtrOut->pack;
  do {
    nMany = pack->nMany;
    if(!nMany)    // last pack
      break;

    pStr += _snprintf(pStr, STEMP_MAX, "\n  ");

    switch(pack->nType) {
      case EVFMT_RAW: {
        RAW_EVENT *pR = (RAW_EVENT *)&pack->data;
        pStr += _snprintf(pStr, STEMP_MAX, "EvNR:=%lld, tStamp:=0x%016llX, TID:=0x%04X nVals=%d ",
                    pR->EvNumber, pR->tStamp, pR->id, pR->nValues);
        for ( UINT32 v = 0 ; v < pR->nValues ; v += 2 )  {
          pStr += _snprintf(pStr, STEMP_MAX, "VAL[%d,%d]=0x%08X, 0x%08X, ", v+0, v+1, pR->payload[v+0], pR->payload[v+1]);
        }
      } break;
      case EVFMT_DEC: {
        DEC_EVENT *pD = (DEC_EVENT *)&pack->data;
        pStr += _snprintf(pStr, STEMP_MAX, "EvNR:=%lld, tStamp:=0x%016llX, ", pD->EvNumber, pD->tStamp);
        pStr += _snprintf(pStr, STEMP_MAX, "Text: %s ", pD->szText);             // szText[] is zero terminated i nany case.
      } break;
    }

    pack = (EVTR_PACK *) ((char *)pack + 2 + nMany);    // 2: size of unsigned char
    if(strlen(str) > STEMP_MAX - 100)
      break;
  } while(nMany);

  pStr += _snprintf(pStr, STEMP_MAX, "]");
  return (pStr);
}


char *CUVSC_Log::DecodeItmRegister(char *pStr, ITMOUT *itmIO) {
  pStr += _snprintf(pStr, STEMP_MAX, "[(ITMOUT) ");
  pStr += _snprintf(pStr, STEMP_MAX, "Register Channel: %d", itmIO->nChannel);
  pStr += _snprintf(pStr, STEMP_MAX, "]");
  return (pStr);
}

char *CUVSC_Log::DecodeItmUnregister(char *pStr, ITMOUT *itmIO) {
  pStr += _snprintf(pStr, STEMP_MAX, "[(ITMOUT) ");
  pStr += _snprintf(pStr, STEMP_MAX, "Unregister Channel: %d", itmIO->nChannel);
  pStr += _snprintf(pStr, STEMP_MAX, "]");
  return (pStr);
}


char *CUVSC_Log::DecodeUVLicInfo(char *pStr, UVLICINFO *licinfo) {

  pStr += _snprintf(pStr, STEMP_MAX, "[(LICINFO) ");
  pStr += _snprintf(pStr, STEMP_MAX, "RV-MDK:%s", licinfo->rvmdk ? "licensed": "evaluation");
  pStr += _snprintf(pStr, STEMP_MAX, "]");
  return (pStr);
}

char *CUVSC_Log::DecodeDbgTrgOpt(char *pStr, DBGTGTOPT *dbgTrgOpt) {

  pStr += _snprintf(pStr, STEMP_MAX, "[(DBGTGTOPT) ");
  pStr += _snprintf(pStr, STEMP_MAX, "target:%s", dbgTrgOpt->target ? "Simulator": "Hardware");
  pStr += _snprintf(pStr, STEMP_MAX, "]");
  return (pStr);
}

char *CUVSC_Log::DecodeStackEnum(char *pStr, STACKENUM *stackEnum) {
  pStr += _snprintf(pStr, STEMP_MAX, "[(STACKENUM) ");
  pStr += _snprintf(pStr, STEMP_MAX, "nItem:%d nAdr:0x%08X nRetAdr:0x%08X", 
                    stackEnum->nItem, (DWORD)stackEnum->nAdr, (DWORD)stackEnum->nRetAdr);
  pStr += _snprintf(pStr, STEMP_MAX, "]");
  return (pStr);
}

char *CUVSC_Log::DecodeAflMap(char *pStr, ADRMTFL *adrFmtl) {
  pStr += _snprintf(pStr, STEMP_MAX, "[(ADRMTFL) ");
  pStr += _snprintf(pStr, STEMP_MAX, "bFull:%d nAdr:0x%08X", 
                    adrFmtl->bFull, (DWORD)adrFmtl->nAdr);

  pStr += _snprintf(pStr, STEMP_MAX, "]");
  return (pStr);
}

char *CUVSC_Log::DecodeAflMap(char *pStr, AFLMAP *aflMap) {
  pStr += _snprintf(pStr, STEMP_MAX, "[(AFLMAP) ");
  pStr += _snprintf(pStr, STEMP_MAX, "nLine:%d nAdr:0x%08X", 
                    aflMap->nLine, (DWORD)aflMap->nAdr);


  if (aflMap->iFile) {
    pStr += _snprintf(pStr, STEMP_MAX, " File:%s", &aflMap->szFile[aflMap->iFile]);
  }
  if (aflMap->iFunc) {
    pStr += _snprintf(pStr, STEMP_MAX, " Func:%s", &aflMap->szFile[aflMap->iFunc]);
  }

  pStr += _snprintf(pStr, STEMP_MAX, "]");
  return (pStr);
}

char *CUVSC_Log::DecodeIShowSync(char *pStr, iSHOWSYNC *iShowSync) {
  pStr += _snprintf(pStr, STEMP_MAX, "[(iSHOWSYNC) ");
  pStr += _snprintf(pStr, STEMP_MAX, "nAdr:0x%08X bAsm:%d bHll:%d bAsmRes:%d bHllRes:%d ", 
                    (DWORD)iShowSync->nAdr, iShowSync->bAsm, iShowSync->bHll, iShowSync->bAsmRes, iShowSync->bHllRes);

  pStr += _snprintf(pStr, STEMP_MAX, "]");
  return (pStr);
}

char *CUVSC_Log::DecodeAVtr(char *pStr, AVTR *iAVtr) {
  pStr += _snprintf(pStr, STEMP_MAX, "[(AVTR) ");
  pStr += _snprintf(pStr, STEMP_MAX, "bValue:%d vtrType:%s vtrFrq:%d vtrClock:%d vtrNo:%d value:", 
                    iAVtr->bValue, UVSCVTTTypeText((VTT_TYPE)iAVtr->vtrType), iAVtr->vtrFrq, iAVtr->vtrClock, iAVtr->vtrNo);
  pStr = DecodeTVal(pStr, &iAVtr->value);
  pStr += _snprintf(pStr, STEMP_MAX, " szName:%s", iAVtr->szName);
  pStr += _snprintf(pStr, STEMP_MAX, "]");
  return (pStr);
}

char *CUVSC_Log::DecodeIVtrEnum(char *pStr, iVTRENUM *iVtrEnum) {
  pStr += _snprintf(pStr, STEMP_MAX, "[(iVTRENUM) ");
  pStr += _snprintf(pStr, STEMP_MAX, "bValue:%d", iVtrEnum->bValue);
  pStr += _snprintf(pStr, STEMP_MAX, "]");
  return (pStr);
}

char *CUVSC_Log::DecodeBpReason(char *pStr, BPREASON *bpReason) {
  pStr += _snprintf(pStr, STEMP_MAX, "[(BPREASON) ");
  pStr += _snprintf(pStr, STEMP_MAX, "eReason:%s nPC:0x%08X nAdr:0x%08X nBpNum:%d nTickMark:0x%08X ", 
                    UVSCStopReasonText(bpReason->eReason), (DWORD)bpReason->nPC, (DWORD)bpReason->nAdr, bpReason->nBpNum, bpReason->nTickMark);
  pStr += _snprintf(pStr, STEMP_MAX, "]");
  return (pStr);
}

char *CUVSC_Log::DecodeIPathReq(char *pStr, iPATHREQ *iPathReq) {
  pStr += _snprintf(pStr, STEMP_MAX, "[(iPATHREQ) ");
  pStr += _snprintf(pStr, STEMP_MAX, "bFull:%d", iPathReq->bFull);

  pStr += _snprintf(pStr, STEMP_MAX, "]");
  return (pStr);
}


char *CUVSC_Log::DecodePowerScale_ShowCode(char *pStr, UVSC_PSTAMP *pPowerScaleData) {
  pStr += _snprintf(pStr, STEMP_MAX, "[(PowerScale) ");
  pStr += _snprintf(pStr, STEMP_MAX, "Show Code: nAdr:0x%08X ticks:%I64d delta:%g time: %g showSyncErr %d ", 
    (DWORD)pPowerScaleData->nAdr, pPowerScaleData->ticks, pPowerScaleData->delta, pPowerScaleData->time, pPowerScaleData->showSyncErr);

  pStr += _snprintf(pStr, STEMP_MAX, "]");
  return (pStr);
}


char *CUVSC_Log::DecodePowerScale_ShowPower(char *pStr, UVSC_PSTAMP *pPowerScaleData) {
  pStr += _snprintf(pStr, STEMP_MAX, "[(PowerScale) ");
  pStr += _snprintf(pStr, STEMP_MAX, "Show Code: nAdr:0x%08X ticks:%I64d delta:%g time: %g showSyncErr %d ", 
    (DWORD)pPowerScaleData->nAdr, pPowerScaleData->ticks, pPowerScaleData->delta, pPowerScaleData->time, pPowerScaleData->showSyncErr);
  pStr += _snprintf(pStr, STEMP_MAX, "]");
  return (pStr);
}


char *CUVSC_Log::DecodeCmdResponse(char *pStr, UVSOCK_CMD_RESPONSE *rsp, UINT len) {

  switch (rsp->status) {
    case UV_STATUS_SUCCESS:
      switch (rsp->cmd) {
        case UV_PRJ_GET_OPTITEM:
          pStr = DecodeTrnOpt(pStr, &rsp->trnopt);
          break;

        case UV_DBG_CALC_EXPRESSION:
        case UV_DBG_VTR_GET:       
          pStr = DecodeVSet(pStr, &rsp->vset);
          break;

        case UV_DBG_MEM_READ:
        case UV_DBG_MEM_WRITE:
        case UV_DBG_DSM_READ:
          pStr = DecodeAMem(pStr, &rsp->amem, (rsp->cmd == UV_DBG_MEM_READ));
          break;

//        case UV_DBG_TIME_INFO:
//          break;

//        case UV_DBG_SET_CALLBACK:
//          break;

  
        case UV_DBG_SERIAL_GET:        
          pStr = DecodeSerIO(pStr, &rsp->serdat, true);
          break;
        
        case UV_DBG_SERIAL_PUT:
          pStr = DecodeSerIO(pStr, &rsp->serdat, false);
          break;
//        case UV_DBG_ENUM_SYMTP:
//          break;

        case UV_DBG_ITM_REGISTER:
          pStr = DecodeItmRegister(pStr, &rsp->itmdat);
          break;

        case UV_DBG_ITM_UNREGISTER:
          pStr = DecodeItmUnregister(pStr, &rsp->itmdat);
          break;

        case UV_PRJ_ACTIVE_FILES:
          pStr = DecodePrjActiveFiles(pStr, rsp->nVal);
          break;

        case UV_GEN_GET_VERSION:
          pStr = DecodeGenGetVersion(pStr, rsp->nVal);
          break;

        case UV_GEN_CHECK_LICENSE:
          pStr = DecodeUVLicInfo(pStr, &rsp->licinfo);
          break;

        case UV_DBG_STATUS:
          pStr = DecodeDbgStatus(pStr, rsp->nVal);
          break;

        case UV_PRJ_GET_DEBUG_TARGET:
          pStr = DecodeDbgTrgOpt(pStr, &rsp->dbgtgtopt);
          break;

        case UV_DBG_ADR_TOFILELINE:
          pStr = DecodeAflMap(pStr, &rsp->aflm);
          break;
        
//        case UV_DBG_ADR_SHOWCODE:
//          pStr = DecodeIShowSync(pStr, (iSHOWSYNC *)&rsp->nVal);
//          break;
          
        case UV_PRJ_GET_OUTPUTNAME:
        case UV_PRJ_SET_OUTPUTNAME:
        case UV_PRJ_GET_CUR_TARGET:
          pStr = DecodeSstr(pStr, &rsp->str);
          break;


        case UV_PRJ_LOAD:
        case UV_PRJ_ADD_GROUP:
        case UV_PRJ_DEL_GROUP:
        case UV_PRJ_SET_TARGET:
        case UV_PRJ_ADD_FILE:
        case UV_PRJ_DEL_FILE:
        case UV_PRJ_ENUM_GROUPS:
        case UV_PRJ_ENUM_TARGETS:
        case UV_PRJ_ENUM_FILES:
        case UV_GEN_UI_UNLOCK:
        case UV_GEN_UI_LOCK:
        case UV_GEN_CPLX_COMPLETE:
        case UV_GEN_HIDE:
        case UV_GEN_SHOW:
        case UV_GEN_RESTORE:
        case UV_GEN_MINIMIZE:
        case UV_GEN_MAXIMIZE:
        case UV_GEN_EXIT:          
        //case UV_GEN_GET_EXTVERSION:
        case UV_PRJ_CLOSE:
        case UV_PRJ_BUILD:
        case UV_PRJ_REBUILD:
        case UV_PRJ_CLEAN:
        case UV_PRJ_BUILD_CANCEL:
        case UV_PRJ_FLASH_DOWNLOAD:
        case UV_PRJ_SET_OPTITEM:
        case UV_PRJ_SET_DEBUG_TARGET:
        //case UV_PRJ_CMD_PROGRESS:        
        case UV_DBG_ENTER:
        case UV_DBG_EXIT:
        case UV_DBG_START_EXECUTION:
        case UV_DBG_STOP_EXECUTION:        
        case UV_DBG_RESET:
        case UV_DBG_STEP_HLL:
        case UV_DBG_STEP_INTO:
        case UV_DBG_STEP_INSTRUCTION:
        case UV_DBG_STEP_OUT:
        case UV_DBG_ADR_SHOWCODE:
        case UV_DBG_POWERSCALE_SHOWCODE:
        case UV_DBG_POWERSCALE_SHOWPOWER:
        case UV_DBG_ENUMERATE_BP:
        case UV_DBG_ENUM_STACK:
        case UV_DBG_ENUM_VTR:
        case UV_DBG_CREATE_BP:
        case UV_DBG_CHANGE_BP:
        case UV_DBG_VTR_SET:
        case UV_DBG_EXEC_CMD:
        case UV_DBG_EVTR_REGISTER:
        case UV_DBG_EVTR_UNREGISTER:
        case UV_DBG_EVTR_ENUMSCVDFILES:
        //case UV_DBG_VERIFY_CODE:
          pStr += _snprintf(pStr, STEMP_MAX, "[OK]");
          break;

        default:
          pStr += _snprintf(pStr, STEMP_MAX, "[???]");
      }
      break;

      default:
        pStr = DecodeError(pStr, rsp);
  }

  return (pStr);
}

char *CUVSC_Log::DecodeAsyncResponse(char *pStr, UVSOCK_CMD_RESPONSE *rsp, UINT len) {

  switch (rsp->status) {
    case UV_STATUS_SUCCESS:
      switch (rsp->cmd) {

        case UV_DBG_STOP_EXECUTION:
          pStr = DecodeBpReason(pStr, &rsp->StopR);
          break;
        
        case UV_PRJ_FLASH_DOWNLOAD:
        case UV_DBG_ENTER:
        case UV_DBG_EXIT:
        case UV_DBG_START_EXECUTION:        
        case UV_DBG_RESET:
          pStr += _snprintf(pStr, STEMP_MAX, "[OK]");
          break;

        case UV_PRJ_LOAD:
        case UV_PRJ_CLOSE:
        case UV_PRJ_PBAR_INIT:
        case UV_PRJ_PBAR_STOP:
        case UV_PRJ_PBAR_SET:
        case UV_PRJ_PBAR_TEXT:
        case UV_DBG_EXEC_CMD:
          pStr = DecodeError(pStr, rsp);
          break;

        case UV_DBG_POWERSCALE_SHOWPOWER:
          pStr = DecodePowerScale_ShowPower(pStr, &rsp->powerScaleData);
          break;

        default:
          pStr += _snprintf(pStr, STEMP_MAX, "[???]");
        }
        break;
    
    default:
      pStr = DecodeError(pStr, rsp);
  }

  return (pStr);
}

char *CUVSC_Log::DecodeMessageData(char *pStr, UVSOCK_CMD *msg, Direction dir){

  // Decode data based on the message type / direction
  switch (dir) {
    case Req:
      switch (msg->m_eCmd) {       
        case UV_PRJ_LOAD:
        case UV_PRJ_ADD_GROUP:
        case UV_PRJ_SET_TARGET:
        case UV_PRJ_DEL_GROUP:
        case UV_PRJ_ADD_FILE:
        case UV_PRJ_DEL_FILE:
          pStr = DecodePrjData(pStr, &msg->data.prjdata);
          break;

        case UV_PRJ_GET_OPTITEM:
        case UV_PRJ_SET_OPTITEM:
          pStr = DecodeTrnOpt(pStr, &msg->data.trnopt);
          break;

        case UV_PRJ_ENUM_FILES:
        case UV_DBG_EXEC_CMD:
          pStr = DecodeExecCmd(pStr, &msg->data.execcmd);
          break;

        case UV_DBG_CALC_EXPRESSION:
        case UV_DBG_VTR_GET:
        case UV_DBG_VTR_SET:
          pStr = DecodeVSet(pStr, &msg->data.vset);
          break;

        case UV_DBG_MEM_READ:
        case UV_DBG_MEM_WRITE:
        case UV_DBG_DSM_READ:
          pStr = DecodeAMem(pStr, &msg->data.amem, (msg->m_eCmd == UV_DBG_MEM_WRITE));
          break;

//        case UV_DBG_TIME_INFO:
//          break;

//        case UV_DBG_SET_CALLBACK:
//          break;

  
        case UV_DBG_SERIAL_GET:
          pStr = DecodeSerIO(pStr, &msg->data.serdat, false);
          break;

        case UV_DBG_SERIAL_PUT:
          pStr = DecodeSerIO(pStr, &msg->data.serdat, true);
          break;

        case UV_DBG_ITM_REGISTER:
          pStr = DecodeItmRegister(pStr, &msg->data.itmdat);
          break;

        case UV_DBG_ITM_UNREGISTER:
          pStr = DecodeItmUnregister(pStr, &msg->data.itmdat);
          break;

        case UV_DBG_CREATE_BP:
          pStr = DecodeBkParm(pStr, &msg->data.bkparm);
          break;

        case UV_DBG_CHANGE_BP:
          pStr = DecodeBkChg(pStr, &msg->data.bkchg);
          break;

        case UV_PRJ_SET_DEBUG_TARGET:
          pStr = DecodeDbgTrgOpt(pStr, &msg->data.dbgtgtopt);
          break;

        case UV_PRJ_GET_OUTPUTNAME:
        case UV_PRJ_GET_CUR_TARGET:
        case UV_PRJ_SET_OUTPUTNAME:
          pStr = DecodeIPathReq(pStr, &msg->data.iPathReq);
          break;

//        case UV_DBG_ENUM_SYMTP:
//          break;

        case UV_DBG_ADR_TOFILELINE:
          pStr = DecodeAflMap(pStr, &msg->data.adrmtfl);
          break;

        case UV_DBG_ADR_SHOWCODE:
          pStr = DecodeIShowSync(pStr, &msg->data.ishowsync);
          break;

        case UV_DBG_POWERSCALE_SHOWCODE:
          pStr = DecodePowerScale_ShowCode(pStr, (UVSC_PSTAMP *)&msg->data.raw);
          break;


        case UV_DBG_ENUM_VTR:
          pStr = DecodeIVtrEnum(pStr, &msg->data.ivtrenum);
          break;

        case UV_GEN_GET_VERSION:
        case UV_GEN_UI_UNLOCK:
        case UV_GEN_UI_LOCK:
        case UV_GEN_CPLX_COMPLETE:
        case UV_GEN_HIDE:
        case UV_GEN_SHOW:
        case UV_GEN_RESTORE:
        case UV_GEN_MINIMIZE:
        case UV_GEN_MAXIMIZE:
        case UV_GEN_EXIT:          
        //case UV_GEN_GET_EXTVERSION:
        case UV_GEN_CHECK_LICENSE:
        case UV_PRJ_CLOSE:
        case UV_PRJ_BUILD:
        case UV_PRJ_REBUILD:
        case UV_PRJ_CLEAN:
        case UV_PRJ_BUILD_CANCEL:
        case UV_PRJ_FLASH_DOWNLOAD:
        case UV_PRJ_ENUM_GROUPS:
        case UV_PRJ_ENUM_TARGETS:
        case UV_PRJ_GET_DEBUG_TARGET:
        //case UV_PRJ_CMD_PROGRESS:
        case UV_PRJ_ACTIVE_FILES:
        case UV_DBG_ENUMERATE_BP:
        case UV_DBG_ENUM_STACK:
        case UV_DBG_EVTR_ENUMSCVDFILES:
        case UV_DBG_ENTER:
        case UV_DBG_EXIT:
        case UV_DBG_START_EXECUTION:
        case UV_DBG_STOP_EXECUTION:
        case UV_DBG_STATUS:
        case UV_DBG_RESET:
        case UV_DBG_STEP_HLL:
        case UV_DBG_STEP_INTO:
        case UV_DBG_STEP_INSTRUCTION:
        case UV_DBG_STEP_OUT:           
        case UV_DBG_EVTR_REGISTER:           
        case UV_DBG_EVTR_UNREGISTER:           
        //case UV_DBG_VERIFY_CODE:
        case UV_DBG_POWERSCALE_SHOWPOWER:
          break;

        default:
          pStr += _snprintf(pStr, STEMP_MAX, "[???]");
        }
      break;

    case Rsp:
    case Async:
      switch (msg->m_eCmd) {
        case UV_CMD_RESPONSE:
          pStr = DecodeCmdResponse(pStr, &msg->data.cmdRsp, msg->m_nBufLen);
          break;

        case UV_ASYNC_MSG:
          pStr = DecodeAsyncResponse(pStr, &msg->data.cmdRsp, msg->m_nBufLen);
          break;

        case UV_DBG_SERIAL_OUTPUT:
          pStr = DecodeSerIO(pStr, &msg->data.serdat, true);
          break;

        case UV_DBG_ITM_OUTPUT:
          pStr = DecodeItmOut(pStr, &msg->data.itmdat, true);
          break;

        case UV_DBG_EVTR_OUTPUT:
          pStr = DecodeEvtrOut(pStr, &msg->data.evtrOut, true);
          break;

        case UV_PRJ_BUILD_COMPLETE:
          pStr = DecodePrjData(pStr, (PRJDATA *)&msg->data);
          break;

        case UV_PRJ_BUILD_OUTPUT:
          pStr = DecodePrjData(pStr, (PRJDATA *)&msg->data);
          break;

        case UV_DBG_CMD_OUTPUT:
          pStr = DecodeSstr(pStr, (SSTR *)&msg->data);
          break;

        case UV_DBG_CHANGE_BP:
//        msg->data.cmdRsp.status;
//        pStr = DecodeBkRsp(pStr, &msg->data.cmdRsp.brk);
//---Hp. /22.7.2009/: need to look also at cmdRsp.status to foind out correct Bp-action:
          pStr = DecodeBkRsp(pStr, &msg->data.cmdRsp.brk, msg->data.cmdRsp.status);
//-------------------
          break;

        case UV_DBG_BP_ENUMERATED:
          pStr = DecodeBkRsp(pStr, &msg->data.cmdRsp.brk);
          break;

        case UV_PRJ_ENUM_GROUPS_ENU:
        case UV_PRJ_ENUM_TARGETS_ENU:
        case UV_PRJ_ENUM_FILES_ENU:
        case UV_DBG_EVTR_ENUMSCVDFILES_ENU:
          pStr = DecodeSstr(pStr, &msg->data.cmdRsp.str);
          break;

        //case UV_DBG_ENUM_SYMTP_ENU:
        //  pStr = DecodePrjSymtpEnu(msg->data.cmdRsp, msg->m_nBufLen);
        //  break;

        case UV_DBG_ENUM_STACK_ENU:
          pStr = DecodeStackEnum(pStr, &msg->data.cmdRsp.stack);
          break;

        case UV_DBG_ENUM_VTR_ENU:
          pStr = DecodeAVtr(pStr, &msg->data.cmdRsp.vtr);
          break;

        //case UV_RTA_MESSAGE:
        //  pStr = DecodeRtaMessage(msg->data.cmdRsp, msg->m_nBufLen);
        //  break;

        //case UV_RTA_INCOMPATIBLE:
        //  pStr = DecodeRtaIncompatible(msg->data.cmdRsp, msg->m_nBufLen);
        //  break;
        
        case UV_DBG_BP_ENUM_START:        
        case UV_DBG_BP_ENUM_END:
        case UV_PRJ_ENUM_GROUPS_START:
        case UV_PRJ_ENUM_GROUPS_END:
        case UV_PRJ_ENUM_TARGETS_START:
        case UV_PRJ_ENUM_TARGETS_END:
        case UV_PRJ_ENUM_FILES_START:
        case UV_PRJ_ENUM_FILES_END:
        case UV_DBG_EVTR_ENUMSCVDFILES_START:
        case UV_DBG_EVTR_ENUMSCVDFILES_END:
        case UV_DBG_ENUM_SYMTP_START:
        case UV_DBG_ENUM_SYMTP_END:
        case UV_DBG_ENUM_STACK_START:
        case UV_DBG_ENUM_STACK_END:
        case UV_DBG_ENUM_VTR_START:
        case UV_DBG_ENUM_VTR_END:
        case UV_DBG_CALLBACK: 
        case UV_DBG_EXEC_CMD:
          pStr += _snprintf(pStr, STEMP_MAX, "[OK]");
          break;

        default:
          pStr += _snprintf(pStr, STEMP_MAX, "[???]");
      }

      break;

    default:
      pStr += _snprintf(pStr, STEMP_MAX, "[???]");
  }
  
  return (pStr);
}

char *CUVSC_Log::DecodeHeader(char *pStr, UVSOCK_CMD *msg, Direction dir) {

  // Write the (truncated) header
  if ((msg->m_eCmd != UV_CMD_RESPONSE) && (msg->m_eCmd != UV_ASYNC_MSG)) {
    pStr += _snprintf(pStr, STEMP_MAX, " [%s] ", RspCmd(msg->m_eCmd));
  }
  else {
    pStr += _snprintf(pStr, STEMP_MAX, " [%s] ", RspCmd(msg->data.cmdRsp.cmd));
  }

  return (pStr);
}

char *CUVSC_Log::DecodeDirection(char *pStr, Direction dir) {
  const char* pDirStr;
  DWORD currentTime, hrs, mins, secs, msecs;

  switch (dir) {
    case Req:
      pDirStr = "-->";
      break;
    case Rsp:
      pDirStr = "<--";
      break;
    case Async:
      pDirStr = "<==";
      break;
    case RTA:
      pDirStr = "RTA";
      break;
    default:
      pDirStr = "???";        
  }
  // Write the time and direction
  currentTime = GetTickCount() - m_TimeZero;
  hrs = currentTime / 3600000;
  currentTime -= hrs * 3600000;
  mins = currentTime / 60000;
  currentTime -= mins * 60000;
  secs = currentTime / 1000;
  currentTime -= secs * 1000;
  msecs = currentTime / 10;
  pStr += _snprintf(pStr, STEMP_MAX, "\n%02d:%02d:%02d.%02d [%s]", 
                    hrs ,       // Hours
                    mins ,      // Minutes
                    secs ,      // Seconds
                    msecs ,     // Milliseconds
                    pDirStr);
  
  return (pStr);
}

char *CUVSC_Log::DecodeMessage(char *pStr, UVSOCK_CMD *msg, Direction dir) {
  pStr = DecodeDirection(pStr, dir);
  pStr = DecodeHeader(pStr, msg, dir); 
  pStr = DecodeMessageData(pStr, msg, dir);
  return (pStr);
}


char *CUVSC_Log::DecodeRaw(char *pStr, UVSOCK_CMD *msg, Direction dir) {
  UINT i, len;

  /* Even in RAW, decode dir and header, as they are important for understanding! */
  pStr = DecodeDirection(pStr, dir);
  pStr = DecodeHeader(pStr, msg, dir);

#if 0
  // Write the header
  pStr += _snprintf(pStr, STEMP_MAX, "[%08X %08X %08X %016X %020lf %08X] ", 
                    msg->m_nTotalLen,
                    msg->m_eCmd,
                    msg->m_nBufLen,
                    msg->cycles,
                    msg->tStamp,
                    msg->m_Id);      
#else
  // Write the (truncated) header (we don't really care about the other stuff)
  pStr += _snprintf(pStr, STEMP_MAX, "[%08X %08X %08X] ", 
                    msg->m_nTotalLen,
                    msg->m_eCmd,
                    msg->m_nBufLen);
#endif

  // Write Data
  pStr[0] = '\0';
  len = msg->m_nBufLen / 4;
  for (i=0; i<len ;i++) {
    pStr += _snprintf(pStr, STEMP_MAX, "%08X ", *((UINT *)&msg->data.raw[i*4]));
  }
  for (i=i*4; i<msg->m_nBufLen ;i++) {
    pStr += _snprintf(pStr, STEMP_MAX, "%02X ", msg->data.raw[i]);
  }

  return (pStr);
}




char *CUVSC_Log::DecodeTraceStatus(char *pStr, UVSC_STATUS status, TraceType type) {

  switch (type) {
    case Enter:
      ; // Do Nothing
      break;
    case Exit:
      pStr += _snprintf(pStr, STEMP_MAX, " = %s", UVSCStatusText(status));
      break;
    default:
      ; // Do Nothing
  }
  // Write the message
  
  
  return (pStr);
}

char *CUVSC_Log::DecodeTraceMessage(char *pStr, const char *msg, TraceType type) {
  const char* pFormatStr;

  switch (type) {
    case Enter:
      pFormatStr = " %s()";
      break;
    case Exit:
      pFormatStr = " %s()";
      break;
    default:
      pFormatStr = " %s";        
  }
  // Write the message
  pStr += _snprintf(pStr, STEMP_MAX, pFormatStr, msg);
  
  return (pStr);
}


char *CUVSC_Log::DecodeTraceType(char *pStr, TraceType type) {
  const char* pTypeStr;
  DWORD currentTime, hrs, mins, secs, msecs;

  switch (type) {
    case Enter:
      pTypeStr = "T->";
      break;
    case Exit:
      pTypeStr = "<-T";
      break;
    default:
      pTypeStr = "???";        
  }
  // Write the time and direction
  currentTime = GetTickCount() - m_TimeZero;
  hrs = currentTime / 3600000;
  currentTime -= hrs * 3600000;
  mins = currentTime / 60000;
  currentTime -= mins * 60000;
  secs = currentTime / 1000;
  currentTime -= secs * 1000;
  msecs = currentTime / 10;
  pStr += _snprintf(pStr, STEMP_MAX, "\n%02d:%02d:%02d.%02d [%s]", 
                    hrs ,       // Hours
                    mins ,      // Minutes
                    secs ,      // Seconds
                    msecs ,     // Milliseconds
                    pTypeStr);
  
  return (pStr);
}

char *CUVSC_Log::DecodeTrace(char *pStr, const char *msg, TraceType type, UVSC_STATUS status) {
  pStr = DecodeTraceType(pStr, type);
  pStr = DecodeTraceMessage(pStr, msg, type);  
  pStr = DecodeTraceStatus(pStr, status, type);
  return (pStr);
}
