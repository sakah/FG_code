#!/bin/sh

VXI_EXEC="./test_cmd"
FG_ip=192.168.10.1

turn_on()
{
   ch=$1
   ${VXI_EXEC} ${FG_ip} "OUTPut$ch:STATe on"
}

turn_off()
{
   ch=$1
   ${VXI_EXEC} ${FG_ip} "OUTPut$ch:STATe off"
}

set_polarity()
{
   ch=$1
   polarity=$2
   ${VXI_EXEC} ${FG_ip} "OUTPUT$ch:POLARITY ${polarity}"
}

set_voltage()
{
   ch=$1
   high=$2
   low=$3
   ${VXI_EXEC} ${FG_ip} "SOURce$ch:VOlTage:HIGH ${high}"
   ${VXI_EXEC} ${FG_ip} "SOURce$ch:VOlTage:LOW ${low}"
}

set_shape()
{
   ch=$1
   shape=$2
   ########
   # Shape
   # SINusoid|SQUare|PULSe|RAMP|PRNoise|DC|SINC|GAUSsian|
   # LORentz|ERISe|EDECay|HAVersine
   ########
   ${VXI_EXEC} ${FG_ip} "SOURce${ch}:Function:Shape ${shape}"
}

set_pulse()
{
   set_shape $1 PULSE
}

set_pulse_delay()
{
   ch=$1
   delay=$2
   ${VXI_EXEC} ${FG_ip} "SOURce${ch}:PULSE:DELAY ${delay}"
}

set_pulse_width()
{
   ch=$1
   width=$2
   ${VXI_EXEC} ${FG_ip} "SOURce${ch}:PULSE:WIDTH ${width}"
}

set_pulse_period()
{
   ch=$1
   period=$2
   ${VXI_EXEC} ${FG_ip} "SOURce${ch}:PULSE:PERIOD ${period}"
}

turn_off 1
turn_off 2

# Test pulse
set_pulse 1
set_pulse_period 1 1ms
set_pulse_width 1 100ns
set_pulse_delay 1 0ns
set_voltage 1 0mV -100mV
set_polarity 1 inverted

# Trigger
set_pulse 2
set_pulse_period 2 1ms
set_pulse_width 2 100ns
set_pulse_delay 2 500ns
set_voltage 2 0mV -800mV
set_polarity 2 inverted

turn_on 1
turn_on 2
