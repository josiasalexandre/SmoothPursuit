Smooth Pursuit Eye Movements - DSP Implementation of the Krauzlis and Lisberger Model

It's a simple project just to evaluate the Krauzlis and Lisberger Smooth Pursuit Eye Movements Control Model.

Usage:

  $ program input_video frame_rate output_video groundtruth
  
  The groundtruth file is optional and when provided it must follow the pattern:
  
  p1_x p1_y p2_x p2_y
  p1_x p1_y p2_x p2_y
  p1_x p1_y p2_x p2_y
  p1_x p1_y p2_x p2_y
  ...
  
  Each line corresponds to a frame, so the first line is the ground truth related to the first frame.
  
When the program starts you'll need to select the target object with double click. The region of interest will be the fovea.
You can change the fovea's size with the + and - buttons.
To start the experiment, you can press the 'q' button in your keyboard. The fovea will be the green box. The blue box is a
conventional Adaboost tracker provide by the OpenCV library.

You'll need OpenCV 3+.

To compile the project, go to the project folder and follow the instructions below:

    $ mkdir build
    
    $ cd build
  
    $ cmake ..
    
    $ make
  
Examples:



  

