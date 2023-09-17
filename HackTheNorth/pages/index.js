
import { useState, useEffect } from 'react';
import axios from 'axios';
import Modal from 'react-modal';
import styles from "./index.module.css";

import LoadingSpinner from './LoadingSpinner';
const CLIENT_ID = '864203813270-htf1k8m4uifhtind4u4vrv78h014vss1.apps.googleusercontent.com'; // Replace with your actual Client ID
const REDIRECT_URI = 'http://localhost:3000'; // Replace with your actual redirect URI
const CALENDAR_API_KEY = 'AIzaSyCtiGh8gS3C48M9p1ICcbufsG92vKM9kyw'; // Replace with your actual Calendar API Key
const GOOGLE_AUTH_URL = `https://accounts.google.com/o/oauth2/auth?response_type=code&client_id=${CLIENT_ID}&redirect_uri=${REDIRECT_URI}&scope=https://www.googleapis.com/auth/calendar`;

export default function Home() {
  const [accessToken, setAccessToken] = useState('');
  const [events, setEvents] = useState([]);
  const [todayDate, setTodayDate] = useState('');
  const [newEventFormVisible, setNewEventFormVisible] = useState(false); // Declare newEventFormVisible state
  const [newEvent, setNewEvent] = useState({
    summary: '',          // Event title
    startDateTime: '',    // Start date and time
    endDateTime: '',      // End date and time
    // location: '',         // Event location

  });
  const [showModal, setShowModal] = useState(false);
  const [isLoading, setIsLoading] = useState(true); // Initialize as loading

  const handleCloseModal = () => {
    setShowModal(false);
  };

  const handleShowModal = () => {
    setShowModal(true);
  };
  
  const handleInputChange = (event) => {
    const { name, value } = event.target;
    setNewEvent({
      ...newEvent,
      [name]: value,
    });
  };

  const createAndSyncEvent = async () => {
    const eventData = {
      summary: newEvent.summary,
      start: {
        dateTime: newEvent.startDateTime + (newEvent.startSeconds ? `:${newEvent.startSeconds}` : ':00'), // Include seconds
        timeZone: 'America/New_York',
      },
      end: {
        dateTime: newEvent.endDateTime + (newEvent.endSeconds ? `:${newEvent.endSeconds}` : ':00'), // Include seconds
        timeZone: 'America/New_York',
      }
      // location: newEvent.location, // Add location field
    };

    const notificationTime = new Date(newEvent.startDateTime);
  notificationTime.setMinutes(notificationTime.getMinutes() - 10);

    try {
      // Send a POST request to create a new event
      const response = await axios.post(
        'https://www.googleapis.com/calendar/v3/calendars/primary/events',
        eventData,
        {
          headers: {
            Authorization: `Bearer ${accessToken}`,
          },
        }
      );

      // Update the events state with the newly created event
      const createdEvent = {
        id: response.data.id,
        summary: response.data.summary,
        start: parseTime(response.data.start.dateTime),
        end: parseTime(response.data.end.dateTime)
        // location: response.data.location || 'No location available', // Handle location
      };
      setEvents((prevEvents) => [...prevEvents, createdEvent]);

      sendNotification(newEvent.summary, notificationTime);

      // Clear the new event form
      setNewEvent({
        summary: '',
        startDateTime: '',
        endDateTime: '',
        // location: '', // Clear location field
      });
    } catch (error) {
      console.error('Error creating and syncing event:', error);
    }
  };

  const sendNotification = async (eventTitle, notificationTime) => {
    try {
      const response = await fetch('/send-notification', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({
          eventTitle,
          notificationTime,
        }),
      });
  
      if (response.ok) {
        console.log('Notification sent successfully.');
      } else {
        console.error('Failed to send notification.');
      }
    } catch (error) {
      console.error('Error sending notification:', error);
    }
  };
  
  useEffect(() => {
    const urlParams = new URLSearchParams(window.location.search);
    const code = urlParams.get('code');

    if (code) {
      // If we have an authorization code, exchange it for an access token
      exchangeCodeForAccessToken(code);
    }
  }, []);

  useEffect(() => {
    // Get today's date and set it as a header
    const today = new Date().toLocaleDateString('en-US', {
      // weekday: 'long',
      year: 'numeric',
      month: 'long',
      day: 'numeric',
    });
    setTodayDate(today);
  }, []);


  const exchangeCodeForAccessToken = async (code) => {
    try {
      const response = await axios.post('https://oauth2.googleapis.com/token', {
        code,
        client_id: CLIENT_ID,
        client_secret: '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', // Replace with your actual Client Secret
        redirect_uri: REDIRECT_URI,
        grant_type: 'authorization_code',
      });

      const { access_token } = response.data;
      setAccessToken(access_token);

      // Fetch calendar events using the access token
      fetchCalendarList(access_token);
      setIsLoading(true); // Set loading to true while fetching

      } catch (error) {
      console.error('Error exchanging code for access token:', error);
    }
  };

  const fetchCalendarList = async (token) => {
    try {
      const response = await axios.get(
        'https://www.googleapis.com/calendar/v3/users/me/calendarList',
        {
          headers: {
            Authorization: `Bearer ${token}`,
          },
        }
      );

      // Extract the list of calendar IDs from the response.
      const calendarIds = response.data.items.map((calendar) => calendar.id);

    //   // Fetch events from each calendar.
    //   for (const calendarId of calendarIds) {
    //     await fetchCalendarEvents(token, calendarId);
    //   }
    // } catch (error) {
    //   console.error('Error fetching calendar list:', error);
    // }
    const tempEvents = [];

    for (const calendarId of calendarIds) {
      const calendarEvents = await fetchCalendarEvents(token, calendarId);
      tempEvents.push(...calendarEvents);
    }

    // Sort and set the combined events array.
    const sortedCombinedEvents = tempEvents.sort((a, b) => {
      return new Date(a.start) - new Date(b.start);
    });

    setEvents(sortedCombinedEvents);
    setIsLoading(false); // Set loading to false when done

} catch (error) {
  console.error('Error fetching calendar list:', error);
}
};

  const fetchCalendarEvents = async (token, calendarId) => {
    const today = new Date().toLocaleString('en-US', { timeZone: 'America/New_York' });
  const todayStart = new Date(today);
  todayStart.setHours(0, 0, 0, 0);
  const todayEnd = new Date(today);
  todayEnd.setHours(23, 59, 59, 999);

  const minTime = todayStart.toISOString();
  const maxTime = todayEnd.toISOString();
    try {
      const response = await axios.get(
        `https://www.googleapis.com/calendar/v3/calendars/${calendarId}/events?timeMin=${minTime}&timeMax=${maxTime}`,
        {
          headers: {
            Authorization: `Bearer ${token}`,
          },
        }
      );


  const calendarEvents = response.data.items.map((event) => ({
    ...event,
    start: parseTime(event.start?.dateTime || event.start?.date),
    end: parseTime(event.end?.dateTime || event.end?.date),
  }));

    return calendarEvents;
  } catch (error) {
    console.error('Error fetching calendar events:', error);
    return [];
  }
  };


const parseTime = (dateTime) => {
if (!dateTime) return 'No time available';
const time = new Date(dateTime).toLocaleTimeString('en-US', {
  hour: 'numeric',
  minute: 'numeric',
  hour12: false, // Display in 12-hour format
});
return time;
};

  const handleLoginClick = () => {
    // Redirect the user to Google Sign-In
    window.location.href = GOOGLE_AUTH_URL;
  };

  const renderEventsForTimeSlot = (startTime, endTime) => {
    const timeSlotEvents = events.filter(
      (event) =>
        event.startTime <= endTime && event.endTime >= startTime
    );
    return timeSlotEvents.map((event, index) => (
      <div key={index} className="event">
        {event.title}
      </div>
    ));
  };

  const calculateMarginLeft = (startTime) => {
    // Parse the start time (assuming it's in HH:mm format)
    const [hours, minutes] = startTime.split(':');
    const totalMinutes = parseInt(hours) * 60 + parseInt(minutes);
  
    // Calculate margin based on the time difference from 6 am
    // Assuming events should be displayed from 6 am onwards
    const margin = (totalMinutes - 360) * 4; // Adjust the multiplier as needed
  
    return `${margin}px`;
  };
  
  const calculateEventWidth = (startTime, endTime) => {
    // Parse the start and end times (assuming they're in HH:mm format)
    const [startHours, startMinutes] = startTime.split(':');
    const [endHours, endMinutes] = endTime.split(':');
  
    // Calculate the duration in minutes
    const startTotalMinutes = parseInt(startHours) * 60 + parseInt(startMinutes);
    const endTotalMinutes = parseInt(endHours) * 60 + parseInt(endMinutes);
    const durationMinutes = endTotalMinutes - startTotalMinutes;
  
    // Calculate the width based on the duration
    // You can adjust the multiplier to control the width of each minute
    const width = durationMinutes * 4.5; // Adjust the multiplier as needed
  
    return `${width}px`;
  };
  
return (
  <div className = {styles.background}>
    <style jsx global>{`
        body {
          margin: 0px;
          padding: 0px;
        }
      `}</style>
    {/* <h1 className = {styles.header}>Calendar Events</h1> */}
    {accessToken ? (
      <div className = {styles.background2}>
        <div className = {styles.container}>
        <h2 className = {styles.date}>{todayDate}</h2>
        <button className = {styles.create} onClick={handleShowModal}>Create New Event</button>
        </div>

        <Modal
          isOpen={showModal}
          onRequestClose={handleCloseModal}
          contentLabel="Create New Event Modal"
          style={{
            content: {
              width: '700px',  // Adjust the width as needed
              height: '400px', // Adjust the height as needed
              margin: 'auto',  // Center the modal horizontally
            },
          }}          // Add any additional modal styles or properties here
        >
          <div className = {styles.window}>
          <h2 className = {styles.sub}>Create New Event</h2>
          <button className = {styles.close} onClick={handleCloseModal}>X</button>
          </div>
          <form  className = {styles.input}>
            <label className = {styles.label}>
              Event Title: 
              <input
                type="text"
                name="summary"
                value={newEvent.summary}
                onChange={handleInputChange}
                style={{ marginBottom: '10px' }} // Add margin to the input

              />
            </label>
            <br />
            <label className = {styles.label}>
              Start Time: 
              <input
                type="datetime-local"
                name="startDateTime"
                value={newEvent.startDateTime}
                onChange={handleInputChange}
                style={{ marginBottom: '10px' }} // Add margin to the input
              />
            </label>
            <br />
            
            <label className = {styles.label}>
              End Time: 
              <input
                type="datetime-local"
                name="endDateTime"
                value={newEvent.endDateTime}
                onChange={handleInputChange}
                style={{ marginBottom: '10px' }} // Add margin to the input

              />
            </label>
            <br />
            <button type="button" className = {styles.create} onClick={createAndSyncEvent}>
              Create and Sync Event
            </button>
          </form>
        </Modal>
        <div className={styles.centeredContainer}>
        <ul className={styles.eventList}>
        {isLoading ? (
  <LoadingSpinner /> // Show the loading spinner
) : (
        <div className={styles.eventContainer}>
  {events.map((event) => (
    <div  className={styles.eventBlock}
    key={event.id}
    style={{ marginLeft: calculateMarginLeft(event.start), width: calculateEventWidth(event.start, event.end) }}>
      <strong>{event.summary}</strong>
      <div className={styles.eventDetails}>
        <div className={styles.eventTime}>
          {event.start} - {event.end}
        </div>
        <div className={styles.eventLocation}>
          {event.location}
        </div>
      </div>
    </div>
  ))}
        </div>
)}
</ul>
</div>
      </div>
    ) : (   <div> 
    <h1 className = {styles.header}>Calendar Events</h1>
    <div className = {styles.contain}>
    <p className = {styles.description}>Sync with your Google Calendar to get reminders directly displayed on your laptop gadget!</p>

      <button className = {styles.login} onClick={handleLoginClick}> -{'>'} Log in with Google</button>
      </div>
      </div>
    )}
  </div>
);
}
