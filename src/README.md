## Requirements

 - [Python](https://www.python.org)
 - [PlatformIO](https://platformio.org)
 - PlatformIO *Board* and *Platform* files for the target devices.
 - [Visual Studio Code](https://code.visualstudio.com/) This is optional but quite handy if you want to work with an IDE. After installing the PlatformIO extension, you can also install the board and platform files from the IDE for the microcontrollers, which will be required to build the applications.
  - For batch processing with the build script, the implementations must be in a folder named `implementations` that is two levels above this folder. If you clone the repository, the implementations will be in the correct location.

The benchmarking framework was developed and tested on Windows 10 for the NIST. It does not use a Windows specific feature, though. Now we modified the framework to work on Linux as well. 

> note: when transferring the framework to a Linux machine, the serial console output may not work as expected. So we reflector the script to python.

## Experiments and Operating Modes

The framework is designed to process implementations and experiments one at a time. The implementation that is to be benchmarked must be copied under the `src\iut` folder. There must be exactly one implementation in this folder, otherwise the build will fail.

The experiment that is going to be carried out is defined in the `lwc_mode.h` file which must be located in the `src` folder. The file must contain exactly one of the following preprocessor definitions:

| Symbol | Experiment Type | |
| :------ | :------: | :----- |
| LWC_MODE_USE_ENCRYPT | Code Size | Only encryption function is used.|
| LWC_MODE_USE_DECRYPT* | Code Size | Only decryption function is used.|
| LWC_MODE_USE_BOTH* | Code Size | encryption and decryption functions are both used.|
| LWC_MODE_TIMING_ENCRYPT* | Timing | Only measure encryption time.|
| LWC_MODE_TIMING | Timing | Performs timing measurements for the implementation.|

> `*` is not supported in the current version of the framework. 

The build script that is explained in the next section creates a new `lwc_mode.h` file for each experiment before the build. Build will fail if there is no `lwc_mode.h` in the `src` folder or it does not contain one of the above definitions.

## Building with the script `build.py`

The python script `build.py` processes the implementations by building them within the framework and depending on the experiment being done it uploads the program to the target device and captures the program output. At a minimum, the script must be provided a target platform name, which can be one of the platforms defined in the `platformio.ini` file. Currently, the valid platform names are `{l475vg}`. 

By default, the script processes all implementations and does all the experiments. However, the behaviour can be changed by providing command line arguments, for instance to process only one or more *submissions*, or *variants*, as well as performing select experiments. Running the script with `-h` gives an explanation of the set of available command line arguments. Some examples:


``` bash
python build.py --target l475vg # Processes all implementations for all experiments
```

``` bash
python build.py --target l475vg --experiment "size timing" # Perform only Code size and Timing experiments on all implementations
```

``` bash
python build.py --target l475vg --submission "warp" # Process all implementations of the submissions warp
```

``` bash
python build.py --target l475vg --primary  # Process only implementations of primary variants
```

``` bash
python build.py --target l475vg --impl arm --experiment size # Perform size experiment on all implementations of the arm implementation
```


**Note:** For code size experiments, the device is not required since the code sizes are extracted from the build output. However, for *Timing* experiments, the program must run on the device and the output needs to be captured. 

## Collect Results with `collect.py`

The build script will save the results in the `outputs` folder. The results for each target platform are stored under their respective folders. For aggregation and analysis, the `collect.py` script can be used. The script will read the results from the `outputs` folder split by the target platform and will generate a CSV file. All types of experiments are collected in the same file. The script can be run as follows:

> warning: now the collect script need the result simultaneously have the code size and timing result. future work will be done to separate the code size and timing result. 

``` bash
python collect.py
```


## Tips

### Debug for a Single Implementation

We need to check one implementation at a time. To do this, we can use the `--submission` and `--impl` arguments together. For example, to check the `warp` submission and the `arm` implementation, we can use the following command:

``` bash
python build.py --target l475vg --submission warp --variant warp64 --impl arm --experiment timing  --overwrite && python collect.py
``` 

