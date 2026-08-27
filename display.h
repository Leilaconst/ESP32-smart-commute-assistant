#ifndef DISPLAY_H
#define DISPLAY_H

#include <SPI.h>
#include <TFT_eSPI.h>
#include <vector>
extern TFT_eSPI tft;

//Fonts
#include "fonts/MatchaMint14.h"
#include "fonts/MatchaMint20.h"
#include "fonts/MatchaMint36.h"


//Display Layout
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320

//Functions
void initDisplay();
void writeDisplay();

//Line Wraping and Trunicating
String truncateToFit(String text, int maxWidth);
std::vector<String> parseRouteLegs(String routeSummary);
void drawRouteLegs(std::vector<String> legs, int startX, int startY, int lineHeight, int maxLines);
void drawWrappedTextPlain(String text, int startX, int startY, int maxWidthPx, int linePadding);

//Text header and Style setters
void setHeaderStyle();
void setValueLargeStyle();
void setValueMediumStyle();
void setValueSmallStyle();
void setDetailStyle();
void setTitleStyle();

//Icon drawing
void drawIcon(int32_t x, int32_t y, const uint16_t* iconData);

#endif