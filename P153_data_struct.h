# define PLUGINSTRUCTS_P153_DATA_STRUCT_H

# include "../../_Plugin_Helper.h"
# include <ESPeasySerial.h>

// For Custom Task settings
# define P153_NR_USER_LABELS_POS    0
# define P153_FIRST_USER_LABEL_POS  6 // i'm not sure why this starts at 6, but ignore for now.
# define P153_MAX_NR_USER_LABELS    4
# define P153_NR_LINES              (P153_FIRST_USER_LABEL_POS + P153_MAX_NR_USER_LABELS)
// For Webforms
# define P153_NR_FORM_chars         10 // used for num chars in textbox
// For State Machine
# define P153_STATE_READ            1
# define P153_STATE_LAST_READ       2
# define P153_MAX_LABEL_LENGTH      130
# define P153_MAX_FIELD_LENGTH      130
# define P153_MAX_STRING_LENGTH     2000
# define P153_NR_FIELDS             20 // 48 FOR BMV
# define P153_LABEL_IDX             0
# define P153_FIELD_IDX             1

struct P153_data_struct : public PluginTaskData_base 
{
public:
    P153_data_struct() = default;

    virtual ~P153_data_struct();

    void reset();

    bool init(  ESPEasySerialPort port,
                const int16_t   serial_rx,
                const int16_t   serial_tx,
                unsigned long   baudrate,
                uint8_t         config);

    // Called after loading the config from the settings.
    // Will interpret some data and load caches.

    bool isInitialized() const;

    void sendString(const String& data);

    void sendData(  uint8_t *data,
                    size_t   size);

    bool loop();

    // Get the received sentence
    // @retval true when the string is not empty.
    bool getSentence(String& string);

    void getSentencesReceived(  uint32_t& success,
                                uint32_t& error,
                                uint32_t& length_last);// const;//change

    void setMaxLength(uint16_t maxlenght); 

    // EISH: 
    void setLine(uint8_t varNr,
                const String& line);

    // get label stored in custom configs
    String get_User_Label(int idx);

    int get_Nr_User_Labels();

    int get_Nr_User_LabelForms_Filled();

    // EISH:
    // Made public so we don't have to copy the values when loading/saving.
    String _lines[P153_NR_LINES];

private:

    void save_input_string();
    bool search_field_value(String& str, String& Label, String& field_value);
    String repeat_char(char c, int num);
    void get_flattened_data(String& flattened_data, String* data_list, int num_data_fields);
    void check_checksum();
    void compare_and_reset_checksum(char received_checksum);
    void reset_state_machine(bool& is_done, uint8_t& nextState, uint8_t currentState);
    

    ESPeasySerial *easySerial       = nullptr;
    String         input_string     = "";
    String         sentence         = "";  
    uint16_t       max_length       = P153_MAX_STRING_LENGTH; 
    uint8_t        field_count      = 0;
    uint32_t       success_count    = 0;
    uint32_t       error_count      = 0;
    uint32_t       last_field_count = 0; //change
    uint8_t        currentState     = P153_STATE_READ;
    uint8_t        nextState        = P153_STATE_READ;
    uint8_t        checksum         = 0;
};
