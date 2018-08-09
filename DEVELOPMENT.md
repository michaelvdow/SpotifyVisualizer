4/17/18:
I have figured out how to use cpprestsdk for OAuth 2.0 so I can retrieve the access tokens on Spotify. This took a lot of time because I have not yet worked with OAuth before. I based my OAuth classes off of the example given on the GitHub repository for this library. I am now able to request user access and can retreive information from the Spotify API. I think the next step is determining how to process the data I receive from the Spotify API

4/21/18:
I created objects for different parts of each song including segments, beats, and sections. I also created a visualizer object that would process the json from the Spotify API into these segment, beat, and section objects. The visualizer would also keep track of the current time in the song so that it could update each bar appropriately

4/22/18:
Connected the audio visualizer object with the Spotify API in the ofApp class so that the screen begins drawing the visualizer. Currently, the visualization is fairly choppy and needs to be worked on smoothing between each time change.

4/23/18:
Visualizer now updates based on the current song. I had issues with creating background thread to check if there was a new song and then processing that data. I figured out that it was because I was still attempting to update the audio visualizer during processing so it would try to get the next beat or segment from an empty array. I fixed this using an flag so that it would only porocess when not updating. I also continued to fix the smoothing and experiment with the visualization by adding randomness to the beat bars

4/24/18:
Add labels for the current song and artist which could be hidden with H, and added a fullscreen button with F. Bars now change size based on window size.

4/25/18:
Created destructor to delete pointers to label objects. Added gui to change the color of the bars as well as randomize the color every beat. The randomize color updates in a background thread every 50 ms until it is untoggled. It will calculate how far the song is into the current beat and appropriately update the color