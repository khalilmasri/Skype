# Client

Info on setting up environment and running the client. 

# Installing dependencies/libraries

### GUI build with QT6.

Install on Arch:

        sudo pacman -S qt6-base qt6-doc qt6-multimedia

Ubuntu

        sudo apt-get install libsdl2-2.0 libsdl2-dev
        
Mac OSX

        brew install qt@6 


### SDL

Arch

       sudo pacman -S sdl2


Mac OSX 

       homebrew install sdl2


### OpenCV

Arch

        sudo pacman -S glew opencv hdf5 vtk
        
Mac OSX 

       homebrew install opencv@2
        
### FFmpeg

Arch
       
       sudo pacman -S ffmpeg

Mac OSX 

       homebrew install ffmpeg
       
       brew info ffmpeg (and brew install any libraries or dependencies that are missing)
       
      
  
# Running Client

Compile the client from the main skype directory

        cd [root directory name]
        
        ./build.sh --run client
        
User can then login as an existing user using a previously made account or one of the default testing accounts we used with the names:
        
        'mario', 
        'john', 
        'shakira',
        'chris' 
        
        all with password '1234'

If new user registration, click new user and proceed to new user creation page, you must then login as newly created user from the original login page. 

### Contact List:

        The contact list on the left side shows online and offline contacts, and has buttons at the bottom to search for, add new, and delete current               contact.
        New users must add at least one contact in order to view the chat/call windows.
        Clicking on contact will select them as current until another is selected. 
        
### Chat Window:

        The chat window is a simple UI which shows the name of the current contact window at the top left. and has a call button at the top         right when that contact is online.
        Bottom has a text message toolbar which allows for sending messages when the contact is both online and offline, and the main chat window which             will display the correspondance with said contact.
        
### Audio/Video Call Window:
        
        This window will appear when a call is requested by the user, or if incoming call is received. 
        Every action will be confirmed by notification messages in the UI, within a call the user has option to turn on or off video calling, audio calling         will always be active in either, until call is terminated by either the hangup button or the close window button in top left. 
        
### Logging Out:

        User will be logged off server automatically on termination of their connection or termination of the client window by clicking the close window           button in top left of the client GUI.
        

        
