# build requirements

# change chrony synchronization configuration
sudo nano /etc/chrony/chrony.conf

# by default the servers used for synchronization are 
# pool ntp.ubuntu.com...
# pool 0.ubuntu.pool.ntp.org...
# pool 1.ubuntu.pool.ntp.org...
# pool 2.ubuntu.pool.ntp.org..

# If you want to configure for co-referenced synchronization you have to comment this pool lines and 
# add on P 2
server "IP address of other reference Pi 1" iburst

# On Pi 1 that here serving as reference you need to add
allow "IP address of other reference Pi 2"

# Build and load dts overlay
dtc -@ -I dts -O dtb -o gpio16_irq.dtbo gpio16_irq.dts
sudo cp gpio16_irq.dtbo /boot/firmware/overlays

#to remove overlay
sudo rm /boot/firmware/overlays/gpio16_irq.dtbo

# Add overlay to boot config
sudo nano /boot/firmware/config.txt
Add dtoverlay=gpio16_irq to file
# then reboot
sudo reboot


# Build and load module
# In module folder
make clean && make
sudo insmod gpio16_irq.ko

# Build and run c++ programs at max priority on core 3
# In program folder
For Pi client
g++ -o sync_test_client sync_test_client.cpp -lgpiod
sudo taskset -c 3 chrt -f 99 ./sync_test_client

For Pi server
g++ -o sync_test_server sync_test_server.cpp
sudo taskset -c 3 chrt -f 99 ./sync_test_server
