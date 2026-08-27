#ifndef LAYOUT_H
#define LAYOUT_H

#define DETAIL_PADDING 15

//Title
#define TITLE_X 20
#define TITLE_Y 20

//Event Block
#define EVENT_ICON_X 9
#define EVENT_ICON_Y 45
#define EVENT_HEADER_X 77
#define EVENT_HEADER_Y 71
#define EVENT_1_TITLE_X 28
#define EVENT_1_TITLE_Y 108
#define EVENT_2_TIME_X 47
#define EVENT_2_TIME_Y 142
#define EVENT_3_LOCATION_X (EVENT_2_TIME_X)
#define EVENT_3_LOCATION_Y (EVENT_2_TIME_Y + DETAIL_PADDING)

//Transit Block
#define TRANSIT_ICON_X 290
#define TRANSIT_ICON_Y 53
#define TRANSIT_HEADER_X 347
#define TRANSIT_HEADER_Y 61
#define TRANSIT_1_DURATION_X 314
#define TRANSIT_1_DURATION_Y 111
#define TRANSIT_2_ROUTE_X (TRANSIT_1_DURATION_X)
#define TRANSIT_2_ROUTE_Y 142

//Leave-by Block
#define LEAVE_BY_ICON_X 9
#define LEAVE_BY_ICON_Y 195
#define LEAVE_BY_HEADER_X 77
#define LEAVE_BY_HEADER_Y 219
#define LEAVE_BY_1_TIME_X 31
#define LEAVE_BY_1_TIME_Y 250
#define LEAVE_BY_2_TIME_LEFT_X 47
#define LEAVE_BY_2_TIME_LEFT_Y 292

//Weather Block
#define WEATHER_ICON_X 225
#define WEATHER_ICON_Y 189
#define WEATHER_HEADER_X 288
#define WEATHER_HEADER_Y 219
#define WEATHER_1_TEMP_X 251
#define WEATHER_1_TEMP_Y 251
#define WEATHER_2_NOTE_X WEATHER_1_TEMP_X
#define WEATHER_2_NOTE_Y (WEATHER_1_TEMP_Y + DETAIL_PADDING)

//functions
void drawTitle();
void drawEventBlock();
void drawTransitBlock();
void drawLeaveByBlock();
void drawWeatherBlock();

void drawLayout();

#endif