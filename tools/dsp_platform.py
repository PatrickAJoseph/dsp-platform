
import serial_rpc
import time
import pytest
from matplotlib import pyplot as plt

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

    print(number_of_samples_to_get)

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
       
    time.sleep( ((float(sample_buffer_size)/float(sampling_rate)) * 2.0) + 1.0 )

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
    
    print(end_time - start_time)

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

def get_phase_and_amplitude(timestamp, input_samples, output_samples):


########################################### Main function. ###########################################################

handle = serial_rpc.serial_rpc(port_name = PORT_NAME, baud_rate = BAUD_RATE, parameter_file = 'dsp_platform.yaml')

set_input_frequency(handle, 250)

(timestamp, input_samples, output_samples) = acquire(handle, sampling_rate = 10000, input_frequency = 250, number_of_cycles = 5)

plot_input_and_output_waveforms(timestamp, input_samples, output_samples)

handle.close()