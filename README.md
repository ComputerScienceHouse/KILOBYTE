# KILOBYTE
KILOBYTE (Kinetic Inovative Load Optimizing Basket for Your Transportation Efficiency) is a motorized shopping cart made from scrap parts left over from the KUDOS project.

![Comedic poster of a photograph of KILOBYTE containg the text KILOBYTE and Kinetic Innovative Load Optimizing Basket for Your Transportation Efficiency in a font which makes the letters look like they are on fire](images/poster.jpg)

# Timeline

<div>
<img src="images/original_blueprints.jpg" align="right" width="300" style="margin: 20px; margin-top: 0px">
The original blueprints for Kilobyte, at the time simply named "Motorized Shopping Cart", were much simpler than current designs. The cart would include one wheel at the back, with one central throttle. This design failed to take into account steering and stability. 
</div>
<br>
<br>
<div style="margin-top: 100px;">
<img src="images/kilobyte-prototype-one.jpg" align="left" width="300" style="margin: 20px; margin-top: 0px">

The first prototype was completed overnight for CSH's Major-projectathon by Riley Barshak and Tyler Severino. This design reflects many fundamental changes to our plans for Kilobyte. We decided to change from a one wheel design to a two wheel design. Our design features two rear wheels, each with their own motor and throttle. This allows for steering, greater stability, and more power overall. In this prototype, throttle buttons were loosley held onto the handle, and the potentiometer was mounted at the bottom, used to control velocity. Velocity control (speed & direction) were based on potentiometer values 0-255 where, 0-127 sets reverse speeds, 128 is idle, and 129-255 set forward speeds. 
</div>
<br>
<br>
<div style="margin-top: 100px;">
<img src="images/kilobyte-controlpanel.jpg" align="right" width="300" style="margin: 20px; margin-top: 0px">

The next iteration of Kilobyte introduces many structural and user interface improvements. The most prominent new feature is the control panel. The control panel includes many useful features and status displays. The right-most button is on/off (with a status LED), the left-most button is forward/reverse (with a status LED), and the potentiometer in the middle controls speed (not velocity anymore). Additionally, there is a digital display to show battery percentage/voltage, throttle percentage, forward/reverse status, and controller connectivity status (for plans to make Kilobyte controllable with an xbox controller). Also coming with this version, electronics were reorganized, with many being placed within the control panel, and structural improvements were made to the bottom. 
</div>

<br>
<br>
<br>
<br>

# In Progress (Revision 3)
- Battery/Power
  - [x] Implement new LIFePo4 Battery
    - [x] Attatch Anderson Powerpole connector
    - [x] Design and implement new mounting brackets
    - [ ] Test battery lifetime
  - [x] Add 5V regulator to power display unit and control unit
- Display Unit
  - [x] Implement new display unit housing
    - [x] Create new Arduino shield for power and all IO
  - [ ] Upgrade and finalize firmware
    - [ ] Show battery voltage and percentage on screen
    - [ ] Calculate RPM and velocity from hall effect sensor
    - [ ] Implement master software shutoff switch
    - [ ] Make UI more user friendly
    - [ ] Add safeguards for when communication to control unit is lost
    - [x] Implement UI LEDs
    - [x] 
- Control Unit
  - [ ] Implement acceleration smoothing to reduce initial current and QOL
  - [ ] Ensure that if any error occurs, motors always default to a safe state
- Electronics
  - [ ] Finalize mounting off all electronics and batteries
    - [ ] Test mounting for strength and neatness
  - [x] Ensure all electronics are properly fused
  - [ ] Ground entire chassis?
  - [ ] Neatly route power+communication wiring to display unit
    - [ ] Design custom cable harness/mounting atachments?
  - [ ] Cut and paint the board electronics will be mounted on
- Safety
  - [ ] Detect human presence on chariot, do not allow operation if not detector
  - [ ] Add configurable speed limiter
  - [ ] Add a hardware battery disconnect
  - [ ] Test all automatic fuses
- Cosmetics
  - [x] Implement CSH colors in all parts where possible
  - [ ] Add custom accents and eye candy/decorations
  - [ ] Decide on a primary color (CSH colors are accents)
    - Should we replace the original blue plastic parts of the cart with custom 3d printed alternatives?

