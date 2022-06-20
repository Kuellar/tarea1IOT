from db.api_db import get_last_data_prot
import numpy as np



def updatePlots(self):
        if self.started_monitoring:
            last_data = get_last_data_prot(self)
            self.batteryProgressBar.setProperty("value", last_data.Batt_level)

            if self.protocol == self.protocol_list[1]:
                self.battery_data = np.append(self.battery_data, last_data.Batt_level)[1:]

            elif self.protocol == self.protocol_list[2]:
                self.battery_data = np.append(self.battery_data, last_data.Batt_level)[1:]
                self.temp_data = np.append(self.temp_data, last_data.Temp)[1:]
                self.press_data = np.append(self.press_data, last_data.Press)[1:]
                self.hum_data = np.append(self.hum_data, last_data.Hum)[1:]
                self.co_data = np.append(self.co_data, last_data.Co)[1:]

            elif self.protocol == self.protocol_list[3]:
                self.battery_data = np.append(self.battery_data, last_data.Batt_level)[1:]
                self.temp_data = np.append(self.temp_data, last_data.Temp)[1:]
                self.press_data = np.append(self.press_data, last_data.Press)[1:]
                self.hum_data = np.append(self.hum_data, last_data.Hum)[1:]
                self.co_data = np.append(self.co_data, last_data.Co)[1:]
                self.RMS_data = np.append(self.RMS_data, last_data.RMS)[1:]

            elif self.protocol == self.protocol_list[4]:
                self.battery_data = np.append(self.battery_data, last_data.Batt_level)[1:]
                self.temp_data = np.append(self.temp_data, last_data.Temp)[1:]
                self.press_data = np.append(self.press_data, last_data.Press)[1:]
                self.hum_data = np.append(self.hum_data, last_data.Hum)[1:]
                self.co_data = np.append(self.co_data, last_data.Co)[1:]
                self.RMS_data = np.append(self.RMS_data, last_data.RMS)[1:]
                self.Amp_x_data = np.append(self.Amp_x_data, last_data.Amp_x)[1:]
                self.Frec_x_data = np.append(self.Frec_x_data, last_data.Frec_x)[1:]
                self.Amp_y_data = np.append(self.Amp_y_data, last_data.Amp_y)[1:]
                self.Frec_y_data = np.append(self.Frec_y_data, last_data.Frec_y)[1:]
                self.Amp_z_data = np.append(self.Amp_z_data, last_data.Amp_z)[1:]
                self.Frec_z_data = np.append(self.Frec_z_data, last_data.Frec_z)[1:]

            elif self.protocol == self.protocol_list[5]:
                self.battery_data = np.append(self.battery_data, last_data.Batt_level)[1:]
                self.temp_data = np.append(self.temp_data, last_data.Temp)[1:]
                self.press_data = np.append(self.press_data, last_data.Press)[1:]
                self.hum_data = np.append(self.hum_data, last_data.Hum)[1:]
                self.co_data = np.append(self.co_data, last_data.Co)[1:]
                self.Acc_x_data = np.append(self.Acc_x_data, last_data.Acc_x)[1:]
                self.Acc_y_data = np.append(self.Acc_y_data, last_data.Acc_y)[1:]
                self.Acc_z_data = np.append(self.Acc_z_data, last_data.Acc_z)[1:]

            self.time_data = np.append(self.time_data, last_data.Timestamp.strftime("%M:%S"))[1:]
            
            if self.plots_started[0]:
                self.graph_item_1.clear()
                updatePlotsData(self.graph_item_1, self.time_data, self.plots_data[1])
            if self.plots_started[1]:
                self.graph_item_2.clear()
                updatePlotsData(self.graph_item_2, self.time_data, self.plots_data[2])
            if self.plots_started[2]:
                self.graph_item_3.clear()
                updatePlotsData(self.graph_item_3, self.time_data, self.plots_data[3])
            updatePlotsData(self)
 
def updatePlotsData(self):
    self.graph_item_1.plot(self.time_data, self.PLOTS_DATA_DICT[self.plots_data[0]])
    self.graph_item_2.plot(self.time_data, self.PLOTS_DATA_DICT[self.plots_data[1]])
    self.graph_item_3.plot(self.time_data, self.PLOTS_DATA_DICT[self.plots_data[2]])
