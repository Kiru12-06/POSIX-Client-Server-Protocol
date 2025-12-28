import os
import time
import subprocess
import shutil

READY_FILE = "/home/raspberry/CIS27_Final/pipeline/ready.txt"
L2_INPUT_DIR = "/home/raspberry/CIS27_Final/layer2/input/"
L2_EXEC_PATH = "/home/raspberry/CIS27_Final/layer2/exc_main2"
L3_EXC_PATH = "/home/raspberry/CIS27_Final/layer3/main3EXC"

L3_DIR = "/home/raspberry/CIS27_Final/layer3/"
L3_INPUT_DIR = "/home/raspberry/CIS27_Final/layer3/input"
TARG3_IP_FILE ="/home/raspberry/CIS27_Final/layer3/input/IP_destination.txt"


last_mod_time = 0

def find_open_ip():
    print("-- Searching for open IP's --")
    try:
        results = subprocess.run(
            "ip neighbor show | grep -i 'reachable\|stale\|permanent' | awk '{print $1}'", shell = True, capture_output = True, text = True,
            check = True,
        )

        active_ips = []
        raw_output_list = results.stdout.split('\n')

        for ip in raw_output_list:
            striped_ip = ip.strip()

            if striped_ip and striped_ip != "127.0.0.1":
                active_ips.append(striped_ip)

        return active_ips
        
    except subprocess.CalledProcessError as e:
        print(f"Error during network scan: {e}")
        return []

while True:
    try:
        current_mod_time =  os.path.getmtime(READY_FILE) ## Gets the last time the ready.txt was modified
        
        if current_mod_time != last_mod_time:
            last_mod_time = current_mod_time   
            
            with open(READY_FILE, "r") as f:
                message_path = f.read().strip()   ## ready.txt saves a filepath to the message txt generated in layer 1 main
                                                  ## and now message_path has that same filepath
        
            if os.path.exists(message_path):  ## with this line the pipline is checking if that file exists in the message path
                shutil.copy(message_path, L2_INPUT_DIR) ## and now in this line it actually copies that file into the input folder on layer 2
                print(f"Message copied to {L2_INPUT_DIR}")
                    
                    
                exc_L2 = subprocess.run(L2_EXEC_PATH, capture_output = True, text = True) 
                ## the line above has python send the message to the shell to tell the kernel to run the exc file in layer 2
                ## the diffrent True values are to read the diffrent ouputs from the kernel and convert that text output into readable
                ## python strings

                if exc_L2.returncode == 0: ## in my layer 2 main 0 will be returned if the main runs correctly
                    print(" Layer 2 executed successfully. Scaning diffrent IP's on the network.") 

                    
                    ip_choose_list = find_open_ip() ## creats holding list that has all the values found with the help of the find_open_ip
                                                    ## method above
                    if not ip_choose_list:
                        print("No active IP addresses found on the network")
                    else:
                        print("\n--- Available IP's ---\n")
                        for i, ip in enumerate(ip_choose_list):  ## this is a special for loop that couples the diffrent ip's in the
                            print(f"[{i}] {ip}")               ## ip_choose_list with a number that will help us when we want to choose the
                                                                 ## ip we want to send to   
                        print ("----------------------")
                    
                        selected_ip = None
                        while selected_ip is None:
                            print ("manually input a IP address by entering -10 and to select that IP typein the index after the final (Ex. if final index in list is 10 your manual input will be selected after typingin 11)")
                            user_input = input("enter the number of the IP address you want to choose: ").strip()
                            
                            try:

                                index = int(user_input)
                                if index == -10:
                                    manual_input = input("enter the IP addres from your window").strip()
                                    ip_choose_list.append(manual_input)
                                    continue 
                                if 0 <= index < len(ip_choose_list):
                                    selected_ip = ip_choose_list[index]
                                else:
                                    print(f"Invalid number. Please enter a number between 1 and {len(ip_choose_list) - 1}.")
        
                            except ValueError:
                                print("Invalid input. Please enter a number.")

                        os.makedirs(L3_INPUT_DIR, exist_ok=True) ## this just checks to make sure that the input folder in layer 3 dir
                                                                 ## path exists just to be safe
                        with open (TARG3_IP_FILE, "w") as f:
                            f.write(selected_ip)
                            
                        exc_L3 = subprocess.run(L3_EXC_PATH, stdout = None, stderr = subprocess.PIPE, text = True, cwd = L3_DIR, shell = True)
                        
                        if exc_L3.returncode == 0:
                            print("Layer 3 transmission successful.")
                        else: 
                            print(f"Layer 3 failed (Code: {exc_L3.returncode}). Error:\n{exc_L3.stderr}")
                    
             
            else:
                print("Message file not found!")

            
                

        

        
        time.sleep(2)
    
    except Exception as e:
        print(f"Pipline stopped due to error: {e}")
        break
