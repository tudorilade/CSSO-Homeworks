#pragma once

// Constants related to image processing
#define CHUNK_SIZE 4000
#define BMP_HEADER_SIZE 54

// Constants related to the creation of windows

// ID's related to my performances
#define ID_COMPUTE_MY_PERFORMANCES 1001
#define ID_MY_P_TITLE 100010
#define ID_SID_DISPLAY 10011
#define ID_SID_LABEL 10012
#define ID_HT_DISPLAY 10013
#define ID_HT_LABEL 10014
#define ID_API_DISPLAY 10015
#define ID_API_LABEL 10016
#define ID_CPU_DISPLAY 10017
#define ID_CPU_LABEL 10018
#define ID_COLLECT_BUTTON 10019
#define ID_LOG_WINDOW_P 10020
#define ID_LOG_WINDOW_P_LABEL 10021
#define ID_CLEAR_LOGS_P 10022

#define STARTING_POSITION 30
#define WIDTH_LABEL 150
#define HEIGHT_LABEL 20
#define WIDTH_DISPLAY 300
#define HEIGHT_DISPLAY 60


// ID's related to evaluate my performances
#define ID_EVALUATE_MY_PERFORMANCES 1005

/*
Interfata grafica

1. Window with "My PC performances" 
	-> Aici vom avea display pt pct-ul 2, si atat. 
		Deci, o clasa care va fi responsabila de prelucrarea acestor cerinte si pe care le va afisa, nu in timp real, ci executabil printr-un buton la cerere.

2. Window with "Evaluate my PC"
	-> Aici vom avea logica ce se va ocupa de procesarea si timing ul imaginilor. 
	TBD cand ajungem aici
*/

