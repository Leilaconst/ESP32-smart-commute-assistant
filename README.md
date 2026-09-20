# 🚌 Commute Assistant

An ESP32-based desk display that answers one question every morning: **"When do I actually need to leave?"**

Commute Assistant pulls your next Google Calendar event, the current weather, and a live transit route, then boils it all down into a single leave-by time — plus a plain-language nudge on what to wear — rendered on a small pixel-art TFT display.

![Display mockup](./assets/display_layout.png)

## Overview

The device polls a lightweight backend every few minutes and draws four blocks onto the screen:

- **Event** — your next Google Calendar event, with time and location
- **Leave-By** — the exact time you need to walk out the door, and how long you have left
- **Transit Route** — the bus/subway route and total travel time to get there
- **Weather** — current temperature, conditions, and a plain-language dressing tip (e.g. *"It is very cold and snowy, dress warm."*)

## Data Sources

| Source | Purpose |
|---|---|
| Google Calendar | Pulls the next upcoming event (time + location) |
| OpenWeatherMap | Current temperature and weather conditions |
| Transit App API | Route and travel time from home to the event location |

## How It Works

To keep the ESP32's workload light, almost all the real work happens off-device, in a backend proxy:

```
[Google Calendar]   [Weather API]   [Transit API]
         \                |               /
          \               |              /
                [Backend Proxy] <- does the hard work
                        |
              (merges everything into
                one JSON reply)
                        |
                  [ESP32] <- does the easy work
                        |
                  [TFT screen]
```

**ESP32 responsibilities:**
- Connect to Wi-Fi
- Every few minutes, send one HTTPS `GET /status` request to the backend
- Parse the small JSON reply
- Draw the parsed values onto the TFT screen

**Backend proxy responsibilities:**
- Handle Google OAuth token refresh
- Call the Calendar, Weather, and Transit APIs
- Merge everything into a single JSON payload
- Serve that JSON to the ESP32 on a simple `GET /status` endpoint
- Runs as a serverless function (Cloudflare Worker), so there's no server to keep running — it just executes on request

This split means the ESP32 never talks to Google, OpenWeatherMap, or the Transit API directly. It only ever has to make one simple request and draw the result.

### Example backend response

```json
{
  "next_event": "CS Club meeting",
  "leave_by": "14:35",
  "temp_c": 3,
  "condition": "snow",
  "weather_note": "It is very cold and snowy, dress warm.",
  "route_summary": "GO Bus 12 → Subway Line 1"
}
```

### Leave-by calculation

```
leave_by_time = event_start_time - travel_duration - buffer
```

### Google OAuth flow

Since Calendar data is private, the backend authenticates once and then refreshes access as needed:

1. Log in once through a Google consent screen, authorizing the app to read the calendar
2. Google issues a refresh token
3. The backend stores that refresh token securely and uses it to request short-lived access tokens whenever it needs to call the Calendar API

## Hardware

**Board:** ESP32-S3-N8R2
**Display:** SPI TFT (touch-capable)

The TFT and touch controller share the SPI bus, so several pins are wired to both a display and a touch function:

| TFT / Touch Pin | ESP32-S3-N8R2 Pin |
|---|---|
| VCC | 3V3 |
| GND | GND |
| LED | 3V3 |
| RESET | 8 |
| D/C | 9 |
| CS | 10 |
| SDI (MOSI) / T_DIN | 11 |
| SCK / T_CLK | 12 |
| SDO (MISO) / T_OUT | 13 |
| T_CS | 14 |
| T_IRQ | 21 |

## Display Design

The UI is drawn as four independent blocks (event, leave-by, transit, weather), each redrawn only when its underlying data changes — so the screen doesn't flicker on every poll:

```cpp
void writeDisplay(){
  static String lastLeaveBy = "";
  static String lastCondition = "";

  if (leave_by != lastLeaveBy) {
    drawLeaveByBlock();
    lastLeaveBy = leave_by;
  }
  if (condition != lastCondition) {
    drawWeatherBlock();
    lastCondition = condition;
  }
  // ...etc.
}
```

Text styling is centralized into small style helpers (`setHeaderStyle()`, `setValueStyle()`, `setDetailStyle()`, `setTitleStyle()`) so every block shares the same font sizes and palette without repeating style code.

### Color Palette

![Color palette](./assets/project_palette.jpg)

| Name | Hex |
|---|---|
| Butter | `#F4D892` |
| Moss | `#C0B05B` |
| Palm | `#657652` |
| Guava | `#F2B6A3` |
| Sunset | `#E89C73` |
| Sangria | `#E36559` |
| Seabreeze | `#E5E9EB` |
| Lagoon | `#94BEBB` |
| Odyssey | `#23617E` |

## Project Status

- [x] Set up ESP32 and TFT wiring
- [x] Set up ESP32 Wi-Fi connectivity
- [x] Set up backend with test data
- [ ] Connect ESP32 to the backend and parse JSON
- [ ] Render/write parsed data onto the TFT
- [ ] Set up OpenWeatherMap in the backend
- [ ] Set up Google Calendar OAuth
- [ ] Set up Transit App API for routing
- [ ] Set up and test polling and error handling
- [ ] Fine-tune display positioning

## Tech Stack

- **Firmware:** C++ (Arduino IDE), TFT SPI library
- **Backend:** Cloudflare Workers (serverless)
- **APIs:** Google Calendar (OAuth2), OpenWeatherMap, Transit App

## Author

**Leila Constant**
[GitHub](https://github.com/Leilaconst) · [LinkedIn](https://www.linkedin.com/in/leila-constant-76639b3ab/)
