com(){
    ports_USB=$(ls /dev/ttyUSB*)
    ports_ACM=$(ls /dev/ttyACM*)  #arduino
    ports="$ports_USB $ports_ACM"
    select port in $ports;do
        if [[ "$port" ]]; then
            echo "You select the choice '$port"
            port_com=${port##*/}
            minicom "$port_com" $@
            break
        else
            echo "Invalid selection"
        fi
    done
}
