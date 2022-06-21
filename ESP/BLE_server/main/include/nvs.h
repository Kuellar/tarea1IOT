int Write_NVS(int32_t data, int key);
esp_err_t Write_NVS_str(char* data, int key);
esp_err_t Read_NVS(int32_t* data,int key);
esp_err_t Read_NVS_str(char* data, int key, size_t *length);