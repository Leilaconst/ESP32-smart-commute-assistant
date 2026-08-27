#include "display.h"
#include "graphics.h"
#include "backend.h"
#include "layout.h"

TFT_eSPI tft = TFT_eSPI();

void initDisplay(){
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(SEABREEZE);
  tft.setTextWrap(false);
  tft.setTextDatum(0);
  tft.setSwapBytes(true);

  writeDisplay();
}

//Draw the Display
void writeDisplay(){
  drawLayout();
}

//trunicate long text
String truncateToFit(String text, int maxWidth) {
  if (tft.textWidth(text) <= maxWidth) return text;
  
  while (tft.textWidth(text + "...") > maxWidth && text.length() > 0) {
    text.remove(text.length() - 1); // chop last char
  }
  return text + "...";
}

//Parse and return the route legs as a vector
std::vector<String> parseRouteLegs(String routeSummary) {
  std::vector<String> legs;
  int start = 0;

  while (start < routeSummary.length()) {
    int commaIndex = routeSummary.indexOf(",", start);

    String leg;
    if (commaIndex == -1) {
      // Last leg — no more commas found
      leg = routeSummary.substring(start);
      start = routeSummary.length();
    } else {
      leg = routeSummary.substring(start, commaIndex);
      start = commaIndex + 1;
    }

    leg.trim();  // removes leading/trailing whitespace, e.g. the space before/after the comma
    if (leg.length() > 0) {
      legs.push_back(leg);
    }
  }

  return legs;
}

//Draw the route legs
void drawRouteLegs(std::vector<String> legs, int startX, int startY, int lineHeight, int maxLines) {
  int linesToShow = min((int)legs.size(), maxLines);
  bool hasMore = legs.size() > maxLines;

  int legsToDraw = hasMore ? linesToShow - 1 : linesToShow;

  int currentY = startY;

  for (int i = 0; i < legsToDraw; i++) {
    tft.drawString(truncateToFit("| " +legs[i], 160), startX, currentY);
    currentY += lineHeight;
  }

  if (hasMore) {
    int remaining = legs.size() - legsToDraw;
    String moreText = (String)"| " + "+" + String(remaining) + " more";
    tft.drawString(moreText, startX, currentY);
  }
}

//Wrap text function
void drawWrappedTextPlain(String text, int startX, int startY, int maxWidthPx, int linePadding) {
  String currentLine = "";
  int currentY = startY;

  for (int i = 0; i < text.length(); i++) {
    String testLine = currentLine + text[i];

    if (tft.textWidth(testLine) > maxWidthPx) {
      tft.drawString(currentLine, startX, currentY);
      currentY += tft.fontHeight() + linePadding;
      currentLine = String(text[i]);
    } else {
      currentLine = testLine;
    }
  }

  if (currentLine.length() > 0) {
    tft.drawString(currentLine, startX, currentY);
  }
}

//Text Styles
void setHeaderStyle(){
  tft.loadFont(MatchaMint20);
  tft.setTextColor(PALM, BACKGROUND, false);
}

void setValueLargeStyle(){
  tft.loadFont(MatchaMint36);
  tft.setTextColor(SUNSET, BACKGROUND, false);
}

void setValueMediumStyle(){
  tft.loadFont(MatchaMint20);
  tft.setTextColor(SUNSET, BACKGROUND, false);
}

void setValueSmallStyle(){
  tft.loadFont(MatchaMint14);
  tft.setTextColor(SUNSET, BACKGROUND, false);
}

void setDetailStyle(){
  tft.loadFont(MatchaMint14);
  tft.setTextColor(BUTTER, BACKGROUND, false);
}

void setTitleStyle(){
  tft.loadFont(MatchaMint36);
  tft.setTextColor(ODYSSEY, BACKGROUND, false);
}

void drawIcon(int32_t x, int32_t y, const uint16_t* iconData) {
  tft.setSwapBytes(true);
  tft.pushImage(x, y, ICON_W, ICON_H, iconData, 0x0000);
  tft.setSwapBytes(false);
}