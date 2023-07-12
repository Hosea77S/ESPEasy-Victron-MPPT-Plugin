# include "../PluginStructs/P153_data_struct.h"
# include <ESPeasySerial.h>

// DONE-
P153_data_struct::~P153_data_struct() 
{
    if (easySerial != nullptr) 
    {
        delete easySerial;
        easySerial = nullptr;
    }
}

// DONE-
void P153_data_struct::reset() 
{
    if (easySerial != nullptr) {
        delete easySerial;
        easySerial = nullptr;
    }
}

// DONE-
bool P153_data_struct::init(ESPEasySerialPort port, const int16_t serial_rx, const int16_t serial_tx, unsigned long baudrate,
                            uint8_t config) 
{
    if ((serial_rx < 0) && (serial_tx < 0)) 
    {
        return false;
    }
    reset();
    easySerial = new (std::nothrow) ESPeasySerial(port, serial_rx, serial_tx);

    if (isInitialized()) 
    {
        # if defined(ESP8266)
        easySerial->begin(baudrate, (SerialConfig)config);
        # elif defined(ESP32)
        easySerial->begin(baudrate, config);
        # endif // if defined(ESP8266)
        return true;
    }
    return false;
}

// DONE-
bool P153_data_struct::isInitialized() const 
{
    return easySerial != nullptr;
}

// DONE-
void P153_data_struct::sendString(const String& data) 
{
    if (isInitialized() && (!data.isEmpty())) 
    {
        // setDisableFilterWindowTimer();
        easySerial->write(data.c_str());

        if (loglevelActiveFor(LOG_LEVEL_INFO)) 
        {
            String log = F("Proxy: Sending: ");
            log += data;
            addLogMove(LOG_LEVEL_INFO, log);
        }
    }
}

// DONE-
void P153_data_struct::sendData(uint8_t *data, size_t size) 
{
    if (isInitialized() && size) 
    {
        //setDisableFilterWindowTimer();
        easySerial->write(data, size);

        if (loglevelActiveFor(LOG_LEVEL_INFO)) 
        {
            String log = F("Proxy: Sending ");
            log += size;
            log += F(" bytes.");
            addLogMove(LOG_LEVEL_INFO, log);
        }
    }
}

// DONE-
bool P153_data_struct::loop() 
{
    if (!isInitialized()) 
    {
        return false;
    }
    currentState = P153_STATE_READ; //+
    nextState = P153_STATE_READ; //+
    field_count = 0; //+
    bool fullDataReceived = false;

    if (easySerial != nullptr) 
    {
        int available = easySerial->available(); //+

        while (available > 0 && !fullDataReceived) 
        {
            char c = easySerial->read(); 
            input_string += c;
            --available; //+

            if (available == 0) //+
            {
                available = easySerial->available(); //+
                delay(0); //+
            } //+

            if(c == '\n')
            {
                field_count += 1;
            }

            if((input_string.length() > P153_MAX_STRING_LENGTH) || (field_count>22)) //+++
            {
                last_field_count = field_count;
                error_count += 1;
                field_count = 0;
                nextState = P153_STATE_READ;
                //save_input_string(); //+
                input_string = "";
                break; // exit the while loop
            }

            switch (currentState) 
            {
                case P153_STATE_READ:
                {
                    if( input_string.indexOf("Checksum\t")>0 ) //+++
                    {
                        nextState =  P153_STATE_LAST_READ; 
                    }
                    break;
                }

                case P153_STATE_LAST_READ:
                {
                    fullDataReceived = true;
                    nextState =  P153_STATE_READ; 
                    break;
                }

                default:
                {
                    reset_state_machine(fullDataReceived, nextState, currentState);
                    break;
                }
            } // end case

            currentState = nextState;
        } // end while(available > 0 && !fullSentenceReceived)
    } // end if (easySerial != nullptr) 

    if (fullDataReceived) 
    {
        success_count += 1;
        save_input_string();
        input_string = "";
        last_field_count = field_count;
        field_count = 0;
        check_checksum();
    }

    return fullDataReceived;
}

// DONE-ish-
void P153_data_struct::reset_state_machine(bool& is_done, uint8_t& nextState, uint8_t currentState)
{
    nextState = P153_STATE_READ;
    currentState = P153_STATE_READ;
    input_string = "";
    field_count = 0;
    is_done = false;
}

// DONE-
void P153_data_struct::save_input_string()
{
    sentence = input_string.substring(0, input_string.length());
}

// DONE-
bool P153_data_struct::getSentence(String& string) 
{
    int nr_data_labels = get_Nr_User_Labels();
    String user_data[nr_data_labels];
    String user_data_success = String();

    for(int i = 0; i<nr_data_labels; ++i)
    {
        String label = get_User_Label(i);
        user_data_success += String( search_field_value(sentence, label, user_data[i]) );
    }

    get_flattened_data(string, user_data, nr_data_labels);
    string += String(nr_data_labels) + "," + user_data_success + "," + String(last_field_count);

    if (string.isEmpty()) 
    {
        return false;
    }

    return true;
}

// DONE-
bool P153_data_struct::search_field_value(String& str, String& Label, String& field_value)
{
    field_value = "";

    String appended_field_label = String('\n')+Label+String('\t');

    int idx = str.indexOf(appended_field_label);

    if(idx < 0)
    {
        field_value = String('0');
        return false;
    }

    int start_idx = idx + appended_field_label.length();
    int end_idx = str.indexOf('\r', start_idx);

    field_value = str.substring(start_idx, end_idx);

    return true;
}

// DONE-
void P153_data_struct::getSentencesReceived(uint32_t& success, uint32_t& error, uint32_t& length_last) //const 
{
    success     = success_count;
    error       = error_count;
    length_last = last_field_count;
}

// DONE-
void P153_data_struct::setMaxLength(uint16_t maxlenght) 
{
    max_length = maxlenght;
}

// DONE-
// EISH: not sure about this
void P153_data_struct::setLine(uint8_t varNr, const String& line) 
{
    if (varNr < P153_NR_LINES) 
    {
        _lines[varNr] = line;
    }
}

// DONE-
String P153_data_struct::get_User_Label(int idx)
{
        int real_idx = P153_FIRST_USER_LABEL_POS + idx;
        if(real_idx < P153_NR_LINES)
        {
            return _lines[real_idx];
        }
        else
        {
            return "";
        }
}

// DONE-ish-
int P153_data_struct::get_Nr_User_LabelForms_Filled()
{
    // Not sure if i'll be successful with this function
    return 1;
}

// DONE-
int P153_data_struct::get_Nr_User_Labels()
{
    return (_lines[P153_NR_USER_LABELS_POS]).toInt();
    //return 4;
}

// DONE-
String P153_data_struct::repeat_char(char c, int num)
{
    if(num <= 1)
    {
        return "";
    }
    String result = String();
    for(int i=0; i<num; ++i)
    {
        result += c;
    }
    return result;
}

// DONE-
void P153_data_struct::get_flattened_data(String& flattened_data, String* data_list, int num_data_fields)
{
    flattened_data = "";
    for(int i=0; i<num_data_fields; ++i)
    {
        flattened_data += data_list[i] + String(',');
    }
}

// DONE
void P153_data_struct::check_checksum()
{
    int i = sentence.indexOf("\r\n",0);
    while(i<sentence.length()-1)
    {
        char c = sentence[i];
        checksum = (checksum + (uint8_t)c);  
        i += 1;
    }

    char received_checksum = sentence.charAt(sentence.length()-1);

    compare_and_reset_checksum(received_checksum);
}


// DONE
void P153_data_struct::compare_and_reset_checksum(char received_checksum)
{
  uint8_t cs = checksum;
  cs = 256 - cs;
  if (  ((char)cs) != received_checksum)
  {
    error_count += 1;
  }
  checksum = 0;
}
