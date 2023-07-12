// Add relevant include files
# include "_Plugin_Helper.h"
# include "src/PluginStructs/P153_data_struct.h"
// # include <Regexp.h>

// Add relevent #defines 
#define PLUGIN_153
#define PLUGIN_ID_153 153 // plugin id
#define PLUGIN_NAME_153 "Communication - Hosea"

# define P153_BAUDRATE           PCONFIG_LONG(0)
# define P153_BAUDRATE_LABEL     PCONFIG_LABEL(0)
# define P153_SERIAL_CONFIG      PCONFIG_LONG(1)

# define P153_QUERY_VALUE        0 // HOLDON: Temp placement holder until we know what selectors are needed.
# define P153_NR_OUTPUT_OPTIONS  1

# define P153_NR_OUTPUT_VALUES   1
# define P153_QUERY1_CONFIG_POS  3 // HOLDON: what is this

# define P153_DEFAULT_BAUDRATE   19200

// Add relevant global variables


boolean Plugin_153(uint8_t function, struct EventStruct *event, String& string)
{
	boolean success = false;
	switch(function)
	{
		case PLUGIN_DEVICE_ADD:
		{
            Device[++deviceCount].Number           = PLUGIN_ID_153;
            Device[deviceCount].Type               = DEVICE_TYPE_SERIAL; //Must change
            Device[deviceCount].VType              = Sensor_VType::SENSOR_TYPE_STRING; //Must Change
            Device[deviceCount].Ports              = 0;
            Device[deviceCount].ValueCount         = 1; // Must change
            Device[deviceCount].PullUpOption       = false;
            Device[deviceCount].InverseLogicOption = false;
            Device[deviceCount].FormulaOption      = false; // Must consider in final product
            Device[deviceCount].Custom             = false; // IDK. Cannot use formula if enabled
            Device[deviceCount].SendDataOption     = true; // Must change
            Device[deviceCount].TimerOption        = true; // must consider/change
            Device[deviceCount].GlobalSyncOption   = false;
			Device[deviceCount].ExitTaskBeforeSave = false;
			break;
		} // end PLUGIN_DEVICE_ADD
	
		case PLUGIN_GET_DEVICENAME:
		{
            string = F(PLUGIN_NAME_153);
			break;
		} // end PLUGIN_GET_DEVICENAME
	
		case PLUGIN_GET_DEVICEVALUENAMES:
		{
			for (uint8_t i = 0; i < VARS_PER_TASK; ++i) {
				if (i < P153_NR_OUTPUT_VALUES) {
					//const uint8_t pconfigIndex = i + P153_QUERY1_CONFIG_POS;
					//uint8_t choice             = PCONFIG(pconfigIndex);
					safe_strncpy(
						ExtraTaskSettings.TaskDeviceValueNames[i],
						F("Value"),
						sizeof(ExtraTaskSettings.TaskDeviceValueNames[i]));
				} else {
					ZERO_FILL(ExtraTaskSettings.TaskDeviceValueNames[i]);
				}
			}
			break;
		} // end PLUGIN_GET_DEVICEVALUENAMES

		case PLUGIN_GET_DEVICEGPIONAMES:
		{
			serialHelper_getGpioNames(event, false, true); // TX is optional
			break;
		} // end PLUGIN_GET_DEVICEGPIONAMES:

		case PLUGIN_WEBFORM_SHOW_VALUES:
		{
			// Create plugin data struct
			P153_data_struct *P153_data = 
				static_cast<P153_data_struct *>(getPluginTaskData(event->TaskIndex));

			// if data struct successfully created and initialized
			// // Show success, Error, Length Last statistics
			if ((nullptr != P153_data) && P153_data->isInitialized())
			{
				uint32_t success, error, length_last;
				P153_data->getSentencesReceived(success, error, length_last);
				uint8_t varNr = VARS_PER_TASK;
				pluginWebformShowValue(event->TaskIndex, varNr++, F("Success"),     String(success));
				pluginWebformShowValue(event->TaskIndex, varNr++, F("Error"),       String(error));
				pluginWebformShowValue(event->TaskIndex, varNr++, F("Length Last"), String(length_last), true);
			}

			break;
		} // end PLUGIN_WEBFORM_SHOW_VALUES

		case PLUGIN_SET_DEFAULTS:
		{
			P153_BAUDRATE = P153_DEFAULT_BAUDRATE;

			success = true;
			break;
		} // end PLUGIN_SET_DEFAULTS

		case PLUGIN_WEBFORM_SHOW_CONFIG:
		{
			string += serialHelper_getSerialTypeLabel(event);
			success = true;
			break;
		} // end PLUGIN_WEBFORM_SHOW_CONFIG

		case PLUGIN_WEBFORM_SHOW_SERIAL_PARAMS:
		{
			addFormNumericBox(F("Baudrate"), P153_BAUDRATE_LABEL, P153_BAUDRATE, 300, 115200);
      		addUnit(F("baud"));
      		uint8_t serialConfChoice = serialHelper_convertOldSerialConfig(P153_SERIAL_CONFIG);
      		serialHelper_serialconfig_webformLoad(event, serialConfChoice);
      		break;
		} // end PLUGIN_WEBFORM_SHOW_SERIAL_PARAMS

		case PLUGIN_WEBFORM_LOAD:
		{
			addFormSubHeader(F("Select MPPT Fields to Read")); 
			P153_html_show_user_label_forms(event);

			addFormSubHeader(F("Statistics")); // TODO: in plugin data struct, change html code
			P153_html_show_stats(event);

			success = true;
			break;
		} // end PLUGIN_WEBFORM_LOAD
	
		case PLUGIN_WEBFORM_SAVE:
		{
			P153_BAUDRATE = getFormItemInt(P153_BAUDRATE_LABEL);
			P153_SERIAL_CONFIG = serialHelper_serialconfig_webformSave();

			// create plugin data struct ptr
			P153_data_struct *P153_data =
        		static_cast<P153_data_struct *>(getPluginTaskData(event->TaskIndex));
			
			// EISH:
			// if ptr successfully created
			// // add web args for each line_Nr
			// // HOLDON: Add something called HtmlError????
			if (nullptr != P153_data) {
			for (uint8_t line_Nr = 0; line_Nr < P153_NR_LINES; line_Nr++)
			{
				P153_data->setLine(line_Nr, webArg(getPluginCustomArgName(line_Nr)));
			}

				addHtmlError(SaveCustomTaskSettings(event->TaskIndex, P153_data->_lines, P153_NR_LINES, 0));
				success = true;
			}

			break;
		} // end PLUGIN_WEBFORM_SAVE

		case PLUGIN_INIT:
		{
			const int16_t serial_rx = CONFIG_PIN1;
			const int16_t serial_tx = CONFIG_PIN2;
			const ESPEasySerialPort port = static_cast<ESPEasySerialPort>(CONFIG_PORT);

			// using some function from datastruct file, initialise Task data
			// creat plugin datastruct ptr
			initPluginTaskData(event->TaskIndex, new (std::nothrow) P153_data_struct());
			P153_data_struct *P153_data =
				static_cast<P153_data_struct *>(getPluginTaskData(event->TaskIndex));

			// if ptr is null
			// // return plugin function !!!!
			if (nullptr == P153_data) 
			{
        		return success;
      		}

			// if plugin datastruct initialization successfull
			// // Add custom Task Settings
			// // run post init()
			// // set success = true
			// // log Gpio Description
			// else
			// // Clear Plugin Task Data
			if (P153_data->init(port, serial_rx, serial_tx, P153_BAUDRATE, static_cast<uint8_t>(P153_SERIAL_CONFIG))) {
				LoadCustomTaskSettings(event->TaskIndex, P153_data->_lines, P153_NR_LINES, 0);
				//P087_data->post_init();
				success = true;
				serialHelper_log_GpioDescription(port, serial_rx, serial_tx);
			} 
			else 
			{
				clearPluginTaskData(event->TaskIndex);
			}

			break;

		} // end PLUGIN_INIT

		case PLUGIN_TEN_PER_SECOND: //+++
		{
			// if TaskEnabled
			// // Creat plugin datastruct
			// // if data struct successfully created
			// // // Use Scheduler to start timer to process received sentences
			if (Settings.TaskDeviceEnabled[event->TaskIndex]) 
			{
				P153_data_struct *P153_data =
					static_cast<P153_data_struct *>(getPluginTaskData(event->TaskIndex));

				if ((nullptr != P153_data) && P153_data->loop()) 
				{
					Scheduler.schedule_task_device_timer(event->TaskIndex, millis() + 10);
					delay(0); // Processing a full sentence may take a while, run some
							// background tasks.
				}
				else
				{
					//event->String2 += "555";
				}
				
				success = true;
			}

			break;
		}
	
		case PLUGIN_READ:
		{
			// create plugn datastruct ptr
			P153_data_struct *P153_data =
        		static_cast<P153_data_struct *>(getPluginTaskData(event->TaskIndex));

			// if datastructs created and sentence is available in event->string2
			// // if "matchall" successful
			// // // LOG event>string2
			if ((nullptr != P153_data) && P153_data->getSentence(event->String2))
			{
				//  my string is stored in event->String2
# ifndef BUILD_NO_DEBUG
				addLog(LOG_LEVEL_DEBUG, event->String2);
# endif // infef BUILD_NO_DEBUG
				success = true;
			}
			
			break;
		} // end PLUGIN_READ
	
		case PLUGIN_WRITE:
		{
			// create plugn datastruct ptr
			P153_data_struct *P153_data =
        		static_cast<P153_data_struct *>(getPluginTaskData(event->TaskIndex));

			// if datastruct created
			// // get command from string
			// // if mommand == Hosea_write
			// // // Transmit data
			// // else if command ==  Hosea_writemx
			// // // Transmit data in Hex form
			if ((nullptr != P153_data)) {
				String cmd = parseString(string, 1);

				if (equals(cmd, F("hosea_write"))) 
				{
					// It seems this should work with newline???
					// Otherswise concatenate a new line in the code
					String no_nl_param1 = parseStringKeepCase(string, 2, ',', false); // Don't trim off white-space
					String param1 = no_nl_param1 + '\n';
					parseSystemVariables(param1, false);                        // FIXME tonhuisman: Doesn't seem to be needed?
					P153_data->sendString(param1);
					addLogMove(LOG_LEVEL_INFO, param1);                         // FIXME tonhuisman: Should we always want to write to the log?
					success = true;
				} 
				else if (equals(cmd, F("hosea_writemix"))) 
				{
					std::vector<uint8_t> param1 = parseHexTextData(string);
					P153_data->sendData(&param1[0], param1.size());
					success = true;
				}
			}

			break;
		} // end PLUGIN_WRITE

	} // end case
    
    return success;
} // end function


// plugin match all function
// // return: Plugin datastruct->matchRegexp(received)

// plugin html show match forms function
// // Here we add the Regex text box

// plugin show html statistics
// // from plugin datastruct get values for success, error, length last values 
// // on devices page, show success, error, length last values 
void P153_html_show_stats(struct EventStruct *event) 
{
	P153_data_struct *P153_data =
	static_cast<P153_data_struct *>(getPluginTaskData(event->TaskIndex));

	if ((nullptr == P153_data) || !P153_data->isInitialized()) 
	{
		return;
	}
	{
		addRowLabel(F("Current Sentence"));
		String sentencePart;
		if(P153_data->getSentence(sentencePart))
		{
			addHtml(sentencePart);
		}
	}

	{
		addRowLabel(F("Sentences (pass/fail)"));
		String   chksumStats;
		uint32_t success, error, length_last;
		P153_data->getSentencesReceived(success, error, length_last);
		chksumStats  = success;
		chksumStats += '/';
		chksumStats += error;
		addHtml(chksumStats);
		addRowLabel(F("Length Last Sentence")); // Change Later along with checksum code
		addHtmlInt(length_last);
	}
}

void P153_html_show_user_label_forms(struct EventStruct *event)
{
	P153_data_struct *P153_data = 
		static_cast<P153_data_struct *>(getPluginTaskData(event->TaskIndex));
	// TODO: Try and turn this into a for loop. F("") gives me problems
	if(nullptr != P153_data)
	{
		addFormNumericBox(
				F("Number of Labels"),
				getPluginCustomArgName(P153_NR_USER_LABELS_POS),
				P153_data->get_Nr_User_Labels(),
				0,
				P153_MAX_NR_USER_LABELS

		);

		addFormTextBox(
				F("LABEL 0"), 
				getPluginCustomArgName(P153_FIRST_USER_LABEL_POS+0),
				P153_data->get_User_Label(0),
				P153_NR_FORM_chars
		);
		addFormTextBox(
				F("LABEL 1"), 
				getPluginCustomArgName(P153_FIRST_USER_LABEL_POS+1),
				P153_data->get_User_Label(1),
				P153_NR_FORM_chars
		);
		addFormTextBox(
				F("LABEL 2"), 
				getPluginCustomArgName(P153_FIRST_USER_LABEL_POS+2),
				P153_data->get_User_Label(2),
				P153_NR_FORM_chars
		);

		addFormTextBox(
				F("LABEL 3"), 
				getPluginCustomArgName(P153_FIRST_USER_LABEL_POS+3),
				P153_data->get_User_Label(3),
				P153_NR_FORM_chars
		);
	}
}
