
import serial_rpc
import time
import pytest
from matplotlib import pyplot as plt
import numpy as np
import csv

PORT_NAME = 'COM8'
BAUD_RATE = 460800

def ping_device(handle: serial_rpc.serial_rpc):
    handle.send_request('ping')
    return handle.get_response_parameter_value('ping', 'counter')

def set_input_frequency(handle: serial_rpc.serial_rpc, frequency: int):
    handle.set_request_parameter_value('input_signal_control', 'frequency', frequency)
    handle.send_request('input_signal_control')

def acquire(handle: serial_rpc.serial_rpc, sampling_rate: int, input_frequency: int, number_of_cycles: int = 2):

    number_of_samples_to_get = (number_of_cycles * sampling_rate) // input_frequency
    
    handle.send_request('dsp_status')
    sample_buffer_size = handle.get_response_parameter_value('dsp_status', 'sample_buffer_size')

    if number_of_samples_to_get > sample_buffer_size:
        raise IOError(f"Number of samples to get ({number_of_samples_to_get}) exceeds sample buffer size of {sample_buffer_size}")

    handle.set_request_parameter_value('dsp_control', 'reset' , 1)
    handle.set_request_parameter_value('dsp_control', 'start' , 1)
    handle.set_request_parameter_value('dsp_control', 'stop' , 1)
    handle.set_request_parameter_value('dsp_control', 'set_sampling_rate', 1)
    handle.set_request_parameter_value('dsp_control', 'sampling_rate', sampling_rate)
    handle.set_request_parameter_value('dsp_control', 'set_number_of_samples', 1)
    handle.set_request_parameter_value('dsp_control', 'number_of_samples', number_of_samples_to_get)

    handle.send_request('dsp_control')
       
    time.sleep( ((float(number_of_samples_to_get)/float(sampling_rate)) * 2.0) + 0.01 )

    handle.set_request_parameter_value('dsp_control', 'reset' , 0)
    handle.set_request_parameter_value('dsp_control', 'start' , 0)
    handle.set_request_parameter_value('dsp_control', 'stop' , 1)
    handle.set_request_parameter_value('dsp_control', 'set_sampling_rate', 0)
    handle.set_request_parameter_value('dsp_control', 'set_number_of_samples', 0)

    handle.send_request('dsp_control')

    handle.send_request('dsp_status')

    number_of_samples = handle.get_response_parameter_value('dsp_status', 'number_of_samples')

    input_samples = []
    output_samples = []
    timestamp = []
    
    start_time = time.perf_counter()

    for i in range(0, number_of_samples):

        timestamp.append( (float(i)/float(sampling_rate)) )

    if(number_of_samples <= 13):
        
        handle.set_request_parameter_value('dsp_get_samples', 'index', 0)
        handle.set_request_parameter_value('dsp_get_samples', 'length', number_of_samples)
        
        handle.send_request('dsp_get_samples')
        
        input_samples.extend(handle.get_response_parameter_value('dsp_get_samples', 'input_samples'))
        output_samples.extend(handle.get_response_parameter_value('dsp_get_samples', 'output_samples'))        

    elif(number_of_samples > 13):
        
        number_of_chunks = number_of_samples // 13
        
        for j in range(0, number_of_chunks):
            handle.set_request_parameter_value('dsp_get_samples', 'index', (13*j))
            handle.set_request_parameter_value('dsp_get_samples', 'length', 13)
            
            handle.send_request('dsp_get_samples')
            
            input_samples.extend(handle.get_response_parameter_value('dsp_get_samples', 'input_samples'))
            output_samples.extend(handle.get_response_parameter_value('dsp_get_samples', 'output_samples'))
        
        if((number_of_samples % 13) != 0):
            handle.set_request_parameter_value('dsp_get_samples', 'index', (13*number_of_chunks) )
            handle.set_request_parameter_value('dsp_get_samples', 'length', (number_of_samples % 13))
        
            handle.send_request('dsp_get_samples')
        
            input_samples.extend(handle.get_response_parameter_value('dsp_get_samples', 'input_samples'))
            output_samples.extend(handle.get_response_parameter_value('dsp_get_samples', 'output_samples'))

    end_time = time.perf_counter()

    return((timestamp, input_samples, output_samples))

import matplotlib.pyplot as plt

def plot_input_and_output_waveforms(timestamp, input_samples, output_samples):
    plt.figure(figsize=(12, 6))

    # Plot output first
    plt.plot(
        timestamp,
        output_samples,
        color="red",
        linestyle="-",
        linewidth=2.5,
        label="Output",
        zorder=2
    )

    # Plot input on top
    plt.plot(
        timestamp,
        input_samples,
        color="blue",
        linestyle=":",
        linewidth=2,
        marker="o",
        markersize=4,
        markevery=max(1, len(timestamp) // 30),
        label="Input",
        zorder=3
    )

    plt.title("Input vs Output Waveforms")
    plt.xlabel("Time (s)")
    plt.ylabel("Amplitude")
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.show()


def get_phase_and_amplitude(
    timestamp,
    input_frequency,
    input_samples,
    output_samples
):
    t = np.asarray(timestamp, dtype=float)
    x = np.asarray(input_samples, dtype=float)
    y = np.asarray(output_samples, dtype=float)

    if not (len(t) == len(x) == len(y)):
        raise ValueError("Sample arrays must have equal lengths")

    if len(t) < 4:
        raise ValueError("Insufficient samples")

    if input_frequency <= 0:
        raise ValueError("Frequency must be positive")

    # Fit both signals to:
    # signal = A*sin(w*t) + B*cos(w*t) + DC
    omega_t = 2.0 * np.pi * input_frequency * t

    design_matrix = np.column_stack((
        np.sin(omega_t),
        np.cos(omega_t),
        np.ones_like(t)
    ))

    cx, *_ = np.linalg.lstsq(design_matrix, x, rcond=None)
    cy, *_ = np.linalg.lstsq(design_matrix, y, rcond=None)

    # Peak amplitudes of the fitted fundamental components
    input_amplitude = np.hypot(cx[0], cx[1])
    output_amplitude = np.hypot(cy[0], cy[1])

    if input_amplitude < 1e-12:
        raise ValueError("Input amplitude is too small")

    amplitude_ratio = output_amplitude / input_amplitude

    # Phase of A*sin(w*t) + B*cos(w*t)
    input_phase = np.arctan2(cx[1], cx[0])
    output_phase = np.arctan2(cy[1], cy[0])

    phase = np.degrees(output_phase - input_phase)
    phase = (phase + 180.0) % 360.0 - 180.0

    return amplitude_ratio, phase

def sweep(handle, sampling_rate, start_frequency, end_frequency, number_of_steps):
    
    step_frequency = (end_frequency - start_frequency) // number_of_steps
    
    frequencies = []
    amplitudes= []
    phases = []
    
    for i in range(0, number_of_steps):
        frequencies.append(start_frequency + (float(i) * step_frequency))

    for f in frequencies:
        set_input_frequency(handle, f)
        
        time.sleep(10*(1/f) + 0.5)
        
        (t, _input, _output) = acquire(handle, sampling_rate = sampling_rate, input_frequency = f, number_of_cycles = 4)
        
        amplitude, phase = get_phase_and_amplitude( t, f, _input, _output )
        
        amplitudes.append(amplitude)
        phases.append(phase)
    
    return (frequencies, amplitudes, phases)

def plot_bode_plot(handle, sampling_rate, start_frequency, end_frequency, number_of_steps):

    f, amp, pha = sweep(handle, sampling_rate, start_frequency, end_frequency, number_of_steps)

    plt.figure()
    plt.subplot(2, 1, 1)
    plt.semilogx(f, 20 * np.log10(amp), 'o-')
    plt.ylabel("Amplitude (dB)")
    plt.grid(True)

    plt.subplot(2, 1, 2)
    plt.semilogx(f, pha, 'o-')
    plt.xlabel("Frequency (Hz)")
    plt.ylabel("Phase (degrees)")
    plt.grid(True)

    plt.tight_layout()
    plt.show()
    
    return (f, amp, pha)

def save_frequency_response(f, a, p):
    
    with open("bode_data.csv", "w", newline="") as file:
        writer = csv.writer(file)

        writer.writerow(["Frequency (Hz)", "Amplitude", "Phase (degrees)"])
        
        index = 0
        
        for _f in f:
            writer.writerow([f[index], a[index], p[index]])
            index = index + 1

########################################### Main function. ###########################################################

handle = serial_rpc.serial_rpc(port_name = PORT_NAME, baud_rate = BAUD_RATE, parameter_file = 'dsp_platform.yaml')

#set_input_frequency(handle, 250)
#(timestamp, input_samples, output_samples) = acquire(handle, sampling_rate = 5000, input_frequency = 250, number_of_cycles = 5)
#plot_input_and_output_waveforms(timestamp, input_samples, output_samples)

(f, a, p) = plot_bode_plot(handle, 5000, 4, 500, 20)
save_frequency_response(f,a,p)

handle.close()