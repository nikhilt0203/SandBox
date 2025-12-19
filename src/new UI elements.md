New UI elements:

    1. Must be derived from UIElement in UIElements.h (and implement a render method)

    2.  Must have immediately determinable dimensions and coordinates so that other elements in the frame can use align methods. Dimensions and coordinates must be calculated BEFORE rendering

    3. Must have a GFXCanvas pointer as the last argument in the constructor. Typically the buffer would be passed into draw(), but the Adafruit GFX library can't determine the dimensions of text until it is actually printed.