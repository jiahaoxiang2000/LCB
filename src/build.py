import os
import shutil
import subprocess
import time
from pathlib import Path

# Constants
EXT = ""  # Mac, Linux
platform_list = ["l475vg","esp32s3"]
experiment_list = ["size", "timing"]
skip_submission = []
skip_variant = []
skip_impl = []
eof_marker = "# LCB exit"

# Folders
base_folder = Path.cwd()
impl_folder = base_folder.parent / "implementations"

# Helper functions
def includes(item, items):
    return item in items

def print_info(message):
    print(f"\033[32m{message}\033[0m")

def print_warning(message):
    print(f"\033[33m{message}\033[0m")

def print_error(message):
    print(f"\033[31m{message}\033[0m")

def stop_watch(message, seconds):
    for counter in range(seconds, 0, -1):
        print(f"{message} ({counter} sec)")
        time.sleep(1)

def wait_eof_marker(outfile):
    count = 0
    while count == 0:
        time.sleep(3)
        with open(outfile, 'r') as f:
            content = f.read()
            count = content.count(eof_marker)
            count += content.count("CUT HERE FOR EXCEPTION DECODER")

def measure_code_size(submission, variant, impl, configs, size_folder, out_folder, overwrite):
    allfailed = 1
    print_info(f"+measure_code_size({submission}, {variant}, {impl})")

    infile = "lwc_crypto.cpp" if (impl_folder / f"src/iut/{impl}/lwc_crypto.cpp").exists() else "lwc_crypto.c"

    for conf in configs:
        outfile = size_folder / f"{submission}-{variant}-{impl}-{conf}-out.txt"
        errfile = size_folder / f"{submission}-{variant}-{impl}-{conf}-err.txt"

        with open("src/lwc_mode.h", 'w') as f:
            f.write("#define LWC_MODE_USE_ENCRYPT")

        if not overwrite and outfile.exists():
            print_warning(f"skipping implementation {impl} with config {conf} [output file exists]")
        else:
            with open(out_folder / "sizes_raw.txt", 'a') as f:
                f.write(f"{submission},{variant},{impl},{conf},mode")

            print_info(f"building implementation {impl} with config {conf}")

            result = subprocess.run([f"platformio{EXT}", "run", "--verbose", "--environment", conf], stdout=open(outfile, 'w'), stderr=open(errfile, 'w'))

            if result.returncode != 0:
                print_error(f"build failed for {submission}, {variant}, {impl}, {conf}, mode")
                with open(outfile, 'w') as f:
                    f.write("build failed")
                with open(out_folder / "sizes_raw.txt", 'a') as f:
                    f.write(",error")

            with open(out_folder / "sizes_raw.txt", 'a') as f:
                f.write("\n")

    print_info("-measure_code_size()")
    return allfailed

def measure_timing(submission, variant, impl, configs, timing_folder, temp_folder, overwrite):
    print_info(f"+measure_timing({submission}, {variant}, {impl})")

    for conf in configs:
        outfile = timing_folder / f"{submission}-{variant}-{impl}-{conf}.txt"
        buildout = temp_folder / f"{submission}-{variant}-{impl}-{conf}-bout.txt"
        builderr = temp_folder / f"{submission}-{variant}-{impl}-{conf}-berr.txt"
        uploadout = temp_folder / f"{submission}-{variant}-{impl}-{conf}-uout.txt"
        uploaderr = temp_folder / f"{submission}-{variant}-{impl}-{conf}-uerr.txt"

        # which mode to control the include code on c language source code. 
        with open("src/lwc_mode.h", 'w') as f:
            f.write("#define LWC_MODE_TIMING")

        if not overwrite and outfile.exists():
            print_warning(f"skipping implementation {impl} with config {conf} [output file exists]")
        else:
            print_info(f"building implementation {impl} with config {conf}")
            # use the platformio to build the source code
            result = subprocess.run([f"platformio{EXT}", "run", "--verbose", "--environment", conf], stdout=open(buildout, 'w'), stderr=open(builderr, 'w'))

            if result.returncode != 0:
                print_error(f"build failed for {submission}, {variant}, {impl}, {conf}")
                with open(outfile, 'w') as f:
                    f.write("build failed")
            else:
                # stop_watch("uploading", 1)
                # use the platformio to upload the compiled binary to the board
                subprocess.run([f"platformio{EXT}", "run", "--verbose", "--target", "upload", "--environment", conf], stdout=open(uploadout, 'w'), stderr=open(uploaderr, 'w'))
                # time.sleep(1)
                # Start minicom in the background
                process = subprocess.Popen(
                    [f"platformio{EXT}", "device", "monitor"],
                    stdout=open(outfile, 'w'),
                    stderr=open(temp_folder / "serial_err.txt", 'w')
                )
                wait_eof_marker(outfile)
                process.terminate()

    print_info("-measure_timing()")
    return 0

def update_variant_count():
    global NumVariants
    NumVariants += 1

def update_implementation_count():
    global NumImplementations
    NumImplementations += 1

def check_source_compatibility(submission, variant, impl):
    impl_arch = ""
    folder = impl_folder / submission / variant / impl

    if (folder / "lcb_arch_avr").exists():
        impl_arch = "avr"
    elif (folder / "lcb_arch_armv6").exists():
        impl_arch = "armv6"
    elif (folder / "lcb_arch_armv6m").exists():
        impl_arch = "armv6m"
    elif (folder / "lcb_arch_armv7m").exists():
        impl_arch = "armv7m"
    elif (folder / "lcb_arch_riscv").exists():
        impl_arch = "riscv"
    elif (folder / "lcb_arch_esp32s3").exists():
        impl_arch = "esp32s3"
    elif (folder / "lcb_arch_sse2").exists():
        impl_arch = "sse2"
    elif (folder / "lcb_arch_avx512").exists():
        impl_arch = "avx512"
    elif (folder / "lcb_arch_armv7a").exists():
        impl_arch = "armv7a"

    if not impl_arch:
        return True
    else:
        return includes(impl_arch, supported_impl)

def parse_args():
    import argparse
    parser = argparse.ArgumentParser(description="Build script")
    parser.add_argument("-t", "--target", required=True, help="Target platform; one of {l475vg}.")
    parser.add_argument("-e", "--experiment", help="Experiments to be performed; a subset of {size, timing}.")
    parser.add_argument("-s", "--submission", help="List of submission names to be processed.")
    parser.add_argument("-v", "--variant", help="List of variant names to be processed.")
    parser.add_argument("-i", "--impl", help="List of implementation names to be processed.")
    parser.add_argument("-w", "--overwrite", action="store_true", help="Do not skip an experiment if a result file already exists.")
    parser.add_argument("-p", "--primary", action="store_true", help="Process only primary variants.")
    return parser.parse_args()


if __name__ == "__main__":

    # Main script
    args = parse_args()

    target = args.target
    experiments = args.experiment.split() if args.experiment else ["size", "timing"]
    submissions = args.submission.split() if args.submission else []
    variants = args.variant.split() if args.variant else []
    implementations = args.impl.split() if args.impl else []
    process_primary = args.primary
    overwrite = args.overwrite

    if not includes(target, platform_list):
        print_error(f"error: platform '{target}' does not exist")
        exit(1)

    configs = [f"{target}-release-{opt}" for opt in ["os", "o1", "o2", "o3"]]

    if not submissions:
        submissions = [sub.name for sub in impl_folder.iterdir() if sub.is_dir()]

    print(f"target            : {target}")
    print(f"configs           : {configs}")
    print(f"experiments       : {experiments}")
    print(f"submissions       : {submissions}")
    print(f"variants          : {variants}")
    print(f"implementations   : {implementations}")
    print(f"process_primary   : {process_primary}")
    print(f"overwrite         : {overwrite}")

    supported_impl = "armv7m" if target == "l475vg" else "esp32s3" if target == "esp32s3" else ""

    out_folder = base_folder / f"outputs/{target}"
    temp_folder = out_folder / "temp"
    size_folder = out_folder / "size"
    timing_folder = out_folder / "timing"

    out_folder.mkdir(parents=True, exist_ok=True)
    temp_folder.mkdir(parents=True, exist_ok=True)
    size_folder.mkdir(parents=True, exist_ok=True)
    timing_folder.mkdir(parents=True, exist_ok=True)

    NumSubmissions = 0
    NumVariants = 0
    NumImplementations = 0

    for submission in submissions:
        if includes(submission, skip_submission):
            print_warning(f"skipping submission {submission}")
            continue

        print_info(f"processing {submission}")
        NumSubmissions += 1

        submission_path = impl_folder / submission
        variants_to_process = variants if variants else [var.name for var in submission_path.iterdir() if var.is_dir()]

        for variant in variants_to_process:
            variant_path = submission_path / variant
            if not variant_path.is_dir():
                continue

            if includes(variant, skip_variant):
                print_warning(f"skipping variant {variant}")
                continue

            is_primary = (variant_path / "primary").exists()
            print_info(f"variant {variant} is primary : {is_primary}")

            if process_primary and not is_primary:
                print_warning(f"skipping non-primary variant {variant}")
                continue

            print_info(f"processing {variant}")
            update_variant_count()

            implementations_to_process = implementations if implementations else [impl.name for impl in variant_path.iterdir() if impl.is_dir()]

            for impl in implementations_to_process:
                impl_path = variant_path / impl
                if not impl_path.is_dir():
                    continue

                if includes(impl, skip_impl):
                    print_warning(f"skipping implementation {impl}")
                    continue

                if not check_source_compatibility(submission, variant, impl):
                    print_warning(f"skipping implementation {impl} [not target compatible]")
                    continue

                print_info(f"processing implementation {impl}")
                update_implementation_count()

                src_iut_path = base_folder / "src/iut"
                shutil.rmtree(src_iut_path, ignore_errors=True)
                shutil.copytree(impl_path, src_iut_path)

                constraints_file = src_iut_path / "lwc_constraints.h"
                if constraints_file.exists():
                    shutil.copy(constraints_file, base_folder / "src")
                else:
                    with open(base_folder / "src/lwc_constraints.h", 'w') as f:
                        f.write("")

                if "size" in experiments:
                    measure_code_size(submission, variant, impl, configs, size_folder, out_folder, overwrite)

                if "timing" in experiments:
                    measure_timing(submission, variant, impl, configs, timing_folder, temp_folder, overwrite)

    print_info(f"processed {NumSubmissions} submissions")
    print_info(f"{NumVariants} variants, {NumImplementations} implementations")