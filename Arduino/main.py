import serial 
import time 
import serial.tools.list_ports as ports
import spotipy
from spotipy.oauth2 import SpotifyOAuth
from spotipy import Spotify
import json
from dotenv import load_dotenv
import os

arduino = serial.Serial(port='COM4', baudrate=115200, timeout=.1) 

load_dotenv()

birdy_uri = 'spotify:artist:2WX2uTcsvV5OnS0inACecP'

sp_oauth = SpotifyOAuth(client_id=os.getenv('CLIENT_ID'),
                        client_secret==os.getenv('CLIENT_SECRET'),
                        redirect_uri=os.getenv('REDIRECT_URI'), scope="user-read-currently-playing")



sp = Spotify(auth_manager=sp_oauth)

results = sp.current_user_playing_track()

if results:
    print(results["item"]["name"])
    print(results["item"]["artists"][0]["name"])
else:
    print("Not Playing")


def write_read(x): 
	   arduino.write(bytes(x, 'utf-8')) 
	   time.sleep(0.05) 
	   data = arduino.readline() 
	   return data 


while True: 
	   num = input("Enter a number: ") # Taking input from user 
	   value = write_read(num) 
	   print(value) # printing the value 