# Project Proposal
**Idea**: My project is an audio visualizer for Spotify. While playing music on Spotify, the program should be able to show a music visualization that goes along with the song currently playing.

## Spotify:
To connect to Spotify, I am going to be using the [Spotify Web API](https://beta.developer.spotify.com/documentation/web-api/). I have used a Rest API in a previous project and this API is in a similar format. Using this API, I am able to get a user’s currently playing song, how far into the song the user is, and audio analysis of a track which I can use for the visualization. This [analysis](https://beta.developer.spotify.com/console/get-audio-analysis-track/?id=4mnVTZqDC2fy2Sh1ooKEse) gives the bars, beats, tatums, pitches, and timbre of a track at various points throughout the song. The audio visualizer can be based off of this idea

[C++ Rest API Library](https://github.com/Microsoft/cpprestsdk): The library to access the Spotify Rest API

## OpenFrameworks:
[Application core](http://openframeworks.cc/documentation/application/) : Used to create the window application

[Graphics](http://openframeworks.cc/documentation/graphics/) and/or [3d](http://openframeworks.cc/documentation/3d/): Used for the visualization. Can create 2d and/or 3d visualizations based on the audio analysis provided by Spotify

[ofxGui](http://openframeworks.cc/documentation/ofxGui/): Can be used to create buttons that changes visualizer settings or add controls to play/pause Spotify from this application

## Extra feature - Lyrics:
As an extra feature, I could try to add lyrics to overlay on the visualizer. There are API's that can give you lyrics based on the song name, but I haven't found any free ones that can sync with the music
