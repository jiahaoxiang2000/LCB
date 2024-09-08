#
# NIST-developed software is provided by NIST as a public service.
# You may use, copy and distribute copies of the software in any medium,
# provided that you keep intact this entire notice. You may improve,
# modify and create derivative works of the software or any portion of
# the software, and you may copy and distribute such modifications or
# works. Modified works should carry a notice stating that you changed
# the software and should note the date and nature of any such change.
# Please explicitly acknowledge the National Institute of Standards and
# Technology as the source of the software.
#
# NIST-developed software is expressly provided "AS IS." NIST MAKES NO
# WARRANTY OF ANY KIND, EXPRESS, IMPLIED, IN FACT OR ARISING BY OPERATION
# OF LAW, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT AND DATA ACCURACY. NIST
# NEITHER REPRESENTS NOR WARRANTS THAT THE OPERATION OF THE SOFTWARE WILL BE
# UNINTERRUPTED OR ERROR-FREE, OR THAT ANY DEFECTS WILL BE CORRECTED. NIST
# DOES NOT WARRANT OR MAKE ANY REPRESENTATIONS REGARDING THE USE OF THE SOFTWARE
# OR THE RESULTS THEREOF, INCLUDING BUT NOT LIMITED TO THE CORRECTNESS, ACCURACY,
# RELIABILITY, OR USEFULNESS OF THE SOFTWARE.
#
# You are solely responsible for determining the appropriateness of using and
# distributing the software and you assume all risks associated with its use,
# including but not limited to the risks and costs of program errors, compliance
# with applicable laws, damage to or loss of data, programs or equipment, and
# the unavailability or interruption of operation. This software is not intended
# to be used in any situation where a failure could cause risk of injury or
# damage to property. The software developed by NIST employees is not subject to
# copyright protection within the United States.
#

#!/bin/bash

# Select appropriate extension for PlatformIO executable
# EXT=".exe" # Windows
EXT="" # Mac, Linux

# Target platform names
platform_list=("l475vg")

# List of experiments to be performed
experiment_list=("size timing")

# The list of submission names to be omitted from processing
skip_submission=("")

# The list of variant names to be omitted from processing
skip_variant=("")

# The list of implementation names to be omitted from processing
skip_impl=("")

#
# Folders
#
base_folder="$(pwd)"
impl_folder="$base_folder/../implementations"

# Functionalities for code size measurements
# Size measurements are performed for encryption only, decryption only and both for AEAD implementations.
# We have to provide a dummy entry for hash functionality even though hash implementations do not have different functionalities.
# Note: The mode names when prepended by "LWC_MODE_USE_" must match a predefined mode of operation
# aead_modes=("AEAD_ENCRYPT AEAD_DECRYPT AEAD_BOTH")
# hash_modes=("HASH")
# combined_modes=("COMBINED_AEAD_ENCRYPT COMBINED_AEAD_DECRYPT COMBINED_AEAD_BOTH")

# String searched in serial output indicating the end of program
eof_marker="# LCB exit"

# Checks whether the first argument is in the list of items provided by the second argument
function includes() {

	for name in $2; do
		if [[ $name == $1 ]]; then
			return 1
		fi
	done

	return 0
}

# Outputs the argument to console in green color
function print_info() {
	echo -e "\x1b[32m$@" "\x1b[0m"
}

# Outputs the argument to console in yellow color
function print_warning() {
	echo -e "\x1b[33m$@" "\x1b[0m"
}

# Outputs the argument to console in red color
function print_error() {
	echo -e "\x1b[31m$@" "\x1b[0m"
}

# Stop watch timer
# arg1 : Message to display
# arg2 : Number of seconds
function stop_watch() {

	local counter

	counter=$2

	until [ $counter -eq 0 ]; do

		echo "$1 ($counter sec)"
		sleep 1
		counter=$(($counter - 1))
	done
}

function wait_eof_marker() {

	# Check output file periodically until the end of file marker is found or an exception is generated
	count=0
	while [ $count -eq 0 ]; do
		sleep 10s
		count=$(grep -c "$eof_marker" $outfile)

		# nodemcuv2 can generate exceptions
		except=$(grep -c "CUT HERE FOR EXCEPTION DECODER" $outfile)

		count=$(($count + $except))
	done
}


#
# Code size experiment
#
function measure_code_size() {

	allfailed=1

	print_info "+measure_code_size($submission, $variant, $impl)"

	if [ -f "src/iut/$impl/lwc_crypto.cpp" ]; then
		infile="lwc_crypto.cpp"
	else
		infile="lwc_crypto.c"
	fi

	for conf in ${configs[@]}; do

		outfile=$size_folder/$submission-$variant-$impl-$conf-out.txt
		errfile=$size_folder/$submission-$variant-$impl-$conf-err.txt

		echo "#define LWC_MODE_USE_AEAD_ENCRYPT" >src/lwc_mode.h

		# Skip if an output file exists
		if [[ $overwrite = false ]] && [[ -f $outfile ]]; then
			print_warning "skipping implementation $impl with config $conf [output file exists]"
		else

			printf "$submission,$variant,$impl,$conf,$mode" >>$out_folder/sizes_raw.txt

			print_info "building implementation $impl with config $conf"

			platformio$EXT run --verbose --environment $conf >$outfile 2>$errfile

			if [[ "$?" -ne 0 ]]; then
				print_error "build failed for $submission, $variant, $impl, $conf, $mode"
				echo "build failed" >$outfile
				printf ",error" >>$out_folder/sizes_raw.txt
			fi

			printf "\n" >>$out_folder/sizes_raw.txt

		fi

	done # conf

	print_info "-measure_code_size()"

	return $allfailed
}

#
# Timing experiment
#
function measure_timing() {

	print_info "+measure_timing($submission, $variant, $impl)"

	for conf in ${configs[@]}; do

		outfile=$timing_folder/$submission-$variant-$impl-$conf.txt
		buildout=$temp_folder/$submission-$variant-$impl-$conf-bout.txt
		builderr=$temp_folder/$submission-$variant-$impl-$conf-berr.txt
		uploadout=$temp_folder/$submission-$variant-$impl-$conf-uout.txt
		uploaderr=$temp_folder/$submission-$variant-$impl-$conf-uerr.txt

		echo "#define LWC_MODE_TIMING" >src/lwc_mode.h

		# if an output file already exists, do not overwrite it
		if [[ $overwrite = false ]] && [[ -f $outfile ]]; then
			print_warning "skipping implementation $impl with config $conf [output file exists]"
		else
			print_info "building implementation $impl with config $conf"

			platformio$EXT run --verbose --environment $conf >$buildout 2>$builderr

			if [[ "$?" -ne 0 ]]; then
				print_error "build failed for $submission, $variant, $impl, $conf"
				echo "build failed" >$outfile
			fi
			if [[ "$EXT" == ".exe" ]]; then
				platformio$EXT run --verbose --environment $conf >$buildout 2>$builderr

				if [[ "$?" -ne 0 ]]; then
					print_error "build failed for $submission, $variant, $impl, $conf"
					echo "build failed" >$outfile
				else
					stop_watch "uploading" 3
					platformio$EXT run --verbose --target upload --environment $conf >$temp_folder/upload_out.txt 2>$temp_folder/upload_err.txt
					sleep 3s
					platformio$EXT device monitor >$outfile &
					2>$temp_folder/serial_err.txt
					PID=$!

					wait_eof_marker

					kill -9 $PID
				fi
			else
				platformio$EXT run --verbose --environment $conf >$buildout 2>$builderr

				if [[ "$?" -ne 0 ]]; then
					print_error "build failed for $submission, $variant, $impl, $conf"
					echo "build failed" >$outfile
				else
					stop_watch "uploading" 3
					platformio$EXT run --verbose --target upload --environment $conf >$temp_folder/upload_out.txt 2>$temp_folder/upload_err.txt
					sleep 3s
					# issue the monitor command in the background on linux not working
					# platformio$EXT device monitor --filter log2file &2>$temp_folder/serial_err.txt
					minicom -D /dev/ttyACM0 -b 9600 -C $outfile 2>$temp_folder/serial_err.txt &
					monitor_pid=$!
					echo "monitor_pid: $monitor_pid"
					
					echo $(pwd)
					logfile=$(ls -t logs/platformio-device-monitor* | head -1)
					echo $logfile
					mv $logfile $outfile
				fi
			fi
		fi

	done # conf

	print_info "-measure_timing()"

	return 0
}

function update_variant_count {

	NumVariants=$(($NumVariants + 1))

}

function update_implementation_count {

	NumImplementations=$(($NumImplementations + 1))
}

# Returns non-zero if the implementation is compatible with the current target, zero otw.
function check_source_compatibility() {

	impl_arch=""

	local folder

	folder=$impl_folder/$submission/$variant/$impl

	if [[ -r "$folder/lcb_arch_avr" ]]; then
		impl_arch="avr"
	elif [[ -r "$folder/lcb_arch_armv6" ]]; then
		impl_arch='armv6'
	elif [[ -r "$folder/lcb_arch_armv6m" ]]; then
		impl_arch='armv6m'
	elif [[ -r "$folder/lcb_arch_armv7m" ]]; then
		impl_arch='armv7m'
	elif [[ -r "$folder/lcb_arch_riscv" ]]; then
		impl_arch='riscv'
	elif [[ -r "$folder/lcb_arch_esp8266" ]]; then
		impl_arch='esp8266'
	elif [[ -r "$folder/lcb_arch_sse2" ]]; then
		impl_arch='sse2'
	elif [[ -r "$folder/lcb_arch_avx512" ]]; then
		impl_arch='avx512'
	elif [[ -r "$folder/lcb_arch_armv7a" ]]; then
		impl_arch='armv7a'
	fi

	if [[ -z "$impl_arch" ]]; then
		# Implementation does not have a target constraint
		return 1
	else
		# Check if the implementation arch. is among the supported arch.
		includes $impl_arch "${supported_impl[@]}"
		return $?
	fi
}

# These variables keep track the number of items processed. The values are reported at the end of execution.
NumSubmissions=0
NumVariants=0
NumImplementations=0

# Global variables that control the processing of the implementations
target=""
experiments=""
submissions=""
variants=""
implementations=""
process_primary=false
overwrite=false

function print_usage() {

	echo "Usage: build.sh [-t | --target] target [options]
		Options:
		-t | --target <arg>     Target platform; one of {l475vg}.
		-e | --experiment <arg> Experiments to be performed; a subset of {size, timing}.
		-s | --submission <arg> List of submission names to be processed.
		-v | --variant <arg>    List of variant names to be processed.
		-i | --impl <arg>       List of implementation names to be processed.
		-w | --overwrite        Do not skip an experiment if a result file already exists.
		-p | --primary          Process only primary variants.
	Note: If there are more than one arguments for -e, -s, -v, and -i, they must be provided 
	      in double quotes and separated by space.
	      Ex: -e \"kat size\"
	"
}

function parse_args() {

	while [[ -n "$1" ]]; do

		case $1 in
		-t | --target)
			shift
			target="$1"
			;;
		-e | --experiment)
			shift
			experiments="$1"
			;;
		-s | --submission)
			shift
			submissions="$submissions$1 "
			;;
		-v | --variant)
			shift
			variants="$1"
			;;
		-i | --impl)
			shift
			implementations="$1"
			;;
		-w | --overwrite)
			overwrite=true
			;;
		-p | --primary)
			process_primary=true
			;;
		*)
			echo "error: unknown argument '$1'"
			return 1
			;;
		esac
		shift
	done

	return 0
}

# Parse command line arguments and set the variables
parse_args "$@"
if [[ $? -ne 0 ]]; then
	exit 1
fi

# Target platform name must be specified
if [[ -z $target ]]; then
	print_error "error: no target platform specified"
	print_usage
	exit 1
fi

# Target platform name must be one of the predefined platform names
includes $target "${platform_list[@]}"
if [[ $? -eq 0 ]]; then
	print_error "error: platform '$target' does not exist"
	print_usage
	exit 1
fi

# Construct the configuration names. These configurations must exist in platformio.ini file.
configs=$(echo $target-release-{os,o1,o2,o3})
#configs=$(echo $target-release-o2)

# Set and validate the experiment types
if [[ -z "$experiments" ]]; then
	# Perform all experiments if none specified
	experiments="size timing"
else
	# Validate the experiment names provided by the user
	for exp in ${experiments[@]}; do
		includes $exp "${experiment_list[@]}"
		if [[ $? -eq 0 ]]; then
			print_error "error: invalid experiment type '$exp'"
			exit 1
		fi
	done
fi

# Set the list of submissions to be processed
# If no submission name is provided then all submission folders will be processed
if [[ -z "$submissions" ]]; then
	cd $impl_folder
	for sub in $(ls -d */ | sed 's|[/]||g'); do
		submissions="$submissions$sub "
	done
fi

# Print the variables
echo "target            : $target"
echo "configs           : $configs"
echo "experiments       : $experiments"
echo "submissions       : $submissions"
echo "variants          : $variants"
echo "implementations   : $implementations"
echo "process_primary   : $process_primary"
echo "overwrite         : $overwrite"

# Supported ASM implementations by the target
supported_impl=""
if [[ "$target" == "l475vg" ]]; then
	supported_impl="armv7m"
fi

# Output folders
out_folder="outputs/$target"
temp_folder="$out_folder/temp"
size_folder="$out_folder/size"
timing_folder="$out_folder/timing"

# Create output folders
cd "$base_folder"
mkdir -p "$out_folder"
mkdir -p "$temp_folder"
mkdir -p "$size_folder"
mkdir -p "$timing_folder"

for submission in $submissions; do

	# Check skip list
	includes $submission "${skip_submission[@]}"
	skip=$?
	if [[ $skip == 1 ]]; then
		print_warning "skipping submission $submission"
		continue
	fi

	print_info "processing $submission"
	NumSubmissions=$(($NumSubmissions + 1))

	cd $impl_folder/$submission

	variants_to_process=""

	# If no variant name is provided, process all
	if [[ -z "$variants" ]]; then
		for var in $(ls -d */ | sed 's|[/]||g'); do
			variants_to_process="$variants_to_process$var "
		done
	else
		variants_to_process=$variants
	fi

	for variant in ${variants_to_process[@]}; do

		cd $impl_folder/$submission

		if [ ! -d $variant ]; then
			continue
		fi

		# Check skip list
		includes $variant "${skip_variant[@]}"
		skip=$?
		if [[ $skip == 1 ]]; then
			print_warning "skipping variant $variant"
			continue
		fi

		if [ -f "$variant/primary" ]; then
			is_primary=true
		else
			is_primary=false
		fi

		print_info "variant $variant is primary : $is_primary"

		if [[ $process_primary = true ]] && [[ $is_primary = false ]]; then
			print_warning "skipping non-primary variant $variant"
			continue
		fi

		print_info "processing $prim variant $variant"

		update_variant_count

		cd $impl_folder/$submission/$variant

		implementations_to_process=""

		# If no implementation name is provided, process all
		if [[ -z "$implementations" ]]; then
			for impl in $(ls -d */ | sed 's|[/]||g'); do
				implementations_to_process="$implementations_to_process$impl "
			done
		else
			implementations_to_process=$implementations
		fi

		for impl in ${implementations_to_process[@]}; do

			cd $impl_folder/$submission/$variant

			if [ ! -d $impl ]; then
				continue
			fi

			# Check skip list
			includes $impl "${skip_impl[@]}"
			skip=$?
			if [[ $skip == 1 ]]; then
				print_warning "skipping implementation $impl"
				continue
			fi

			# Check target compatibility
			check_source_compatibility
			if [[ $? -eq 0 ]]; then
				print_warning "skipping implementation $impl [not target compatible]"
				continue
			fi

			print_info "processing implementation $impl"

			update_implementation_count

			# Copy implementation folder to src/iut
			rm -rf $base_folder/src/iut/*
			cp -r $impl_folder/$submission/$prim_dir/$variant/$impl $base_folder/src/iut

			# If the implementation has lwc_constraints.h file, copy it to the src folder.
			# Otherwise create an empty lwc_constraints.h file so that the compiler doesn't complain about not finding it.
			constraints_file=$base_folder/src/iut/$impl/lwc_constraints.h
			if [ -f $constraints_file ]; then
				cp $constraints_file $base_folder/src
			else
				echo "" >$base_folder/src/lwc_constraints.h
			fi

			cd $base_folder

			#
			# Code Size
			#
			includes "size" "${experiments[@]}"
			process=$?

			if [[ $process == 0 ]]; then
				print_warning "skipping code size measurements"
			else
				measure_code_size
				retval=$?
				print_info "measure_code_size() returned $retval"
			fi

			#
			# Timings
			#
			includes "timing" "${experiments[@]}"
			process=$?

			if [[ $process == 0 ]]; then
				print_warning "skipping timing measurements"
			else
				measure_timing
				retval=$?
				print_info "measure_timing() returned $retval"
			fi

		done # impl

	done # variant

done # submission

cd $base_folder

print_info "processed $NumSubmissions submissions"
print_info "$NumVariants  variants, $NumImplementations  implementations"
