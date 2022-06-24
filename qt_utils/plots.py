from db.api_db import get_last_data_prot
import numpy as np
from datetime import datetime


def updatePlots(self):
        last_data = get_last_data_prot(self)
        if self.started_monitoring:
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
                self.RMS_data = np.append(self.RMS_data, float(last_data.RMS))[1:]

            elif self.protocol == self.protocol_list[4]:
                self.battery_data = np.append(self.battery_data, last_data.Batt_level)[1:]
                self.temp_data = np.append(self.temp_data, last_data.Temp)[1:]
                self.press_data = np.append(self.press_data, last_data.Press)[1:]
                self.hum_data = np.append(self.hum_data, last_data.Hum)[1:]
                self.co_data = np.append(self.co_data, last_data.Co)[1:]
                self.RMS_data = np.append(self.RMS_data, float(last_data.RMS))[1:]
                self.Amp_x_data = np.append(self.Amp_x_data, float(last_data.Amp_x))[1:]
                self.Frec_x_data = np.append(self.Frec_x_data, float(last_data.Frec_x))[1:]
                self.Amp_y_data = np.append(self.Amp_y_data, float(last_data.Amp_y))[1:]
                self.Frec_y_data = np.append(self.Frec_y_data, float(last_data.Frec_y))[1:]
                self.Amp_z_data = np.append(self.Amp_z_data, float(last_data.Amp_z))[1:]
                self.Frec_z_data = np.append(self.Frec_z_data, float(last_data.Frec_z))[1:]

            elif self.protocol == self.protocol_list[5]:
                self.battery_data = np.append(self.battery_data, last_data.Batt_level)[1:]
                self.temp_data = np.append(self.temp_data, last_data.Temp)[1:]
                self.press_data = np.append(self.press_data, last_data.Press)[1:]
                self.hum_data = np.append(self.hum_data, last_data.Hum)[1:]
                self.co_data = np.append(self.co_data, last_data.Co)[1:]
                self.Acc_x_data = np.append(self.Acc_x_data, last_data.Acc_x)[1:]
                self.Acc_y_data = np.append(self.Acc_y_data, last_data.Acc_y)[1:]
                self.Acc_z_data = np.append(self.Acc_z_data, last_data.Acc_z)[1:]

        self.time_data = np.append(self.time_data, int(datetime.now().strftime("%S")))[1:]
        if self.plots_started[0]:
            self.graph_item_1.clear()
            updatePlotsData(self, 0)
        if self.plots_started[1]:
            self.graph_item_2.clear()
            updatePlotsData(self, 1)
        if self.plots_started[2]:
            self.graph_item_3.clear()
            updatePlotsData(self, 2)
 
def updatePlotsData(self, idx):
    if idx == 0:
        self.graph_item_1.plot(self.time_data, self.getPlotData(self.plots_data[0]))
    elif idx == 1:
        self.graph_item_2.plot(self.time_data, self.getPlotData(self.plots_data[1]))
    elif idx == 2:
        self.graph_item_3.plot(self.time_data, self.getPlotData(self.plots_data[2]))

def getVariablesList(self):
    if self.protocol == self.protocol_list[0]:
        return [""]
    elif self.protocol == self.protocol_list[1]:
        return ["Batt_level"]
    elif self.protocol == self.protocol_list[2]:
        return ["Batt_level", "Temp", "Press", "Hum", "Co"]
    elif self.protocol == self.protocol_list[3]:
        return ["Batt_level", "Temp", "Press", "Hum", "Co", "RMS"]
    elif self.protocol == self.protocol_list[4]:
        return ["Batt_level", "Temp", "Press", "Hum", "Co", "RMS", "Amp_x", "Frec_x", "Amp_y", "Frec_y", "Amp_z", "Frec_z"]
    elif self.protocol == self.protocol_list[5]:
        return ["Batt_level", "Temp", "Press", "Hum", "Co", "Acc_x", "Acc_y", "Acc_z"]
    return [""]
