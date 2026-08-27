
//Geo Location -- for transit
async function geocodeLocation(address) {
  const url = new URL("https://nominatim.openstreetmap.org/search");
  url.searchParams.set("q", address);
  url.searchParams.set("format", "json");
  url.searchParams.set("limit", "1");

  const response = await fetch(url.toString(), {
    headers: { "User-Agent": "smart-commute-assistant (personal project)" }
  });

  if (!response.ok) {
    throw new Error(`Geocoding request failed with status ${response.status}`);
  }

  const results = await response.json();

  if (!results || results.length === 0) {
    throw new Error(`No geocoding results found for "${address}"`);
  }

  return {
    lat: parseFloat(results[0].lat),
    lon: parseFloat(results[0].lon)
  };
}

function isoToUnixSecs(calEventStart){
  // Convert to Unix seconds
  const unixSeconds = Math.floor(Date.parse(calEventStart) / 1000);


  return unixSeconds;
}

function unixTo12(unixTimestamp){
  const date = new Date(unixTimestamp * 1000);

  // Convert to 12-hour format string
  const time12h = date.toLocaleTimeString('en-US', { 
    hour12: true, 
    hour: 'numeric', 
    minute: 'numeric',
    timeZone: "America/Toronto" 
  });

  return time12h.toString();
}

function isoTo12(isoString){
  if (!isoString) return null;

  const date = new Date(isoString);

  return date.toLocaleTimeString("en-US", {
    hour: "numeric",
    minute: "2-digit",
    hour12: true,
    timeZone: "America/Toronto"
  });
}

function formatTravelDuration(totalMinutes) {
  if (!totalMinutes && totalMinutes !== 0) return null;

  const hours = Math.floor(totalMinutes / 60);
  const minutes = totalMinutes % 60;

  if (hours > 0) {
    return `${hours}h ${minutes}min Travel`;
  }
  return `${minutes}min Travel`;
}

//Get the transit route for the next event
async function getTransitRoute(env, destinationText, arriveBy) {
  // Skip entirely if there's no destination to route to
  if (!destinationText) {
    return { route_summary: null, travel_duration_seconds: null };
  }

  const destination = await geocodeLocation(destinationText);
  
  const url = new URL("https://external.transitapp.com/v4/public/plan");
  url.searchParams.set("from_lat", env.HOME_LAT);
  url.searchParams.set("from_lon", env.HOME_LON);
  url.searchParams.set("to_lat", destination.lat.toString());
  url.searchParams.set("to_lon", destination.lon.toString());
  url.searchParams.set("mode", "transit");
  url.searchParams.set("arrival_time", isoToUnixSecs(arriveBy));

  const response = await fetch(url.toString(), {
    headers: { apiKey: env.TRANSIT_API_KEY }
  });

  if (!response.ok) {
    const errorBody = await response.text();
    throw new Error(`Transit request failed with status ${response.status}: ${errorBody}`);
  }

  const data = await response.json();

  if (!data.results || data.results.length === 0) {
    return { route_summary: "No route found", travel_duration_seconds: null };
  }

  const bestTrip = data.results[0];

  let route_duration;

  const routeSummary = bestTrip.legs
    .map(leg => {
      route_duration = Math.round(leg.duration/60);
      if (leg.leg_mode === "walk"){
        return `Walk for ${route_duration} mins`;}
      if (leg.leg_mode === "personal_bike"){
        return "Bike";
      } 
      else{
        return `Take the ${leg.routes[0].route_short_name} for ${route_duration} minutes` || "null";
      }
    })
    .join(" , ");

  return {
    route_summary: routeSummary,
    travel_duration: formatTravelDuration(Math.round(bestTrip.duration/60)),
    leave_by: bestTrip.start_time
  };
}

function getTimeUntilLeaveBy(leaveByUnixSeconds) {
  if (!leaveByUnixSeconds) return null;

  const nowMs = Date.now();               // current time in milliseconds
  const leaveByMs = leaveByUnixSeconds * 1000;  // convert seconds → milliseconds

  const diffMs = leaveByMs - nowMs;

  if (diffMs <= 0) {
    return "Time to leave now";
  }

  const totalMinutes = Math.floor(diffMs / 1000 / 60);
  const hours = Math.floor(totalMinutes / 60);
  const minutes = totalMinutes % 60;

  if (hours > 0) {
    return `${hours}h ${minutes}m`;
  }
  return `${minutes}m`;
}

function hasLeaveByPassed(leaveByUnixSeconds) {
  if (!leaveByUnixSeconds) return null; // no leave-by time to check against

  const nowSeconds = Math.floor(Date.now() / 1000);

  return nowSeconds >= leaveByUnixSeconds;
}


// Call to OpenWeatherMap API --> weather data
const CITY = 'Niagara Falls,ON,CA';

async function getWeather(env) {
  const url = `https://api.openweathermap.org/data/2.5/weather?q=${encodeURIComponent(CITY)}&units=metric&appid=${env.OPENWEATHER_API_KEY}`;

  const response = await fetch(url);

  if (!response.ok) {
    const errorBody = await response.text();
    throw new Error(`OpenWeatherMap request failed with status ${response.status}: ${errorBody}`);
  }

  const data = await response.json();
  const temp_c = Math.round(data.main.temp);
  const condition = data.weather[0].main;
  const condition_note = data.weather[0].description;

  return {
    temp_c : `${temp_c}C`,
    condition,
    condition_note,
    weather_note: getDressNote(temp_c, condition)
  };
}

//Call to Google Calendar
async function getNextEvent(env) {
  const tokenResponse = await fetch("https://oauth2.googleapis.com/token", {
    method: "POST",
    headers: { "Content-Type": "application/x-www-form-urlencoded" },
    body: new URLSearchParams({
      client_id: env.GOOGLE_CLIENT_ID,
      client_secret: env.GOOGLE_CLIENT_SECRET,
      refresh_token: env.GOOGLE_REFRESH_TOKEN,
      grant_type: "refresh_token"
    })
  });

  if (!tokenResponse.ok) {
    const errorBody = await tokenResponse.text();
    throw new Error(`Token refresh failed with status ${tokenResponse.status}: ${errorBody}`);
  }

  const tokenData = await tokenResponse.json();
  const accessToken = tokenData.access_token;

  const now = new Date().toISOString();
  const calendarUrl = new URL("https://www.googleapis.com/calendar/v3/calendars/primary/events");
  calendarUrl.searchParams.set("timeMin", now);
  calendarUrl.searchParams.set("singleEvents", "true");
  calendarUrl.searchParams.set("orderBy", "startTime");
  calendarUrl.searchParams.set("maxResults", "1");

  const eventsResponse = await fetch(calendarUrl.toString(), {
    headers: { Authorization: `Bearer ${accessToken}` }
  });

  if (!eventsResponse.ok) {
    const errorBody = await eventsResponse.text();
    throw new Error(`Calendar request failed with status ${eventsResponse.status}: ${errorBody}`);
  }

  const eventsData = await eventsResponse.json();

  // Handle the "nothing on the calendar" case
  if (!eventsData.items || eventsData.items.length === 0) {
    return {
      next_event: "No upcoming events",
      leave_by: "N/A"
    };
  }

  const nextEvent = eventsData.items[0];

  return {
    next_event: nextEvent.summary || "Untitled event",
    event_start: nextEvent.start.dateTime || nextEvent.start.date,
    event_location: nextEvent.location || null
  };
}

//Generate the Dress Note based on the weather
function getDressNote(tempC, condition) {
  const wetConditions = ["Rain", "Drizzle", "Thunderstorm"];
  const isSnowy = condition === "Snow";
  const isWet = wetConditions.includes(condition);

  let tempPhrase;
  if (tempC <= -10) {
    tempPhrase = "very cold";
  } else if (tempC <= 0) {
    tempPhrase = "cold";
  } else if (tempC <= 10) {
    tempPhrase = "cool";
  } else if (tempC <= 22) {
    tempPhrase = "mild";
  } else if (tempC <= 28) {
    tempPhrase = "warm";
  } else {
    tempPhrase = "hot";
  }

  let advice;
  if (tempC <= 0) {
    advice = "dress warm";
  } else if (tempC <= 10) {
    advice = "wear a jacket";
  } else if (tempC <= 22) {
    advice = "a light layer should be fine";
  } else {
    advice = "dress light";
  }

  let conditionPhrase = "";
  if (isSnowy) {
    conditionPhrase = " and snowy";
  } else if (isWet) {
    conditionPhrase = " and rainy";
  }

  return `It is ${tempPhrase}${conditionPhrase}, ${advice}.`;
}

export default {
  async fetch(request, env, ctx) {
    const provided_key = request.headers.get("X-Device-Key");

    if (provided_key !== env.DEVICE_AUTH_KEY){
       return new Response("Unauthorized", {status: 401});
     }

    let weather;
    try {
      weather = await getWeather(env);
    } catch (error) {
      console.error('Error fetching weather data:', error.message);
      // fallback so the response still works if the API call fails
      weather = {
        temp_c: null,
        condition: "Unknown",
        condition_note: "Unknown",
        weather_note: "Weather data unavailable"
      };
    }

    let calendar;
    try {
      calendar = await getNextEvent(env);
    } catch (error) {
      console.error('Error fetching calendar data:', error.message);
      calendar = { next_event: "Unavailable", event_start: null };
    }

    let transit;
    try {
      transit = await getTransitRoute(env, calendar.event_location, calendar.event_start);
    } catch (error) {
      console.error('Error fetching transit data:', error.message);
      transit = { route_summary: "Unavailable", travel_duration_seconds: null, leave_by:  "Unavailable"};
    }

    const timeUntilLeave = getTimeUntilLeaveBy(transit.leave_by);

    const leaveByPassed = hasLeaveByPassed(transit.leave_by);

    if (leaveByPassed) {
      transit.route_summary = "No routes";
      transit.leave_by = "Passed";
    }

    const data = {
      ...weather,
      ...calendar,
      ...transit,
      event_start: isoTo12(calendar.event_start),
      leave_by: leaveByPassed ? "Passed" : unixTo12(transit.leave_by),
      time_remain: timeUntilLeave
    };

    return new Response(JSON.stringify(data), {
      headers: { "Content-Type": "application/json" }
    });
  }
};