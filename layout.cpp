#include "display.h"
#include "graphics.h"
#include "layout.h"
#include "backend.h"

void drawTitle(){
  setTitleStyle();
  tft.drawString("Commute Assistant", TITLE_X, TITLE_Y);
  tft.unloadFont();
}

void drawEventBlock(){
  //event icon
  drawIcon(EVENT_ICON_X, EVENT_ICON_Y, calendar);

  //event header
  setHeaderStyle();
  tft.drawString("EVENT", EVENT_HEADER_X, EVENT_HEADER_Y);
  tft.unloadFont();

  //event title
  setValueMediumStyle();
  tft.drawString("+ " + next_event, EVENT_1_TITLE_X, EVENT_1_TITLE_Y);
  tft.unloadFont();

  //event time and location
  setDetailStyle();
  tft.drawString("| "+ event_start, EVENT_2_TIME_X, EVENT_2_TIME_Y);
  tft.drawString("| "+ event_location, EVENT_3_LOCATION_X, EVENT_3_LOCATION_Y);
  tft.unloadFont();

}

void drawTransitBlock(){
  //transit icon
  drawIcon(TRANSIT_ICON_X, TRANSIT_ICON_Y, bus_stop);

  //transit header
  setHeaderStyle();
  drawWrappedTextPlain("TRANSIT ROUTE", TRANSIT_HEADER_X, TRANSIT_HEADER_Y, tft.textWidth("TRANSIT"), 3);
  tft.unloadFont();

  //travel duration
  setValueSmallStyle();
  tft.drawString(travel_duration, TRANSIT_1_DURATION_X, TRANSIT_1_DURATION_Y);
  tft.unloadFont();

  //route summary
  setDetailStyle();
  drawRouteLegs(parseRouteLegs(route_summary), TRANSIT_2_ROUTE_X, TRANSIT_2_ROUTE_Y, tft.fontHeight(), 4);
  tft.unloadFont();
}

void drawLeaveByBlock(){
  //icon
  drawIcon(LEAVE_BY_ICON_X, LEAVE_BY_ICON_Y, clock_icon);

  //header
  setHeaderStyle();
  tft.drawString("LEAVE-BY", LEAVE_BY_HEADER_X, LEAVE_BY_HEADER_Y);
  tft.unloadFont();

  //time
  setValueLargeStyle();
  tft.drawString(leave_by, LEAVE_BY_1_TIME_X, LEAVE_BY_1_TIME_Y);
  tft.unloadFont();

  //time remaining
  setDetailStyle();
  tft.drawString(time_remain + " left", LEAVE_BY_2_TIME_LEFT_X, LEAVE_BY_2_TIME_LEFT_Y);
  tft.unloadFont();
}

void drawWeatherBlock(){
  //icon
  if(condition == "Clouds"){
    if(condition_note == "few clouds: 11-25%" || condition_note == "scattered clouds: 25-50%"){
      drawIcon(WEATHER_ICON_X, WEATHER_ICON_Y, partly_cloudy);
    }
    else{
      drawIcon(WEATHER_ICON_X, WEATHER_ICON_Y, cloudy);
    }
  }
  else if(condition == "Clear"){
    drawIcon(WEATHER_ICON_X, WEATHER_ICON_Y, sunny);
  }
  else if(condition == "Snow"){
    drawIcon(WEATHER_ICON_X, WEATHER_ICON_Y, snowing);
  }
  else if(condition == "Rain" || condition == "Drizzle"){
    drawIcon(WEATHER_ICON_X, WEATHER_ICON_Y, raining);
  }
  else if(condition == "Squall"){
    drawIcon(WEATHER_ICON_X, WEATHER_ICON_Y, windy);
  }
  else if(condition == "Thunderstorm"){
    drawIcon(WEATHER_ICON_X, WEATHER_ICON_Y, lightning);
  }
  else{
    drawIcon(WEATHER_ICON_X, WEATHER_ICON_Y, cloudy);
  }

  //header
  setHeaderStyle();
  tft.drawString(truncateToFit(condition_note, 181), WEATHER_HEADER_X, WEATHER_HEADER_Y);
  tft.unloadFont();

  //temp and weather note
  setDetailStyle();
  tft.drawString("| " + temp_c, WEATHER_1_TEMP_X, WEATHER_1_TEMP_Y);
  drawWrappedTextPlain("| " + weather_note, WEATHER_2_NOTE_X, WEATHER_2_NOTE_Y, 171, 3);
  tft.unloadFont();
  
}

void drawLayout(){
  drawTitle();
  drawEventBlock();
  drawTransitBlock();
  drawLeaveByBlock();
  drawWeatherBlock();
}
