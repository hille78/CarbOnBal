// This software, known as CarbOnBal is
// Copyright, 2017 L.L.M. (Dennis) Meulensteen. dennis@meulensteen.nl
//
// This file is part of CarbOnBal. A combination of software and hardware.
// I hope it may be of some help to you in balancing your carburetors and throttle bodies.
// Always be careful when working on a vehicle or electronic project like this.
// Your life and health are your sole responsibility, use wisely.
//
// CarbOnBal hardware is covered by the Cern Open Hardware License v1.2
// a copy of the text is incuded with the source code.
//
// CarbOnBal is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CarbOnBal is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with CarbOnBal.  If not, see <http://www.gnu.org/licenses/>.


#include "menu.h"
#include "globals.h"
#include LANGUAGE
#include "lang_generic.h"
#include "lcdWrapper.h"
#include "menuActions.h"
#include "functions.h"
#include "utils.h"

bool quitMenu = false;

// an array of strings contains the menu titles
// an array of function pointers, the same number and order as the above array, determines the actions to be taken when that title is selected
// a size is passed which must be equal to the number of menu entries
void actionDisplayMainMenu() {
  quitMenu = false;
	const char* menu[] = {txtDisplay, txtSettings, txtCalibration, txtDataTransfer};
	void (*actions[])() = {&actionDisplayDisplayMenu, &actionDisplaySettingsMenu, &actionDisplayCalibrationMenu, &actionDisplayCommsMenu};
	uint8_t menuSize = 4;
	handleMenu(menu, actions, menuSize);
}

void actionDisplaySettingsMenu() {
	const char* const menu[] = { txtHardware, txtSoftware, txtExtraMenu};
	void (*actions[])() = { &actionDisplayHardwareSettingsMenu, &actionDisplaySoftwareSettingsMenu, &actionDisplayExtraMenu };
	uint8_t menuSize = 3;
	handleMenu(menu, actions, menuSize);
}

void actionDisplayExtraMenu(){
	const char* const menu[] = {txtDeviceInfo, txtAdvancedMenu, txtSplashScreen, txtFactoryReset};
		void (*actions[])() = { &doDeviceInfo, &doAdvanced, &doSplashScreen, &actionReset};
		uint8_t menuSize = 4;
		handleAdvancedMenu(menu, actions, menuSize, B1100);
}

void actionDisplaySoftwareSettingsMenu() {
	const char* const menu[] = { txtAveragingMethod, txtEmaShift, txtEmaFactor};
	void (*actions[])() = { &actionAveragingMethod, &actionEmaShift, &actionEmaFactor};
	uint8_t menuSize = 3;
	handleAdvancedMenu(menu, actions, menuSize, 0b100);
}

void actionDisplayHardwareSettingsMenu() {
	const char* const menu[] = {txtCylinderCount, txtMasterCylinder, txtContrastButton, txtBrightnessButton};
	void (*actions[])() = {&actionCylinders, &actionMaster, &actionContrastButton, &actionBrightnessButton };
	uint8_t menuSize = 4;
	handleAdvancedMenu(menu, actions, menuSize, B1100);
}


void actionDisplayCommsMenu() {
	const char* const menu[] = {txtCalibrationDump, txtLiveDataDump, txtBaudRate, txtArduinoMode};
	void (*actions[])() = {&doCalibrationDump, &doDataDump, &doBaudRate, &doArduinoMode };
	uint8_t menuSize = 4;
	handleAdvancedMenu(menu, actions, menuSize, B1110);
}

void actionDisplayDisplayMenu() {
	const char* const menu[] = {txtContrast, txtBrightness, txtDetails, txtGraphType,
								txtRpmDisplay, txtUnits, txtMaxZoomRange};
	void (*actions[])() = {&actionContrast, &actionBrightness, &actionSilent, &actionGraphing,
								&doRevs, &doUnits ,&doMaxZoom };
	uint8_t menuSize = 7;
	handleAdvancedMenu(menu, actions, menuSize, B1101100);
}

void actionDisplayCalibrationMenu() {
	const char* const menu[] = {txtViewCalibrations, txtCalibrateNow, txtClearCalibration};
	void (*actions[])() = {&actionDisplayViewCalibrationMenu, &actionDisplayCalibrationSensorMenu, &actionDisplayClearCalibrationMenu };
	uint8_t menuSize = 3;
	handleAdvancedMenu(menu, actions, menuSize, B110);
}

void actionDisplayViewCalibrationMenu() {
	const char* const menu[] = {txtViewSensor2, txtViewSensor3, txtViewSensor4, txtSetCalibrationMax};
	void (*actions[])() = {&doViewCalibration1, &doViewCalibration2, &doViewCalibration3, &actionCalibrationMax };
	uint8_t menuSize = 4;
	handleAdvancedMenu(menu, actions, menuSize, B1110);
}

void actionDisplayClearCalibrationMenu() {
	const char* const menu[] = { txtClearSensor2,  txtClearSensor3,  txtClearSensor4, txtClearAllSensors};
	void (*actions[])() = {&doClearCalibration1, &doClearCalibration2, &doClearCalibration3, &doZeroCalibrations };
	uint8_t menuSize = 4;
	handleMenu(menu, actions, menuSize);
}

void actionDisplayCalibrationSensorMenu() {
	const char* const menu[] = {txtCalibrateSensor2, txtCalibrateSensor3, txtCalibrateSensor4};
	void (*actions[])() = {&doCalibrate1, &doCalibrate2, &doCalibrate3 };
	uint8_t menuSize = 3;
	handleMenu(menu, actions, menuSize);
}

// display a basic setting change screen that does not have to call a function
int doBasicSettingChanger(const char* valueName, int minimum, int maximum, int startValue, int steps ) {
	return doSettingChanger( valueName, minimum, maximum, startValue, steps, NULL );
}

// display a settings change screen that calls a function every time the value changes for an immediate response
// normally used for setting contrast and brightness
int doSettingChanger(const char* valueName, int minimum, int maximum, int startValue, int steps, void (*func)(int i) ) {
	int value = startValue;
	lcd_clear();
	lcd_setCursor(0, 0);
	lcd_print(valueName);
	lcd_setCursor(0, 1);
	lcd_printInt(startValue);

	while (true) {

		int segmentsInCharacter = 5;
		int TotalNumberOfLitSegments = 100000L/maximum * value/1000;
		int numberOfLitBars = TotalNumberOfLitSegments / segmentsInCharacter;
		int numberOfLitSegments = TotalNumberOfLitSegments % segmentsInCharacter;

		char bars[DISPLAY_COLS+1];
		makeBars(bars, numberOfLitBars, 0);

		byte bar[8];
		makeChar(bar,numberOfLitSegments);
		lcd_createChar(6, bar);

		printLcdInteger(value,0,1,6);

		lcd_setCursor(0, 2);
		lcd_print(bars);
		if(numberOfLitBars < DISPLAY_COLS){
			lcd_setCursor(numberOfLitBars, 2);
			lcd_write(byte(6));
		}
		lcd_setCursor(0, 3);
		lcd_print(bars);
		if(numberOfLitBars < DISPLAY_COLS){
			lcd_setCursor(numberOfLitBars, 3);
			lcd_write(byte(6));
		}

		switch ( buttonPressed()) {
		case SELECT: return value; break;
		case RIGHT: if (value < maximum - steps) {
			value += steps;
		} else {
			value = maximum;
		}
		if(func)(*func)(value);
		break;

		case LEFT: if (value > minimum + steps) {
			value -= steps;
		} else {
			value = minimum;
		}
		if(func)(*func)(value);
		break;
		case CANCEL: if(func)(*func)(startValue); return startValue;
		}

		delay(50);
	}
	return startValue;
}

// allows the user to choose a value from an array of strings,
// the selected array index is returned.
int doSettingChooser(const char* valueName, const char* settings[], int count, int startIndex) {
	int index = startIndex;
	boolean settingChanged = true;

	lcd_clear();
	lcd_setCursor(0, 0);
	lcd_print(valueName);

	lcd_setCursor(0, 1);
	lcd_printInt(startIndex);

	while (true) {

		if (settingChanged) {
			lcd_setCursor(0, 1);
			lcd_print(txtSpace20);
			lcd_setCursor(0, 3);
			lcd_print(txtSpace20);
			if(index > 0 ){
				lcd_setCursor(0, 3);
				lcd_printChar(char(MENUCARET+1));           //little arrow to the left
			}
			lcd_setCursor(0, 1);
			lcd_print(settings[index]);

			if(index < count -1){
				lcd_setCursor(19, 3);
				lcd_printChar(char(MENUCARET));             //little arrow to the right

			}
			settingChanged = false;
		}

		switch ( buttonPressed()) {
		case SELECT: return index; break;
		case RIGHT: if (index < count - 1) {
			index += 1;
			settingChanged = true;
		}
		break;

		case LEFT: if (index > 0) {
			index -= 1;
			settingChanged = true;
		}
		break;
		case CANCEL: return startIndex;
		}

		delay(50);
	}
	return startIndex;
}

// displays a menu screen
// menu[] = an array of menu option strings
// *func[] = an array of pointers to the corresponding functions
// menuSize = a counter to say how many values are in both arrays
// display the resulting menu, used by handleMenu()
void drawMenu(const char* const pointerTable[], int count, int offset) {
	lcd_clear();

	for (uint8_t line = 0; ((line < count) && (line < DISPLAY_ROWS)) ; line++) {
		lcd_setCursor(1, line);
		lcd_print((const char*)(pointerTable[line + offset]));
	}

	if (offset > 0) {
		byte upArrow[8] = {
				B00100,
				B01110,
				B10101,
				B00100,
				B00100,
				B00000,
				B00000,
		};
		lcd_createChar(2, upArrow);
		lcd_setCursor(19, 0);
		lcd_write(byte(0x02));
	}

	if (offset < count - DISPLAY_ROWS) {
		byte downArrow[8] = {

				B00000,
				B00000,
				B00100,
				B00100,
				B10101,
				B01110,
				B00100,

		};
		lcd_createChar(3, downArrow);
		lcd_setCursor(19, 3);
		lcd_write(byte(0x03));
	}

	drawCaret(0);
}


void handleMenu(const char* const pointerTable[], void (*func[])(), int menuSize) {
	int cursorLine = 0;
	int offset = 0;
	bool refresh = true;

	drawMenu( pointerTable, menuSize, offset );

	while (!quitMenu) {

		switch ( buttonPressed()) {
		case SELECT:

			(*func[cursorLine + offset])();

			refresh = true;
			break;
		case LEFT:
			if (cursorLine > 0) {
				cursorLine -= 1;
			} else if ((cursorLine == 0) && (offset > 0)) {
				offset -= 1;
			}
			refresh = true;
			break;
		case RIGHT:
			if (cursorLine < DISPLAY_ROWS-1 && cursorLine < menuSize-1) {
				cursorLine += 1;
			} else if ((cursorLine == DISPLAY_ROWS-1) && (offset < menuSize - 4)) {
				offset += 1;
			}
			refresh = true;
			break;
		case CANCEL:
			lcd_clear();
			return;
		}

		if (refresh) {
			drawMenu( pointerTable, menuSize, offset );
			drawCaret(cursorLine);
			refresh = false;
		}
	}
}

void handleAdvancedMenu(const char* const pointerTable[], void (*func[])(), int menuSize, unsigned int mask){
	if(settings.advanced){
		handleMenu(pointerTable, func, menuSize);
	}else {
		const char* menu[16];
		void (*actions[16])();

		int menuIndex=0;
		for(int i = 0; i <menuSize ; i++){
			if(mask & (1<<(menuSize-i-1))){
				menu[menuIndex] = pointerTable[i];
				actions[menuIndex] = func[i];
				menuIndex++;
			}
		}
		handleMenu(menu,actions,menuIndex);
	}
}

// puts the menu cursor in the right place
void drawCaret(uint8_t line) {
	for (uint8_t i = 0; i < DISPLAY_ROWS; i++) {
		lcd_setCursor(0, i);
		lcd_printChar(' ');
	}
	lcd_setCursor(0, line);
	lcd_printChar(char(MENUCARET));//little arrow to the right

}

// builds a custom character used for smooth display (less than 5 stripes)
void makeChar(byte *character, int value){

	byte line = 0x00;

	if(value >=0){
		for (int i = 0; i < value; i++){
			line >>= 1;
			line |= B10000;
		}
	}else{
		for (int i = 0; i > value; i--){
			line <<= 1;
			line |= B00001;
		}
	}

	for (int i=0; i<8;i++){
		character[i]=line;
	}
}

// generates a single bar graph
void makeBars(char *bars, uint8_t number, uint8_t skip) {
	if(number >20) number=20;

	for (uint8_t i = 0; i < skip ; i++) {
		bars[i] = ' ';
	}
	for (uint8_t i = skip; i < number ; i++) {
		bars[i] = 0xff;
	}
	for (uint8_t i = number ; i < 20; i++) {
		bars[i] = ' ';
	}
	bars[20] = 0x00;
}
