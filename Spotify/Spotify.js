const express = require('express');
const app = express();
const port = 3000;

// Step 3: Replace with your Spotify API credentials
const client_id = 'e49912916c8b4f0da13eb7f5391e02c8';
const client_secret = '5989bbaac193485ba7ebf343912ae56a';

// Step 4: Set up OAuth2 authorization
const auth_url = 'https://accounts.spotify.com/authorize';
const redirect_uri = `http://localhost:${port}/callback`; // Localhost callback URI
const scopes = ['user-read-currently-playing', 'user-read-playback-state']; // Scopes

// Generate the authorization URL
const auth_params = {
  client_id: client_id,
  response_type: 'code',
  redirect_uri: redirect_uri,
  scope: scopes.join(' ')
};

const auth_url_with_params = `${auth_url}?${new URLSearchParams(auth_params)}`;

console.log(`Open this URL in your browser: ${auth_url_with_params}`);

// Step 5: Exchange authorization code for access token
app.get('/callback', (req, res) => {
  const authorizationCode = req.query.code;

  // Base64 encode client_id and client_secret
  const credentials = Buffer.from(`${client_id}:${client_secret}`).toString('base64');

  // Request access token
  const token_url = 'https://accounts.spotify.com/api/token';
  const token_data = {
    grant_type: 'authorization_code',
    code: authorizationCode,
    redirect_uri: redirect_uri
  };

  fetch(token_url, {
    method: 'POST',
    headers: {
      'Content-Type': 'application/x-www-form-urlencoded',
      'Authorization': `Basic ${credentials}`
    },
    body: new URLSearchParams(token_data)
  })
    .then(response => response.json())
    .then(data => {
      const access_token = data.access_token;
      console.log('Access Token:', access_token);
      // You can now use the access token to make requests to the Spotify API.
      res.send('Authorization complete. You can close this window.');
    })
    .catch(error => {
      console.error('Error:', error);
      res.status(500).send('Error during authorization.');
    });
});

app.listen(port, () => {
  console.log(`Server is running on port ${port}`);
});


const fetch = require('node-fetch'); // If you haven't already installed the 'node-fetch' package, install it using 'npm install node-fetch'.

// Replace with your access token obtained from the previous steps
//const access_token = 'YOUR_ACCESS_TOKEN';

// Step 6: Make a GET request to the Spotify API for now playing information
const nowPlayingEndpoint = 'https://api.spotify.com/v1/me/player/currently-playing';

fetch(nowPlayingEndpoint, {
  method: 'GET',
  headers: {
    'Authorization': `Bearer ${access_token}`
  }
})
  .then(response => response.json())
  .then(data => {
    if (data && !data.error) {
      const nowPlayingTrack = data.item.name;
      const artist = data.item.artists.map(artist => artist.name).join(', ');

      console.log('Now Playing:', nowPlayingTrack);
      console.log('Artist:', artist);
    } else {
      console.error('Error:', data.error);
    }
  })
  .catch(error => console.error('Error:', error));
