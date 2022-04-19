// Resource.h
// NNetSimu
// Used by NNetSimu.rc

#pragma once

// identifiers below 100 are reserved for win32_utilities
// application identifiers start at 100

#define       IDM_FATAL_ERROR  	             100
								   	         
#define       IDC_NNET_SIMU_MAIN  	         102
#define       IDD_APPLY                      105
#define       IDD_RESET                      107
										    
#define       IDM_SCRIPT_DIALOG              110
#define       IDM_UNDO                       115
#define       IDM_REDO                       116
#define       IDM_DUMP                       117
								   	         
#define       IDI_NNETSIMU			         120
#define       IDI_SMALL				         121
#define       IDC_NNETSIMU			         122
								   	         
#define       IDM_SLOWER                     130
#define       IDM_FASTER                     131
								   	         
#define       IDM_ESCAPE                     140
#define       IDM_RESET                      141
#define       IDM_SELECT_ALL                 142 
#define       IDM_SELECT_SUBTREE             143
#define       IDM_SELECT_NOB                 144
#define       IDM_DESELECT_ALL               145
#define       IDM_DESELECT_SUBTREE           146
#define       IDM_DESELECT_NOB               147
#define       IDM_SELECTION                  148
#define       IDM_DELETE                     149
								   	         
#define       IDM_SAVE_MODEL                 150
#define       IDM_ASK_AND_SAVE_MODEL         151
#define       IDM_SAVE_MODEL_AS              152
#define       IDM_OPEN_MODEL                 153
#define       IDM_ADD_MODEL                  154
#define       IDX_REPLACE_MODEL              155
#define       IDX_FILE_NOT_FOUND             156
#define       IDX_ERROR_IN_FILE              157
#define       IDM_NEW_MODEL                  158
#define       IDM_ADD_IMPORTED_MODEL         159
								   	         
#define       IDD_ARROWS_ON                  161
#define       IDD_ARROWS_OFF                 162
#define       IDD_RULER_ON                   163
#define       IDD_RULER_OFF                  164
#define       IDD_MEASUREMENT_ON             165
#define       IDD_MEASUREMENT_OFF            166
#define       IDD_ADD_TRACK                  168
#define       IDD_DELETE_TRACK               169
#define       IDD_EMPHASIZE                  170
#define       IDD_PERF_MON_MODE_ON           171
#define       IDD_PERF_MON_MODE_OFF          172
#define       IDD_DELETE_EMPTY_TRACKS        173

#define       IDD_SOUND_ON                   200
#define       IDD_SOUND_OFF                  202
#define       IDX_PLAY_SOUND                 203
								   	         
#define       IDD_AUTO_OPEN_ON               210
#define       IDD_AUTO_OPEN_OFF              211
									         
#define       IDD_SENSOR_PNTS_ON             220
#define       IDD_SENSOR_PNTS_OFF            221
									         
#define       IDM_MONITOR_WINDOW             330
#define       IDD_ADD_EEG_SENSOR             345
#define       IDD_DELETE_EEG_SENSOR          346
#define       IDD_SCALE_EEG_SIGNALS          347
#define       IDD_CONNECT                    350
								   	         
#define       IDD_INSERT_KNOT                460
#define       IDD_INSERT_NEURON              461
#define       IDD_NEW_IO_NEURON_PAIR         463
#define       IDD_DELETE_NOB                 468
										    
#define       IDD_ADD_INCOMING2BASEKNOT      571
#define       IDD_ADD_INCOMING2PIPE          572
#define       IDD_ADD_OUTGOING2BASEKNOT      573
#define       IDD_ADD_OUTGOING2PIPE          574
#define       IDD_TRIGGER_SOUND_DLG          575
#define       IDC_TRIGGER_SOUND_ON           576
#define       IDC_TRIGGER_SOUND_FREQ         577
#define       IDC_TRIGGER_SOUND_MSEC         578
#define       IDC_TRIGGER_SOUND_TEST         579
#define       IDD_STOP_ON_TRIGGER            580
								   		     
#define       IDM_CRSR_WINDOW                590
#define       IDM_PARAM_WINDOW  	         591
#define       IDM_PERF_WINDOW                592
#define       IDM_DESC_WINDOW                593
#define       IDM_SIG_DESIGNER               594
#define       IDM_NNET_REFRESH_RATE          595
									        
#define       IDM_ANALYZE                    600
#define       IDM_ANALYZE_LOOPS              601
#define       IDM_ANALYZE_ANOMALIES          602
#define       IDM_CENTER_MODEL               611
							       		     
#define       IDM_COPY_SELECTION             650 
#define       IDM_DELETE_SELECTION           651
#define       IDM_CLEAR_BEEPERS              654
#define       IDM_SELECT_ALL_BEEPERS         655
										     
#define       IDM_TRIGGER_STIMULUS           660
#define       IDM_SIGNAL_DESIGNER_INTEGRATED 662
#define       IDM_SIGNAL_DESIGNER_STACKED    663

#define       IDM_MAKE_CONNECTOR             674
#define       IDD_DISC_IOCONNECTOR           676
#define       IDD_SPLIT_NEURON               678

#define       IDD_STD_SIGNAL_GENERATOR       700
#define       IDD_SELECT_SIGNAL_GENERATOR    701
#define       IDD_NEW_SIGNAL_GENERATOR       702
#define       IDD_DELETE_SIGNAL_GENERATOR    703
#define       IDD_RENAME_SIGNAL_GENERATOR    704
#define       IDD_SELECT_SIG_GEN_CLIENTS     705
#define       IDD_REGISTER_SIG_GEN           706
#define       IDD_UNREGISTER_SIG_GEN         707