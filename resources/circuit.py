class Lights():
    def __init__(self : Lights) -> None:
        self.lights = [1, 0, 0, 0]

    def __str__(self : Lights) -> str:
        """ Returns a string representation of the lights. """
        return str(self.lights)
    
    def publish_lights(lights_topic : str, lights_subtopic : str) -> None:
        """ Publishes the current state of the lights to the broker. """
        for i in range(4):
            client.publish(f'{lights_topic}/{lights_subtopic}{i}', self.lights[i]) # Publishes the state of each light into {light_sgubtopic}{0, 1, 2, 3} topic

    def update_lights(lights : list = self.lights) -> list:
        """ Updates the state of the lights. """
        self.lights = lights
        for idx, element in enumerate(self.lights):
            if element:
                if idx == len(self.lights)-1:
                    self.lights[0], self.lights[idx] = 1, 0
                else:
                    self.lights[idx+1], self.lights[idx] = 1, 0
                return self.lights
    
    def get_lights_off() -> list:
        """ Returns indexes of lights that are off. """
        idxs = []
        for idx, light in enumerate(self.lights):
            if light == 0:
                idxs.append(idx)
        return idxs
    
    def delay(light_time : int, do_sequence : bool) -> None:
        """ Delay for light time. """
        start = time.time()
        while time.time()-start < light_time and do_sequence == True:
            pass

class Buttons():
    def __init__(self : Buttons) -> None:
        self.button_array = [0, 0, 0, 0]
    
    def update_buttons(index : int, value : int) -> None:
        """ Updates the state of the buttons. """
        self.button_array[index] = value

    def button_pressed() -> bool:
        """ Returns True if any button is pressed. """
        return self.button_array != [0, 0, 0, 0]
    
    def get_buttons_state() -> list:
        """ Returns the state of the buttons. """
        return self.button_array

    def wait_for_crossing(which_buttons : list) -> int:
        """ Returns 1 when detects a button activation on any index passed as parameter"""
        for button_index in which_buttons:
            if self.button_array[button_index]:
                return 0
        return 1