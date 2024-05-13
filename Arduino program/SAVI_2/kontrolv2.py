import BlynkLib
import time
import serial

BLYNK_AUTH = '71AJqWwmzH6ecjSnuOpuaDLrRjGbZpCB'

# Initialize Blynk
blynk = BlynkLib.Blynk(BLYNK_AUTH)

# Global dictionary to store values for multiple virtual pins
virtual_pin_values = {}

# Register virtual pin handlers

#Maju
@blynk.on("V0")
def v0_write_handler(value):
    global virtual_pin_values
    virtual_pin_values["V0"] = value[0]
    print('Current slider value for V0: {}'.format(value[0]))

#Mundur
@blynk.on("V1")
def v1_write_handler(value):
    global virtual_pin_values
    virtual_pin_values["V1"] = value[0]
    print('Current slider value for V1: {}'.format(value[0]))

#Kiri
@blynk.on("V2")
def v2_write_handler(value):
    global virtual_pin_values
    virtual_pin_values["V2"] = value[0]
    print('Current slider value for V2: {}'.format(value[0]))

# Kanan
@blynk.on("V3")
def v3_write_handler(value):
    global virtual_pin_values
    virtual_pin_values["V3"] = value[0]
    print('Current slider value for V3: {}'.format(value[0]))

# SerongDKiri
@blynk.on("V4")
def v4_write_handler(value):
    global virtual_pin_values
    virtual_pin_values["V4"] = value[0]
    print('Current slider value for V4: {}'.format(value[0]))

# SerongDkanan
@blynk.on("V5")
def v5_write_handler(value):
    global virtual_pin_values
    virtual_pin_values["V5"] = value[0]
    print('Current slider value for V5: {}'.format(value[0]))

# SerongBKiri
@blynk.on("V6")
def v6_write_handler(value):
    global virtual_pin_values
    virtual_pin_values["V6"] = value[0]
    print('Current slider value for V6: {}'.format(value[0]))

# SerongBkanan
@blynk.on("V7")
def v7_write_handler(value):
    global virtual_pin_values
    virtual_pin_values["V7"] = value[0]
    print('Current slider value for V7: {}'.format(value[0]))

def arduino_write(command):
    arduino.write(command)

def arduino2_write(command):
    arduino2.write(command)
    

def response(chat) :
    #Bagian untuk respon Api
    response_tag = chat
    ##
    if(response_tag == 'SaVi.maju'):
        arduino_write(str.encode('{"chatbot":"Maju"}'))
        print("maju")
        time.sleep(1)

    elif(response_tag == 'SaVi.mundur'):
        arduino_write(str.encode('{"chatbot":"Mundur"}'))
        print("mundur")
        time.sleep(1)

    elif response_tag == 'SaVi.kiri':
        arduino2_write(str.encode('{"chatbot":"Kiri"}'))
        print("kiri")
        time.sleep(1)

    elif response_tag == 'SaVi.kanan':
        arduino2_write(str.encode('{"chatbot":"Kanan"}'))
        print("kanan")
        time.sleep(1)

    elif response_tag == 'SaVi.serong_d_kiri':
        arduino2_write(str.encode('{"chatbot":"SerongdKiri"}'))
        print("serong_d_kiri")
        time.sleep(1)

    elif response_tag == 'SaVi.serong_d_kanan':
        arduino2_write(str.encode('{"chatbot":"SerongdKanan"}'))
        print("serong_d_kanan")
        time.sleep(1)

    elif response_tag == 'SaVi.serong_b_kiri':
        arduino2_write(str.encode('{"chatbot":"SerongbKiri"}'))
        print("serong_b_kiri")
        time.sleep(1)

    elif response_tag == 'SaVi.serong_b_kanan':
        arduino2_write(str.encode('{"chatbot":"SerongbKanan"}'))
        print("serong_b_kanan")
        time.sleep(1)     

    elif(response_tag == 'SaVi.stop'):
        arduino_write(str.encode('{"chatbot":"Stop"}'))
        arduino2_write(str.encode('{"chatbot":"Stop"}'))
        print("stop")
        time.sleep(1)

arduino = serial.Serial('COM15',115200)
arduino2 = serial.Serial('COM5',115200)

while True:
    blynk.run()
    # Now you can access values using virtual_pin_values globally
    if "V0" in virtual_pin_values and virtual_pin_values["V0"] == "1":
        response('SaVi.maju')
    elif "V1" in virtual_pin_values and virtual_pin_values["V1"] == "1":
        response('SaVi.mundur')
    elif "V2" in virtual_pin_values and virtual_pin_values["V2"] == "1":
        response('SaVi.kiri')
    elif "V3" in virtual_pin_values and virtual_pin_values["V3"] == "1":
        response('SaVi.kanan')
    elif "V4" in virtual_pin_values and virtual_pin_values["V4"] == "1":
        response('SaVi.serong_d_kiri')
    elif "V5" in virtual_pin_values and virtual_pin_values["V5"] == "1":
        response('SaVi.serong_d_kanan')
    elif "V6" in virtual_pin_values and virtual_pin_values["V6"] == "1":
        response('SaVi.serong_b_kiri')
    elif "V7" in virtual_pin_values and virtual_pin_values["V7"] == "1":
        response('SaVi.serong_b_kanan')
    else:
        response('SaVi.stop')

    time.sleep(0.1)
